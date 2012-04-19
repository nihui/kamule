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

#include "preferencemodel.h"

#include <KIcon>
#include <KLocale>

#include "preference/generalpage.h"
#include "preference/connectionpage.h"
#include "preference/serverpage.h"
#include "preference/filepage.h"
#include "preference/directorypage.h"
#include "preference/coretweakpage.h"

PreferenceModel::PreferenceModel()
{
    generalpage = new GeneralPageWidget;
    connectionpage = new ConnectionPageWidget;
    serverpage = new ServerPageWidget;
    filepage = new FilePageWidget;
    directorypage = new DirectoryPageWidget;
    coretweakpage = new CoreTweakPageWidget;
}

PreferenceModel::~PreferenceModel()
{
}

int PreferenceModel::columnCount(const QModelIndex& /*parent*/) const
{
    return 1;
}

QVariant PreferenceModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role == Qt::DisplayRole) {
        switch (index.row()) {
            case 0: return i18n("General");
            case 1: return i18n("Connection");
            case 2: return i18n("Server");
            case 3: return i18n("File");
            case 4: return i18n("Directory");
            case 5: return i18n("Core tweak");
        }
    }

    if (role == Qt::DecorationRole) {
        switch (index.row()) {
            case 0: return KIcon("configure");
            case 1: return KIcon("network-connect");
            case 2: return KIcon("network-server");
            case 3: return KIcon("document-properties");
            case 4: return KIcon("document-open-folder");
            case 5: return KIcon("edit-bomb");
        }
    }

    if (role == KPageModel::HeaderRole) {
        switch (index.row()) {
            case 0: return i18n("General");
            case 1: return i18n("Connection");
            case 2: return i18n("Server");
            case 3: return i18n("File");
            case 4: return i18n("Directory");
            case 5: return i18n("Core tweak");
        }
    }

    if (role == KPageModel::WidgetRole) {
        switch (index.row()) {
            case 0: return QVariant::fromValue(generalpage);
            case 1: return QVariant::fromValue(connectionpage);
            case 2: return QVariant::fromValue(serverpage);
            case 3: return QVariant::fromValue(filepage);
            case 4: return QVariant::fromValue(directorypage);
            case 5: return QVariant::fromValue(coretweakpage);
        }
    }

    return QVariant();
}

QModelIndex PreferenceModel::index(int row, int column, const QModelIndex& parent) const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    if (!parent.isValid()) {
        return createIndex(row, column);
    }

    return QModelIndex();
}

QModelIndex PreferenceModel::parent(const QModelIndex& /*index*/) const
{
    return QModelIndex();
}

int PreferenceModel::rowCount(const QModelIndex& parent) const
{
    if (!parent.isValid()) {
        return 6;
    }

    return 0;
}

