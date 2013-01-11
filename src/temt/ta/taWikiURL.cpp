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

#include "taWikiURL.h"
#include <taMisc>

void taWikiURL::Initialize() {
  sync = false;
}

String taWikiURL::GetURL() {
  if(!sync) return _nilString;
  if(wiki.nonempty()) {
    String wiki_url = taMisc::GetWikiURL(wiki, true); // true = add index.php/
    if(TestError(wiki_url.empty(), "GetURL", "wiki named:", wiki,
                 "not found in global preferences/options under wiki_url settings"))
      return _nilString;
    return wiki_url + url;
  }
  return url;
}
