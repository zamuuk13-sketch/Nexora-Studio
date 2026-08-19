#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "Config.h"
#include "Agent.h"
#include "BridgeClient.h"
int main(int argc,char** argv){QGuiApplication app(argc,argv);app.setOrganizationName("LemonadeAI");app.setApplicationName("LemonadeAI");Config config;BridgeClient bridge;Agent agent(&config,&bridge);QQmlApplicationEngine e;e.rootContext()->setContextProperty("appConfig",&config);e.rootContext()->setContextProperty("agent",&agent);e.rootContext()->setContextProperty("bridge",&bridge);e.load(QUrl("qrc:/qml/Main.qml"));if(e.rootObjects().isEmpty())return 1;return app.exec();}
