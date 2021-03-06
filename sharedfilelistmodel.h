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

#ifndef SHAREDFILELISTMODEL_H
#define SHAREDFILELISTMODEL_H

#include <QAbstractItemModel>
#include <QStyledItemDelegate>
#include "qecpacketsubscriber.h"

class SharedFileInfo
{
public:
    QString name;
    quint64 sizefull;
    quint8 priority;
    QByteArray partstatus;
    QByteArray gapstatus;
    QByteArray reqstatus;
    QString ed2klink;
    QByteArray hash;
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

    quint32 ecid;
};

Q_DECLARE_METATYPE(const SharedFileInfo*)

class SharedFileListModel : public QAbstractItemModel, public QECPacketSubscriber
{
public:
    explicit SharedFileListModel();
    virtual ~SharedFileListModel();
    virtual int columnCount(const QModelIndex& parent = QModelIndex()) const;
    virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    virtual QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const;
    virtual QModelIndex parent(const QModelIndex& index) const;
    virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;

    virtual void handlePacket(const QECPacket& p);
private:
    QList<SharedFileInfo> m_files;
};

class SharedFileListDelegate : public QStyledItemDelegate
{
public:
    explicit SharedFileListDelegate(QObject* parent = 0);
    virtual void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;
};

#endif // SHAREDFILELISTMODEL_H
