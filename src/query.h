#ifndef CUTEFIRE_QUERY_H
#define CUTEFIRE_QUERY_H

#include "cutefire.h"
#include <QObject>
#include <QString>

class QNetworkAccessManager;

namespace CuteFire
{
    class Firebase;
    class QueryPrivate;

    class CUTEFIRE_API Query : public QObject
    {
        Q_OBJECT
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

    private:
        friend class Firebase;
        Query(Firebase *parent);
        Query(Query *query);

        static Query * orderByKey(Firebase *firebase);
        static Query * orderByPriority(Firebase *firebase);
        static Query * orderByValue(Firebase *firebase);
        static Query * orderByChild(Firebase *firebase, const QString &child);

    private:
        QueryPrivate * const d_ptr;
        Q_DECLARE_PRIVATE(Query)
        Q_DISABLE_COPY(Query)
    };
}

#endif // CUTEFIRE_QUERY_H
