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
#ifndef CGFIRE_TOKENGENERATOR_H
#define CGFIRE_TOKENGENERATOR_H
#pragma once

#include "cgfire.h"
#include <QByteArray>
#include <QVariant>

namespace cg
{
    class TokenGeneratorPrivate;

    class CGFIRE_API TokenGenerator
    {
        Q_DECLARE_PRIVATE(TokenGenerator)
        Q_DISABLE_COPY(TokenGenerator)
        TokenGeneratorPrivate * const d_ptr;

    public:
        TokenGenerator(const QByteArray &secret);
        ~TokenGenerator();

        QByteArray createToken(const QVariantMap &data, QVariantMap &options);

        QVariantMap payload(const QByteArray &token);
        bool isValid(const QByteArray &token);
    };
}

#endif // CGFIRE_TOKENGENERATOR_H
