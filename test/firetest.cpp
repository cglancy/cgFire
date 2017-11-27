/**
* Copyright 2017 Charles Glancy (charles@glancyfamily.net)
*
* Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation
* files (the "Software"), to deal in the Software without restriction, including  without limitation the rights to use, copy,
* modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software
* is furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
* WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
* COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
* ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#include "firetest.h"
#include "firebasesecret.h"
#include "firebase.h"
#include "query.h"
#include "tokengenerator.h"
#include "eventsource.h"
#include "pushidgenerator.h"

#include <QTest>
#include <QSharedPointer>
#include <QSignalSpy>
#include <QThread>
#include <QDebug>


using namespace cg;

QTEST_MAIN(FireTest)

void FireTest::initTestCase()
{
    QByteArray firebaseUrlStr = FIREBASE_URL;
    QVERIFY(!firebaseUrlStr.isEmpty());
    firebaseUrl.setUrl(firebaseUrlStr);
    QVERIFY(firebaseUrl.isValid());
    QVERIFY(firebaseUrl.scheme() == "https");

    firebaseSecret = FIREBASE_SECRET;
    QVERIFY(!firebaseSecret.isEmpty());
}

void FireTest::cleanupTestCase()
{
}

void FireTest::testAccessors()
{
    QString urlStr = FIREBASE_URL + QString("/test");
    QSharedPointer<Firebase> testRef = QSharedPointer<Firebase>(new Firebase(QUrl(urlStr)));
    QVERIFY(testRef->url().url() == urlStr);

    QVERIFY(testRef->key() == "test");
}

void FireTest::testReferences()
{
    QString urlStr = FIREBASE_URL + QString("/test");
    QSharedPointer<Firebase> testRef = QSharedPointer<Firebase>(new Firebase(QUrl(urlStr)));
    QVERIFY(testRef->url().toString() == urlStr);

    Firebase *root = testRef->root();
    QVERIFY(root->url().toString() == FIREBASE_URL);

    QVERIFY(root->root() == root);
    QVERIFY(root->parent() == 0);

    Firebase *parent = testRef->parent();
    QVERIFY(parent->url().toString() == FIREBASE_URL);

    Firebase *child = testRef->child("test");
    QVERIFY(child->url().toString() == urlStr + "/test");
}

void FireTest::testRead()
{
    QSharedPointer<Firebase> testRef = QSharedPointer<Firebase>(new Firebase(QUrl(FIREBASE_URL)));
    Firebase *temp = testRef->child("test/value1");

    temp->once();
    QSignalSpy getSpy(temp, &Firebase::onceFinished);
    QVERIFY(getSpy.wait(5000));
    QList<QVariant> arguments = getSpy.takeFirst();
    QVERIFY(arguments.count() == 1);
    QCOMPARE(42, arguments.at(0).toInt());
}

void FireTest::testAuth()
{
    QVariantMap data;
    data["uid"] = "WSCXoL8KW6PELBfmdeBymsX2NKt2";

    QVariantMap options;
    options["admin"] = true;
    options["debug"] = true;

    TokenGenerator tokenGenerator(firebaseSecret);
    QByteArray token = tokenGenerator.createToken(data, options);

    QSharedPointer<Firebase> rootRef = QSharedPointer<Firebase>(new Firebase(firebaseUrl));
    Firebase *testRef = rootRef->child("test/pirate");
    testRef->authWithCustomToken(token);

    QVariantMap value;
    value["first"] = "Jack";
    value["last"] = "Sparrow";

    testRef->set(value);
    QSignalSpy setSpy(testRef, &Firebase::setFinished);
    QVERIFY(setSpy.wait(5000));

    testRef->unauth();
}

void FireTest::testSetAndOnce()
{
    QSharedPointer<Firebase> rootRef = QSharedPointer<Firebase>(new Firebase(firebaseUrl));
    Firebase *testRef = rootRef->child("test/users");

    QVariantMap user;
    QVariantMap value;
    value["first"] = "Jack";
    value["last"] = "Sparrow";

    user["pirate1"] = value;

    testRef->set(user);
    QSignalSpy setSpy(testRef, &Firebase::setFinished);
    QVERIFY(setSpy.wait(5000));

    Firebase *userRef = testRef->child("pirate1");

    userRef->once();
    QSignalSpy onceSpy(userRef, &Firebase::onceFinished);
    QVERIFY(onceSpy.wait(5000));
    QList<QVariant> arguments = onceSpy.takeFirst();
    QVERIFY(arguments.count() == 1);
    QVERIFY(arguments.at(0) == value);

    //testRef->set(true);
    //QVERIFY(setSpy.wait(5000));

    //testRef->once();
    //QVERIFY(onceSpy.wait(5000));
    //arguments = onceSpy.takeFirst();
    //QVERIFY(arguments.count() == 1);
    //QVERIFY(arguments.at(0).toBool() == true);
}

void FireTest::testQuery()
{
}

void FireTest::testListen()
{
    QSharedPointer<Firebase> rootRef = QSharedPointer<Firebase>(new Firebase(firebaseUrl));
    Firebase *testRef = rootRef->child("test/people");
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

void FireTest::testEventSource()
{
    QSharedPointer<EventSource> eventSource = QSharedPointer<EventSource>(new EventSource());
    QSignalSpy redirectSpy(eventSource.data(), &EventSource::redirected);
    QSignalSpy openedSpy(eventSource.data(), &EventSource::opened);
    QSignalSpy closedSpy(eventSource.data(), &EventSource::closed);

    QUrl sourceUrl(firebaseUrl.toString() + "listen.json");
    eventSource->open(sourceUrl, Firebase::networkAccessManager());

    QVERIFY(redirectSpy.wait(5000));
    QVERIFY(openedSpy.wait(5000));

    eventSource->close();
    QVERIFY(closedSpy.wait(5000));
}

void FireTest::testTokenGenerator()
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

void FireTest::testPushIdGenerator()
{
    PushIdGenerator pig;
    QByteArray id1, id2, id3, id4, id5, id6, id7;

    id1 = pig.generatePushId();
    id2 = pig.generatePushId();
    id3 = pig.generatePushId();
    id4 = pig.generatePushId();
    QVERIFY(id1 < id2 && id2 < id3 && id3 < id4);

    QThread::sleep(1);
    id5 = pig.generatePushId();
    id6 = pig.generatePushId();
    QVERIFY(id4 < id5 && id5 < id6);

    QThread::sleep(1);
    id6 = pig.generatePushId();
    id7 = pig.generatePushId();
    QVERIFY(id5 < id6 && id6 < id7);

    QThread::sleep(1);
    qint64 now = QDateTime::currentMSecsSinceEpoch();
    QByteArray id = pig.generatePushId();
    qint64 timestamp = pig.timestamp(id);
    QCOMPARE(timestamp, now);

    // that'll do pig, that'll do
}