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

#ifndef Program_Group_h
#define Program_Group_h 1

// parent includes:
#include <taGroup>

// member includes:
#include <ProgLib>
#include <ProgramRef>

// declare all other types mentioned but not required to include:
class taBase; // 

taTypeDef_Of(Program_Group);

class TA_API Program_Group : public taGroup<Program> {
  // ##EXT_progp ##FILETYPE_ProgramGroup #CAT_Program ##TOKENS a collection of programs sharing common global variables and a control panel interface
INHERITED(taGroup<Program>)
public:
  
  static ProgLib prog_lib; // #TREE_HIDDEN #HIDDEN #NO_SAVE library of available programs
  
  String                tags;
  // #EDIT_DIALOG list of comma separated tags that indicate the basic function of this program -- should be listed in hierarchical order, with most important/general tags first, as this is how they will be sorted in the program library
  String                desc; // #EDIT_DIALOG description of what this program group does and when it should be used (used for searching in prog_lib -- be thorough!)
  
  bool                  debug_mode; // ProgEls can check this to conditionally turn on/off elements

  virtual void    BuildProgLib();
  // #CAT_IGNORE build the program library -- find all the programs -- called just-in-time when needed
  
  virtual taBase* AddFromProgLib(ObjLibEl* prog_lib_item);
  // #BUTTON #MENU_CONTEXT #FROM_LIST_prog_lib.library #NO_SAVE_ARG_VAL #PRE_CALL_BuildProgLib #CAT_PatchLib create a new program from a library of existing programs
  virtual taBase* AddFromProgLibByName(const String& prog_nm);
  // #CAT_PatchLib create a new program from a library of existing program types, looking up by name (NULL if name not found) -- used by web interface to auto-load a new program
   virtual void   UpdateFromProgLib(ObjLibEl* prog_lib_item);
  // #MENU #MENU_ON_Object #MENU_CONTEXT #FROM_LIST_prog_lib.library #ARG_VAL_FM_FUN #PRE_CALL_BuildProgLib #CAT_PatchLib (re)load the current program group from the selected program library element
  virtual void    BrowseProgLib(ProgLib::LibLocs location = ProgLib::WEB_APP_LIB);
  // #BUTTON #MENU_CONTEXT #CAT_ProgLib browse given program library location using web browser (or file browser for local files -- less useful) -- USER_LIB: user's personal library -- located in app user dir (~/lib/emergent or ~/Library/Emergent prog_lib), SYSTEM_LIB: local system library, installed with software, in /usr/share/Emergent/prog_lib, WEB_APP_LIB: web-based application-specific library (e.g., emergent, WEB_SCI_LIB: web-based scientifically oriented library (e.g., CCN), WEB_USER_LIB: web-based user's library (e.g., from lab wiki)

  virtual void  ToggleTrace();
  // #MENU #MENU_ON_Object #DYN1 toggle the TRACE flag to opposite of current state for all programs in the group: flag indicates whether to record a trace of program execution in the css console or not
  virtual void  ClearAllBreakpoints();
  // #MENU #MENU_ON_Object #DYN1 remove breakpoints for all programs in the group:
  virtual void  ToggleDebug();
  // #BUTTON #MENU_CONTEXT #DYN1 toggle the debug_mode, which determines whether any print statements with the debug flag set will print
  virtual void  SetGuiUpdt(bool objs_updt_gui = true);
  // #MENU #MENU_ON_Object #DYN1 set the OBJS_UPDT_GUI flag in all the programs in this group -- can be faster without this on but it also interferes with interactive updates and can lead to confusing behavior -- from version 8.1.0 the default is to have it ON, before it was off
  
  virtual void  RestorePanels();
  // if the panel was pinned when the program was saved redisplay it on project open
  virtual bool  RunStartupProgs();
  // run programs marked as STARTUP_RUN -- typically only done by system at startup -- returns true if any run

  virtual void  BrowserSelectFirstEl();
  // #EXPERT callback to select first element in the group


  virtual bool  BrowserEditTest();
  // #MENU #MENU_ON_Object #MENU_SEP_BEFORE test the parsing functions of all program elements in all programs within this group -- just compares output of BrowserEditString before and after running BrowserEditSet on BrowserEditString -- does the thing parse its own output string?

  void          SetProgsStale(); // set all progs in this group/subgroup to be dirty

  String        GetTypeDecoKey() const override { return "Program"; }
  String        GetStateDecoKey() const override;

  Variant       GetGuiArgVal(const String& fun_name, int arg_idx) override;
  
  virtual bool  InDebugMode();
  // test whether we are in debug mode or not -- looks at parent Program_Group's as well
  
  void  InitLinks() override;
  void  CutLinks() override;
  TA_BASEFUNS(Program_Group);
  
private:
  void  Copy_(const Program_Group& cp);
  void  Initialize();
  void  Destroy()               {Reset(); };
};

TA_SMART_PTRS(TA_API, Program_Group); // Program_GroupRef

#endif // Program_Group_h
