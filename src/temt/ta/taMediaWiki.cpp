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

#include "taMediaWiki.h"

#include <iNetworkAccessManager>
#include <DataTable>
#include <Program>
#include <iSynchronousNetRequest>
#include <algorithm>

#include <taMisc>

#include <QFile>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QXmlStreamReader>
#include <QEventLoop>
#include <QByteArray>
#include <QDebug>
#if (QT_VERSION >= 0x050000)
#include <QUrlQuery>
#endif

TA_BASEFUNS_CTORS_DEFN(taMediaWiki);

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
  // Placeholder -- taMediaWiki objects currently do not need to be initialized.
}

/////////////////////////////////////////////////////
//            ACCOUNT OPERATIONS

String taMediaWiki::GetLoggedInUsername(const String &wiki_name)
{
  // #CAT_Account Build the request URL.

  String wikiUrl = GetApiURL(wiki_name);
  if (wikiUrl.empty()) return "";

  QUrl url(wikiUrl);
#if (QT_VERSION >= 0x050000)
  QUrlQuery urq;
  urq.addQueryItem("action", "query");
  urq.addQueryItem("format", "xml");
  urq.addQueryItem("meta", "userinfo");
  url.setQuery(urq);
#else
  url.addQueryItem("action", "query");
  url.addQueryItem("format", "xml");
  url.addQueryItem("meta", "userinfo");
#endif

  // Make the network request.
  // Note: The reply will be deleted when the request goes out of scope.
  iSynchronousNetRequest request;
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
  // #CAT_Account Log the given username into the wiki, prompting user for password -- if the given username is already logged in, do nothing -- returns true on success.

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
  message = message.arg(QString(wiki_name.chars())).arg(QString(wikiUrl.chars()));

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
#if (QT_VERSION >= 0x050000)
  QUrlQuery urq;
  urq.addQueryItem("action", "login");
  urq.addQueryItem("format", "xml");
  urq.addQueryItem("lgname", qUsername);
  urq.addQueryItem("lgpassword", qPassword);
  url.setQuery(urq);
#else
  url.addQueryItem("action", "login");
  url.addQueryItem("format", "xml");
  url.addQueryItem("lgname", qUsername);
  url.addQueryItem("lgpassword", qPassword);
#endif

  // Make the network request, once per stage.
  iSynchronousNetRequest request;
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
#if (QT_VERSION >= 0x050000)
        urq.addQueryItem("lgtoken", attrs.value("token").toString());
        url.setQuery(urq);
#else
        url.addQueryItem("lgtoken", attrs.value("token").toString());
#endif
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

  // Make sure wiki name is valid before doing anything else.
  String wikiUrl = GetApiURL(wiki_name);
  if (wikiUrl.empty()) { return false; }

  QUrl url(wikiUrl);
#if (QT_VERSION >= 0x050000)
  QUrlQuery urq;
  urq.addQueryItem("action", "logout");
  url.setQuery(urq);
#else
  url.addQueryItem("action", "logout");
#endif

  iSynchronousNetRequest request;
  return request.httpGet(url);
}

/////////////////////////////////////////////////////
//              UPLOAD/DOWNLOAD OPERATIONS

