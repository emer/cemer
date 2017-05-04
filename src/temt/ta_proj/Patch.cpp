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

PatchRec* Patch::NewRec_impl(const String& subgp) {
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
  return rval;
}

PatchRec* Patch::NewRec_AssignMbr
(taBase* trg_indep_obj, taBase* trg_mbr_obj, MemberDef* md, const String& val) {
  PatchRec* rval = NewRec_impl(cur_subgp);
  rval->NewRec_AssignMbr(trg_indep_obj, trg_mbr_obj, md, val);
  return rval;
}

PatchRec* Patch::NewRec_AssignObj(taBase* trg_obj, taBase* src_obj) {
  PatchRec* rval = NewRec_impl(cur_subgp);
  rval->NewRec_AssignObj(trg_obj, src_obj);
  return rval;
}

PatchRec* Patch::NewRec_Replace(taList_impl* own_obj, taBase* trg_obj, taBase* src_obj) {
  PatchRec* rval = NewRec_impl(cur_subgp);
  rval->NewRec_Replace(own_obj, trg_obj, src_obj);
  return rval;
}

PatchRec* Patch::NewRec_Delete(taBase* obj) {
  PatchRec* rval = NewRec_impl(cur_subgp);
  rval->NewRec_Delete(obj);
  return rval;
}

PatchRec* Patch::NewRec_Insert
(taList_impl* own_obj, taBase* add_obj, taBase* aft_obj, taBase* bef_obj) {
  PatchRec* rval = NewRec_impl(cur_subgp);
  rval->NewRec_Insert(own_obj, add_obj, aft_obj, bef_obj);
  return rval;
}

bool Patch::ApplyPatch(taProject* proj) {
  if(!proj) return false;
  proj->undo_mgr.SaveUndo(proj, "ApplyPatch", NULL, false, proj); // global save
  last_insert_own_path = "";
  last_before_idx = -1;
  last_obj_added_idx = -1;
  proj->StructUpdate(true);
  ++taMisc::is_loading;         // prevent lots of error messages etc -- patching is like loading
  bool fail_prompt = true;
  bool rval = true;
  FOREACH_ELEM_IN_GROUP(PatchRec, pat, patch_recs) {
    bool ok = pat->ApplyPatch(proj);
    if(!ok) {
      rval = false;
      if(fail_prompt) {
        int chs = taMisc::Choice("Last patch failed with message: " + pat->apply_info,
                                 "Continue -- Prompt Again", "Continue -- No More Prompts", "Abort");
        if(chs == 1) {
          fail_prompt = false;
        }
        if(chs == 2) {
          break;
        }
      }
    }
  }
  --taMisc::is_loading;
  proj->StructUpdate(false);
  return rval;
}

