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

#include "networkpage.h"

#include <QAction>
#include <QSortFilterProxyModel>

#include <KDebug>
#include <KIcon>
#include <KLocale>
#include <KMenu>

#include "qecpacket.h"

#include "qecamule.h"

#include "serverlistmodel.h"

NetworkPageWidget::NetworkPageWidget(QWidget* parent)
: QWidget(parent)
{
    setupUi(this);

    // set action
    updateFromUrlAct = new QAction(KIcon("view-refresh"), i18n("Update From Url"), this);
    connect(updateFromUrlAct, SIGNAL(triggered()), this, SLOT(slotUpdateFromUrl()));
    addServerAct = new QAction(KIcon("list-add"), i18n("Add Server"), this);
    connect(addServerAct, SIGNAL(triggered()), this, SLOT(slotAddServer()));
    connectAct = new QAction(KIcon("network-connect"), i18n("Connect"), this);
    connect(connectAct, SIGNAL(triggered()), this, SLOT(slotConnect()));
    disconnectAct = new QAction(KIcon("network-disconnect"), i18n("Disconnect"), this);
    connect(disconnectAct, SIGNAL(triggered()), this, SLOT(slotDisconnect()));
    removeAct = new QAction(KIcon("list-remove"), i18n("Remove"), this);
    connect(removeAct, SIGNAL(triggered()), this, SLOT(slotRemove()));

    // set model
    ServerListModel* model = new ServerListModel;
    QSortFilterProxyModel* proxyModel = new QSortFilterProxyModel;
    proxyModel->setSourceModel(model);
    proxyModel->setSortRole(Qt::UserRole + 1);
    m_serverListView->setModel(proxyModel);

    connect(model, SIGNAL(rowsInserted(QModelIndex,int,int)), this, SLOT(resizeColumns()));
    connect(model, SIGNAL(rowsRemoved(QModelIndex,int,int)), this, SLOT(resizeColumns()));

    // button action
    m_updateFromUrl->setDefaultAction(updateFromUrlAct);
    m_addServer->setDefaultAction(addServerAct);

    // context menu
    connect(m_serverListView, SIGNAL(customContextMenuRequested(QPoint)),
            this, SLOT(showContextMenu(QPoint)));
}

NetworkPageWidget::~NetworkPageWidget()
{
}

void NetworkPageWidget::resizeColumns()
{
    for (int i = 0; i < m_serverListView->model()->columnCount(); ++i) {
        m_serverListView->resizeColumnToContents(i);
    }
}

void NetworkPageWidget::slotUpdateFromUrl()
{
    QECPacket p(EC_OP_SERVER_UPDATE_FROM_URL);
    p.addTag(EC_TAG_SERVERS_UPDATE_URL, m_urlEdit->text());
    QECaMule::self()->sendPacket(p);
}

void NetworkPageWidget::slotAddServer()
{
    QECPacket p(EC_OP_SERVER_ADD);
    p.addTag(EC_TAG_SERVER_NAME, m_serverNameEdit->text());
    p.addTag(EC_TAG_SERVER_ADDRESS, m_ipEdit->text() + ':' + m_portEdit->text());
    QECaMule::self()->sendPacket(p);
}

void NetworkPageWidget::slotConnect()
{
    QModelIndex index = m_serverListView->currentIndex();
    if (!index.isValid())
        return;

    const ServerInfo* si = index.data(Qt::UserRole).value<const ServerInfo*>();
    QECPacket p(EC_OP_SERVER_CONNECT);
    p.addTag(EC_TAG_SERVER, si->ip, si->port);
    QECaMule::self()->sendPacket(p);
}

void NetworkPageWidget::slotDisconnect()
{
    QModelIndex index = m_serverListView->currentIndex();
    if (!index.isValid())
        return;

    QECPacket p(EC_OP_SERVER_DISCONNECT);
    QECaMule::self()->sendPacket(p);
}

void NetworkPageWidget::slotRemove()
{
    QModelIndex index = m_serverListView->currentIndex();
    if (!index.isValid())
        return;

    const ServerInfo* si = index.data(Qt::UserRole).value<const ServerInfo*>();
    QECPacket p(EC_OP_SERVER_REMOVE);
    p.addTag(EC_TAG_SERVER, si->ip, si->port);
    QECaMule::self()->sendPacket(p);
}

void NetworkPageWidget::showContextMenu(const QPoint& pos)
{
    QModelIndex index = m_serverListView->indexAt(pos);
    if (!index.isValid())
        return;

    KMenu menu;
    menu.addAction(connectAct);
    menu.addAction(disconnectAct);
    menu.addAction(removeAct);
    menu.exec(m_serverListView->viewport()->mapToGlobal(pos));
}
