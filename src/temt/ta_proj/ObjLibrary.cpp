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

#include "ObjLibrary.h"
#include <taFiler>
#include <iHelpBrowser>
#include <taMediaWiki>
#include <DataTable>
#include <taProjVersion>

#include <taMisc>

#include <QStringList>
#include <QDir>

TA_BASEFUNS_CTORS_DEFN(ObjLibrary);


void ObjLibrary::Initialize() {
  is_built = false;
}

void ObjLibrary::BrowseLibrary(LibLocs location) {
  if(file_paths.size == 0) {
    InitLibrary();
  }
  if(location < WEB_APP_LIB) {
    String path = file_paths[location];
    taFiler* flr = StatGetFiler(obj_type, file_ext);
    flr->SetFileName(path);
    flr->Open();
  }
  else {
    String wiki_url = taMisc::GetWikiURL(wiki_names[location]);
    String url = wiki_url + wiki_category + "List";
    iHelpBrowser::StatLoadUrl(url);
  }
}

void ObjLibrary::BuildLibrary() {
  if(is_built) return;
  BuildLibrary_impl();
}

void ObjLibrary::BuildLibrary_impl() {
  if(file_paths.size == 0) {
    InitLibrary();
  }
  taMisc::Busy();
  taMisc::Info("loading", obj_type->name, "library -- can take a few moments depending..");
  library.Reset();  // clear existing
  int i;
  for(i=0; i< WEB_APP_LIB; i++) {
    AddFromFiles(file_paths[i], lib_loc_names[i]);
  }
  for(; i<N_LIB_LOCS; i++) {
    AddFromWiki(wiki_names[i], file_paths[i], lib_loc_names[i]);
  }
  is_built = true;
  taMisc::DoneBusy();
}

void ObjLibrary::InitLibrary() {
  file_paths.Reset();
  wiki_names.Reset();
  lib_loc_names.Reset();
  file_paths.Add(taMisc::user_app_dir + PATH_SEP + file_subdir);
  lib_loc_names.Add("UserLib");
  wiki_names.Add("localhost");
  file_paths.Add(taMisc::app_dir + PATH_SEP + file_subdir);
  lib_loc_names.Add("SystemLib");
  wiki_names.Add("localhost");
  InitWikiData(taMisc::obj_lib_app_wiki, "WebAppLib");
  InitWikiData(taMisc::obj_lib_sci_wiki, "WebSciLib");
  InitWikiData(taMisc::obj_lib_user_wiki, "WebUserLib");
}

void ObjLibrary::InitWikiData(const String& wiki_name, const String& lib_loc_name) {
  String subdir_name = lib_loc_name.before("Lib").CamelToSnake() + "_" + file_subdir;
  file_paths.Add(taMisc::user_app_dir + PATH_SEP + subdir_name);
  wiki_names.Add(wiki_name);
  lib_loc_names.Add(lib_loc_name);
}


void ObjLibrary::AddFromFiles(const String& path, const String& lib_loc) {
  QDir dir(path);
  QStringList files = dir.entryList();
  for(int i=0;i<files.size();i++) {
    String fl = files[i];
    if(!fl.contains(file_ext))
      continue;
    ObjLibEl* pe = (ObjLibEl*)library.New(1);
    pe->lib_loc = lib_loc;
    if(!SetLibElFromFile(pe, fl, path)) {
      library.Pop();          // get rid of last if set fails
    }
  }
}

void ObjLibrary::AddFromWiki(const String& wiki_name, const String& loc_path, const String& lib_loc) {
  if(wiki_name.empty()) return;
  if(!taMisc::InternetConnected()) return;
  DataTable obj_list;
  taMediaWiki::QueryPagesByCategory(&obj_list, wiki_name, wiki_category);
  DataCol* pt_col = obj_list.FindColName("PageTitle");
  DataCol* pid_col = obj_list.FindColName("PageId");

  QFileInfo qfi(loc_path);
  if(!qfi.isDir()) {
    QDir qd;
    qd.mkpath(loc_path);          // attempt to make it..
    taMisc::Warning("Note: did mkdir for object library directory:", loc_path);
  }
  
  for (int i = 0; i < obj_list.rows; i++) {
    String obj_name = obj_list.GetVal(pt_col, i).toString();
    String obj_file = obj_name + file_ext;
    String path = loc_path + PATH_SEP + obj_file;
    // todo: could do a comparison of modification dates here and only download if newer!
    taMediaWiki::DownloadFile(wiki_name, obj_name, path);
    ObjLibEl* pe = (ObjLibEl*)library.New(1);
    pe->lib_loc = lib_loc;
    if(!SetLibElFromFile(pe, obj_file, loc_path)) {
      library.Pop();          // get rid of last if set fails
    }
  }
}

