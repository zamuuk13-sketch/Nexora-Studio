#include "GeminiClient.h"
#include "Config.h"
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
GeminiClient::GeminiClient(Config*c,QObject*p):QObject(p),c_(c){}
void GeminiClient::generate(const QString&prompt){if(c_->apiKey().isEmpty()){emit error("Configure a Gemini API Key primeiro.");return;}QUrl u("https://generativelanguage.googleapis.com/v1beta/models/gemini-2.5-flash:generateContent?key="+c_->apiKey());QNetworkRequest r(u);r.setHeader(QNetworkRequest::ContentTypeHeader,"application/json");QJsonObject part{{"text",c_->instructions()+"\n\nUSUARIO:\n"+prompt}};QJsonObject body{{"contents",QJsonArray{QJsonObject{{"parts",QJsonArray{part}}}}}};auto*x=n_.post(r,QJsonDocument(body).toJson());connect(x,&QNetworkReply::finished,this,[this,x](){if(x->error()!=QNetworkReply::NoError){emit error(x->errorString());x->deleteLater();return;}auto d=QJsonDocument::fromJson(x->readAll());auto cs=d.object().value("candidates").toArray();QString out;if(!cs.isEmpty()){auto ps=cs.first().toObject().value("content").toObject().value("parts").toArray();for(auto v:ps)out+=v.toObject().value("text").toString();}emit out.isEmpty()?error("Resposta vazia do Gemini."):ready(out);x->deleteLater();});}
