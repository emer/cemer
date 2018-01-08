// Co2018ght 2013-2017, Regents of the University of Colorado,
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

#ifndef taWikiSpec_h
#define taWikiSpec_h 1

// parent includes:
#include <taOBase>

// member includes:

// declare all other types mentioned but not required to include:


taTypeDef_Of(taWikiSpec);

class TA_API taWikiSpec: public taOBase {
  // ##INLINE ##NO_TOKENS wiki URL location information -- include in objects that can be sync'd with a wiki -- relative wiki links
INHERITED(taOBase)
public:
  String        wiki;           // name of a wiki, as specified in global preferences, where this object was last saved
  String        page_name;      // full wiki page name to save / load this object to / from -- by default it is just the name of the object directly

  String        GetURL();       // #CAT_Wiki gets the full url of the wiki, based on wiki name, looking up in the preferences in taMisc

  TA_SIMPLE_BASEFUNS(taWikiSpec);
private:
  void Initialize();
  void Destroy() {}
};

#endif // taWikiSpec_h
