QT += core gui widgets network
CONFIG += c++17
TEMPLATE = app
TARGET = Nexora

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    bridgeclient.cpp \
    aiprofiler.cpp \
    agent.cpp

HEADERS += \
    mainwindow.h \
    bridgeclient.h \
    aiprofiler.h \
    agent.h

RESOURCES += resources.qrc

win32:RC_ICONS = resources/nexora.ico
