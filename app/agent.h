#pragma once
#include <QObject>
#include <QJsonArray>
#include <QStringList>
#include "aiprofile.h"

class Agent final : public QObject {
    Q_OBJECT
public:
    explicit Agent(QObject *parent = nullptr);
    void setProfile(const AIProfile &profile) { m_profile = profile; }
    QString buildSystemPrompt() const;
    QJsonArray planLocalTools(const QString &prompt) const;

signals:
    void planReady(const QJsonArray &commands);
    void error(const QString &message);

private:
    AIProfile m_profile;
};
