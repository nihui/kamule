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

#include "downloadlistmodel.h"

#include <QImage>
#include <QPainter>

#include <KDebug>
#include <KIcon>
#include <KIconLoader>
#include <kio/global.h>
#include <KLocale>
#include <KMimeType>
#include <KUrl>

#include "qecpacket.h"

#include "drawprogress.h"

#include "qecamule.h"

#include "Constants.h"

PartfileInfo QECTag2PartfileInfo(const QECTag& t, bool* ok)
{
    if (ok) *ok = (t.tagName() == EC_TAG_PARTFILE);
    PartfileInfo pfi;
    foreach (const QECTag& subtag, t) {
        switch (subtag.tagName()) {
            case EC_TAG_PARTFILE_NAME:
                pfi.name = subtag.toString();
                break;
            case EC_TAG_PARTFILE_PARTMETID:
                pfi.metid = subtag.toUInt16();
                break;
            case EC_TAG_PARTFILE_SIZE_FULL:
                pfi.sizefull = subtag.toUInt64();
                break;
            case EC_TAG_PARTFILE_SIZE_XFER:
                pfi.sizexfer = subtag.toUInt64();
                break;
            case EC_TAG_PARTFILE_SIZE_XFER_UP:
                pfi.sizexferup = subtag.toUInt64();
                break;
            case EC_TAG_PARTFILE_SIZE_DONE:
                pfi.sizedone = subtag.toUInt64();
                break;
            case EC_TAG_PARTFILE_SPEED:
                pfi.speed = subtag.toUInt32();
                break;
            case EC_TAG_PARTFILE_STATUS:
                pfi.status = subtag.toUInt8();
                break;
            case EC_TAG_PARTFILE_PRIO:
                pfi.priority = subtag.toUInt8();
                break;
            case EC_TAG_PARTFILE_SOURCE_COUNT:
                pfi.sourcecount = subtag.toUInt16();
                break;
            case EC_TAG_PARTFILE_SOURCE_COUNT_A4AF:
                pfi.sourcecountA4AF = subtag.toUInt16();
                break;
            case EC_TAG_PARTFILE_SOURCE_COUNT_NOT_CURRENT:
                pfi.sourcecountnotcurrent = subtag.toUInt16();
                break;
            case EC_TAG_PARTFILE_SOURCE_COUNT_XFER:
                pfi.sourcecountxfer = subtag.toUInt16();
                break;
            case EC_TAG_PARTFILE_ED2K_LINK:
                pfi.ed2klink = subtag.toString();
                break;
            case EC_TAG_PARTFILE_CAT:
                pfi.cat = subtag.toUInt8();
                break;
            case EC_TAG_PARTFILE_LAST_RECV:
                pfi.lastrecv = subtag.toUInt32();
                break;
            case EC_TAG_PARTFILE_LAST_SEEN_COMP:
                pfi.lastseencomplete = subtag.toUInt32();
                break;
            case EC_TAG_PARTFILE_PART_STATUS:
                pfi.partstatus = subtag.toByteArray();
                break;
            case EC_TAG_PARTFILE_GAP_STATUS:
                pfi.gapstatus = subtag.toByteArray();
                break;
            case EC_TAG_PARTFILE_REQ_STATUS:
                pfi.reqstatus = subtag.toByteArray();
                break;
            case EC_TAG_PARTFILE_SOURCE_NAMES:
//                 kWarning() << "partfile source names" << subtag;
                break;
            case EC_TAG_PARTFILE_COMMENTS:
//                 kWarning() << "partfile comments" << subtag.toMd5().toHex();
                break;
            case EC_TAG_PARTFILE_STOPPED:
                pfi.stopped = subtag.toUInt8();
                break;
            case EC_TAG_PARTFILE_DOWNLOAD_ACTIVE:
                pfi.downloadactive = subtag.toUInt32();
                break;
            case EC_TAG_PARTFILE_LOST_CORRUPTION:
                pfi.lostcorruption = subtag.toUInt64();
                break;
            case EC_TAG_PARTFILE_GAINED_COMPRESSION:
                pfi.gainedcompression = subtag.toUInt64();
                break;
            case EC_TAG_PARTFILE_SAVED_ICH:
                pfi.savedICH = subtag.toUInt32();
                break;
            case EC_TAG_PARTFILE_SOURCE_NAMES_COUNTS:
//                 kWarning() << "partfile source names count" << subtag;
                break;
            case EC_TAG_PARTFILE_AVAILABLE_PARTS:
                pfi.availableparts = subtag.toUInt16();
                break;
            case EC_TAG_PARTFILE_HASH:
                pfi.hash = subtag.toMd5();
                break;
            case EC_TAG_PARTFILE_SHARED:
                pfi.shared = subtag.toUInt8();
                break;
            case EC_TAG_PARTFILE_HASHED_PART_COUNT:
//                 kWarning() << subtag;
//                             kWarning() << "partfile hashed part count" << subtag.toUInt8();
                break;
            case EC_TAG_PARTFILE_A4AFAUTO:
                pfi.A4AFauto = subtag.toUInt8();
                break;
            case EC_TAG_PARTFILE_A4AF_SOURCES:
                pfi.A4AFsources = subtag.toMd5().toHex();
                break;
            default:
//                 kWarning() << subtag;
                break;
        }
    }
    return pfi;
}


