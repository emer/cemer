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
#include <iDialogPublishDocs>
#include <algorithm>
#include <taMisc>
#include <taProject>

#include <QFile>
#include <QDir>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QXmlStreamReader>
#include <QEventLoop>
#include <QByteArray>
#include <QDebug>
#if (QT_VERSION >= 0x040800)
#include <QHttpPart>
#include <QHttpMultiPart>
#endif
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

/**
 * When a mediaWiki API call fails, it returns an XML document with an "error" element in it, that has the error details
 * Parse the document and then return either true if it has an error or false if not. It prints then prints any error messages
 */
bool taMediaWiki::CheckResponseError(const QString &xmlResponse) {
  bool hasErrors = false;
  QXmlStreamReader reader(xmlResponse.toStdString().c_str());
  
  while(!reader.atEnd()) {
    QXmlStreamReader::TokenType token = reader.readNext();
    
    ///Check if the actual XML document has an error during parsing
    if(reader.hasError()) {
      hasErrors = true;
      taMisc::Error("MediaWiki Api call returned invalid XML: " + xmlResponse);
      //taMisc::Warning(xmlResponse);
      return hasErrors;
    }
    if (token == QXmlStreamReader::StartElement) {
      if (reader.name() == "error") {
        QXmlStreamAttributes attrs = reader.attributes();
        QString err_code = attrs.value("code").toString();
        QString err_info = attrs.value("info").toString();
        QString err_text = reader.readElementText();
        taMisc::Error("MediaWiki API call failed with error code: ", err_code, " (", err_info, ") ", err_text);
        taMisc::Warning(xmlResponse);
        hasErrors = true;
      }
      if (reader.name() == "warnings") {
        QString err_text = reader.readElementText();
        taMisc::Warning("MediaWiki API call contained warnings: ",  err_text);
        taMisc::Info(xmlResponse);
      }
    } else if (token == QXmlStreamReader::Invalid) {
      hasErrors = true;
      taMisc::Error("MediaWiki Api call returned invalid XML");
      taMisc::Warning(xmlResponse);
      return hasErrors;
    }
  }
  return hasErrors;
}

/////////////////////////////////////////////////////
//            ACCOUNT OPERATIONS

String taMediaWiki::GetLoggedInUsername(const String &wiki_name)
{
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
    QString apiResponse(reply->readAll());
    if (CheckResponseError(apiResponse)) {
      taMisc::Error("Could not retrieve userinfo");
      return "";
    }
    // Find the <userinfo> tag and check for a non-zero ID.
    QXmlStreamReader reader(apiResponse.toStdString().c_str());
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
  message = message.arg(wiki_name.toQString()).arg(wikiUrl.toQString());

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
      QString apiResponse(reply->readAll());
      if (CheckResponseError(apiResponse)) {
        taMisc::Error("Could not log in to", wiki_name, "wiki.");
        return false;
      }
      QXmlStreamReader reader(apiResponse.toStdString().c_str());
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
        taMisc::Confirm("Username", qPrintable(qUsername), " not recognized on wiki ", wiki_name);
        return false;
      }
      else if (result == "WrongPass") {
        taMisc::Confirm("Incorrect password for user", qPrintable(qUsername),
          "on wiki", wiki_name);
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

bool taMediaWiki::UploadFile(const String& wiki_name, const String& local_file_name, bool new_revision, const String& wiki_file_name)
{
  // Make sure wiki name is valid before doing anything else.
  String wikiUrl = GetApiURL(wiki_name);
  if (wikiUrl.empty()) { return false; }

  // Get the custom destination file name, if the user has specified one.
  String dst_filename;
  if (wiki_file_name.empty()) {
    dst_filename = local_file_name.after('/', -1);
  }
  else {
    dst_filename = wiki_file_name;
  }
  
  // Get the edit token for this post request.
  String token = GetEditToken(wiki_name);
  if (token.empty()) { return false; }
  
  QUrl url(wikiUrl);
  
  // Make the multi-part network request (see iSynchronousNetRequest.cpp for implementation).
  // Note: The reply will be deleted when the request goes out of scope.
  iSynchronousNetRequest request;
  if (QNetworkReply *reply = request.httpPost(url, local_file_name, dst_filename, token)) {
    QString apiResponse(reply->readAll());
    if (CheckResponseError(apiResponse)) {
      taMisc::Error("Failed to upload local file", local_file_name, "to", wiki_file_name, "on", wiki_name, "wiki.");
      return false;
    }
    
  }
  return true;
}

bool taMediaWiki::UploadFileAndLink(const String& wiki_name, const String& proj_name, const String& local_file_name, bool new_revision, const String& wiki_file_name)
{
  bool proceed = false;
  proceed = UploadFile(wiki_name, local_file_name, new_revision, wiki_file_name);

  if (proceed) {
    String filename_only = local_file_name.after('/', -1);
    proceed = LinkFile(filename_only, wiki_name, proj_name);  // link the file to the project page
    AppendFileType(wiki_name, filename_only, "other");  // "other" means not a .proj file - we could add .wts, .dat etc
  }
  return proceed;
}

bool taMediaWiki::DownloadFile(const String& wiki_name, const String& wiki_file_name,
                               const String& local_file_name)
{
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
#if (QT_VERSION	> 0x040800)
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
    QString apiResponse(reply->readAll());
    if (CheckResponseError(apiResponse)) {
      taMisc::Error("File delete request for", file_name, "on", wiki_name, "wiki failed.");
      return false;
    }
    taMisc::DebugInfo("Successfully deleted", file_name, "file on", wiki_name, "wiki!");
    return true;
  }
  taMisc::Warning("File delete request failed -- check the wiki/file names");
  return false;
}

