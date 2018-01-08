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

#include "MemberProgEl.h"
#include <Program>
#include <ProgVar>
#include <taMisc>
#include <MemberDef>

TA_BASEFUNS_CTORS_DEFN(MemberProgEl);


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
    if(member_lookup->type->IsActualClassNoEff())
      path += ".";
    member_lookup = NULL;
  }
  path = trim(path);                                       // keep it clean
  GetTypeFromPath();
}

bool MemberProgEl::GetTypeFromPath() {
  if(!obj) {
    obj_type = &TA_taBase; // placeholder
    return false;
  }
  TypeDef* ot = obj->act_object_type();
  taBase* base_base = obj->object_val;

  taBase* path_obj = NULL;
  TypeDef* path_type = MemberProgEl::GetObjTypeFromPath(path, ot, base_base, path_obj);

  if(path_type) {
    obj_type = path_type;
    return true;
  }
  return false;
}

TypeDef* MemberProgEl::GetObjTypeFromPath
(const String& path, TypeDef* base_type, taBase* base_obj, taBase*& path_obj) {
  if(!base_type) {
    return NULL;
  }
  MemberDef* md = NULL;
  if(base_obj) {
    taBase* mb_tab = base_obj->FindFromPath(path, md);
    if(mb_tab) {
      return mb_tab->GetTypeDef();
    }
  }

  // didn't get it yet, try with static
  int net_base_off = 0;
  md = TypeDef::FindMemberPathStatic(base_type, net_base_off, path, false);
  // gets static path based just on member types..
  if(md && md->type->IsActualTaBase()) {
    return md->type;
  }

  // failed again -- try parent..

  int path_delim = taBase::GetLastPathDelimPos(path);
  int arrow_idx = taMisc::find_not_in_quotes(path, '>',-1);
  if(arrow_idx > 0) {
    if(path[arrow_idx-1] == '-')
      arrow_idx--;            // get at start
    else
      arrow_idx = -1;
  }
  if(arrow_idx > path_delim)
    path_delim = arrow_idx;
  if(path_delim > 0) {
    String pre_path = path.before(path_delim);
    return GetObjTypeFromPath(pre_path, base_type, base_obj, path_obj);
  }
  return NULL;
}

bool MemberProgEl::UAEInProgram(const String& path, TypeDef* base_type, taBase* base_obj) {
  const String uae_in_prog = "UAE_IN_PROGRAM";
  if(!base_type) {
    return false;
  }
  if(base_type->HasOption(uae_in_prog))
    return true;
  
  MemberDef* md = NULL;
  if(base_obj) {
    taBase* mb_tab = base_obj->FindFromPath(path, md);
    if(mb_tab) {
      if(mb_tab->GetTypeDef()->HasOption(uae_in_prog))
        return true;
    }
  }

  int net_base_off = 0;
  md = TypeDef::FindMemberPathStatic(base_type, net_base_off, path, false);
  // gets static path based just on member types..
  if(md && md->type->HasOption(uae_in_prog)) {
    return true;
  }

  // failed again -- try parent..
  int path_delim = taBase::GetLastPathDelimPos(path);
  int arrow_idx = taMisc::find_not_in_quotes(path, '>',-1);
  if(arrow_idx > 0) {
    if(path[arrow_idx-1] == '-')
      arrow_idx--;            // get at start
    else
      arrow_idx = -1;
  }
  if(arrow_idx > path_delim)
    path_delim = arrow_idx;
  if(path_delim > 0) {
    String pre_path = path.before(path_delim);
    return UAEInProgram(pre_path, base_type, base_obj);
  }
  return false;
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

