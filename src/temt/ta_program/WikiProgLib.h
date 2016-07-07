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

#ifndef WikiProgLib_h
#define WikiProgLib_h 1

// parent includes:
#include <taNBase>
#include <ProgLib>

// member includes:

// declare all other types mentioned but not required to include:

taTypeDef_Of(WikiProgLib);

class TA_API WikiProgLib : public ProgLib {
  // <describe here in full detail in one extended line comment>
INHERITED(ProgLib)
public:
  WikiProgLib(String wiki_url, String lib_name);
  void  FindPrograms() override;         // search paths to find all available programs
  taBase* NewProgram(ProgLibEl* prog_type, Program_Group* new_owner);

  taBase* NewProgramFmName(const String& prog_nm, Program_Group* new_owner);
  // create a new program (lookup by name) (return value could be a Program or a Program_Group, or NULL if not found); new_owner is group where program will be created
  bool SaveProgToProgLib(Program* prg, ProgLibs library) override;
  bool SaveProgGrpToProgLib(Program_Group* prg_grp, ProgLibs library);


  TA_SIMPLE_BASEFUNS(WikiProgLib);
private:
  String wiki_url;
  String lib_name;
  void Initialize();
  void Destroy()     { };
};

#endif // WikiProgLib_h
