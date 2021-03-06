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

#include "downloadpage.h"

#include <QAction>
#include <QApplication>
#include <QClipboard>
#include <QDateTime>
#include <QMenu>
#include <QSortFilterProxyModel>

#include <kio/global.h>
#include <KLocalizedString>

#include "qecpacket.h"
#include "Constants.h"

#include "qecamule.h"

#include "downloadlistmodel.h"

#include "ui_downloadinfodialog.h"

DownloadPageWidget::DownloadPageWidget(QWidget* parent)
: QWidget(parent)
{
    setupUi(this);

    // set action
    addLinkAct = new QAction(QIcon::fromTheme("list-add"), i18n("Add Link"), this);
    connect(addLinkAct, SIGNAL(triggered()), this, SLOT(slotAddLink()));
    infoAct = new QAction(QIcon::fromTheme("preferences-desktop-notification"), i18n("Info"), this);
    connect(infoAct, SIGNAL(triggered()), this, SLOT(slotInfo()));
    pauseAct = new QAction(QIcon::fromTheme("media-playback-pause"), i18n("Pause"), this);
    connect(pauseAct, SIGNAL(triggered()), this, SLOT(slotPause()));
    resumeAct = new QAction(QIcon::fromTheme("media-playback-start"), i18n("Resume"), this);
    connect(resumeAct, SIGNAL(triggered()), this, SLOT(slotResume()));
    stopAct = new QAction(QIcon::fromTheme("media-playback-stop"), i18n("Stop"), this);
    connect(stopAct, SIGNAL(triggered()), this, SLOT(slotStop()));
    deleteAct = new QAction(QIcon::fromTheme("edit-delete"), i18n("Delete"), this);
    connect(deleteAct, SIGNAL(triggered()), this, SLOT(slotDelete()));
    copyLinkAct = new QAction(QIcon::fromTheme("document-export"), i18n("Copy Ed2k"), this);
    connect(copyLinkAct, SIGNAL(triggered()), this, SLOT(slotCopyLink()));
    priorityAutoAct = new QAction(i18n("Auto"), this);
    priorityAutoAct->setCheckable(true);
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
    priorityAct->addAction(priorityVeryHighAct);
    priorityAct->addAction(priorityHighAct);
    priorityAct->addAction(priorityNormalAct);
    priorityAct->addAction(priorityLowAct);
    priorityAct->addAction(priorityVeryLowAct);
    connect(priorityAct, SIGNAL(triggered(QAction*)), this, SLOT(slotPriority(QAction*)));

    // set model
    DownloadListModel* model = new DownloadListModel;
    QSortFilterProxyModel* proxyModel = new QSortFilterProxyModel;
    proxyModel->setSourceModel(model);
    proxyModel->setSortRole(Qt::UserRole + 1);
    m_downloadListView->setModel(proxyModel);

    DownloadListDelegate* delegate = new DownloadListDelegate;
    m_downloadListView->setItemDelegate(delegate);

    connect(model, SIGNAL(rowsInserted(QModelIndex,int,int)), this, SLOT(resizeColumns()));
    connect(model, SIGNAL(rowsRemoved(QModelIndex,int,int)), this, SLOT(resizeColumns()));

    // button action
    m_addLink->setDefaultAction(addLinkAct);

    // context menu
    connect(m_downloadListView, SIGNAL(customContextMenuRequested(QPoint)),
            this, SLOT(showContextMenu(QPoint)));
}

DownloadPageWidget::~DownloadPageWidget()
{
}

void DownloadPageWidget::resizeColumns()
{
    for (int i = 0; i < m_downloadListView->model()->columnCount(); ++i) {
        m_downloadListView->resizeColumnToContents(i);
    }
}

void DownloadPageWidget::slotAddLink()
{
    QECPacket p(EC_OP_ADD_LINK);
    p.addTag(EC_TAG_STRING, m_linkEdit->text());
    QECaMule::self()->sendPacket(p);
}

void DownloadPageWidget::slotInfo()
{
    QModelIndex index = m_downloadListView->currentIndex();
    if (!index.isValid())
        return;

    const PartfileInfo* pfi = index.data(Qt::UserRole).value<const PartfileInfo*>();

    Ui::DownloadInfoDialog ui;
    QDialog dialog;
    ui.setupUi(&dialog);

    ui.m_filenameEdit->setText(pfi->name);
    ui.m_ed2kLinkEdit->setText(pfi->ed2klink);
    ui.m_sizeFullEdit->setText(KIO::convertSize(pfi->sizefull));
    ui.m_sizeDoneEdit->setText(KIO::convertSize(pfi->sizedone));
    ui.m_priorityEdit->setText(QString::number(pfi->priority));
    ui.m_availablePartsEdit->setText(QString::number(pfi->availableparts));
    ui.m_lostCorruptionEdit->setText(KIO::convertSize(pfi->lostcorruption));
    ui.m_gainedCompressionEdit->setText(KIO::convertSize(pfi->gainedcompression));
    ui.m_lastReceiveEdit->setText(QDateTime::fromTime_t(pfi->lastrecv).toString());
    ui.m_lastSeenEdit->setText(QDateTime::fromTime_t(pfi->lastseencomplete).toString());

    dialog.exec();
}

