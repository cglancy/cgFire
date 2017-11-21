#ifndef TESTCUTEFIRE_H
#define TESTCUTEFIRE_H

#include <QObject>
#include <QByteArray>
#include <QUrl>

class TestCuteFire : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
    void cleanupTestCase();

    void testAccessors();
    void testReferences();
    void testRead();
    void testTokenGenerator();
    void testAuth();
    void testSetAndOnce();
    void testQuery();
    void testListen();
    void testEventSource();

private:
    QUrl firebaseUrl;
    QByteArray firebaseSecret;
};

#endif // TESTCUTEFIRE_H
