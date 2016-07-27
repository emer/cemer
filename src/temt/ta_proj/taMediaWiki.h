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

#ifndef taMediaWiki_h
#define taMediaWiki_h 1

#define RESPONSE_OK 200
#define RESPONSE_ERROR 500
#define RESPONSE_BAD_REQUEST 400

// parent includes:
#include <taNBase>

// member includes:

// declare all other types mentioned but not required to include:
class DataTable; //
class QByteArray; //
class taProjVersion; //


taTypeDef_Of(taMediaWiki);

class TA_API taMediaWiki : public taNBase {
  // ##INLINE ##NO_TOKENS MediaWiki API interface -- all static functions for performing functions we actually want to perform via http requests
  INHERITED(taNBase)

public:
  /////////////////////////////////////////////////////
  //            ACCOUNT OPERATIONS

  static String GetApiURL(const String& wiki_name);
  // #CAT_Wiki Gets the url for the wiki api

  static String GetWikiNameFromURL(const String& wiki_url);
  // get the wiki name in our list of known wikis based on given url

  static String GetLoggedInUsername(const String &wiki_name);
  // #CAT_Account Get the username of the user currently logged in via emergent's Webkit browser.

  static bool   Login(const String &wiki_name, const String &username = "");
  // #CAT_Account Login to the wiki.  Returns true if username specified is already logged in.  Otherwise prompts for username/password and returns true if login succeeds.

  static bool   Logout(const String &wiki_name);
  // #CAT_Account Logout from the wiki.

  /////////////////////////////////////////////////////
  //            FILE OPERATIONS

  static bool   UploadFile(const String& wiki_name, const String& local_file_name, bool new_revision, const String& wiki_file_name="", const String& comment="");
  // #CAT_File Upload file to wiki, optionally giving it a different file name on the wiki

  static bool   DownloadFile(const String& wiki_name, const String& wiki_file_name,
                             const String& local_file_name="");
  // #CAT_File Download given file name from wiki, optionally giving it a different file name than what it was on the wiki

  static bool   DeleteFile(const String& wiki_name, const String& file_name, const String& reason="");
  // #CAT_File Delete given file from wiki, optionally providing a reason for the deletion -- returns true on success

  static bool   GetDirectoryContents(DataTable* results);
  // #CAT_File Fill results data table with all of the files contained in the current working directory -- string column "FileName" has name of file, int column "Size" has file size -- returns true on success

  /////////////////////////////////////////////////////
  //            QUERY OPERATIONS

  static bool   QueryPages(DataTable* results, const String& wiki_name,
                           const String& name_space="", const String& start_nm="",
                           const String& prefix="", int max_results=-1);
  // #CAT_Query Fill results data table with pages in given name space, starting at given name, and with each name starting with given prefix (empty = all) -- string column "PageTitle" has page title, int column "PageId" has page ID number

  static bool   QueryPagesByCategory(DataTable* results, const String& wiki_name,
                                     const String& category, const String& name_space="",
                                     int max_results=-1);
  // #CAT_Query Fill results data table with pages in given category, starting at given name, and with each name starting with given prefix (empty = all) -- string column "PageTitle" has page title, int column "PageId" has page ID number

  static bool   QueryFiles(DataTable* results, const String& wiki_name,
                           const String& start_nm="", const String& prefix="",
                           int max_results=-1);
  // #CAT_Query Fill results data table with files uploaded to wiki, starting at given name, and with each name starting with given prefix (empty = all) -- string column "FileName" has name of file, int column "Size" has file size, string column "MimeType" has mime type

  static bool   SearchPages(DataTable* results, const String& wiki_name,
                            const String& search_str, bool title_only = false,
                            const String& name_space="", int max_results=-1);
  // #CAT_Query Fill results data table with pages containing given search string, starting at given name, and with each name starting with given prefix (empty = all) -- if title_only is true, only search for matches in page titles; else, search for matches in page contents -- string column "PageTitle" has page title

  static bool   FileExists(const String& wiki_name, const String& file_name, bool quiet = true);
  // #CAT_File Determine if given file exists on wiki -- returns true if it does, false if it doesn't

