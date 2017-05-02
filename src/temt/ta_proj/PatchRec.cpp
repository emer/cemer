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
  targ_idx = 0;
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

  if(TestWarning(!rval, "FindPathRobust",
                 "unable to find key object using following paths -- failing:\n",
                 obj_path_names, "\n",
                 obj_path_idx)) {
    return NULL;
  }
  return rval;
}

taList_impl* PatchRec::FindPathRobust_List(taProject* proj) {
  taBase* obj = FindPathRobust(proj);
  if(!obj) return NULL;
  if(TestWarning(!obj->InheritsFrom(&TA_taList_impl), "FindPathRobust_List",
                 "key object found at path is not of a List / Group type as required -- paths:\n",
                 obj_path_names, "\n",
                 obj_path_idx, "\nFound Obj Path and Type:",
                 obj->DisplayPath(), "type:", obj->GetTypeDef()->name)) {
    return NULL;
  }
  return (taList_impl*)obj;
}
  
bool PatchRec::ApplyPatch(taProject* proj) {
  bool rval = false;
  switch(action) {
  case NO_APPLY: {
    rval = true;                // nop
    break;
  }
  case ASSIGN: {
    rval = ApplyPatch_Assign(proj);
    break;
  }
  case REPLACE: {
    rval = ApplyPatch_Replace(proj);
    break;
  }
  case INSERT: {
    rval = ApplyPatch_Insert(proj);
    break;
  }
  case DELETE: {
    rval = ApplyPatch_Delete(proj);
    break;
  }
  }
  failed = !rval;
  if(failed) {
    taMisc::Warning("Apply of patch:", GetDisplayName(), "FAILED");
  }
  SigEmitUpdated();
  return rval;
}

bool PatchRec::ApplyPatch_Assign(taProject* proj) {
  taBase* obj = FindPathRobust(proj);
  if(!obj) return false;
  if(mbr_path.nonempty()) {
    TypeDef* own_td = obj->GetTypeDef();
    ta_memb_ptr net_mbr_off = 0;      int net_base_off = 0;
    MemberDef* md = TypeDef::FindMemberPathStatic
      (own_td, net_base_off, net_mbr_off, mbr_path, false); // no warn
    if(!md) {
      taMisc::Info("could not find member:",mbr_path,"in object",
                   obj->DisplayPath());
      return false;
    }
    void* addr = MemberDef::GetOff_static(obj, net_base_off, net_mbr_off);
    md->type->SetValStr(value, addr);
    taMisc::Info("ASSIGN target:", obj_path_names, "Mbr:\n",
                 obj->DisplayPath() + "." + mbr_path, "= " + value);
  }
  else {
    obj->Load_String(value);
    taMisc::Info("ASSIGN target:", obj_path_names, "Obj:\n",
                 obj->DisplayPath());
  }
  obj->UpdateAfterEdit();
  return true;
}

bool PatchRec::ApplyPatch_Replace(taProject* proj) {
  taList_impl* own = FindPathRobust_List(proj);
  if(!own) return false;
  TypeDef* new_typ = taMisc::FindTypeName(new_obj_type, true);
  if(!new_typ) return false;

  taBase* repl_obj = (taBase*)own->SafeEl_(targ_idx);
  if(!repl_obj || (repl_obj->GetName() != replace_name)) {
    repl_obj = (taBase*)own->FindName_(replace_name);
    if(!repl_obj) {
      taMisc::Warning("Replace patch cannot find object of name:", replace_name,
                      "or by index:", String(targ_idx), "failing");
      return false;
    }
  }
  int act_idx = own->FindEl_(repl_obj);
  String old_path = repl_obj->DisplayPath();
  taBase* tok = taBase::MakeToken(new_typ);
  own->ReplaceIdx(act_idx, tok);
  tok->Load_String(value);
  taMisc::Info("REPLACE target:", obj_path_names, "\n",
               old_path, "now ->", tok->DisplayPath());
  return true;
}
  
bool PatchRec::ApplyPatch_Delete(taProject* proj) {
  taBase* obj = FindPathRobust(proj);
  if(!obj) return false;
  taMisc::Info("DELETE target:", obj_path_names, "\n",
               obj->DisplayPath());
  obj->Close();
  return true;
}

int PatchRec::ApplyPatch_Insert_GetIdx(taList_impl* own) {
  if(insert_before.nonempty()) { // we can check this
    if(own->size > targ_idx) {   // does target index and insert_before match??
      taBase* bef_obj = (taBase*)own->FastEl_(targ_idx);
      if(bef_obj->GetName() == insert_before) {
        return targ_idx;        // good!  best case!
      }
    }
    int bef_idx = own->FindNameIdx(insert_before); // now try to find by name
    if(bef_idx >= 0) {
      return bef_idx;           // trust the name more than the original target index
    }
  }
  // still not sure -- try insert_after
  if(insert_after.nonempty()) {
    int aft_idx = own->FindNameIdx(insert_after);
    if(aft_idx >= 0) {
      return aft_idx+1;       // probably better than original index.. but.. maybe not..
    }
  }
  // ok, go with targ_idx in absence of any other info
  if(targ_idx > own->size) {
    return own->size;       // can't go bigger than that
  }
  return targ_idx;
}

