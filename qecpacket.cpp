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

#include "qecpacket.h"

static const QECTag QECTagNull;

static const ec_taglen_t tagheaderlen
    = sizeof(ec_tagname_t) + sizeof(ec_tagtype_t) + sizeof(ec_taglen_t);

QECTag::QECTag()
:tagtype(EC_TAGTYPE_UNKNOWN),
taglen(0),
subtagcount(0)
{
}

QECTag::QECTag(ec_tagname_t name)
:tagname(name),
tagtype(EC_TAGTYPE_UNKNOWN),
taglen(0),
subtagcount(0)
{
}

QECTag::QECTag(ec_tagname_t name, const QString& str)
:tagname(name),
tagtype(EC_TAGTYPE_UNKNOWN),
taglen(0),
subtagcount(0)
{
    setTagData(str);
}

QECTag::QECTag(ec_tagname_t name, const char* str)
:tagname(name),
tagtype(EC_TAGTYPE_UNKNOWN),
taglen(0),
subtagcount(0)
{
    setTagData(str);
}

QECTag::QECTag(ec_tagname_t name, bool value)
:tagname(name),
tagtype(EC_TAGTYPE_UNKNOWN),
taglen(0),
subtagcount(0)
{
    setTagData(value);
}

QECTag::QECTag(ec_tagname_t name, int value)
:tagname(name),
tagtype(EC_TAGTYPE_UNKNOWN),
taglen(0),
subtagcount(0)
{
    setTagData(value);
}

QECTag::QECTag(ec_tagname_t name, quint64 number)
:tagname(name),
tagtype(EC_TAGTYPE_UNKNOWN),
taglen(0),
subtagcount(0)
{
    setTagData(number);
}

QECTag::QECTag(ec_tagname_t name, double number)
:tagname(name),
tagtype(EC_TAGTYPE_UNKNOWN),
taglen(0),
subtagcount(0)
{
    setTagData(number);
}

QECTag::QECTag(ec_tagname_t name, quint32 ipv4, quint16 port)
:tagname(name),
tagtype(EC_TAGTYPE_UNKNOWN),
taglen(0),
subtagcount(0)
{
    setTagData(ipv4, port);
}

QECTag::QECTag(ec_tagname_t name, const char* data, int len)
:tagname(name),
tagtype(EC_TAGTYPE_UNKNOWN),
taglen(0),
subtagcount(0)
{
    setTagData(data, len);
}

QECTag::QECTag(ec_tagname_t name, const QByteArray& md5)
:tagname(name),
tagtype(EC_TAGTYPE_UNKNOWN),
taglen(0),
subtagcount(0)
{
    setTagData(md5);
}

QECTag::~QECTag()
{
}

ec_tagname_t QECTag::tagName() const
{
    return tagname;
}

ec_tagtype_t QECTag::tagType() const
{
    return tagtype;
}

void QECTag::setTagName(ec_tagname_t name)
{
    tagname = name;
}

void QECTag::setTagData(const QString& str)
{
    tagtype = EC_TAGTYPE_STRING;
    taglen -= tagdata.size();
    tagdata = str.toUtf8();
    tagdata.append('\0');
    taglen += tagdata.size();
}

void QECTag::setTagData(const char* str)
{
    tagtype = EC_TAGTYPE_STRING;
    taglen -= tagdata.size();
    tagdata = str;
    tagdata.append('\0');
    taglen += tagdata.size();
}

void QECTag::setTagData(bool value)
{
    setTagData((quint64)value);
}

void QECTag::setTagData(int value)
{
    setTagData((quint64)value);
}

void QECTag::setTagData(quint64 number)
{
    taglen -= tagdata.size();
    tagdata.clear();
    QDataStream stream(&tagdata, QIODevice::WriteOnly);
    if (number <= 0xff) {
        tagtype = EC_TAGTYPE_UINT8;
        stream << (quint8)number;
        taglen += 1;
    }
    else if (number <= 0xffff) {
        tagtype = EC_TAGTYPE_UINT16;
        stream << (quint16)number;
        taglen += 2;
    }
    else if (number <= 0xffffffff) {
        tagtype = EC_TAGTYPE_UINT32;
        stream << (quint32)number;
        taglen += 4;
    }
    else {
        tagtype = EC_TAGTYPE_UINT64;
        stream << number;
        taglen += 8;
    }
}

