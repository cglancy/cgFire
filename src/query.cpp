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
#include "query.h"
#include "query_p.h"
#include "firebase.h"
#include "eventsource.h"

#include <QNetworkRequest>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QJsonDocument>
#include <QUrlQuery>

namespace cg
{

Query::Query(Firebase *parent)
: QObject(parent),
  d_ptr(new QueryPrivate(this))
{
}

Query::Query(Query *query)
: QObject(query->parent()),
  d_ptr(new QueryPrivate(this))
{
}

Firebase * Query::firebase() const
{
    return qobject_cast<Firebase*>(parent());
}

Query * Query::orderByKey(Firebase *firebase)
{
    Query *q = new Query(firebase);
    q->d_ptr->init(QueryPrivate::ByKeyOrder, "$key");
    return q;
}

Query * Query::orderByPriority(Firebase *firebase)
{
    Query *q = new Query(firebase);
    q->d_ptr->init(QueryPrivate::ByPriorityOrder, "$priority");
    return q;
}

Query * Query::orderByValue(Firebase *firebase)
{
    Query *q = new Query(firebase);
    q->d_ptr->init(QueryPrivate::ByValueOrder, "$value");
    return q;
}

Query * Query::orderByChild(Firebase *firebase, const QString &child)
{
    Query *q = new Query(firebase);
    q->d_ptr->init(QueryPrivate::ByChildOrder, child);
    return q;
}

Query * Query::equalTo(const QString &param)
{
    Query *q = new Query(this);
    q->d_ptr->init(this, QueryPrivate::EqualToRange, param);
    return q;
}

Query * Query::startAt(const QString &param)
{
    Query *q = new Query(this);
    q->d_ptr->init(this, QueryPrivate::StartAtRange, param);
    return q;
}

Query * Query::endAt(const QString &param)
{
    Query *q = new Query(this);
    q->d_ptr->init(this, QueryPrivate::EndAtRange, param);
    return q;
}

Query * Query::limitToFirst(const QString &param)
{
    Query *q = new Query(this);
    q->d_ptr->init(this, QueryPrivate::ToFirstLimit, param);
    return q;
}

Query * Query::limitToLast(const QString &param)
{
    Query *q = new Query(this);
    q->d_ptr->init(this, QueryPrivate::ToLastLimit, param);
    return q;
}


void Query::once()
{
    Q_D(Query);
    QNetworkRequest request(d->queryUrl());
    QNetworkReply *reply = firebase()->networkAccessManager()->get(request);
    connect(reply, &QNetworkReply::finished, d, &QueryPrivate::onceFinished);
}

void Query::listen()
{
    Q_D(Query);
    d->eventSource->open(d->queryUrl(), firebase()->networkAccessManager());
}

QueryPrivate::QueryPrivate(Query *query)
    : QObject(query),
      q_ptr(query)
{
    url = query->firebase()->url();
    order = NoOrder;
    limit = NoLimit;
    rangeField = NoRange;

    eventSource = new EventSource(this);
    connect(eventSource, &EventSource::redirected, query, &Query::redirected);
    connect(eventSource, &EventSource::valueChanged, query, &Query::valueChanged);
    connect(eventSource, &EventSource::valueUpdated, query, &Query::valueUpdated);
    connect(eventSource, &EventSource::childChanged, query, &Query::childChanged);
    connect(eventSource, &EventSource::childUpdated, query, &Query::childUpdated);
}

void QueryPrivate::init(Order order, const QString &param)
{
    this->order = order;
    orderParam = param;
}

void QueryPrivate::init(Query *query, Limit limit, const QString &param)
{
    order = query->d_ptr->order;
    orderParam = query->d_ptr->orderParam;
    rangeField |= query->d_ptr->rangeField;
    startAtRangeParam = query->d_ptr->startAtRangeParam;
    endAtRangeParam = query->d_ptr->endAtRangeParam;

    this->limit = limit;
    limitParam = param;
}

void QueryPrivate::init(Query *query, Range range, const QString &param)
{
    order = query->d_ptr->order;
    orderParam = query->d_ptr->orderParam;
    limit = query->d_ptr->limit;
    limitParam = query->d_ptr->limitParam;

    rangeField |= range;
    if (range == StartAtRange)
        startAtRangeParam = param;
    else if (range == EndAtRange)
        endAtRangeParam = param;
    else if (range == EqualToRange)
        equalToRangeParam = param;
}

void QueryPrivate::onceFinished()
{
    Q_Q(Query);

    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    if (reply)
    {
        if (reply->error() == QNetworkReply::NoError)
        {
            QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
            if (doc.isObject())
            {
                emit q->onceFinished(doc.toVariant());
            }
        }
        else
        {
            qDebug() << "Error:: onceFinished()";
            qDebug() << reply->errorString();
            emit q->onceFinished(QVariant());
        }

        reply->deleteLater();
    }
}

QUrl QueryPrivate::queryUrl() const
{
    Q_Q(const Query);

    QUrl resultUrl(url);
    resultUrl.setPath(url.path() + ".json");

    QUrlQuery query;

    // order
    query.addQueryItem("orderBy", quoteParameter(orderParam));

    // limit
    if (limit == ToFirstLimit)
        query.addQueryItem("limitToFirst", limitParam);
    else if (limit == ToLastLimit)
        query.addQueryItem("limitToLast", limitParam);

    // range
    if (rangeField & StartAtRange && rangeField & EndAtRange)
    {
        query.addQueryItem("startAt", startAtRangeParam);
        query.addQueryItem("endAt", endAtRangeParam);
    }
    else if (rangeField & StartAtRange)
        query.addQueryItem("startAt", startAtRangeParam);
    else if (rangeField & EndAtRange)
        query.addQueryItem("endAt", endAtRangeParam);
    else if (rangeField & EqualToRange)
        query.addQueryItem("equalTo", equalToRangeParam);

    //query.addQueryItem("print", "pretty");

    QByteArray token = q->firebase()->customToken();
    if (!token.isEmpty())
        query.addQueryItem("auth", token);

    resultUrl.setQuery(query);
    return resultUrl;
}

QString QueryPrivate::quoteParameter(const QString &p) const
{
    return QString("%1%2%3").arg("\"").arg(p).arg("\"");
}

}
