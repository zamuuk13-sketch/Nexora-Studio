QT += core network
CONFIG += c++17 console
CONFIG -= app_bundle
TEMPLATE = app

TARGET = NexoraBridgeServer

SOURCES += \
    main.cpp \
    NexoraBridgeServer.cpp

HEADERS += \
    NexoraBridgeServer.h

# The bridge server can be embedded into the Nexora desktop later.
# This standalone target is useful for transport testing.
