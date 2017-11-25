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
#ifndef CUTEFIRE_FIREBASE_P_H
#define CUTEFIRE_FIREBASE_P_H
#pragma once

#include <QObject>
#include <QUrl>

class QNetworkAccessManager;

namespace CuteFire
{
    class Firebase;
    class EventSource;
    class QueryPrivate;

    class FirebasePrivate : public QObject
    {
        Q_OBJECT
    public:
        FirebasePrivate(Firebase *firebase, const QUrl &url);

    public slots:
        void onceFinished();
        void setFinished();
        void updateFinished();
        void pushFinished();
        void removeFinished();

    public:
        QUrl restUrl() const;
        QUrl priorityUrl() const;
        QByteArray convert(const QVariant &value);

        static QNetworkAccessManager *networkAccessManager;
        static QByteArray authToken;
        QUrl url;
        EventSource *eventSource;

        Firebase * const q_ptr;
        Q_DECLARE_PUBLIC(Firebase)
        Q_DISABLE_COPY(FirebasePrivate)
    };
}

#endif // CUTEFIRE_FIREBASE_P_H
