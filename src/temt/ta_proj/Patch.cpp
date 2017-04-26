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

PatchRec* Patch::NewPatchRec_Replace(taBase* obj, const String& val) {
  PatchRec* rval = NewPatchRec_impl(obj, val, cur_subgp);
  rval->action = PatchRec::REPLACE;
  return rval;
}

PatchRec* Patch::NewPatchRec_Insert
(taBase* obj, taBase* own, const String& val, const String& nw_tp) {
  PatchRec* rval = NewPatchRec_impl(obj, val, cur_subgp);
  rval->action = PatchRec::INSERT;
  rval->new_obj_type = nw_tp;

  String chldpath = obj->GetPath(own); // path relative to owner
  if(chldpath.contains(']')) {
    String sidx = chldpath.between('[',']');
    int idx = sidx.toInt();
    if(idx > 0) {
      String nwpath = chldpath.through('[') + String(idx-1) + "]";
      MemberDef* md;
      taBase* bfr = own->FindFromPath(nwpath, md);
      if(bfr) {
        rval->path_before_names = bfr->GetPathFromProj();
      }
    }
  }
  return rval;
}

PatchRec* Patch::NewPatchRec_Delete(taBase* obj, const String& val) {
  PatchRec* rval = NewPatchRec_impl(obj, val, cur_subgp);
  rval->action = PatchRec::DELETE;
  return rval;
}

bool Patch::ApplyPatch(taProject* proj) {
  proj->StructUpdate(true);
  bool rval = true;
  FOREACH_ELEM_IN_GROUP(PatchRec, pat, patch_recs) {
    bool ok = pat->ApplyPatch(proj);
    if(!ok) rval = false;
  }
  proj->StructUpdate(false);
  return rval;
}

