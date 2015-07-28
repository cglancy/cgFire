#include "cutefire_test.h"
#include <firebase.h>
#include <query.h>
#include <tokengenerator.h>
#include <eventsource.h>

#include <QTest>
#include <QSharedPointer>
#include <QSignalSpy>
#include <QDebug>

#define WEATHER_URL "https://publicdata-weather.firebaseio.com"

using namespace CuteFire;


QTEST_MAIN(CuteFireTest)

void CuteFireTest::initTestCase()
{
    // verify environment variables
    QByteArray firebaseUrlStr = qgetenv("FIREBASE_URL");
    QVERIFY(!firebaseUrlStr.isEmpty());
    firebaseUrl.setUrl(firebaseUrlStr);
    QVERIFY(firebaseUrl.isValid());
    QVERIFY(firebaseUrl.scheme() == "https");

    firebaseSecret = qgetenv("FIREBASE_SECRET");
    QVERIFY(!firebaseSecret.isEmpty());
}

void CuteFireTest::testAccessors()
{
    QString urlStr = QString(WEATHER_URL) + "/portland";
    QSharedPointer<Firebase> testRef = QSharedPointer<Firebase>(new Firebase(QUrl(urlStr)));
    QVERIFY(testRef->url().url() == urlStr);

    QVERIFY(testRef->key() == "portland");
}

void CuteFireTest::testReferences()
{
    QString urlStr = QString(WEATHER_URL) + "/portland";
    QSharedPointer<Firebase> testRef = QSharedPointer<Firebase>(new Firebase(QUrl(urlStr)));
    QVERIFY(testRef->url().toString() == urlStr);

    Firebase *root = testRef->root();
    QVERIFY(root->url().toString() == WEATHER_URL);

    QVERIFY(root->root() == root);
    QVERIFY(root->parent() == 0);

    Firebase *parent = testRef->parent();
    QVERIFY(parent->url().toString() == WEATHER_URL);

    Firebase *child = testRef->child("currently");
    QVERIFY(child->url().toString() == urlStr + "/currently");
}

void CuteFireTest::testRead()
{
    QSharedPointer<Firebase> testRef = QSharedPointer<Firebase>(new Firebase(QUrl(WEATHER_URL)));
    Firebase *temp = testRef->child("portland/currently/temperature");

    temp->once();
    QSignalSpy getSpy(temp, &Firebase::onceFinished);
    QVERIFY(getSpy.wait(5000));
    QList<QVariant> arguments = getSpy.takeFirst();
    QVERIFY(arguments.count() == 1);

    qDebug() << "Temperature in Portland is " << arguments.at(0).toString();
}

void CuteFireTest::testTokenGenerator()
{
    TokenGenerator tokenGenerator(firebaseSecret);

    QVariantMap data;
    data["uid"] = "custom:1";

    QVariantMap options;
    options["admin"] = true;
    options["debug"] = true;

    QByteArray token = tokenGenerator.createToken(data, options);
    qDebug() << "token = " << token;
    qDebug() << "verify with http://jwt.io";

    QVERIFY(tokenGenerator.isValid(token));

    QVariantMap payload = tokenGenerator.payload(token);
    QVariantMap dmap = payload["d"].toMap();
    QVERIFY(dmap["uid"] == "custom:1");
    QVERIFY(payload["admin"] == true);
    QVERIFY(payload["debug"] == true);
}

void CuteFireTest::testAuth()
{
    QVariantMap data;
    data["uid"] = "custom:1";

    QVariantMap options;
    options["admin"] = true;
    options["debug"] = true;

    TokenGenerator tokenGenerator(firebaseSecret);
    QByteArray token = tokenGenerator.createToken(data, options);

    QSharedPointer<Firebase> rootRef = QSharedPointer<Firebase>(new Firebase(firebaseUrl));
    Firebase *testRef = rootRef->child("test");
    testRef->authWithCustomToken(token);

    QVariantMap value;
    value["first"] = "Jack";
    value["last"] = "Sparrow";

    testRef->set(value);
    QSignalSpy setSpy(testRef, &Firebase::setFinished);
    QVERIFY(setSpy.wait(5000));

    testRef->unauth();
}

void CuteFireTest::testSetAndOnce()
{
    QSharedPointer<Firebase> rootRef = QSharedPointer<Firebase>(new Firebase(firebaseUrl));
    Firebase *testRef = rootRef->child("test");

    QVariantMap value;
    value["first"] = "Jack";
    value["last"] = "Sparrow";

    testRef->set(value);
    QSignalSpy setSpy(testRef, &Firebase::setFinished);
    QVERIFY(setSpy.wait(5000));

    testRef->once();
    QSignalSpy onceSpy(testRef, &Firebase::onceFinished);
    QVERIFY(onceSpy.wait(5000));
    QList<QVariant> arguments = onceSpy.takeFirst();
    QVERIFY(arguments.count() == 1);
    QVERIFY(arguments.at(0) == value);

    testRef->set(true);
    QVERIFY(setSpy.wait(5000));

    testRef->once();
    QVERIFY(onceSpy.wait(5000));
    arguments = onceSpy.takeFirst();
    QVERIFY(arguments.count() == 1);
    QVERIFY(arguments.at(0).toBool() == true);
}

void CuteFireTest::testQuery()
{
}

void CuteFireTest::testListen()
{
    QSharedPointer<Firebase> rootRef = QSharedPointer<Firebase>(new Firebase(firebaseUrl));
    Firebase *testRef = rootRef->child("test");
    testRef->listen();

    // wait for redirect
    QSignalSpy redirectSpy(testRef, &Firebase::redirected);
    QSignalSpy watchSpy(testRef, &Firebase::valueChanged);

    QVERIFY(redirectSpy.wait(5000));

    QVariantMap value1;
    value1["first"] = "Will";
    value1["last"] = "Turner";
    testRef->set(value1);
    QVERIFY(watchSpy.wait(5000));

    QVERIFY(watchSpy.count() == 1);

    QVariantMap value2;
    value2["first"] = "Elizabeth";
    value2["last"] = "Swann";
    testRef->set(value2);
    QVERIFY(watchSpy.wait(5000));

    QVERIFY(watchSpy.count() == 2);
}

void CuteFireTest::testEventSource()
{
    QSharedPointer<EventSource> eventSource = QSharedPointer<EventSource>(new EventSource());
    QSignalSpy redirectSpy(eventSource.data(), &EventSource::redirected);
    QSignalSpy openedSpy(eventSource.data(), &EventSource::opened);
    QSignalSpy closedSpy(eventSource.data(), &EventSource::closed);

    QUrl sourceUrl(firebaseUrl.toString() + "test.json");
    eventSource->open(sourceUrl, Firebase::networkAccessManager());

    QVERIFY(redirectSpy.wait(5000));
    QVERIFY(openedSpy.wait(5000));

    eventSource->close();
    QVERIFY(closedSpy.wait(5000));
}
