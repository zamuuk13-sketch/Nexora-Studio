#include "NexoraBridgeServer.h"

#include <QDateTime>
#include <QHostAddress>
#include <QJsonDocument>
#include <QRandomGenerator>
#include <QUuid>

NexoraBridgeServer::NexoraBridgeServer(QObject *parent) : QObject(parent) {
    connect(&m_server, &QTcpServer::newConnection, this, &NexoraBridgeServer::onNewConnection);
    connect(&m_heartbeatTimer, &QTimer::timeout, this, &NexoraBridgeServer::onHeartbeatTimeout);
    m_heartbeatTimer.setInterval(2000);
}

bool NexoraBridgeServer::start(quint16 port) {
    if (m_server.isListening()) return true;
    m_port = port;

    // Loopback only: the bridge is never exposed to the LAN/Internet.
    if (!m_server.listen(QHostAddress::LocalHost, m_port)) {
        setState(QStringLiteral("error"));
        return false;
    }

    // A per-run token is available for clients that opt into Authorization.
    // The plugin may run in loopback-only compatibility mode until pairing UI is added.
    m_token = QUuid::createUuid().toString(QUuid::WithoutBraces);
    m_heartbeatTimer.start();
    setState(QStringLiteral("listening"));
    return true;
}

void NexoraBridgeServer::stop() {
    m_heartbeatTimer.stop();
    for (auto *socket : m_clients.keys()) socket->disconnectFromHost();
    m_clients.clear();
    m_server.close();
    m_studioSeen = false;
    setState(QStringLiteral("stopped"));
}

bool NexoraBridgeServer::isRunning() const { return m_server.isListening(); }
QString NexoraBridgeServer::baseUrl() const { return QStringLiteral("http://127.0.0.1:%1").arg(m_port); }
QString NexoraBridgeServer::status() const { return m_state; }

QString NexoraBridgeServer::enqueueCommand(const QString &command, const QJsonObject &args) {
    const QString id = nextRequestId();
    QJsonObject request;
    request.insert(QStringLiteral("requestId"), id);
    request.insert(QStringLiteral("command"), command);
    request.insert(QStringLiteral("args"), args);
    m_commands.enqueue(request);
    return id;
}

void NexoraBridgeServer::onNewConnection() {
    while (m_server.hasPendingConnections()) {
        auto *socket = m_server.nextPendingConnection();
        m_clients.insert(socket, Client{socket, {}});
        connect(socket, &QTcpSocket::readyRead, this, &NexoraBridgeServer::onSocketReadyRead);
        connect(socket, &QTcpSocket::disconnected, this, &NexoraBridgeServer::onSocketDisconnected);
    }
}

void NexoraBridgeServer::onSocketReadyRead() {
    auto *socket = qobject_cast<QTcpSocket *>(sender());
    if (!socket || !m_clients.contains(socket)) return;

    auto &client = m_clients[socket];
    client.buffer.append(socket->readAll());

    // One request per connection is enough for the plugin's short polling model.
    const int headerEnd = client.buffer.indexOf("\r\n\r\n");
    if (headerEnd < 0) return;

    const QByteArray headerPart = client.buffer.left(headerEnd);
    const QList<QByteArray> lines = headerPart.split('\n');
    if (lines.isEmpty()) return;

    int contentLength = 0;
    for (int i = 1; i < lines.size(); ++i) {
        const QByteArray line = lines.at(i).trimmed();
        const int colon = line.indexOf(':');
        if (colon <= 0) continue;
        if (line.left(colon).trimmed().compare("Content-Length", Qt::CaseInsensitive) == 0)
            contentLength = line.mid(colon + 1).trimmed().toInt();
    }

    const int bodyStart = headerEnd + 4;
    if (client.buffer.size() < bodyStart + contentLength) return;

    const QByteArray request = client.buffer.left(bodyStart + contentLength);
    client.buffer.remove(0, bodyStart + contentLength);
    handleRequest(socket, request);
}

