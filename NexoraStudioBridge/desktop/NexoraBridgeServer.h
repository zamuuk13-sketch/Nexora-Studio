#pragma once

#include <QObject>
#include <QQueue>
#include <QJsonObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QTimer>

class NexoraBridgeServer final : public QObject {
    Q_OBJECT
public:
    explicit NexoraBridgeServer(QObject *parent = nullptr);

    bool start(quint16 port = 28473);
    void stop();
    bool isRunning() const;
    QString baseUrl() const;
    QString status() const;

    QString enqueueCommand(const QString &command, const QJsonObject &args = {});

signals:
    void stateChanged(const QString &state);
    void studioConnected();
    void studioDisconnected();
    void commandResult(const QJsonObject &result);

private slots:
    void onNewConnection();
    void onSocketReadyRead();
    void onSocketDisconnected();
    void onHeartbeatTimeout();

private:
    struct Client {
        QTcpSocket *socket = nullptr;
        QByteArray buffer;
    };

    void handleRequest(QTcpSocket *socket, const QByteArray &request);
    void sendJson(QTcpSocket *socket, int statusCode, const QJsonObject &body);
    QJsonObject dequeueCommand();
    QString nextRequestId();
    void setState(const QString &state);
    bool authorized(QTcpSocket *socket, const QMap<QByteArray, QByteArray> &headers) const;

    QTcpServer m_server;
    QQueue<QJsonObject> m_commands;
    QHash<QTcpSocket *, Client> m_clients;
    QTimer m_heartbeatTimer;
    QString m_state = QStringLiteral("stopped");
    QString m_token;
    quint16 m_port = 28473;
    bool m_studioSeen = false;
};
