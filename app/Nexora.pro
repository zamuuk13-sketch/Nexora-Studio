QT += core gui widgets network
CONFIG += c++17
TEMPLATE = app
TARGET = Nexora

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    bridge/BridgeServer.cpp \
    aiprofile.cpp \
    agent.cpp

HEADERS += \
    mainwindow.h \
    bridge/BridgeServer.h \
    aiprofile.h \
    agent.h

# Icon is optional; the build must not depend on a missing .ico file.
# win32:RC_ICONS = resources/nexora.ico
