QT += core testlib network
QT -= gui

TARGET = cgFireTest
CONFIG   += testcase 

TEMPLATE = app

SOURCES += firetest.cpp

HEADERS += firetest.h \
	firebasesecret.h

INCLUDEPATH += ../src

CONFIG(debug, debug|release) {
    LIBS += -L../src/debug -lcgFire0
    PRE_TARGETDEPS += ../src/debug/cgFire0.dll
}
else {
    LIBS += -L../src/release -lcgFire0
    PRE_TARGETDEPS += ../src/release/cgFire0.dll
}
