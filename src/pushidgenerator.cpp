
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
*
*
* Fancy ID generator that creates 20-character string identifiers with the following properties:
*
* 1. They're based on timestamp so that they sort *after* any existing ids.
* 2. They contain 72-bits of random data after the timestamp so that IDs won't collide with other clients' IDs.
* 3. They sort *lexicographically* (so the timestamp is converted to characters that will sort properly).
* 4. They're monotonically increasing. Even if you generate more than one in the same timestamp, 
* the latter ones will sort after the former ones. We do this by using the previous random bits but "incrementing" them by 1 
* (only in the case of a timestamp collision).
* 
* see: https://gist.github.com/mikelehen/3596a30bd69384624c11#file-generate-pushid-js
*/
#include "pushidgenerator.h"
#include "pushidgenerator_p.h"
#include <QDateTime>
#include <random>

namespace cg
{
    const QByteArray PushIdGeneratorPrivate::pushChars = "-0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ_abcdefghijklmnopqrstuvwxyz";

    PushIdGeneratorPrivate::PushIdGeneratorPrivate()
        : lastTimestamp(0L)
    {
        for (int i = 0; i < 12; i++)
            lastRandomChars[i] = 0;
    }

    char PushIdGeneratorPrivate::randomChar64()
    {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        static std::uniform_int_distribution<> dis(0, 63);
        return static_cast<char>(dis(gen));
    }

    PushIdGenerator::PushIdGenerator()
        : d_ptr(new PushIdGeneratorPrivate())
    {
    }

    QByteArray PushIdGenerator::generatePushId()
    {
        qint64 now = QDateTime::currentMSecsSinceEpoch();
        bool duplicateTime = (now == d_ptr->lastTimestamp) ? true : false;
        d_ptr->lastTimestamp = now;

        QByteArray id(20, '0');

        for (int i = 7; i >= 0; i--)
        {
            id[i] = d_ptr->pushChars[(int)(now % 64)];
            now = now >> 6;
        }

        if (now != 0)
            return QByteArray();

        if (!duplicateTime)
        {
            for (int i = 0; i < 12; i++)
                d_ptr->lastRandomChars[i] = d_ptr->randomChar64();
        }
        else
        {
            int i;
            for (i = 11; i >= 0 && d_ptr->lastRandomChars[i] == 63; i--)
                d_ptr->lastRandomChars[i] = 0;

            d_ptr->lastRandomChars[i]++;
        }

        for (int i = 0; i < 12; i++)
            id[i + 8] = d_ptr->pushChars[d_ptr->lastRandomChars[i]];

        return id;
    }

    qint64 PushIdGenerator::timestamp(const QByteArray &id) const
    {
        qint64 ts = 0L;
        for (int i = 0; i < 8; i++)
        {
            int index = d_ptr->pushChars.indexOf(id[i]);
            ts = (ts << 6) + index;
        }

        return ts;
    }
}