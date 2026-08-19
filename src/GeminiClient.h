#pragma once
#include <QObject>
#include <QNetworkAccessManager>
class Config;class GeminiClient:public QObject{Q_OBJECT public:explicit GeminiClient(Config*c,QObject*p=nullptr);void generate(const QString&);signals:void ready(const QString&);void error(const QString&);private:Config*c_;QNetworkAccessManager n_;};
