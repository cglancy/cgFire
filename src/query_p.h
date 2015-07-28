#ifndef CUTEFIRE_QUERY_P_H
#define CUTEFIRE_QUERY_P_H

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
