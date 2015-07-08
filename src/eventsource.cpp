#include "eventsource.h"

#include <QNetworkRequest>
#include <QNetworkAccessManager>
#include <QJsonDocument>

namespace QtFirebase
{

EventSource::EventSource(QObject *parent) :
    QObject(parent),
    _state(Closed)
{
}

void EventSource::open(const QUrl &url, QNetworkAccessManager *nam)
{
    _url = url;
    _nam = nam;

    QNetworkRequest request(url);
    request.setRawHeader("Accept", "text/event-stream");
    QNetworkReply *reply = nam->get(request);

    connect(reply, &QNetworkReply::readyRead, this, &EventSource::readyRead);
    connect(reply, &QNetworkReply::finished, this, &EventSource::finished);
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(errorCode(QNetworkReply::NetworkError)));
    connect(reply, &QNetworkReply::sslErrors, this, &EventSource::sslErrors);

    _state = Connecting;
}

void EventSource::close()
{
    _state = Closing;
}

void EventSource::finished()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    if (reply)
    {
        QUrl redirectUrl = reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
        if (!redirectUrl.isEmpty() && _url != redirectUrl)
        {
            qDebug() << "redirected to " << redirectUrl;
            emit redirected();
            open(redirectUrl, _nam);
        }
        else if (_state == Closing)
        {
            _state = Closed;
            emit closed();
        }

        reply->deleteLater();
    }
}

void EventSource::readyRead()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    if (reply)
    {
        if (_state == Connecting)
        {
            _state = Opened;
            emit opened();
        }

        QByteArray line = reply->readLine();
        if (!line.isEmpty())
        {
            QByteArray eventStr = eventValue(line);
            qDebug() << "Event = " << eventStr;

            line = reply->readLine();
            if (!line.isEmpty())
            {
                QByteArray data = eventValue(line);
                qDebug() << "Data = " << data;
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

        if (_state == Closing)
            reply->close();
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
    errors;
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