int ObjLibrary::FindNameInLocIdx(LibLocs location, const String& el_name) {
  return FindNameInLocNmIdx(lib_loc_names[location], el_name);
}

int ObjLibrary::FindNameInLocNmIdx(const String& lib_loc, const String& el_name) {
  for(int i=0; i<library.size; i++) {
    ObjLibEl* el = library.FastEl(i);
    if(el->name == el_name && el->lib_loc == lib_loc) {
      return i;
    }
  }
  return -1;
}

ObjLibEl* ObjLibrary::FindNameInLocEl(LibLocs location, const String& el_name) {
  return FindNameInLocNmEl(lib_loc_names[location], el_name);
}

ObjLibEl* ObjLibrary::FindNameInLocNmEl(const String& lib_loc, const String& el_name) {
  int idx = FindNameInLocNmIdx(lib_loc, el_name);
  if(idx < 0) return NULL;
  return library.FastEl(idx);
}

bool ObjLibrary::SaveToLibrary(LibLocs location, taBase* obj) {
  BuildLibrary();
  if(location >= WEB_APP_LIB) {
    return SaveToWiki(obj, file_paths[location], wiki_names[location],
                      lib_loc_names[location]);
  }
  else {
    return SaveToFile(obj, file_paths[location], lib_loc_names[location]);
  }
}


bool ObjLibrary::SaveToFile(taBase* obj, const String& path, const String& lib_loc) {
  QFileInfo qfi(path);
  if(!qfi.isDir()) {
    QDir qd;
    qd.mkpath(path);          // attempt to make it..
    taMisc::Warning("Note: did mkdir for program library directory:", path);
  }
  String fname = obj->GetName() + file_ext;
  String fpath = path + PATH_SEP + fname;
  QFileInfo qfi2(fpath);
  if(qfi2.isFile()) {
    int chs = taMisc::Choice("Object library file: " + fpath + " already exists: Overwrite?",
                             "Ok", "Cancel");
    if(chs == 1) return false;
  }
  obj->SaveAs(fpath);
  int cur_el_idx = FindNameInLocNmIdx(lib_loc, obj->GetName());
  if(cur_el_idx < 0) {
    ObjLibEl* pe = (ObjLibEl*)library.New(1);
    pe->lib_loc = lib_loc;
    if(!SetLibElFromFile(pe, fname, path)) {
      library.Pop();          // get rid of last if set fails
    }
  }
  return true;
}

bool ObjLibrary::SaveToWiki(taBase* obj, const String& wiki_name, const String& path, const String& lib_loc) {
  QFileInfo qfi(path);
  if(!qfi.isDir()) {
    QDir qd;
    qd.mkpath(path);          // attempt to make it..
    taMisc::Warning("Note: did mkdir for object library directory:", path);
  }

  String objnm = obj->GetName();
  
  String fname = objnm + file_ext;
  String fpath = path + PATH_SEP + fname;
  QFileInfo qfi2(fpath);
  if(qfi2.isFile()) {
    int chs = taMisc::Choice("Object library file: " + fpath + " already exists: Overwrite?",
                             "Ok", "Cancel");
    if(chs == 1) return false;
  }

  SetWikiInfoToObj(obj, wiki_name);
  obj->SaveAs(fpath);
  String page_name = objnm;

  String tago;
  String desco;
  taProjVersion verso;
  String autho;
  String emailo;
  String pubco;
  
  String* tags = &tago;
  String* desc = & desco;
  taProjVersion* version = &verso;
  String* author = &autho;
  String* email = &emailo;
  String* pub_cite = &pubco;

  GetWikiInfoFromObj(obj, tags, desc, version, author, email, pub_cite);

  taMediaWiki::PublishItemOnWeb
    (wiki_name, obj_type->name, objnm, fpath, page_name,
     *tags, *desc, *version, *author, *email, *pub_cite, obj);

  int cur_el_idx = FindNameInLocNmIdx(lib_loc, objnm);
  if(cur_el_idx < 0) {
    ObjLibEl* pe = (ObjLibEl*)library.New(1);
    pe->lib_loc = lib_loc;
    if(!SetLibElFromFile(pe, fname, path)) {
      library.Pop();          // get rid of last if set fails
    }
  }
  return true;
}
