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

#include "MemberProgEl.h"
#include <Program>
#include <ProgVar>
#include <taMisc>
#include <MemberDef>


void MemberProgEl::Initialize() {
  obj_type = &TA_taBase; // placeholder
  member_lookup = NULL;
}

void MemberProgEl::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  if(member_lookup) {
    if(!path.empty() && (path.lastchar() != '.')) {
      if(path.contains('.')) {
        path = path.through('.',-1);
      }
      else {
        path = "";
      }
    }
    path += member_lookup->name;
    if(member_lookup->type->InheritsFormal(&TA_class) &&
       !member_lookup->type->InheritsFrom(&TA_taString))
      path += ".";
    member_lookup = NULL;
  }
  path = trim(path);                                       // keep it clean
  GetTypeFromPath();
}

// StringFieldLookupFun is in ta_program_qt.cpp

bool MemberProgEl::GetTypeFromPath(bool quiet) {
  if(!obj) {
    obj_type = &TA_taBase; // placeholder
    return false;
  }
  TypeDef* ot = obj->act_object_type();
  taBase* base_base = obj->object_val;
  MemberDef* md = NULL;
  bool rval = false;
  if(base_base) {
    taBase* mb_tab = base_base->FindFromPath(path, md);
    if(mb_tab) {
      obj_type = mb_tab->GetTypeDef();
      rval = true;
    }
  }
  if(!rval) {                   // didn't get it yet, try with static
    int net_base_off = 0;
    ta_memb_ptr net_mbr_off = 0;
    md = TypeDef::FindMemberPathStatic(ot, net_base_off, net_mbr_off, path, false);
    // gets static path based just on member types..
    if(md) {
      obj_type = md->type;      // it is the type of the member, not the owner type.
    }
    rval = (bool)md;
  }
  return rval;
}

void MemberProgEl::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  CheckError(!obj, quiet, rval, "obj is NULL");
  CheckError(path.empty(), quiet, rval, "path is empty");
}

void MemberProgEl::Help() {
  if(obj && (bool)obj->object_val) {
    obj->object_val->Help();
    // todo: add check for member and select that in class browser guy..
  }
  else {
    inherited::Help();
  }
}

// bool MemberProgEl::CanCvtFmCode(const String& code, ProgEl* scope_el) const {
//   return false;
// }

// bool MemberProgEl::CvtFmCode(const String& code) {
//   return false;
// }