bool taMediaWiki::GetDirectoryContents(DataTable* results)
{
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

// TODO: Rohrlich 2/21/15
// in addition to project page existing - check that it has category etc - i.e. not conincidence of page name
bool taMediaWiki::IsPublished(const String& wiki_name, const String& project_name) {
  if (PageExists(wiki_name, project_name)) {
    return true;
  }
  else {
    return false;
  }
}

bool taMediaWiki::FileExists(const String& wiki_name, const String& file_name, bool quiet)
{
  // Make sure wiki name is valid before doing anything else.
  String wikiUrl = GetApiURL(wiki_name);
  if (wikiUrl.empty()) { return false; }
  
  String full_file_name = "File:" + file_name;
  
  // Build the request URL.
  // .../api.php?action=query&format=xml&titles=<page_name>
  QUrl url(wikiUrl);
#if (QT_VERSION >= 0x050000)
  QUrlQuery urq;
  urq.addQueryItem("action", "query");
  urq.addQueryItem("format", "xml");
  urq.addQueryItem("titles", full_file_name);
  url.setQuery(urq);
#else
  url.addQueryItem("action", "query");
  url.addQueryItem("format", "xml");
  url.addQueryItem("titles", full_file_name);
#endif
  
  
  // Default the normalized name to the provided page name;
  // will be changed if normalization was performed by the server.
  String normalizedName = full_file_name;
  
  iSynchronousNetRequest request;
  if (QNetworkReply *reply = request.httpGet(url)) {
    QString apiResponse(reply->readAll());
    if (CheckResponseError(apiResponse)) {
      taMisc::Error("Could not determine if file", file_name, "exists on", wiki_name, "wiki.");
      return false;
    }
    QXmlStreamReader reader(apiResponse.toStdString().c_str());
    
    while (!reader.atEnd()) {
      if (reader.readNext() == QXmlStreamReader::StartElement) {
        // Check for a <page> element
        if (reader.name() == "page") {
          QXmlStreamAttributes attrs = reader.attributes();
          // Check if the page name is marked as invalid or missing.
          if (attrs.hasAttribute("invalid")) {
            // For example, the page name "_" is invalid.
            taMisc::Warning("Wike file name " + file_name + " is invalid");
            return false;
          }
          else if (attrs.hasAttribute("missing")) {
            // This is the normal case when a page does NOT exist.
            if (!quiet) {
              taMisc::Error("Wiki file " + file_name + " not found");
            }
            return false;
          }
          else if (attrs.hasAttribute("pageid")) {
            taMisc::DebugInfo("pageid");
          }
        }
        if (reader.name() == "result") {
          taMisc::DebugInfo("result");
        }
      }
    }
  }
  return true;
}

/////////////////////////////////////////////////////
//              QUERY OPERATIONS

bool taMediaWiki::QueryPages(DataTable* results, const String& wiki_name,
                             const String& name_space, const String& start_nm,
                             const String& prefix, int max_results)
{
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
  // Make sure we have a valid data table to write to.
  if (!results) {
    taMisc::Warning("taMediaWiki::QueryPagesByCategory -- results data table is NULL -- must supply a valid data table!");
    return false;
  }

  // Make sure wiki name is valid before doing anything else.
  String wikiUrl = GetApiURL(wiki_name);
  if (wikiUrl.empty()) { return false; }
  // Prepare the data table.
  results->RemoveAllRows();
  DataCol* pt_col = results->FindMakeCol("PageTitle", VT_STRING);
  DataCol* pid_col = results->FindMakeCol("PageId", VT_INT);

  
  bool gotAllResults = false;
  String continutationURL = "";

  // Build the request URL.
  // .../api.php?action=query&format=xml&list=categorymembers&cmtitle=Category:<category>&cmnamespace=0&cmtype=page|subcat&cmlimit=<max_results>
  while (!gotAllResults) {
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
    if (!continutationURL.empty()) { urq.addQueryItem("cmcontinue",continutationURL);}
  
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
    if (!continutationURL.empty()) { url.addQueryItem("cmcontinue",continutationURL);}
#endif

    // Make the network request.
    // Note: The reply will be deleted when the request goes out of scope.
    iSynchronousNetRequest request;
    if (QNetworkReply *reply = request.httpGet(url)) {

      // For all <cm> elements in the XML, add the page title and page ID to the data table.
      QString apiResponse(reply->readAll());
      QXmlStreamReader reader(apiResponse);
      while (findNextElement(reader, "cm")) {
        QXmlStreamAttributes attrs = reader.attributes();
        String pt = attrs.value("title").toString();
        int pid = attrs.value("pageid").toString().toInt();
        results->AddBlankRow();
        pt_col->SetVal(pt, -1);
        pid_col->SetVal(pid, -1);
      }
      if (reader.hasError()) {return false;}
      // The way the findNextElement works, we will need to reparse the whole API response if we are looking
      // for a new type of element. Not elegant, but given the sizes of API response, this should be acceptable inefficiency.
      reader.clear();
      reader.addData(apiResponse);
      if (findNextElement(reader,"continue")) {
        QXmlStreamAttributes attrs = reader.attributes();
        continutationURL = attrs.value("cmcontinue").toString();
        gotAllResults = false;
        if (reader.hasError()) {return false;}
      } else {
        gotAllResults = true;
        // Return true (success) as long as there were no errors in the XML.
        return !reader.hasError();
      }
      
    }
    
  }
  return false;
}

// TODO - rohrlich - 2/21/15 - not good for taMediaWiki to know about DataTable -
// better to return a set of strings that the caller can put into DataTable or display in other form.
bool taMediaWiki::QueryFiles(DataTable* results, const String& wiki_name,
                             const String& start_nm, const String& prefix,
                             int max_results)
{
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

    QString apiResponse(reply->readAll());
    if (CheckResponseError(apiResponse)) {
      taMisc::Error("Could not determine if page", page_name, "exists on", wiki_name, "wiki.");
      taMisc::Warning("Tried to get information from: " + url.toString());
      return false;
    }
    QXmlStreamReader reader(apiResponse.toStdString().c_str());

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
    QString apiResponse(reply->readAll());
    if (CheckResponseError(apiResponse)) {
      taMisc::Error("Could not delete page", page_name, "on", wiki_name, "wiki.");
      return false;
    } else {
      taMisc::Info("Successfully deleted", page_name, "page on", wiki_name, "wiki!");
      return true;
    }
  }
  taMisc::Warning("Page delete request failed -- check the wiki/page names");
  return false;
}

