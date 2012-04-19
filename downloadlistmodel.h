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

#ifndef DOWNLOADLISTMODEL_H
#define DOWNLOADLISTMODEL_H

#include <QAbstractItemModel>
#include <QStyledItemDelegate>
#include "qecpacketsubscriber.h"

class PartfileInfo
{
public:
    QString name;
    quint16 metid;
    quint64 sizefull;
    quint64 sizexfer;
    quint64 sizexferup;
    quint64 sizedone;
    quint32 speed;
    quint8 status;
    quint8 priority;
    quint16 sourcecount;
    quint16 sourcecountA4AF;
    quint16 sourcecountnotcurrent;
    quint16 sourcecountxfer;
    QString ed2klink;
    quint8 cat;
    quint32 lastrecv;
    quint32 lastseencomplete;
    QByteArray partstatus;
    QByteArray gapstatus;
    QByteArray reqstatus;
    QStringList sourcenames;
    QStringList comments;
    quint8 stopped;
    quint32 downloadactive;
    quint64 lostcorruption;
    quint64 gainedcompression;
    quint32 savedICH;
    QList<quint32> sourcenamecounts;
    quint16 availableparts;
    QByteArray hash;
    quint8 shared;
    quint8 A4AFauto;
    QByteArray A4AFsources;


    quint64 xferred;
    quint64 xferredall;
    quint16 reqcount;
    quint32 reqcountall;
    quint16 acceptcount;
    quint32 acceptcountall;
    QString AICHmasterhash;
    QString filename;
    quint16 completesourceslow;
    quint16 completesourceshigh;

    quint16 completesources;
    QString comment;
    quint8 rating;
};

Q_DECLARE_METATYPE(const PartfileInfo*)

class DownloadListModel : public QAbstractItemModel, public QECPacketSubscriber
{
public:
    explicit DownloadListModel();
    virtual ~DownloadListModel();
    virtual int columnCount(const QModelIndex& parent = QModelIndex()) const;
    virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    virtual QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const;
    virtual QModelIndex parent(const QModelIndex& index) const;
    virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;

    virtual void handlePacket(const QECPacket& p);
private:
    QList<PartfileInfo> m_files;
};

class DownloadListDelegate : public QStyledItemDelegate
{
public:
    explicit DownloadListDelegate(QObject* parent = 0);
    virtual void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;
};

#endif // DOWNLOADLISTMODEL_H
