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

#include <taDoc>
#include "Doc_Group.h"
#include <taProject>
#include <tabMisc>

TA_BASEFUNS_CTORS_DEFN(Doc_Group);

void Doc_Group::AutoEdit() {
  FOREACH_ELEM_IN_GROUP(taDoc, doc, *this) {
    if (doc->auto_open)
      doc->EditPanel(true, true); // true,true = new tab, pinned in place
  }
}


taDoc* Doc_Group::NewProjWikiDoc(const String& wiki_name) {
  // todo: make a chooser for taMisc::wikis!
  taProject* proj = GET_MY_OWNER(taProject);
  if(!proj) return NULL;
  taDoc* doc = NewEl(1);
  doc->name = "ProjectDoc";
  doc->wiki = wiki_name;
  doc->url = proj->name;
  doc->auto_open = true;
  doc->web_doc = true;
  doc->UpdateAfterEdit();
  tabMisc::DelayedFunCall_gui(doc, "BrowserSelectMe");
  return doc;
}
