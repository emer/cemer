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
class Program; //

taTypeDef_Of(ProgLib);

class TA_API ProgLib: public ProgLibEl_List {
  // #VIRT_BASE program library virtual base class
INHERITED(ProgLibEl_List)
public:
  enum ProgLibs {               // program library locations: must be sync'd with Program
    USER_LIB,                   // user's personal library -- located in app user dir (~/lib/emergent or ~/Library/Emergent prog_lib)
    SYSTEM_LIB,                 // local system library, installed with software, in /usr/share/Emergent/prog_lib
    WEB_APP_LIB,                // web-based application-specific library (e.g., emergent)
    WEB_SCI_LIB,                // web-based scientifically oriented library (e.g., CCN)
    WEB_USER_LIB,               // web-based user's library (e.g., from lab wiki)
    SEARCH_LIBS,                // search through the libraries (for loading)
  };

  bool  init; // has list been initialized yet?

  virtual void  FindPrograms() {};
  // search paths to find all available programs in this library
  taBase* NewProgram(ProgLibEl* prog_type, Program_Group* new_owner);
  // #MENU #MENU_ON_Object #MENU_CONTEXT #NO_SAVE_ARG_VAL create a new program in new_owner of given type (return value could be a Program or a Program_Group);  new_owner is group where program will be created
  taBase* NewProgramFmName(const String& prog_nm, Program_Group* new_owner);
  // create a new program (lookup by name) (return value could be a Program or a Program_Group, or NULL if not found); new_owner is group where program will be created
  virtual bool SaveProgToProgLib(Program* prg, ProgLibs library)
  { return false; }
  // save a program to a program library -- defined for specific subtypes
  virtual bool SaveProgGrpToProgLib(Program_Group* prg_grp, ProgLibs library)
  { return false; }
  // save a program group to a program library -- defined for specific subtypes

  TA_SIMPLE_BASEFUNS(ProgLib);
protected:


private:
  void  Initialize();
  void  Destroy() { CutLinks(); }
};

taTypeDef_Of(ProgLib_List);

class TA_API ProgLib_List : public taList<ProgLib> {
  // ##NO_TOKENS ##NO_UPDATE_AFTER ##CHILDREN_INLINE ##CAT_Program list of program libraries
INHERITED(taList<ProgLib>)
public:
  TA_SIMPLE_BASEFUNS(ProgLib_List);
protected:

private:
  void  Initialize() { };
  void  Destroy() { Reset(); CutLinks(); }
};




#endif // ProgLib_h
