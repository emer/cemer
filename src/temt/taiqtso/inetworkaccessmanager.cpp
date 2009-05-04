/*
 * Copyright 2008 Benjamin C. Meyer <ben@meyerhome.net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA  02110-1301  USA
 */

/****************************************************************************
**
** Copyright (C) 2007-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the demonstration applications of the Qt Toolkit.
**
** This file may be used under the terms of the GNU General Public
** License versions 2.0 or 3.0 as published by the Free Software
** Foundation and appearing in the files LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file.  Alternatively you may (at
** your option) use any later version of the GNU General Public
** License if such license has been publicly approved by Trolltech ASA
** (or its successors, if any) and the KDE Free Qt Foundation. In
** addition, as a special exception, Trolltech gives you certain
** additional rights. These rights are described in the Trolltech GPL
** Exception version 1.2, which can be found at
** http://www.trolltech.com/products/qt/gplexception/ and in the file
** GPL_EXCEPTION.txt in this package.
**
** Please review the following information to ensure GNU General
** Public Licensing requirements will be met:
** http://trolltech.com/products/qt/licenses/licensing/opensource/. If
** you are unsure which license is appropriate for your use, please
** review the following information:
** http://trolltech.com/products/qt/licenses/licensing/licensingoverview
** or contact the sales department at sales@trolltech.com.
**
** In addition, as a special exception, Trolltech, as the sole
** copyright holder for Qt Designer, grants users of the Qt/Eclipse
** Integration plug-in the right for the Qt/Eclipse Integration to
** link to functionality provided by Qt Designer and its related
** libraries.
**
** This file is provided "AS IS" with NO WARRANTY OF ANY KIND,
** INCLUDING THE WARRANTIES OF DESIGN, MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE. Trolltech reserves all rights not expressly
** granted herein.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#include "inetworkaccessmanager.h"

#include <ui_passworddialog.h>
//#include "ui_proxy.h"

#include <qdialog.h>
#include <QMainWindow>
#include <qmessagebox.h>
#include <qsettings.h>
#include <qstyle.h>
#include <qtextdocument.h>

#include <qauthenticator.h>
#include <qnetworkreply.h>
#include <qsslconfiguration.h>
#include <qsslerror.h>
#include <qdatetime.h>

// #if QT_VERSION >= 0x040500
// #include <qnetworkdiskcache.h>
// #include <qdesktopservices.h>
// #endif

iNetworkAccessManager::iNetworkAccessManager(QObject *parent)
  : QNetworkAccessManager(parent)
{
  m_main_win = NULL;		// must be set!
  connect(this, SIGNAL(authenticationRequired(QNetworkReply*, QAuthenticator*)),
	  SLOT(authenticationRequired(QNetworkReply*, QAuthenticator*)));
#ifndef QT_NO_OPENSSL
  connect(this, SIGNAL(sslErrors(QNetworkReply*, const QList<QSslError>&)),
	  SLOT(sslErrors(QNetworkReply*, const QList<QSslError>&)));
#endif
  loadSettings();

  // #if QT_VERSION >= 0x040500
  //     QNetworkDiskCache *diskCache = new QNetworkDiskCache(this);
  //     QString location = QDesktopServices::storageLocation(QDesktopServices::CacheLocation)
  //                             + QLatin1String("/browser");
  //     diskCache->setCacheDirectory(location);
  //     setCache(diskCache);
  // #endif
}

void iNetworkAccessManager::loadSettings()
{
  m_auth_saver.load();
  QSettings settings;
  //     settings.beginGroup(QLatin1String("proxy"));
  //     QNetworkProxy proxy;
  //     if (settings.value(QLatin1String("enabled"), false).toBool()) {
  //         int proxyType = settings.value(QLatin1String("type"), 0).toInt();
  //         if (proxyType == 0)
  //             proxy = QNetworkProxy::Socks5Proxy;
  //         else if (proxyType == 1)
  //             proxy = QNetworkProxy::HttpProxy;
  //         else { // 2
  //             proxy.setType(QNetworkProxy::HttpCachingProxy);
  // #if QT_VERSION >= 0x040500
  //             proxy.setCapabilities(QNetworkProxy::CachingCapability | QNetworkProxy::HostNameLookupCapability);
  // #endif
  //         }
  //         proxy.setHostName(settings.value(QLatin1String("hostName")).toString());
  //         proxy.setPort(settings.value(QLatin1String("port"), 1080).toInt());
  //         proxy.setUser(settings.value(QLatin1String("userName")).toString());
  //         proxy.setPassword(settings.value(QLatin1String("password")).toString());
  //     }
  // #if QT_VERSION >= 0x040500
  //     NetworkProxyFactory *proxyFactory = new NetworkProxyFactory;
  //     if (proxy.type() == QNetworkProxy::HttpCachingProxy) {
  //       proxyFactory->setHttpProxy(proxy);
  //       proxyFactory->setGlobalProxy(QNetworkProxy::DefaultProxy);
  //     } else {
  //       proxyFactory->setHttpProxy(QNetworkProxy::DefaultProxy);
  //       proxyFactory->setGlobalProxy(proxy);
  //     }
  //     setProxyFactory(proxyFactory);
  // #else
  //     setProxy(proxy);
  // #endif
  //     settings.endGroup();

#ifndef QT_NO_OPENSSL
  QSslConfiguration sslCfg = QSslConfiguration::defaultConfiguration();
  QList<QSslCertificate> ca_list = sslCfg.caCertificates();
  QList<QSslCertificate> ca_new = QSslCertificate::fromData(settings.value(QLatin1String("CaCertificates")).toByteArray());
  ca_list += ca_new;

  sslCfg.setCaCertificates(ca_list);
  QSslConfiguration::setDefaultConfiguration(sslCfg);
#endif
}

void iNetworkAccessManager::authenticationRequired(QNetworkReply *reply, QAuthenticator *auth)
{
//   QString realm = Qt::escape(auth->realm());
//   QString url_str = Qt::escape(reply->url().toString());
//   QString host = Qt::escape(reply->url().host());
  QString realm = auth->realm();
  QString url_str = reply->url().toString();
  QString host = reply->url().host();

  static QString last_realm;
  static QString last_host;

  bool same_place = (realm == last_realm && host == last_host);
  last_realm = realm;  last_host = host;

  QString user; QString password;
  bool found = m_auth_saver.findAuthRecord(user, password, realm, host);
  if(found && !same_place) {
    auth->setUser(user);
    auth->setPassword(password);
    return;
  }

  QDialog dialog(m_main_win);
  dialog.setWindowFlags(Qt::Sheet);

  Ui::PasswordDialog passwordDialog;
  passwordDialog.setupUi(&dialog);

  passwordDialog.iconLabel->setText(QString());
  passwordDialog.iconLabel->setPixmap(m_main_win->style()->standardIcon(QStyle::SP_MessageBoxQuestion, 0, m_main_win).pixmap(32, 32));

  QString introMessage = tr("<qt>Enter username and password for \"%1\" at %2</qt>");
  introMessage = introMessage.arg(realm).arg(url_str);
  passwordDialog.introLabel->setText(introMessage);
  passwordDialog.introLabel->setWordWrap(true);

  passwordDialog.userNameLineEdit->setText(user);
  passwordDialog.passwordLineEdit->setText(password);

  if (dialog.exec() == QDialog::Accepted) {
    user = passwordDialog.userNameLineEdit->text();
    password = passwordDialog.passwordLineEdit->text();
    auth->setUser(user);
    auth->setPassword(password);

    if(passwordDialog.saveFlag->isChecked()) {
      m_auth_saver.saveAuthRecord(user, password, realm, host);
    }
  }
}

// void iNetworkAccessManager::proxyAuthenticationRequired(const QNetworkProxy &proxy, QAuthenticator *auth)
// {
//     QDialog dialog(m_main_win);
//     dialog.setWindowFlags(Qt::Sheet);

//     Ui::ProxyDialog proxyDialog;
//     proxyDialog.setupUi(&dialog);

//     proxyDialog.iconLabel->setText(QString());
//     proxyDialog.iconLabel->setPixmap(m_main_win->style()->standardIcon(QStyle::SP_MessageBoxQuestion, 0, m_main_win).pixmap(32, 32));

//     QString introMessage = tr("<qt>Connect to proxy \"%1\" using:</qt>");
//     introMessage = introMessage.arg(Qt::escape(proxy.hostName()));
//     proxyDialog.introLabel->setText(introMessage);
//     proxyDialog.introLabel->setWordWrap(true);

//     if (dialog.exec() == QDialog::Accepted) {
//         auth->setUser(proxyDialog.userNameLineEdit->text());
//         auth->setPassword(proxyDialog.passwordLineEdit->text());
//     }
// }

#ifndef QT_NO_OPENSSL
static QString certToFormattedString(QSslCertificate cert)
{
  QString resultstring = QLatin1String("<p>");
  QStringList tmplist;

  resultstring += cert.subjectInfo(QSslCertificate::CommonName);

  resultstring += QString::fromLatin1("<br/>Issuer: %1")
    .arg(cert.issuerInfo(QSslCertificate::CommonName));

  resultstring += QString::fromLatin1("<br/>Not valid before: %1<br/>Valid Until: %2")
    .arg(cert.effectiveDate().toString(Qt::ISODate))
    .arg(cert.expiryDate().toString(Qt::ISODate));

  QMultiMap<QSsl::AlternateNameEntryType, QString> names = cert.alternateSubjectNames();
  if (names.count() > 0) {
    tmplist = names.values(QSsl::DnsEntry);
    resultstring += QLatin1String("<br/>Alternate Names:<ul><li>")
      + tmplist.join(QLatin1String("</li><li>"))
      + QLatin1String("</li><</ul>");
  }

  resultstring += QLatin1String("</p>");

  return resultstring;
}

void iNetworkAccessManager::sslErrors(QNetworkReply *reply, const QList<QSslError> &error)
{
  QSettings settings;
  QList<QSslCertificate> ca_merge = QSslCertificate::fromData(settings.value(QLatin1String("CaCertificates")).toByteArray());

  QList<QSslCertificate> ca_new;
  QStringList errorStrings;
  for (int i = 0; i < error.count(); ++i) {
    if (ca_merge.contains(error.at(i).certificate()))
      continue;
    errorStrings += error.at(i).errorString();
    if (!error.at(i).certificate().isNull()) {
      ca_new.append(error.at(i).certificate());
    }
  }
  if (errorStrings.isEmpty()) {
    reply->ignoreSslErrors();
    return;
  }

  QString errors = errorStrings.join(QLatin1String("</li><li>"));
  int ret = QMessageBox::warning(m_main_win,
				 QCoreApplication::applicationName() + tr(" - SSL Errors"),
				 tr("<qt>SSL Errors:"
				    "<br/><br/>for: <tt>%1</tt>"
				    "<ul><li>%2</li></ul>\n\n"
				    "Do you want to ignore these errors?</qt>").arg(reply->url().toString()).arg(errors),
				 QMessageBox::Yes | QMessageBox::No,
				 QMessageBox::No);

  if (ret == QMessageBox::Yes) {
    if (ca_new.count() > 0) {
      QStringList certinfos;
      for (int i = 0; i < ca_new.count(); ++i)
	certinfos += certToFormattedString(ca_new.at(i));
      ret = QMessageBox::question(m_main_win, QCoreApplication::applicationName(),
				  tr("<qt>Certifactes:<br/>"
				     "%1<br/>"
				     "Do you want to accept all these certificates?</qt>")
				  .arg(certinfos.join(QString())),
				  QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
      if (ret == QMessageBox::Yes) {
	ca_merge += ca_new;

	QSslConfiguration sslCfg = QSslConfiguration::defaultConfiguration();
	QList<QSslCertificate> ca_list = sslCfg.caCertificates();
	ca_list += ca_new;
	sslCfg.setCaCertificates(ca_list);
	QSslConfiguration::setDefaultConfiguration(sslCfg);
	reply->setSslConfiguration(sslCfg);

	QByteArray pems;
	for (int i = 0; i < ca_merge.count(); ++i)
	  pems += ca_merge.at(i).toPem() + '\n';
	settings.setValue(QLatin1String("CaCertificates"), pems);
      }
    }
    reply->ignoreSslErrors();
  }
}
#endif

/////////////////////////////////////////////////////////////////
// 		AuthSaver

#include <qdesktopservices.h>
#include <qfile.h>
#include <qdir.h>

// tmp:
#include <qdebug.h>

QDataStream &operator<<(QDataStream &out, const iAuthRecord& aurec) {
  out << aurec.realm << aurec.host << aurec.user << aurec.password;
  return out;
}

QDataStream &operator>>(QDataStream &in, iAuthRecord& aurec) {
  in >> aurec.realm >> aurec.host >> aurec.user >> aurec.password;
  return in;
}

iAuthRecord::iAuthRecord(QObject* parent) : QObject(parent) {
}

iAuthRecord::iAuthRecord(const iAuthRecord& cp) {
  realm = cp.realm;
  host = cp.host;
  user = cp.user;
  password = cp.password;
}

iAuthRecord::~iAuthRecord() {
}

iAuthSaver::iAuthSaver(QObject* parent) : QObject(parent) {
}

iAuthSaver::~iAuthSaver() {
}

void iAuthSaver::save() {
  QString directory = QDesktopServices::storageLocation(QDesktopServices::DataLocation);
  if (directory.isEmpty())
    directory = QDir::homePath() + QLatin1String("/.") + QCoreApplication::applicationName();
  if (!QFile::exists(directory)) {
    QDir dir;
    dir.mkpath(directory);
  }

  QString fnm = directory + "/misc_net.ini";

  QFile file(fnm);
  if(!file.open(QIODevice::WriteOnly)) return; // todo: err
  QDataStream stream(&file);
  stream << savedAuths;
  file.close();
}

void iAuthSaver::load() {
  QString directory = QDesktopServices::storageLocation(QDesktopServices::DataLocation);
  if (directory.isEmpty())
    directory = QDir::homePath() + QLatin1String("/.") + QCoreApplication::applicationName();
  if (!QFile::exists(directory)) {
    QDir dir;
    dir.mkpath(directory);
  }

  QString fnm = directory + "/misc_net.ini";

  QFile file(fnm);
  if(!file.open(QIODevice::ReadOnly)) return; // todo: err
  QDataStream stream(&file);
  stream >> savedAuths;
  file.close();
}

bool iAuthSaver::findAuthRecord(QString& user, QString& password, const QString& realm,
				const QString& host) const {
  user = "";
  password = "";
  for(int i=0;i<savedAuths.size(); i++) {
    const iAuthRecord& aurec = savedAuths.at(i);
    if(aurec.realm == realm && aurec.host == host) {
      user = aurec.user;
      password = aurec.password;
      return true;
    }
  }
  return false;
}

bool iAuthSaver::saveAuthRecord(const QString& user, const QString& password,
				const QString& realm, const QString& host) {
  bool rval = false;
  for(int i=0;i<savedAuths.size(); i++) {
    iAuthRecord& aurec = savedAuths[i];
    if(aurec.realm == realm && aurec.host == host) {
      aurec.user = user;
      aurec.password = password;
      rval = true;
    }
  }
  if(!rval) {
    iAuthRecord aurec;
    aurec.realm = realm;
    aurec.host = host;
    aurec.user = user;
    aurec.password = password;
    savedAuths.append(aurec);
  }
  save();
  return rval;
}
