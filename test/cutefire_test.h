#ifndef CUTEFIRE_TEST_H
#define CUTEFIRE_TEST_H

#include <QObject>
#include <QByteArray>
#include <QUrl>

class CuteFireTest : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();

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

#endif // CUTEFIRE_TEST_H
