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

#include "serverlistmodel.h"

#include <QHostAddress>
#include <QIcon>

#include <kio/global.h>
#include <KLocalizedString>

#include "qecpacket.h"

#include "qecamule.h"

static ServerInfo QECTag2ServerInfo(const QECTag& t, bool* ok)
{
    if (ok) *ok = (t.tagName() == EC_TAG_SERVER);
    ServerInfo si;
    QByteArray ipport = t.toByteArray();
    QDataStream ds(ipport);
    ds >> si.ip;
    ds >> si.port;
    foreach (const QECTag& subtag, t) {
        switch (subtag.tagName()) {
            case EC_TAG_SERVER_NAME:
                si.name = subtag.toString();
                break;
            case EC_TAG_SERVER_DESC:
                si.description = subtag.toString();
                break;
            case EC_TAG_SERVER_ADDRESS:
                si.address = subtag.toString();
                break;
            case EC_TAG_SERVER_PING:
                si.ping = subtag.toUInt32();
                break;
            case EC_TAG_SERVER_USERS:
                si.users = subtag.toUInt32();
                break;
            case EC_TAG_SERVER_USERS_MAX:
                si.usersmax = subtag.toUInt32();
                break;
            case EC_TAG_SERVER_FILES:
                si.files = subtag.toUInt32();
                break;
            case EC_TAG_SERVER_PRIO:
                si.priority = subtag.toUInt16();
                break;
            case EC_TAG_SERVER_FAILED:
                si.failed = subtag.toUInt16();
                break;
            case EC_TAG_SERVER_STATIC:
                si.statical = subtag.toBool();
                break;
            case EC_TAG_SERVER_VERSION:
                si.version = subtag.toString();
                break;
            default:
//                 qWarning() << subtag;
                break;
        }
    }
    return si;
}


ServerListModel::ServerListModel()
{
    QECaMule::self()->addSubscriber(EC_OP_SERVER_LIST, this);
}

ServerListModel::~ServerListModel()
{
    QECaMule::self()->removeSubscriber(EC_OP_SERVER_LIST, this);
}

int ServerListModel::columnCount(const QModelIndex& /*parent*/) const
{
    return 11;
}

QVariant ServerListModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role == Qt::UserRole) {
        const ServerInfo& si = m_servers.at(index.row());
        return QVariant::fromValue(&si);
    }

    if (role == Qt::UserRole + 1) {
        const ServerInfo& si = m_servers.at(index.row());
        switch (index.column()) {
            case 0: return si.name;
            case 1: return si.description;
            case 2: return si.ping;
            case 3: return si.users;
            case 4: return si.usersmax;
            case 5: return si.files;
            case 6: return si.priority;
            case 7: return si.failed;
            case 8: return si.statical;
            case 9: return si.version;
            case 10: return QString("%1:%2").arg(QHostAddress(si.ip).toString()).arg(si.port);
            default: return QVariant();
        }
    }

    if (role == Qt::DisplayRole) {
        const ServerInfo& si = m_servers.at(index.row());
        switch (index.column()) {
            case 0: return si.name;
            case 1: return si.description;
            case 2: return si.ping;
            case 3: return KIO::convertSize(si.users);
            case 4: return KIO::convertSize(si.usersmax);
            case 5: return KIO::convertSize(si.files);
            case 6: return si.priority;
            case 7: return si.failed;
            case 8: return si.statical ? i18n("Yes") : i18n("No");
            case 9: return si.version;
            case 10: return QString("%1:%2").arg(QHostAddress(si.ip).toString()).arg(si.port);
            default: return QVariant();
        }
    }

    if (role == Qt::DecorationRole) {
        if (index.column() == 0) {
            return QIcon::fromTheme("network-server");
        }
    }

    return QVariant();
}

QVariant ServerListModel::headerData(int section, Qt::Orientation /*orientation*/, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    switch (section) {
        case 0: return i18n("Name");
        case 1: return i18n("Description");
        case 2: return i18n("Ping");
        case 3: return i18n("Users");
        case 4: return i18n("UsersMax");
        case 5: return i18n("Files");
        case 6: return i18n("Priority");
        case 7: return i18n("Failed");
        case 8: return i18n("Static");
        case 9: return i18n("Version");
        case 10: return i18n("IP:Port");
        default: return QVariant();
    }
}

QModelIndex ServerListModel::index(int row, int column, const QModelIndex& parent) const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    if (!parent.isValid()) {
        return createIndex(row, column);
    }

    return QModelIndex();
}

QModelIndex ServerListModel::parent(const QModelIndex& /*index*/) const
{
    return QModelIndex();
}

int ServerListModel::rowCount(const QModelIndex& parent) const
{
    if (!parent.isValid()) {
        return m_servers.count();
    }

    return 0;
}

void ServerListModel::handlePacket(const QECPacket& p)
{
    QList<ServerInfo> servers;
    foreach (const QECTag& tag, p) {
        bool ok;
        ServerInfo si = QECTag2ServerInfo(tag, &ok);
        if (ok) {
            servers << si;
        }
    }

    QList<int> toremove;
    QList<ServerInfo> toadd;
    for (int i = 0; i < m_servers.count(); ++i) {
        ServerInfo& m_si = m_servers[i];
        /// find if exist in servers, update it
        bool found = false;
        foreach (const ServerInfo& si, servers) {
            if (m_si.ip == si.ip && m_si.port == si.port) {
                m_si = si;
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

    foreach (const ServerInfo& si, servers) {
        /// find new ones
        bool found = false;
        foreach (const ServerInfo& m_si, m_servers) {
            if (m_si.ip == si.ip && m_si.port == si.port) {
                found = true;
                break;
            }
        }
        /// mark to add it
        if (!found) {
            toadd.append(si);
        }
    }

    /// add new lines
    if (toadd.count() > 0) {
        beginInsertRows(QModelIndex(), m_servers.count(), m_servers.count() - 1 + toadd.count());
        m_servers.append(toadd);
        endInsertRows();
    }

    /// remove lines
    if (toremove.count() > 0) {
        foreach (int i, toremove) {
            beginRemoveRows(QModelIndex(), i, i);
            m_servers.removeAt(i);
            endRemoveRows();
        }
    }
}

