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

#include <FileProgLib>
#include <NameVar>
#include <taMisc>
#include <Program>
#include <Program_Group>

#include <QDir>
#include <QStringList>

TA_BASEFUNS_CTORS_DEFN(FileProgLib);

FileProgLib::FileProgLib(const String& pth, const String& lib)
  : inherited(false)
  , path(pth)
{
  Initialize__(true);
  name = lib;
}

void FileProgLib::Initialize() {
}

void FileProgLib::FindPrograms() {
  Reset();                      // clear existing
  QDir dir(path);
  QStringList files = dir.entryList();
  for(int i=0;i<files.size();i++) {
    String fl = files[i];
    if(!fl.contains(".prog")) continue;
    ProgLibEl* pe = new ProgLibEl;
    pe->lib_name = name;
    if(pe->ParseProgFile(fl, path))
      Add(pe);
    else
      delete pe;
  }
  init = true;
}

bool FileProgLib::SaveProgGrpToProgLib(Program_Group* prg_grp, ProgLibs library) {
  QFileInfo qfi(path);
  if(!qfi.isDir()) {
    QDir qd;
    qd.mkpath(path);          // attempt to make it..
    taMisc::Warning("Note: did mkdir for program library directory:", path);
  }
  String fname = prg_grp->name + ".progp";
  String fpath = path + PATH_SEP + fname;
  QFileInfo qfi2(fpath);
  if(qfi2.isFile()) {
    int chs = taMisc::Choice("Program group library file: " + fpath + " already exists: Overwrite?",
                             "Ok", "Cancel");
    if(chs == 1) return false;
  }
  prg_grp->SaveAs(fpath);
  ProgLibEl* pe = new ProgLibEl;
  pe->lib_name = name;
  if(pe->ParseProgFile(fname, path))
    AddUniqNameNew(pe);
  else
    delete pe;
  return true;
}

bool FileProgLib::SaveProgToProgLib(Program* prg, ProgLibs library) {
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
  prg->SaveAs(fpath);
  ProgLibEl* pe = new ProgLibEl;
  pe->lib_name = name;
  if(pe->ParseProgFile(fname, path))
    AddUniqNameNew(pe);
  else
    delete pe;
  return true;
}