bool taMediaWiki::FindMakePage(const String& wiki_name, const String& page_name,
                               const String& page_content, const String& page_category)
{
  // If given page exists on wiki...
  if(PageExists(wiki_name, page_name)) {
    // Append page with given content.
    return EditPage(wiki_name, page_name, page_content);
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
  // Make sure wiki name is valid before doing anything else.
  String wikiUrl = GetApiURL(wiki_name);
  if (wikiUrl.empty()) { return false; }

  // Also make sure the page does not exist on the wiki before moving on.
  if (PageExists(wiki_name, page_name)) {
    taMisc::Warning("Page", page_name, "already exists on", wiki_name, "wiki! Call FindMakePage to make edits to existing pages");
    return false;
  }

  // Get the edit token for this post request.
  QByteArray token = GetEditToken(wiki_name);
  if (token.isEmpty()) { return false; }

  // Build the request URL.
  // .../api.php?action=edit&title=<page_name>&section=new&preload=Template:Project&createonly=&text=<page_content>&format=xml&token=<token>
  QUrl url(wikiUrl);
  
#if (QT_VERSION >= 0x040800)
  QHttpMultiPart *multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);
  
  QHttpPart actionPart;
  actionPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"action\""));
  actionPart.setBody("edit");
  
  QHttpPart titlePart;
  titlePart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"title\""));
  titlePart.setBody(page_name);

  QHttpPart sectionPart;
  sectionPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"section\""));
  sectionPart.setBody("new");

  QHttpPart createonlyPart;
  createonlyPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"createonly\""));
  createonlyPart.setBody("true");
  
  QHttpPart tokenPart;
  tokenPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"token\""));
  tokenPart.setBody(token);
  
  QHttpPart formatPart;
  formatPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"format\""));
  formatPart.setBody("xml");
  
  QHttpPart contentPart;
  contentPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"text\""));
  contentPart.setBody(page_content);
  
  multiPart->append(actionPart);
  multiPart->append(titlePart);
  multiPart->append(createonlyPart);
  multiPart->append(formatPart);
  multiPart->append(contentPart);
  multiPart->append(tokenPart);
  
  // Make the network request.
  // Note: The reply will be deleted when the request goes out of scope.
  iSynchronousNetRequest request;
  if (QNetworkReply *reply = request.httpPost(url,multiPart)) {
    QString apiResponse(reply->readAll());
    if(CheckResponseError(apiResponse)) {
      taMisc::Error("Page creation failed for ", page_name, " on ", wiki_name, "wiki!");
      return false;
    } else {
      taMisc::Info("Successfully created", page_name, "page on", wiki_name, "wiki!");
      return true;
    }
  }
