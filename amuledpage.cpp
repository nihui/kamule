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
#include <QIcon>
#include <QTimer>

#include <KAboutData>
#include <KPtyDevice>
#include <KPtyProcess>

#include "qecpacket.h"

#include "qecamule.h"

#include "kamulesettings.h"

AMuledPageWidget::AMuledPageWidget(QWidget* parent)
: QWidget(parent)
{
    setupUi(this);

    amuled = 0;

    // set action
    connectAct = new QAction(QIcon::fromTheme("view-refresh"), i18n("Connect"), this);
    connect(connectAct, SIGNAL(triggered()), this, SLOT(slotConnect()));
    loginAct = new QAction(QIcon::fromTheme("view-refresh"), i18n("Login"), this);
    connect(loginAct, SIGNAL(triggered()), this, SLOT(slotLogin()));
    connectEd2kAct = new QAction(QIcon::fromTheme("network-connect"), i18n("Connect Ed2k"), this);
    connect(connectEd2kAct, SIGNAL(triggered()), this, SLOT(slotConnectEd2k()));
    disconnectEd2kAct = new QAction(QIcon::fromTheme("network-disconnect"), i18n("Disconnect Ed2k"), this);
    connect(disconnectEd2kAct, SIGNAL(triggered()), this, SLOT(slotDisconnectEd2k()));
    connectKadAct = new QAction(QIcon::fromTheme("network-connect"), i18n("Connect Kad"), this);
    connect(connectKadAct, SIGNAL(triggered()), this, SLOT(slotConnectKad()));
    disconnectKadAct = new QAction(QIcon::fromTheme("network-disconnect"), i18n("Disconnect Kad"), this);
    connect(disconnectKadAct, SIGNAL(triggered()), this, SLOT(slotDisconnectKad()));

    // button actions
    m_connect->setDefaultAction(connectAct);
    m_login->setDefaultAction(loginAct);
    m_connectEd2k->setDefaultAction(connectEd2kAct);
    m_disconnectEd2k->setDefaultAction(disconnectEd2kAct);
    m_connectKad->setDefaultAction(connectKadAct);
    m_disconnectKad->setDefaultAction(disconnectKadAct);

    // load amuled settings
    KaMuleSettings::self()->readConfig();
    m_useExternalDaemon->setChecked(KaMuleSettings::self()->useExternalDaemon());
    m_hostEdit->setText(KaMuleSettings::self()->host());
    m_portEdit->setText(KaMuleSettings::self()->port());
    m_passwordEdit->setText(KaMuleSettings::self()->eCPassword());

    m_externalDaemonWidget->setVisible(KaMuleSettings::self()->useExternalDaemon());

    connect(QECaMule::self(), SIGNAL(connected()), this, SLOT(slotConnected()));
    connect(QECaMule::self(), SIGNAL(disconnected()), this, SLOT(slotDisconnected()));
    connect(QECaMule::self(), SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(slotError()));

    QECaMule::self()->addSubscriber(EC_OP_AUTH_SALT, this);
    QECaMule::self()->addSubscriber(EC_OP_AUTH_OK, this);

    if (!KaMuleSettings::self()->useExternalDaemon()) {
        // run internal amuled instance
        amuled = new KPtyProcess(this);
        amuled->setPtyChannels(KPtyProcess::AllChannels);
        connect(amuled->pty(), SIGNAL(readyRead()), this, SLOT(slotReadPtyOutput()));
        connect(amuled, SIGNAL(error(QProcess::ProcessError)), this, SLOT(slotProcessError(QProcess::ProcessError)));
        amuled->setProgram("amuled", QStringList() << "-e" << "-o");
        amuled->start();
    }
}

AMuledPageWidget::~AMuledPageWidget()
{
    if (amuled) {
        // terminate internal amuled instance
        amuled->terminate();

        // just block to wait
        amuled->waitForFinished();
    }

    // save amuled settings
    KaMuleSettings::self()->setUseExternalDaemon(m_useExternalDaemon->isChecked());
    KaMuleSettings::self()->setHost(m_hostEdit->text());
    KaMuleSettings::self()->setPort(m_portEdit->text());
    KaMuleSettings::self()->setECPassword(m_passwordEdit->text());
    KaMuleSettings::self()->writeConfig();

    QECaMule::self()->removeSubscriber(EC_OP_AUTH_SALT, this);
    QECaMule::self()->removeSubscriber(EC_OP_AUTH_OK, this);
}

void AMuledPageWidget::handlePacket(const QECPacket& p)
{
    if (p.opCode() == EC_OP_AUTH_OK) {
        m_loggerEdit->appendPlainText(i18n("[KaMule] Logined"));
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

void AMuledPageWidget::slotReadPtyOutput()
{
    QString data = QString::fromUtf8(amuled->pty()->readAll());
    QStringList lines = data.split("\n", QString::SkipEmptyParts);
    foreach (const QString& line, lines) {
        m_loggerEdit->appendPlainText(i18n("[Daemon] %1", line.trimmed()));
    }

    if (data.indexOf("Enter password for mule connection:") != -1) {
        // write ec password
        amuled->pty()->setEcho(false);
        amuled->pty()->write("qwerty\n");
        amuled->pty()->setEcho(true);

        // auto connect and login
        QTimer::singleShot(1000, this, SLOT(slotConnect()));
        QTimer::singleShot(1500, this, SLOT(slotLogin()));
    }
}

void AMuledPageWidget::slotProcessError(QProcess::ProcessError error)
{
    QString errorString;
    switch (error) {
        case QProcess::FailedToStart:
            errorString = i18n("Failed to start");
            break;
        case QProcess::Crashed:
            errorString = i18n("Crashed");
            break;
        case QProcess::Timedout:
            errorString = i18n("Timed out");
            break;
        case QProcess::WriteError:
            errorString = i18n("Write error");
            break;
        case QProcess::ReadError:
            errorString = i18n("Read error");
            break;
        case QProcess::UnknownError:
        default:
            errorString = i18n("Unknown error");
            break;
    }
    m_loggerEdit->appendPlainText(i18n("[KaMule] %1", errorString));
}

void AMuledPageWidget::slotConnected()
{
    m_loggerEdit->appendPlainText(i18n("[KaMule] Connected to amule daemon"));
}

void AMuledPageWidget::slotDisconnected()
{
    m_loggerEdit->appendPlainText(i18n("[KaMule] Disconnected to amule daemon"));
}

void AMuledPageWidget::slotError()
{
    m_loggerEdit->appendPlainText(i18n("[KaMule] %1", QECaMule::self()->errorString()));
}

void AMuledPageWidget::slotConnect()
{
    QECaMule::self()->connectToHost(m_hostEdit->text(), m_portEdit->text().toUShort());
}

void AMuledPageWidget::slotLogin()
{
    const KAboutData about = KAboutData::applicationData();

    QECPacket p(EC_OP_AUTH_REQ);
    p.addTag(EC_TAG_CLIENT_NAME, about.componentName());
    p.addTag(EC_TAG_CLIENT_VERSION, about.version());
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
