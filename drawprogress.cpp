/*
 *  This file is part of KaMule, aMule frontend for KDE
 *  Copyright (C) 2012 Ni Hui <shuizhuyuanluo@126.com>
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License as
 *  published by the Free Software Foundation; either version 2 of
 *  the License or (at your option) version 3 or any later version
 *  accepted by the membership of KDE e.V. (or its successor approved
 *  by the membership of KDE e.V.), which shall act as a proxy
 *  defined in Section 14 of version 3 of the license.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "drawprogress.h"

namespace QEC
{
    bool RLEDecode(const QByteArray& in, QByteArray& out);
    bool RLEDecode(const QByteArray& in, quint64*& ns, int& nslen);
}

bool QEC::RLEDecode(const QByteArray& in, QByteArray& out)
{
    int size = in.size();
    if (size < 3) {
        out = in;
        return true;
    }

    const char* data = in.constData();
    out.reserve(size);
    for (int i = 0; i < size;) {
        if (i < size - 2 && data[i] == data[i+1]) {
            /// run length
            /// NOTE no +2 here for amule RLE implementation
            quint8 len = data[i+2];
            while (len--)
                out.append(data[i]);
            i += 3;
        }
        else {
            /// single
            out.append(data[i]);
            ++i;
        }
    }
    return true;
}

bool QEC::RLEDecode(const QByteArray& in, quint64*& ns, int& nslen)
{
    QByteArray out;
    QEC::RLEDecode(in, out);

    nslen = out.size() / 8;
    ns = (quint64*)malloc(nslen * sizeof(quint64));

    quint64 n;
    for (int i=0; i < nslen; ++i) {
        n = 0;
        for (int j = 8; j--;) {
            n <<= 8;
            n |= out[ i + j * nslen ] & 0xff;
        }
        ns[i] = n;
    }

    return true;
}

QImage drawProgressBar(const QByteArray& part, const QByteArray& gap, const QByteArray& req, quint64 sizefull)
{
    static const quint64 partsize = 9728000;
    static const quint32 blocksize = 184320;
    static const quint32 ppb = partsize / blocksize;

    quint32 blocks = sizefull / blocksize;
    quint32 remain = sizefull % blocksize;
    if (remain != 0)
        blocks++;

    quint32* imageline = (quint32*)malloc(blocks * sizeof(quint32));
    memset(imageline, 0x00, blocks * sizeof(quint32));

    /// mark gap blocks with 0xff
    {
        quint64* ns;
        int nslen;
        QEC::RLEDecode(gap, ns, nslen);

        for (int i = 0; i < nslen; i += 2) {
            quint32 startb = ns[i] / blocksize;
            quint32 endb = qMin((quint32)(ns[i+1] / blocksize), blocks);
            if (startb < endb) {
                memset(imageline + startb, 0xff, (endb - startb) * sizeof(quint32));
            }
        }

        free(ns);
    }

    /// fill part with 0xffffffff & color
    {
        QByteArray out;
        QEC::RLEDecode(part, out);

        quint32 bs = 0;
        for (int i = 0; i < out.size(); ++i) {
            qint8 blue = qMax(0, 210 - (22 * (out[i] - 1)));
            quint32 color = qRgb(0, blue, 255);
            quint32 jend = qMin(ppb, blocks - bs);
            quint32* scanline = imageline + bs;
            for (quint32 j = 0; j < jend; ++j) {
                scanline[j] &= color;
            }
            bs += ppb;
        }
    }

    /// fill req blocks with color
    {
        quint64* ns;
        int nslen;
        QEC::RLEDecode(req, ns, nslen);

        for (int i = 0; i < nslen; i += 2) {
            quint32 startb = ns[i] / blocksize;
            quint32 endb = qMin((quint32)(ns[i+1] / blocksize), blocks);
            while (startb < endb) {
                imageline[startb] = qRgb(200, 200, 50);
                ++startb;
            }
        }

        free(ns);
    }

    QImage image(blocks, 1, QImage::Format_RGB32);
    uchar* scanline = image.scanLine(0);
    memcpy(scanline, imageline, blocks * sizeof(quint32));

    free(imageline);

    return image;
}


