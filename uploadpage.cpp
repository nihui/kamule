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

#include "uploadpage.h"

#include <QAction>
#include <QHostAddress>
#include <QSortFilterProxyModel>

#include <KDebug>
#include <KIcon>
#include <kio/global.h>
#include <KLocale>
#include <KMenu>

#include "qecamule.h"

#include "uploadlistmodel.h"

#include "ui_uploadinfodialog.h"

UploadPageWidget::UploadPageWidget(QWidget* parent)
: QWidget(parent)
{
    setupUi(this);

    // set action
    infoAct = new QAction(KIcon("preferences-desktop-notification"), i18n("Info"), this);
    connect(infoAct, SIGNAL(triggered()), this, SLOT(slotInfo()));

    // set model
    UploadListModel* model = new UploadListModel;
    QSortFilterProxyModel* proxyModel = new QSortFilterProxyModel;
    proxyModel->setSourceModel(model);
    proxyModel->setSortRole(Qt::UserRole + 1);
    m_uploadListView->setModel(proxyModel);

    connect(model, SIGNAL(rowsInserted(QModelIndex,int,int)), this, SLOT(resizeColumns()));
    connect(model, SIGNAL(rowsRemoved(QModelIndex,int,int)), this, SLOT(resizeColumns()));

    // context menu
    connect(m_uploadListView, SIGNAL(customContextMenuRequested(QPoint)),
            this, SLOT(showContextMenu(QPoint)));
}

UploadPageWidget::~UploadPageWidget()
{
}

void UploadPageWidget::resizeColumns()
{
    for (int i = 0; i < m_uploadListView->model()->columnCount(); ++i) {
        m_uploadListView->resizeColumnToContents(i);
    }
}

void UploadPageWidget::slotInfo()
{
    QModelIndex index = m_uploadListView->currentIndex();
    if (!index.isValid())
        return;

    const ClientInfo* ci = index.data(Qt::UserRole).value<const ClientInfo*>();

    Ui::UploadInfoDialog ui;
    QDialog dialog;
    ui.setupUi(&dialog);

    ui.m_nameEdit->setText(ci->name);
    ui.m_userIPPortEdit->setText(QString("%1:%2").arg(QHostAddress(ci->userip).toString()).arg(ci->userport));
    ui.m_serverEdit->setText(ci->servername);
    ui.m_serverIPPortEdit->setText(QString("%1:%2").arg(QHostAddress(ci->serverip).toString()).arg(ci->serverport));
    ui.m_userHashEdit->setText(QString(ci->userhash.toHex()));
    ui.m_softwareEdit->setText(ci->softverstr);
    ui.m_uploadFileEdit->setText(ci->filename);
    ui.m_uploadTotalEdit->setText(KIO::convertSize(ci->uploadtotal));
    ui.m_downloadTotalEdit->setText(KIO::convertSize(ci->downloadtotal));

    dialog.exec();
}

void UploadPageWidget::showContextMenu(const QPoint& pos)
{
    QModelIndex index = m_uploadListView->indexAt(pos);
    if (!index.isValid())
        return;

    KMenu menu;
    menu.addAction(infoAct);
    menu.exec(m_uploadListView->viewport()->mapToGlobal(pos));
}
