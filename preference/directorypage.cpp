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

#include "directorypage.h"

#include "qecpacket.h"

#include "qecamule.h"

DirectoryPageWidget::DirectoryPageWidget(QWidget* parent)
: PreferenceWidget(parent)
{
    setupUi(this);

    QECaMule::self()->addSubscriber(EC_OP_SET_PREFERENCES, this);
}

DirectoryPageWidget::~DirectoryPageWidget()
{
    QECaMule::self()->removeSubscriber(EC_OP_SET_PREFERENCES, this);
}

void DirectoryPageWidget::handlePacket(const QECPacket& p)
{
    const QECTag& tag = p.findTag(EC_TAG_PREFS_DIRECTORIES);
    if (tag.isNull()) {
        return;
    }

    foreach (const QECTag& subtag, tag) {
        switch (subtag.tagName()) {
            case EC_TAG_DIRECTORIES_INCOMING:
                m_incomingUrl->setText(subtag.toString());
                break;
            case EC_TAG_DIRECTORIES_TEMP:
                m_tempUrl->setText(subtag.toString());
                break;
            default:
                break;
        }
    }
}

void DirectoryPageWidget::load()
{
    QECPacket p(EC_OP_GET_PREFERENCES);
    p.addTag(EC_TAG_SELECT_PREFS, (quint64)EC_PREFS_DIRECTORIES);
    QECaMule::self()->sendPacket(p);
}

void DirectoryPageWidget::save()
{
    QECPacket p(EC_OP_SET_PREFERENCES);
    QECTag tag(EC_TAG_PREFS_DIRECTORIES);
    tag.addSubTag(EC_TAG_DIRECTORIES_INCOMING, m_incomingUrl->text());
    tag.addSubTag(EC_TAG_DIRECTORIES_TEMP, m_tempUrl->text());
    p.addTag(tag);
    QECaMule::self()->sendPacket(p);
}
