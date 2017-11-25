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
#ifndef CUTEFIRE_QUERY_P_H
#define CUTEFIRE_QUERY_P_H
#pragma once

#include <QObject>
#include <QUrl>

namespace CuteFire
{
    class Firebase;
    class Query;
    class EventSource;

    class QueryPrivate : public QObject
    {
        Q_OBJECT
    public:
        enum Order
        {
            NoOrder,
            ByKeyOrder,
            ByValueOrder,
            ByPriorityOrder,
            ByChildOrder
        };

        enum Limit
        {
            NoLimit,
            ToFirstLimit,
            ToLastLimit
        };

        enum Range
        {
            NoRange         = 0x00,
            StartAtRange    = 0x01,
            EndAtRange      = 0x02,
            EqualToRange    = 0x04
        };


    public slots:
        void onceFinished();

    public:
        QueryPrivate(Query *query);
        void init(Order order, const QString &param);
        void init(Query *query, Limit limit, const QString &param);
        void init(Query *query, Range range, const QString &param);

        QUrl queryUrl() const;
        QString quoteParameter(const QString &p) const;

        QUrl url;
        Order order;
        QString orderParam;
        int rangeField;
        QString startAtRangeParam;
        QString endAtRangeParam;
        QString equalToRangeParam;
        Limit limit;
        QString limitParam;
        EventSource *eventSource;

        Query * const q_ptr;
        Q_DECLARE_PUBLIC(Query)
        Q_DISABLE_COPY(QueryPrivate)
    };
}

#endif // CUTEFIRE_QUERY_P_H
