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

#include <ControlPanel>
#include "ControlPanel_Group.h"
#include <DataTable>

TA_BASEFUNS_CTORS_DEFN(ControlPanel_Group);

void ControlPanel_Group::AddMember
(taBase* base, MemberDef* mbr, const String& xtra_lbl, const String& dscr, const String& sub_gp_nm, bool short_label) {
  FOREACH_ELEM_IN_GROUP(ControlPanel, cp, *this) {
    cp->AddMember(base, mbr, xtra_lbl, dscr, sub_gp_nm, short_label);
  }
}

void ControlPanel_Group::RemoveMember(taBase* base, MemberDef* mbr) {
  FOREACH_ELEM_IN_GROUP(ControlPanel, cp, *this) {
    cp->RemoveMember(base, mbr);
  }
}

void ControlPanel_Group::RestorePanels() {
  FOREACH_ELEM_IN_GROUP(ControlPanel, cp, *this) {
    if(cp->GetUserDataAsBool("user_pinned")) {
      cp->EditPanel(true, true); // true,true = new tab, pinned in place
    }
  }
}

void ControlPanel_Group::CopyFromDataTable(DataTable* table) {
  if(TestError(!table, "CopyFromDataTable",
               "table is NULL")) {
    return;
  }
  if(TestError(table->rows == 0, "CopyFromDataTable",
               "table has no rows")) {
    return;
  }
  if(TestError(table->cols() == 0, "CopyFromDataTable",
               "table has no columns")) {
    return;
  }
  if(TestError(leaves == 0, "CopyFromDataTable",
               "group has no existing control panels -- must have at least one -- first one is used as a template for any new ones that are needed")) {
    return;
  }
  ControlPanel* templ = Leaf(0);
  for(int i=0; i<table->rows; i++) {
    String nm = table->GetValAsString(0, i);
    ControlPanel* cp = FindLeafName(nm);
    if(cp) {
      cp->CopyFromDataTable(table, i);
    }
    else {
      cp = (ControlPanel*)templ->Clone();
      cp->name = nm;
      Add(cp);
      cp->CopyFromDataTable(table, i);
    }
  }
}

void ControlPanel_Group::CopyToDataTable(DataTable* table) {
  FOREACH_ELEM_IN_GROUP(ControlPanel, cp, *this) {
    cp->CopyToDataTable(table);
  }
}
