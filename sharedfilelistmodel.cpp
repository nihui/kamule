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

#include "sharedfilelistmodel.h"

#include <QIcon>
#include <QPainter>
#include <QUrl>

#include <KIconLoader>
#include <kio/global.h>
#include <KLocalizedString>

#include "qecpacket.h"
#include "Constants.h"

#include "drawprogress.h"

#include "qecamule.h"

SharedFileInfo QECTag2SharedFileInfo(const QECTag& t, bool* ok)
{
    if (ok) *ok = (t.tagName() == EC_TAG_KNOWNFILE);
    SharedFileInfo sfi;
    sfi.ecid = t.toUInt32();
    foreach (const QECTag& subtag, t) {
        switch (subtag.tagName()) {
            case EC_TAG_PARTFILE_NAME:
                sfi.name = subtag.toString();
                break;
            case EC_TAG_PARTFILE_SIZE_FULL:
                sfi.sizefull = subtag.toUInt64();
                break;
            case EC_TAG_KNOWNFILE_PRIO:
                sfi.priority = subtag.toUInt8();
                break;
            case EC_TAG_PARTFILE_PART_STATUS:
                sfi.partstatus = subtag.toByteArray();
                break;
            case EC_TAG_PARTFILE_GAP_STATUS:
                sfi.gapstatus = subtag.toByteArray();
                break;
            case EC_TAG_PARTFILE_REQ_STATUS:
                sfi.reqstatus = subtag.toByteArray();
                break;
            case EC_TAG_PARTFILE_ED2K_LINK:
                sfi.ed2klink = subtag.toString();
                break;
            case EC_TAG_PARTFILE_HASH:
                sfi.hash = subtag.toByteArray();
                break;
            case EC_TAG_KNOWNFILE_XFERRED:
                sfi.xferred = subtag.toUInt64();
                break;
            case EC_TAG_KNOWNFILE_XFERRED_ALL:
                sfi.xferredall = subtag.toUInt64();
                break;
            case EC_TAG_KNOWNFILE_REQ_COUNT:
                sfi.reqcount = subtag.toUInt16();
                break;
            case EC_TAG_KNOWNFILE_REQ_COUNT_ALL:
                sfi.reqcountall = subtag.toUInt32();
                break;
            case EC_TAG_KNOWNFILE_ACCEPT_COUNT:
                sfi.acceptcount = subtag.toUInt16();
                break;
            case EC_TAG_KNOWNFILE_ACCEPT_COUNT_ALL:
                sfi.acceptcountall = subtag.toUInt32();
                break;
            case EC_TAG_KNOWNFILE_AICH_MASTERHASH:
                sfi.AICHmasterhash = subtag.toString();
                break;
            case EC_TAG_KNOWNFILE_FILENAME:
                sfi.filename = subtag.toString();
                break;
            case EC_TAG_KNOWNFILE_COMPLETE_SOURCES_LOW:
                sfi.completesourceslow = subtag.toUInt16();
                break;
            case EC_TAG_KNOWNFILE_COMPLETE_SOURCES_HIGH:
                sfi.completesourceshigh = subtag.toUInt16();
                break;
            case EC_TAG_KNOWNFILE_COMPLETE_SOURCES:
                sfi.completesources = subtag.toUInt16();
                break;
            case EC_TAG_KNOWNFILE_COMMENT:
                sfi.comment = subtag.toString();
                break;
            case EC_TAG_KNOWNFILE_RATING:
                sfi.rating = subtag.toUInt8();
                break;
            default:
//                 qWarning() << subtag;
                break;
        }
    }
    return sfi;
}

SharedFileListModel::SharedFileListModel()
{
    QECaMule::self()->addSubscriber(EC_OP_SHARED_FILES, this);
}

SharedFileListModel::~SharedFileListModel()
{
    QECaMule::self()->removeSubscriber(EC_OP_SHARED_FILES, this);
}

int SharedFileListModel::columnCount(const QModelIndex& /*parent*/) const
{
    return 6;
}

