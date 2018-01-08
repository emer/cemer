// Copyright 2013-2017, Regents of the University of Colorado,
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

#ifndef ProgLib_h
#define ProgLib_h 1

// parent includes:
#include <ObjLibrary>

// member includes:

// declare all other types mentioned but not required to include:
class Program_Group; // 
class Program; //

taTypeDef_Of(ProgLib);

class TA_API ProgLib : public ObjLibrary {
  // program library -- library of programs
INHERITED(ObjLibrary)
public:

  virtual taBase* NewProgram(Program_Group* new_owner, ObjLibEl* lib_el);
  // create a new program in new_owner of based on given lib element (return value could be a Program or a Program_Group);  new_owner is group where program will be created
  virtual taBase* NewProgramFmName(Program_Group* new_owner, const String& prog_nm);
  // create a new program (lookup by name) (return value could be a Program or a Program_Group, or NULL if not found); new_owner is group where program will be created -- used by web interface to auto-load a new program

  virtual bool  UpdateProgram(Program* prog, ObjLibEl* lib_el);
  // #CAT_ObjLib load into given program; true if loaded, false if not
  virtual bool  DiffProgram(Program* prog, ObjLibEl* lib_el);
  // #CAT_ObjLib diff compare program with program library version
  virtual bool  UpdateProgramFmName(Program* prog, const String& prog_nm);
  // #CAT_ObjLib update given program from program element lookup by name -- used by web interface to auto-update a program
  virtual bool  UpdateProgramGroup(Program_Group* prog_gp, ObjLibEl* lib_el);
  // #CAT_ObjLib load into given program; true if loaded, false if not

  void  SetWikiInfoToObj(taBase* obj, const String& wiki_name) override;
  void  GetWikiInfoFromObj
    (taBase* obj, String*& tags, String*& desc, taProjVersion*& version, String*& author,
     String*& email, String*& pub_cite) override;
  
  
  TA_BASEFUNS_NOCOPY(ProgLib);
private:
  void  Initialize();
  void  Destroy()  { }
};

#endif // ProgLib_h
