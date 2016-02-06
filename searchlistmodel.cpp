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

#include "searchlistmodel.h"

#include <QUrl>
#include <QPixmap>

#include <KIconLoader>
#include <kio/global.h>
#include <KLocalizedString>

#include "qecpacket.h"

#include "qecamule.h"

SearchFileInfo QECTag2SearchFileInfo(const QECTag& t, bool* ok)
{
    if (ok) *ok = (t.tagName() == EC_TAG_SEARCHFILE);
    SearchFileInfo sfi;
    sfi.ecid = t.toUInt32();
    foreach (const QECTag& subtag, t) {
        switch (subtag.tagName()) {
            case EC_TAG_PARTFILE_NAME:
                sfi.name = subtag.toString();
                break;
            case EC_TAG_PARTFILE_SIZE_FULL:
                sfi.sizefull = subtag.toUInt64();
                break;
            case EC_TAG_PARTFILE_STATUS:
                sfi.status = subtag.toUInt8();
                break;
            case EC_TAG_PARTFILE_SOURCE_COUNT:
                sfi.sourcecount = subtag.toUInt16();
                break;
            case EC_TAG_PARTFILE_SOURCE_COUNT_XFER:
                sfi.sourcecountxfer = subtag.toUInt16();
                break;
            case EC_TAG_PARTFILE_HASH:
                sfi.hash = subtag.toMd5();
                break;
            default:
//                 qWarning() << subtag;
                break;
        }
    }
    return sfi;
}

SearchListModel::SearchListModel()
{
    QECaMule::self()->addSubscriber(EC_OP_SEARCH_RESULTS, this);
}

SearchListModel::~SearchListModel()
{
    QECaMule::self()->removeSubscriber(EC_OP_SEARCH_RESULTS, this);
}

int SearchListModel::columnCount(const QModelIndex& /*parent*/) const
{
    return 4;
}

QVariant SearchListModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role == Qt::UserRole) {
        const SearchFileInfo& sfi = m_files.at(index.row());
        return QVariant::fromValue(&sfi);
    }

    if (role == Qt::UserRole + 1) {
        const SearchFileInfo& sfi = m_files.at(index.row());
        switch (index.column()) {
            case 0: return sfi.name;
            case 1: return sfi.sizefull;
            case 2: return sfi.status;
            case 3: return sfi.sourcecount;
            default: return QVariant();
        }
    }

    if (role == Qt::DisplayRole) {
        const SearchFileInfo& sfi = m_files.at(index.row());
        switch (index.column()) {
            case 0: return sfi.name;
            case 1: return KIO::convertSize(sfi.sizefull);
            case 2: return sfi.status;
            case 3: return QString("%1 (%2)").arg(sfi.sourcecount).arg(sfi.sourcecountxfer);
            default: return QVariant();
        }
    }

    if (role == Qt::DecorationRole && index.column() == 0) {
        const SearchFileInfo& sfi = m_files.at(index.row());
        QString iconName = KIO::iconNameForUrl(QUrl::fromLocalFile(sfi.name));
        return KIconLoader::global()->loadMimeTypeIcon(iconName, KIconLoader::Small);
    }

    return QVariant();
}

QVariant SearchListModel::headerData(int section, Qt::Orientation /*orientation*/, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    switch (section) {
        case 0: return i18n("Filename");
        case 1: return i18n("Size");
        case 2: return i18n("Status");
        case 3: return i18n("Source Count (XFER)");
        default: return QVariant();
    }
}

QModelIndex SearchListModel::index(int row, int column, const QModelIndex& parent) const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    if (!parent.isValid()) {
        return createIndex(row, column);
    }

    return QModelIndex();
}

QModelIndex SearchListModel::parent(const QModelIndex& /*index*/) const
{
    return QModelIndex();
}

int SearchListModel::rowCount(const QModelIndex& parent) const
{
    if (!parent.isValid()) {
        return m_files.count();
    }

    return 0;
}

void SearchListModel::handlePacket(const QECPacket& p)
{
    QList<SearchFileInfo> files;
    foreach (const QECTag& tag, p) {
        bool ok;
        SearchFileInfo sfi = QECTag2SearchFileInfo(tag, &ok);
        if (ok) {
            files << sfi;
        }
    }

    beginResetModel();
    m_files.clear();
    endResetModel();

    beginInsertRows(QModelIndex(), 0, files.count() - 1);
    m_files = files;
    endInsertRows();
}
