QT       += core testlib network
QT       -= gui

TARGET = TestCuteFire
CONFIG   += testcase 

TEMPLATE = app

SOURCES += \
    testcutefire.cpp

HEADERS += firebasesecret.h \
    testcutefire.h

INCLUDEPATH += ../src

CONFIG(debug, debug|release) {
    LIBS += -L../src/debug -lCuteFire0
    PRE_TARGETDEPS += ../src/debug/CuteFire0.dll
}
else {
    LIBS += -L../src/release -lCuteFire0
    PRE_TARGETDEPS += ../src/release/CuteFire0.dll
}