bool taMediaWiki::UploadFile(const String& wiki_name, const String& local_file_name,
                             const String& wiki_file_name, bool convert_to_camel)
{
  // #CAT_File Upload given file name to wiki, optionally giving it a different file name on the wiki relative to what it is locally.

  String dst_filename;

  if (wiki_file_name.empty()) {
    dst_filename = local_file_name;
    if (convert_to_camel) {
      dst_filename = dst_filename.FileToCamel();
      taMisc::Info("Converted destination filename from", local_file_name, "to", dst_filename);
    }
  }
  else {
    dst_filename = wiki_file_name;
    if (convert_to_camel) {
      dst_filename = dst_filename.FileToCamel();
      taMisc::Info("Converted destination filename from", wiki_file_name, "to", dst_filename);
    }
  }

  String wikiUrl = GetApiURL(wiki_name);
  if (wikiUrl.empty()) { return false; }

  String token = GetEditToken(wiki_name);
  if (token.empty()) { return false; }

  QUrl url(wikiUrl);

  // Make the network request.
  iSynchronousNetRequest request;
  if (QNetworkReply *reply = request.httpPost(url, local_file_name, dst_filename, token)) {
    QXmlStreamReader reader(reply);
    
    while(!reader.atEnd()) {
      if (reader.readNext() == QXmlStreamReader::StartElement) {
        QXmlStreamAttributes attrs = reader.attributes();
        if(reader.hasError()) {
          QString err_code = attrs.value("code").toString();
          QString err_info = attrs.value("info").toString();
          taMisc::Warning("File upload failed with error code:", qPrintable(err_code), "(", qPrintable(err_info), ")");
          
          return false;
        }
        else {
          taMisc::Info("File upload successful!");
          
          return true;
        }
      }
    }
  }
  taMisc::Warning("File upload request failed");
  
  return false;
}

bool taMediaWiki::DownloadFile(const String& wiki_name, const String& wiki_file_name,
                               const String& local_file_name, bool convert_to_camel)
{
  // #CAT_File Download given file name from wiki, optionally giving it a different file name than what it was on the wiki.

  String dst_filename;

  if (local_file_name.empty()) {
    dst_filename = wiki_file_name;
    if (convert_to_camel) {
      dst_filename = dst_filename.FileToCamel();
      taMisc::Info("Converted destination filename from", wiki_file_name, "to", dst_filename);
    }
  }
  else {
    dst_filename = local_file_name;
    if (convert_to_camel) {
      dst_filename = dst_filename.FileToCamel();
      taMisc::Info("Converted destination filename from", local_file_name, "to", dst_filename);
    }
  }
  
  String wikiUrl = GetApiURL(wiki_name);
  if (wikiUrl.empty()) { return false; }

  QUrl url(wikiUrl);

#if (QT_VERSION >= 0x050000)
  QUrlQuery urq;
  urq.addQueryItem("action", "query");
  urq.addQueryItem("list", "allimages");
  urq.addQueryItem("aiprefix", wiki_file_name);
  urq.addQueryItem("format", "xml");
  url.setQuery(urq);
#else
  url.addQueryItem("action", "query");
  url.addQueryItem("list", "allimages");
  url.addQueryItem("aiprefix", wiki_file_name);
  url.addQueryItem("format", "xml");
#endif

  // Make the network request.
  iSynchronousNetRequest request;
  if (QNetworkReply *reply = request.httpGet(url)) {
    QXmlStreamReader reader(reply);
    if (findNextElement(reader, "img")) {
      QXmlStreamAttributes attrs = reader.attributes();
      if (attrs.hasAttribute("url")) {
        String downStr = (String) attrs.value("url").toString();
        if (!downStr.empty()) {
          taMisc::Info("Found file at URL:", downStr);
          
          QUrl downUrl(downStr);
          if (QNetworkReply *downReply = request.httpGet(downUrl)) {
            if (downReply->error() == QNetworkReply::NoError) {
              QString replyStr;
              int httpStatusCode = downReply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toUInt();
              
              switch (httpStatusCode) {
                case RESPONSE_OK:
                  if (downReply->isReadable()) {
                    replyStr = QString::fromUtf8(downReply->readAll().data());
                  }
                  break;
                case RESPONSE_ERROR:
                case RESPONSE_BAD_REQUEST:
                default:
                  break;
              }

              if (!(replyStr.isNull())) {
                QFile file(dst_filename);
                if (file.open(QIODevice::WriteOnly)) {
                  file.write(qPrintable(replyStr));
                  file.close();
                  taMisc::Info(dst_filename, "successfully downloaded!");

                  return true;
                }
                else { taMisc::Warning("Could not open", dst_filename, "for writing:", qPrintable(file.errorString())); }
              }
              else { taMisc::Warning("Request returned a bad response or null file"); }
            }
            else {
#if (QT_VERSION >= 0x040800)
	      taMisc::Warning("File download request failed with error:", qPrintable(downReply->error()));
#else
	      taMisc::Warning("File download request failed with an error");
#endif
	    }
          }
          else { taMisc::Warning("File download request failed"); }
        }
        else { taMisc::Warning("Request returned an empty URL attribute"); }
      }
      else { taMisc::Warning("Request returned no URL attribute"); }
    }
    else { taMisc::Warning(wiki_file_name, "not found on", wiki_name, "wiki"); }
  }
  else { taMisc::Warning("File URL request failed"); }

  return false;
}

