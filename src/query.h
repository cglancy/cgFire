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
#ifndef CGFIRE_QUERY_H
#define CGFIRE_QUERY_H
#pragma once

#include "cgfire.h"
#include <QObject>
#include <QString>

class QNetworkAccessManager;

namespace cg
{
    class Firebase;
    class QueryPrivate;

    class CGFIRE_API Query : public QObject
    {
        Q_OBJECT
        Q_DISABLE_COPY(Query)
        Q_DECLARE_PRIVATE(Query)
        QueryPrivate * const d_ptr;

        friend class Firebase;
        Query(Firebase *parent);
        Query(Query *query);

        static Query * orderByKey(Firebase *firebase);
        static Query * orderByPriority(Firebase *firebase);
        static Query * orderByValue(Firebase *firebase);
        static Query * orderByChild(Firebase *firebase, const QString &child);

    public:
        Firebase * firebase() const;

        Query * equalTo(const QString &param);
        Query * startAt(const QString &param);
        Query * endAt(const QString &param);
        Query * limitToFirst(const QString &param);
        Query * limitToLast(const QString &param);

        void once();
        void listen();

    signals:
        void onceFinished(const QVariant &value);

        void redirected();
        void valueChanged(const QVariant &value);
        void valueUpdated(const QVariant &value);
        void childChanged(const QString &path, const QVariant &value);
        void childUpdated(const QString &path, const QVariant &value);
    };
}

#endif // CGFIRE_QUERY_H
