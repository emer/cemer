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

#include "ProgLib.h"
#include <Program>
#include <Program_Group>

#include <taMisc>
#include <tabMisc>

#include <QDir>
#include <QDateTime>
#include <taDateTime>

TA_BASEFUNS_CTORS_DEFN(ProgLib);

using namespace std;

void ProgLib::Initialize() {
  file_subdir = "prog_lib";
  file_ext = ".prog";
  wiki_category = "PublishedProgram";
  obj_type = &TA_Program;
}

taBase* ProgLib::NewProgram(Program_Group* new_owner, ObjLibEl* lib_el) {
  String path = lib_el->URL;
  if(path.contains("file:"))
    path = path.after("file:");
  if(path.endsWith(".progp")) {
    Program_Group* pg = (Program_Group*)new_owner->NewGp(1);
    UpdateProgramGroup(pg, lib_el);
    tabMisc::DelayedFunCall_gui(pg, "BrowserSelectFirstEl");
    return pg;
  }
  Program* pg = new_owner->NewEl(1, &TA_Program);
  UpdateProgram(pg, lib_el);
  if(taMisc::gui_active)
    tabMisc::DelayedFunCall_gui(pg, "BrowserSelectMe");
  return pg;
}

taBase* ProgLib::NewProgramFmName(Program_Group* new_owner, const String& prog_nm) {
  ObjLibEl* el = library.FindName(prog_nm);
  if(!el) {
    taMisc::Error("NewProgramFmName: could not find program of given name in library:",
                  prog_nm);
    return NULL;
  }
  return NewProgram(new_owner, el);
}

bool ProgLib::UpdateProgram(Program* prog, ObjLibEl* lib_el) {
  String path = lib_el->URL;
  if(path.contains("file:"))
    path = path.after("file:");
  if(path.endsWith(".progp")) {
    taMisc::Error("ProgLib::UpdateProgram -- cannot load a program group file into a single program!");
    return false;
  }
  prog->Load(path);
  prog->UpdateAfterEdit();      // make sure
  prog->SigEmitUpdateAllMembers();
  prog->RunLoadInitCode();
  return true;
}

bool ProgLib::UpdateProgramFmName(Program* prog, const String& prog_nm) {
  ObjLibEl* el = library.FindName(prog_nm);
  if(!el) {
    taMisc::Error("UpdateProgramFmName: could not find program of given name in library:",
                  prog_nm);
    return false;
  }
  return UpdateProgram(prog, el);
}

bool ProgLib::UpdateProgramGroup(Program_Group* prog_gp, ObjLibEl* lib_el) {
  String path = lib_el->URL;
  if(path.contains("file:"))
    path = path.after("file:");
  if(!path.endsWith(".progp")) {
    taMisc::Error("ProgLib::LoadProgramGroup -- cannot load a single program file into a program group!");
    return false;
  }
  prog_gp->Load(path);
  prog_gp->UpdateAfterEdit_NoGui();
  for(int i=0;i<prog_gp->leaves;i++) {
    Program* prog = prog_gp->Leaf(i);
    prog->UpdateAfterEdit_NoGui();    // make sure
    prog->SigEmitUpdated();
    prog->SigEmitUpdateAllMembers();
    prog->RunLoadInitCode();
  }
  return true;
}

void ProgLib::SetWikiInfoToObj(taBase* obj, const String& wiki_name) {
  Program* prg = (Program*)obj;
  prg->doc.wiki = wiki_name;
  prg->doc.url = prg->name;
}

void ProgLib::GetWikiInfoFromObj
(taBase* obj, String*& tags, String*& desc, taProjVersion*& version, String*& author,
 String*& email, String*& pub_cite) {
  Program* prg = (Program*)obj;
  tags = &prg->tags;
  desc = &prg->desc;
  version = &prg->version;
  author = &prg->author;
  email = &prg->email;
  // nothing for pub_cite -- has a backup val so just leave
}
  
bool ProgLib::SetLibElFromFile(ObjLibEl* lib_el, const String& fnm, const String& path) {
  lib_el->filename = fnm;
  lib_el->URL = "file:" + path + "/" + lib_el->filename;
  String openfnm = path + "/" + lib_el->filename;
  fstream strm;
  strm.open(openfnm, ios::in);
  if(strm.bad() || strm.eof()) {
    taMisc::Error("ObjLibEl::SetLibElFromFile: could not open file name:", openfnm);
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

