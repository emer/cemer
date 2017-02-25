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

TA_BASEFUNS_CTORS_DEFN(ControlPanel_Group);

void ControlPanel_Group::AddMember(taBase* base, MemberDef* mbr, const String& xtra_lbl,
                                   const String& dscr, const String& sub_gp_nm) {
  FOREACH_ELEM_IN_GROUP(ControlPanel, cp, *this) {
    cp->AddMember(base, mbr, xtra_lbl, dscr, sub_gp_nm);
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
