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

#include "kamule.h"

#include <QIcon>
#include <QMenuBar>
#include <QTabWidget>
#include <QTimer>

#include <KStatusNotifierItem>

#include "qecpacket.h"

#include "qecamule.h"

#include "amuledpage.h"
#include "networkpage.h"
#include "searchpage.h"
#include "downloadpage.h"
#include "uploadpage.h"
#include "sharedfilepage.h"
#include "preferencepage.h"

#include "kamulesettings.h"

#include "mystatusbar.h"

KaMule::KaMule()
: QMainWindow()
{
    tabwidget = new QTabWidget;
    tabwidget->setDocumentMode(true);
    tabwidget->setIconSize(QSize(22, 22));
    setCentralWidget(tabwidget);

    amuledpage = new AMuledPageWidget(this);
    tabwidget->addTab(amuledpage, QIcon::fromTheme("kamule"), i18n("AMuled"));

    networkpage = new NetworkPageWidget(this);
    tabwidget->addTab(networkpage, QIcon::fromTheme("applications-internet"), i18n("Network"));

    searchpage = new SearchPageWidget(this);
    tabwidget->addTab(searchpage, QIcon::fromTheme("edit-find"), i18n("Search"));

    downloadpage = new DownloadPageWidget(this);
    tabwidget->addTab(downloadpage, QIcon::fromTheme("go-down"), i18n("Download"));

    uploadpage = new UploadPageWidget(this);
    tabwidget->addTab(uploadpage, QIcon::fromTheme("go-up"), i18n("Upload"));

    sharedfilepage = new SharedFilePageWidget(this);
    tabwidget->addTab(sharedfilepage, QIcon::fromTheme("folder-remote"), i18n("Shared files"));

    preferencepage = new PreferencePageWidget(this);
    tabwidget->addTab(preferencepage, QIcon::fromTheme("preferences-system"), i18n("Preference"));

    timer = new QTimer(this);
    timer->setSingleShot(true);
    connect(timer, SIGNAL(timeout()), this, SLOT(slotRefreshTab()));

    timer2 = new QTimer(this);
    timer2->setSingleShot(true);
    connect(timer2, SIGNAL(timeout()), this, SLOT(slotRefreshConnState()));

    m_logined = false;

    MyStatusBar* sb = new MyStatusBar(this);
    setStatusBar(sb);

    KStatusNotifierItem* tray = new KStatusNotifierItem(this);
    tray->setIconByName("kamule");
    tray->setTitle(i18n("KaMule"));
    tray->setToolTipIconByName("kamule");
    tray->setToolTipTitle(i18n("KaMule"));
    tray->setToolTipSubTitle(i18n("KDE aMule Client"));
    tray->setCategory(KStatusNotifierItem::ApplicationStatus);
    tray->setStatus(KStatusNotifierItem::Active);

    menuBar()->hide();

    QECaMule::self()->addSubscriber(EC_OP_AUTH_OK, this);
}

KaMule::~KaMule()
{
    QECaMule::self()->removeSubscriber(EC_OP_AUTH_OK, this);
}

void KaMule::handlePacket(const QECPacket& /*p*/)
{
    m_logined = true;

    connect(tabwidget, SIGNAL(currentChanged(int)), this, SLOT(slotCurrentChanged(int)));

    timer->start(1500);
}

void KaMule::hideEvent(QHideEvent* /*event*/)
{
    timer->stop();
}

void KaMule::showEvent(QShowEvent* /*event*/)
{
    if (m_logined)
        timer->start(0);
}

void KaMule::slotCurrentChanged(int index)
{
    static QList<int> m_tabchangelist;
    if (!m_tabchangelist.contains(index)) {
        m_tabchangelist.append(index);
        slotRefreshTab();
    }
}

void KaMule::slotRefreshTab()
{
    timer->start(3000);
    timer2->start(1500);

    QWidget* cw = tabwidget->currentWidget();

    if (cw == networkpage) {
        QECPacket p(EC_OP_GET_SERVER_LIST);
        QECaMule::self()->sendPacket(p);
    }
    else if (cw == downloadpage) {
        QECPacket p(EC_OP_GET_DLOAD_QUEUE);
        QECaMule::self()->sendPacket(p);
    }
    else if (cw == uploadpage) {
        QECPacket p(EC_OP_GET_ULOAD_QUEUE);
        QECaMule::self()->sendPacket(p);
    }
    else if (cw == sharedfilepage) {
        QECPacket p(EC_OP_GET_SHARED_FILES);
        QECaMule::self()->sendPacket(p);
    }
    else if (cw == preferencepage) {
        preferencepage->slotCurrentPageChanged();
    }
}

void KaMule::slotRefreshConnState()
{
    QECPacket p(EC_OP_STAT_REQ);
    p.addTag(EC_TAG_DETAIL_LEVEL, (quint64)EC_DETAIL_CMD);
    QECaMule::self()->sendPacket(p);
}
