#include "BridgeClient.h"
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <QJsonDocument>
#include <QJsonObject>
BridgeClient::BridgeClient(QObject*p):QObject(p){}bool BridgeClient::connected()const{return ok_;}void BridgeClient::ping(){auto*x=n_.get(QNetworkRequest(QUrl("http://127.0.0.1:38471/ping")));connect(x,&QNetworkReply::finished,this,[this,x](){ok_=x->error()==QNetworkReply::NoError;emit changed();if(ok_)emit result(QString::fromUtf8(x->readAll()));else emit error(x->errorString());x->deleteLater();});}void BridgeClient::createPart(const QString&name){QNetworkRequest r(QUrl("http://127.0.0.1:38471/createPart"));r.setHeader(QNetworkRequest::ContentTypeHeader,"application/json");auto*x=n_.post(r,QJsonDocument(QJsonObject{{"name",name}}).toJson());connect(x,&QNetworkReply::finished,this,[this,x](){if(x->error()==QNetworkReply::NoError)emit result(QString::fromUtf8(x->readAll()));else emit error(x->errorString());x->deleteLater();});}
