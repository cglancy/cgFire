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
#ifndef CGFIRE_FIREBASE_H
#define CGFIRE_FIREBASE_H
#pragma once

#include "cgfire.h"
#include <QObject>
#include <QUrl>
#include <QVariant>

class QNetworkAccessManager;
class QNetworkReply;

namespace cg
{
    class FirebasePrivate;
    class Query;

    class CGFIRE_API Firebase : public QObject
    {
        Q_OBJECT
        Q_DISABLE_COPY(Firebase)
        Q_DECLARE_PRIVATE(Firebase)
        FirebasePrivate * const d_ptr;

    public:
        Firebase(const QUrl &url, QObject *parent = 0);

        QUrl url() const;
        QString key() const;

        Firebase * root();
        Firebase * parent();
        Firebase * child(const QString &child);

        // query
        Query * orderByKey();
        Query * orderByPriority();
        Query * orderByValue();
        Query * orderByChild(const QString &child);

        // read
        void once();
        void listen();

        // write
        void set(const QVariant &value);
        void setPriority(const QVariant &priority);
        void setWithPriority(const QVariant &value, const QVariant &priority);
        void update(const QVariant &value);
        void push(const QVariant &value);

        // delete
        void remove();

        // authentication
        void authWithCustomToken(const QByteArray &token);
        QByteArray customToken() const;
        void unauth();

    signals:
        void onceFinished(const QVariant &value);
        void setFinished();
        void updateFinished();
        void pushFinished(const QString &id);
        void removeFinished();

        void redirected();
        void valueChanged(const QVariant &value);
        void valueUpdated(const QVariant &value);
        void childChanged(const QString &path, const QVariant &value);
        void childUpdated(const QString &path, const QVariant &value);

    public:
        static QNetworkAccessManager * networkAccessManager();
        static void setNetworkAccessManager(QNetworkAccessManager *nam);
    };
}

#endif // CGFIRE_FIREBASE_H
