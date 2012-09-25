// Copyright, 1995-2007, Regents of the University of Colorado,
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

// ta_mediawiki.h: interface to mediawiki api

#include "ta_mediawiki.h"

#include "inetworkaccessmanager.h"
#include "ta_datatable.h"
#include "ta_platform.h"
#include "ta_program.h"
#include "ta_qt.h"

#include <QFile>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QXmlStreamReader>

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
  m_reply = m_netManager->post(QNetworkRequest(url), &file);
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
  m_reply = m_netManager->post(QNetworkRequest(url), byteArray);
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
    taPlatform::msleep(50); // milliseconds

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

///////////////////////////
//      taMediaWiki      //
///////////////////////////

namespace { // anonymous
  // Find the next element in the XML stream, regardless of nesting level.
  bool findNextElement(QXmlStreamReader &reader, const char *element)
  {
    // As long as the XML stream isn't at the end (or errored),
    while (!reader.atEnd()) {
      // read tokens until a matching start element is found.
      if (reader.readNext() == QXmlStreamReader::StartElement) {
        if (reader.name() == element) {
          return true;
        }
      }
    }
    return false;
  }
}

void taMediaWiki::Initialize()
{
}

String taMediaWiki::GetApiURL(const String& wiki_name)
{
  bool appendIndexPhp = false;
  String wiki_url = taMisc::GetWikiURL(wiki_name, appendIndexPhp);
  if (wiki_url.empty()) {
    taMisc::Error("taMediaWiki::GetApiURL", "wiki named:", wiki_name,
                  "not found in global preferences/options under wiki_url settings");
    return _nilString;
  }
  return wiki_url + "/api.php";
}

/////////////////////////////////////////////////////
//            Account operations

String taMediaWiki::GetLoggedInUsername(const String &wiki_name)
{
  // Build the request URL.
  String wikiUrl = GetApiURL(wiki_name);
  if (wikiUrl.empty()) return "";

  QUrl url(wikiUrl);
  url.addQueryItem("action", "query");
  url.addQueryItem("format", "xml");
  url.addQueryItem("meta", "userinfo");

  // Make the network request.
  // Note: The reply will be deleted when the request goes out of scope.
  SynchronousNetRequest request;
  if (QNetworkReply *reply = request.httpGet(url)) {
    // Find the <userinfo> tag and check for a non-zero ID.
    QXmlStreamReader reader(reply);
    if (findNextElement(reader, "userinfo")) {
      QXmlStreamAttributes attrs = reader.attributes();
      if (attrs.value("id") != "0") {
        return attrs.value("name").toString();
      }
    }
  }

  return "";
}

bool taMediaWiki::Login(const String &wiki_name, const String &username)
{
  // Make sure wiki name is valid before doing anything else.
  String wikiUrl = GetApiURL(wiki_name);
  if (wikiUrl.empty()) return false;

  // Return true if username specified is already logged in.
  if (!username.empty()) {
    String loggedInUser = GetLoggedInUsername(wiki_name);
    if (username == loggedInUser) {
      // User is already logged in.
      return true;
    }
  }

  // Prompt for username/password.
  QString qUsername = username;
  QString qPassword;
  QString message = "Enter username and password for \"%1\" wiki at %2";
  message = message.arg(QString(wiki_name)).arg(QString(wikiUrl));

  if (!getUsernamePassword(qUsername, qPassword, message)) {
    // User hit cancel on dialog.
    return false;
  }

  // Mediawiki login is (potentially) a two-stage process.  The first
  // attempt to log in, a token is returned.  The second attempt must
  // pass the token back along with a cookie.  The QNAM should handle
  // the cookie processing for us.

  // Set up the URL for the first stage.
  QUrl url(wikiUrl);
  url.addQueryItem("action", "login");
  url.addQueryItem("format", "xml");
  url.addQueryItem("lgname", qUsername);
  url.addQueryItem("lgpassword", qPassword);

  // Make the network request, once per stage.
  SynchronousNetRequest request;
  for (int stage = 0; stage < 2; ++stage) {
    if (QNetworkReply *reply = request.httpPost(url)) {
      // Find the <login> tag.
      QXmlStreamReader reader(reply);
      if (!findNextElement(reader, "login")) {
        taMisc::Warning("Malformed response logging in to ", wiki_name, "wiki");
        return false;
      }

      // Check the login result.
      QXmlStreamAttributes attrs = reader.attributes();
      QString result = attrs.value("result").toString();

      // Return true if success or false if user not recognized.
      if (result == "Success") {
        taMisc::Info("User logged in to", wiki_name, "wiki:",
          qPrintable(qUsername));
        return true;
      }
      else if (result == "NotExists") {
        taMisc::Warning("Username not recognized on", wiki_name, "wiki:",
          qPrintable(qUsername));
        return false;
      }
      else if (result == "WrongPass") {
        taMisc::Warning("Incorrect password for user", qPrintable(qUsername),
          "on", wiki_name, "wiki.");
        return false;
      }
      else if (result == "NeedToken") {
        // This indicates we need to do the second stage.  Set up the URL
        // for the second stage before the attrs goes out of scope.
        taMisc::DebugInfo("Performing 2-stage login to", wiki_name, "wiki.");
        url.addQueryItem("lgtoken", attrs.value("token").toString());
      }
      else {
        taMisc::Warning("Unexpected error during login:", qPrintable(result));
        return false;
      }
    }
  }

  // The only way to get here is if we made two passes through the loop
  // and got "NeedToken" both times -- login has failed.
  taMisc::Warning("Could not log in to", wiki_name, "wiki.");

  return false;
}

