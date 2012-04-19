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

#ifndef QECSOCKET_H
#define QECSOCKET_H

#include "ECCodes.h"

#include <QMultiHash>
#include <QTcpSocket>

class QECPacket;
class QECPacketSubscriber;
class QECSocket : public QTcpSocket
{
    Q_OBJECT
public:
    explicit QECSocket(QObject* parent = 0);
    virtual ~QECSocket();
public:
    void sendPacket(const QECPacket& p);
    void addSubscriber(ec_opcode_t opCode, QECPacketSubscriber* psb);
    void removeSubscriber(ec_opcode_t opCode, QECPacketSubscriber* psb);
private Q_SLOTS:
    void slotReadyRead();
private:
    typedef QMultiHash<ec_opcode_t, QECPacketSubscriber*> QECPacketSubscriberHash;
    QECPacketSubscriberHash m_subscribers;
};

#endif // QECSOCKET_H
