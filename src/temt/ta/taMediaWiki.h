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


taTypeDef_Of(taMediaWiki);

class TA_API taMediaWiki : public taNBase {
  // ##INLINE ##NO_TOKENS media wiki api interface -- all static functions for performing functions we actually want to perform
  INHERITED(taNBase)

public:
  /////////////////////////////////////////////////////
  //            ACCOUNT OPERATIONS

  static String GetLoggedInUsername(const String &wiki_name);
  // #CAT_Account Get the username of the user currently logged in via emergent's Webkit browser.

  static bool   Login(const String &wiki_name, const String &username = "");
  // #CAT_Account Login to the wiki.  Returns true if username specified is already logged in.  Otherwise prompts for username/password and returns true if login succeeds.

  static bool   Logout(const String &wiki_name);
  // #CAT_Account Logout from the wiki.

  /////////////////////////////////////////////////////
  //            UPLOAD/DOWNLOAD OPERATIONS

  static bool   UploadFile(const String& wiki_name, const String& local_file_name,
                           const String& wiki_file_name="", bool convert_to_camel=true);
  // #CAT_File upload given file name to wiki, optionally giving it a different file name on the wiki relative to what it is locally -- if convert_to_camel is true, destination file name will be converted to CamelCase format

  static bool   DownloadFile(const String& wiki_name, const String& wiki_file_name,
                             const String& local_file_name="", bool convert_to_camel=true);
  // #CAT_File download given file name from wiki, optionally giving it a different file name than what it was on the wiki -- if convert_to_camel is true, destination file name will be converted to CamelCase format

  /////////////////////////////////////////////////////
  //            QUERY OPERATIONS

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
  // #CAT_Query fill results data table with pages containing given search string, starting at given name, and with each name starting with given prefix (empty = all), string column "PageTitle" has page title

  /////////////////////////////////////////////////////
  //            PAGE OPERATIONS

  static bool   PageExists(const String& wiki_name, const String& page_name);
  // #CAT_Page determine if given page exists on wiki -- returns true if it does, false if it doesn't

  static bool   DeletePage(const String& wiki_name, const String& page_name);
  // #CAT_Page delete given page from the wiki -- returns true on success

  static bool   FindMakePage(const String& wiki_name, const String& page_name,
                             const String& page_content="", const String& page_category="");
  // #CAT_Page find or create given page on the wiki and populate it with given content -- calls EditPage if the given page already exists on the wiki, otherwise calls CreatePage -- returns true on success

  static bool   CreatePage(const String& wiki_name, const String& page_name,
                           const String& page_content="", const String& page_category="");
  // #CAT_Page create given page on the wiki and populate it with given content if it does not currently exist  -- returns true on success

  static bool   EditPage(const String& wiki_name, const String& page_name,
                         const String& page_content="", const String& page_category="");
  // #CAT_Page append given page on the wiki with given content if it currently exists -- returns true on success

  /////////////////////////////////////////////////////
  //            WIKI OPERATIONS

  static String GetApiURL(const String& wiki_name);
  // #CAT_Wiki gets the url for the wiki api

  static String GetEditToken(const String& wiki_name);
  // #CAT_Wiki return a String containing an unencoded edit token for the wiki (need to percent-encode this to make push requests to the API directly through a URL query) -- on failure, return an empty String

  static bool   PublishProject(const String& wiki_name, const String& proj_filename,
                               const String& page_content="", const String& proj_category="");
  // #CAT_Wiki create/edit the wiki page for this project, upload all files from the local project directory, then post links to these files on the project's wiki page -- returns true on success

protected:
  TA_BASEFUNS_NOCOPY(taMediaWiki);

private:
  void Initialize();
  void Destroy() {}
};

#endif // taMediaWiki_h
