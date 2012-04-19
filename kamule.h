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

#ifndef KAMULE_H
#define KAMULE_H

#include <KXmlGuiWindow>
#include "qecpacketsubscriber.h"

class QTabWidget;
class QTimer;

class AMuledPageWidget;
class NetworkPageWidget;
class SearchPageWidget;
class DownloadPageWidget;
class UploadPageWidget;
class SharedFilePageWidget;
class PreferencePageWidget;

class KaMule : public KXmlGuiWindow, public QECPacketSubscriber
{
    Q_OBJECT
public:
    explicit KaMule();
    virtual ~KaMule();
    virtual void handlePacket(const QECPacket& p);
protected:
    virtual void hideEvent(QHideEvent* event);
    virtual void showEvent(QShowEvent* event);
private Q_SLOTS:
    void slotCurrentChanged(int index);
    void slotRefreshTab();
    void slotRefreshConnState();

private:
    QTabWidget* tabwidget;
    QTimer* timer;
    QTimer* timer2;
    bool m_logined;

    AMuledPageWidget* amuledpage;
    NetworkPageWidget* networkpage;
    SearchPageWidget* searchpage;
    DownloadPageWidget* downloadpage;
    UploadPageWidget* uploadpage;
    SharedFilePageWidget* sharedfilepage;
    PreferencePageWidget* preferencepage;
};

#endif // KAMULE_H