#endif
  taMisc::Warning("Page create request failed -- check the wiki/page names");
  return false;
}

bool taMediaWiki::EditPage(const String& wiki_name, const String& page_name,
                           const String& page_content, bool append)
{
  // Make sure wiki name is valid before doing anything else.
  String wikiUrl = GetApiURL(wiki_name);
  if (wikiUrl.empty()) {
    return false;
  }

  // Also make sure the page exists on the wiki before moving on.
  if (!PageExists(wiki_name, page_name)) {
    taMisc::Warning("Page", page_name, "not found on", wiki_name, "wiki! Call FindMakePage to create a new page");
    return false;
  }

  // Get the edit token for this post request.
  QByteArray token = GetEditToken(wiki_name);
  if (token.isEmpty()) {
    return false;
  }

  // Build the request URL.
  // .../api.php?action=edit&title=<page_name>&nocreate=&appendtext=<page_content>&format=xml&token=<token>
  QUrl url(wikiUrl);
  
  
#if (QT_VERSION >= 0x040800)
  QHttpMultiPart *multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);
  
  QHttpPart actionPart;
  actionPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"action\""));
  actionPart.setBody("edit");
  
  QHttpPart titlePart;
  titlePart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"title\""));
  titlePart.setBody(page_name);
  
  QHttpPart nocreatePart;
  nocreatePart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"nocreate\""));
  nocreatePart.setBody("true");
  
  QHttpPart tokenPart;
  tokenPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"token\""));
  tokenPart.setBody(token);
  
  
  QHttpPart formatPart;
  formatPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"format\""));
  formatPart.setBody("xml");

  
  QHttpPart contentPart;
  if (append) {
    contentPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"appendtext\""));
  } else {
    contentPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"pagetext\""));
  }
  contentPart.setBody(page_content);
  
  multiPart->append(actionPart);
  multiPart->append(formatPart);
  multiPart->append(titlePart);
  multiPart->append(nocreatePart);
  multiPart->append(contentPart);
  multiPart->append(tokenPart);

  
  // Make the network request.
  // Note: The reply will be deleted when the request goes out of scope.
  iSynchronousNetRequest request;
  if (QNetworkReply *reply = request.httpPost(url,multiPart)) {
    QString apiResponse(reply->readAll());
    if(CheckResponseError(apiResponse)) {
      taMisc::Error("Page edit failed for ", page_name, " on ", wiki_name, "wiki!");
      return false;
    } else {
      return true;
    }
  }
