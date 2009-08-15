/*
  Copyright (C) 2009 Torch Mobile Inc. http://www.torchmobile.com/
*/

/* ***** BEGIN LICENSE BLOCK *****
 * Version: MPL 1.1/GPL 2.0/LGPL 2.1
 *
 * The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Original Code is Torch Mobile Inc. (http://www.torchmobile.com/) code
 *
 * The Initial Developer of the Original Code is:
 *   Benjamin Meyer (benjamin.meyer@torchmobile.com)
 *
 * Alternatively, the contents of this file may be used under the terms of
 * either the GNU General Public License Version 2 or later (the "GPL"), or
 * the GNU Lesser General Public License Version 2.1 or later (the "LGPL"),
 * in which case the provisions of the GPL or the LGPL are applicable instead
 * of those above. If you wish to allow use of your version of this file only
 * under the terms of either the GPL or the LGPL, and not to allow others to
 * use your version of this file under the terms of the MPL, indicate your
 * decision by deleting the provisions above and replace them with the notice
 * and other provisions required by the GPL or the LGPL. If you do not delete
 * the provisions above, a recipient may use your version of this file under
 * the terms of any one of the MPL, the GPL or the LGPL.
 *
 * ***** END LICENSE BLOCK ***** */

#include "icookiejar.h"
#include <qstringlist.h>

// Updated from https://wiki.mozilla.org/TLD_List#External_Links
// To set a custom list use NetworkCookieJar::setSecondLevelDomains()
static const char *const twoLevelDomains[] = {
  "ao",
  "ar",
  "arpa",
  "bd",
  "bn",
  "br",
  "co",
  "cr",
  "cy",
  "do",
  "eg",
  "et",
  "fj",
  "fk",
  "gh",
  "gn",
  "gu",
  "id",
  "il",
  "jm",
  "ke",
  "kh",
  "ki",
  "kw",
  "kz",
  "lb",
  "lc",
  "lr",
  "ls",
  "ml",
  "mm",
  "mv",
  "mw",
  "mx",
  "my",
  "ng",
  "ni",
  "np",
  "nz",
  "om",
  "pa",
  "pe",
  "pg",
  "pw",
  "py",
  "qa",
  "sa",
  "sb",
  "sv",
  "sy",
  "th",
  "tn",
  "tz",
  "uk",
  "uy",
  "va",
  "ve",
  "ye",
  "yu",
  "za",
  "zm",
  "zw",
  0
};

#if defined(TRIE_DEBUG)
#include <qdebug.h>
#endif

// #define COOKIE_DEBUG 1

/*
  A Trie tree (prefix tree) where the lookup takes m in the worst case.

  The key is stored in *reverse* order

  Example:
  Keys: x,a y,a

  Trie:
  a
  | \
  x  y
*/

template<class T>
class Trie {
public:
  Trie();
  ~Trie();

  void clear();
  void insert(const QStringList &key, const T &value);
  bool remove(const QStringList &key, const T &value);
  QList<T> find(const QStringList &key) const;
  QList<T> all() const;

  inline bool contains(const QStringList &key) const;
  inline bool isEmpty() const { return children.isEmpty() && values.isEmpty(); }

private:
  const Trie<T>* walkTo(const QStringList &key) const;
  Trie<T>* walkTo(const QStringList &key, bool create = false);

  template<class T1> friend QDataStream &operator<<(QDataStream &, const Trie<T1>&);
  template<class T1> friend QDataStream &operator>>(QDataStream &, Trie<T1>&);

  QList<T> values;
  QStringList childrenKeys;
  QList<Trie<T> > children;
};

template<class T>
Trie<T>::Trie() {
}

template<class T>
Trie<T>::~Trie() {
}

template<class T>
void Trie<T>::clear() {
#if defined(TRIE_DEBUG)
  qDebug() << "Trie::" << __FUNCTION__;
#endif
  values.clear();
  childrenKeys.clear();
  children.clear();
}

template<class T>
bool Trie<T>::contains(const QStringList &key) const {
  return walkTo(key);
}

