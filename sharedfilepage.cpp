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

#include "sharedfilepage.h"

#include <QAction>
#include <QApplication>
#include <QClipboard>
#include <QIcon>
#include <QMenu>
#include <QSortFilterProxyModel>

#include <KLocalizedString>

#include "qecpacket.h"

#include "Constants.h"
#include "qecamule.h"

#include "sharedfilelistmodel.h"

SharedFilePageWidget::SharedFilePageWidget(QWidget* parent)
: QWidget(parent)
{
    setupUi(this);

    // set action
    copyLinkAct = new QAction(QIcon::fromTheme("document-export"), i18n("Copy Ed2k"), this);
    connect(copyLinkAct, SIGNAL(triggered()), this, SLOT(slotCopyLink()));
    priorityAutoAct = new QAction(i18n("Auto"), this);
    priorityAutoAct->setCheckable(true);
    priorityPowerShareAct = new QAction(i18n("Power Share"), this);
    priorityPowerShareAct->setCheckable(true);
    priorityVeryHighAct = new QAction(i18n("Very High"), this);
    priorityVeryHighAct->setCheckable(true);
    priorityHighAct = new QAction(i18n("High"), this);
    priorityHighAct->setCheckable(true);
    priorityNormalAct = new QAction(i18n("Normal"), this);
    priorityNormalAct->setCheckable(true);
    priorityLowAct = new QAction(i18n("Low"), this);
    priorityLowAct->setCheckable(true);
    priorityVeryLowAct = new QAction(i18n("Very Low"), this);
    priorityVeryLowAct->setCheckable(true);
    priorityAct = new QActionGroup(this);
    priorityAct->setExclusive(true);
    priorityAct->addAction(priorityAutoAct);
    priorityAct->addAction(priorityPowerShareAct);
    priorityAct->addAction(priorityVeryHighAct);
    priorityAct->addAction(priorityHighAct);
    priorityAct->addAction(priorityNormalAct);
    priorityAct->addAction(priorityLowAct);
    priorityAct->addAction(priorityVeryLowAct);
    connect(priorityAct, SIGNAL(triggered(QAction*)), this, SLOT(slotPriority(QAction*)));

    // set model
    SharedFileListModel* model = new SharedFileListModel;
    QSortFilterProxyModel* proxyModel = new QSortFilterProxyModel;
    proxyModel->setSourceModel(model);
    proxyModel->setSortRole(Qt::UserRole + 1);
    m_sharedFileListView->setModel(proxyModel);

    SharedFileListDelegate* delegate = new SharedFileListDelegate;
    m_sharedFileListView->setItemDelegate(delegate);

    connect(model, SIGNAL(rowsInserted(QModelIndex,int,int)), this, SLOT(resizeColumns()));
    connect(model, SIGNAL(rowsRemoved(QModelIndex,int,int)), this, SLOT(resizeColumns()));

    // context menu
    connect(m_sharedFileListView, SIGNAL(customContextMenuRequested(QPoint)),
            this, SLOT(showContextMenu(QPoint)));
}

SharedFilePageWidget::~SharedFilePageWidget()
{
}

void SharedFilePageWidget::resizeColumns()
{
    for (int i = 0; i < m_sharedFileListView->model()->columnCount(); ++i) {
        m_sharedFileListView->resizeColumnToContents(i);
    }
}

void SharedFilePageWidget::slotCopyLink()
{
    QModelIndex index = m_sharedFileListView->currentIndex();
    if (!index.isValid())
        return;

    const SharedFileInfo* sfi = index.data(Qt::UserRole).value<const SharedFileInfo*>();
    QApplication::clipboard()->setText(sfi->ed2klink);
}

void SharedFilePageWidget::slotPriority(QAction* act)
{
    QModelIndex index = m_sharedFileListView->currentIndex();
    if (!index.isValid())
        return;

    const SharedFileInfo* sfi = index.data(Qt::UserRole).value<const SharedFileInfo*>();
    QECPacket p(EC_OP_SHARED_SET_PRIO);
    QECTag tag(EC_TAG_PARTFILE, sfi->hash);

    if (act == priorityAutoAct)
        tag.addSubTag(EC_TAG_PARTFILE_PRIO, (quint64)PR_AUTO);
    else if (act == priorityPowerShareAct)
        tag.addSubTag(EC_TAG_PARTFILE_PRIO, (quint64)PR_POWERSHARE);
    else if (act == priorityVeryHighAct)
        tag.addSubTag(EC_TAG_PARTFILE_PRIO, (quint64)PR_VERYHIGH);
    else if (act == priorityHighAct)
        tag.addSubTag(EC_TAG_PARTFILE_PRIO, (quint64)PR_HIGH);
    else if (act == priorityNormalAct)
        tag.addSubTag(EC_TAG_PARTFILE_PRIO, (quint64)PR_NORMAL);
    else if (act == priorityLowAct)
        tag.addSubTag(EC_TAG_PARTFILE_PRIO, (quint64)PR_LOW);
    else if (act == priorityVeryLowAct)
        tag.addSubTag(EC_TAG_PARTFILE_PRIO, (quint64)PR_VERYLOW);
    else {
        // should never get here
        return;
    }

    p.addTag(tag);
    QECaMule::self()->sendPacket(p);
}

void SharedFilePageWidget::showContextMenu(const QPoint& pos)
{
    QModelIndex index = m_sharedFileListView->indexAt(pos);
    if (!index.isValid())
        return;

    const SharedFileInfo* sfi = index.data(Qt::UserRole).value<const SharedFileInfo*>();
    quint8 priority = sfi->priority;
    bool autopriority = (priority >= 10);
    if (autopriority) priority -= 10;

    QMenu menu;
    menu.addAction(copyLinkAct);
    QMenu* priorityMenu = menu.addMenu(QIcon::fromTheme("flag"), i18n("Set priority"));
    priorityMenu->addAction(priorityAutoAct);
    priorityMenu->addAction(priorityPowerShareAct);
    priorityMenu->addAction(priorityVeryHighAct);
    priorityMenu->addAction(priorityHighAct);
    priorityMenu->addAction(priorityNormalAct);
    priorityMenu->addAction(priorityLowAct);
    priorityMenu->addAction(priorityVeryLowAct);

    if (autopriority) {
        priorityAutoAct->setChecked(true);
    }
    else {
        switch (priority) {
            case PR_POWERSHARE: priorityPowerShareAct->setChecked(true); break;
            case PR_VERYHIGH: priorityVeryHighAct->setChecked(true); break;
            case PR_HIGH: priorityHighAct->setChecked(true); break;
            case PR_NORMAL: priorityNormalAct->setChecked(true); break;
            case PR_LOW: priorityLowAct->setChecked(true); break;
            case PR_VERYLOW: priorityVeryLowAct->setChecked(true); break;
            default: break;
        }
    }

    menu.exec(m_sharedFileListView->viewport()->mapToGlobal(pos));
}
