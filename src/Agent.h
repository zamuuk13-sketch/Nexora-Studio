#pragma once
#include <QObject>
class Config;class BridgeClient;class GeminiClient;class Agent:public QObject{Q_OBJECT Q_PROPERTY(bool busy READ busy NOTIFY busyChanged)public:explicit Agent(Config*c,BridgeClient*b,QObject*p=nullptr);bool busy()const;Q_INVOKABLE void ask(const QString&);signals:void message(const QString&,const QString&);void busyChanged();private:Config*c_;BridgeClient*b_;GeminiClient*g_;bool busy_=false;};
