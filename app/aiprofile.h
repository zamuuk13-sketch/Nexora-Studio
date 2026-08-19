#pragma once
#include <QString>
#include <QJsonObject>

struct AIProfile {
    QString name;
    QString provider;
    QString model;
    QString apiKey;
    QString endpoint;
    QString systemPrompt;

    QJsonObject toJson() const;
    static AIProfile fromJson(const QJsonObject &obj);
};
