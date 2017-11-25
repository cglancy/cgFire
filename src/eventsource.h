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
#ifndef CUTEFIRE_EVENTSOURCE_H
#define CUTEFIRE_EVENTSOURCE_H
#pragma once

#include <QObject>
#include <QUrl>
#include <QNetworkReply>
#include <QSslError>
#include "cutefire.h"

class QString;
class QVariant;
class QNetworkAccessManager;

namespace CuteFire
{

    class CUTEFIRE_API EventSource : public QObject
    {
        Q_OBJECT
    public:
        EventSource(QObject *parent = 0);

        void open(const QUrl &url, QNetworkAccessManager *nam);
        void close();

    signals:
        void redirected();
        void opened();
        void closed();
        void valueChanged(const QVariant &value);
        void valueUpdated(const QVariant &value);
        void childChanged(const QString &path, const QVariant &value);
        void childUpdated(const QString &path, const QVariant &value);
        void keepAlive();
        void authRevoked();
        void canceled();
        void error(const QString &message);

    private slots:
        void finished();
        void readyRead();
        void errorCode(QNetworkReply::NetworkError code);
        void sslErrors(const QList<QSslError> &errors);

    private:
        QByteArray eventValue(const QByteArray &line) const;

    private:
        enum State
        {
            Closed,
            Connecting,
            Opened,
            Closing
        };

        State _state;
        QUrl _url;
        QNetworkAccessManager *_nam;
        QNetworkReply *_reply;
    };

}

#endif // CUTEFIRE_EVENTSOURCE_H
