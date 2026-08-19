#pragma once
#include <QObject>
#include <QNetworkAccessManager>
class BridgeClient:public QObject{Q_OBJECT Q_PROPERTY(bool connected READ connected NOTIFY changed)public:explicit BridgeClient(QObject*p=nullptr);bool connected()const;Q_INVOKABLE void ping();Q_INVOKABLE void createPart(const QString&);signals:void changed();void result(const QString&);void error(const QString&);private:QNetworkAccessManager n_;bool ok_=false;};
