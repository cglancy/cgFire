#ifndef QTFIREBASE_TOKENGENERATOR_P_H
#define QTFIREBASE_TOKENGENERATOR_P_H

#include <QByteArray>
#include <QVariant>

namespace QtFirebase
{
    class TokenGeneratorPrivate
    {
    public:
        TokenGeneratorPrivate(const QByteArray &secret);

        QByteArray computeToken(const QVariantMap &claims);
        QVariantMap commonHeader();
        QByteArray jsonEncode(const QVariantMap &map);
        QByteArray sign(const QByteArray &b);
        QByteArray base64UrlEncode(const QByteArray &b);

        QVariantMap jsonDecode(const QByteArray &b);
        QByteArray base64UrlDecode(const QByteArray &b);

        static const int version;
        static const char tokenSeparator;
        QByteArray secret;

        Q_DISABLE_COPY(TokenGeneratorPrivate)
    };
}

#endif // QTFIREBASE_TOKENGENERATOR_P_H
