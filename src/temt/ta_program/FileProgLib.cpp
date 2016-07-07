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

#include <QDir>
#include <QStringList>

TA_BASEFUNS_CTORS_DEFN(FileProgLib);

FileProgLib::FileProgLib(String path, String lib_name) {
  this->path = path;
  this->lib_name = lib_name;
}

void FileProgLib::Initialize() {
  not_init = true;
}

void FileProgLib::FindPrograms() {
  Reset();                      // clear existing
  QDir dir(path);
  QStringList files = dir.entryList();
  for(int i=0;i<files.size();i++) {
    String fl = files[i];
    if(!fl.contains(".prog")) continue;
    ProgLibEl* pe = new ProgLibEl;
    pe->lib_name = lib_name;
    if(pe->ParseProgFile(fl, path))
      Add(pe);
    else
      delete pe;
  }
  not_init = false;
}

taBase* FileProgLib::NewProgram(ProgLibEl* prog_type, Program_Group* new_owner) {
  if(prog_type == NULL) return NULL;
  return prog_type->NewProgram(new_owner);
}

taBase* FileProgLib::NewProgramFmName(const String& prog_nm, Program_Group* new_owner) {
  return NewProgram(FindName(prog_nm), new_owner);
}

bool FileProgLib::SaveProgGrpToProgLib(Program_Group* prg_grp) {
  return false;
}

bool FileProgLib::SaveProgToProgLib(Program* prg,  ProgLib::ProgLibs library) {
  QFileInfo qfi(path);
  if(!qfi.isDir()) {
    QDir qd;
    qd.mkpath(path);          // attempt to make it..
    taMisc::Warning("Note: did mkdir for program library directory:", path);
  }
  String fname = path + "/" + prg->name + ".prog";
  QFileInfo qfi2(fname);
  if(qfi2.isFile()) {
    int chs = taMisc::Choice("Program library file: " + fname + " already exists: Overwrite?",
                             "Ok", "Cancel");
    if(chs == 1) return false;
  }
  prg->SaveAs(fname);
  FindPrograms();
  return false;
}
