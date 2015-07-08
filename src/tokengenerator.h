#ifndef QTFIREBASE_TOKENGENERATOR_H
#define QTFIREBASE_TOKENGENERATOR_H

#include "qt-firebase.h"
#include <QByteArray>
#include <QVariant>

namespace QtFirebase
{
    class TokenGeneratorPrivate;

    class QTFIREBASE_API TokenGenerator
    {
    public:
        TokenGenerator(const QByteArray &secret);
        ~TokenGenerator();

        QByteArray createToken(const QVariantMap &data, QVariantMap &options = QVariantMap());

        QVariantMap payload(const QByteArray &token);
        bool isValid(const QByteArray &token);

    private:
        TokenGeneratorPrivate * const d_ptr;
        Q_DECLARE_PRIVATE(TokenGenerator)
        Q_DISABLE_COPY(TokenGenerator)
    };
}

#endif // QTFIREBASE_TOKENGENERATOR_H
