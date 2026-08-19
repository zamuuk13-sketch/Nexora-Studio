#include "bridgeclient.h"
#include <QTimer>
#include <QNetworkRequest>
#include <QJsonDocument>
#include <QUuid>

BridgeClient::BridgeClient(QObject *parent) : QObject(parent), m_timer(new QTimer(this)) {
    m_timer->setInterval(500);
    connect(m_timer, &QTimer::timeout, this, &BridgeClient::poll);
    connect(&m_network, &QNetworkAccessManager::finished, this, &BridgeClient::onFinished);
}

void BridgeClient::start() { if (!m_timer->isActive()) m_timer->start(); poll(); }
void BridgeClient::stop() { m_timer->stop(); if (m_connected) { m_connected = false; emit connectionChanged(false); } }

QString BridgeClient::enqueue(const QString &command, const QJsonObject &args) {
    Q_UNUSED(command); Q_UNUSED(args);
    // Commands are queued by the embedded bridge server. The desktop UI calls
    // the server-side enqueue API in the integrated build; this standalone
    // client remains read-only until paired with that server instance.
    return QUuid::createUuid().toString(QUuid::WithoutBraces);
}

void BridgeClient::poll() {
    QNetworkRequest req(QUrl(m_baseUrl + "/health"));
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    m_network.get(req);
}

void BridgeClient::onFinished(QNetworkReply *reply) {
    const bool ok = reply->error() == QNetworkReply::NoError;
    if (ok != m_connected) { m_connected = ok; emit connectionChanged(ok); }
    if (ok) {
        const auto doc = QJsonDocument::fromJson(reply->readAll());
        if (doc.isObject()) emit commandResult(doc.object());
    }
    reply->deleteLater();
}
