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

TA_BASEFUNS_CTORS_DEFN(Patch);

String Patch::cur_subgp;

bool Patch::ApplyPatch(taProject* proj) {
  return false;
}

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
  rval->obj_path_names = obj->GetPathFromProj();
  rval->obj_path_idx = obj->GetPath(obj->GetOwner(&TA_taProject));
  rval->obj_type = obj->GetTypeDef()->name;
  rval->value = val;
  return rval;
}

PatchRec* Patch::NewPatchRec_Assign(taBase* obj, const String& val) {
  PatchRec* rval = NewPatchRec_impl(obj, val, cur_subgp);
  rval->action = PatchRec::ASSIGN;
  return rval;
}

PatchRec* Patch::NewPatchRec_Replace(taBase* obj, const String& val) {
  PatchRec* rval = NewPatchRec_impl(obj, val, cur_subgp);
  rval->action = PatchRec::REPLACE;
  return rval;
}
PatchRec* Patch::NewPatchRec_Insert(taBase* obj, const String& val) {
  PatchRec* rval = NewPatchRec_impl(obj, val, cur_subgp);
  rval->action = PatchRec::INSERT;
  return rval;
}
PatchRec* Patch::NewPatchRec_Delete(taBase* obj, const String& val) {
  PatchRec* rval = NewPatchRec_impl(obj, val, cur_subgp);
  rval->action = PatchRec::DELETE;
  return rval;
}
