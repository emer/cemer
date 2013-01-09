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

#ifndef Program_Group_h
#define Program_Group_h 1

// parent includes:
#include <Program>
#include <taGroup>

// member includes:
#include <Program>
#include <ProgLib>

// declare all other types mentioned but not required to include:
class taBase; // 
class ProgLibEl; // 


class TA_API Program_Group : public taGroup<Program> {
  // ##EXT_progp ##FILETYPE_ProgramGroup #CAT_Program ##EXPAND_DEF_2 a collection of programs sharing common global variables and a control panel interface
INHERITED(taGroup<Program>)
public:
  String                tags;
  // #EDIT_DIALOG list of comma separated tags that indicate the basic function of this program -- should be listed in hierarchical order, with most important/general tags first, as this is how they will be sorted in the program library
  String                desc; // #EDIT_DIALOG description of what this program group does and when it should be used (used for searching in prog_lib -- be thorough!)
  ProgramRef            step_prog;
  // #READ_ONLY #NO_SAVE #OBSOLETE this is just here for loading prior versions and is no longer used in any fashion

  static ProgLib        prog_lib; // #HIDDEN_TREE library of available programs

  taBase* NewFromLib(ProgLibEl* prog_type);
  // #BUTTON #MENU_CONTEXT #FROM_GROUP_prog_lib #NO_SAVE_ARG_VAL #CAT_Program create a new program from a library of existing program types
  taBase* NewFromLibByName(const String& prog_nm);
  // #CAT_Program create a new program from a library of existing program types, looking up by name (NULL if name not found)

  void          SaveToProgLib(Program::ProgLibs library = Program::USER_LIB);
  // #MENU #MENU_ON_Object #MENU_CONTEXT #CAT_Program save the program group to given program library -- file name = object name -- be sure to add good desc comments!!
  virtual void  LoadFromProgLib(ProgLibEl* prog_type);
  // #MENU #MENU_ON_Object #MENU_CONTEXT #FROM_GROUP_prog_lib #ARG_VAL_FM_FUN #CAT_Program (re)load the program from the program library element of given type

  virtual void  ToggleTrace();
  // #MENU #MENU_ON_Object #DYN1 toggle the TRACE flag to opposite of current state for all programs in the group: flag indicates whether to record a trace of program execution in the css console or not

  virtual bool  RunStartupProgs();
  // run programs marked as STARTUP_RUN -- typically only done by system at startup -- returns true if any run

  void          SetProgsStale(); // set all progs in this group/subgroup to be dirty

  override String       GetTypeDecoKey() const { return "Program"; }

  override Variant      GetGuiArgVal(const String& fun_name, int arg_idx);

  void  InitLinks();
  void  CutLinks();
  TA_BASEFUNS(Program_Group);

private:
  void  Copy_(const Program_Group& cp);
  void  Initialize();
  void  Destroy()               {Reset(); };
};

TA_SMART_PTRS(Program_Group); // Program_GroupRef

#endif // Program_Group_h
