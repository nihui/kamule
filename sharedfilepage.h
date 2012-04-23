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

#ifndef SHAREDFILEPAGE_H
#define SHAREDFILEPAGE_H

#include "ui_sharedfilepage.h"

class QAction;
class QActionGroup;

class SharedFilePageWidget : public QWidget, public Ui::SharedFilePage
{
    Q_OBJECT
public:
    explicit SharedFilePageWidget(QWidget* parent = 0);
    virtual ~SharedFilePageWidget();
private Q_SLOTS:
    void resizeColumns();
    void slotCopyLink();
    void slotPriority(QAction* act);
    void showContextMenu(const QPoint& pos);
private:
    QAction* copyLinkAct;
    QAction* priorityAutoAct;
    QAction* priorityPowerShareAct;
    QAction* priorityVeryHighAct;
    QAction* priorityHighAct;
    QAction* priorityNormalAct;
    QAction* priorityLowAct;
    QAction* priorityVeryLowAct;
    QActionGroup* priorityAct;
};

#endif // SHAREDFILEPAGE_H
