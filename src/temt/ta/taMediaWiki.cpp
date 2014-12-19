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
#include <QDir>
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
  // #CAT_Account Get the name of the currently logged-in user.

  // Make sure wiki name is valid before doing anything else.
  String wikiUrl = GetApiURL(wiki_name);
  if (wikiUrl.empty()) { return ""; }

  // Build the request URL.
  // .../api.php?action=query&format=xml&meta=userinfo
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
  // #CAT_Account Log the given username into the wiki, prompting user for password.

  // Make sure wiki name is valid before doing anything else.
  String wikiUrl = GetApiURL(wiki_name);
  if (wikiUrl.empty()) { return false; }

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

  // Build the request URL for the first stage.
  // .../api.php?action=login&format=xml&lgname=<qUsername>&lgpassword=<qPassword>
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
  // Note: The reply will be deleted when the request goes out of scope.
  iSynchronousNetRequest request;
  for (int stage = 0; stage < 2; ++stage) {
    if (QNetworkReply *reply = request.httpPost(url)) {
      // Find the <login> tag.
      QXmlStreamReader reader(reply);
      if (!findNextElement(reader, "login")) {
        taMisc::Error("Malformed response logging in to ", wiki_name, "wiki");
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
        // This indicates we need to do the second stage.  Build the URL
        // for the second stage before the attrs goes out of scope.
        // .../api.php?action=login&format=xml&lgname=<qUsername>&lgpassword=<qPassword>&lgtoken=<token>
        taMisc::DebugInfo("Performing 2-stage login to", wiki_name, "wiki.");
#if (QT_VERSION >= 0x050000)
        urq.addQueryItem("lgtoken", attrs.value("token").toString());
        url.setQuery(urq);
#else
        url.addQueryItem("lgtoken", attrs.value("token").toString());
#endif
      }
      else {
        taMisc::Error("Unexpected error during login:", qPrintable(result));
        return false;
      }
    }
  }

  // The only way to get here is if we made two passes through the loop
  // and got "NeedToken" both times -- login has failed.
  taMisc::Error("Could not log in to", wiki_name, "wiki.");

  return false;
}

bool taMediaWiki::Logout(const String &wiki_name)
{
  // #CAT_Account Logout from the wiki.

  // Make sure wiki name is valid before doing anything else.
  String wikiUrl = GetApiURL(wiki_name);
  if (wikiUrl.empty()) { return false; }

  // Build the request URL.
  // .../api.php?action=logout
  QUrl url(wikiUrl);
#if (QT_VERSION >= 0x050000)
  QUrlQuery urq;
  urq.addQueryItem("action", "logout");
  url.setQuery(urq);
#else
  url.addQueryItem("action", "logout");
#endif

  // Make the network request.
  iSynchronousNetRequest request;
  return request.httpGet(url);
}

/////////////////////////////////////////////////////
//              FILE OPERATIONS

bool taMediaWiki::UploadFile(const String& wiki_name, const String& local_file_name,
                             const String& wiki_file_name)
{
  // #CAT_File Upload given file to the wiki, optionally giving it a different file name on the wiki relative to what it is locally.

  // Make sure wiki name is valid before doing anything else.
  String wikiUrl = GetApiURL(wiki_name);
  if (wikiUrl.empty()) { return false; }

  // Get the edit token for this post request.
  String token = GetEditToken(wiki_name);
  if (token.empty()) { return false; }

  // Get the custom destination file name, if the user has specified one.
  String dst_filename;
  if (wiki_file_name.empty()) { dst_filename = local_file_name; }
  else { dst_filename = wiki_file_name; }

  QUrl url(wikiUrl);

  // Make the multi-part network request (see iSynchronousNetRequest.cpp for implementation).
  // Note: The reply will be deleted when the request goes out of scope.
  iSynchronousNetRequest request;
  if (QNetworkReply *reply = request.httpPost(url, local_file_name, dst_filename, token)) {
    QXmlStreamReader reader(reply);
    
    while (!reader.atEnd()) {
      if (reader.readNext() == QXmlStreamReader::StartElement) {
        QXmlStreamAttributes attrs = reader.attributes();
        if (reader.hasError()) {
          QString err_code = attrs.value("code").toString();
          QString err_info = attrs.value("info").toString();
          taMisc::Error("File upload failed with error code:", qPrintable(err_code), "(", qPrintable(err_info), ")");
          
          return false;
        }
        else {
          taMisc::Info(dst_filename, "successfully uploaded to", wiki_name, "wiki!");
          
          return true;
        }
      }
    }
  }
  taMisc::Error("File upload request failed");
  
  return false;
}

