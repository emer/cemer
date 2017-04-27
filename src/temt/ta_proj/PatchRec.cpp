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

#include "PatchRec.h"
#include <taProject>
#include <MemberDef>
#include <taMisc>

TA_BASEFUNS_CTORS_DEFN(PatchRec);

void PatchRec::Initialize() {
  action = NO_APPLY;
  failed = false;
}

String PatchRec::GetDisplayName() const {
  MemberDef* md = GetTypeDef()->members.FindName("action");
  String rval = md->type->Get_C_EnumString(action, false);
  if(action == INSERT) {
    rval += "_" + new_obj_type;
  }
  else if(action == REPLACE) {
    rval += "_" + obj_type + "->" + new_obj_type;
  }
  else {
    rval += "_" + obj_type;
  }
  return rval;
}

int PatchRec::GetEnabled() const {
  return (action != NO_APPLY);
}

int PatchRec::GetSpecialState() const {
  if(failed) return 4;          // red
  return 0;
}

taBase* PatchRec::CheckObjType(taProject* proj, taBase* obj, const String& path_used) {
  if(!obj) return obj;
  if(!obj->InheritsFromName(obj_type)) {
    // todo: maybe prompt user for finding something else instead?
    taMisc::Info("object type:", obj->GetTypeDef()->name, "doesn't match target:", 
                 obj_type, "at path:", path_used);
  }
  return obj;
}

taBase* PatchRec::FindPathRobust(taProject* proj) {
  MemberDef* md;
  taBase* rval = proj->FindFromPath(obj_path_names, md);
  if(rval) {
    return CheckObjType(proj, rval, obj_path_names);
  }
  rval = proj->FindFromPath(obj_path_idx, md);
  if(rval) {
    return CheckObjType(proj, rval, obj_path_idx);
  }
  // todo: more fancy stuff here..
  return rval;
}

bool PatchRec::ApplyPatch(taProject* proj) {
  bool rval = false;
  switch(action) {
  case NO_APPLY: {
    rval = true;                // nop
    break;
  }
  case ASSIGN: {
    rval = ApplyPatch_assign(proj);
    break;
  }
  case REPLACE: {
    rval = ApplyPatch_replace(proj);
    break;
  }
  case INSERT: {
    rval = ApplyPatch_insert(proj);
    break;
  }
  case DELETE: {
    rval = ApplyPatch_delete(proj);
    break;
  }
  }
  failed = !rval;
  if(failed) {
    taMisc::Warning("Apply of patch:", GetDisplayName(), "FAILED");
    SigEmitUpdated();
  }
  return rval;
}

bool PatchRec::ApplyPatch_assign(taProject* proj) {
  taBase* obj = FindPathRobust(proj);
  if(!obj) return false;
  if(mbr_path.nonempty()) {
    MemberDef* md = obj->GetTypeDef()->members.FindName(mbr_path);
    if(!md) {
      taMisc::Info("could not find member:",mbr_path,"in object",
                   obj->DisplayPath());
      return false;
    }
    md->SetValStr(value, obj, TypeDef::SC_STREAMING, false);
    taMisc::Info("ASSIGN of:", obj->DisplayPath() + "." + mbr_path, "to:" + value);
  }
  else {
    obj->Load_String(value);
    taMisc::Info("ASSIGN of:", obj->DisplayPath());
  }
  obj->UpdateAfterEdit();
  return true;
}

bool PatchRec::ApplyPatch_replace(taProject* proj) {
  taBase* obj = FindPathRobust(proj);
  if(!obj) return false;
  taBase* own = obj->GetOwner();
  if(!own) return false;
  TypeDef* new_typ = taMisc::FindTypeName(new_obj_type, true);
  if(!new_typ) return false;
  taBase* tok = taBase::MakeToken(new_typ);
  String old_path = obj->DisplayPath();
  taBase* new_guy = own->CopyChildBefore(tok, obj);
  if(!new_guy) {
    return false;
  }
  new_guy->Load_String(value);
  taMisc::Info("REPLACE of:", old_path, "with:", new_guy->DisplayPath());
  obj->Close();       // nuke old guy
  return true;
}
  
bool PatchRec::ApplyPatch_insert(taProject* proj) {
  // todo: need a new routine to find the owner here..
  taBase* obj = FindPathRobust(proj);
  if(!obj) return false;
  taBase* own = obj->GetOwner();
  if(!own) return false;
  TypeDef* new_typ = taMisc::FindTypeName(new_obj_type, true);
  if(!new_typ) return false;
  taBase* tok = taBase::MakeToken(new_typ);
  taBase* new_guy = own->CopyChildBefore(tok, obj);
  if(!new_guy) {
    return false;
  }
  new_guy->Load_String(value);
  taMisc::Info("INSERT of:", new_guy->DisplayPath());
  return true;
}

bool PatchRec::ApplyPatch_delete(taProject* proj) {
  taBase* obj = FindPathRobust(proj);
  if(!obj) return false;
  taMisc::Info("DELETE of:", obj->DisplayPath());
  obj->Close();
  return true;
}
