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
#include <QSortFilterProxyModel>

#include <KDebug>
#include <KIcon>
#include <KLocale>
#include <KMenu>

#include "qecamule.h"

#include "sharedfilelistmodel.h"

SharedFilePageWidget::SharedFilePageWidget(QWidget* parent)
: QWidget(parent)
{
    setupUi(this);

    // set action
    copyLinkAct = new QAction(KIcon("document-export"), i18n("Copy Ed2k"), this);
    connect(copyLinkAct, SIGNAL(triggered()), this, SLOT(slotCopyLink()));

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

void SharedFilePageWidget::showContextMenu(const QPoint& pos)
{
    QModelIndex index = m_sharedFileListView->indexAt(pos);
    if (!index.isValid())
        return;

    KMenu menu;
    menu.addAction(copyLinkAct);
    menu.exec(m_sharedFileListView->viewport()->mapToGlobal(pos));
}