bool taMediaWiki::DownloadFile(const String& wiki_name, const String& wiki_file_name,
                               const String& local_file_name)
{
  // #CAT_File Download given file name from the wiki, optionally giving it a different file name locally relative to what it is on the wiki.

  // Make sure wiki name is valid before doing anything else.
  String wikiUrl = GetApiURL(wiki_name);
  if (wikiUrl.empty()) { return false; }

  // Get the custom destination file name, if the user has specified one.
  String dst_filename;
  if (local_file_name.empty()) { dst_filename = wiki_file_name; }
  else { dst_filename = local_file_name; }

  // Build the request URL.
  // .../api.php?action=query&list=allimages&aiprefix=<wiki_file_name>&format=xml
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
  // Note: The replies will be deleted when the request goes out of scope.
  iSynchronousNetRequest request;
  if (QNetworkReply *reply = request.httpGet(url)) {
    QXmlStreamReader reader(reply);

    // Find the <img> tag.
    if (findNextElement(reader, "img")) {
      QXmlStreamAttributes attrs = reader.attributes();

      // Get the url of the file.
      if (attrs.hasAttribute("url")) {
        String downStr = (String) attrs.value("url").toString();
        if (!downStr.empty()) {
          taMisc::DebugInfo("Found file at URL:", downStr);
          
          // Make a new request to retrieve the file contents.
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

              // If the request was successful, write the file contents to a new file.
              if (!(replyStr.isNull())) {
                QFile file(dst_filename);
                if (file.open(QIODevice::WriteOnly)) {
                  file.write(qPrintable(replyStr));
                  file.close();
                  taMisc::Info("File", dst_filename, "successfully downloaded!");

                  return true;
                }
                else { taMisc::Error("Could not open file", dst_filename, "for writing:", qPrintable(file.errorString())); }
              }
              else { taMisc::Error("File download request returned a bad response or null file"); }
            }
#if (QT_VERSION	> 0x040500)    
            else { taMisc::Error("File download request failed with error:", qPrintable(downReply->error())); }
#else
            else { taMisc::Error("File download request failed with an error"); }
#endif
          }
          else { taMisc::Error("File download request failed"); }
        }
        else { taMisc::Error("File URL request returned an empty URL attribute"); }
      }
      else { taMisc::Error("File URL request returned no URL attribute"); }
    }
    else { taMisc::Warning("File", wiki_file_name, "not found on", wiki_name, "wiki! Check the file/wiki names"); }
  }
  else { taMisc::Error("File URL request failed"); }

  return false;
}

bool taMediaWiki::DeleteFile(const String& wiki_name, const String& file_name, const String& reason)
{
  // #CAT_File Delete given file from wiki, optionally providing a reason for the deletion.

  // Make sure wiki name is valid before doing anything else.
  String wikiUrl = GetApiURL(wiki_name);
  if (wikiUrl.empty()) { return false; }

  // Also make sure the file exists on the wiki before moving on.
  if (!FileExists(wiki_name, file_name)) {
    taMisc::Warning("Cannot find", file_name, "file on", wiki_name, "wiki");
    return false;
  }

  // Get the edit token for this post request.
  QByteArray token = QUrl::toPercentEncoding(GetEditToken(wiki_name));
  if (token.isEmpty()) { return false; }

  // Build the request URL.
  // ...api.php?action=delete&title=File:<file_name>&reason=<reason>&format=xml&token=<token>
  QUrl url(wikiUrl);
#if (QT_VERSION >= 0x050000)
  QUrlQuery urq;
  urq.addQueryItem("action", "delete");
  urq.addQueryItem("title", "File:" + file_name);
  if (!reason.empty()) { urq.addQueryItem("reason", reason); }
  urq.addQueryItem("format", "xml");
  urq.addQueryItem("token", token);
  url.setQuery(urq);
#else
  url.addQueryItem("action", "delete");
  url.addQueryItem("title", "File:" + file_name);
  if (!reason.empty()) { url.addQueryItem("reason", reason); }
  url.addQueryItem("format", "xml");
  url.addQueryItem("token", token);
#endif

  // Make the network request.
  // Note: The reply will be deleted when the request goes out of scope.
  iSynchronousNetRequest request;
  if (QNetworkReply *reply = request.httpPost(url)) {
    QXmlStreamReader reader(reply);
    while(!reader.atEnd()) {
      if (reader.readNext() == QXmlStreamReader::StartElement) {
        QXmlStreamAttributes attrs = reader.attributes();
        if(reader.hasError()) {
          QString err_code = attrs.value("code").toString();
          QString err_info = attrs.value("info").toString();
          taMisc::Error("File deletion failed with error code: ", qPrintable(err_code), " (", qPrintable(err_info), ")");
          
          return false;
        }
        else {
          taMisc::DebugInfo("Successfully deleted", file_name, "file on", wiki_name, "wiki!");
          
          return true;
        }
      }
    }
  }
  taMisc::Warning("File delete request failed -- check the wiki/file names");
  return false;
}

bool taMediaWiki::GetDirectoryContents(DataTable* results)
{
  // #CAT_File Fill results data table with all of the files contained in the current working directory -- string column "FileName" has name of file, int column "Size" has file size.

  // Make sure we have a valid data table to write to.
  if (!results) {
    taMisc::Warning("taMediaWiki::GetDirectoryContents -- results data table is NULL -- must supply a valid data table!");
    return false;
  }

  // Create a QDir object with info about the current working directory.
  QDir dir = QDir::current();
  dir.setFilter(QDir::Files | QDir::NoSymLinks);
  dir.setSorting(QDir::Name);

  // Get the list of files contained in the directory.
  QFileInfoList file_list = dir.entryInfoList();

  // Prepare the data table.
  results->RemoveAllRows();
  DataCol* fn_col = results->FindMakeCol("FileName", VT_STRING);
  DataCol* sz_col = results->FindMakeCol("Size", VT_INT);

  // For each file in the list, write the file's name & size to the data table.
  int i, total_files = file_list.size();
  if (total_files <= 0) { return false; }
  for (i = 0; i < total_files; i++) {
    QFileInfo file_info = file_list.at(i);
    String fn = String(file_info.fileName());
    int sz = (int) file_info.size();
    results->AddBlankRow();
    fn_col->SetVal(fn, -1);
    sz_col->SetVal(sz, -1);
  }

  return true;
}