#endif
  taMisc::Warning("Page edit request failed -- check the wiki/page names");
  return false;
}

bool taMediaWiki::AppendVersionInfo(const String& wiki_name, const String& proj_filename, const String& proj_version, const String& emer_version) {
  String content = "[[EmerVersion::" + emer_version + "]][[EmerProjVersion::" + proj_version + "]]";
  String prefixed_filename = "File:" + proj_filename;
  return EditPage(wiki_name, prefixed_filename, content);
}

bool taMediaWiki::AppendFileType(const String& wiki_name, const String& proj_filename, const String& file_type) {
  String content = "[[EmerFileType::" + file_type + "]]";
  String prefixed_filename = "File:" + proj_filename;
  return EditPage(wiki_name, prefixed_filename, content);
}

#if 0
bool taMediaWiki::AddCategories(const String& wiki_name, const String& page_name, const String& page_category)
{
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
    QString apiResponse(reply->readAll());
    if(CheckResponseError(apiResponse)) {
      taMisc::Error("Add Category", page_category, "to", page_name,"on", wiki_name, "wiki!");
      return false;
    } else {
      return true;
    }
  }
  taMisc::Warning("Add category request failed -- check the wiki/page names");
  return false;
}
#endif

bool taMediaWiki::LinkFile(const String& file_name, const String& wiki_name, const String& proj_name) {
  // Make sure wiki name is valid before doing anything else.
  String wikiUrl = GetApiURL(wiki_name);
  if (wikiUrl.empty()) { return false; }
  
  if (!IsPublished(wiki_name, proj_name)) {
    taMisc::Warning("Project", proj_name, "not found on", wiki_name, "wiki! Make sure you've published it first.");
    return false;
  }
  
  String full_file_name = "File:" + file_name;
  String proj_line_property = "[[EmerProjName::" + proj_name + "]]";
  
  // Get the edit token for this post request.
  QByteArray token = GetEditToken(wiki_name);
  if (token.isEmpty()) { return false; }

  QUrl url(wikiUrl);
  
#if (QT_VERSION >= 0x040800)
  QHttpMultiPart *multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);
  
  QHttpPart actionPart;
  actionPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"action\""));
  actionPart.setBody("edit");
  
  QHttpPart titlePart;
  titlePart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"title\""));
  titlePart.setBody(full_file_name);
  
  QHttpPart nocreatePart;
  nocreatePart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"nocreate\""));
  nocreatePart.setBody("");
  
  QHttpPart tokenPart;
  tokenPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"token\""));
  tokenPart.setBody(token);
  
  QHttpPart formatPart;
  formatPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"format\""));
  formatPart.setBody("xml");
  
  
  QHttpPart contentPart;
  contentPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"appendtext\""));
  contentPart.setBody(proj_line_property);
  
  multiPart->append(actionPart);
  multiPart->append(titlePart);
  multiPart->append(nocreatePart);
  multiPart->append(formatPart);
  multiPart->append(contentPart);
  multiPart->append(tokenPart);
  
  
  // Make the network request.
  // Note: The reply will be deleted when the request goes out of scope.
  iSynchronousNetRequest request;
  if (QNetworkReply *reply = request.httpPost(url,multiPart)) {
    QString apiResponse(reply->readAll());
    if(CheckResponseError(apiResponse)) {
      taMisc::Error("File linking failed for ", proj_name, "to", file_name, "on", wiki_name, "wiki!");
      return false;
    } else {
      return true;
    }
  }
