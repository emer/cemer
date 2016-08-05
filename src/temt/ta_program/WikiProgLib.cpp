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

WikiProgLib::WikiProgLib(const String& wiki_nm, const String& lib)
  : inherited(false)
  , wiki_name(wiki_nm)
{
  Initialize__(true);
  name = lib;
}

void WikiProgLib::Initialize() {
}


String WikiProgLib::GetLocalCacheDir() {
  String rval = taMisc::prog_lib_paths.GetVal(name).toString();
  TestError(rval.empty(), "could not find prog_lib_paths for wiki prog lib:", name);
  return rval;
}

void WikiProgLib::FindPrograms() {
  Reset();  // clear existing
  if(!taMisc::InternetConnected()) return;
  DataTable progList;
  taMediaWiki::QueryPagesByCategory(&progList, wiki_name, "PublishedProgram");
  DataCol* pt_col = progList.FindColName("PageTitle");
  DataCol* pid_col = progList.FindColName("PageId");

  String loc_path = GetLocalCacheDir();
  QFileInfo qfi(loc_path);
  if(!qfi.isDir()) {
    QDir qd;
    qd.mkpath(loc_path);          // attempt to make it..
    taMisc::Warning("Note: did mkdir for program library directory:", loc_path);
  }
  
  for (int i = 0; i < progList.rows; i++) {
    String prog_name = progList.GetVal(pt_col, i).toString();
    String prog_file = prog_name + ".prog";
    String path = loc_path + PATH_SEP + prog_file;
    // todo: could do a comparison of modification dates here and only download if newer!
    taMediaWiki::DownloadFile(wiki_name, prog_name, path);
    ProgLibEl* pe = new ProgLibEl;
    pe->lib_name = name;
    if(pe->ParseProgFile(prog_file, loc_path))
      Add(pe);
    else
      delete pe;
  }
  init = true;
}

bool WikiProgLib::SaveProgGrpToProgLib(Program_Group* prg_grp, ProgLibs library) {
  taMisc::Error("saving program groups to Wiki is not supported");
  return false;
}

bool WikiProgLib::SaveProgToProgLib(Program* prg, ProgLibs library) {
  String path = GetLocalCacheDir();
  QFileInfo qfi(path);
  if(!qfi.isDir()) {
    QDir qd;
    qd.mkpath(path);          // attempt to make it..
    taMisc::Warning("Note: did mkdir for program library directory:", path);
  }
  String fname = prg->name + ".prog";
  String fpath = path + PATH_SEP + fname;
  QFileInfo qfi2(fpath);
  if(qfi2.isFile()) {
    int chs = taMisc::Choice("Program library file: " + fpath + " already exists: Overwrite?",
                             "Ok", "Cancel");
    if(chs == 1) return false;
  }
  prg->doc.wiki = wiki_name;
  prg->doc.url = prg->name;
  prg->SaveAs(fpath);
  String page_name = prg->name;
  String pub_cite;
  taMediaWiki::PublishItemOnWeb
    (wiki_name, "Program", prg->name, fpath, page_name,
     prg->tags, prg->desc, prg->version, prg->author, prg->email, pub_cite, prg);
  ProgLibEl* pe = new ProgLibEl;
  pe->lib_name = name;
  if(pe->ParseProgFile(fname, path))
    AddUniqNameNew(pe);
  else
    delete pe;
  return true;
}
