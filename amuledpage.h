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

#ifndef AMULEDPAGE_H
#define AMULEDPAGE_H

#include "ui_amuledpage.h"
#include "qecpacketsubscriber.h"

class QAction;

class AMuledPageWidget : public QWidget, public Ui::AMuledPage, public QECPacketSubscriber
{
    Q_OBJECT
public:
    explicit AMuledPageWidget(QWidget* parent = 0);
    virtual ~AMuledPageWidget();
    virtual void handlePacket(const QECPacket& p);
private Q_SLOTS:
    void slotConnected();
    void slotConnect();
    void slotLogin();
    void slotConnectEd2k();
    void slotDisconnectEd2k();
    void slotConnectKad();
    void slotDisconnectKad();
private:
    QAction* connectAct;
    QAction* loginAct;
    QAction* connectEd2kAct;
    QAction* disconnectEd2kAct;
    QAction* connectKadAct;
    QAction* disconnectKadAct;
};

#endif // AMULEDPAGE_H
