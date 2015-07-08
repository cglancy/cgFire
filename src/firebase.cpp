#include "firebase.h"
#include "firebase_p.h"
#include "eventsource.h"
#include "query.h"

#include <QNetworkRequest>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QJsonDocument>
#include <QBuffer>
#include <QUrlQuery>

namespace QtFirebase {

QNetworkAccessManager * Firebase::networkAccessManager()
{
    if (0 == FirebasePrivate::networkAccessManager)
        FirebasePrivate::networkAccessManager = new QNetworkAccessManager();

    return FirebasePrivate::networkAccessManager;
}

void Firebase::setNetworkAccessManager(QNetworkAccessManager *nam)
{
    if (0 == FirebasePrivate::networkAccessManager)
        FirebasePrivate::networkAccessManager = nam;
}


Firebase::Firebase(const QUrl &url, QObject *parent) :
    QObject(parent),
    d_ptr(new FirebasePrivate(this, url))
{
}

QUrl Firebase::url() const
{
    Q_D(const Firebase);
    return d->url;
}

QString Firebase::key() const
{
    Q_D(const Firebase);
    QString key;
    QString path = d->url.path();
    if (!path.isEmpty())
    {
        QStringList list = path.split('/');
        if (list.size() > 0)
            key = list[list.size()-1];
    }

    return key;
}

Firebase * Firebase::root()
{
    Q_D(Firebase);
    QString path = d->url.path();
    if (path.isEmpty())
        return this;

    QUrl rootUrl(d->url);
    rootUrl.setPath("");
    return new Firebase(rootUrl, this);
}

Firebase * Firebase::parent()
{
    Q_D(Firebase);
    QString path = d->url.path();
    if (path.isEmpty())
        return 0;

    int index = path.lastIndexOf("/");
    QString parentPath = path.left(index);

    QUrl parentUrl(d->url);
    parentUrl.setPath(parentPath);
    return new Firebase(parentUrl, this);
}

Firebase * Firebase::child(const QString &child)
{
    Q_D(Firebase);
    QUrl childUrl(d->url);
    childUrl.setPath(d->url.path() + "/" + child);
    return new Firebase(childUrl, this);
}

Query * Firebase::orderByKey()
{
    return Query::orderByKey(this);
}

Query * Firebase::orderByPriority()
{
    return Query::orderByPriority(this);
}

Query * Firebase::orderByValue()
{
    return Query::orderByValue(this);
}

Query * Firebase::orderByChild(const QString &child)
{
    return Query::orderByChild(this, child);
}

void Firebase::once()
{
    Q_D(Firebase);
    QNetworkRequest request(d->restUrl());
    QNetworkReply *reply = networkAccessManager()->get(request);
    connect(reply, &QNetworkReply::finished, d, &FirebasePrivate::onceFinished);
}

void Firebase::set(const QVariant &value)
{
    Q_D(Firebase);
    QNetworkRequest request(d->restUrl());
    QNetworkReply *reply = networkAccessManager()->put(request, d->convert(value));
    connect(reply, &QNetworkReply::finished, d, &FirebasePrivate::setFinished);
}

void Firebase::update(const QVariant &value)
{
    Q_D(Firebase);
#if 0
    QByteArray ba = d->convert(value);
    QBuffer buffer(&ba);
    QNetworkRequest request(d->restUrl());
    QNetworkReply *reply = networkAccessManager()->sendCustomRequest(request, "PATCH", &buffer);
#else
    QNetworkRequest request(d->restUrl());
    request.setRawHeader("X-HTTP-Method-Override", "PATCH");
    QNetworkReply *reply = networkAccessManager()->post(request, d->convert(value));
#endif
    connect(reply, &QNetworkReply::finished, d, &FirebasePrivate::updateFinished);
}

void Firebase::push(const QVariant &value)
{
    Q_D(Firebase);
    QNetworkRequest request(d->restUrl());
    QNetworkReply *reply = networkAccessManager()->post(request, d->convert(value));
    connect(reply, &QNetworkReply::finished, d, &FirebasePrivate::pushFinished);
}

void Firebase::remove()
{
    Q_D(Firebase);
    QNetworkRequest request(d->restUrl());
    QNetworkReply *reply = networkAccessManager()->deleteResource(request);
    connect(reply, &QNetworkReply::finished, d, &FirebasePrivate::removeFinished);
}

void Firebase::listen()
{
    Q_D(Firebase);
    d->eventSource->open(d->restUrl(), networkAccessManager());
}

void Firebase::authWithCustomToken(const QByteArray &token)
{
    FirebasePrivate::authToken = token;
}

QByteArray Firebase::customToken() const
{
    return FirebasePrivate::authToken;
}

void Firebase::unauth()
{
    FirebasePrivate::authToken.clear();
}

//
//
//

QNetworkAccessManager * FirebasePrivate::networkAccessManager = 0;
QByteArray FirebasePrivate::authToken;

FirebasePrivate::FirebasePrivate(Firebase *firebase, const QUrl &u)
    : QObject(firebase),
      q_ptr(firebase),
      url(u),
      eventSource(0)
{
    eventSource = new EventSource(this);
    connect(eventSource, &EventSource::redirected, firebase, &Firebase::redirected);
    connect(eventSource, &EventSource::valueChanged, firebase, &Firebase::valueChanged);
    connect(eventSource, &EventSource::valueUpdated, firebase, &Firebase::valueUpdated);
    connect(eventSource, &EventSource::childChanged, firebase, &Firebase::childChanged);
    connect(eventSource, &EventSource::childUpdated, firebase, &Firebase::childUpdated);
}

QUrl FirebasePrivate::restUrl() const
{
    QUrl copy(url);
    copy.setPath(url.path() + ".json");

    if (!authToken.isEmpty())
    {
        QUrlQuery query;
        query.addQueryItem("auth", authToken);
        copy.setQuery(query);
    }

    return copy;
}

QByteArray FirebasePrivate::convert(const QVariant &value)
{
    QByteArray ba;
    QJsonDocument doc = QJsonDocument::fromVariant(value);
    if (!doc.isNull())
        ba = doc.toJson();
    else
        ba = value.toByteArray();

    return ba;
}

void FirebasePrivate::onceFinished()
{
    Q_Q(Firebase);

    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    if (reply)
    {
        if (reply->error() == QNetworkReply::NoError)
        {
            QByteArray data = reply->readAll();
            QJsonDocument doc = QJsonDocument::fromJson(data);
            if (doc.isObject())
            {
                emit q->onceFinished(doc.toVariant());
            }
            else
            {
                emit q->onceFinished(QVariant(data));
            }
        }
        else
        {
            qDebug() << "Error:: getValueFinishedInternal()";
            qDebug() << reply->errorString();
            emit q->onceFinished(QVariant());
        }

        reply->deleteLater();
    }
}

void FirebasePrivate::setFinished()
{
    Q_Q(Firebase);

    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    if (reply)
    {
        if (reply->error() == QNetworkReply::NoError)
        {
            emit q->setFinished();

            QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
            if (doc.isObject())
            {
                qDebug() << "setValueFinishedInternal()";
                qDebug() << doc;
            }
        }
        else
        {
            qDebug() << "Error:: setValueFinishedInternal()";
            qDebug() << reply->errorString();
            emit q->setFinished();
        }

        reply->deleteLater();
    }
}

void FirebasePrivate::updateFinished()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    if (reply)
    {
        if (reply->error() == QNetworkReply::NoError)
        {
            emit updateFinished();

            QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
            if (doc.isObject())
            {
                qDebug() << "updateFinishedInternal()";
                qDebug() << doc;
            }
        }
        else
        {
            qDebug() << "Error:: updateFinishedInternal()";
            qDebug() << reply->errorString();
            emit updateFinished();
        }

        reply->deleteLater();
    }
}

void FirebasePrivate::pushFinished()
{
    Q_Q(Firebase);

    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    if (reply)
    {
        if (reply->error() == QNetworkReply::NoError)
        {
            QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
            if (doc.isObject())
            {
                qDebug() << "pushFinishedInternal()";
                qDebug() << doc;
                QVariantMap map = doc.toVariant().toMap();
                QString id = map["name"].toString();
                emit q->pushFinished(id);
            }
            else
            {
                emit q->pushFinished(QString());
            }
        }
        else
        {
            qDebug() << "Error:: pushFinishedInternal()";
            qDebug() << reply->errorString();
            emit q->pushFinished(QString());
        }

        reply->deleteLater();
    }
}

void FirebasePrivate::removeFinished()
{
    Q_Q(Firebase);

    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    if (reply)
    {
        if (reply->error() == QNetworkReply::NoError)
        {
            emit q->removeFinished();
        }
        else
        {
            qDebug() << "Error:: removeFinishedInternal()";
            qDebug() << reply->errorString();
            emit q->removeFinished();
        }

        reply->deleteLater();
    }
}

}
