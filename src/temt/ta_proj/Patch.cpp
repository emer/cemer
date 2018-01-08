// Co2018ght 2017-2017, Regents of the University of Colorado,
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
#include <Patch_Group>
#include <NameVar_Array>
#include <ObjDiff>

#include <taMisc>

TA_BASEFUNS_CTORS_DEFN(Patch);

String Patch::cur_subgp;
PatchLib* Patch::patch_lib = NULL;

void Patch::Initialize() {
  if(!patch_lib) {
    patch_lib = &Patch_Group::patch_lib;
  }
}


void Patch::SavePatch() {
  CallFun("SaveAs");
}

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
(ObjDiff* diff, bool a_or_b, taBase* trg_indep_obj, taBase* trg_mbr_obj, MemberDef* md, const String& val) {
  PatchRec* rval = NewRec_impl(cur_subgp);
  rval->NewRec_AssignMbr(diff, a_or_b, trg_indep_obj, trg_mbr_obj, md, val);
  return rval;
}

PatchRec* Patch::NewRec_AssignObj(ObjDiff* diff, bool a_or_b, taBase* trg_obj, taBase* src_obj) {
  PatchRec* rval = NewRec_impl(cur_subgp);
  rval->NewRec_AssignObj(diff, a_or_b, trg_obj, src_obj);
  return rval;
}

PatchRec* Patch::NewRec_Replace(ObjDiff* diff, bool a_or_b, taList_impl* own_obj, taBase* trg_obj, taBase* src_obj) {
  PatchRec* rval = NewRec_impl(cur_subgp);
  rval->NewRec_Replace(diff, a_or_b, own_obj, trg_obj, src_obj);
  return rval;
}

PatchRec* Patch::NewRec_Delete(ObjDiff* diff, bool a_or_b, taBase* obj) {
  PatchRec* rval = NewRec_impl(cur_subgp);
  rval->NewRec_Delete(diff, a_or_b, obj);
  return rval;
}

PatchRec* Patch::NewRec_Insert
(ObjDiff* diff, bool a_or_b, taList_impl* own_obj, taBase* add_obj, taBase* aft_obj, taBase* bef_obj) {
  PatchRec* rval = NewRec_impl(cur_subgp);
  rval->NewRec_Insert(diff, a_or_b, own_obj, add_obj, aft_obj, bef_obj);
  return rval;
}

bool Patch::ApplyPatch(taProject* proj) {
  if(!proj) return false;
  proj->undo_mgr.SaveUndo(proj, "ApplyPatch", NULL, false, proj); // global save
  last_insert_own_path = "";
  last_before_idx = -1;
  last_obj_added_idx = -1;
  ++taMisc::is_loading;         // prevent lots of error messages etc -- patching is like loading
  taMisc::loading_version = taMisc::version_bin;
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
  proj->UpdateAll();            // big one!
  --taMisc::is_loading;
  proj->DelayedUpdateUi();
  taMisc::Choice("The patch changes have now been applied.  Sometimes an additional F5 Update is needed to update the display completely.  Individual PatchRec records show the status, and contain informative messages about the Patch application process -- examining failed and warning diff cases is recommended.", "Ok");
  return rval;
}

void Patch::BuildPatchLib() {
  patch_lib->BuildLibrary();
}

void Patch::SaveToPatchLib(PatchLib::LibLocs location) {
  patch_lib->SaveToLibrary(location, this);
}

void Patch::UpdateFromPatchLib(ObjLibEl* patch_lib_item) {
  if(TestError(!patch_lib_item, "UpdateFromPatchLib", "patch library item is null")) return;
  patch_lib->UpdatePatch(this, patch_lib_item);
}

void Patch::RemoveOffs() {
  int n_leaves = patch_recs.leaves;
  for(int i=n_leaves-1; i >= 0; i--) {
    PatchRec* pat = patch_recs.Leaf(i);
    if(pat->off) {
      patch_recs.RemoveLeafIdx(i);
    }
  }
}


PatchRec* Patch::FindClosestRec(PatchRec* prec) {
  NameVar_Array vals;
  vals.SetSize(patch_recs.leaves);
  int idx = 0;
  FOREACH_ELEM_IN_GROUP(PatchRec, pat, patch_recs) {
    int value = pat->CompareRecs(prec);
    vals[idx].value = value;
    vals[idx].name = String(idx);
    idx++;
  }
  vals.Sort(true);              // descending, first is best
  int closest_idx = vals[0].name.toInt();
  PatchRec* closest = patch_recs.Leaf(closest_idx);
  return closest;
}
  
