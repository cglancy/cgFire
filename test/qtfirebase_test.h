#ifndef QTFIREBASE_TEST_H
#define QTFIREBASE_TEST_H

#include <QObject>
#include <QByteArray>
#include <QUrl>

class QtFirebaseTest : public QObject
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

private:
    QUrl firebaseUrl;
    QByteArray firebaseSecret;
};

#endif // QTFIREBASE_TEST_H