#endif
  taMisc::Warning("File link request failed -- check the wiki/page names");
  return false;
}

/////////////////////////////////////////////////////
//            WIKI OPERATIONS

String taMediaWiki::GetApiURL(const String& wiki_name) {
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

QByteArray taMediaWiki::GetEditToken(const String& wiki_name) {
  // Make sure wiki name is valid before doing anything else.
  String wikiUrl = GetApiURL(wiki_name);
  if (wikiUrl.empty()) {
	  return QByteArray(_nilString.chars());
  }

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
    QString apiResponse(reply->readAll());
    if(CheckResponseError(apiResponse)) {
      taMisc::Error("Could not retrieve edit token for", wiki_name, "wiki!");
      return QByteArray(_nilString.chars());
    }
    QXmlStreamReader reader(apiResponse.toStdString().c_str());
    while(!reader.atEnd()) {
      if (reader.readNext() == QXmlStreamReader::StartElement) {
        QXmlStreamAttributes attrs = reader.attributes();
        QString token = attrs.value("edittoken").toString();
        if(!token.isEmpty()) {
          taMisc::Info("Edit token retrieval successful");
          return QByteArray(token.toUtf8().constData());
        }
      }
    }
  }
  return QByteArray(_nilString.chars());
}

bool taMediaWiki::PublishProject(taProjPubInfo* pub_info) {
  return PublishItem(pub_info, "Project");
}

bool taMediaWiki::PublishItem(taProjPubInfo* pub_info, String publish_type) {
  String proj_category = "Published" + publish_type;
  String first_pub = String(QDate::currentDate().toString(Qt::TextDate));
  String emer_version = taMisc::version;  // use the version currently running
  
  String page_content = "{{Published" + publish_type + "|name=" + pub_info->proj_name + "|emer_proj_overview=" + pub_info->proj_desc + "|EmerProjAuthor = " + pub_info->proj_author + "|EmerProjEmail = " + pub_info->proj_email  + "|EmerProjFirstPub = " + first_pub + "|EmerProjKeyword = " + pub_info->proj_keywords + "}}";
  
  bool page_created = CreatePage(pub_info->wiki_name, pub_info->page_name, page_content, proj_category);
  if (!page_created)
    return false;
  
  // If project filename empty, the user does not want to upload the project file
  if (!pub_info->proj_filename.empty()) {
    String filename_only;  // no path
    bool loaded_and_linked = UploadFile(pub_info->wiki_name, pub_info->proj_filename, "");
    if (loaded_and_linked) {
      filename_only = pub_info->proj_filename.after('/', -1);
      loaded_and_linked = LinkFile(filename_only, pub_info->wiki_name, pub_info->proj_name);  // link the file to the project page
    }
    if (loaded_and_linked) {
      AppendVersionInfo(pub_info->wiki_name, filename_only, pub_info->proj_version, emer_version);
      AppendFileType(pub_info->wiki_name, filename_only, publish_type);
    }
    else {
      taMisc::Error(publish_type + " page created BUT upload of " + publish_type + " file or linking of uploaded " + publish_type + " file has failed ", pub_info->wiki_name, "wiki");
    }
  }
  return true; // return true if item page created - even if upload of item file fails
}

