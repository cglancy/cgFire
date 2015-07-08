#-------------------------------------------------
#
# Project created by QtCreator 2015-06-22T09:32:26
#
#-------------------------------------------------

QT       += core network
QT       -= gui

TARGET = QtFirebase
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
    qt-firebase.h

DEFINES += QTFIREBASE_EXPORTS