bool taMediaWiki::FileExists(const String& wiki_name, const String& file_name)
{
  // #CAT_File Determine if given file exists on the wiki.

  // Make sure wiki name is valid before doing anything else.
  String wikiUrl = GetApiURL(wiki_name);
  if (wikiUrl.empty()) { return false; }

  // Build the request URL.
  // .../api.php?action=query&format=xml&list=allimages&aisort=name&aifrom=<file_name>&aiprefix=<file_name>&aiprop=timestamp|url|size|mime&ailimit=1
  QUrl url(wikiUrl);
#if (QT_VERSION >= 0x050000)
  QUrlQuery urq;
  urq.addQueryItem("action", "query");
  urq.addQueryItem("format", "xml");
  urq.addQueryItem("list", "allimages");
  urq.addQueryItem("aisort", "name");
  urq.addQueryItem("aifrom", file_name);
  urq.addQueryItem("aiprefix", file_name);
  urq.addQueryItem("aiprop", "timestamp|url|size|mime");
  urq.addQueryItem("ailimit", "1");
  url.setQuery(urq);
#else
  url.addQueryItem("action", "query");
  url.addQueryItem("format", "xml");
  url.addQueryItem("list", "allimages");
  url.addQueryItem("aisort", "name");
  url.addQueryItem("aifrom", file_name);
  url.addQueryItem("aiprefix", file_name);
  url.addQueryItem("aiprop", "timestamp|url|size|mime");
  url.addQueryItem("ailimit", "1");
#endif

  // Make the network request.
  // Note: The reply will be deleted when the request goes out of scope.
  iSynchronousNetRequest request;
  if (QNetworkReply *reply = request.httpGet(url)) {
    QXmlStreamReader reader(reply);
    // Find the <img> tag.
    while (findNextElement(reader, "img")) {
      QXmlStreamAttributes attrs = reader.attributes();
      // Get the file name, and compare it to the one we're looking for.
      String fn = String(attrs.value("name").toString());
      return (fn == file_name);
    }
  }

  return false;
}

/////////////////////////////////////////////////////
//              QUERY OPERATIONS

