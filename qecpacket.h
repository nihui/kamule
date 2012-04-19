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

#ifndef QECPACKET_H
#define QECPACKET_H

#include "ECCodes.h"
#include "ECTagTypes.h"

#include <QDataStream>
#include <QDebug>
#include <QString>
#include <QVector>

class QECTag
{
public:
    explicit QECTag();
    explicit QECTag(ec_tagname_t name);
    explicit QECTag(ec_tagname_t name, const QString& str);
    explicit QECTag(ec_tagname_t name, const char* str);
    explicit QECTag(ec_tagname_t name, bool value);
    explicit QECTag(ec_tagname_t name, int value);
    explicit QECTag(ec_tagname_t name, quint64 number);
    explicit QECTag(ec_tagname_t name, double number);
    explicit QECTag(ec_tagname_t name, quint32 ipv4, quint16 port);
    explicit QECTag(ec_tagname_t name, const char* data, int len);
    explicit QECTag(ec_tagname_t name, const QByteArray& md5);
    ~QECTag();
    ec_tagname_t tagName() const;
    ec_tagtype_t tagType() const;
    void setTagName(ec_tagname_t name);
    void setTagData(const QString& str);
    void setTagData(const char* str);
    void setTagData(bool value);
    void setTagData(int value);
    void setTagData(quint64 number);
    void setTagData(double number);
    void setTagData(quint32 ipv4, quint16 port);
    void setTagData(const char* data, int len);
    void setTagData(const QByteArray& md5);
    void addSubTag(const QECTag& subtag);
    void addSubTag(ec_tagname_t name);
    void addSubTag(ec_tagname_t name, const QString& str);
    void addSubTag(ec_tagname_t name, const char* str);
    void addSubTag(ec_tagname_t name, bool value);
    void addSubTag(ec_tagname_t name, int value);
    void addSubTag(ec_tagname_t name, quint64 number);
    void addSubTag(ec_tagname_t name, double number);
    void addSubTag(ec_tagname_t name, quint32 ipv4, quint16 port);
    void addSubTag(ec_tagname_t name, const char* data, int len);
    void addSubTag(ec_tagname_t name, const QByteArray& md5);
    const QECTag& findSubTag(ec_tagname_t name) const;

    bool isNull() const;

    QString toString(bool* ok = 0) const;
    bool toBool(bool* ok = 0) const;
    quint8 toUInt8(bool* ok = 0) const;
    quint16 toUInt16(bool* ok = 0) const;
    quint32 toUInt32(bool* ok = 0) const;
    quint64 toUInt64(bool* ok = 0) const;
    double toDouble(bool* ok = 0) const;
    QByteArray toMd5(bool* ok = 0) const;
    QByteArray toByteArray(bool* ok = 0) const;

    typedef QVector<QECTag>::const_iterator const_iterator;
    const_iterator begin() const { return subtags.constBegin(); }
    const_iterator end() const { return subtags.constEnd(); }

    friend QDataStream& operator<<(QDataStream& stream, const QECTag& tag);
    friend QDataStream& operator>>(QDataStream& stream, QECTag& tag);
    friend QDebug operator<<(QDebug dbg, const QECTag& tag);
private:
    friend class QECPacket;
    ec_tagname_t tagname;
    ec_tagtype_t tagtype;
    ec_taglen_t taglen;

    quint16 subtagcount;
    QVector<QECTag> subtags;

    QByteArray tagdata;
};

class QECPacket
{
public:
    explicit QECPacket();
    explicit QECPacket(ec_opcode_t code);
    ~QECPacket();
    ec_opcode_t opCode() const;
    void setOpCode(ec_opcode_t code);
    void addTag(const QECTag& tag);
    void addTag(ec_tagname_t name);
    void addTag(ec_tagname_t name, const QString& str);
    void addTag(ec_tagname_t name, const char* str);
    void addTag(ec_tagname_t name, bool value);
    void addTag(ec_tagname_t name, int value);
    void addTag(ec_tagname_t name, quint64 number);
    void addTag(ec_tagname_t name, double number);
    void addTag(ec_tagname_t name, quint32 ipv4, quint16 port);
    void addTag(ec_tagname_t name, const char* data, int len);
    void addTag(ec_tagname_t name, const QByteArray& md5);
    const QECTag& findTag(ec_tagname_t name) const;

    typedef QVector<QECTag>::const_iterator const_iterator;
    const_iterator begin() const { return tags.constBegin(); }
    const_iterator end() const { return tags.constEnd(); }

    friend QDataStream& operator<<(QDataStream& stream, const QECPacket& p);
    friend QDataStream& operator>>(QDataStream& stream, QECPacket& p);
    friend QDebug operator<<(QDebug dbg, const QECPacket& p);
private:
    ec_opcode_t opcode;
    quint16 tagcount;
    QVector<QECTag> tags;
};

QDataStream& operator<<(QDataStream& stream, const QECTag& tag);
QDataStream& operator>>(QDataStream& stream, QECTag& tag);

QDataStream& operator<<(QDataStream& stream, const QECPacket& p);
QDataStream& operator>>(QDataStream& stream, QECPacket& p);

QDebug operator<<(QDebug dbg, const QECTag& tag);
QDebug operator<<(QDebug dbg, const QECPacket& p);

#endif // QECPACKET_H