template<class T>
void Trie<T>::insert(const QStringList &key, const T &value) {
#if defined(TRIE_DEBUG)
  qDebug() << "Trie::" << __FUNCTION__ << key << value;
#endif
  Trie<T> *node = walkTo(key, true);
  if (node)
    node->values.append(value);
}

template<class T>
bool Trie<T>::remove(const QStringList &key, const T &value) {
#if defined(TRIE_DEBUG)
  qDebug() << "Trie::" << __FUNCTION__ << key << value;
#endif
  Trie<T> *node = walkTo(key, true);
  if (node) {
    bool removed = node->values.removeOne(value);
    if (!removed)
      return false;

    // A faster implementation of removing nodes up the tree
    // can be created if profile shows this to be slow
    QStringList subKey = key;
    while (node->values.isEmpty()
	   && node->children.isEmpty()
	   && !subKey.isEmpty()) {
      QString currentLevelKey = subKey.first();
      QStringList parentKey = subKey.mid(1);
      Trie<T> *parent = walkTo(parentKey, false);
      Q_ASSERT(parent);
      QStringList::iterator iterator;
      iterator = qBinaryFind(parent->childrenKeys.begin(),
			     parent->childrenKeys.end(),
			     currentLevelKey);
      Q_ASSERT(iterator != parent->childrenKeys.end());
      int index = iterator - parent->childrenKeys.begin();
      parent->children.removeAt(index);
      parent->childrenKeys.removeAt(index);

      node = parent;
      subKey = parentKey;
    }
    return removed;
  }
  return false;
}

template<class T>
QList<T> Trie<T>::find(const QStringList &key) const {
#if defined(TRIE_DEBUG)
  qDebug() << "Trie::" << __FUNCTION__ << key;
#endif
  const Trie<T> *node = walkTo(key);
  if (node)
    return node->values;
  return QList<T>();
}

template<class T>
QList<T> Trie<T>::all() const {
#if defined(TRIE_DEBUG)
  qDebug() << "Trie::" << __FUNCTION__;
#endif
  QList<T> all = values;
  for (int i = 0; i < children.count(); ++i)
    all += children[i].all();
  return all;
}

template<class T>
QDataStream &operator<<(QDataStream &out, const Trie<T>&trie) {
  out << trie.values;
  out << trie.childrenKeys;
  out << trie.children;
  Q_ASSERT(trie.childrenKeys.count() == trie.children.count());
  return out;
}

template<class T>
QDataStream &operator>>(QDataStream &in, Trie<T> &trie) {
  trie.clear();
  in >> trie.values;
  in >> trie.childrenKeys;
  in >> trie.children;
  Q_ASSERT(trie.childrenKeys.count() == trie.children.count());
  return in;
}

// Very fast const walk
template<class T>
const Trie<T>* Trie<T>::walkTo(const QStringList &key) const {
  const Trie<T> *node = this;
  QStringList::const_iterator childIterator;
  QStringList::const_iterator begin, end;

  int depth = key.count() - 1;
  while (depth >= 0) {
    const QString currentLevelKey = key.at(depth--);
    begin = node->childrenKeys.constBegin();
    end = node->childrenKeys.constEnd();
    childIterator = qBinaryFind(begin, end, currentLevelKey);
    if (childIterator == end)
      return 0;
    node = &node->children.at(childIterator - begin);
  }
  return node;
}

template<class T>
Trie<T>* Trie<T>::walkTo(const QStringList &key, bool create) {
  QStringList::iterator iterator;
  Trie<T> *node = this;
  QStringList::iterator begin, end;
  int depth = key.count() - 1;
  while (depth >= 0) {
    const QString currentLevelKey = key.at(depth--);
    begin = node->childrenKeys.begin();
    end = node->childrenKeys.end();
    iterator = qBinaryFind(begin, end, currentLevelKey);
#if defined(TRIE_DEBUG)
    qDebug() << "\t" << node << key << currentLevelKey << node->childrenKeys;
#endif
    int index = -1;
    if (iterator == end) {
      if (!create)
	return 0;
      iterator = qLowerBound(begin,
			     end,
			     currentLevelKey);
      index = iterator - begin;
      node->childrenKeys.insert(iterator, currentLevelKey);
      node->children.insert(index, Trie<T>());
    } else {
      index = iterator - begin;
    }
    Q_ASSERT(index >= 0 && index < node->children.count());
    node = &node->children[index];
  }
  return node;
}

