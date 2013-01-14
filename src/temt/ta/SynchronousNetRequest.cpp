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

#include "SynchronousNetRequest.h"

#include <taMisc>
#include <taiMisc>
#include <iNetworkAccessManager>
#include <Program>


#include <QNetworkReply>
#include <QNetworkRequest>
#include <QXmlStreamReader>
#if (QT_VERSION >= 0x050000)
#include <QUrlQuery>
#endif
#include <QFile>




/////////////////////////////////////
//      SynchronousNetRequest      //
/////////////////////////////////////

// Implementation notes
//
// Each QNetworkAccessManager (QNAM) object has its own cookiejar, proxy
// settings, etc.  A global QNAM object (taiMisc::net_access_mgr) is already
// created for the Webkit stuff.  That QNAM is really an instance of
// iNetworkAccessManager, which has code to manage authentications, SSL
// errors, and (potentially) proxies.  A new cookie jar is created and given
// to net_access_mgr when it is created.
//
// Code in this file could potentially use the same QNAM or create its own.
// Originally (pre r5613), it was using the net_access_mgr QNAM object and
// connecting directly to the finished() signal as so:
//   QObject::connect(taiMisc::net_access_mgr, SIGNAL(finished(QNetworkReply*)),
//                    read_ready, SLOT(finished(QNetworkReply*)));
//
// That isn't viable since every webkit operation would inadvertently trigger
// read_ready's finished() slot.  SynchronousNetRequest was created with its
// own QNAM to avoid such problems, but after a few design iterations there
// was no longer any need to connect slots at all -- the "wait" function
// essentially polled the QNetworkReply's isFinished() function until the
// operation was complete, failed, or was cancelled.  This code was checked
// in as r5613 and worked well for simple query operations like searching,
// where no authentication or login was required.
//
// However, there are disadvantages to using a separate QNAM.  Each QNAM
// manages its own cookies and authentications, so a user would have to
// provide credentials for emergent's webkit browser, then provide them again
// to use this class's APIs.  The icookiejar.h and inetworkaccessmanager.h
// files save cookies and authentications to disk in a user's "local app data"
// folder (e.g., C:\Users\USER\AppData\Local\emergent on Windows).  With
// multiple QNAMs, each would write data to this store, clobbering the others.
//
// Sharing a cookie jar is a potentially fragile option -- documentation for
// QNetworkAccessManager::setCookieJar() says:
//   http://doc.qt.digia.com/4.7-snapshot/qnetworkaccessmanager.html
//   If cookieJar is in the same thread as this QNetworkAccessManager,
//   it will set the parent of the cookieJar so that the cookie jar is
//   deleted when this object is deleted as well. If you want to share
//   cookie jars between different QNetworkAccessManager objects, you
//   may want to set the cookie jar's parent to 0 after calling this
//   function.
//
// It seems like a better option is to share a QNAM instead, and if there's
// need to connect signals, connect them to the QNetworkReply object instead
// of to the QNAM object.  That option is recommended here:
//   http://www.johanpaul.com/blog/2011/07/why-qnetworkaccessmanager-should-not-have-the-finishedqnetworkreply-signal/
//   My guess is that the finished(QNetworkReply *) signal in
//   QNetworkAccessManager is there for the sake of convenience.
//   It's an obvious place to put a finished signal for a request
//   that the manager does. But it can be dangerous too since you
//   will probably use just one QNetworkAccessManager in your
//   application which means that it cannot bind directly to your
//   application states.
//
// The r5613 commit checkpointed the initial (working) implementation of
// SynchronousNetRequest before migrating to a scheme that shares the QNAM
// created for Webkit (r5616).

SynchronousNetRequest::SynchronousNetRequest(QNetworkAccessManager *qnam)
  : m_netManager(qnam)
  , m_reply(0)
  , m_isCancelled(false)
  , m_isFinished(false)
{
  if (!m_netManager) {
    // If the provided QNAM is null (default), use the QNAM object
    // created for emergent's embedded Webkit browser.
    m_netManager = taiMisc::net_access_mgr;
  }
}

SynchronousNetRequest::~SynchronousNetRequest()
{
  cancel();
}

QNetworkReply * SynchronousNetRequest::httpGet(const QUrl &url)
{
  // Make HTTP GET request, wait for reply.
  reset();
  m_reply = m_netManager->get(QNetworkRequest(url));
  waitForReply();
  return getReplyIfSuccess();
}