/////////////////////////////////////////////////////
//              QUERY OPERATIONS

bool taMediaWiki::QueryPages(DataTable* results, const String& wiki_name,
                             const String& name_space,
                             const String& start_nm,
                             const String& prefix,
                             int max_results)
{
  // #CAT_Query Fill results data table with pages in given name space, starting at given name, and with each name starting with given prefix (empty = all), string column "PageTitle" has page tiltle, int column "PageId" has page id number.
  
  if (!results) {
    taMisc::Error("taMediaWiki::QueryPages -- results data table is NULL -- must supply a valid data table!");
    return false;
  }

  // Build the request URL.
  String wikiUrl = GetApiURL(wiki_name);
  if (wikiUrl.empty()) return false;

  QUrl url(wikiUrl);
#if (QT_VERSION >= 0x050000)
  QUrlQuery urq;
  urq.addQueryItem("action", "query");
  urq.addQueryItem("format", "xml");
  urq.addQueryItem("list", "allpages");
  urq.addQueryItem("apfrom", start_nm);
  urq.addQueryItem("apprefix", prefix);
  if (name_space.empty()) {
    urq.addQueryItem("apnamespace", QString::number(0));
  }
  else {
    urq.addQueryItem("apnamespace", QString::number(0)); // TODO: convert namespace string to appropriate namespace ID (int)
  }
  if (max_results > 0) {
    urq.addQueryItem("aplimit", QString::number(max_results));
  }
  url.setQuery(urq);
#else
  url.addQueryItem("action", "query");
  url.addQueryItem("format", "xml");
  url.addQueryItem("list", "allpages");
  url.addQueryItem("apfrom", start_nm);
  url.addQueryItem("apprefix", prefix);
  if (name_space.empty()) {
    url.addQueryItem("apnamespace", QString::number(0));
  }
  else {
    url.addQueryItem("apnamespace", QString::number(0)); // TODO: convert namespace string to appropriate namespace ID (int)
  }
  if (max_results > 0) {
    url.addQueryItem("aplimit", QString::number(max_results));
  }
#endif

  // Make the network request.
  iSynchronousNetRequest request;
  if (QNetworkReply *reply = request.httpGet(url)) {
    // Prepare the datatable.
    results->RemoveAllRows();
    DataCol* pt_col = results->FindMakeCol("PageTitle", VT_STRING);
    DataCol* pid_col = results->FindMakeCol("PageId", VT_INT);

  // For all <p> elements in the XML, add the page title and page ID to the datatable.
    QXmlStreamReader reader(reply);
    while (findNextElement(reader, "p")) {
      QXmlStreamAttributes attrs = reader.attributes();
      String pt = attrs.value("title").toString();
      int pid = attrs.value("pageid").toString().toInt();
      results->AddBlankRow();
      pt_col->SetVal(pt, -1);
      pid_col->SetVal(pid, -1);
    }

    // Return true (success) as long as there were no errors in the XML.
    return !reader.hasError();
  }

  return false;
}

