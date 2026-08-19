#pragma once
#include <QObject>
#include <QSettings>
class Config:public QObject{Q_OBJECT Q_PROPERTY(QString apiKey READ apiKey WRITE setApiKey NOTIFY changed) Q_PROPERTY(QString aiName READ aiName WRITE setAiName NOTIFY changed) Q_PROPERTY(QString instructions READ instructions WRITE setInstructions NOTIFY changed) public:explicit Config(QObject*p=nullptr);QString apiKey()const;QString aiName()const;QString instructions()const;void setApiKey(const QString&);void setAiName(const QString&);void setInstructions(const QString&);Q_INVOKABLE void save();signals:void changed();private:QSettings s_;QString key_,name_,instructions_;};