QT_BEGIN_NAMESPACE
QDataStream &operator<<(QDataStream &stream, const QNetworkCookie &cookie)
{
  stream << cookie.toRawForm();
  return stream;
}

QDataStream &operator>>(QDataStream &stream, QNetworkCookie &cookie)
{
  QByteArray value;
  stream >> value;
  QList<QNetworkCookie> newCookies = QNetworkCookie::parseCookies(value);
  if (!newCookies.isEmpty())
    cookie = newCookies.first();
  return stream;
}
QT_END_NAMESPACE

class iNetworkCookieJarPrivate {
public:
  iNetworkCookieJarPrivate()
    : setSecondLevelDomain(false)
  {}

  Trie<QNetworkCookie> tree;
  mutable bool setSecondLevelDomain;
  mutable QStringList secondLevelDomains;

  bool matchesBlacklist(const QString &string) const;
  bool matchingDomain(const QNetworkCookie &cookie, const QUrl &url) const;
  QString urlPath(const QUrl &url) const;
  bool matchingPath(const QNetworkCookie &cookie, const QString &urlPath) const;
};


//#define NETWORKCOOKIEJAR_DEBUG

#ifndef QT_NO_DEBUG
// ^ Prevent being left on in a released product by accident
// qDebug any cookies that are rejected for further inspection
#define NETWORKCOOKIEJAR_LOGREJECTEDCOOKIES
#include <qdebug.h>
#endif

#include <qurl.h>
#include <qdatetime.h>

#if defined(NETWORKCOOKIEJAR_DEBUG)
#include <qdebug.h>
#endif


iNetworkCookieJar::iNetworkCookieJar(QObject *parent)
  : QNetworkCookieJar(parent)
{
  d = new iNetworkCookieJarPrivate;
}

iNetworkCookieJar::~iNetworkCookieJar()
{
  delete d;
}

static QStringList splitHost(const QString &host) {
  QStringList parts = host.split(QLatin1Char('.'), QString::KeepEmptyParts);
  // Remove empty components that are on the start and end
  while (!parts.isEmpty() && parts.last().isEmpty())
    parts.removeLast();
  while (!parts.isEmpty() && parts.first().isEmpty())
    parts.removeFirst();
  return parts;
}

inline static bool shorterPaths(const QNetworkCookie &c1, const QNetworkCookie &c2)
{
  return c2.path().length() < c1.path().length();
}

QList<QNetworkCookie> iNetworkCookieJar::cookiesForUrl(const QUrl &url) const
{
#if defined(COOKIE_DEBUG)
  qDebug() << "iNetworkCookieJar::" << __FUNCTION__ << url;
#endif
  // Generate split host
  QString host = url.host();
  if (url.scheme().toLower() == QLatin1String("file"))
    host = QLatin1String("localhost");
  QStringList urlHost = splitHost(host);

  // Get all the cookies for url
  QList<QNetworkCookie> cookies = d->tree.find(urlHost);
  if (urlHost.count() > 2) {
    int top = 2;
    if (d->matchesBlacklist(urlHost.last()))
      top = 3;

    urlHost.removeFirst();
    while (urlHost.count() >= top) {
      cookies += d->tree.find(urlHost);
      urlHost.removeFirst();
    }
  }

  // Prevent doing anything expensive in the common case where
  // there are no cookies to check
  if (cookies.isEmpty())
    return cookies;

  QDateTime now = QDateTime::currentDateTime().toTimeSpec(Qt::UTC);
  const QString urlPath = d->urlPath(url);
  const bool isSecure = url.scheme().toLower() == QLatin1String("https");
  QList<QNetworkCookie>::iterator i = cookies.begin();
  for (; i != cookies.end();) {
    if (!d->matchingPath(*i, urlPath)) {
#if defined(COOKIE_DEBUG)
      qDebug() << __FUNCTION__ << "Ignoring cookie, path does not match" << *i << urlPath;
#endif
      i = cookies.erase(i);
      continue;
    }
    if (!isSecure && i->isSecure()) {
      i = cookies.erase(i);
#if defined(COOKIE_DEBUG)
      qDebug() << __FUNCTION__ << "Ignoring cookie, security mismatch"
	       << *i << !isSecure;
#endif
      continue;
    }
    if (!i->isSessionCookie() && now > i->expirationDate()) {
      // remove now (expensive short term) because there will
      // probably be many more cookiesForUrl calls for this host
      d->tree.remove(splitHost(i->domain()), *i);
#if defined(COOKIE_DEBUG)
      qDebug() << __FUNCTION__ << "Ignoring cookie, expiration issue"
	       << *i << now;
#endif
      i = cookies.erase(i);
      continue;
    }
    ++i;
  }

  // shorter paths should go first
  qSort(cookies.begin(), cookies.end(), shorterPaths);
#if defined(COOKIE_DEBUG)
  qDebug() << "iNetworkCookieJar::" << __FUNCTION__ << "returning" << cookies.count();
  qDebug() << cookies;
#endif
  return cookies;
}