void DownloadPageWidget::slotPause()
{
    QModelIndex index = m_downloadListView->currentIndex();
    if (!index.isValid())
        return;

    const PartfileInfo* pfi = index.data(Qt::UserRole).value<const PartfileInfo*>();
    QECPacket p(EC_OP_PARTFILE_PAUSE);
    p.addTag(EC_TAG_PARTFILE, pfi->hash);
    QECaMule::self()->sendPacket(p);
}

void DownloadPageWidget::slotResume()
{
    QModelIndex index = m_downloadListView->currentIndex();
    if (!index.isValid())
        return;

    const PartfileInfo* pfi = index.data(Qt::UserRole).value<const PartfileInfo*>();
    QECPacket p(EC_OP_PARTFILE_RESUME);
    p.addTag(EC_TAG_PARTFILE, pfi->hash);
    QECaMule::self()->sendPacket(p);
}

void DownloadPageWidget::slotStop()
{
    QModelIndex index = m_downloadListView->currentIndex();
    if (!index.isValid())
        return;

    const PartfileInfo* pfi = index.data(Qt::UserRole).value<const PartfileInfo*>();
    QECPacket p(EC_OP_PARTFILE_STOP);
    p.addTag(EC_TAG_PARTFILE, pfi->hash);
    QECaMule::self()->sendPacket(p);
}

void DownloadPageWidget::slotDelete()
{
    QModelIndex index = m_downloadListView->currentIndex();
    if (!index.isValid())
        return;

    const PartfileInfo* pfi = index.data(Qt::UserRole).value<const PartfileInfo*>();
    QECPacket p(EC_OP_PARTFILE_DELETE);
    p.addTag(EC_TAG_PARTFILE, pfi->hash);
    QECaMule::self()->sendPacket(p);
}

void DownloadPageWidget::slotCopyLink()
{
    QModelIndex index = m_downloadListView->currentIndex();
    if (!index.isValid())
        return;

    const PartfileInfo* pfi = index.data(Qt::UserRole).value<const PartfileInfo*>();
    QApplication::clipboard()->setText(pfi->ed2klink);
}

void DownloadPageWidget::slotPriority(QAction* act)
{
    QModelIndex index = m_downloadListView->currentIndex();
    if (!index.isValid())
        return;

    const PartfileInfo* pfi = index.data(Qt::UserRole).value<const PartfileInfo*>();
    QECPacket p(EC_OP_PARTFILE_PRIO_SET);
    QECTag tag(EC_TAG_PARTFILE, pfi->hash);

    if (act == priorityAutoAct)
        tag.addSubTag(EC_TAG_PARTFILE_PRIO, (quint64)PR_AUTO);
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

void DownloadPageWidget::showContextMenu(const QPoint& pos)
{
    QModelIndex index = m_downloadListView->indexAt(pos);
    if (!index.isValid())
        return;

    const PartfileInfo* pfi = index.data(Qt::UserRole).value<const PartfileInfo*>();
    quint8 priority = pfi->priority;
    bool autopriority = (priority >= 10);
    if (autopriority) priority -= 10;

    QMenu menu;
    menu.addAction(infoAct);
    menu.addAction(pauseAct);
    menu.addAction(resumeAct);
    menu.addAction(stopAct);
    menu.addAction(deleteAct);
    menu.addAction(copyLinkAct);
    QMenu* priorityMenu = menu.addMenu(QIcon::fromTheme("flag"), i18n("Set priority"));
    priorityMenu->addAction(priorityAutoAct);
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
            case PR_VERYHIGH: priorityVeryHighAct->setChecked(true); break;
            case PR_HIGH: priorityHighAct->setChecked(true); break;
            case PR_NORMAL: priorityNormalAct->setChecked(true); break;
            case PR_LOW: priorityLowAct->setChecked(true); break;
            case PR_VERYLOW: priorityVeryLowAct->setChecked(true); break;
            default: break;
        }
    }

    menu.exec(m_downloadListView->viewport()->mapToGlobal(pos));
}
