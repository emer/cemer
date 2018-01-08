// Copyright 2013-2017, Regents of the University of Colorado,
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

void Doc_Group::RestorePanels() {
  FOREACH_ELEM_IN_GROUP(taDoc, doc, *this) {
    if(doc->GetUserDataAsBool("user_pinned")) {
      doc->EditPanel(true, true);
    }
  }
}

taDoc* Doc_Group::NewProjWikiDoc() {
  taProject* proj = GetMyProj();
  if(!proj)
    return NULL;
  taDoc* doc = NewEl(1);
  doc->name = "ProjectDocs";
  doc->wiki = proj->wiki.wiki;
  doc->url = proj->wiki.page_name;
  doc->web_doc = true;
  // pin by default
  doc->SetUserData("user_pinned", true, false); // false = no notify
  doc->UpdateAfterEdit();
  if(size > 1)                  // put in first position
    MoveEl(doc, FastEl(0));
  tabMisc::DelayedFunCall_gui(doc, "BrowserSelectMe");
  return doc;
}

