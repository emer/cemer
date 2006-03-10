// Copyright, 1995-2005, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of TA/CSS
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


// ta_script.h: basic types for script-managing objects

#ifndef TA_SCRIPT_H
#define TA_SCRIPT_H

#include "ta_base.h"
#include "ta_group.h"
#include "ta_filer.h"
#include "ta_TA_type.h"

#ifdef TA_GUI
class taiAction;
#endif

class cssProgSpace;		// #IGNORE

class TA_API ScriptBase {
  // class for adding a script to other objects
public:
  cssProgSpace* script;			// #HIDDEN the script, if defined
  taFiler*	script_file;		// file to use for the script
  String	script_string;
  // #CONDEDIT_OFF_ssro_true script code to be run, instead of loading from file
  bool		ssro;
  // #NO_SHOW #NO_SAVE for subclasses that manage script_string themselves

  virtual bool	HasScript();
  // returns true if we have some kind of script file or string to run
  virtual bool  RunScript();
  // run the script (returns false for no scr)
  virtual void 	SetScript(const char* file_nm);
  // set the script file (e.g. from the script)
  virtual void	LoadScript(const char* file_nm = NULL);
  // #MENU #LABEL_Compile #MENU_ON_Actions #ARGC_0 compile script from script file into internal runnable format
  virtual void	LoadScriptString(const char* string = NULL);
  // load and recompile the script string
  virtual void	InteractScript();
  // #MENU #LABEL_Interact change to this shell in script (terminal) window to interact, debug etc script

  virtual void	UpdateReCompile();
  // #IGNORE recompile if reselected by user
  virtual TypeDef* GetThisTypeDef();
  // #IGNORE overload this function to get the typedef of 'this' object, which must be taBase
  virtual void*	GetThisPtr()		{ return (void*)this; }
  // #IGNORE overload this function to get the 'this' pointer for object (must be taBase)

  ScriptBase();
  virtual ~ScriptBase();
  
protected:
  virtual void	LoadScript_impl(); // #IGNORE
  virtual void ScriptCompiled() {} // #IGNORE called when script is recompiled;
private:
  ScriptBase(const ScriptBase& cp); // #IGNORE not allowed
  ScriptBase& operator=(const ScriptBase& cp); // #IGNORE not allowed
};

class TA_API ScriptBase_List : public taPtrList<ScriptBase> {
  // ##NO_TOKENS ##NO_UPDATE_AFTER list of script base objects (doesn't own anything)
public:
  ~ScriptBase_List()	{ Reset(); }
};

class TA_API Script : public taNBase, public ScriptBase {
  // ##EXT_scr an object for maintaining and running arbitrary scripts
public:
  static ScriptBase_List recompile_scripts; // #HIDDEN list of scripts to be recompiled in wait proc
  static bool		 Wait_RecompileScripts(); // wait process for recompiling

  bool		recording;	// #READ_ONLY #NO_SAVE currently recording?
  bool		auto_run;	// run automatically at startup?
  SArg_Array	s_args;		// string-valued arguments to pass to script

  virtual bool  Run();
  // #BUTTON #GHOST_OFF_recording run the script (returns false for no scr)
  virtual void	Record(const char* file_nm = NULL);
  // #BUTTON #GHOST_OFF_recording #ARGC_0 #NO_SCRIPT record script code for interface actions
  virtual void	StopRecording();
  // #BUTTON #LABEL_StopRec #GHOST_ON_recording stop recording script code
  virtual void	Interact();
  // #BUTTON #GHOST_OFF_recording change to this shell in script (terminal) window to interact, debug etc script
  virtual void	Clear();
  // #BUTTON #CONFIRM clear script file
  virtual void	Compile();
  // #BUTTON #GHOST_OFF_recording compile script from script file into internal runnable format

  virtual void	ScriptAllWinPos();
  // #MENU #MENU_ON_Actions #NO_SCRIPT record script code to set window positions, iconified

  virtual void	AutoRun();
  // run this script if auto_run is set

  TypeDef*	GetThisTypeDef()	{ return GetTypeDef(); }
  void*		GetThisPtr()		{ return (void*)this; }

  void	UpdateAfterEdit();
  void	Initialize();
  void	Destroy()	{ CutLinks(); }
  void	InitLinks();
  void	CutLinks();
  void	Copy_(const Script& cp);
  COPY_FUNS(Script, taNBase);
  TA_BASEFUNS(Script);
};


// note: Script_MGroup name is for compatiblity with v3.2 files
class TA_API Script_MGroup : public taGroup<Script> {
public:
  virtual void	StopRecording();
  virtual void	AutoRun();

  void	Initialize();
  void 	Destroy()		{ };
  TA_BASEFUNS(Script_MGroup);

public:
  // more callbacks
#ifdef TA_GUI
  virtual void		Run_mc(taiAction* sel);
#endif

//protected:
//  override void 	GenMenu_impl(taiMenu* menu);		// add a 'run' menu..
};

/*
class Script_MGroupAdapter : public MenuGroup_implAdapter {
  Q_OBJECT
public:
  Script_MGroupAdapter(Script_MGroup* owner_): MenuGroup_implAdapter(owner_) {}

public slots:
  // more callbacks
  void	Run_mc(taiAction* sel)  {if(owner) ((Script_MGroup*)owner)->Run_mc(sel);}
};*/

#endif // script_base_h


