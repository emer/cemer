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

#include "ProgLibEl.h"
#include <Program_Group>
#include <taMisc>
#include <tabMisc>
#include <taRootBase>
#include <Program>

#include <QFileInfo>
#include <QDateTime>

TA_BASEFUNS_CTORS_DEFN(ProgLibEl);


using namespace std;


void ProgLibEl::Initialize() {
  is_group = false;
}

void ProgLibEl::Destroy() {
  is_group = false;             // just for a breakpoint..
}

taBase* ProgLibEl::NewProgram(Program_Group* new_owner) {
  // todo: need to support full URL types -- assumed to be file right now
  String path = URL;
  if(path.contains("file:"))
    path = path.after("file:");
  if(is_group) {
    Program_Group* pg = (Program_Group*)new_owner->NewGp(1);
    LoadProgramGroup(pg);
    Program* first_guy = pg->Leaf(0);
    if(taMisc::gui_active && first_guy)
      tabMisc::DelayedFunCall_gui(first_guy, "BrowserSelectMe");
    return pg;
  }

  Program* pg = new_owner->NewEl(1, &TA_Program);
  LoadProgram(pg);
  if(taMisc::gui_active)
    tabMisc::DelayedFunCall_gui(pg, "BrowserSelectMe");
  return pg;
}

bool ProgLibEl::LoadProgram(Program* prog) {
  // todo: need to support full URL types -- assumed to be file right now
  String path = URL;
  if(path.contains("file:"))
    path = path.after("file:");
  if(is_group) {
    taMisc::Error("ProgLibEl::LoadProgram -- cannot load a program group file into a single program!");
    return false;
  }
  prog->Load(path);
  prog->UpdateAfterEdit();      // make sure
  prog->RunLoadInitCode();
  return true;
}

bool ProgLibEl::LoadProgramGroup(Program_Group* prog_gp) {
  // todo: need to support full URL types -- assumed to be file right now
  String path = URL;
  if(path.contains("file:"))
    path = path.after("file:");
  if(!is_group) {
    taMisc::Error("ProgLibEl::LoadProgram -- cannot load a single program file into a program group!");
    return false;
  }
  prog_gp->Load(path);
  prog_gp->UpdateAfterEdit();
  for(int i=0;i<prog_gp->leaves;i++) {
    Program* prog = prog_gp->Leaf(i);
    prog->UpdateAfterEdit();    // make sure
    prog->RunLoadInitCode();
  }
  return true;
}

bool ProgLibEl::ParseProgFile(const String& fnm, const String& path) {
  filename = fnm;
  if(filename.contains(".progp"))
    is_group = true;
  else
    is_group = false;
  URL = "file:" + path + "/" + filename;
  String openfnm = path + "/" + filename;
  fstream strm;
  strm.open(openfnm, ios::in);
  if(strm.bad() || strm.eof()) {
    taMisc::Error("ProgLibEl::ParseProgFile: could not open file name:", openfnm);
    return false;
  }
  bool rval = false;
  int c = taMisc::read_till_rb_or_semi(strm); // skips over entire path header!
  while((c != EOF) && !strm.eof() && !strm.bad()) {
    c = taMisc::read_till_eol(strm); // skip next line
    if(c == EOF) break;
    if(taMisc::LexBuf.contains("name=")) {
      name = taMisc::LexBuf.after("name=");
      name.gsub("\"", "");
      if(name.lastchar() == ';') name = name.before(';');
    }
    if(taMisc::LexBuf.contains("tags=")) {
      tags = taMisc::LexBuf.after("tags=");
      tags.gsub("\"", "");
      if(tags.lastchar() == ';') tags = tags.before(';');
      ParseTags();
    }
    if(taMisc::LexBuf.contains("desc=")) {
      desc = taMisc::LexBuf.after("desc=");
      desc.gsub("\"", "");
      if(desc.lastchar() == ';') desc = desc.before(';');
      rval = true;
      break;
    }
  }
  strm.close();
  // todo: should use QUrlInfo instead -- need QtNetwork module access!
  QFileInfo urlinfo(openfnm);
  QDateTime mod = urlinfo.lastModified();
  date = mod.toString(Qt::ISODate);
  return rval;
}

void ProgLibEl::ParseTags() {
  tags_array.Reset();
  if(tags.empty())
    return;
  String tmp = tags;
  while(tmp.contains(',')) {
    String tag = tmp.before(',');
    tag.gsub(" ","");           // nuke spaces
    tags_array.Add(tag);
    tmp = tmp.after(',');
  }
  if(!tmp.empty()) {
    tmp.gsub(" ","");           // nuke spaces
    tags_array.Add(tmp);
  }
}
