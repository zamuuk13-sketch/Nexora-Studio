#include <QCoreApplication>
#include <QDebug>
#include <QJsonDocument>
#include "NexoraBridgeServer.h"

int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);
    NexoraBridgeServer bridge;

    QObject::connect(&bridge, &NexoraBridgeServer::stateChanged, [](const QString &state) {
        qInfo().noquote() << "[NexoraBridge] state:" << state;
    });
    QObject::connect(&bridge, &NexoraBridgeServer::studioConnected, [] {
        qInfo() << "[NexoraBridge] Roblox Studio connected";
    });
    QObject::connect(&bridge, &NexoraBridgeServer::studioDisconnected, [] {
        qInfo() << "[NexoraBridge] Roblox Studio disconnected";
    });
    QObject::connect(&bridge, &NexoraBridgeServer::commandResult, [](const QJsonObject &result) {
        qInfo().noquote() << "[NexoraBridge] result:" << QString::fromUtf8(QJsonDocument(result).toJson(QJsonDocument::Compact));
    });

    if (!bridge.start(28473)) {
        qCritical() << "[NexoraBridge] Could not bind 127.0.0.1:28473";
        return 1;
    }

    qInfo().noquote() << "[NexoraBridge] listening on" << bridge.baseUrl();
    qInfo() << "[NexoraBridge] transport ready";

    return app.exec();
}
