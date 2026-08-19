QT += core gui widgets network
CONFIG += c++17
TEMPLATE = app
TARGET = Nexora

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    bridge/BridgeServer.cpp \
    aiprofiler.cpp \
    agent.cpp

HEADERS += \
    mainwindow.h \
    bridge/BridgeServer.h \
    aiprofiler.h \
    agent.h

win32:RC_ICONS = resources/nexora.ico
