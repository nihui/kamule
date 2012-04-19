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

#include "filepage.h"

#include "qecpacket.h"

#include "qecamule.h"

FilePageWidget::FilePageWidget(QWidget* parent)
: PreferenceWidget(parent)
{
    setupUi(this);

    QECaMule::self()->addSubscriber(EC_OP_SET_PREFERENCES, this);
}

FilePageWidget::~FilePageWidget()
{
    QECaMule::self()->removeSubscriber(EC_OP_SET_PREFERENCES, this);
}

void FilePageWidget::handlePacket(const QECPacket& p)
{
    const QECTag& tag = p.findTag(EC_TAG_PREFS_FILES);
    if (tag.isNull()) {
        return;
    }

    foreach (const QECTag& subtag, tag) {
        switch (subtag.tagName()) {
            case EC_TAG_FILES_ICH_ENABLED:
                m_enableICH->setChecked(subtag.toBool());
                break;
            case EC_TAG_FILES_NEW_AUTO_DL_PRIO:
                m_autoDownloadPriority->setChecked(subtag.toBool());
                break;
            case EC_TAG_FILES_NEW_AUTO_UL_PRIO:
                m_autoUploadPriority->setChecked(subtag.toBool());
                break;
            case EC_TAG_FILES_CHECK_FREE_SPACE:
                m_checkFreeSpace->setChecked(subtag.toBool());
                break;
            case EC_TAG_FILES_MIN_FREE_SPACE:
                m_mimFreeSpace->setValue(subtag.toUInt32());
                break;
            default:
                break;
        }
    }
}

void FilePageWidget::load()
{
    QECPacket p(EC_OP_GET_PREFERENCES);
    p.addTag(EC_TAG_SELECT_PREFS, (quint64)EC_PREFS_FILES);
    QECaMule::self()->sendPacket(p);
}

void FilePageWidget::save()
{
    QECPacket p(EC_OP_SET_PREFERENCES);
    QECTag tag(EC_TAG_PREFS_FILES);
    tag.addSubTag(EC_TAG_FILES_ICH_ENABLED, m_enableICH->isChecked());
    tag.addSubTag(EC_TAG_FILES_NEW_AUTO_DL_PRIO, m_autoDownloadPriority->isChecked());
    tag.addSubTag(EC_TAG_FILES_NEW_AUTO_UL_PRIO, m_autoUploadPriority->isChecked());
    tag.addSubTag(EC_TAG_FILES_CHECK_FREE_SPACE, m_checkFreeSpace->isChecked());
    tag.addSubTag(EC_TAG_FILES_MIN_FREE_SPACE, m_mimFreeSpace->value());
    p.addTag(tag);
    QECaMule::self()->sendPacket(p);
}