bool taMediaWiki::QueryPagesByCategory(DataTable* results, const String& wiki_name,
                                       const String& category,
                                       const String& name_space,
                                       int max_results)
{
  // #CAT_Query Fill results data table with pages in given category, starting at given name, and with each name starting with given prefix (empty = all), string column "PageTitle" has page tiltle, int column "PageId" has page id number.
  
  if (!results) {
    taMisc::Error("taMediaWiki::QueryPagesByCategory -- results data table is NULL -- must supply a valid data table!");
    return false;
  }

  // Build the request URL.
  String wikiUrl = GetApiURL(wiki_name);
  if (wikiUrl.empty()) return false;

  QUrl url(wikiUrl);
#if (QT_VERSION >= 0x050000)
  QUrlQuery urq;
  urq.addQueryItem("action", "query");
  urq.addQueryItem("format", "xml");
  urq.addQueryItem("list", "categorymembers");
  urq.addQueryItem("cmtitle", "Category:" + category);
  if (name_space.empty()) {
    urq.addQueryItem("cmnamespace", QString::number(0));
  }
  else {
    urq.addQueryItem("cmnamespace", QString::number(0)); // TODO: convert namespace string to appropriate namespace ID (int)
  }
  urq.addQueryItem("cmtype", "page|subcat");
  if (max_results > 0) {
    urq.addQueryItem("cmlimit", QString::number(max_results));
  }
  url.setQuery(urq);
#else
  url.addQueryItem("action", "query");
  url.addQueryItem("format", "xml");
  url.addQueryItem("list", "categorymembers");
  url.addQueryItem("cmtitle", "Category:" + category);
  if (name_space.empty()) {
    url.addQueryItem("cmnamespace", QString::number(0));
  }
  else {
    url.addQueryItem("cmnamespace", QString::number(0)); // TODO: convert namespace string to appropriate namespace ID (int)
  }
  url.addQueryItem("cmtype", "page|subcat");
  if (max_results > 0) {
    url.addQueryItem("cmlimit", QString::number(max_results));
  }
#endif

  // Make the network request.
  iSynchronousNetRequest request;
  if (QNetworkReply *reply = request.httpGet(url)) {
    // Prepare the datatable.
    results->RemoveAllRows();
    DataCol* pt_col = results->FindMakeCol("PageTitle", VT_STRING);
    DataCol* pid_col = results->FindMakeCol("PageId", VT_INT);

  // For all <cm> elements in the XML, add the page title and page ID to the datatable.
    QXmlStreamReader reader(reply);
    while (findNextElement(reader, "cm")) {
      QXmlStreamAttributes attrs = reader.attributes();
      String pt = attrs.value("title").toString();
      int pid = attrs.value("pageid").toString().toInt();
      results->AddBlankRow();
      pt_col->SetVal(pt, -1);
      pid_col->SetVal(pid, -1);
    }

    // Return true (success) as long as there were no errors in the XML.
    return !reader.hasError();
  }

  return false;
}