DownloadListModel::DownloadListModel()
{
    QECaMule::self()->addSubscriber(EC_OP_DLOAD_QUEUE, this);
}

DownloadListModel::~DownloadListModel()
{
    QECaMule::self()->removeSubscriber(EC_OP_DLOAD_QUEUE, this);
}

int DownloadListModel::columnCount(const QModelIndex& /*parent*/) const
{
    return 7;
}

QVariant DownloadListModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role == Qt::UserRole) {
        const PartfileInfo& pfi = m_files.at(index.row());
        return QVariant::fromValue(&pfi);
    }

    if (role == Qt::UserRole + 1) {
        const PartfileInfo& pfi = m_files.at(index.row());
        switch (index.column()) {
            case 0: return pfi.name;
            case 1: return pfi.sizefull;
            case 2: return pfi.sizedone;
            case 3: return pfi.speed;
            case 4: return pfi.priority >= 10 ? pfi.priority - 10 : pfi.priority;
            case 5: return pfi.status;
            case 6: return (double)pfi.sizedone / (double)pfi.sizefull;
        }
    }

    if (role == Qt::DisplayRole) {
        const PartfileInfo& pfi = m_files.at(index.row());
        switch (index.column()) {
            case 0: return pfi.name;
            case 1: return KIO::convertSize(pfi.sizefull);
            case 2: return KIO::convertSize(pfi.sizedone);
            case 3: return KIO::convertSize(pfi.speed) + "/s";
            case 4: {
                quint8 priority = pfi.priority;
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
            case 5: {
                switch (pfi.status) {
                    case PS_WAITINGFORHASH: return i18n("Waiting for hash");
                    case PS_HASHING: return i18n("Hashing");
                    case PS_ERROR: return i18n("Error");
                    case PS_PAUSED: return i18n("Paused");
                    case PS_COMPLETING: return i18n("Completing");
                    case PS_COMPLETE: return i18n("Complete");
                    case PS_ALLOCATING: return i18n("Allocating");
                    case PS_INSUFFICIENT:
                    case PS_UNKNOWN:
                    case PS_READY:
                    case PS_EMPTY:
                    default: return pfi.sourcecountxfer > 0 ? i18n("Downloading") : i18n("Waiting");
                }
            }
            case 6: return drawProgressBar(pfi.partstatus, pfi.gapstatus, pfi.reqstatus, pfi.sizefull);
            default: return QVariant();
        }
    }

    if (role == Qt::DecorationRole && index.column() == 0) {
        const PartfileInfo& pfi = m_files.at(index.row());
        QString iconName = KMimeType::iconNameForUrl(KUrl(pfi.name));
        return KIconLoader::global()->loadMimeTypeIcon(iconName, KIconLoader::Small);
    }

    return QVariant();
}

QVariant DownloadListModel::headerData(int section, Qt::Orientation /*orientation*/, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    switch (section) {
        case 0: return i18n("Name");
        case 1: return i18n("Size full");
        case 2: return i18n("Size done");
        case 3: return i18n("Speed");
        case 4: return i18n("Priority");
        case 5: return i18n("Status");
        case 6: return i18n("Progress");
        default: return QVariant();
    }
}

QModelIndex DownloadListModel::index(int row, int column, const QModelIndex& parent) const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    if (!parent.isValid()) {
        return createIndex(row, column);
    }

    return QModelIndex();
}

QModelIndex DownloadListModel::parent(const QModelIndex& /*index*/) const
{
    return QModelIndex();
}

int DownloadListModel::rowCount(const QModelIndex& parent) const
{
    if (!parent.isValid()) {
        return m_files.count();
    }

    return 0;
}

void DownloadListModel::handlePacket(const QECPacket& p)
{
    QList<PartfileInfo> files;
    foreach (const QECTag& tag, p) {
        bool ok;
        PartfileInfo pfi = QECTag2PartfileInfo(tag, &ok);
        if (ok) {
            files << pfi;
        }
    }

    QList<int> toremove;
    QList<PartfileInfo> toadd;
    for (int i = 0; i < m_files.count(); ++i) {
        PartfileInfo& m_pfi = m_files[i];
        /// find if exist in files, update it
        bool found = false;
        foreach (const PartfileInfo& pfi, files) {
            if (m_pfi.hash == pfi.hash) {
                m_pfi = pfi;
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

    foreach (const PartfileInfo& pfi, files) {
        /// find new ones
        bool found = false;
        foreach (const PartfileInfo& m_pfi, m_files) {
            if (m_pfi.hash == pfi.hash) {
                found = true;
                break;
            }
        }
        /// mark to add it
        if (!found) {
            toadd.append(pfi);
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


DownloadListDelegate::DownloadListDelegate(QObject* parent)
: QStyledItemDelegate(parent)
{
}

void DownloadListDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    if (index.column() != 6)
        return QStyledItemDelegate::paint(painter, option, index);

    painter->save();
    QImage image = index.data().value<QImage>();
    image = image.scaled(option.rect.size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    painter->drawImage(option.rect, image);
    painter->restore();
}
