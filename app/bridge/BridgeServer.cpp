#include "BridgeServer.h"
#include <QDateTime>
#include <QHostAddress>
#include <QJsonDocument>
#include <QUuid>

BridgeServer::BridgeServer(QObject *p):QObject(p){
    connect(&m_server,&QTcpServer::newConnection,this,&BridgeServer::newConnection);
    m_heartbeat.setInterval(1000);
    connect(&m_heartbeat,&QTimer::timeout,this,&BridgeServer::heartbeat);
}

bool BridgeServer::start(quint16 port){
    if(m_server.isListening()) return true;
    if(!m_server.listen(QHostAddress::LocalHost,port)) return false;
    m_heartbeat.start(); return true;
}

QString BridgeServer::enqueue(const QString &command,const QJsonObject &args){
    const QString id=QUuid::createUuid().toString(QUuid::WithoutBraces);
    m_queue.enqueue(QJsonObject{{"requestId",id},{"command",command},{"args",args}});
    return id;
}

void BridgeServer::newConnection(){
    while(m_server.hasPendingConnections()){
        auto *s=m_server.nextPendingConnection(); m_buffers[s]={};
        connect(s,&QTcpSocket::readyRead,this,&BridgeServer::readyRead);
        connect(s,&QTcpSocket::disconnected,this,&BridgeServer::disconnected);
    }
}

void BridgeServer::readyRead(){
    auto *s=qobject_cast<QTcpSocket*>(sender()); if(!s) return;
    auto &b=m_buffers[s]; b+=s->readAll();
    const int h=b.indexOf("\r\n\r\n"); if(h<0) return;
    int len=0; for(const auto &line:b.left(h).split('\n')){
        const int c=line.indexOf(':');
        if(c>0 && line.left(c).trimmed().compare("Content-Length",Qt::CaseInsensitive)==0) len=line.mid(c+1).trimmed().toInt();
    }
    const int start=h+4; if(b.size()<start+len) return;
    const QByteArray req=b.left(start+len); b.remove(0,start+len); route(s,req);
}

void BridgeServer::route(QTcpSocket *s,const QByteArray &req){
    const int first=req.indexOf("\r\n"); if(first<0){reply(s,400,{{"error","bad request"}});return;}
    const auto parts=req.left(first).split(' '); if(parts.size()<2){reply(s,400,{{"error","bad request"}});return;}
    const QByteArray method=parts[0]; const QByteArray path=parts[1].split('?')[0];
    const int h=req.indexOf("\r\n\r\n"); const QByteArray body=h<0?QByteArray():req.mid(h+4);
    if(s->peerAddress()!=QHostAddress::LocalHost){reply(s,403,{{"error","loopback only"}});return;}
    if(method=="GET" && path=="/health") { reply(s,200,{{"status","ok"},{"protocol","NEXORA_BRIDGE_V1"},{"studioConnected",m_studio}}); return; }
    if(method=="GET" && path=="/next") {
        m_lastSeen=QDateTime::currentMSecsSinceEpoch();
        if(!m_studio){m_studio=true;emit studioChanged(true);}
        reply(s,200,m_queue.isEmpty()?QJsonObject{{"command",QJsonValue::Null}}:m_queue.dequeue()); return;
    }
    if(method=="POST" && path=="/hello") {m_lastSeen=QDateTime::currentMSecsSinceEpoch(); if(!m_studio){m_studio=true;emit studioChanged(true);} reply(s,200,{{"ok",true}});return;}
    if(method=="POST" && path=="/result") {auto d=QJsonDocument::fromJson(body);if(!d.isObject()){reply(s,400,{{"error","invalid json"}});return;}emit resultReceived(d.object());reply(s,200,{{"ok",true}});return;}
    reply(s,404,{{"error","not found"}});
}

void BridgeServer::reply(QTcpSocket *s,int code,const QJsonObject &json){
    const QByteArray p=QJsonDocument(json).toJson(QJsonDocument::Compact);
    const QByteArray text=code==200?"OK":(code==403?"Forbidden":(code==404?"Not Found":"Bad Request"));
    QByteArray r="HTTP/1.1 "+QByteArray::number(code)+" "+text+"\r\nContent-Type: application/json\r\nContent-Length: "+QByteArray::number(p.size())+"\r\nConnection: close\r\n\r\n"+p;
    s->write(r);s->disconnectFromHost();
}

void BridgeServer::disconnected(){
    auto *s=qobject_cast<QTcpSocket*>(sender()); if(!s)return; m_buffers.remove(s);s->deleteLater();
}
void BridgeServer::heartbeat(){
    if(m_studio && QDateTime::currentMSecsSinceEpoch()-m_lastSeen>3000){m_studio=false;emit studioChanged(false);}
}
