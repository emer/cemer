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

#include "WikiProgLib.h"
#include <taMediaWiki>
#include <QDir>
#include <QFileInfo>
#include <Program>
#include <taMisc>
#include <DataTable>
#include <DataCol>

TA_BASEFUNS_CTORS_DEFN(WikiProgLib);

WikiProgLib::WikiProgLib(String wiki_url, String lib_name) {
  this->wiki_url = wiki_url;
  this->lib_name = lib_name;
}

void WikiProgLib::Initialize() {
  not_init = true;
}

void WikiProgLib::FindPrograms() {
  Reset();  // clear existing
  DataTable progList;
  taMediaWiki::QueryPagesByCategory(&progList, wiki_url, "PublishedProgram");
  DataCol* pt_col = progList.FindColName("PageTitle");
  DataCol* pid_col = progList.FindColName("PageId");
  
  for (int i = 0; i < progList.rows; i++) {
    String prog_name = progList.GetVal(pt_col, i).toString();
    String path = QDir::tempPath() + "/" + prog_name + ".prog";
    taMediaWiki::DownloadFile(wiki_url, prog_name, path);
    ProgLibEl* pe = new ProgLibEl;
    pe->lib_name = lib_name;
    if(pe->ParseProgFile(prog_name + ".prog", QDir::tempPath()))
      Add(pe);
    else
      delete pe;
  }
  //TODO: Implement this
}

taBase* WikiProgLib::NewProgram(ProgLibEl* prog_type, Program_Group* new_owner) {
  if(prog_type == NULL) return NULL;
  return prog_type->NewProgram(new_owner);
}

taBase* WikiProgLib::NewProgramFmName(const String& prog_nm, Program_Group* new_owner) {
  return NewProgram(FindName(prog_nm), new_owner);
}

bool WikiProgLib::SaveProgGrpToProgLib(Program_Group* prg_grp, ProgLibs library) {
  return false;
}

bool WikiProgLib::SaveProgToProgLib(Program* prg, ProgLibs library) {
  String path = QDir::tempPath();
  String fname = path + "/" + prg->name + ".prog";
  QFileInfo qfi(fname);
  if(qfi.isFile()) {
    int chs = taMisc::Choice("Program library file: " + fname + " already exists: Overwrite?",
                             "Ok", "Cancel");
    if(chs == 1) return false;
  }
  prg->SaveAs(fname);
  taMediaWiki::PublishItemOnWeb("Program", prg->name, fname, wiki_url , prg->GetMyProj());
  QFile::remove(fname);
  return false;
}
