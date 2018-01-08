// Copyright 2017-2017, Regents of the University of Colorado,
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
#include <taDateTime>

#include <taMisc>

#include <QStringList>
#include <QDir>

TA_BASEFUNS_CTORS_DEFN(ObjLibrary);

using namespace std;

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
    AddFromFiles((LibLocs)i);
  }
  if (taMisc::gui_active) {
    for(; i<N_LIB_LOCS; i++) {
      AddFromWiki((LibLocs)i);
    }
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


ObjLibEl* ObjLibrary::NewLibRec(LibLocs lib_loc, const String& fname, const String& obj_nm) {
  ObjLibEl* lel = (ObjLibEl*)library.New(1);
  lel->lib_loc = lib_loc_names[lib_loc];
  lel->lib_loc_no = lib_loc;
  lel->filename = fname;
  if(obj_nm.empty()) {
    lel->name = fname.before(file_ext);
  }
  else {
    lel->name = obj_nm;
  }
  lel->path = file_paths[lib_loc] + PATH_SEP + fname;
  if(lib_loc < WEB_APP_LIB) {
    lel->URL = "file:" + lel->path;
  }
  else {
    lel->URL = taMisc::GetWikiURL(wiki_names[lib_loc]) + lel->name;
  }
  if(!SetLibElFromFile(lel)) {
    library.Pop();          // get rid of last if set fails
    return NULL;
  }
  return lel;
}

void ObjLibrary::AddFromFiles(LibLocs lib_loc) {
  String path = file_paths[lib_loc];
  QDir dir(path);
  QStringList files = dir.entryList();
  for(int i=0;i<files.size();i++) {
    String fl = files[i];
    if(!fl.contains(file_ext))
      continue;
    ObjLibEl* pe = NewLibRec(lib_loc, fl, ""); // gets name from filename
  }
}

void ObjLibrary::AddFromWiki(LibLocs lib_loc) {
  if(!taMisc::InternetConnected()) return;
  
  String wiki_name = wiki_names[lib_loc];
  String loc_path = file_paths[lib_loc];
  String lib_loc_nm = lib_loc_names[lib_loc];
  DataTable obj_list(false);
  obj_list.OwnTempObj(); // this is ESSENTIAL for temp data tables -- otherwise cols can't access their parent table b/c owner is not set!
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
    QFileInfo qfi2(path);
    if(!qfi2.exists()) {
      // todo: could do a comparison of modification dates here and only download if newer!
      // meanwhile, assuming that if you have it, it is good enough for the chooser
      // but download if you actually want to use it!
      taMediaWiki::DownloadFile(wiki_name, obj_name, path);
    }
    ObjLibEl* pe = NewLibRec(lib_loc, obj_file, obj_name);
  }
}

bool ObjLibrary::EnsureDownloaded(ObjLibEl* lib_el) {
  if(lib_el->lib_loc_no < WEB_APP_LIB)
    return true;
  // todo: need to get timestamp from server..
  taMediaWiki::DownloadFile(wiki_names[lib_el->lib_loc_no], lib_el->name, lib_el->path);
  return true;
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
    return SaveToWiki(location, obj);
  }
  else {
    return SaveToFile(location, obj);
  }
}


bool ObjLibrary::SaveToFile(LibLocs lib_loc, taBase* obj) {
  String path = file_paths[lib_loc];
  if(lib_loc == SYSTEM_LIB && !taMisc::in_dev_exe) { // dev exe already loads from dev dir
    // attempt to save to emergent build dir if that is found..
    String try_path = taMisc::user_dir + PATH_SEP + taMisc::app_name + PATH_SEP +
      file_subdir;
    QFileInfo qfit(try_path);
    if(qfit.isDir()) {
      path = try_path;
    }
  }
  QFileInfo qfi(path);
  if(!qfi.isDir()) {
    QDir qd;
    qd.mkpath(path);          // attempt to make it..
    taMisc::Warning("Note: did mkdir for program library directory:", path);
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
  obj->SaveAs(fpath);
  taMisc::Info("Saved:", objnm, "to:", fpath);
  int cur_el_idx = FindNameInLocNmIdx(lib_loc, objnm);
  if(cur_el_idx < 0) {
    ObjLibEl* pe = NewLibRec(lib_loc, fname, objnm);
  }
  return true;
}

bool ObjLibrary::SaveToWiki(LibLocs lib_loc, taBase* obj) {
  String wiki_name = wiki_names[lib_loc];
  String path = file_paths[lib_loc];
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
    ObjLibEl* pe = NewLibRec(lib_loc, fname, objnm);
  }
  return true;
}

bool ObjLibrary::SetLibElFromFile(ObjLibEl* lib_el) {
  String openfnm = lib_el->path;
  fstream strm;
  strm.open(openfnm, ios::in);
  if(strm.bad() || strm.eof()) {
    taMisc::Error("ObjLibrary::SetLibElFromFile: could not open file name:", openfnm);
    return false;
  }
  bool rval = false;
  int c = taMisc::read_till_rb_or_semi(strm); // skips over entire path header!
  while((c != EOF) && !strm.eof() && !strm.bad()) {
    c = taMisc::read_till_eol(strm); // skip next line
    if(c == EOF) break;
    if(taMisc::LexBuf.contains("name=")) {
      lib_el->name = taMisc::LexBuf.after("name=");
      lib_el->name.gsub("\"", "");
      if(lib_el->name.lastchar() == ';') lib_el->name = lib_el->name.before(';');
    }
    if(taMisc::LexBuf.contains("tags=")) {
      lib_el->tags = taMisc::LexBuf.after("tags=");
      lib_el->tags.gsub("\"", "");
      if(lib_el->tags.lastchar() == ';') lib_el->tags = lib_el->tags.before(';');
      lib_el->TagsToArray();
    }
    if(taMisc::LexBuf.contains("desc=")) {
      lib_el->desc = taMisc::LexBuf.after("desc=");
      lib_el->desc.gsub("\"", "");
      if(lib_el->desc.lastchar() == ';') lib_el->desc = lib_el->desc.before(';');
      rval = true;
      break;
    }
  }
  strm.close();
  QFileInfo urlinfo(openfnm);
  QDateTime mod = urlinfo.lastModified();
  lib_el->date = mod.toString(taDateTime::DateTimeStampFormat);
  return rval;
}