bool taMediaWiki::QueryFiles(DataTable* results, const String& wiki_name,
                             const String& start_nm,
                             const String& prefix,
                             int max_results)
{
  // #CAT_Query Fill results data table with files uploaded to wiki, starting at given name, and with each name starting with given prefix (empty = all), string column "FileName" has name of file, int column "Size" has file size, string column "MimeType" has mime type.
  
  if (!results) {
    taMisc::Error("taMediaWiki::QueryFiles -- results data table is NULL -- must supply a valid data table!");
    return false;
  }

  // Build the request URL.
  String wikiUrl = GetApiURL(wiki_name);
  if (wikiUrl.empty()) return false;

  QUrl url(wikiUrl);
#if (QT_VERSION >= 0x050000)
  QUrlQuery urq;
  urq.addQueryItem("action", "query");
  urq.addQueryItem("format", "xml");
  urq.addQueryItem("list", "allimages");
  urq.addQueryItem("aisort", "name");
  urq.addQueryItem("aifrom", start_nm);
  urq.addQueryItem("aiprefix", prefix);
  urq.addQueryItem("aiprop", "timestamp|url|size|mime");
  if (max_results > 0) {
    urq.addQueryItem("ailimit", QString::number(max_results));
  }
  url.setQuery(urq);
#else
  url.addQueryItem("action", "query");
  url.addQueryItem("format", "xml");
  url.addQueryItem("list", "allimages");
  url.addQueryItem("aisort", "name");
  url.addQueryItem("aifrom", start_nm);
  url.addQueryItem("aiprefix", prefix);
  url.addQueryItem("aiprop", "timestamp|url|size|mime");
  if (max_results > 0) {
    url.addQueryItem("ailimit", QString::number(max_results));
  }
#endif

  // Make the network request.
  iSynchronousNetRequest request;
  if (QNetworkReply *reply = request.httpGet(url)) {
    // Prepare the datatable.
    results->RemoveAllRows();
    DataCol* fn_col = results->FindMakeCol("FileName", VT_STRING);
    DataCol* sz_col = results->FindMakeCol("Size", VT_INT);
    DataCol* mt_col = results->FindMakeCol("MimeType", VT_STRING);

  // For all <img> elements in the XML, add the file name, file size, and mime type to the datatable.
    QXmlStreamReader reader(reply);
    while (findNextElement(reader, "img")) {
      QXmlStreamAttributes attrs = reader.attributes();
      String fn = attrs.value("name").toString();
      int sz = attrs.value("size").toString().toInt();
      String mt = attrs.value("mime").toString();
      results->AddBlankRow();
      fn_col->SetVal(fn, -1);
      sz_col->SetVal(sz, -1);
      mt_col->SetVal(mt, -1);
    }

    // Return true (success) as long as there were no errors in the XML.
    return !reader.hasError();
  }

  return false;
}

