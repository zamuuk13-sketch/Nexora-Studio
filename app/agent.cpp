#include "agent.h"

Agent::Agent(QObject *parent) : QObject(parent) {}

QString Agent::buildSystemPrompt() const {
    return m_profile.systemPrompt.isEmpty()
        ? QStringLiteral("You are Nexora, an autonomous Roblox Studio creation agent. Do not return instructions for the user to paste manually. Produce structured Nexora tool calls that modify the connected Roblox Studio project. Inspect before mutating, keep operations deterministic, and report results.")
        : m_profile.systemPrompt;
}

QJsonArray Agent::planLocalTools(const QString &prompt) const {
    // Deterministic starter planner used for transport smoke tests. The real AI
    // adapter supplies the same command schema after the provider response is parsed.
    QJsonArray plan;
    const QString p = prompt.toLower();
    if (p.contains("part") || p.contains("bloco") || p.contains("plataforma")) {
        plan.append(QJsonObject{{"command", "create_instance"}, {"args", QJsonObject{{"className", "Part"}, {"name", "NexoraPart"}, {"parentPath", QJsonArray{"Workspace"}}}}});
    }
    if (plan.isEmpty()) {
        plan.append(QJsonObject{{"command", "ping"}, {"args", QJsonObject{}}});
    }
    return plan;
}