static const qint32 iNetworkCookieJarMagic = 0xae;

QByteArray iNetworkCookieJar::saveState () const
{
  int version = 1;
  QByteArray data;
  QDataStream stream(&data, (QIODevice::OpenMode)QIODevice::WriteOnly);

  stream << qint32(iNetworkCookieJarMagic);
  stream << qint32(version);
  stream << d->tree;
  return data;
}

bool iNetworkCookieJar::restoreState(const QByteArray &state)
{
  int version = 1;
  QByteArray sd = state;
  QDataStream stream(&sd, (QIODevice::OpenMode)QIODevice::ReadOnly);
  if (stream.atEnd())
    return false;
  qint32 marker;
  qint32 v;
  stream >> marker;
  stream >> v;
  if (marker != iNetworkCookieJarMagic || v != version)
    return false;
  stream >> d->tree;
  return true;
}

/*!
  Remove any session cookies or cookies that have expired.
*/
void iNetworkCookieJar::endSession()
{
  const QList<QNetworkCookie> cookies = d->tree.all();
  QDateTime now = QDateTime::currentDateTime().toTimeSpec(Qt::UTC);
  QList<QNetworkCookie>::const_iterator i = cookies.constBegin();
  for (; i != cookies.constEnd();) {
    if (i->isSessionCookie()
	|| (!i->isSessionCookie() && now > i->expirationDate())) {
      d->tree.remove(splitHost(i->domain()), *i);
    }
    ++i;
  }
}

bool iNetworkCookieJar::setCookiesFromUrl(const QList<QNetworkCookie> &cookieList, const QUrl &url)
{
#if defined(COOKIE_DEBUG)
  qDebug() << "iNetworkCookieJar::" << __FUNCTION__ << url;
  qDebug() << cookieList;
#endif
  QDateTime now = QDateTime::currentDateTime().toTimeSpec(Qt::UTC);
  bool changed = false;
  QString fullUrlPath = url.path();
  QString defaultPath = fullUrlPath.mid(0, fullUrlPath.lastIndexOf(QLatin1Char('/')) + 1);
  if (defaultPath.isEmpty())
    defaultPath = QLatin1Char('/');

  QString urlPath = d->urlPath(url);
  foreach (QNetworkCookie cookie, cookieList) {
    bool alreadyDead = !cookie.isSessionCookie() && cookie.expirationDate() < now;

    if (cookie.path().isEmpty()) {
      cookie.setPath(defaultPath);
    } else if (!d->matchingPath(cookie, urlPath)) {
#ifdef COOKIE_DEBUG
      qDebug() << "iNetworkCookieJar::" << __FUNCTION__
	       << "Blocked cookie because: path doesn't match: " << cookie << url;
#endif
      continue;
    }

    if (cookie.domain().isEmpty()) {
      QString host = url.host().toLower();
      if (host.isEmpty())
	continue;
      cookie.setDomain(host);
    } else if (!d->matchingDomain(cookie, url)) {
#ifdef COOKIE_DEBUG
      qDebug() << "iNetworkCookieJar::" << __FUNCTION__
	       << "Blocked cookie because: domain doesn't match: " << cookie << url;
#endif
      continue;
    }

    // replace/remove existing cookies
    QString domain = cookie.domain();
    Q_ASSERT(!domain.isEmpty());
    QStringList urlHost = splitHost(domain);

    QList<QNetworkCookie> cookies = d->tree.find(urlHost);
    QList<QNetworkCookie>::const_iterator it = cookies.constBegin();
    for (; it != cookies.constEnd(); ++it) {
      if (cookie.name() == it->name() &&
	  cookie.domain() == it->domain() &&
	  cookie.path() == it->path()) {
	d->tree.remove(urlHost, *it);
	break;
      }
    }

    if (alreadyDead)
      continue;

    changed = true;
    d->tree.insert(urlHost, cookie);
  }

  return changed;
}