bool PatchRec::ApplyPatch_Insert(taProject* proj) {
  taList_impl* own = FindPathRobust_List(proj);
  if(!own) return false;
  TypeDef* new_typ = taMisc::FindTypeName(new_obj_type, true);
  if(!new_typ) return false;
  taBase* tok = taBase::MakeToken(new_typ);

  static taList_impl* last_own = NULL;
  static int last_idx = -1;
  static int use_idx = -1;

  if(last_own == own && targ_idx == last_idx) {
    own->Insert(tok, use_idx);
    use_idx++;
  }
  else {
    int act_idx = ApplyPatch_Insert_GetIdx(own);
    own->Insert(tok, act_idx);
    use_idx = act_idx+1;        // next guy comes after us
  }
  
  last_own = own;
  last_idx = targ_idx;
    
  tok->Load_String(value);
  taMisc::Info("INSERT target:", obj_path_names, String(targ_idx), new_obj_type, "\n",
               tok->DisplayPath());
  return true;
}


/////////////////////////////////////////////////////
// new


bool PatchRec::NewRec_impl(taBase* obj, const String& val) {
  if(obj) {
    obj_path_names = obj->GetPathFromProj();
    obj_path_idx = obj->GetPath(obj->GetOwner(&TA_taProject));
    obj_type = obj->GetTypeDef()->name;
  }
  else {
    taMisc::Warning("Null obj in patch -- shouldn't happen!");
    return false;
  }
  value = val;
  return true;
}

bool PatchRec::NewRec_AssignMbr
(taBase* trg_indep_obj, taBase* trg_mbr_obj, MemberDef* md, const String& val) {
  bool rval = NewRec_impl(trg_indep_obj, val);
  if(!rval) return false;
  action = PatchRec::ASSIGN;
  String path;
  if(trg_mbr_obj != trg_indep_obj) { // sub-object member -- get path
    path = trg_mbr_obj->GetPath(trg_indep_obj) + ".";
    if(path.startsWith('.'))
      path = path.after('.');
  }
  path += md->name;
  mbr_path = path;
  return rval;
}

bool PatchRec::NewRec_AssignObj(taBase* trg_obj, taBase* src_obj) {
  String val;
  src_obj->Save_String(val);
  bool rval = NewRec_impl(trg_obj, val);
  if(!rval) return false;
  action = PatchRec::ASSIGN;
  return rval;
}

bool PatchRec::NewRec_Replace(taList_impl* own_obj, taBase* trg_obj, taBase* src_obj) {
  if(!own_obj || !own_obj->InheritsFrom(&TA_taList_impl)) {
    taMisc::Warning("For Replace Patch action, owning object not a list!",
                    own_obj->GetPathNames());
    return false;
  }
  String val;
  src_obj->Save_String(val);
  bool rval = NewRec_impl(own_obj, val);
  if(!rval) return false;
  action = PatchRec::REPLACE;
  new_obj_type = src_obj->GetTypeDef()->name;
  targ_idx = own_obj->FindEl(trg_obj); //
  replace_name = trg_obj->GetName();
  return rval;
}

bool PatchRec::NewRec_Delete(taBase* obj) {
  String val;
  obj->Save_String(val);
  bool rval = NewRec_impl(obj, val);
  if(!rval) return false;
  action = PatchRec::DELETE;
  return rval;
}

bool PatchRec::NewRec_Insert
(taList_impl* own_obj, taBase* add_obj, taBase* aft_obj, taBase* bef_obj) {
  if(!own_obj || !own_obj->InheritsFrom(&TA_taList_impl)) {
    taMisc::Warning("For Insert Patch action, owning object not a list!",
                    own_obj->GetPathNames());
    return false;
  }

  String val;
  add_obj->Save_String(val);
  bool rval = NewRec_impl(own_obj, val);
  if(!rval) return false;
  action = PatchRec::INSERT;
  new_obj_type = add_obj->GetTypeDef()->name;

  // NOTE: we typically have the "insert before" object or nothing (right now!)
  
  if(bef_obj) {
    insert_before = bef_obj->GetName();
    targ_idx = own_obj->FindEl(bef_obj);
  }
  else {
    targ_idx = 0;
    insert_before = "";
  }
  if(aft_obj) {
    insert_after = aft_obj->GetName();
  }
  return rval;
}

