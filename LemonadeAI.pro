QT += core gui network qml quick quickcontrols2
CONFIG += c++17
TEMPLATE = app
TARGET = LemonadeAI
SOURCES += src/main.cpp src/Config.cpp src/Agent.cpp src/GeminiClient.cpp src/BridgeClient.cpp
HEADERS += src/Config.h src/Agent.h src/GeminiClient.h src/BridgeClient.h
RESOURCES += resources.qrc
