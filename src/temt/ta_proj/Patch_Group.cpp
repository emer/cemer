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

#include "Patch_Group.h"
#include <Patch>
#include <PatchRec>
#include <taProject>
#include <taDateTime>
#include <taFiler>
#include <iHelpBrowser>

#include <taMisc>

TA_BASEFUNS_CTORS_DEFN(Patch_Group);

PatchLib Patch_Group::patch_lib;

void Patch_Group::Initialize() {
  SetBaseType(&TA_Patch);
}

void Patch_Group::LoadPatch() {
  CallFun("Load");              // args etc
}

Patch* Patch_Group::NewPatch() {
  Patch* patch = (Patch*)NewEl(1);
  taProject* own = GET_MY_OWNER(taProject);
  if(own) {
    patch->author = own->author;
    patch->email = own->email;
    patch->date = taDateTime::CurrentDateTimeStampString();
  }
  return patch;
}

void Patch_Group::BuildPatchLib() {
  patch_lib.BuildLibrary();
}

Patch* Patch_Group::AddFromPatchLib(ObjLibEl* patch_lib_item) {
  return patch_lib.NewPatch(this, patch_lib_item);
}

Patch* Patch_Group::AddFromPatchLibByName(const String& patch_nm) {
  return patch_lib.NewPatchFmName(this, patch_nm);
}

void Patch_Group::BrowsePatchLib(PatchLib::LibLocs location) {
  patch_lib.BrowseLibrary(location);
}

Patch* Patch_Group::MergePatches(Patch* patch1, Patch* patch2) {
  Patch* merge = NewPatch();
  merge->CopyFrom(patch1);
  merge->name = "merge_" + patch1->name + "_" + patch2->name;
  FOREACH_ELEM_IN_GROUP(PatchRec, pat, patch2->patch_recs) {
    PatchRec* closest = merge->FindClosestRec(pat);
    taList_impl* clown = (taList_impl*)closest->GetOwner(); // could be in subgp
    int clidx = clown->FindEl(closest);
    PatchRec* newpat = (PatchRec*)pat->Clone();
    clown->Insert(newpat, clidx+1); // insert after
    closest->ConflictOrDupeCheck(newpat);
  }
  merge->SigEmitUpdated();
  return merge;
}
