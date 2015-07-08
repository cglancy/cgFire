QT       += core testlib network
QT       -= gui

TARGET = firebase_test
CONFIG   += testcase 

TEMPLATE = app

SOURCES += \
    qtfirebase_test.cpp

HEADERS += \
    qtfirebase_test.h

INCLUDEPATH += ../src

CONFIG(debug, debug|release) {
    LIBS += -L../src/debug -lQtFirebase0
    PRE_TARGETDEPS += ../src/debug/QtFirebase0.dll
}
else {
    LIBS += -L../src/release -lQtFirebase0
    PRE_TARGETDEPS += ../src/release/QtFirebase0.dll
}