bool taMediaWiki::QueryPages(DataTable* results, const String& wiki_name,
                             const String& name_space, const String& start_nm,
                             const String& prefix, int max_results)
{
  // #CAT_Query Fill results data table with pages in given name space, starting at given name, and with each name starting with given prefix (empty = all) -- string column "PageTitle" has page title, int column "PageId" has page ID number.
  
  // Make sure we have a valid data table to write to.
  if (!results) {
    taMisc::Warning("taMediaWiki::QueryPages -- results data table is NULL -- must supply a valid data table!");
    return false;
  }

  // Make sure wiki name is valid before doing anything else.
  String wikiUrl = GetApiURL(wiki_name);
  if (wikiUrl.empty()) { return false; }

  // Build the request URL.
  // .../api.php?action=query&format=xml&list=allpages&apfrom=<start_nm>&apprefix=<prefix>&apnamespace=0&aplimit=<max_results>
  QUrl url(wikiUrl);
#if (QT_VERSION >= 0x050000)
  QUrlQuery urq;
  urq.addQueryItem("action", "query");
  urq.addQueryItem("format", "xml");
  urq.addQueryItem("list", "allpages");
  urq.addQueryItem("apfrom", start_nm);
  urq.addQueryItem("apprefix", prefix);
  if (name_space.empty()) { urq.addQueryItem("apnamespace", "0"); }
  else { urq.addQueryItem("apnamespace", "0"); } // TODO: convert namespace string to appropriate namespace ID (int)
  if (max_results > 0) { urq.addQueryItem("aplimit", QString::number(max_results)); }
  url.setQuery(urq);
#else
  url.addQueryItem("action", "query");
  url.addQueryItem("format", "xml");
  url.addQueryItem("list", "allpages");
  url.addQueryItem("apfrom", start_nm);
  url.addQueryItem("apprefix", prefix);
  if (name_space.empty()) { url.addQueryItem("apnamespace", "0"); }
  else { url.addQueryItem("apnamespace", "0"); } // TODO: convert namespace string to appropriate namespace ID (int)
  if (max_results > 0) { url.addQueryItem("aplimit", QString::number(max_results)); }
#endif

  // Make the network request.
  // Note: The reply will be deleted when the request goes out of scope.
  iSynchronousNetRequest request;
  if (QNetworkReply *reply = request.httpGet(url)) {

    // Prepare the data table.
    results->RemoveAllRows();
    DataCol* pt_col = results->FindMakeCol("PageTitle", VT_STRING);
    DataCol* pid_col = results->FindMakeCol("PageId", VT_INT);

    // For all <p> elements in the XML, add the page title and page ID to the data table.
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
                                       const String& category, const String& name_space,
                                       int max_results)
{
  // #CAT_Query Fill results data table with pages in given category, starting at given name, and with each name starting with given prefix (empty = all) -- string column "PageTitle" has page title, int column "PageId" has page ID number.
  
  // Make sure we have a valid data table to write to.
  if (!results) {
    taMisc::Warning("taMediaWiki::QueryPagesByCategory -- results data table is NULL -- must supply a valid data table!");
    return false;
  }

  // Make sure wiki name is valid before doing anything else.
  String wikiUrl = GetApiURL(wiki_name);
  if (wikiUrl.empty()) { return false; }

  // Build the request URL.
  // .../api.php?action=query&format=xml&list=categorymembers&cmtitle=Category:<category>&cmnamespace=0&cmtype=page|subcat&cmlimit=<max_results>
  QUrl url(wikiUrl);
#if (QT_VERSION >= 0x050000)
  QUrlQuery urq;
  urq.addQueryItem("action", "query");
  urq.addQueryItem("format", "xml");
  urq.addQueryItem("list", "categorymembers");
  urq.addQueryItem("cmtitle", "Category:" + category);
  if (name_space.empty()) { urq.addQueryItem("cmnamespace", "0"); }
  else { urq.addQueryItem("cmnamespace", "0"); } // TODO: convert namespace string to appropriate namespace ID (int)
  urq.addQueryItem("cmtype", "page|subcat");
  if (max_results > 0) { urq.addQueryItem("cmlimit", QString::number(max_results)); }
  url.setQuery(urq);
#else
  url.addQueryItem("action", "query");
  url.addQueryItem("format", "xml");
  url.addQueryItem("list", "categorymembers");
  url.addQueryItem("cmtitle", "Category:" + category);
  if (name_space.empty()) { url.addQueryItem("cmnamespace", "0"); }
  else { url.addQueryItem("cmnamespace", "0"); } // TODO: convert namespace string to appropriate namespace ID (int)
  url.addQueryItem("cmtype", "page|subcat");
  if (max_results > 0) { url.addQueryItem("cmlimit", QString::number(max_results)); }
#endif

  // Make the network request.
  // Note: The reply will be deleted when the request goes out of scope.
  iSynchronousNetRequest request;
  if (QNetworkReply *reply = request.httpGet(url)) {

    // Prepare the data table.
    results->RemoveAllRows();
    DataCol* pt_col = results->FindMakeCol("PageTitle", VT_STRING);
    DataCol* pid_col = results->FindMakeCol("PageId", VT_INT);

    // For all <cm> elements in the XML, add the page title and page ID to the data table.
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
                             const String& start_nm, const String& prefix,
                             int max_results)
{
  // #CAT_Query Fill results data table with files uploaded to wiki, starting at given name, and with each name starting with given prefix (empty = all) -- string column "FileName" has name of file, int column "Size" has file size, string column "MimeType" has mime type.
  
  // Make sure we have a valid data table to write to.
  if (!results) {
    taMisc::Warning("taMediaWiki::QueryFiles -- results data table is NULL -- must supply a valid data table!");
    return false;
  }

  // Make sure wiki name is valid before doing anything else.
  String wikiUrl = GetApiURL(wiki_name);
  if (wikiUrl.empty()) { return false; }

  // Build the request URL.
  // .../api.php?action=query&format=xml&list=allimages&aisort=name&aifrom=<start_nm>&aiprefix=<prefix>&aiprop=timestamp|url|size|mime&ailimit=<max_results>
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
  if (max_results > 0) { urq.addQueryItem("ailimit", QString::number(max_results)); }
  url.setQuery(urq);
#else
  url.addQueryItem("action", "query");
  url.addQueryItem("format", "xml");
  url.addQueryItem("list", "allimages");
  url.addQueryItem("aisort", "name");
  url.addQueryItem("aifrom", start_nm);
  url.addQueryItem("aiprefix", prefix);
  url.addQueryItem("aiprop", "timestamp|url|size|mime");
  if (max_results > 0) { url.addQueryItem("ailimit", QString::number(max_results)); }
#endif

  // Make the network request.
  // Note: The reply will be deleted when the request goes out of scope.
  iSynchronousNetRequest request;
  if (QNetworkReply *reply = request.httpGet(url)) {

    // Prepare the data table.
    results->RemoveAllRows();
    DataCol* fn_col = results->FindMakeCol("FileName", VT_STRING);
    DataCol* sz_col = results->FindMakeCol("Size", VT_INT);
    DataCol* mt_col = results->FindMakeCol("MimeType", VT_STRING);

    // For all <img> elements in the XML, add the file name, file size, and mime type to the data table.
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
                              const String& name_space, int max_results)
{
  // #CAT_Query Fill results data table with the pages matching the given search string -- if title_only is true, only search for matches in page titles; else, search for matches in page contents -- string column "PageTitle" has page title.

  // Make sure we have a valid data table to write to.
  if (!results) {
    taMisc::Warning("taMediaWiki::SearchPages -- results data table is NULL -- must supply a valid data table!");
    return false;
  }

  // Make sure wiki name is valid before doing anything else.
  String wikiUrl = GetApiURL(wiki_name);
  if (wikiUrl.empty()) { return false; }

  // Build the request URL.
  // .../api.php?action=query&format=xml&list=search&srsearch=<search_str>&srwhat=<title|text>&srlimit=<max_results>
  QUrl url(wikiUrl);
#if (QT_VERSION >= 0x050000)
  QUrlQuery urq;
  urq.addQueryItem("action", "query");
  urq.addQueryItem("format", "xml");
  urq.addQueryItem("list", "search");
  urq.addQueryItem("srsearch", search_str);
  urq.addQueryItem("srwhat", title_only ? "title" : "text");
  if (max_results > 0) { urq.addQueryItem("srlimit", QString::number(max_results)); }
  url.setQuery(urq);
#else
  url.addQueryItem("action", "query");
  url.addQueryItem("format", "xml");
  url.addQueryItem("list", "search");
  url.addQueryItem("srsearch", search_str);
  url.addQueryItem("srwhat", title_only ? "title" : "text");
  if (max_results > 0) { url.addQueryItem("srlimit", QString::number(max_results)); }
#endif

  // Make the network request.
  // Note: The reply will be deleted when the request goes out of scope.
  iSynchronousNetRequest request;
  if (QNetworkReply *reply = request.httpGet(url)) {
    
    // Prepare the data table.
    results->RemoveAllRows();
    DataCol* pt_col = results->FindMakeCol("PageTitle", VT_STRING);

    // For all <p> elements in the XML, add the page title to the data table.
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
  // #CAT_Page Determine if given page exists on the wiki.

  // Make sure wiki name is valid before doing anything else.
  String wikiUrl = GetApiURL(wiki_name);
  if (wikiUrl.empty()) { return false; }

  // Build the request URL.
  // .../api.php?action=query&format=xml&titles=<page_name>
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
  // Note: The reply will be deleted when the request goes out of scope.
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
              taMisc::Error(
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
      taMisc::Error("Unsure if page exists:", page_name);
    }
  }

  // If request cancelled or response malformed, assume page doesn't exist.
  return false;
}

bool taMediaWiki::DeletePage(const String& wiki_name, const String& page_name, const String& reason)
{
  // #CAT_Page Delete given page from the wiki.

  // Make sure wiki name is valid before doing anything else.
  String wikiUrl = GetApiURL(wiki_name);
  if (wikiUrl.empty()) { return false; }

  // Also make sure the page exists on the wiki before moving on.
  if (!PageExists(wiki_name, page_name)) {
    taMisc::Warning("Cannot find", page_name, "page on", wiki_name, "wiki!");
    return false;
  }

  // Get the edit token for this post request.
  QByteArray token = QUrl::toPercentEncoding(GetEditToken(wiki_name));
  if (token.isEmpty()) { return false; }

  // Build the request URL.
  // .../api.php?action=delete&title=<page_name>&reason=<reason>&format=xml&token=<token>
  QUrl url(wikiUrl);
#if (QT_VERSION >= 0x050000)
  QUrlQuery urq;
  urq.addQueryItem("action", "delete");
  urq.addQueryItem("title", page_name);
  if (!reason.empty()) { urq.addQueryItem("reason", reason); }
  urq.addQueryItem("format", "xml");
  urq.addQueryItem("token", token);
  url.setQuery(urq);
#else
  url.addQueryItem("action", "delete");
  url.addQueryItem("title", page_name);
  if (!reason.empty()) { url.addQueryItem("reason", reason); }
  url.addQueryItem("format", "xml");
  url.addQueryItem("token", token);
#endif

  // Make the network request.
  // Note: The reply will be deleted when the request goes out of scope.
  iSynchronousNetRequest request;
  if (QNetworkReply *reply = request.httpPost(url)) {
    QXmlStreamReader reader(reply);
    while(!reader.atEnd()) {
      if (reader.readNext() == QXmlStreamReader::StartElement) {
        QXmlStreamAttributes attrs = reader.attributes();
        if(reader.hasError()) {
          QString err_code = attrs.value("code").toString();
          QString err_info = attrs.value("info").toString();
          taMisc::Error("Page deletion failed with error code: ", qPrintable(err_code), " (", qPrintable(err_info), ")");
          
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

bool taMediaWiki::FindMakePage(const String& wiki_name, const String& page_name,
                               const String& page_content, const String& page_category)
{
  // #CAT_Page Find or create given page on the wiki and populate it with given content.

  // If given page exists on wiki...
  if(PageExists(wiki_name, page_name)) {
    // Append page with given content.
    return EditPage(wiki_name, page_name, page_content, page_category);
  }
  // If given page does not exist on wiki...
  else {
    // Create page and populate it with given content.
    return CreatePage(wiki_name, page_name, page_content, page_category);
  }
}

bool taMediaWiki::CreatePage(const String& wiki_name, const String& page_name,
                             const String& page_content, const String& page_category)
{
  // #CAT_Page Create a page on the wiki with given name, and populate it with given content.

  // Make sure wiki name is valid before doing anything else.
  String wikiUrl = GetApiURL(wiki_name);
  if (wikiUrl.empty()) { return false; }

  // Also make sure the page does not exist on the wiki before moving on.
  if (PageExists(wiki_name, page_name)) {
    taMisc::Warning("Page", page_name, "already exists on", wiki_name, "wiki! Call FindMakePage to make edits to existing pages");
    return false;
  }

  // Get the edit token for this post request.
  QByteArray token = QUrl::toPercentEncoding(GetEditToken(wiki_name));
  if (token.isEmpty()) { return false; }

  // Build the request URL.
  // .../api.php?action=edit&title=<page_name>&section=new&preload=Template:Project&createonly=&text=<page_content>&format=xml&token=<token>
  QUrl url(wikiUrl);
#if (QT_VERSION >= 0x050000)
  QUrlQuery urq;
  urq.addQueryItem("action", "edit");
  urq.addQueryItem("title", page_name);
  urq.addQueryItem("section", "new");
  urq.addQueryItem("createonly", "");
  urq.addQueryItem("text", page_content);
  urq.addQueryItem("format", "xml");
  urq.addQueryItem("token", token);
  url.setQuery(urq);
#else
  url.addQueryItem("action", "edit");
  url.addQueryItem("title", page_name);
  url.addQueryItem("section", "new");
  url.addQueryItem("createonly", "");
  url.addQueryItem("text", page_content);
  url.addQueryItem("format", "xml");
  url.addQueryItem("token", token);
#endif

  // Make the network request.
  // Note: The reply will be deleted when the request goes out of scope.
  iSynchronousNetRequest request;
  if (QNetworkReply *reply = request.httpPost(url)) {
    QXmlStreamReader reader(reply);
    while(!reader.atEnd()) {
      if (reader.readNext() == QXmlStreamReader::StartElement) {
        QXmlStreamAttributes attrs = reader.attributes();
        if(reader.hasError()) {
          QString err_code = attrs.value("code").toString();
          QString err_info = attrs.value("info").toString();
          taMisc::Error("Page creation failed with error code: ", qPrintable(err_code), " (", qPrintable(err_info), ")");
          
          return false;
        }
        else {
          taMisc::Info("Successfully created", page_name, "page on", wiki_name, "wiki!");
          
          if (!page_category.empty()) {
            return AddCategories(wiki_name, page_name, page_category);
          }
          else { return true; }
        }
      }
    }
  }
  taMisc::Warning("Page create request failed -- check the wiki/page names");
  return false;
}

bool taMediaWiki::EditPage(const String& wiki_name, const String& page_name,
                           const String& page_content, const String& page_category)
{
  // #CAT_Page Append given page on the wiki with given content.

  // Make sure wiki name is valid before doing anything else.
  String wikiUrl = GetApiURL(wiki_name);
  if (wikiUrl.empty()) { return false; }

  // Also make sure the page exists on the wiki before moving on.
  if (!PageExists(wiki_name, page_name)) {
    taMisc::Warning("Page", page_name, "not found on", wiki_name, "wiki! Call FindMakePage to create a new page");
    return false;
  }

  // Get the edit token for this post request.
  QByteArray token = QUrl::toPercentEncoding(GetEditToken(wiki_name));
  if (token.isEmpty()) { return false; }

  // Build the request URL.
  // .../api.php?action=edit&title=<page_name>&nocreate=&appendtext=<page_content>&format=xml&token=<token>
  QUrl url(wikiUrl);
#if (QT_VERSION >= 0x050000)
  QUrlQuery urq;
  urq.addQueryItem("action", "edit");
  urq.addQueryItem("title", page_name);
  urq.addQueryItem("nocreate", "");
  urq.addQueryItem("appendtext", page_content);
  urq.addQueryItem("format", "xml");
  urq.addQueryItem("token", token);
  url.setQuery(urq);
#else
  url.addQueryItem("action", "edit");
  url.addQueryItem("title", page_name);
  url.addQueryItem("nocreate", "");
  url.addQueryItem("appendtext", page_content);
  url.addQueryItem("format", "xml");
  url.addQueryItem("token", token);
#endif

  // Make the network request.
  // Note: The reply will be deleted when the request goes out of scope.
  iSynchronousNetRequest request;
  if (QNetworkReply *reply = request.httpPost(url)) {
    QXmlStreamReader reader(reply);
    while(!reader.atEnd()) {
      if (reader.readNext() == QXmlStreamReader::StartElement) {
        QXmlStreamAttributes attrs = reader.attributes();
        if(reader.hasError()) {
          QString err_code = attrs.value("code").toString();
          QString err_info = attrs.value("info").toString();
          taMisc::Error("Page edit failed with error code: ", qPrintable(err_code), " (", qPrintable(err_info), ")");
          
          return false;
        }
        else {
          taMisc::Info("Successfully edited", page_name, "page on", wiki_name, "wiki!");
          if (!page_category.empty()) {
            return AddCategories(wiki_name, page_name, page_category);
          }
          else { return true; }
        }
      }
    }
  }
  taMisc::Warning("Page edit request failed -- check the wiki/page names");
  return false;
}

bool taMediaWiki::AddCategories(const String& wiki_name, const String& page_name, const String& page_category)
{
  // #CAT_Page Append given page on the wiki with given list of comma-separated categories.

  // Make sure we actually have categories to add.
  if (page_category.empty()) { return false; }

  // Make sure wiki name is valid before doing anything else.
  String wikiUrl = GetApiURL(wiki_name);
  if (wikiUrl.empty()) { return false; }

  // Also make sure the page exists on the wiki before moving on.
  if (!PageExists(wiki_name, page_name)) {
    taMisc::Warning("Page", page_name, "not found on", wiki_name, "wiki! Make sure you've created it first.");
    return false;
  }

  // Build the URL-encoded category list.
  String categories = "";
  String cats = page_category;
  String_Array* cat_array = cats.split(",");
  int i, cat_sz = cat_array->size;
  for (i = 0; i < cat_sz; i++) {
    categories += "[[Category:" + trim(cat_array->FastEl(i)) + "]]";
  }

  // Get the edit token for this post request.
  QByteArray token = QUrl::toPercentEncoding(GetEditToken(wiki_name));
  if (token.isEmpty()) { return false; }

  // Build the request URL.
  // .../api.php?action=edit&title=<page_name>&nocreate=&appendtext=<categories>&format=xml&token=<token>
  QUrl url(wikiUrl);
#if (QT_VERSION >= 0x050000)
  QUrlQuery urq;
  urq.addQueryItem("action", "edit");
  urq.addQueryItem("title", page_name);
  urq.addQueryItem("nocreate", "");
  urq.addQueryItem("appendtext", categories);
  urq.addQueryItem("format", "xml");
  urq.addQueryItem("token", token);
  url.setQuery(urq);
#else
  url.addQueryItem("action", "edit");
  url.addQueryItem("title", page_name);
  url.addQueryItem("nocreate", "");
  url.addQueryItem("appendtext", categories);
  url.addQueryItem("format", "xml");
  url.addQueryItem("token", token);
#endif

  // Make the network request.
  // Note: The reply will be deleted when the request goes out of scope.
  iSynchronousNetRequest request;
  if (QNetworkReply *reply = request.httpPost(url)) {
    QXmlStreamReader reader(reply);
    while(!reader.atEnd()) {
      if (reader.readNext() == QXmlStreamReader::StartElement) {
        if(reader.hasError()) {
          QXmlStreamAttributes attrs = reader.attributes();
          QString err_code = attrs.value("code").toString();
          QString err_info = attrs.value("info").toString();
          taMisc::Error("Add category failed with error code: ", qPrintable(err_code), " (", qPrintable(err_info), ")");
          
          return false;
        }
        else {
          taMisc::Info("Successfully added categories to", page_name, "page on", wiki_name, "wiki!");
          
          return true;
        }
      }
    }
  }
  taMisc::Warning("Add category request failed -- check the wiki/page names");
  return false;
}

bool taMediaWiki::LinkFile(const String& file_name, const String& wiki_name, const String& page_name)
{
  // #CAT_Page Append given page on the wiki with a link to given uploaded file.

  // Make sure wiki name is valid before doing anything else.
  String wikiUrl = GetApiURL(wiki_name);
  if (wikiUrl.empty()) { return false; }

  // Also make sure the page exists on the wiki before moving on.
  if (!PageExists(wiki_name, page_name)) {
    taMisc::Warning("Page", page_name, "not found on", wiki_name, "wiki! Make sure you've created it first.");
    return false;
  }

  // Build the URL-encoded file name, checking to make sure the file exists on the wiki.
  String file_link;
  if (FileExists(wiki_name, file_name)) {
    file_link = "[[File:" + file_name + "]]";
  }
  else {
    taMisc::Warning("File", file_name, "not found on", wiki_name, "wiki! Make sure you've uploaded it first.");
    return false;
  }

  // Get the edit token for this post request.
  QByteArray token = QUrl::toPercentEncoding(GetEditToken(wiki_name));
  if (token.isEmpty()) { return false; }

  // Build the request URL.
  // .../api.php?action=edit&title=<page_name>&nocreate=&appendtext=<file_link>&format=xml&token=<token>
  QUrl url(wikiUrl);
#if (QT_VERSION >= 0x050000)
  QUrlQuery urq;
  urq.addQueryItem("action", "edit");
  urq.addQueryItem("title", page_name);
  urq.addQueryItem("nocreate", "");
  urq.addQueryItem("appendtext", file_link);
  urq.addQueryItem("format", "xml");
  urq.addQueryItem("token", token);
  url.setQuery(urq);
#else
  url.addQueryItem("action", "edit");
  url.addQueryItem("title", page_name);
  url.addQueryItem("nocreate", "");
  url.addQueryItem("appendtext", file_link);
  url.addQueryItem("format", "xml");
  url.addQueryItem("token", token);
#endif

  // Make the network request.
  // Note: The reply will be deleted when the request goes out of scope.
  iSynchronousNetRequest request;
  if (QNetworkReply *reply = request.httpPost(url)) {
    QXmlStreamReader reader(reply);
    while(!reader.atEnd()) {
      if (reader.readNext() == QXmlStreamReader::StartElement) {
        if(reader.hasError()) {
          QXmlStreamAttributes attrs = reader.attributes();
          QString err_code = attrs.value("code").toString();
          QString err_info = attrs.value("info").toString();
          taMisc::Error("File link failed with error code: ", qPrintable(err_code), " (", qPrintable(err_info), ")");
          
          return false;
        }
        else {
          taMisc::Info("Successfully linked", file_name, "file to", page_name, "page on", wiki_name, "wiki!");
          
          return true;
        }
      }
    }
  }
  taMisc::Warning("File link request failed -- check the wiki/page names");
  return false;
}

bool taMediaWiki::LinkFiles(DataTable* files, const String& wiki_name, const String& page_name)
{
  // #CAT_Page Append given page on the wiki with links to given list of uploaded files.

  // Make sure wiki name is valid before doing anything else.
  String wikiUrl = GetApiURL(wiki_name);
  if (wikiUrl.empty()) { return false; }

  // Make sure we have a valid data table to write to.
  if (!files) {
    taMisc::Warning("taMediaWiki::LinkFiles -- files data table is NULL -- must supply a valid data table!");
    return false;
  }

  // Also make sure the page exists on the wiki before moving on.
  if (!PageExists(wiki_name, page_name)) {
    taMisc::Warning("Page", page_name, "not found on", wiki_name, "wiki! Make sure you've created it first.");
    return false;
  }

  // Build the URL-encoded file name list, checking to make sure each file exists on the wiki.
  String file_links = "";
  int i, total_files = files->rows;
  for (i = 0; i < total_files; i++) {
    String file_name = files->GetVal(0, i).toString();
    if (FileExists(wiki_name, file_name)) {
      file_links += "[[File:" + file_name + "]]";
    }
  }
  if (file_links.empty()) {
    taMisc::Warning("Files not found on", wiki_name, "wiki! Make sure you've uploaded them first.");
    return false;
  }

  // Get the edit token for this post request.
  QByteArray token = QUrl::toPercentEncoding(GetEditToken(wiki_name));
  if (token.isEmpty()) { return false; }

  // Build the request URL.
  // .../api.php?action=edit&title=<page_name>&nocreate=&appendtext=<file_links>&format=xml&token=<token>
  QUrl url(wikiUrl);
#if (QT_VERSION >= 0x050000)
  QUrlQuery urq;
  urq.addQueryItem("action", "edit");
  urq.addQueryItem("title", page_name);
  urq.addQueryItem("nocreate", "");
  urq.addQueryItem("appendtext", file_links);
  urq.addQueryItem("format", "xml");
  urq.addQueryItem("token", token);
  url.setQuery(urq);
#else
  url.addQueryItem("action", "edit");
  url.addQueryItem("title", page_name);
  url.addQueryItem("nocreate", "");
  url.addQueryItem("appendtext", file_links);
  url.addQueryItem("format", "xml");
  url.addQueryItem("token", token);
#endif

  // Make the network request.
  // Note: The reply will be deleted when the request goes out of scope.
  iSynchronousNetRequest request;
  if (QNetworkReply *reply = request.httpPost(url)) {
    QXmlStreamReader reader(reply);
    while(!reader.atEnd()) {
      if (reader.readNext() == QXmlStreamReader::StartElement) {
        if(reader.hasError()) {
          QXmlStreamAttributes attrs = reader.attributes();
          QString err_code = attrs.value("code").toString();
          QString err_info = attrs.value("info").toString();
          taMisc::Error("File link failed with error code: ", qPrintable(err_code), " (", qPrintable(err_info), ")");
          
          return false;
        }
        else {
          taMisc::Info("Successfully linked files to", page_name, "page on", wiki_name, "wiki!");
          
          return true;
        }
      }
    }
  }
  taMisc::Warning("File link request failed -- check the wiki/page names");
  return false;
}

/////////////////////////////////////////////////////
//            WIKI OPERATIONS

String taMediaWiki::GetApiURL(const String& wiki_name)
{
  // #CAT_Wiki Get the URL for the wiki API.

  // Get the URL corresponding to wiki_name.
  bool appendIndexPhp = false;
  String wiki_url = taMisc::GetWikiURL(wiki_name, appendIndexPhp);
  if (wiki_url.empty()) {
    taMisc::Warning("taMediaWiki::GetApiURL", "wiki named:", wiki_name,
                  "not found in global preferences/options under wiki_url settings");
    return _nilString;
  }

  // Append the URL with "/api.php".
  return wiki_url + "/api.php";
}

String taMediaWiki::GetEditToken(const String& wiki_name)
{
  // #CAT_Wiki Return a String containing an unencoded edit token for the wiki (need to percent-encode this to make push requests to the API directly through a URL query, as in CreatePage).
  
  // Make sure wiki name is valid before doing anything else.
  String wikiUrl = GetApiURL(wiki_name);
  if (wikiUrl.empty()) { return _nilString; }

  // Build the request URL.
  // .../api.php?action=tokens&type=edit&format=xml
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
  // Note: The reply will be deleted when the request goes out of scope.
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

bool taMediaWiki::PublishProject(const String& wiki_name, const String& page_name,
                                 const String& proj_name, const String& proj_filename,
                                 const String& proj_desc, const String& proj_category)
{
  // #CAT_Wiki Create or edit the wiki page for this project, upload the project file to the wiki, then post a link to this file on the project's wiki page.
  
  // First check to make sure the page doesn't already exist.
  if (PageExists(wiki_name, page_name)) {
    taMisc::Warning(page_name, "page already exists on", wiki_name, "wiki! Call FindMakePage to make edits");
    return false;
  }
  // TODO: Finish designing the 'Template:Project' page on the wiki, and give it arguments for each variable by replacing that spot with {{{arg_name}}}
  //       Supply any such arguments in the page_content string.  See the 'Template:Project' and 'Template_Test' pages on the emergent-test wiki for examples.
  String page_content = "{{Project|name=" + proj_name + "|desc=" + proj_desc + "}}";

  // If the project filename is empty, the user does not want to upload the project file. Just create/edit the project page.
  if (proj_filename.empty()) { return CreatePage(wiki_name, page_name, page_content, proj_category); }

  // Due to short-circuit evaluation, each of these methods must return true before the next method is called.
  return (CreatePage(wiki_name, page_name, page_content, proj_category) &&
          UploadFile(wiki_name, proj_filename, "") &&
          LinkFile(proj_filename, wiki_name, page_name));
}
