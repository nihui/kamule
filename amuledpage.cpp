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

#include "amuledpage.h"

#include <QAction>
#include <QCryptographicHash>

#include <KAboutData>
#include <KComponentData>
#include <KGlobal>

#include "qecpacket.h"

#include "qecamule.h"

#include "kamulesettings.h"

AMuledPageWidget::AMuledPageWidget(QWidget* parent)
: QWidget(parent)
{
    setupUi(this);

    // set action
    connectAct = new QAction(KIcon("view-refresh"), i18n("Connect"), this);
    connect(connectAct, SIGNAL(triggered()), this, SLOT(slotConnect()));
    loginAct = new QAction(KIcon("view-refresh"), i18n("Login"), this);
    connect(loginAct, SIGNAL(triggered()), this, SLOT(slotLogin()));
    connectEd2kAct = new QAction(KIcon("network-connect"), i18n("Connect Ed2k"), this);
    connect(connectEd2kAct, SIGNAL(triggered()), this, SLOT(slotConnectEd2k()));
    disconnectEd2kAct = new QAction(KIcon("network-disconnect"), i18n("Disconnect Ed2k"), this);
    connect(disconnectEd2kAct, SIGNAL(triggered()), this, SLOT(slotDisconnectEd2k()));
    connectKadAct = new QAction(KIcon("network-connect"), i18n("Connect Kad"), this);
    connect(connectKadAct, SIGNAL(triggered()), this, SLOT(slotConnectKad()));
    disconnectKadAct = new QAction(KIcon("network-disconnect"), i18n("Disconnect Kad"), this);
    connect(disconnectKadAct, SIGNAL(triggered()), this, SLOT(slotDisconnectKad()));

    // button actions
    m_connect->setDefaultAction(connectAct);
    m_login->setDefaultAction(loginAct);
    m_connectEd2k->setDefaultAction(connectEd2kAct);
    m_disconnectEd2k->setDefaultAction(disconnectEd2kAct);
    m_connectKad->setDefaultAction(connectKadAct);
    m_disconnectKad->setDefaultAction(disconnectKadAct);

    // load amuled settings
    m_hostEdit->setText(KaMuleSettings::self()->host());
    m_portEdit->setText(KaMuleSettings::self()->port());
    m_passwordEdit->setText(KaMuleSettings::self()->eCPassword());

    connect(QECaMule::self(), SIGNAL(connected()), this, SLOT(slotConnected()));
    connect(QECaMule::self(), SIGNAL(disconnected()), this, SLOT(slotDisconnected()));
    connect(QECaMule::self(), SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(slotError()));

    QECaMule::self()->addSubscriber(EC_OP_AUTH_SALT, this);
    QECaMule::self()->addSubscriber(EC_OP_AUTH_OK, this);
}

AMuledPageWidget::~AMuledPageWidget()
{
    // save amuled settings
    KaMuleSettings::self()->setHost(m_hostEdit->text());
    KaMuleSettings::self()->setPort(m_portEdit->text());
    KaMuleSettings::self()->setECPassword(m_passwordEdit->text());

    QECaMule::self()->removeSubscriber(EC_OP_AUTH_SALT, this);
    QECaMule::self()->removeSubscriber(EC_OP_AUTH_OK, this);
}

void AMuledPageWidget::handlePacket(const QECPacket& p)
{
    if (p.opCode() == EC_OP_AUTH_OK) {
        m_loggerEdit->append(i18n("[Success] login"));
        return;
    }

    const QECTag& tag_passwd_salt = p.findTag(EC_TAG_PASSWD_SALT);
    if (tag_passwd_salt.isNull()) {
        return;
    }

    QByteArray salt = tag_passwd_salt.toMd5().toHex().toUpper();

    QByteArray ph = QCryptographicHash::hash(m_passwordEdit->text().toUtf8(), QCryptographicHash::Md5).toHex();
    QByteArray sh = QCryptographicHash::hash(salt, QCryptographicHash::Md5).toHex();
    QByteArray hash = QCryptographicHash::hash(ph + sh, QCryptographicHash::Md5);

    QECPacket p2(EC_OP_AUTH_PASSWD);
    p2.addTag(EC_TAG_PASSWD_HASH, hash);
    QECaMule::self()->sendPacket(p2);
}

void AMuledPageWidget::slotConnected()
{
    m_loggerEdit->append(i18n("[Success] connect to amule daemon"));
}

void AMuledPageWidget::slotDisconnected()
{
    m_loggerEdit->append(i18n("[Warning] disconnect to amule daemon"));
}

void AMuledPageWidget::slotError()
{
    m_loggerEdit->append(i18n("[Error] %1", QECaMule::self()->errorString()));
}

void AMuledPageWidget::slotConnect()
{
    QECaMule::self()->connectToHost(m_hostEdit->text(), m_portEdit->text().toUShort());
}

void AMuledPageWidget::slotLogin()
{
    const KComponentData& kcd = KGlobal::mainComponent();
    const KAboutData* about = kcd.aboutData();

    QECPacket p(EC_OP_AUTH_REQ);
    p.addTag(EC_TAG_CLIENT_NAME, about->appName());
    p.addTag(EC_TAG_CLIENT_VERSION, about->version());
    p.addTag(EC_TAG_PROTOCOL_VERSION, (quint64)EC_CURRENT_PROTOCOL_VERSION);
//     p.addTag(EC_TAG_CAN_NOTIFY);

    QECaMule::self()->sendPacket(p);
}

void AMuledPageWidget::slotConnectEd2k()
{
    QECPacket p(EC_OP_SERVER_CONNECT);
    QECaMule::self()->sendPacket(p);
}

void AMuledPageWidget::slotDisconnectEd2k()
{
    QECPacket p(EC_OP_SERVER_DISCONNECT);
    QECaMule::self()->sendPacket(p);
}

void AMuledPageWidget::slotConnectKad()
{
    QECPacket p(EC_OP_KAD_START);
    QECaMule::self()->sendPacket(p);
}

void AMuledPageWidget::slotDisconnectKad()
{
    QECPacket p(EC_OP_KAD_STOP);
    QECaMule::self()->sendPacket(p);
}
