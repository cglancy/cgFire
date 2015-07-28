#ifndef CUTEFIRE_FIREBASE_H
#define CUTEFIRE_FIREBASE_H

#include "cutefire.h"
#include <QObject>
#include <QUrl>
#include <QVariant>

class QNetworkAccessManager;
class QNetworkReply;

namespace CuteFire
{
    class FirebasePrivate;
    class Query;

    class CUTEFIRE_API Firebase : public QObject
    {
        Q_OBJECT
    public:
        static QNetworkAccessManager * networkAccessManager();
        static void setNetworkAccessManager(QNetworkAccessManager *nam);

    public:
        Firebase(const QUrl &url, QObject *parent = 0);

        QUrl url() const;
        QString key() const;

        Firebase * root();
        Firebase * parent();
        Firebase * child(const QString &child);

        // query
        Query * orderByKey();
        Query * orderByPriority();
        Query * orderByValue();
        Query * orderByChild(const QString &child);

        // read
        void once();
        void listen();

        // write
        void set(const QVariant &value);
        void setPriority(const QVariant &priority);
        void setWithPriority(const QVariant &value, const QVariant &priority);
        void update(const QVariant &value);
        void push(const QVariant &value);

        // delete
        void remove();

        // authentication
        void authWithCustomToken(const QByteArray &token);
        QByteArray customToken() const;
        void unauth();

    signals:
        void onceFinished(const QVariant &value);
        void setFinished();
        void updateFinished();
        void pushFinished(const QString &id);
        void removeFinished();

        void redirected();
        void valueChanged(const QVariant &value);
        void valueUpdated(const QVariant &value);
        void childChanged(const QString &path, const QVariant &value);
        void childUpdated(const QString &path, const QVariant &value);

    private:
        FirebasePrivate * const d_ptr;
        Q_DECLARE_PRIVATE(Firebase)
        Q_DISABLE_COPY(Firebase)
    };
}

#endif // CUTEFIRE_FIREBASE_H
