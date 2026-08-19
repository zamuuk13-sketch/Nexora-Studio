#pragma once
#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonObject>

class BridgeClient final : public QObject {
    Q_OBJECT
public:
    explicit BridgeClient(QObject *parent = nullptr);
    void start();
    void stop();
    bool connected() const { return m_connected; }
    QString enqueue(const QString &command, const QJsonObject &args = {});

signals:
    void connectionChanged(bool connected);
    void commandResult(const QJsonObject &result);

private slots:
    void poll();
    void onFinished(QNetworkReply *reply);

private:
    QNetworkAccessManager m_network;
    QTimer *m_timer = nullptr;
    QString m_baseUrl = QStringLiteral("http://127.0.0.1:28473");
    bool m_connected = false;
};
