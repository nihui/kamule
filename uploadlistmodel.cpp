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

#include "uploadlistmodel.h"

#include <KDebug>
#include <KIcon>
#include <kio/global.h>
#include <KLocale>

#include "qecpacket.h"

#include "qecamule.h"

ClientInfo QECTag2ClientInfo(const QECTag& t, bool* ok)
{
    if (ok) *ok = (t.tagName() == EC_TAG_CLIENT);
    ClientInfo ci;
    ci.ecid = t.toUInt32();
    foreach (const QECTag& subtag, t) {
        switch (subtag.tagName()) {
            case EC_TAG_CLIENT_NAME:
                ci.name = subtag.toString();
                break;
            case EC_TAG_PARTFILE_NAME:
                ci.filename = subtag.toString();
                break;
            case EC_TAG_PARTFILE_SIZE_XFER:
                ci.filesizexfer = subtag.toUInt64();
                break;
            case EC_TAG_CLIENT_SOFTWARE:
                ci.software = subtag.toUInt8();
                break;
            case EC_TAG_CLIENT_SCORE:
                ci.score = subtag.toUInt32();
                break;
            case EC_TAG_CLIENT_HASH:
                ci.userhash = subtag.toByteArray();
                break;
            case EC_TAG_CLIENT_FRIEND_SLOT:
                ci.friendslot = subtag.toBool();
                break;
            case EC_TAG_CLIENT_UPLOAD_SESSION:
                ci.uploadsession = subtag.toUInt64();
                break;
            case EC_TAG_CLIENT_UPLOAD_TOTAL:
                ci.uploadtotal = subtag.toUInt64();
                break;
            case EC_TAG_CLIENT_DOWNLOAD_TOTAL:
                ci.downloadtotal = subtag.toUInt64();
                break;
            case EC_TAG_CLIENT_DOWNLOAD_STATE:
                ci.downloadstate = subtag.toUInt8();
                break;
            case EC_TAG_CLIENT_UP_SPEED:
                ci.upspeed = subtag.toUInt32();
                break;
            case EC_TAG_CLIENT_DOWN_SPEED:
                ci.downspeed = subtag.toDouble();
                break;
            case EC_TAG_CLIENT_FROM:
                ci.from = subtag.toUInt8();
                break;
            case EC_TAG_CLIENT_USER_IP:
                ci.userip = subtag.toUInt32();
                break;
            case EC_TAG_CLIENT_USER_PORT:
                ci.userport = subtag.toUInt16();
                break;
            case EC_TAG_CLIENT_SERVER_IP:
                ci.serverip = subtag.toUInt32();
                break;
            case EC_TAG_CLIENT_SERVER_PORT:
                ci.serverport = subtag.toUInt16();
                break;
            case EC_TAG_CLIENT_SERVER_NAME:
                ci.servername = subtag.toString();
                break;
            case EC_TAG_CLIENT_SOFT_VER_STR:
                ci.softverstr = subtag.toString();
                break;
            case EC_TAG_CLIENT_WAITING_POSITION:
                ci.waitingposition = subtag.toUInt16();
                break;
            case EC_TAG_CLIENT_IDENT_STATE:
                ci.identstate = subtag.toUInt32();
                break;
            case EC_TAG_CLIENT_OBFUSCATION_STATUS:
                ci.obfuscationstatus = subtag.toUInt8();
                break;
            case EC_TAG_CLIENT_REMOTE_QUEUE_RANK:
                ci.remotequeuerank = subtag.toUInt16();
                break;
            case EC_TAG_CLIENT_UPLOAD_STATE:
                ci.uploadstate = subtag.toUInt8();
                break;
            case EC_TAG_CLIENT_EXT_PROTOCOL:
                ci.extprotocol = subtag.toBool();
                break;
            case EC_TAG_CLIENT_USER_ID:
                ci.userid = subtag.toUInt32();
                break;
            case EC_TAG_CLIENT_UPLOAD_FILE:
                ci.uploadfile = subtag.toUInt32();
                break;
            case EC_TAG_CLIENT_REQUEST_FILE:
                ci.requestfile = subtag.toUInt32();
                break;
            case EC_TAG_CLIENT_OLD_REMOTE_QUEUE_RANK:
                ci.oldremotequeuerank = subtag.toUInt16();
                break;
            case EC_TAG_CLIENT_KAD_PORT:
                ci.kadport = subtag.toUInt16();
                break;
            case EC_TAG_CLIENT_REMOTE_FILENAME:
                ci.remotefilename = subtag.toString();
                break;
            default:
//                 kWarning() << subtag;
                break;
        }
    }
    return ci;
}

