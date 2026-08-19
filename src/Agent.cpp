#include "Agent.h"
#include "Config.h"
#include "BridgeClient.h"
#include "GeminiClient.h"
Agent::Agent(Config*c,BridgeClient*b,QObject*p):QObject(p),c_(c),b_(b){g_=new GeminiClient(c_,this);connect(g_,&GeminiClient::ready,this,[this](const QString&t){busy_=false;emit busyChanged();emit message("assistant",t);});connect(g_,&GeminiClient::error,this,[this](const QString&t){busy_=false;emit busyChanged();emit message("system",t);});}bool Agent::busy()const{return busy_;}void Agent::ask(const QString&p){if(p.trimmed().isEmpty()||busy_)return;busy_=true;emit busyChanged();emit message("user",p);g_->generate(p);}
