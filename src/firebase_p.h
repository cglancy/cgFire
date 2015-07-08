#ifndef QTFIREBASE_FIREBASE_P_H
#define QTFIREBASE_FIREBASE_P_H

#include <QObject>
#include <QUrl>

class QNetworkAccessManager;

namespace QtFirebase
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

#endif // QTFIREBASE_FIREBASE_P_H
