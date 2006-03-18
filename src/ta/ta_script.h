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
#include "ta_variant.h"
#include "ta_TA_type.h"

#ifdef TA_GUI
class taiAction;
#endif

// externals
class cssProgSpace;		// #IGNORE
class cssTA_Base;		// #IGNORE

// forwards
class ScriptBase_List;
class ScriptVar;

class TA_API ScriptVar: public taOBase { // ##NO_TOKENS #UAE_OWNER a script variable, accessible from the outer system, and inside the script
INHERITED(taOBase)
public:
  String		name; // name of the variable -- must be a legal css name
  Variant		value; // the actual variableuse_init_value
  
  const String		GenCss(); // css code (no terminator or newline)
  
  bool	SetName(const char* nm) {name = nm; return true;} // #IGNORE
  bool	SetName(const String& nm) {name = nm; return true;} // #IGNORE
  String GetName() const { return name; } // #IGNORE
  void 	SetDefaultName() {} // make it local to list, set by list
  void	UpdateAfterEdit();
  void	Copy_(const ScriptVar& cp);
  COPY_FUNS(ScriptVar, taOBase);
  TA_BASEFUNS(ScriptVar);
private:
  void	Initialize();
  void	Destroy();
};

class TA_API ScriptVar_List : public taList<ScriptVar> {
  // ##NO_TOKENS ##NO_UPDATE_AFTER #UAE_OWNER list of script variables
INHERITED(taList<ScriptVar>)
public:
  enum VarContext {
    VC_ProgVars,  // #LABEL_ProgramVariables program variables
    VC_FuncArgs  //  #LABEL_FunctionArguments function arguments
  };
  
  VarContext	    	var_context; // #DEF_VC_ProgVars #HIDDEN context of vars, set by owner
  
  virtual const String 	GenCss(int indent_level) const; // generate css script code for the context
  
  TA_BASEFUNS(ScriptVar_List);
  
protected:
  override void	El_SetIndex_(void*, int);
  
private:
  void	Initialize();
  void	Destroy() {Reset();}
};


class TA_API AbstractScriptBase {
  // #VIRT_BASE #NO_INSTANCE abstract class for adding a script to other objects
public:
  static ScriptBase_List recompile_scripts; // #HIDDEN list of scripts to be recompiled in wait proc
  static bool		 Wait_RecompileScripts(); // wait process for recompiling
  
  enum ScriptSource {
    NoScript,		// there is no script
    ScriptString,	// the script is in a string
    ScriptFile 		// the script will be provided by a file
  };
  
  cssProgSpace* 	script;	// #HIDDEN #NO_SAVE the script, created on first compile
  cssTA_Base* 		ths; // #HIDDEN #NO_SAVE #READ_ONLY the taBase outer this, as represented in script obj
  bool		    	script_compiled; // #SHOW #NO_SAVE #READ_ONLY true when compiled

  virtual ScriptSource	scriptSource() {return NoScript;} // override this
  virtual const String	scriptFilename() {return _nilString;} // override this
  virtual const String	scriptString() {return _nilString;} // override this
  
  virtual bool	HasScript() {return (scriptSource() != NoScript);}
  // returns true if we have some kind of script file or string to run
  virtual bool  RunScript();
  // run the script (returns false for no scr)
  virtual bool	CompileScript(bool force = false);
  // #MENU #LABEL_Compile #MENU_ON_Actions #ARGC_0 compile script from source into internal runnable format
  virtual void	InteractScript();
  // #MENU #LABEL_Interact change to this shell in script (terminal) window to interact, debug etc script

  virtual TypeDef* GetThisTypeDef() = 0;
  // #IGNORE overload this function to get the typedef of 'this' object, which must be taBase
  virtual void*	GetThisPtr() = 0;
  // #IGNORE overload this function to get the 'this' pointer for object (must be taBase)

  AbstractScriptBase();
  virtual ~AbstractScriptBase();
  
protected:
  virtual void		InitScriptObj_impl(); // #IGNORE called to initialize script object prior to compiling, create 'script' and 'ths'; can be extended to add more to script etc. 
  virtual bool		CompileScript_impl(); // #IGNORE
  virtual void 		ScriptCompiled() {} // #IGNORE called when script is recompiled;
private:
  AbstractScriptBase(const AbstractScriptBase& cp); // #IGNORE not allowed
  AbstractScriptBase& operator=(const AbstractScriptBase& cp); // #IGNORE not allowed
};

class TA_API ScriptBase_List : public taPtrList<AbstractScriptBase> {
  // ##NO_TOKENS ##NO_UPDATE_AFTER list of script base objects (doesn't own anything)
public:
  ~ScriptBase_List()	{ Reset(); }
};

class TA_API ScriptBase: public AbstractScriptBase {
  // #VIRT_BASE #NO_INSTANCE class for adding a script to other objects
public:
  taFiler*	script_file;		// file to use for the script
  String	script_string;
  
  override ScriptSource	scriptSource();
  override const String	scriptFilename();
  override const String	scriptString() {return script_string;}

  virtual void	LoadScript(const String& file_nm);
  // load script from script file and compile it
  virtual void	LoadScriptString(const String& string);
  // load script from script string and compile it
  virtual void 	SetScript(const String& file_nm);
  // set the script file name (also clears script string)
  
  void		Copy_(const ScriptBase& cp);
  ScriptBase();
  ~ScriptBase();
};

class TA_API Script : public taNBase, public ScriptBase {
  // ##EXT_scr an object for maintaining and running arbitrary scripts
public:

  // script code to be run, instead of loading from file
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
  void	InitLinks();
  void	CutLinks();
  void	Copy_(const Script& cp);
  COPY_FUNS(Script, taNBase);
  TA_BASEFUNS(Script);
private:
  void	Initialize();
  void	Destroy();
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


