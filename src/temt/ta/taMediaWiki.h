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

// parent includes:
#include <taNBase>

// member includes:

// declare all other types mentioned but not required to include:
class DataTable; // 


class TA_API taMediaWiki : public taNBase
{
  // ##INLINE ##NO_TOKENS media wiki api interface -- all static functions for performing functions we actually want to perform
  INHERITED(taNBase)

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

#endif // taMediaWiki_h
