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

#include "connectionpage.h"

#include "qecpacket.h"

#include "qecamule.h"

ConnectionPageWidget::ConnectionPageWidget(QWidget* parent)
: PreferenceWidget(parent)
{
    setupUi(this);

    connect(m_downloadMax, SIGNAL(valueChanged(int)), this, SLOT(slotDownloadMaxChanged(int)));
    connect(m_uploadMax, SIGNAL(valueChanged(int)), this, SLOT(slotUploadMaxChanged(int)));

    QECaMule::self()->addSubscriber(EC_OP_SET_PREFERENCES, this);
}

ConnectionPageWidget::~ConnectionPageWidget()
{
    QECaMule::self()->removeSubscriber(EC_OP_SET_PREFERENCES, this);
}

void ConnectionPageWidget::handlePacket(const QECPacket& p)
{
    const QECTag& tag = p.findTag(EC_TAG_PREFS_CONNECTIONS);
    if (tag.isNull()) {
        return;
    }

    foreach (const QECTag& subtag, tag) {
        switch (subtag.tagName()) {
            case EC_TAG_CONN_DL_CAP:
                m_downloadMax->setValue(subtag.toUInt16());
                break;
            case EC_TAG_CONN_UL_CAP:
                m_uploadMax->setValue(subtag.toUInt16());
                break;
            case EC_TAG_CONN_MAX_DL:
                m_downloadLimit->setValue(subtag.toUInt16());
                break;
            case EC_TAG_CONN_MAX_UL:
                m_uploadLimit->setValue(subtag.toUInt16());
                break;
            case EC_TAG_CONN_SLOT_ALLOCATION:
                m_slotAllocation->setValue(subtag.toUInt16());
                break;
            case EC_TAG_CONN_TCP_PORT:
                m_tcpPort->setValue(subtag.toUInt16());
                break;
            case EC_TAG_CONN_UDP_PORT:
                m_udpPort->setValue(subtag.toUInt16());
                break;
            case EC_TAG_CONN_MAX_FILE_SOURCES:
                m_maxFileSources->setValue(subtag.toUInt16());
                break;
            case EC_TAG_CONN_MAX_CONN:
                m_maxConnections->setValue(subtag.toUInt16());
                break;
            default:
                break;
        }
    }
}

void ConnectionPageWidget::load()
{
    QECPacket p(EC_OP_GET_PREFERENCES);
    p.addTag(EC_TAG_SELECT_PREFS, (quint64)EC_PREFS_CONNECTIONS);
    QECaMule::self()->sendPacket(p);
}

void ConnectionPageWidget::save()
{
    QECPacket p(EC_OP_SET_PREFERENCES);
    QECTag tag(EC_TAG_PREFS_CONNECTIONS);
    tag.addSubTag(EC_TAG_CONN_DL_CAP, m_downloadMax->value());
    tag.addSubTag(EC_TAG_CONN_UL_CAP, m_uploadMax->value());
    tag.addSubTag(EC_TAG_CONN_MAX_DL, m_downloadLimit->value());
    tag.addSubTag(EC_TAG_CONN_MAX_UL, m_uploadLimit->value());
    tag.addSubTag(EC_TAG_CONN_SLOT_ALLOCATION, m_slotAllocation->value());
    tag.addSubTag(EC_TAG_CONN_TCP_PORT, m_tcpPort->value());
    tag.addSubTag(EC_TAG_CONN_UDP_PORT, m_udpPort->value());
    tag.addSubTag(EC_TAG_CONN_MAX_FILE_SOURCES, m_maxFileSources->value());
    tag.addSubTag(EC_TAG_CONN_MAX_CONN, m_maxConnections->value());
    p.addTag(tag);
    QECaMule::self()->sendPacket(p);
}

void ConnectionPageWidget::slotDownloadMaxChanged(int value)
{
    m_downloadLimit->setMaximum(value);
    m_downloadSlider->setMaximum(value);
}

void ConnectionPageWidget::slotUploadMaxChanged(int value)
{
    m_uploadLimit->setMaximum(value);
    m_uploadSlider->setMaximum(value);
}