void QECTag::setTagData(double number)
{
    tagtype = EC_TAGTYPE_DOUBLE;
    taglen -= tagdata.size();
    tagdata = QByteArray::number(number);
    taglen += tagdata.size();
}

void QECTag::setTagData(quint32 ipv4, quint16 port)
{
    tagtype = EC_TAGTYPE_IPV4;
    taglen -= tagdata.size();
    tagdata.clear();
    QDataStream stream(&tagdata, QIODevice::WriteOnly);
    stream << ipv4;
    stream << port;
    taglen += 6;
}

void QECTag::setTagData(const char* data, int len)
{
    tagtype = EC_TAGTYPE_CUSTOM;
    taglen -= tagdata.size();
    tagdata = QByteArray::fromRawData(data, len);
    taglen += len;
}

void QECTag::setTagData(const QByteArray& md5)
{
    tagtype = EC_TAGTYPE_HASH16;
    taglen -= tagdata.size();
    tagdata = md5;
    taglen += 16;
}

void QECTag::addSubTag(const QECTag& subtag)
{
    if (subtag.subtagcount != 0) {
        taglen += sizeof(subtag.subtagcount);
    }
    taglen += tagheaderlen;
    taglen += subtag.taglen;
    ++subtagcount;
    subtags << subtag;
}

void QECTag::addSubTag(ec_tagname_t name)
{
    addSubTag(QECTag(name));
}

void QECTag::addSubTag(ec_tagname_t name, const QString& str)
{
    addSubTag(QECTag(name, str));
}

void QECTag::addSubTag(ec_tagname_t name, const char* str)
{
    addSubTag(QECTag(name, str));
}

void QECTag::addSubTag(ec_tagname_t name, bool value)
{
    addSubTag(QECTag(name, value));
}

void QECTag::addSubTag(ec_tagname_t name, int value)
{
    addSubTag(QECTag(name, value));
}

void QECTag::addSubTag(ec_tagname_t name, quint64 number)
{
    addSubTag(QECTag(name, number));
}

void QECTag::addSubTag(ec_tagname_t name, double number)
{
    addSubTag(QECTag(name, number));
}

void QECTag::addSubTag(ec_tagname_t name, quint32 ipv4, quint16 port)
{
    addSubTag(QECTag(name, ipv4, port));
}

void QECTag::addSubTag(ec_tagname_t name, const char* data, int len)
{
    addSubTag(QECTag(name, data, len));
}

void QECTag::addSubTag(ec_tagname_t name, const QByteArray& md5)
{
    addSubTag(QECTag(name, md5));
}

const QECTag& QECTag::findSubTag(ec_tagname_t name) const
{
    for (quint16 i = 0; i < subtagcount; ++i) {
        if (subtags.at(i).tagname == name) {
            return subtags.at(i);
        }
    }
    return QECTagNull;
}

bool QECTag::isNull() const
{
    return this == &QECTagNull;
}

QString QECTag::toString(bool* ok) const
{
    if (ok) *ok = (tagtype == EC_TAGTYPE_STRING);
    return QString::fromUtf8(tagdata.constData());
}

bool QECTag::toBool(bool* ok) const
{
    quint64 number = toUInt64(ok);
    return (number > 0);
}

quint8 QECTag::toUInt8(bool* ok) const
{
    if (ok) *ok = (tagtype == EC_TAGTYPE_UINT8);
    QDataStream stream(tagdata);
    quint8 number;
    stream >> number;
    return number;
}

quint16 QECTag::toUInt16(bool* ok) const
{
    quint16 number;
    switch (tagtype) {
        case EC_TAGTYPE_UINT8: {
            QDataStream stream(tagdata);
            quint8 n;
            stream >> n;
            number = n;
            break;
        }
        case EC_TAGTYPE_UINT16: {
            QDataStream stream(tagdata);
            stream >> number;
            break;
        }
        default: {
            if (ok) *ok = false;
            return 0;
        }
    }
    if (ok) *ok = true;
    return number;
}

