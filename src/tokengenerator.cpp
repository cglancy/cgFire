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
#include "tokengenerator.h"
#include "tokengenerator_p.h"

#include <QDateTime>
#include <QJsonDocument>
#include <QMessageAuthenticationCode>

namespace cg
{

TokenGenerator::TokenGenerator(const QByteArray &secret)
    : d_ptr(new TokenGeneratorPrivate(secret))
{
}

TokenGenerator::~TokenGenerator()
{

}

QByteArray TokenGenerator::createToken(const QVariantMap &data, QVariantMap &options)
{
    Q_D(TokenGenerator);

    QByteArray token;

    if (data.isEmpty())
        return token;

    QVariantMap claims;
    claims["v"] = d->version;
    claims["iat"] = QDateTime::currentMSecsSinceEpoch() / 1000;
    claims["d"] = data;

    if (!options.isEmpty())
    {
        if (options.contains("expires"))
            claims["exp"] = options["expires"].toDateTime().toMSecsSinceEpoch() / 1000;
        if (options.contains("notBefore"))
            claims["nbf"] = options["notBefore"].toDateTime().toMSecsSinceEpoch() / 1000;
        if (options.contains("admin"))
            claims["admin"] = options["admin"];
        if (options.contains("debug"))
            claims["debug"] = options["debug"];
    }

    token = d->computeToken(claims);

    return token;
}

QVariantMap TokenGenerator::payload(const QByteArray &token)
{
    Q_D(TokenGenerator);

    QVariantMap map;

    QList<QByteArray> list = token.split('.');
    if (list.count() != 3)
        return map;

    map = d->jsonDecode(d->base64UrlDecode(list[1]));
    return map;
}

bool TokenGenerator::isValid(const QByteArray &token)
{
    Q_D(TokenGenerator);

    QList<QByteArray> list = token.split('.');
    if (list.count() != 3)
        return false;

    QVariantMap header = d->jsonDecode(d->base64UrlDecode(list[0]));
    if (header != d->commonHeader())
        return false;

    QByteArray secureBits = list[0] + d->tokenSeparator + list[1];
    QByteArray signedBits = d->sign(secureBits);

    if (signedBits != list[2])
        return false;

    return true;
}

const int TokenGeneratorPrivate::version = 0;
const char TokenGeneratorPrivate::tokenSeparator = '.';

TokenGeneratorPrivate::TokenGeneratorPrivate(const QByteArray &s)
    : secret(s)
{
}

QByteArray TokenGeneratorPrivate::computeToken(const QVariantMap &claims)
{
    QByteArray token;

    QByteArray encodedHeader = jsonEncode(commonHeader());
    QByteArray encodedClaims = jsonEncode(claims);
    QByteArray secureBits = encodedHeader + tokenSeparator + encodedClaims;
    QByteArray signedBits = sign(secureBits);

    token = secureBits + tokenSeparator + signedBits;
    return token;
}

QVariantMap TokenGeneratorPrivate::commonHeader()
{
    QVariantMap map;
    map["typ"] = "JWT";
    map["alg"] = "HS256";
    return map;
}

QByteArray TokenGeneratorPrivate::jsonEncode(const QVariantMap &map)
{
    QByteArray value;

    QJsonDocument doc = QJsonDocument::fromVariant(map);
    if (doc.isNull())
        return value;

    value = doc.toJson(QJsonDocument::Compact);
    return base64UrlEncode(value);
}

QByteArray TokenGeneratorPrivate::sign(const QByteArray &b)
{
    QMessageAuthenticationCode code(QCryptographicHash::Sha256, secret);
    code.addData(b);
    return base64UrlEncode(code.result());
}

QByteArray TokenGeneratorPrivate::base64UrlEncode(const QByteArray &b)
{
    return b.toBase64(QByteArray::Base64UrlEncoding | QByteArray::OmitTrailingEquals);
}

QVariantMap TokenGeneratorPrivate::jsonDecode(const QByteArray &b)
{
    QJsonDocument doc = QJsonDocument::fromJson(b);
    if (doc.isNull())
        return QVariantMap();

    return doc.toVariant().toMap();
}

QByteArray TokenGeneratorPrivate::base64UrlDecode(const QByteArray &b)
{
    return QByteArray::fromBase64(b, QByteArray::Base64UrlEncoding);
}

}
