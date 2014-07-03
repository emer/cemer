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

#include "taNBase.h"
#include <taProject>
#include <taList_impl>

#include <taMisc>

TA_BASEFUNS_CTORS_DEFN(taNBase);

void taNBase::Copy_(const taNBase& cp) {
  SetCopyName(cp);
}

void taNBase::SetDefaultName() {
  if(taMisc::not_constr || taMisc::in_init || GetTypeDef() == &TA_taNBase)
    return;
  SetDefaultName_();
}

void taNBase::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  // When a user enters a name in a dialog, it updates the 'name' member
  // without making a SetName call.  Make the call here so it can do its
  // validation.
  SetName(name);
  if(taMisc::is_loading) {
    // make sure my members are named with their member names!
    AutoNameMyMembers();
  }
}

void taNBase::MakeNameUnique() {
  if(owner && owner->InheritsFrom(&TA_taList_impl)) {
    ((taList_impl*)owner)->MakeElNameUnique(this); // just this guy
  }
}

bool taNBase::SetName(const String& nm) {
  // Ensure name is a legal C-language identifier.
  String new_name = taMisc::StringCVar(nm);
  if (name == new_name) return true;
  name = new_name;
  if (!taMisc::is_changing_type)
    MakeNameUnique();
  //UpdateAfterEdit();          // this turns out to be a bad idea -- just do it where needed
  return true;
}

bool taNBase::SetCopyName(const taBase& cp) {
  if(!cp.InheritsFrom(&TA_taNBase) || !owner || !owner->InheritsFrom(&TA_taList_impl))
    return false;
  
  if(dynamic_cast<const taNBase&>(cp).name.empty())
    return false;
  
  String tmp_name = dynamic_cast<const taNBase&>(cp).name;
  String root_name = dynamic_cast<const taNBase&>(cp).name;
  if (tmp_name.contains("_copy")) {
    // remove any suffix - could be just _copy or could be _copy_1 etc
    root_name = tmp_name.before("_copy");
  }
  // add back the extension
  tmp_name = root_name + "_copy";
  int idx = dynamic_cast<taList_impl*>(owner)->FindNameIdx(tmp_name);
  if (idx == -1) {
    name = tmp_name;
  }
  else {
    bool done = false;
    int i = 1;
    while (!done) {
      tmp_name = root_name + "_copy_" + taString(i);
      idx = dynamic_cast<taList_impl*>(owner)->FindNameIdx(tmp_name);
      if(idx == -1) {
        name = tmp_name;
        done = true;
      }
      else {
        i++;
      }
    }
  }
  return true;
}

bool taNBase::BrowserEditEnable() {
  if(GetMemberOwner() != NULL) {
    return false;               // if we are a member, can't edit
  }
  return true;
}

bool taNBase::BrowserEditSet(const String& new_val_str, int move_after) {
  if(move_after != -11) {
    taProject* proj = GET_MY_OWNER(taProject);
    if(proj) {
      proj->undo_mgr.SaveUndo(this, "BrowserEditSet", this);
    }
  }
  String use_val_str = new_val_str;
  if(use_val_str.contains(' '))
    use_val_str = use_val_str.before(' '); // exclude any additional info after spaces
  bool rval = SetName(use_val_str);
  UpdateAfterEdit();            // need full UAE
  return rval;
}
