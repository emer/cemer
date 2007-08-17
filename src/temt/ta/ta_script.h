// Copyright, 1995-2007, Regents of the University of Colorado,
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
class ScriptBase_List; //


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
  
  virtual bool		HasScript() {return (scriptSource() != NoScript);}
  // returns true if we have some kind of script file or string to run
  virtual bool  	RunScript();
  // run the script (returns false for no scr)
  virtual bool		CompileScript(bool force = false);
  // #MENU #LABEL_Compile #MENU_ON_Actions #ARGC_0 compile script from source into internal runnable format;\n 'true' if compiled, 'false' if not or if deferred due to being in readline
  virtual void  	StopScript();
  // stops the running script
  virtual void		CmdShellScript();
  // #MENU #LABEL_CmdShell #MENU_ON_Actions set the css command shell to operate on this script, so that you can interact with (run, debug, etc) this script
  virtual void		ExitShellScript();
  // #MENU #LABEL_ExitShell #MENU_ON_Actions if this script is currently being operated on by the command shell (via CmdShell function), exit from that shell (shell returns to previous script it was operating on)

  virtual TypeDef* GetThisTypeDef() const = 0;
  // #IGNORE overload this function to get the typedef of 'this' object, which must be taBase
  virtual void*	GetThisPtr() = 0;
  // #IGNORE overload this function to get the 'this' pointer for object (must be taBase)

  AbstractScriptBase();
  virtual ~AbstractScriptBase();
  
protected:
  bool			DoCompileScript(); // called directly, or in WaitProc
  virtual void		InitScriptObj_impl(); // #IGNORE called to initialize script object prior to compiling, create 'script' and 'ths'; can be extended to add more to script etc. 
  virtual bool 		PreCompileScript_impl(); // #IGNORE called before compile: base impl clears existing script: MUST CALL THIS BEFORE CHANGING ANY VARIABLE SPACES (e.g.,hard_vars, prog_vars)
  virtual bool		CompileScript_impl(); // #IGNORE
  virtual void 		ScriptCompiled() {} // #IGNORE called when script is recompiled
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

#endif // script_base_h


