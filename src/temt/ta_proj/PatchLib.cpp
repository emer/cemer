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

#include "PatchLib.h"
#include <Patch>
#include <Patch_Group>

#include <taMisc>
#include <tabMisc>

TA_BASEFUNS_CTORS_DEFN(PatchLib);

using namespace std;

void PatchLib::Initialize() {
  file_subdir = "patch_lib";
  file_ext = ".patch";
  wiki_category = "PublishedPatch";
  obj_type = &TA_Patch;
}

Patch* PatchLib::NewPatch(Patch_Group* new_owner, ObjLibEl* lib_el) {
  Patch* pat = (Patch*)new_owner->NewEl(1);
  UpdatePatch(pat, lib_el);
  if(taMisc::gui_active)
    tabMisc::DelayedFunCall_gui(pat, "BrowserSelectMe");
  return pat;
}

Patch* PatchLib::NewPatchFmName(Patch_Group* new_owner, const String& patch_nm) {
  ObjLibEl* el = library.FindName(patch_nm);
  if(!el) {
    taMisc::Error("NewPatchFmName: could not find patch of given name in library:",
                  patch_nm);
    return NULL;
  }
  return NewPatch(new_owner, el);
}

bool PatchLib::UpdatePatch(Patch* pat, ObjLibEl* lib_el) {
  EnsureDownloaded(lib_el);
  String path = lib_el->path;
  pat->Load(path);
  return true;
}

void PatchLib::SetWikiInfoToObj(taBase* obj, const String& wiki_name) {
  Patch* pat = (Patch*)obj;
  // pat->doc.wiki = wiki_name;
  // pat->doc.url = pat->name;
}

void PatchLib::GetWikiInfoFromObj
(taBase* obj, String*& tags, String*& desc, taProjVersion*& version, String*& author,
 String*& email, String*& pub_cite) {
  Patch* pat = (Patch*)obj;
  tags = &pat->tags;
  desc = &pat->desc;
  //  version = &pat->version;
  author = &pat->author;
  email = &pat->email;
  // nothing for pub_cite -- has a backup val so just leave
}
  
