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

#include "Patch_Group.h"
#include <Patch>
#include <taProject>
#include <taDateTime>
#include <taFiler>
#include <iHelpBrowser>

#include <taMisc>

TA_BASEFUNS_CTORS_DEFN(Patch_Group);

void Patch_Group::Initialize() {
  SetBaseType(&TA_Patch);
}

void Patch_Group::LoadPatch() {
  CallFun("Load");              // args etc
}

void Patch_Group::FindPatches(PatchLibs patch_library) {
  String local_path;
  String web_name;
  switch(patch_library) {
  case USER_LIB: {
    local_path = taMisc::prog_lib_paths.GetVal("UserLib").toString();
    break;
  }
  case SYSTEM_LIB: {
    local_path = taMisc::prog_lib_paths.GetVal("SystemLib").toString();
    break;
  }
  case WEB_APP_LIB: {
    web_name = taMisc::plib_app_wiki;
    break;
  }
  case WEB_SCI_LIB: {
    web_name = taMisc::plib_sci_wiki;
    break;
  }
  case WEB_USER_LIB: {
    web_name = taMisc::plib_user_wiki;
    break;
  }
  }

  if(local_path.nonempty()) {
    taFiler* flr = StatGetFiler(&TA_Patch, ".patch");
    flr->SetFileName(local_path);
    flr->Open();
  }
  else {
    String wiki_url = taMisc::GetWikiURL(web_name);
    String url = wiki_url + "PatchList";
    iHelpBrowser::StatLoadUrl(url);
  }
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