QList<QNetworkCookie> iNetworkCookieJar::allCookies() const
{
#if defined(COOKIE_DEBUG)
  qDebug() << "iNetworkCookieJar::" << __FUNCTION__;
#endif
  return d->tree.all();
}

void iNetworkCookieJar::setAllCookies(const QList<QNetworkCookie> &cookieList)
{
#if defined(COOKIE_DEBUG)
  qDebug() << "iNetworkCookieJar::" << __FUNCTION__ << cookieList.count();
#endif
  d->tree.clear();
  foreach (const QNetworkCookie &cookie, cookieList) {
    QString domain = cookie.domain();
    d->tree.insert(splitHost(domain), cookie);
  }
}

QString iNetworkCookieJarPrivate::urlPath(const QUrl &url) const
{
  QString urlPath = url.path();
  if (!urlPath.endsWith(QLatin1Char('/')))
    urlPath += QLatin1Char('/');
  return urlPath;
}

bool iNetworkCookieJarPrivate::matchingPath(const QNetworkCookie &cookie, const QString &urlPath) const
{
  QString cookiePath = cookie.path();
  if (!cookiePath.endsWith(QLatin1Char('/')))
    cookiePath += QLatin1Char('/');

  return urlPath.startsWith(cookiePath);
}

bool iNetworkCookieJarPrivate::matchesBlacklist(const QString &string) const
{
  if (!setSecondLevelDomain) {
    // Alternatively to save a little bit of ram we could just
    // use bsearch on twoLevelDomains in place
    for (int j = 0; twoLevelDomains[j]; ++j)
      secondLevelDomains += QLatin1String(twoLevelDomains[j]);
    setSecondLevelDomain = true;
  }
  QStringList::const_iterator i =
    qBinaryFind(secondLevelDomains.constBegin(), secondLevelDomains.constEnd(), string);
  return (i != secondLevelDomains.constEnd());
}

bool iNetworkCookieJarPrivate::matchingDomain(const QNetworkCookie &cookie, const QUrl &url) const
{
  QString domain = cookie.domain().simplified().toLower();
  domain.remove(QLatin1Char(' '));
  QStringList parts = splitHost(domain);
  if (parts.isEmpty())
    return false;

  // When there is only one part only file://localhost/ is accepted
  if (parts.count() == 1) {
    QString s = parts.first();
    if (parts.first() != QLatin1String("localhost"))
      return false;
    if (url.scheme().toLower() == QLatin1String("file"))
      return true;
  }

  // Check for blacklist
  if (parts.count() == 2 && matchesBlacklist(parts.last()))
    return false;

  QStringList urlParts = url.host().toLower().split(QLatin1Char('.'), QString::SkipEmptyParts);
  if (urlParts.isEmpty())
    return false;
  while (urlParts.count() > parts.count())
    urlParts.removeFirst();

  for (int j = 0; j < urlParts.count(); ++j) {
    if (urlParts.at(j) != parts.at(j)) {
      return false;
    }
  }

  return true;
}

void iNetworkCookieJar::setSecondLevelDomains(const QStringList &secondLevelDomains)
{
  d->setSecondLevelDomain = true;
  d->secondLevelDomains = secondLevelDomains;
  qSort(d->secondLevelDomains);
}

