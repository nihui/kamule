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

#include "qecsocket.h"

#include <QDataStream>

#include "qecpacket.h"
#include "qecpacketsubscriber.h"

QECSocket::QECSocket(QObject* parent)
: QTcpSocket(parent)
{
    connect(this, SIGNAL(readyRead()), this, SLOT(slotReadyRead()));
}

QECSocket::~QECSocket()
{
}

void QECSocket::slotReadyRead()
{
    static quint32 flags = 0x20;
    static quint32 packetlen = 0;

    if (packetlen == 0 && bytesAvailable() >= 8) {
        QDataStream in(this);
        in >> flags;
        in >> packetlen;
    }

    if (packetlen > 0 && bytesAvailable() >= packetlen) {
        /// get complete packet
        QECPacket p;
        QDataStream in(this);
        in >> p;

        QECPacketSubscriberHash::iterator it = m_subscribers.find(p.opCode());
        while (it != m_subscribers.end() && it.key() == p.opCode()) {
            QECPacketSubscriber*& subsciber = it.value();
            subsciber->handlePacket(p);
            ++it;
        }

        packetlen = 0;
    }
}

void QECSocket::sendPacket(const QECPacket& p)
{
    static quint32 flags = 0x20;
    static quint32 packetlen = 0;

    QByteArray buffer;
    QDataStream s(&buffer, QIODevice::WriteOnly);
    s << p;
    packetlen = buffer.size();

    QDataStream out(this);
    out << flags;
    out << packetlen;
    out << p;
}

void QECSocket::addSubscriber(ec_opcode_t opCode, QECPacketSubscriber* psb)
{
    m_subscribers.insert(opCode, psb);
}

void QECSocket::removeSubscriber(ec_opcode_t opCode, QECPacketSubscriber* psb)
{
    m_subscribers.remove(opCode, psb);
}
