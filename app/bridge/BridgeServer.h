#pragma once
#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QQueue>
#include <QJsonObject>
#include <QTimer>
#include <QHash>

class BridgeServer final : public QObject {
    Q_OBJECT
public:
    explicit BridgeServer(QObject *parent=nullptr);
    bool start(quint16 port=28473);
    QString enqueue(const QString &command, const QJsonObject &args={});
    bool running() const { return m_server.isListening(); }

signals:
    void studioChanged(bool connected);
    void resultReceived(const QJsonObject &result);

private slots:
    void newConnection();
    void readyRead();
    void disconnected();
    void heartbeat();

private:
    void route(QTcpSocket *socket, const QByteArray &request);
    void reply(QTcpSocket *socket, int code, const QJsonObject &json);
    QTcpServer m_server;
    QHash<QTcpSocket*, QByteArray> m_buffers;
    QQueue<QJsonObject> m_queue;
    QTimer m_heartbeat;
    bool m_studio=false;
    qint64 m_lastSeen=0;
};