/*
 * Copyright 2008-2009 Benjamin C. Meyer <ben@meyerhome.net>
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

#include "iautosaver.h"

#include <qapplication.h>
#include <qdesktopservices.h>
#include <qdir.h>
#include <qmetaobject.h>
#include <qsettings.h>
#include <qurl.h>

#include <qwebsettings.h>

#include <qdebug.h>

static const unsigned int JAR_VERSION = 23;

QT_BEGIN_NAMESPACE
QDataStream &operator<<(QDataStream &stream, const QList<QNetworkCookie> &list)
{
  stream << JAR_VERSION;
  stream << quint32(list.size());
  for (int i = 0; i < list.size(); ++i)
    stream << list.at(i).toRawForm();
  return stream;
}

QDataStream &operator>>(QDataStream &stream, QList<QNetworkCookie> &list)
{
  list.clear();

  quint32 version;
  stream >> version;

  if (version != JAR_VERSION)
    return stream;

  quint32 count;
  stream >> count;
  for (quint32 i = 0; i < count; ++i) {
    QByteArray value;
    stream >> value;
    QList<QNetworkCookie> newCookies = QNetworkCookie::parseCookies(value);
    if (newCookies.count() == 0 && value.length() != 0) {
      qWarning() << "iCookieJar: Unable to parse saved cookie:" << value;
    }
    for (int j = 0; j < newCookies.count(); ++j)
      list.append(newCookies.at(j));
    if (stream.atEnd())
      break;
  }
  return stream;
}
QT_END_NAMESPACE

iCookieJar::iCookieJar(QObject *parent)
  : iNetworkCookieJar(parent)
  , m_loaded(false)
  , m_saveTimer(new AutoSaver(this))
  , m_acceptCookies(AcceptOnlyFromSitesNavigatedTo)
{
}

iCookieJar::~iCookieJar()
{
  if (m_loaded && m_keepCookies == KeepUntilExit)
    clear();
  m_saveTimer->saveIfNeccessary();
}

void iCookieJar::clear()
{
  setAllCookies(QList<QNetworkCookie>());
  m_saveTimer->changeOccurred();
  emit cookiesChanged();
}

void iCookieJar::load()
{
  if (m_loaded)
    return;
  // load cookies and exceptions
  qRegisterMetaTypeStreamOperators<QList<QNetworkCookie> >("QList<QNetworkCookie>");
  QSettings cookieSettings(QDesktopServices::storageLocation(QDesktopServices::DataLocation) + QLatin1String("/cookies.ini"), QSettings::IniFormat);
  setAllCookies(qvariant_cast<QList<QNetworkCookie> >(cookieSettings.value(QLatin1String("cookies"))));
  cookieSettings.beginGroup(QLatin1String("Exceptions"));
  m_exceptions_block = cookieSettings.value(QLatin1String("block")).toStringList();
  m_exceptions_allow = cookieSettings.value(QLatin1String("allow")).toStringList();
  m_exceptions_allowForSession = cookieSettings.value(QLatin1String("allowForSession")).toStringList();
  qSort(m_exceptions_block.begin(), m_exceptions_block.end());
  qSort(m_exceptions_allow.begin(), m_exceptions_allow.end());
  qSort(m_exceptions_allowForSession.begin(), m_exceptions_allowForSession.end());

  loadSettings();
}

void iCookieJar::loadSettings()
{
  QSettings settings;
  settings.beginGroup(QLatin1String("cookies"));
  QByteArray value = settings.value(QLatin1String("acceptCookies"),
				    QLatin1String("AcceptOnlyFromSitesNavigatedTo")).toByteArray();
  QMetaEnum acceptPolicyEnum = staticMetaObject.enumerator(staticMetaObject.indexOfEnumerator("AcceptPolicy"));
  m_acceptCookies = acceptPolicyEnum.keyToValue(value) == -1 ?
    AcceptOnlyFromSitesNavigatedTo :
    static_cast<AcceptPolicy>(acceptPolicyEnum.keyToValue(value));

  value = settings.value(QLatin1String("keepCookiesUntil"), QLatin1String("KeepUntilExpire")).toByteArray();
  QMetaEnum keepPolicyEnum = staticMetaObject.enumerator(staticMetaObject.indexOfEnumerator("KeepPolicy"));
  m_keepCookies = keepPolicyEnum.keyToValue(value) == -1 ?
    KeepUntilExpire :
    static_cast<KeepPolicy>(keepPolicyEnum.keyToValue(value));

  if (m_keepCookies == KeepUntilExit)
    setAllCookies(QList<QNetworkCookie>());

  m_loaded = true;
  emit cookiesChanged();
}

void iCookieJar::save()
{
  if (!m_loaded)
    return;
  purgeOldCookies();
  QString directory = QDesktopServices::storageLocation(QDesktopServices::DataLocation);
  if (directory.isEmpty())
    directory = QDir::homePath() + QLatin1String("/.") + QCoreApplication::applicationName();
  if (!QFile::exists(directory)) {
    QDir dir;
    dir.mkpath(directory);
  }
  QSettings cookieSettings(directory + QLatin1String("/cookies.ini"), QSettings::IniFormat);
  QList<QNetworkCookie> cookies = allCookies();
  for (int i = cookies.count() - 1; i >= 0; --i) {
    if (cookies.at(i).isSessionCookie())
      cookies.removeAt(i);
  }
  cookieSettings.setValue(QLatin1String("cookies"), qVariantFromValue<QList<QNetworkCookie> >(cookies));
  cookieSettings.beginGroup(QLatin1String("Exceptions"));
  cookieSettings.setValue(QLatin1String("block"), m_exceptions_block);
  cookieSettings.setValue(QLatin1String("allow"), m_exceptions_allow);
  cookieSettings.setValue(QLatin1String("allowForSession"), m_exceptions_allowForSession);

  // save cookie settings
  QSettings settings;
  settings.beginGroup(QLatin1String("cookies"));
  QMetaEnum acceptPolicyEnum = staticMetaObject.enumerator(staticMetaObject.indexOfEnumerator("AcceptPolicy"));
  settings.setValue(QLatin1String("acceptCookies"), QLatin1String(acceptPolicyEnum.valueToKey(m_acceptCookies)));

  QMetaEnum keepPolicyEnum = staticMetaObject.enumerator(staticMetaObject.indexOfEnumerator("KeepPolicy"));
  settings.setValue(QLatin1String("keepCookiesUntil"), QLatin1String(keepPolicyEnum.valueToKey(m_keepCookies)));
}

void iCookieJar::purgeOldCookies()
{
  QList<QNetworkCookie> cookies = allCookies();
  if (cookies.isEmpty())
    return;
  int oldCount = cookies.count();
  QDateTime now = QDateTime::currentDateTime();
  for (int i = cookies.count() - 1; i >= 0; --i) {
    if (!cookies.at(i).isSessionCookie() && cookies.at(i).expirationDate() < now)
      cookies.removeAt(i);
  }
  if (oldCount == cookies.count())
    return;
  setAllCookies(cookies);
  emit cookiesChanged();
}

QList<QNetworkCookie> iCookieJar::cookiesForUrl(const QUrl &url) const
{
  iCookieJar *that = const_cast<iCookieJar*>(this);
  if (!m_loaded)
    that->load();

  QWebSettings *globalSettings = QWebSettings::globalSettings();
  if (globalSettings->testAttribute(QWebSettings::PrivateBrowsingEnabled)) {
    QList<QNetworkCookie> noCookies;
    return noCookies;
  }

  return iNetworkCookieJar::cookiesForUrl(url);
}

bool iCookieJar::setCookiesFromUrl(const QList<QNetworkCookie> &cookieList, const QUrl &url)
{
  if (!m_loaded)
    load();

#if defined(COOKIE_DEBUG)
  qDebug() << "iCookieJar::" << __FUNCTION__ << url;
  qDebug() << cookieList;
#endif

  QWebSettings *globalSettings = QWebSettings::globalSettings();
  if (globalSettings->testAttribute(QWebSettings::PrivateBrowsingEnabled))
    return false;

  QString host = url.host();
  bool eBlock = qBinaryFind(m_exceptions_block.begin(), m_exceptions_block.end(), host) != m_exceptions_block.end();
  bool eAllow = qBinaryFind(m_exceptions_allow.begin(), m_exceptions_allow.end(), host) != m_exceptions_allow.end();
  bool eAllowSession = qBinaryFind(m_exceptions_allowForSession.begin(), m_exceptions_allowForSession.end(), host) != m_exceptions_allowForSession.end();

  bool addedCookies = false;
  // pass exceptions
  bool acceptInitially = (m_acceptCookies != AcceptNever);
  if ((acceptInitially && !eBlock)
      || (!acceptInitially && (eAllow || eAllowSession))) {
    // pass url domain == cookie domain
    QDateTime soon = QDateTime::currentDateTime();
    soon = soon.addDays(90);
    foreach (QNetworkCookie cookie, cookieList) {
      QList<QNetworkCookie> lst;
      if (m_keepCookies == KeepUntilTimeLimit
	  && !cookie.isSessionCookie()
	  && cookie.expirationDate() > soon) {
	cookie.setExpirationDate(soon);
      }
      lst += cookie;
      if (iNetworkCookieJar::setCookiesFromUrl(lst, url)) {
	addedCookies = true;
      } else {
	// finally force it in if wanted
	if (m_acceptCookies == AcceptAlways) {
	  QList<QNetworkCookie> cookies = allCookies();
	  QList<QNetworkCookie>::Iterator it = cookies.begin(),
	    end = cookies.end();
	  for ( ; it != end; ++it) {
	    // does this cookie already exist?
	    if (cookie.name() == it->name() &&
		cookie.domain() == it->domain() &&
		cookie.path() == it->path()) {
	      // found a match
	      cookies.erase(it);
	      break;
	    }
	  }

	  cookies += cookie;
	  setAllCookies(cookies);
	  addedCookies = true;
	}
#if 0
	else
	  qWarning() << "setCookiesFromUrl failed" << url << cookieList.value(0).toRawForm();
#endif
      }
    }
  }

  if (addedCookies) {
    m_saveTimer->changeOccurred();
    emit cookiesChanged();
  }
  return addedCookies;
}

iCookieJar::AcceptPolicy iCookieJar::acceptPolicy() const
{
  if (!m_loaded)
    (const_cast<iCookieJar*>(this))->load();
  return m_acceptCookies;
}

void iCookieJar::setAcceptPolicy(AcceptPolicy policy)
{
  if (!m_loaded)
    load();
  if (policy == m_acceptCookies)
    return;
  m_acceptCookies = policy;
  m_saveTimer->changeOccurred();
}

iCookieJar::KeepPolicy iCookieJar::keepPolicy() const
{
  if (!m_loaded)
    (const_cast<iCookieJar*>(this))->load();
  return m_keepCookies;
}

void iCookieJar::setKeepPolicy(KeepPolicy policy)
{
  if (!m_loaded)
    load();
  if (policy == m_keepCookies)
    return;
  m_keepCookies = policy;
  m_saveTimer->changeOccurred();
}

QStringList iCookieJar::blockedCookies() const
{
  if (!m_loaded)
    (const_cast<iCookieJar*>(this))->load();
  return m_exceptions_block;
}

QStringList iCookieJar::allowedCookies() const
{
  if (!m_loaded)
    (const_cast<iCookieJar*>(this))->load();
  return m_exceptions_allow;
}

QStringList iCookieJar::allowForSessionCookies() const
{
  if (!m_loaded)
    (const_cast<iCookieJar*>(this))->load();
  return m_exceptions_allowForSession;
}

void iCookieJar::setBlockedCookies(const QStringList &list)
{
  if (!m_loaded)
    load();
  m_exceptions_block = list;
  qSort(m_exceptions_block.begin(), m_exceptions_block.end());
  m_saveTimer->changeOccurred();
}

void iCookieJar::setAllowedCookies(const QStringList &list)
{
  if (!m_loaded)
    load();
  m_exceptions_allow = list;
  qSort(m_exceptions_allow.begin(), m_exceptions_allow.end());
  m_saveTimer->changeOccurred();
}

void iCookieJar::setAllowForSessionCookies(const QStringList &list)
{
  if (!m_loaded)
    load();
  m_exceptions_allowForSession = list;
  qSort(m_exceptions_allowForSession.begin(), m_exceptions_allowForSession.end());
  m_saveTimer->changeOccurred();
}
