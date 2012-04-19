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

#ifndef NETWORKPAGE_H
#define NETWORKPAGE_H

#include "ui_networkpage.h"

class QAction;

class NetworkPageWidget : public QWidget, public Ui::NetworkPage
{
    Q_OBJECT
public:
    explicit NetworkPageWidget(QWidget* parent = 0);
    virtual ~NetworkPageWidget();
private Q_SLOTS:
    void resizeColumns();
    void slotUpdateFromUrl();
    void slotAddServer();
    void slotConnect();
    void slotDisconnect();
    void slotRemove();
    void showContextMenu(const QPoint& pos);
private:
    QAction* updateFromUrlAct;
    QAction* addServerAct;
    QAction* connectAct;
    QAction* disconnectAct;
    QAction* removeAct;
};

#endif // NETWORKPAGE_H
