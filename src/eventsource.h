#ifndef QTFIREBASE_EVENTSOURCE_H
#define QTFIREBASE_EVENTSOURCE_H

#include <QObject>
#include <QUrl>
#include <QNetworkReply>
#include <QSslError>

class QString;
class QVariant;
class QNetworkAccessManager;

namespace QtFirebase
{

    class EventSource : public QObject
    {
        Q_OBJECT
    public:
        EventSource(QObject *parent = 0);

        void open(const QUrl &url, QNetworkAccessManager *nam);
        void close();

    signals:
        void redirected();
        void opened();
        void closed();
        void valueChanged(const QVariant &value);
        void valueUpdated(const QVariant &value);
        void childChanged(const QString &path, const QVariant &value);
        void childUpdated(const QString &path, const QVariant &value);
        void keepAlive();
        void authRevoked();
        void canceled();
        void error(const QString &message);

    private slots:
        void finished();
        void readyRead();
        void errorCode(QNetworkReply::NetworkError code);
        void sslErrors(const QList<QSslError> &errors);

    private:
        QByteArray eventValue(const QByteArray &line) const;

    private:
        enum State
        {
            Closed,
            Connecting,
            Opened,
            Closing
        };

        State _state;
        QUrl _url;
        QNetworkAccessManager *_nam;
    };

}

#endif // QTFIREBASE_EVENTSOURCE_H