QVariant SharedFileListModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role == Qt::UserRole) {
        const SharedFileInfo& sfi = m_files.at(index.row());
        return QVariant::fromValue(&sfi);
    }

    if (role == Qt::UserRole + 1) {
        const SharedFileInfo& sfi = m_files.at(index.row());
        switch (index.column()) {
            case 0: return sfi.name;
            case 1: return sfi.sizefull;
            case 2: return sfi.priority >= 10 ? sfi.priority - 10 : sfi.priority;
            case 3: return sfi.reqcount;
            case 4: return sfi.acceptcount;
            case 5: return sfi.sizefull;
            default: return QVariant();
        }
    }

    if (role == Qt::DisplayRole) {
        const SharedFileInfo& sfi = m_files.at(index.row());
        switch (index.column()) {
            case 0: return sfi.name;
            case 1: return KIO::convertSize(sfi.sizefull);
            case 2: {
                quint8 priority = sfi.priority;
                bool autopriority = (priority >= 10);
                if (autopriority) {
                    priority -= 10;
                    switch (priority) {
                        case PR_VERYLOW: return i18n("Auto[Very Low]");
                        case PR_LOW: return i18n("Auto[Low]");
                        case PR_NORMAL: return i18n("Auto[Normal]");
                        case PR_HIGH: return i18n("Auto[High]");
                        case PR_VERYHIGH: return i18n("Auto[Very High]");
                        case PR_AUTO: return i18n("Auto[Auto]");
                        case PR_POWERSHARE: return i18n("Auto[Power Share]");
                        default: return priority;
                    }
                }
                switch (priority) {
                    case PR_VERYLOW: return i18n("Very Low");
                    case PR_LOW: return i18n("Low");
                    case PR_NORMAL: return i18n("Normal");
                    case PR_HIGH: return i18n("High");
                    case PR_VERYHIGH: return i18n("Very High");
                    case PR_AUTO: return i18n("Auto");
                    case PR_POWERSHARE: return i18n("Power Share");
                    default: return priority;
                }
            }
            case 3: return QString("%1 / %2").arg(sfi.reqcount).arg(sfi.reqcountall);
            case 4: return QString("%1 / %2").arg(sfi.acceptcount).arg(sfi.acceptcountall);
            case 5: return drawProgressBar(sfi.partstatus, sfi.gapstatus, sfi.reqstatus, sfi.sizefull);
            default: return QVariant();
        }
    }

    if (role == Qt::DecorationRole && index.column() == 0) {
        const SharedFileInfo& sfi = m_files.at(index.row());
        QString iconName = KIO::iconNameForUrl(QUrl::fromLocalFile(sfi.name));
        return KIconLoader::global()->loadMimeTypeIcon(iconName, KIconLoader::Small);
    }

    return QVariant();
}

QVariant SharedFileListModel::headerData(int section, Qt::Orientation /*orientation*/, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    switch (section) {
        case 0: return i18n("Filename");
        case 1: return i18n("Size");
        case 2: return i18n("Priority");
        case 3: return i18n("Require count");
        case 4: return i18n("Accept count");
        case 5: return i18n("Progress");
        default: return QVariant();
    }
}

QModelIndex SharedFileListModel::index(int row, int column, const QModelIndex& parent) const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    if (!parent.isValid()) {
        return createIndex(row, column);
    }

    return QModelIndex();
}

QModelIndex SharedFileListModel::parent(const QModelIndex& /*index*/) const
{
    return QModelIndex();
}

int SharedFileListModel::rowCount(const QModelIndex& parent) const
{
    if (!parent.isValid()) {
        return m_files.count();
    }

    return 0;
}

void SharedFileListModel::handlePacket(const QECPacket& p)
{
    QList<SharedFileInfo> files;
    foreach (const QECTag& tag, p) {
        bool ok;
        SharedFileInfo sfi = QECTag2SharedFileInfo(tag, &ok);
        if (ok) {
            files << sfi;
        }
    }

    QList<int> toremove;
    QList<SharedFileInfo> toadd;
    for (int i = 0; i < m_files.count(); ++i) {
        SharedFileInfo& m_sfi = m_files[i];
        /// find if exist in files, update it
        bool found = false;
        foreach (const SharedFileInfo& sfi, files) {
            if (m_sfi.hash == sfi.hash) {
                m_sfi = sfi;
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

    foreach (const SharedFileInfo& sfi, files) {
        /// find new ones
        bool found = false;
        foreach (const SharedFileInfo& m_sfi, m_files) {
            if (m_sfi.hash == sfi.hash) {
                found = true;
                break;
            }
        }
        /// mark to add it
        if (!found) {
            toadd.append(sfi);
        }
    }

    /// add new lines
    if (toadd.count() > 0) {
        beginInsertRows(QModelIndex(), m_files.count(), m_files.count() - 1 + toadd.count());
        m_files.append(toadd);
        endInsertRows();
    }

    /// remove lines
    if (toremove.count() > 0) {
        foreach (int i, toremove) {
            beginRemoveRows(QModelIndex(), i, i);
            m_files.removeAt(i);
            endRemoveRows();
        }
    }
}


SharedFileListDelegate::SharedFileListDelegate(QObject* parent)
: QStyledItemDelegate(parent)
{
}

void SharedFileListDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    if (index.column() != 5)
        return QStyledItemDelegate::paint(painter, option, index);

    painter->save();
    QImage image = index.data().value<QImage>();
    image = image.scaled(option.rect.size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    painter->drawImage(option.rect, image);
    painter->restore();

}