void NexoraBridgeServer::handleRequest(QTcpSocket *socket, const QByteArray &request) {
    const int lineEnd = request.indexOf("\r\n");
    if (lineEnd < 0) { sendJson(socket, 400, {{"error", "bad request"}}); return; }

    const QByteArray requestLine = request.left(lineEnd);
    const QList<QByteArray> parts = requestLine.split(' ');
    if (parts.size() < 2) { sendJson(socket, 400, {{"error", "bad request"}}); return; }

    const QByteArray method = parts.at(0);
    const QByteArray path = parts.at(1);
    const int headerEnd = request.indexOf("\r\n\r\n");
    const QByteArray body = headerEnd >= 0 ? request.mid(headerEnd + 4) : QByteArray();

    QMap<QByteArray, QByteArray> headers;
    const QByteArray headerBlock = request.left(headerEnd < 0 ? request.size() : headerEnd);
    for (const QByteArray &line : headerBlock.split('\n')) {
        const int colon = line.indexOf(':');
        if (colon > 0) headers.insert(line.left(colon).trimmed().toLower(), line.mid(colon + 1).trimmed());
    }

    // All endpoints are loopback-only by construction. Authorization is accepted when supplied.
    if (!authorized(socket, headers)) {
        sendJson(socket, 401, {{"error", "unauthorized"}});
        return;
    }

    if (method == "GET" && path == "/health") {
        sendJson(socket, 200, {
            {"protocol", "NEXORA_BRIDGE_V1"},
            {"version", "0.2.0"},
            {"status", "ok"},
            {"studioSeen", m_studioSeen}
        });
        return;
    }

    if (method == "GET" && path == "/next") {
        m_studioSeen = true;
        if (!m_commands.isEmpty()) sendJson(socket, 200, m_commands.dequeue());
        else sendJson(socket, 200, {{"command", QJsonValue::Null}});
        return;
    }

    if (method == "POST" && path == "/hello") {
        m_studioSeen = true;
        setState(QStringLiteral("connected"));
        emit studioConnected();
        sendJson(socket, 200, {{"ok", true}});
        return;
    }

    if (method == "POST" && path == "/result") {
        QJsonParseError parseError{};
        const QJsonDocument document = QJsonDocument::fromJson(body, &parseError);
        if (parseError.error != QJsonParseError::NoError || !document.isObject()) {
            sendJson(socket, 400, {{"error", "invalid JSON"}});
            return;
        }
        emit commandResult(document.object());
        sendJson(socket, 200, {{"ok", true}});
        return;
    }

    sendJson(socket, 404, {{"error", "not found"}});
}

void NexoraBridgeServer::sendJson(QTcpSocket *socket, int statusCode, const QJsonObject &body) {
    const QByteArray payload = QJsonDocument(body).toJson(QJsonDocument::Compact);
    const QByteArray statusText = statusCode == 200 ? "OK" : (statusCode == 401 ? "Unauthorized" : (statusCode == 404 ? "Not Found" : "Bad Request"));
    QByteArray response;
    response += "HTTP/1.1 " + QByteArray::number(statusCode) + " " + statusText + "\r\n";
    response += "Content-Type: application/json\r\n";
    response += "Content-Length: " + QByteArray::number(payload.size()) + "\r\n";
    response += "Connection: close\r\n\r\n";
    response += payload;
    socket->write(response);
    socket->disconnectFromHost();
}

QJsonObject NexoraBridgeServer::dequeueCommand() {
    return m_commands.isEmpty() ? QJsonObject{{"command", QJsonValue::Null}} : m_commands.dequeue();
}

QString NexoraBridgeServer::nextRequestId() {
    return QUuid::createUuid().toString(QUuid::WithoutBraces);
}

void NexoraBridgeServer::setState(const QString &state) {
    if (m_state == state) return;
    m_state = state;
    emit stateChanged(m_state);
}

bool NexoraBridgeServer::authorized(QTcpSocket *socket, const QMap<QByteArray, QByteArray> &headers) const {
    if (!socket || socket->peerAddress() != QHostAddress::LocalHost) return false;
    const QByteArray auth = headers.value("authorization");
    if (auth.isEmpty()) return true; // loopback compatibility mode; pairing can enforce m_token later.
    const QByteArray expected = QByteArray("Bearer ") + m_token.toUtf8();
    return auth == expected;
}

void NexoraBridgeServer::onSocketDisconnected() {
    auto *socket = qobject_cast<QTcpSocket *>(sender());
    if (!socket) return;
    m_clients.remove(socket);
    socket->deleteLater();
    if (m_studioSeen && m_clients.isEmpty()) {
        m_studioSeen = false;
        setState(QStringLiteral("reconnecting"));
        emit studioDisconnected();
    }
}

void NexoraBridgeServer::onHeartbeatTimeout() {
    if (!m_studioSeen) return;
    // The plugin's /next polling creates a fresh connection each cycle.
    // A separate timeout-based offline detector can be layered here once
    // the desktop UI has a real last-seen timestamp.
}
