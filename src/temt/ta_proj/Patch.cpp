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

#include "Patch.h"
#include <PatchRec>
#include <PatchRec_Group>
#include <taProject>

#include <taMisc>

TA_BASEFUNS_CTORS_DEFN(Patch);

String Patch::cur_subgp;

PatchRec* Patch::NewPatchRec_impl(taBase* obj, const String& val,
                                  const String& subgp) {
  PatchRec* rval = NULL;
  if(subgp.empty()) {
    rval = (PatchRec*)patch_recs.NewEl(1);
  }
  else {
    PatchRec_Group* sub = (PatchRec_Group*)patch_recs.gp.FindName(subgp);
    if(!sub) {
      sub = (PatchRec_Group*)patch_recs.NewGp(1);
      sub->name = subgp;
    }
    rval = (PatchRec*)sub->NewEl(1);
  }
  if(obj) {
    rval->obj_path_names = obj->GetPathFromProj();
    rval->obj_path_idx = obj->GetPath(obj->GetOwner(&TA_taProject));
    rval->obj_type = obj->GetTypeDef()->name;
  }
  else {
    taMisc::Warning("Null obj in patch -- shouldn't happen!");
  }
  rval->value = val;
  return rval;
}

PatchRec* Patch::NewPatchRec_Assign(taBase* obj, const String& val) {
  PatchRec* rval = NewPatchRec_impl(obj, val, cur_subgp);
  rval->action = PatchRec::ASSIGN;
  return rval;
}

PatchRec* Patch::NewPatchRec_Replace(taBase* obj, const String& val, taBase* new_obj) {
  PatchRec* rval = NewPatchRec_impl(obj, val, cur_subgp);
  rval->action = PatchRec::REPLACE;
  rval->new_obj_type = new_obj->GetTypeDef()->name;
  return rval;
}

PatchRec* Patch::NewPatchRec_Insert
(taBase* add_obj, taBase* own_obj, taBase* bef_obj, taBase* aft_obj, const String& val) {
  PatchRec* rval = NewPatchRec_impl(own_obj, val, cur_subgp);
  rval->action = PatchRec::INSERT;
  rval->new_obj_type = add_obj->GetTypeDef()->name;

  if(bef_obj) {
    rval->path_before = bef_obj->GetPathNames(own_obj);
  }
  else {
    rval->path_before = "NULL";
  }
  if(aft_obj) {
    rval->path_after = aft_obj->GetPathNames(own_obj);
  }
  else {
    rval->path_after = "NULL";
  }
  return rval;
}

PatchRec* Patch::NewPatchRec_Delete(taBase* obj, const String& val) {
  PatchRec* rval = NewPatchRec_impl(obj, val, cur_subgp);
  rval->action = PatchRec::DELETE;
  return rval;
}

bool Patch::ApplyPatch(taProject* proj) {
  last_insert_own_path = "";
  last_before_idx = -1;
  last_obj_added_idx = -1;
  proj->StructUpdate(true);
  bool rval = true;
  FOREACH_ELEM_IN_GROUP(PatchRec, pat, patch_recs) {
    bool ok = pat->ApplyPatch(proj);
    if(!ok) rval = false;
  }
  proj->StructUpdate(false);
  return rval;
}