QNetworkReply * SynchronousNetRequest::httpPost(const QUrl &url)
{
  // Make HTTP POST request with no data to send (other than URL query params).
  return httpPost(url, 0, 0);
}

QNetworkReply * SynchronousNetRequest::httpPost(const QUrl &url, const char *filename)
{
  reset();
  QFile file(filename);
  if (!file.open(QIODevice::ReadOnly)) {
    taMisc::Warning("SynchronousNetRequest: could not open file", filename);
    return 0;
  }

  // Make HTTP POST request, wait for reply.
  QNetworkRequest request(url);
  request.setHeader(
    QNetworkRequest::ContentTypeHeader, "application/octet-stream");
  m_reply = m_netManager->post(request, &file);
  waitForReply();
  return getReplyIfSuccess();
}

QNetworkReply * SynchronousNetRequest::httpPost(const QUrl &url, const char *data, int size)
{
  reset();

  // Note: it's valid for data to be null and/or size to be 0.
  // For example, Mediawiki's login API requires POST but no additional data
  // other than the query terms.
  QByteArray byteArray(data, size);

  // Make HTTP POST request, wait for reply.
  QNetworkRequest request(url);
  request.setHeader(
    QNetworkRequest::ContentTypeHeader, "application/octet-stream");
  m_reply = m_netManager->post(request, byteArray);
  waitForReply();
  return getReplyIfSuccess();
}

QNetworkReply * SynchronousNetRequest::getReply()
{
  // Get the reply object.  If request failed, user can call
  // reply->error() or reply->errorString() to determine cause of error.
  return m_reply;
}

QNetworkReply * SynchronousNetRequest::getReplyIfSuccess()
{
  // If operation finished without being cancelled, a reply exists,
  // and no error occurred, then return the reply.
  if (!m_isCancelled && m_isFinished && m_reply
      && (m_reply->error() == QNetworkReply::NoError))
  {
    return m_reply;
  }
  return 0;
}

void SynchronousNetRequest::cancel()
{
  // Abort the request.
  if (m_reply) {
    m_reply->abort();

    // Use disconnect() and deleteLater() instead of just 'delete m_reply'
    // in case there are any pending events for the reply.
    m_reply->disconnect();
    m_reply->deleteLater();

    // Since m_reply will be deleted later, we are no longer responsible for
    // it and can safely null our pointer as if it were already deleted.
    m_reply = 0;
  }

  // Set our flag that the request was cancelled.
  m_isCancelled = true;
}

void SynchronousNetRequest::finished()
{
  // TODO: This method and member are only needed to support Qt 4.5, which
  // doesn't have the QNetworkReply::isFinished() method.  Once support for
  // Qt 4.5 is no longer needed, clean this up.
  m_isFinished = true;
}

void SynchronousNetRequest::reset()
{
  // Cancel any existing operation.
  cancel();

  // Get ready for a new request.
  m_isCancelled = false;
  m_isFinished = false;
}

void SynchronousNetRequest::waitForReply()
{
  // Loop until reply is completely received or the request is cancelled.

  // See comment in finished() re: Qt 4.5.
  if (m_reply) {
    connect(m_reply, SIGNAL(finished()), this, SLOT(finished()));
  }

  // TODO: It may be better to rewrite this using a QEventLoop, but this
  // does the trick for now (and allows us to poll isAborted() easily).
  // See this page for details on using QEventLoop:
  // http://doc.qt.digia.com/qq/qq27-responsive-guis.html#waitinginalocaleventloop
  while (m_reply && !m_isFinished && !m_isCancelled) {
    // Sleep to prevent 100% CPU usage.
    taMisc::SleepMs(50); // milliseconds

    // Allow other operations to continue -- specifically, process events such
    // as the user cancelling the operation.
    taMisc::ProcessEvents();

    // Poll to check if the request should be aborted.
    if (isAborted()) {
      cancel();
    }
  }

  if (m_isCancelled) {
    taMisc::Warning("SynchronousNetRequest: operation was cancelled");
  }
  else if (m_reply && m_reply->error() != QNetworkReply::NoError) {
    taMisc::Warning("SynchronousNetRequest: error occurred:",
      qPrintable(m_reply->errorString()));
  }
}

bool SynchronousNetRequest::isAborted()
{
  // Subclass may override to check for other conditions.
  if (Program::stop_req) {
    return true;
  }

  return false;
}
