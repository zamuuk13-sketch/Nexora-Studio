#include "aiprofile.h"

QJsonObject AIProfile::toJson() const {
    return {{"name", name}, {"provider", provider}, {"model", model}, {"apiKey", apiKey}, {"endpoint", endpoint}, {"systemPrompt", systemPrompt}};
}

AIProfile AIProfile::fromJson(const QJsonObject &o) {
    AIProfile p;
    p.name = o.value("name").toString();
    p.provider = o.value("provider").toString();
    p.model = o.value("model").toString();
    p.apiKey = o.value("apiKey").toString();
    p.endpoint = o.value("endpoint").toString();
    p.systemPrompt = o.value("systemPrompt").toString();
    return p;
}
