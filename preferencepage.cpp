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

#include "preferencepage.h"

#include <KDebug>
#include <KIcon>
#include <KLocale>

#include "qecpacket.h"

#include "qecamule.h"

#include "preferencemodel.h"
#include "preferencewidget.h"

PreferencePageWidget::PreferencePageWidget(QWidget* parent)
: QWidget(parent)
{
    setupUi(this);

    // set action
    resetAct = new QAction(KIcon("edit-undo"), i18n("Reset"), this);
    connect(resetAct, SIGNAL(triggered()), this, SLOT(slotReset()));
    applyAct = new QAction(KIcon("dialog-ok-apply"), i18n("Apply"), this);
    connect(applyAct, SIGNAL(triggered()), this, SLOT(slotApply()));

    // set model
    PreferenceModel* model = new PreferenceModel;
    m_pageView->setModel(model);

    // button action
    m_reset->setDefaultAction(resetAct);
    m_apply->setDefaultAction(applyAct);

    QECaMule::self()->addSubscriber(EC_OP_AUTH_OK, this);
}

PreferencePageWidget::~PreferencePageWidget()
{
    QECaMule::self()->removeSubscriber(EC_OP_AUTH_OK, this);
}

void PreferencePageWidget::handlePacket(const QECPacket& /*p*/)
{
    connect(m_pageView, SIGNAL(currentPageChanged(QModelIndex,QModelIndex)),
            this, SLOT(slotCurrentPageChanged()));
}

void PreferencePageWidget::slotCurrentPageChanged()
{
    QModelIndex index = m_pageView->currentPage();
    if (!index.isValid())
        return;

    int row = index.row();

    static QList<int> m_pagechangelist;
    if (m_pagechangelist.contains(row)) {
        return;
    }
    m_pagechangelist.append(row);

    QWidget* w = index.data(KPageModel::WidgetRole).value<QWidget*>();
    PreferenceWidget* pw = static_cast<PreferenceWidget*>(w);
    pw->load();
}

void PreferencePageWidget::slotReset()
{
    QModelIndex index = m_pageView->currentPage();
    if (!index.isValid())
        return;

    QWidget* w = index.data(KPageModel::WidgetRole).value<QWidget*>();
    PreferenceWidget* pw = static_cast<PreferenceWidget*>(w);
    pw->load();
}

void PreferencePageWidget::slotApply()
{
    QModelIndex index = m_pageView->currentPage();
    if (!index.isValid())
        return;

    QWidget* w = index.data(KPageModel::WidgetRole).value<QWidget*>();
    PreferenceWidget* pw = static_cast<PreferenceWidget*>(w);
    pw->save();
}
