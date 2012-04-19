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

#include "mystatusbar.h"

#include <kio/global.h>
#include <KLocale>

#include "qecpacket.h"

#include "qecamule.h"

class StatInfo
{
public:
    quint8 stat;
    quint32 upspeed;
    quint32 downspeed;
    quint64 ed2kusers;
    quint64 kadusers;
    quint64 ed2kfiles;
    quint64 kadfiles;

    QString servername;
};

StatInfo QECPacket2StatInfo(const QECPacket& p, bool* ok)
{
    StatInfo si;
    foreach (const QECTag& subtag, p) {
        switch (subtag.tagName()) {
            case EC_TAG_CONNSTATE: {
                si.stat = subtag.toUInt8();
//                 qWarning() << "ED2K connected" << ((stat & 0x01) != 0);
//                 qWarning() << "ED2K connecting" << ((stat & 0x02) != 0);
//                 qWarning() << "KAD connected" << ((stat & 0x04) != 0);
//                 qWarning() << "KAD firewalled" << ((stat & 0x08) != 0);
//                 qWarning() << "KAD running" << ((stat & 0x10) != 0);
                const QECTag& tag_server = subtag.findSubTag(EC_TAG_SERVER);
                if (!tag_server.isNull()) {
                    const QECTag& tag_servername = tag_server.findSubTag(EC_TAG_SERVER_NAME);
                    if (!tag_servername.isNull()) {
                        si.servername = tag_servername.toString();
                    }
//                     qWarning() << "Server Info" << tag_server;
                }
                break;
            }
            case EC_TAG_STATS_UL_SPEED:
                si.upspeed = subtag.toUInt32();
                break;
            case EC_TAG_STATS_DL_SPEED:
                si.downspeed = subtag.toUInt32();
                break;
            case EC_TAG_STATS_ED2K_USERS:
                si.ed2kusers = subtag.toUInt64();
                break;
            case EC_TAG_STATS_KAD_USERS:
                si.kadusers = subtag.toUInt64();
                break;
            case EC_TAG_STATS_ED2K_FILES:
                si.ed2kfiles = subtag.toUInt64();
                break;
            case EC_TAG_STATS_KAD_FILES:
                si.kadfiles = subtag.toUInt64();
                break;
            default:
//                 qWarning() << subtag;
                break;
        }
    }
    return si;
}

MyStatusBar::MyStatusBar(QWidget* parent)
: KStatusBar(parent)
{
    insertItem(i18n("Users: E %1 K %2", 0, 0), 0);
    insertItem(i18n("Files: E %1 K %2", 0, 0), 1);
    insertItem(i18n("Up: %1", 0), 2);
    insertItem(i18n("Down: %1", 0), 3);
    insertItem(i18n("ED2K: %1", i18n("Disconnected")), 4);
    insertItem(i18n("KAD: %1", i18n("Disconnected")), 5);

    QECaMule::self()->addSubscriber(EC_OP_STATS, this);
}

MyStatusBar::~MyStatusBar()
{
    QECaMule::self()->removeSubscriber(EC_OP_STATS, this);
}

void MyStatusBar::handlePacket(const QECPacket& p)
{
    bool ok;
    StatInfo si = QECPacket2StatInfo(p, &ok);

    changeItem(i18n("Users: E %1 K %2", KIO::convertSize(si.ed2kusers), KIO::convertSize(si.kadusers)), 0);
    changeItem(i18n("Files: E %1 K %2", KIO::convertSize(si.ed2kfiles), KIO::convertSize(si.kadfiles)), 1);
    changeItem(i18n("Up: %1", KIO::convertSize(si.upspeed) + "/s"), 2);
    changeItem(i18n("Down: %1", KIO::convertSize(si.downspeed) + "/s"), 3);
    quint8 stat = si.stat;
    if (stat & 0x01) {
        changeItem(i18n("ED2K: %1", si.servername), 4);
    }
    if (stat & 0x02) {
        changeItem(i18n("ED2K: %1", i18n("Connecting")), 4);
    }
    if (stat & 0x04) {
        changeItem(i18n("KAD: %1", i18n("Disconnected")), 5);
    }
    if (stat & 0x08) {
        changeItem(i18n("KAD: %1", i18n("Firewalled")), 5);
    }
    if (stat & 0x10) {
        changeItem(i18n("KAD: %1", i18n("Open")), 5);
    }
}