  static bool   PageExists(const String& wiki_name, const String& page_name);
  // #CAT_Page Determine if given page exists on wiki -- returns true if it does, false if it doesn't

  /////////////////////////////////////////////////////
  //            PAGE OPERATIONS


  static bool   DeletePage(const String& wiki_name, const String& page_name, const String& reason="");
  // #CAT_Page Delete given page from the wiki, optionally providing a reason for the deletion -- returns true on success

  static bool   FindMakePage(const String& wiki_name, const String& page_name,
                             const String& page_content="");
  // #CAT_Page Find or create given page on the wiki and populate it with given content -- calls EditPage if the given page already exists on the wiki, otherwise calls CreatePage -- returns true on success

  static bool   CreatePage(const String& wiki_name, const String& page_name,
                           const String& page_content="");
  // #CAT_Page Create given page on the wiki and populate it with given content if it does not currently exist  -- returns true on success

  static QByteArray GetEditToken(const String& wiki_name);
  //#IGNORE  #CAT_Wiki Return a String containing an unencoded edit token for the wiki (need to percent-encode this to make post requests to the API directly through a URL query) -- on failure, return an empty String

  static bool   EditPage(const String& wiki_name, const String& page_name,
                         const String& page_content, bool append = true);
  // #CAT_Page Append given page on the wiki with given content if it currently exists -- returns true on success - if append is false content is prepended

#if 0
  static bool   AddCategories(const String& wiki_name, const String& page_name,
                              const String& page_category);
  // #CAT_Page Append given page on the wiki with given list of comma-separated categories -- returns true on success
#endif
  
  /////////////////////////////////////////////////////
  //            WIKI OPERATIONS

  static bool   AppendVersionInfo(const String& wiki_name, const String& publish_type, const String& file_name, const String& version, const String& emer_version);
  // #CAT_Page Append page with both project version and emergent program version
  
  static bool   AppendFileType(const String& wiki_name, const String& file_type, const String& page_name);
  // #CAT_Page Append page content with EmerFileType = file_type of Project, Program, or other
  
  static bool   LinkFile(const String& wiki_name, const String& publish_type, const String& file_name, const String& obj_name);
  // #CAT_Page Add the object name to the the file page so that the object page can find files containing the object name as property using the given wiki tag, which should be either EmerProjName (publish_type = Project) or EmerProgName (Program)-- note that the linkage is with the object name, not the page name, which can be different
  
  static bool   UploadOtherFile(const String& wiki_name, const String& publish_type, const String& file_name, const String& obj_name, bool new_revision, const String& wiki_file_name="");
  // #CAT_File Upload "other" file to wiki and link to specific obj page
  
  static bool   PubProjPagesInstalled(const String& wiki_name);
  //  #CAT_Wiki Are the pages that support the PublishProject featured installed on this wiki? Actually only checks for one crucial page!

  static bool PubProgPagesInstalled(const String& wiki_name);
  //  #CAT_Wiki Are the pages that support the PublishProgram featured installed on this wiki? Actually only checks for one crucial page!

  static bool PublishItemOnWeb(const String& wiki_name, const String& publish_type, const String& obj_name, const String& file_name, String& page_name, String& tags, String& desc, taProjVersion& version, String& author, String& email, taBase* obj);
  // main interface for publishing an item on a wiki -- prompts user for updated info of items passed -- note that page_name, version, tags, desc, will be updated by user input in the publish dialog, and should be applied back to the object upon successful completion 
  static bool PublishItem_impl(const String& wiki_name, const String& publish_type, const String& obj_name, const String& file_name, const String& page_name, const String& tags, const String& desc, const String& version, const String& author, const String& email);
  // #IGNORE actually publish the item -- if edit_page then edit exsting page, else create
  
  static bool UpdateItemOnWeb(const String& wiki_name, const String& publish_type, const String& obj_name, const String& file_name, taProjVersion& version, taBase* obj);
  // main interface for updating an item on a wiki

  TA_BASEFUNS_NOCOPY(taMediaWiki);
private:
  static bool CheckResponseError(const QString &xmlRespone);
  void Initialize();
  void Destroy() {}
};

#endif // taMediaWiki_h
