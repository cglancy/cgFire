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
    LIBS += -L./debug -lCuteFire0
    PRE_TARGETDEPS += ./debug/CuteFire0.dll
}
else {
    LIBS += -L./release -lCuteFire0
    PRE_TARGETDEPS += ./release/CuteFire0.dll
}
