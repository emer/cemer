// Copyright, 1995-2013, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of The Emergent Toolkit
//
//   This library is free software; you can redistribute it and/or
//   modify it under the terms of the GNU Lesser General Public
//   License as published by the Free Software Foundation; either
//   version 2.1 of the License, or (at your option) any later version.
//
//   This library is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//   Lesser General Public License for more details.

#ifndef iNetworkAccessManager_h
#define iNetworkAccessManager_h 1

#include <qnetworkaccessmanager.h>

class QDataStream;
class QMainWindow;

class iAuthRecord : public QObject {
  // #IGNORE one authentication record
  Q_OBJECT
public:
  iAuthRecord(QObject* parent = NULL);
  iAuthRecord(const iAuthRecord& cp);
  ~iAuthRecord();

  iAuthRecord& operator=(const iAuthRecord& cp) {
    realm = cp.realm;  host = cp.host; user = cp.user; password = cp.password;
    return *this;
  }

  QString  realm;
  QString  host;
  QString  user;
  QString  password;

  friend QDataStream &operator<<(QDataStream &, const iAuthRecord&);
  friend QDataStream &operator>>(QDataStream &, iAuthRecord&);
};

class iAuthSaver : public QObject {
  // #IGNORE saves authentication data
  Q_OBJECT
public:
  iAuthSaver(QObject* parent = NULL);
  ~iAuthSaver();

  QList<iAuthRecord>  savedAuths;
  QMainWindow*  m_main_win;

  bool  findAuthRecord(QString& user, QString& password, const QString& realm,
                       const QString& host) const;
  // find authentication record based on realm and host, returning user and password into args -- returns false if not found, true if found
  bool  saveAuthRecord(const QString& user, const QString& password, const QString& realm,
                       const QString& host);
  // save given authentication record, and save to disk too -- returns true if updating existing record, false if new record

public slots:
  void  load();                 // save to persistent file storage
  void  save();                 // load from persistent file storage

private slots:
  void provideAuthentication(QNetworkReply *reply, QAuthenticator *auth);
  //     void proxyAuthenticationRequired(const QNetworkProxy &proxy, QAuthenticator *auth);

private:
  QString       m_last_realm;
  QString       m_last_host;
  QDateTime     m_last_time;
};

class iNetworkAccessManager : public QNetworkAccessManager {
  Q_OBJECT
public:
  iNetworkAccessManager(QObject *parent = 0);

  void  setMainWindow(QMainWindow* mw);

protected:
  QMainWindow*  m_main_win;
  iAuthSaver    m_auth_saver;

public slots:
  void loadSettings();

private slots:
#ifndef QT_NO_OPENSSL
  void handleSslErrors(QNetworkReply *reply, const QList<QSslError> &error);
#endif
};

bool getUsernamePassword(
  QString &username,
  QString &password,
  QString message = QString(),
  bool *saveFlag = 0,
  QMainWindow *mw = 0,
  bool usernameOnly = false
);

#endif // iNetworkAccessManager_h