bool taMediaWiki::SearchPages(DataTable* results, const String& wiki_name,
                              const String& search_str, bool title_only,
                              const String& name_space,
                              int max_results)
{
  // #CAT_Query Fill results data table with the pages matching the given search string -- if title_only is true, only search for matches in page titles; else, search for matches in page contents -- returns true on success.

  if (!results) {
    taMisc::Error("taMediaWiki::SearchPages -- results data table is NULL -- must supply a valid data table!");
    return false;
  }

  // Build the request URL.
  String wikiUrl = GetApiURL(wiki_name);
  if (wikiUrl.empty()) return false;

  QUrl url(wikiUrl);
#if (QT_VERSION >= 0x050000)
  QUrlQuery urq;
  urq.addQueryItem("action", "query");
  urq.addQueryItem("format", "xml");
  urq.addQueryItem("list", "search");
  urq.addQueryItem("srsearch", search_str);
  urq.addQueryItem("srwhat", title_only ? "title" : "text");
  if (max_results > 0) {
    urq.addQueryItem("srlimit", QString::number(max_results));
  }
  url.setQuery(urq);
#else
  url.addQueryItem("action", "query");
  url.addQueryItem("format", "xml");
  url.addQueryItem("list", "search");
  url.addQueryItem("srsearch", search_str);
  url.addQueryItem("srwhat", title_only ? "title" : "text");
  if (max_results > 0) {
    url.addQueryItem("srlimit", QString::number(max_results));
  }
#endif

  // Make the network request.
  iSynchronousNetRequest request;
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

/////////////////////////////////////////////////////
//            PAGE OPERATIONS

bool taMediaWiki::PageExists(const String& wiki_name, const String& page_name)
{
  // #CAT_Page Determine if given page exists on wiki.

  // Build the request URL.
  String wikiUrl = GetApiURL(wiki_name);
  if (wikiUrl.empty()) return false;

  QUrl url(wikiUrl);
#if (QT_VERSION >= 0x050000)
  QUrlQuery urq;
  urq.addQueryItem("action", "query");
  urq.addQueryItem("format", "xml");
  urq.addQueryItem("titles", page_name);
  url.setQuery(urq);
#else
  url.addQueryItem("action", "query");
  url.addQueryItem("format", "xml");
  url.addQueryItem("titles", page_name);
#endif
  // Make the network request.
  iSynchronousNetRequest request;
  if (QNetworkReply *reply = request.httpGet(url)) {
    // Default the normalized name to the provided page name;
    // will be changed if normalization was performed by the server.
    String normalizedName = page_name;

    QXmlStreamReader reader(reply);
    int pageCount = 0;
    while (!reader.atEnd()) {
      if (reader.readNext() == QXmlStreamReader::StartElement) {
        // Check for an <n> element indicating the normalized name.
        // This is only present if the proivded page_name isn't
        // canonical.
        if (reader.name() == "n") {
          QXmlStreamAttributes attrs = reader.attributes();
          if (attrs.value("from").toString() == page_name) {
            normalizedName = attrs.value("to").toString();
          }
        }
        // Check for a <page> element
        else if (reader.name() == "page") {
          ++pageCount;
          QXmlStreamAttributes attrs = reader.attributes();
          if (attrs.value("title").toString() == normalizedName) {
            // Check if the page name is marked as invalid or missing.
            if (attrs.hasAttribute("invalid")) {
              // For example, the page name "_" is invalid.
              taMisc::Warning("Page name is invalid:", page_name);
              return false;
            }
            else if (attrs.hasAttribute("missing")) {
              // This is the normal case when a page does NOT exist.
              return false;
            }
            else if (attrs.hasAttribute("pageid")) {
              // Double check that the page ID is a positive number -- it
              // should be since the page isn't marked invalid or missing!
              bool ok = false;
              QString pageIdStr = attrs.value("pageid").toString();
              int pageId = pageIdStr.toInt(&ok);
              if (!ok) {
                // Shouldn't happen.
                taMisc::Warning("Page ID is not numeric:", page_name,
                  qPrintable(pageIdStr));
                return false;
              }
              else if (pageId <= 0) {
                // Shouldn't happen.
                taMisc::Warning("Page ID is invalid:", page_name,
                  qPrintable(pageIdStr));
                return false;
              }
              else {
                // This is the normal case when a page DOES exist.
                return true;
              }
            }
            else {
              // Shouldn't happen.
              taMisc::Warning(
                "Unexpected condition; page isn't missing and doesn't exist:",
                page_name);
            }
          }
        }
      }
    }

    if (pageCount == 0) {
      // If page_name is "" or " ", for example, the entire response will be:
      //   <api/>
      taMisc::Warning("No page name provided:", page_name);
    }
    else {
      // Not sure what happened if we got here.
      taMisc::Warning("Unsure if page exists:", page_name);
    }
  }

  // If request cancelled or response malformed, assume page doesn't exist.
  return false;
}

bool taMediaWiki::DeletePage(const String& wiki_name, const String& page_name, const String& reason)
{
  // #CAT_Page Delete given page from the wiki.

  if (PageExists(wiki_name, page_name)) {
    QByteArray token = QUrl::toPercentEncoding(GetEditToken(wiki_name));
    if (token.isEmpty()) { return false; }

    String wikiUrl = GetApiURL(wiki_name);
    QUrl url(wikiUrl);

#if (QT_VERSION >= 0x050000)
    QUrlQuery urq;
    urq.addQueryItem("action", "delete");
    urq.addQueryItem("title", page_name);
    if (!reason.empty()) {
      urq.addQueryItem("reason", reason);
    }
    urq.addQueryItem("format", "xml");
    urq.addQueryItem("token", token);
    url.setQuery(urq);
#else
    url.addQueryItem("action", "delete");
    url.addQueryItem("title", page_name);
    if (!reason.empty()) {
      url.addQueryItem("reason", reason);
    }
    url.addQueryItem("format", "xml");
    url.addQueryItem("token", token);
#endif

    // Make the network request.
    iSynchronousNetRequest request;
    if (QNetworkReply *reply = request.httpPost(url)) {
      QXmlStreamReader reader(reply);
      while(!reader.atEnd()) {
        if (reader.readNext() == QXmlStreamReader::StartElement) {
          QXmlStreamAttributes attrs = reader.attributes();
          if(reader.hasError()) {
            QString err_code = attrs.value("code").toString();
            QString err_info = attrs.value("info").toString();
            taMisc::Warning("Page deletion failed with error code: ", qPrintable(err_code), " (", qPrintable(err_info), ")");
            
            return false;
          }
          else {
            taMisc::Info("Successfully deleted", page_name, "page on", wiki_name, "wiki!");
            
            return true;
          }
        }
      }
    }
    taMisc::Warning("Page delete request failed -- check the wiki/page names");
    return false;
  }
  else {
    taMisc::Warning("Cannot find given page on the given wiki!");
    return false;
  }
}

bool taMediaWiki::FindMakePage(const String& wiki_name, const String& page_name,
                               const String& page_content, const String& page_category)
{
  // #CAT_Page Find or create given page on the wiki and populate it with given content if non-empty -- return true on success.

  // Given page exists on wiki.
  if(PageExists(wiki_name, page_name)) {
    // Append page with given content.
    return EditPage(wiki_name, page_name, page_content);
  }
  // Given page does not exist on wiki.
  else {
    // Create page and populate it with given content.
    return CreatePage(wiki_name, page_name, page_content);
  }
}

bool taMediaWiki::CreatePage(const String& wiki_name, const String& page_name,
                             const String& page_content, const String& page_category)
{
  // #CAT_Page Create given page on the wiki and populate it with given content.

  if (!(PageExists(wiki_name, page_name))) {
    QByteArray token = QUrl::toPercentEncoding(GetEditToken(wiki_name));
    if (token.isEmpty()) { return false; }

    String wikiUrl = GetApiURL(wiki_name);
    QUrl url(wikiUrl);

#if (QT_VERSION >= 0x050000)
    QUrlQuery urq;
    urq.addQueryItem("action", "edit");
    urq.addQueryItem("title", page_name);
    urq.addQueryItem("section", "0");
    urq.addQueryItem("createonly", "");
    urq.addQueryItem("text", page_content);
    urq.addQueryItem("format", "xml");
    urq.addQueryItem("token", token);
    url.setQuery(urq);
#else
    url.addQueryItem("action", "edit");
    url.addQueryItem("title", page_name);
    url.addQueryItem("section", "0");
    url.addQueryItem("createonly", "");
    url.addQueryItem("text", page_content);
    url.addQueryItem("format", "xml");
    url.addQueryItem("token", token);
#endif

    // Make the network request.
    iSynchronousNetRequest request;
    if (QNetworkReply *reply = request.httpPost(url)) {
      QXmlStreamReader reader(reply);
      while(!reader.atEnd()) {
        if (reader.readNext() == QXmlStreamReader::StartElement) {
          QXmlStreamAttributes attrs = reader.attributes();
          if(reader.hasError()) {
            QString err_code = attrs.value("code").toString();
            QString err_info = attrs.value("info").toString();
            taMisc::Warning("Page creation failed with error code: ", qPrintable(err_code), " (", qPrintable(err_info), ")");
            
            return false;
          }
          else {
            taMisc::Info("Successfully created", page_name, "page on", wiki_name, "wiki!");
            
            return true;
          }
        }
      }
    }
    taMisc::Warning("Page create request failed -- check the wiki/page names");
    return false;
  }
  else {
    taMisc::Warning("Page already exists!  Call FindMakePage to make edits to existing pages");
    return false;
  }
}

bool taMediaWiki::EditPage(const String& wiki_name, const String& page_name,
                           const String& page_content, const String& page_category)
{
  // #CAT_Page Append given page on the wiki with given content.

  if (PageExists(wiki_name, page_name)) {
    QByteArray token = QUrl::toPercentEncoding(GetEditToken(wiki_name));
    if (token.isEmpty()) { return false; }

    String wikiUrl = GetApiURL(wiki_name);
    QUrl url(wikiUrl);

#if (QT_VERSION >= 0x050000)
    QUrlQuery urq;
    urq.addQueryItem("action", "edit");
    urq.addQueryItem("title", page_name);
    urq.addQueryItem("section", "new");
    urq.addQueryItem("nocreate", "");
    urq.addQueryItem("appendtext", page_content);
    urq.addQueryItem("format", "xml");
    urq.addQueryItem("token", token);
    url.setQuery(urq);
#else
    url.addQueryItem("action", "edit");
    url.addQueryItem("title", page_name);
    url.addQueryItem("section", "new");
    url.addQueryItem("nocreate", "");
    url.addQueryItem("appendtext", page_content);
    url.addQueryItem("format", "xml");
    url.addQueryItem("token", token);
#endif

    // Make the network request.
    iSynchronousNetRequest request;
    if (QNetworkReply *reply = request.httpPost(url)) {
      QXmlStreamReader reader(reply);
      while(!reader.atEnd()) {
        if (reader.readNext() == QXmlStreamReader::StartElement) {
          QXmlStreamAttributes attrs = reader.attributes();
          if(reader.hasError()) {
            QString err_code = attrs.value("code").toString();
            QString err_info = attrs.value("info").toString();
            taMisc::Warning("Page edit failed with error code: ", qPrintable(err_code), " (", qPrintable(err_info), ")");
            
            return false;
          }
          else {
            taMisc::Info("Successfully edited", page_name, "page on", wiki_name, "wiki!");
            
            return true;
          }
        }
      }
    }
    taMisc::Warning("Page edit request failed -- check the wiki/page names");
    return false;
  }
  else {
    taMisc::Warning("Page does not exist!  Call FindMakePage to create a new page");
    return false;
  }
}

/////////////////////////////////////////////////////
//            WIKI OPERATIONS

String taMediaWiki::GetApiURL(const String& wiki_name)
{
  // #CAT_Wiki Get the URL for the wiki API.

  bool appendIndexPhp = false;
  String wiki_url = taMisc::GetWikiURL(wiki_name, appendIndexPhp);
  if (wiki_url.empty()) {
    taMisc::Error("taMediaWiki::GetApiURL", "wiki named:", wiki_name,
                  "not found in global preferences/options under wiki_url settings");
    return _nilString;
  }
  return wiki_url + "/api.php";
}

String taMediaWiki::GetEditToken(const String& wiki_name)
{
  // #CAT_Wiki On success, return a String containing an unencoded edit token for the wiki (need to percent-encode this to make push requests to the API directly through a URL query, as in CreatePage) -- On failure, return an empty String.
  
  String wikiUrl = GetApiURL(wiki_name);
  if (wikiUrl.empty()) { return _nilString; }

  QUrl url(wikiUrl);

#if (QT_VERSION >= 0x050000)
  QUrlQuery urq;
  urq.addQueryItem("action", "tokens");
  urq.addQueryItem("type", "edit");
  urq.addQueryItem("format", "xml");
  url.setQuery(urq);
#else
  url.addQueryItem("action", "tokens");
  url.addQueryItem("type", "edit");
  url.addQueryItem("format", "xml");
#endif

  // Make the network request.
  iSynchronousNetRequest request;
  if (QNetworkReply *reply = request.httpPost(url)) {

    QXmlStreamReader reader(reply);
    if(reader.hasError()) { return _nilString; }

    while(!reader.atEnd()) {
      if (reader.readNext() == QXmlStreamReader::StartElement) {
        QXmlStreamAttributes attrs = reader.attributes();
        QString token = attrs.value("edittoken").toString();
        if(!token.isEmpty()) {
          taMisc::Info("Edit token retrieval successful");
          return token.toUtf8().constData();
        }
      }
    }
  }
  return _nilString;
}

bool taMediaWiki::PublishProject(const String& wiki_name, const String& proj_filename,
                                   const String& page_content, const String& proj_category)
{
  // #CAT_Wiki Create or edit the wiki page for this project, upload all files in the local project directory, then post links to these files on the project's wiki page

  // STUB

  return false;
}
