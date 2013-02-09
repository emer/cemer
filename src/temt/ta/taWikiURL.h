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

#ifndef taWikiURL_h
#define taWikiURL_h 1

// parent includes:
#include <taOBase>

// member includes:

// declare all other types mentioned but not required to include:


taTypeDef_Of(taWikiURL);

class TA_API taWikiURL: public taOBase {
  // ##INLINE ##NO_TOKENS wiki URL location information -- include in objects that can be sync'd with a wiki -- relative wiki links
INHERITED(taOBase)
public:
  bool          sync;           // enable synchronizing this object with the wiki
  String        wiki;           // #CONDEDIT_ON_sync name of a wiki, as specified in global preferences, where this object should be stored -- this is used to lookup the wiki name -- if blank then url must be a full URL path
  String        url;            // #CONDEDIT_ON_sync a URL location for this object -- if wiki name is set, then this is relative to that wiki, as wiki_url/index.php/url (use Projects/ as start of url for storing project-related information), otherwise it is a full URL path to a valid location

  String        GetURL();       // #CAT_URL gets the full url to sync to

  TA_SIMPLE_BASEFUNS(taWikiURL);
private:
  void Initialize();
  void Destroy() {}
};

#endif // taWikiURL_h
