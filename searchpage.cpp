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

#include "searchpage.h"

#include <QAction>
#include <QApplication>
#include <QClipboard>
#include <QSortFilterProxyModel>
#include <QTimer>

#include <KDebug>
#include <KIcon>
#include <KLocale>
#include <KMenu>

#include "qecpacket.h"

#include "qecamule.h"

#include "searchlistmodel.h"

SearchPageWidget::SearchPageWidget(QWidget* parent)
: QWidget(parent)
{
    setupUi(this);

    // set action
    searchAct = new QAction(KIcon("edit-find"), i18n("Search"), this);
    connect(searchAct, SIGNAL(triggered()), this, SLOT(slotSearch()));
    cancelAct = new QAction(KIcon("process-stop"), i18n("Cancel"), this);
    connect(cancelAct, SIGNAL(triggered()), this, SLOT(slotCancel()));
    downloadAct = new QAction(KIcon("download"), i18n("Download"), this);
    connect(downloadAct, SIGNAL(triggered()), this, SLOT(slotDownload()));
    copyLinkAct = new QAction(KIcon("document-export"), i18n("Copy Ed2k"), this);
    connect(copyLinkAct, SIGNAL(triggered()), this, SLOT(slotCopyLink()));

    // set model
    SearchListModel* model = new SearchListModel;
    QSortFilterProxyModel* proxyModel = new QSortFilterProxyModel;
    proxyModel->setSourceModel(model);
    proxyModel->setSortRole(Qt::UserRole + 1);
    m_searchListView->setModel(proxyModel);

    connect(model, SIGNAL(rowsInserted(QModelIndex,int,int)), this, SLOT(resizeColumns()));
    connect(model, SIGNAL(rowsRemoved(QModelIndex,int,int)), this, SLOT(resizeColumns()));

    // button action
    m_search->setDefaultAction(searchAct);
    m_cancel->setDefaultAction(cancelAct);

    // context menu
    connect(m_searchListView, SIGNAL(customContextMenuRequested(QPoint)),
            this, SLOT(showContextMenu(QPoint)));

    QECaMule::self()->addSubscriber(EC_OP_SEARCH_PROGRESS, this);
}

SearchPageWidget::~SearchPageWidget()
{
    QECaMule::self()->removeSubscriber(EC_OP_SEARCH_PROGRESS, this);
}

void SearchPageWidget::resizeColumns()
{
    for (int i = 0; i < m_searchListView->model()->columnCount(); ++i) {
        m_searchListView->resizeColumnToContents(i);
    }
}

void SearchPageWidget::handlePacket(const QECPacket& p)
{
    kWarning();
    const QECTag& tag_searchstatus = p.findTag(EC_TAG_SEARCH_STATUS);
    if (tag_searchstatus.isNull()) {
        return;
    }

    quint32 progress = tag_searchstatus.toUInt32();
    if (progress == 0) {
        /// finished
        m_progress->setValue(0);
        searchResult();
        return;
    }

    /// progress
    m_progress->setValue(progress);
    QTimer::singleShot(500, this, SLOT(searchProgress()));
}

void SearchPageWidget::slotSearch()
{
    QECPacket p(EC_OP_SEARCH_START);
    QECTag tag(EC_TAG_SEARCH_TYPE);
    if (m_searchTypeCombo->currentIndex() == 0)
        tag.setTagData((quint64)EC_SEARCH_GLOBAL);
    else if (m_searchTypeCombo->currentIndex() == 1)
        tag.setTagData((quint64)EC_SEARCH_LOCAL);
    else
        tag.setTagData((quint64)EC_SEARCH_KAD);
    tag.addSubTag(EC_TAG_SEARCH_NAME, m_searchEdit->text());
    p.addTag(tag);
    QECaMule::self()->sendPacket(p);

    QTimer::singleShot(1000, this, SLOT(searchProgress()));
}

void SearchPageWidget::slotCancel()
{
    QECPacket p(EC_OP_SEARCH_STOP);
    QECaMule::self()->sendPacket(p);
}

void SearchPageWidget::searchProgress()
{
    QECPacket p(EC_OP_SEARCH_PROGRESS);
    QECaMule::self()->sendPacket(p);
}

void SearchPageWidget::searchResult()
{
    QECPacket p(EC_OP_SEARCH_RESULTS);
    p.addTag(EC_DETAIL_FULL);
    QECaMule::self()->sendPacket(p);
}

void SearchPageWidget::slotDownload()
{
    QModelIndex index = m_searchListView->currentIndex();
    if (!index.isValid())
        return;

    const SearchFileInfo* sfi = index.data(Qt::UserRole).value<const SearchFileInfo*>();
    QECPacket p(EC_OP_DOWNLOAD_SEARCH_RESULT);
    p.addTag(EC_TAG_PARTFILE, sfi->hash);
    QECaMule::self()->sendPacket(p);
}

void SearchPageWidget::slotCopyLink()
{
    QModelIndex index = m_searchListView->currentIndex();
    if (!index.isValid())
        return;

    const SearchFileInfo* sfi = index.data(Qt::UserRole).value<const SearchFileInfo*>();
    QString ed2kscheme("ed2k://|file|%1|%2|%3|/");
    QString ed2klink = ed2kscheme.arg(sfi->name).arg(sfi->sizefull).arg(QString(sfi->hash.toHex()));
    QApplication::clipboard()->setText(ed2klink);
}

void SearchPageWidget::showContextMenu(const QPoint& pos)
{
    QModelIndex index = m_searchListView->indexAt(pos);
    if (!index.isValid())
        return;

    QMenu menu;
    menu.addAction(downloadAct);
    menu.addAction(copyLinkAct);
    menu.exec(m_searchListView->viewport()->mapToGlobal(pos));
}
