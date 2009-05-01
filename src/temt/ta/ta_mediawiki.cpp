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
#include "ta_qt.h"
#include "ta_datatable.h"
#include "inetworkaccessmanager.h"

#include <QNetworkRequest>
#include <QNetworkReply>
#include <QXmlStreamReader>

// todo: at a later point, dispatch a thread to do this stuff??

taMediaWikiReadReady::taMediaWikiReadReady(taMediaWikiReadCB cb, DataTable* res)
  : QObject(NULL) {
  cb_fun = cb;
  results = res;
}

taMediaWikiReadReady::~taMediaWikiReadReady() {
  cb_fun = NULL;
  results = NULL;
}

void taMediaWikiReadReady::finished(QNetworkReply* reply) {
  if(reply->error() != QNetworkReply::NoError ) {
    taMisc::Error("taMediaWiki Request failed:", reply->errorString());
    delete reply;
    return;
  }
  (*cb_fun)(results, reply);
  delete reply;
  taMediaWiki::read_ready = NULL;
  delete this;			// kill us as our last step!!
}

taMediaWikiReadReady* taMediaWiki::read_ready = NULL;

void taMediaWiki::Initialize() {
}

String taMediaWiki::GetApiURL(const String& wiki_name) {
  String wiki_url = taMisc::GetWikiURL(wiki_name, false); // false = don't add index.php/
  if(wiki_url.empty()) {
    taMisc::Error("taMediaWiki::GetApiURL", "wiki named:", wiki_name,
		  "not found in global preferences/options under wiki_url settings");
    return _nilString;
  }
  return wiki_url + "/api.php";
}

bool taMediaWiki::PageExists(const String& wiki_name, const String& page_name) {
  // #CAT_Page determine if given page exists on wiki
  return false;
}

bool taMediaWiki::CreatePage(const String& wiki_name, const String& page_name,
			     const String& page_content) {
  // #CAT_Page create given page on the wiki and populate it with given content if non-empty  -- return true on success
  return false;
}

bool taMediaWiki::FindMakePage(const String& wiki_name, const String& page_name,
			       const String& page_content) {
  // #CAT_Page find or create given page on the wiki and populate it with given content if non-empty -- return true on success
  return false;
}

/////////////////////////////////////////////////////
//		Upload/Download operations

bool taMediaWiki::UploadFile(const String& wiki_name, const String& file_name, 
			     const String& wiki_file_name) {
  // #CAT_File upload given file name to wiki, optionally giving it a different file name on the wiki relative to what it is locally

  return false;
}
bool taMediaWiki::DownloadFile(const String& wiki_name, const String& file_name,
			       const String& local_file_name) {
  // #CAT_File download given file name from wiki, optionally giving it a different file name than what it was on the wiki
  return false;
}

/////////////////////////////////////////////////////
//		Query operations

bool taMediaWiki::QueryPages(DataTable* results, const String& wiki_name,
			     const String& name_space,
			     const String& start_nm, 
			     const String& prefix, 
			     int max_results) {
  // #CAT_Query fill results data table with pages in given name space, starting at given name, and with each name starting with given prefix (empty = all), string column "PageTitle" has page tiltle, int column "PageId" has page id number
  return false;
}
  
bool taMediaWiki::QueryPagesByCategory(DataTable* results, const String& wiki_name,
				       const String& category,
				       const String& name_space,
				       int max_results) {
  // #CAT_Query fill results data table with pages in given category, starting at given name, and with each name starting with given prefix (empty = all), string column "PageTitle" has page tiltle, int column "PageId" has page id number
  return false;
}
  
bool taMediaWiki::QueryFiles(DataTable* results, const String& wiki_name,
			     const String& start_nm, 
			     const String& prefix, 
			     int max_results) {
  // #CAT_Query fill results data table with files uploaded to wiki, starting at given name, and with each name starting with given prefix (empty = all), string column "FileName" has name of file, int column "Size" has file size, string column "MimeType" has mime type
  return false;
}

bool taMediaWiki::SearchPages(DataTable* results, const String& wiki_name,
			      const String& search_str, bool title_only,
			      const String& name_space,
			      int max_results) {
  if(read_ready || !results || !taiMisc::net_access_mgr) return false; // all failures
  // reserve the read_ready guy now just in case someone else jumps in..
  read_ready = new taMediaWikiReadReady(&taMediaWiki::SearchPages_read, results);

  String wiurl = GetApiURL(wiki_name);
  if(wiurl.empty()) return false;

  QUrl url(wiurl);
  url.addQueryItem( QString("action"), QString("query") );
  url.addQueryItem( QString("format"), QString("xml") );
  url.addQueryItem( QString("list"), QString("search") );
  url.addQueryItem( QString("srsearch"), search_str );
  if(title_only) 
    url.addQueryItem( QString("srwhat"), QString("title") );
  else
    url.addQueryItem( QString("srwhat"), QString("text") );
  if(max_results > 0)
    url.addQueryItem( QString("srlimit"), QString::number(max_results) );

  QObject::connect(taiMisc::net_access_mgr, SIGNAL(finished(QNetworkReply*)),
		   read_ready, SLOT(finished(QNetworkReply*)));

  taiMisc::net_access_mgr->get( QNetworkRequest(url) );
}

bool taMediaWiki::SearchPages_read(DataTable* results, QNetworkReply* reply) {
  results->RemoveAllRows();
  DataCol* pt_col = results->FindMakeCol("PageTitle", VT_STRING);

  QXmlStreamReader reader( reply );
  while ( !reader.atEnd() ) {
    QXmlStreamReader::TokenType tokenType = reader.readNext();
    if ( tokenType == QXmlStreamReader::StartElement ) {
      if ( reader.name() == QString("p") ) {
	QXmlStreamAttributes attrs = reader.attributes();
	String pt = attrs.value( QString("title") ).toString();
	results->AddBlankRow();
	pt_col->SetVal(pt, -1);
      }
    }
    else if ( tokenType == QXmlStreamReader::Invalid ) {
      return false;
    }
  }
  return true;
}
