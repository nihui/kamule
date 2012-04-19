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

#include "qecamule.h"

QECaMule* QECaMule::m_self = 0;

QECaMule* QECaMule::self()
{
    if (!m_self)
        m_self = new QECaMule;
    return m_self;
}

QECaMule::QECaMule(QObject* parent)
: QECSocket(parent)
{
}

QECaMule::~QECaMule()
{
}

#if 0
void QECaMule::getConnectionState()
{
    QECPacket p(EC_OP_GET_CONNSTATE);
    p.addTag(EC_TAG_DETAIL_LEVEL, (uint64_t)EC_DETAIL_CMD);
    sendPacket(p);
}

void QECaMule::downloadSetPriority(const QByteArray& hash, int priority)
{
    QECPacket p(EC_OP_PARTFILE_PRIO_SET);
    QECTag tag(EC_TAG_PARTFILE, hash);
    tag.addSubTag(EC_TAG_PARTFILE_PRIO, (uint64_t)priority);
    p.addTag(tag);
    sendPacket(p);
}

#endif
#if 0

void QECaMule::recvPacket(const QECPacket& p)
{
//     qWarning() << "IN " << p;
switch (p.opCode()) {

    case EC_OP_MISC_DATA: {
        const QECTag& tag_connectionstate = p.findTag(EC_TAG_CONNSTATE);
        if (!tag_connectionstate.isNull()) {
//             qWarning() << "Connection state" << tag_connectionstate;
//             emit gotConnectionState();
        }
        break;
    }
}

}

#endif
