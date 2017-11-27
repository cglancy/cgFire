QT += core network
QT -= gui

TARGET = cgFire
CONFIG   += dll
TEMPLATE = lib
VERSION	= 0.1.1

SOURCES += firebase.cpp \
    query.cpp \
    eventsource.cpp \
    tokengenerator.cpp

HEADERS += \
    firebase.h \
    firebase_p.h \
    query.h \
    query_p.h \
    eventsource.h \
    tokengenerator.h \
    tokengenerator_p.h \
    cgfire.h

DEFINES += CGFIRE_EXPORTS