quint32 QECTag::toUInt32(bool* ok) const
{
    quint32 number;
    switch (tagtype) {
        case EC_TAGTYPE_UINT8: {
            QDataStream stream(tagdata);
            quint8 n;
            stream >> n;
            number = n;
            break;
        }
        case EC_TAGTYPE_UINT16: {
            QDataStream stream(tagdata);
            quint16 n;
            stream >> n;
            number = n;
            break;
        }
        case EC_TAGTYPE_UINT32: {
            QDataStream stream(tagdata);
            stream >> number;
            break;
        }
        default: {
            if (ok) *ok = false;
            return 0;
        }
    }
    if (ok) *ok = true;
    return number;
}

quint64 QECTag::toUInt64(bool* ok) const
{
    quint64 number;
    switch (tagtype) {
        case EC_TAGTYPE_UINT8: {
            QDataStream stream(tagdata);
            quint8 n;
            stream >> n;
            number = n;
            break;
        }
        case EC_TAGTYPE_UINT16: {
            QDataStream stream(tagdata);
            quint16 n;
            stream >> n;
            number = n;
            break;
        }
        case EC_TAGTYPE_UINT32: {
            QDataStream stream(tagdata);
            quint32 n;
            stream >> n;
            number = n;
            break;
        }
        case EC_TAGTYPE_UINT64: {
            QDataStream stream(tagdata);
            stream >> number;
            break;
        }
        default: {
            if (ok) *ok = false;
            return 0;
        }
    }
    if (ok) *ok = true;
    return number;
}

double QECTag::toDouble(bool* ok) const
{
    if (ok) *ok = (tagtype == EC_TAGTYPE_DOUBLE);
    return tagdata.toDouble();
}

QByteArray QECTag::toMd5(bool* ok) const
{
    if (ok) *ok = (tagtype == EC_TAGTYPE_HASH16);
    return tagdata;
}

QByteArray QECTag::toByteArray(bool* ok) const
{
    if (ok) *ok = (tagtype == EC_TAGTYPE_CUSTOM);
    return tagdata;
}

QECPacket::QECPacket()
:tagcount(0)
{
}

QECPacket::QECPacket(ec_opcode_t code)
:opcode(code),
tagcount(0)
{
}

QECPacket::~QECPacket()
{
}

ec_opcode_t QECPacket::opCode() const
{
    return opcode;
}

void QECPacket::setOpCode(ec_opcode_t code)
{
    opcode = code;
}

void QECPacket::addTag(const QECTag& tag)
{
    ++tagcount;
    tags << tag;
}

void QECPacket::addTag(ec_tagname_t name)
{
    addTag(QECTag(name));
}

void QECPacket::addTag(ec_tagname_t name, const QString& str)
{
    addTag(QECTag(name, str));
}

void QECPacket::addTag(ec_tagname_t name, const char* str)
{
    addTag(QECTag(name, str));
}

void QECPacket::addTag(ec_tagname_t name, quint64 number)
{
    addTag(QECTag(name, number));
}

void QECPacket::addTag(ec_tagname_t name, bool value)
{
    addTag(QECTag(name, value));
}

void QECPacket::addTag(ec_tagname_t name, int value)
{
    addTag(QECTag(name, value));
}

void QECPacket::addTag(ec_tagname_t name, double number)
{
    addTag(QECTag(name, number));
}

void QECPacket::addTag(ec_tagname_t name, quint32 ipv4, quint16 port)
{
    addTag(QECTag(name, ipv4, port));
}

void QECPacket::addTag(ec_tagname_t name, const char* data, int len)
{
    addTag(QECTag(name, data, len));
}

void QECPacket::addTag(ec_tagname_t name, const QByteArray& md5)
{
    addTag(QECTag(name, md5));
}

const QECTag& QECPacket::findTag(ec_tagname_t name) const
{
    for (quint16 i = 0; i < tagcount; ++i) {
        if (tags.at(i).tagname == name) {
            return tags.at(i);
        }
    }
    return QECTagNull;
}