bool taMediaWiki::PublishItemOnWeb(const String& publish_type, const String& name, const String& fname, const String& repo_name, const taProject * proj, const String& tags)
{
  // Is pub to program supported on this wiki?
  
  if (!PageExists(repo_name, "Category:Published" + publish_type)) {
    taMisc::Error("The publish " + publish_type + " feature requires the installation of some pages on your wiki. Your wiki administrator can find the installation instructions at ......");
    return false;
  }
  
  // First check to make sure the page doesn't already exist.
  String item_filename = "File:" + fname.after('/', -1);
  
  if (taMediaWiki::IsPublished(repo_name, item_filename)) {
    int choice = taMisc::Choice("The " + publish_type  + " " + name + " is already published on  wiki \"" + repo_name + ".\" Would you like to upload a new version?", "Upload", "Cancel");
    if (choice == 0) {
      return UpdateItemOnWeb(publish_type, name, fname, repo_name, proj, tags);
    }
    else {
      return false;
    }
  }
  
  String username = taMediaWiki::GetLoggedInUsername(repo_name);
  
  // TODO - if username not empty ask if they want to stay logged in under that name
  bool was_published = false;
  String page_name;
  
  bool logged_in = taMediaWiki::Login(repo_name, username);
  if (logged_in) {
    iDialogPublishDocs dialog(repo_name, name, true, publish_type);
    dialog.SetName(name.toQString());
    if (!proj->author.empty()) {
      dialog.SetAuthor((proj->author.toQString()));
    }
    else {
      dialog.SetAuthor(QString("Set default in preferences."));
    }
    if (!proj->email.empty()) {
      dialog.SetEmail((proj->email.toQString()));
    }
    else {
      dialog.SetEmail(QString("Set default in preferences."));
    }
    dialog.SetDesc(QString("A brief description of the program. You will be able to edit later on the wiki."));
    if(tags.nonempty()) {
      dialog.SetTags(tags);
    }
    else {
      dialog.SetTags(QString("comma separated, please"));
    }
    //dialog.SetVersion((this->version.GetString().toQString()));
    if (dialog.exec()) {
      // User clicked OK.
      page_name = String(name); // needed for call to create the taDoc
      QString author = dialog.GetAuthor();
      if (author == "Set default in preferences.") {
        author = "";
      }
      QString email = dialog.GetEmail();
      if (email == "Set default in preferences.") {
        email = "";
      }
      QString desc = dialog.GetDesc();
      QString keywords = dialog.GetTags();
      if (keywords == "comma separated, please") {
        keywords = "";
      }
      QString version = dialog.GetVersion();
      //      bool upload = dialog.GetUploadChoice();
      
      taProjPubInfo* pub_info = new taProjPubInfo();
      pub_info->wiki_name = repo_name;
      pub_info->page_name = page_name;
      pub_info->proj_name = name;
      pub_info->proj_version = version;
      pub_info->proj_filename = fname;
      pub_info->proj_author = author;
      pub_info->proj_email = email;
      pub_info->proj_desc = desc;
      pub_info->proj_keywords = keywords;
      was_published = taMediaWiki::PublishItem(pub_info,publish_type);
    }
  }
  return was_published;
}

bool taMediaWiki::UpdateItemOnWeb(const String& publish_type, const String& name, const String& fname, const String& repo_name, const taProject * proj, const String& tags) {
  String proj_filename_only = fname.after('/', -1);
  
  String emer_version = taMisc::version;
  if (!taMediaWiki::IsPublished(repo_name, name)) {
    taMisc::Error(publish_type + " named \"" + name + "\" not found on wiki \"" + repo_name + "\"");
    return false;
  }
  
  String username = taMediaWiki::GetLoggedInUsername(repo_name);
  bool logged_in = taMediaWiki::Login(repo_name, username);
  if (!logged_in) {
    return false;
  }
  
  // just project name and version for already published project
  iDialogPublishDocs dialog(repo_name, name, false, publish_type); // false = update
  dialog.SetName((name.toQString()));
  //dialog.SetVersion((this->version.GetString().toQString()));
  QString version;
  if (dialog.exec()) {
    version = dialog.GetVersion();
  }
  
  bool rval = taMediaWiki::UploadFile(repo_name, fname, false); // true - update new revision
  if (rval == false) {
    taMisc::Error("Upload failure");
    return false;
  }
  rval = taMediaWiki::AppendVersionInfo(repo_name, proj_filename_only, version, emer_version);
  if (rval == false) {
    taMisc::Error("AppendVersionInfo failure");
    return false;
  }
  return true;
}


bool taMediaWiki::PubProjPagesInstalled(const String& wiki_name) {
  if (PageExists(wiki_name, "Category:PublishedProject"))
    return true;
  
  return false;
}

bool taMediaWiki::PubProgPagesInstalled(const String& wiki_name) {
  if (PageExists(wiki_name, "Category:PublishedProgram"))
    return true;
  
  return false;
}