UploadListModel::UploadListModel()
{
    QECaMule::self()->addSubscriber(EC_OP_ULOAD_QUEUE, this);
}

UploadListModel::~UploadListModel()
{
    QECaMule::self()->removeSubscriber(EC_OP_ULOAD_QUEUE, this);
}

int UploadListModel::columnCount(const QModelIndex& /*parent*/) const
{
    return 5;
}

QVariant UploadListModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role == Qt::UserRole) {
        const ClientInfo& ci = m_clients.at(index.row());
        return QVariant::fromValue(&ci);
    }

    if (role == Qt::UserRole + 1) {
        const ClientInfo& ci = m_clients.at(index.row());
        switch (index.column()) {
            case 0: return ci.name;
            case 1: return ci.filename;
            case 2: return ci.upspeed;
            case 3: return ci.downspeed;
            case 4: return ci.softverstr;
            default: return QVariant();
        }
    }

    if (role == Qt::DisplayRole) {
        const ClientInfo& ci = m_clients.at(index.row());
        switch (index.column()) {
            case 0: return ci.name;
            case 1: return ci.filename;
            case 2: return KIO::convertSize(ci.upspeed) + "/s";
            case 3: return QString::number(ci.downspeed) + "/s";
            case 4: return ci.softverstr;
            default: return QVariant();
        }
    }

    if (role == Qt::DecorationRole && index.column() == 0) {
        return KIcon("im-user");
    }

    return QVariant();
}

QVariant UploadListModel::headerData(int section, Qt::Orientation /*orientation*/, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    switch (section) {
        case 0: return i18n("Client");
        case 1: return i18n("Filename");
        case 2: return i18n("Up Speed");
        case 3: return i18n("Down Speed");
        case 4: return i18n("Software");
        default: return QVariant();
    }
}

QModelIndex UploadListModel::index(int row, int column, const QModelIndex& parent) const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    if (!parent.isValid()) {
        return createIndex(row, column);
    }

    return QModelIndex();
}

QModelIndex UploadListModel::parent(const QModelIndex& /*index*/) const
{
    return QModelIndex();
}

int UploadListModel::rowCount(const QModelIndex& parent) const
{
    if (!parent.isValid()) {
        return m_clients.count();
    }

    return 0;
}

void UploadListModel::handlePacket(const QECPacket& p)
{
    QList<ClientInfo> clients;
    foreach (const QECTag& tag, p) {
        bool ok;
        ClientInfo ci = QECTag2ClientInfo(tag, &ok);
        if (ok) {
            clients << ci;
        }
    }

    QList<int> toremove;
    QList<ClientInfo> toadd;
    for (int i = 0; i < m_clients.count(); ++i) {
        ClientInfo& m_ci = m_clients[i];
        /// find if exist in clients, update it
        bool found = false;
        foreach (const ClientInfo& ci, clients) {
            if (m_ci.userhash == ci.userhash) {
                m_ci = ci;
                emit dataChanged(index(i, 0, QModelIndex()), index(i, columnCount() - 1, QModelIndex()));
                found = true;
                break;
            }
        }
        /// mark to remove it, largest index is at the front
        if (!found) {
            toremove.prepend(i);
        }
    }

    foreach (const ClientInfo& ci, clients) {
        /// find new ones
        bool found = false;
        foreach (const ClientInfo& m_ci, m_clients) {
            if (m_ci.userhash == ci.userhash) {
                found = true;
                break;
            }
        }
        /// mark to add it
        if (!found) {
            toadd.append(ci);
        }
    }

    /// add new lines
    if (toadd.count() > 0) {
        beginInsertRows(QModelIndex(), m_clients.count(), m_clients.count() - 1 + toadd.count());
        m_clients.append(toadd);
        endInsertRows();
    }

    /// remove lines
    if (toremove.count() > 0) {
        foreach (int i, toremove) {
            beginRemoveRows(QModelIndex(), i, i);
            m_clients.removeAt(i);
            endRemoveRows();
        }
    }
}
