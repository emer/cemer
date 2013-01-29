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

#ifndef iCookieJar_h
#define iCookieJar_h 1

#include <QNetworkCookieJar>
#include <QStringList>

class iNetworkCookieJarPrivate;
class iNetworkCookieJar : public QNetworkCookieJar {
  Q_OBJECT
  public:
  iNetworkCookieJar(QObject *parent = 0);
  ~iNetworkCookieJar();

  virtual QList<QNetworkCookie> cookiesForUrl(const QUrl & url) const;
  virtual bool setCookiesFromUrl(const QList<QNetworkCookie> &cookieList, const QUrl &url);

protected:
  QByteArray saveState() const;
  bool restoreState(const QByteArray &state);
  void endSession();

  QList<QNetworkCookie> allCookies() const;
  void setAllCookies(const QList<QNetworkCookie> &cookieList);
  void setSecondLevelDomains(const QStringList &secondLevelDomains);

private:
  iNetworkCookieJarPrivate *d;
};

class AutoSaver;
class iCookieJar : public iNetworkCookieJar {
  friend class CookieModel;
  Q_OBJECT
  Q_PROPERTY(AcceptPolicy acceptPolicy READ acceptPolicy WRITE setAcceptPolicy)
    Q_PROPERTY(KeepPolicy keepPolicy READ keepPolicy WRITE setKeepPolicy)
    Q_PROPERTY(QStringList blockedCookies READ blockedCookies WRITE setBlockedCookies)
    Q_PROPERTY(QStringList allowedCookies READ allowedCookies WRITE setAllowedCookies)
    Q_PROPERTY(QStringList allowForSessionCookies READ allowForSessionCookies WRITE setAllowForSessionCookies)
    Q_ENUMS(KeepPolicy)
    Q_ENUMS(AcceptPolicy)

    signals:
    void cookiesChanged();

public:
  enum AcceptPolicy {
    AcceptAlways,
    AcceptNever,
    AcceptOnlyFromSitesNavigatedTo
  };

  enum KeepPolicy {
    KeepUntilExpire,
    KeepUntilExit,
    KeepUntilTimeLimit
  };

  iCookieJar(QObject *parent = 0);
  ~iCookieJar();

  QList<QNetworkCookie> cookiesForUrl(const QUrl &url) const;
  bool setCookiesFromUrl(const QList<QNetworkCookie> &cookieList, const QUrl &url);

  AcceptPolicy acceptPolicy() const;
  void setAcceptPolicy(AcceptPolicy policy);

  KeepPolicy keepPolicy() const;
  void setKeepPolicy(KeepPolicy policy);

  QStringList blockedCookies() const;
  QStringList allowedCookies() const;
  QStringList allowForSessionCookies() const;

  void setBlockedCookies(const QStringList &list);
  void setAllowedCookies(const QStringList &list);
  void setAllowForSessionCookies(const QStringList &list);

public slots:
  void clear();
  void loadSettings();

private slots:
  void save();

private:
  void purgeOldCookies();
  void load();
  bool m_loaded;
  AutoSaver *m_saveTimer;

  AcceptPolicy m_acceptCookies;
  KeepPolicy m_keepCookies;

  QStringList m_exceptions_block;
  QStringList m_exceptions_allow;
  QStringList m_exceptions_allowForSession;
};

#endif // iCookieJar_h
