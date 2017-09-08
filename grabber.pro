TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp
CONFIG +=network
LIBS += -lwsock32 -lws2_32
QMAKE_LFLAGS += -static -static-libgcc

include(deployment.pri)
qtcAddDeployment()

