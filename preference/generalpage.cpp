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

#include "generalpage.h"

#include "qecpacket.h"

#include "qecamule.h"

GeneralPageWidget::GeneralPageWidget(QWidget* parent)
: PreferenceWidget(parent)
{
    setupUi(this);

    QECaMule::self()->addSubscriber(EC_OP_SET_PREFERENCES, this);
}

GeneralPageWidget::~GeneralPageWidget()
{
    QECaMule::self()->removeSubscriber(EC_OP_SET_PREFERENCES, this);
}

void GeneralPageWidget::handlePacket(const QECPacket& p)
{
    const QECTag& tag = p.findTag(EC_TAG_PREFS_GENERAL);
    if (tag.isNull()) {
        return;
    }

    foreach (const QECTag& subtag, tag) {
        switch (subtag.tagName()) {
            case EC_TAG_USER_NICK:
                m_nickEdit->setText(subtag.toString());
                break;
            case EC_TAG_USER_HASH:
                m_hashEdit->setText(subtag.toByteArray().toHex());
                break;
            case EC_TAG_USER_HOST:
                m_hostEdit->setText(subtag.toString());
                break;
            case EC_TAG_GENERAL_CHECK_NEW_VERSION:
                m_checkNewVersion->setChecked(subtag.toBool());
                break;
            default:
                break;
        }
    }
}

void GeneralPageWidget::load()
{
    QECPacket p(EC_OP_GET_PREFERENCES);
    p.addTag(EC_TAG_SELECT_PREFS, (quint64)EC_PREFS_GENERAL);
    QECaMule::self()->sendPacket(p);
}

void GeneralPageWidget::save()
{
    QECPacket p(EC_OP_SET_PREFERENCES);
    QECTag tag(EC_TAG_PREFS_GENERAL);
    tag.addSubTag(EC_TAG_USER_NICK, m_nickEdit->text());
    tag.addSubTag(EC_TAG_USER_HOST, m_hostEdit->text());
    tag.addSubTag(EC_TAG_GENERAL_CHECK_NEW_VERSION, m_checkNewVersion->isChecked());
    p.addTag(tag);
    QECaMule::self()->sendPacket(p);
}
