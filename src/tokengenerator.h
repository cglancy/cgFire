#ifndef CUTEFIRE_TOKENGENERATOR_H
#define CUTEFIRE_TOKENGENERATOR_H

#include "cutefire.h"
#include <QByteArray>
#include <QVariant>

namespace CuteFire
{
    class TokenGeneratorPrivate;

    class CUTEFIRE_API TokenGenerator
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

#endif // CUTEFIRE_TOKENGENERATOR_H