bool taMediaWiki::Logout(const String &wiki_name)
{
  // #CAT_Account Logout from the wiki.
  return false;
}

/////////////////////////////////////////////////////
//            Page operations

bool taMediaWiki::PageExists(const String& wiki_name, const String& page_name)
{
  // #CAT_Page determine if given page exists on wiki
  return false;
}

bool taMediaWiki::CreatePage(const String& wiki_name, const String& page_name,
                             const String& page_content)
{
  // #CAT_Page create given page on the wiki and populate it with given content if non-empty  -- return true on success
  return false;
}

bool taMediaWiki::FindMakePage(const String& wiki_name, const String& page_name,
                               const String& page_content)
{
  // #CAT_Page find or create given page on the wiki and populate it with given content if non-empty -- return true on success
  return false;
}

/////////////////////////////////////////////////////
//              Upload/Download operations

bool taMediaWiki::UploadFile(const String& wiki_name, const String& file_name,
                             const String& wiki_file_name)
{
  // #CAT_File upload given file name to wiki, optionally giving it a different file name on the wiki relative to what it is locally

  return false;
}

bool taMediaWiki::DownloadFile(const String& wiki_name, const String& file_name,
                               const String& local_file_name)
{
  // #CAT_File download given file name from wiki, optionally giving it a different file name than what it was on the wiki
  return false;
}

/////////////////////////////////////////////////////
//              Query operations

bool taMediaWiki::QueryPages(DataTable* results, const String& wiki_name,
                             const String& name_space,
                             const String& start_nm,
                             const String& prefix,
                             int max_results)
{
  // #CAT_Query fill results data table with pages in given name space, starting at given name, and with each name starting with given prefix (empty = all), string column "PageTitle" has page tiltle, int column "PageId" has page id number
  return false;
}

bool taMediaWiki::QueryPagesByCategory(DataTable* results, const String& wiki_name,
                                       const String& category,
                                       const String& name_space,
                                       int max_results)
{
  // #CAT_Query fill results data table with pages in given category, starting at given name, and with each name starting with given prefix (empty = all), string column "PageTitle" has page tiltle, int column "PageId" has page id number
  return false;
}

bool taMediaWiki::QueryFiles(DataTable* results, const String& wiki_name,
                             const String& start_nm,
                             const String& prefix,
                             int max_results)
{
  // #CAT_Query fill results data table with files uploaded to wiki, starting at given name, and with each name starting with given prefix (empty = all), string column "FileName" has name of file, int column "Size" has file size, string column "MimeType" has mime type
  return false;
}

bool taMediaWiki::SearchPages(DataTable* results, const String& wiki_name,
                              const String& search_str, bool title_only,
                              const String& name_space,
                              int max_results)
{
  if (!results) {
    taMisc::Error("taMediaWiki::SearchPages -- results data table is NULL -- must supply a valid data table!");
    return false;
  }

  // Build the request URL.
  String wikiUrl = GetApiURL(wiki_name);
  if (wikiUrl.empty()) return false;

  QUrl url(wikiUrl);
  url.addQueryItem("action", "query");
  url.addQueryItem("format", "xml");
  url.addQueryItem("list", "search");
  url.addQueryItem("srsearch", search_str);
  url.addQueryItem("srwhat", title_only ? "title" : "text");

  if (max_results > 0) {
    url.addQueryItem("srlimit", QString::number(max_results));
  }

  // Make the network request.
  SynchronousNetRequest request;
  if (QNetworkReply *reply = request.httpGet(url)) {
    // Prepare the datatable.
    results->RemoveAllRows();
    DataCol* pt_col = results->FindMakeCol("PageTitle", VT_STRING);

    // For all <p> elements in the XML, add the page title to the datatable.
    QXmlStreamReader reader(reply);
    while (findNextElement(reader, "p")) {
      QXmlStreamAttributes attrs = reader.attributes();
      String pt = attrs.value("title").toString();
      results->AddBlankRow();
      pt_col->SetVal(pt, -1);
    }

    // Return true (success) as long as there were no errors in the XML.
    return !reader.hasError();
  }

  return false;
}
