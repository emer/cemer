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

#ifndef ProgLib_h
#define ProgLib_h 1

// parent includes:
#include <ProgLibEl_List>

// member includes:

// declare all other types mentioned but not required to include:
class taBase; // 
class ProgLibEl; // 
class Program_Group; // 


TypeDef_Of(ProgLib);

class TA_API ProgLib: public ProgLibEl_List {
  // #INSTANCE #INLINE #CAT_Program the program library
INHERITED(ProgLibEl_List)
public:
  bool                  not_init; // list has not been initialized yet

  void  FindPrograms();         // search paths to find all available programs
  taBase* NewProgram(ProgLibEl* prog_type, Program_Group* new_owner);
  // #MENU #MENU_ON_Object #MENU_CONTEXT #NO_SAVE_ARG_VAL create a new program in new_owner of given type (return value could be a Program or a Program_Group);  new_owner is group where program will be created
  taBase* NewProgramFmName(const String& prog_nm, Program_Group* new_owner);
  // create a new program (lookup by name) (return value could be a Program or a Program_Group, or NULL if not found); new_owner is group where program will be created

  TA_SIMPLE_BASEFUNS(ProgLib);
protected:

private:
  void  Initialize();
  void  Destroy() { CutLinks(); }
};

#endif // ProgLib_h
