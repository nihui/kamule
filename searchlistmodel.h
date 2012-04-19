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

#ifndef SEARCHLISTMODEL_H
#define SEARCHLISTMODEL_H

#include <QAbstractItemModel>
#include "qecpacketsubscriber.h"

class SearchFileInfo
{
public:
    QString name;
    quint64 sizefull;
    quint8 status;
    quint16 sourcecount;
    quint16 sourcecountxfer;
    QByteArray hash;
    quint32 ecid;
};

Q_DECLARE_METATYPE(const SearchFileInfo*)

class SearchListModel : public QAbstractItemModel, public QECPacketSubscriber
{
    Q_OBJECT
public:
    explicit SearchListModel();
    virtual ~SearchListModel();
    virtual int columnCount(const QModelIndex& parent = QModelIndex()) const;
    virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    virtual QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const;
    virtual QModelIndex parent(const QModelIndex& index) const;
    virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;

    virtual void handlePacket(const QECPacket& p);
private:
    QList<SearchFileInfo> m_files;
};

#endif // SEARCHLISTMODEL_H
