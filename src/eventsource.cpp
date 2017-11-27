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
#include "eventsource.h"

#include <QNetworkRequest>
#include <QNetworkAccessManager>
#include <QJsonDocument>

namespace cg
{

EventSource::EventSource(QObject *parent) :
    QObject(parent),
    _state(Closed),
    _reply(0)
{
}

void EventSource::open(const QUrl &url, QNetworkAccessManager *nam)
{
    _url = url;
    _nam = nam;

    QNetworkRequest request(url);
    request.setRawHeader("Accept", "text/event-stream");
    _reply = nam->get(request);

    connect(_reply, &QNetworkReply::readyRead, this, &EventSource::readyRead);
    connect(_reply, &QNetworkReply::finished, this, &EventSource::finished);
    connect(_reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(errorCode(QNetworkReply::NetworkError)));
    connect(_reply, &QNetworkReply::sslErrors, this, &EventSource::sslErrors);

    _state = Connecting;
}

void EventSource::close()
{
    if (_state == Opened || _state == Connecting)
    {
        _state = Closing;
        if (_reply)
            _reply->abort();
    }
}

void EventSource::finished()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    Q_ASSERT(_reply == reply);

    if (reply)
    {
        QUrl redirectUrl = reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
        if (!redirectUrl.isEmpty() && _url != redirectUrl)
        {
            qDebug() << "EventSource: redirected to " << redirectUrl;
            emit redirected();
            reply->deleteLater();
            open(redirectUrl, _nam);
            return;
        }
        else if (_state == Closing)
        {
            _state = Closed;
            qDebug() << "EventSource: closed";
            emit closed();
        }

        reply->deleteLater();
        _reply = 0;
    }
}

void EventSource::readyRead()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());

    if (reply)
    {
        QByteArray line = reply->readLine();
        if (!line.isEmpty())
        {
            QByteArray eventStr = eventValue(line);
            qDebug() << "EventSource: Event = " << eventStr;

            if (!eventStr.isEmpty() && _state == Connecting)
            {
                _state = Opened;
                qDebug() << "EventSource: opened";
                emit opened();
            }

            line = reply->readLine();
            if (!line.isEmpty())
            {
                QByteArray data = eventValue(line);
                qDebug() << "EventSource: Data = " << data;
                QJsonDocument doc = QJsonDocument::fromJson(data);
                QVariantMap eventMap = doc.toVariant().toMap();
                QString path = eventMap["path"].toString();
                QVariantMap dataMap = eventMap["data"].toMap();

                if (eventStr == "put")
                {
                    if (path == "/")
                        emit valueChanged(dataMap);
                    else
                        emit childChanged(path, dataMap);
                }
                else if (eventStr == "patch")
                {
                    if (path == "/")
                        emit valueUpdated(dataMap);
                    else
                        emit childUpdated(path, dataMap);
                }
            }
            else
            {
                if (eventStr == "keep-alive")
                    emit keepAlive();
                else if (eventStr == "cancel")
                    emit canceled();
                else if (eventStr == "auth-revoked")
                    emit authRevoked();
            }
        }

        // read all data remaining
        reply->readAll();
    }
}

void EventSource::errorCode(QNetworkReply::NetworkError code)
{
    Q_UNUSED(code);
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    if (reply)
    {
        emit error(reply->errorString());
        reply->deleteLater();
    }
}

void EventSource::sslErrors(const QList<QSslError> &errors)
{
    Q_UNUSED(errors);
}

QByteArray EventSource::eventValue(const QByteArray &line) const
{
    QByteArray value;
    int index = line.indexOf(':');
    if (index > 0)
        value = line.right(line.size() - index  - 1);
    return value.trimmed();
}

}