QDataStream& operator<<(QDataStream& stream, const QECTag& tag)
{
    bool has_subtag = (tag.subtagcount != 0);

    ec_tagname_t realtagname = (tag.tagname << 1) | (has_subtag ? 1 : 0);
    stream << realtagname;

    stream << tag.tagtype;
    stream << tag.taglen;

    if (has_subtag) {
        stream << tag.subtagcount;
        for (quint16 i = 0; i < tag.subtagcount; ++i) {
            stream << tag.subtags.at(i);
        }
    }

    stream.writeRawData(tag.tagdata.constData(), tag.tagdata.size());
    return stream;
}

QDataStream& operator>>(QDataStream& stream, QECTag& tag)
{
    ec_tagname_t realtagname;
    stream >> realtagname;
    tag.tagname = realtagname >> 1;
    bool has_subtag = (realtagname & 0x01);

    stream >> tag.tagtype;
    stream >> tag.taglen;

    ec_taglen_t tagdatalen = tag.taglen;

    if (has_subtag) {
        stream >> tag.subtagcount;
        tag.subtags.resize(tag.subtagcount);
        for (quint16 i = 0; i < tag.subtagcount; ++i) {
            stream >> tag.subtags[i];
            tagdatalen -= tag.subtags[i].taglen;
            if (tag.subtags[i].subtagcount != 0) {
                tagdatalen -= sizeof(tag.subtags[i].subtagcount);
            }
        }
        tagdatalen -= tagheaderlen * tag.subtagcount;
    }

    tag.tagdata.resize(tagdatalen);
    stream.readRawData(tag.tagdata.data(), tagdatalen);
    return stream;
}

QDataStream& operator<<(QDataStream& stream, const QECPacket& p)
{
    stream << p.opcode;
    stream << p.tagcount;
    for (quint16 i = 0; i < p.tagcount; ++i) {
        stream << p.tags.at(i);
    }
    return stream;
}

QDataStream& operator>>(QDataStream& stream, QECPacket& p)
{
    stream >> p.opcode;
    stream >> p.tagcount;
    if (p.tagcount > 0) {
        p.tags.resize(p.tagcount);
        for (quint16 i = 0; i < p.tagcount; ++i) {
            stream >> p.tags[i];
        }
    }
    return stream;
}


QDebug operator<<(QDebug dbg, const QECTag& tag)
{
    dbg.nospace() << "[";
    dbg.nospace() << QByteArray::number(tag.tagname, 16).constData();
    for (quint16 i = 0; i < tag.subtagcount; ++i) {
        dbg << tag.subtags.at(i);
    }
    switch (tag.tagtype) {
        case EC_TAGTYPE_STRING:
            dbg.nospace() << QString::fromUtf8(tag.tagdata);
            break;
        case EC_TAGTYPE_UINT8:
            dbg.nospace() << "\'u8=" << tag.tagdata.toHex().constData() << "\'";
            break;
        case EC_TAGTYPE_UINT16:
            dbg.nospace() << "\'u16=" << tag.tagdata.toHex().constData() << "\'";
            break;
        case EC_TAGTYPE_UINT32:
            dbg.nospace() << "\'u32=" << tag.tagdata.toHex().constData() << "\'";
            break;
        case EC_TAGTYPE_UINT64:
            dbg.nospace() << "\'u64=" << tag.tagdata.toHex().constData() << "\'";
            break;
        case EC_TAGTYPE_DOUBLE:
            dbg.nospace() << "\'d=" << QString::fromUtf8(tag.tagdata) << "\'";
            break;
        case EC_TAGTYPE_CUSTOM:
        default:
            dbg.nospace() << "#" << tag.tagdata.toHex().constData() << "#";
            break;
    }
    dbg.nospace() << "]";
    return dbg.space();
}

QDebug operator<<(QDebug dbg, const QECPacket& p)
{
    dbg.nospace() << "{";
    dbg.nospace() << QByteArray::number(p.opcode, 16).constData();
    for (quint16 i = 0; i < p.tagcount; ++i) {
        dbg << p.tags.at(i);
    }
    dbg.nospace() << "}";
    return dbg.space();
}
