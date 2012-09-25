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

#ifndef TA_MEDIAWIKI_H
#define TA_MEDIAWIKI_H

#include "ta_stdef.h"
#include "ta_base.h"

class DataTable;

#ifndef __MAKETA__
class QNetworkAccessManager;
class QNetworkReply;
class QUrl;

// This class provides a synchronous way to perform HTTP requests.
// It wraps the QNetworkAccessManager, QNetworkRequest, and QNetworkReply
// functionality provided by Qt so the user doesn't need to explicitly set
// up signals and slots.  It is possible, though not required, to connect
// a signal to this class's cancel() slot.  Also, a subclass may override
// isAborted() to return true if some external condition indicates the
// transfer should be aborted.
class SynchronousNetRequest : public QObject
{
  Q_OBJECT

public:
  SynchronousNetRequest(QNetworkAccessManager *qnam = 0);
  virtual ~SynchronousNetRequest();

  // Operations to perform HTTP requests (GET or POST).
  // As a convenience, if the request was successful, the reply object
  // is returned.  If the request failed, it is still possible to get
  // the reply object using getReply(), in order to check for errors.
  // If the request was cancelled, getReply() will return null.
  QNetworkReply * httpGet(const QUrl &url);
  QNetworkReply * httpPost(const QUrl &url);
  QNetworkReply * httpPost(const QUrl &url, const char *filename);
  QNetworkReply * httpPost(const QUrl &url, const char *data, int size);

  QNetworkReply * getReply();
  QNetworkReply * getReplyIfSuccess();

public slots:
  void cancel();

private slots:
  void finished();

private:
  void reset();
  void waitForReply();
  virtual bool isAborted();

  QNetworkAccessManager *m_netManager;
  QNetworkReply *m_reply;
  bool m_isCancelled;
  bool m_isFinished;
};
#endif

class TA_API taMediaWiki : public taOBase
{
  // ##INLINE ##NO_TOKENS media wiki api interface -- all static functions for performing functions we actually want to perform
  INHERITED(taOBase)

public:
  static String GetApiURL(const String& wiki_name);
  // #CAT_Wiki gets the url for the wiki api

  /////////////////////////////////////////////////////
  //            Account operations

  static String GetLoggedInUsername(const String &wiki_name);
  // #CAT_Account Get the username of the user currently logged in via emergent's Webkit browser.

  static bool Login(const String &wiki_name, const String &username = "");
  // #CAT_Account Login to the wiki.  Returns true if username specified is already logged in.  Otherwise prompts for username/password and returns true if login succeeds.

  static bool Logout(const String &wiki_name);
  // #CAT_Account Logout from the wiki.

  /////////////////////////////////////////////////////
  //            Page operations

  static bool   PageExists(const String& wiki_name, const String& page_name);
  // #CAT_Page determine if given page exists on wiki

  static bool   CreatePage(const String& wiki_name, const String& page_name,
                           const String& page_content="");
  // #CAT_Page create given page on the wiki and populate it with given content if non-empty  -- return true on success

  static bool   FindMakePage(const String& wiki_name, const String& page_name,
                             const String& page_content="");
  // #CAT_Page find or create given page on the wiki and populate it with given content if non-empty -- return true on success

  /////////////////////////////////////////////////////
  //            Upload/Download operations

  static bool   UploadFile(const String& wiki_name, const String& file_name,
                           const String& wiki_file_name="");
  // #CAT_File upload given file name to wiki, optionally giving it a different file name on the wiki relative to what it is locally

  static bool   DownloadFile(const String& wiki_name, const String& file_name,
                             const String& local_file_name="");
  // #CAT_File download given file name from wiki, optionally giving it a different file name than what it was on the wiki

  /////////////////////////////////////////////////////
  //            Query operations

  static bool   QueryPages(DataTable* results, const String& wiki_name,
                           const String& name_space="",
                           const String& start_nm="",
                           const String& prefix="",
                           int max_results=-1);
  // #CAT_Query fill results data table with pages in given name space, starting at given name, and with each name starting with given prefix (empty = all), string column "PageTitle" has page tiltle, int column "PageId" has page id number

  static bool   QueryPagesByCategory(DataTable* results, const String& wiki_name,
                                     const String& category,
                                     const String& name_space="",
                                     int max_results=-1);
  // #CAT_Query fill results data table with pages in given category, starting at given name, and with each name starting with given prefix (empty = all), string column "PageTitle" has page tiltle, int column "PageId" has page id number

  static bool   QueryFiles(DataTable* results, const String& wiki_name,
                           const String& start_nm="",
                           const String& prefix="",
                           int max_results=-1);
  // #CAT_Query fill results data table with files uploaded to wiki, starting at given name, and with each name starting with given prefix (empty = all), string column "FileName" has name of file, int column "Size" has file size, string column "MimeType" has mime type

  static bool   SearchPages(DataTable* results, const String& wiki_name,
                            const String& search_str, bool title_only = false,
                            const String& name_space="",
                            int max_results=-1);
  // #CAT_Query fill results data table with pages containing given search string, starting at given name, and with each name starting with given prefix (empty = all), string column "PageTitle" has page tiltle

protected:
  TA_BASEFUNS_NOCOPY(taMediaWiki);

private:
  void Initialize();
  void Destroy() {}
};

#endif // TA_MEDIAWIKI_H
