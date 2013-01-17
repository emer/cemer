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

#ifndef AbstractScriptBase_h
#define AbstractScriptBase_h 1

// parent includes:
#include "ta_def.h"

// member includes:
#include <ScriptBase_List>

// declare all other types mentioned but not required to include:
class cssProgSpace; // 
class cssTA_Base; // 


class TA_API AbstractScriptBase {
  // #VIRT_BASE #NO_INSTANCE abstract class for adding a script to other objects
public:
#ifndef __MAKETA__
  static ScriptBase_List recompile_scripts; // #IGNORE list of scripts to be recompiled in wait proc
#endif

  static bool            Wait_RecompileScripts(); // wait process for recompiling

  enum ScriptSource {
    NoScript,           // there is no script
    ScriptString,       // the script is in a string
    ScriptFile          // the script will be provided by a file
  };

  cssProgSpace*         script; // #IGNORE the script, created on first compile
  cssTA_Base*           ths; // #IGNORE the taBase outer this, as represented in script obj
  bool                  script_compiled; // #SHOW #NO_SAVE #READ_ONLY true when compiled

  virtual ScriptSource  scriptSource() {return NoScript;} // override this
  virtual const String  scriptFilename() {return _nilString;} // override this
  virtual const String  scriptString() {return _nilString;} // override this

  virtual bool          HasScript() {return (scriptSource() != NoScript);}
  // returns true if we have some kind of script file or string to run
  virtual bool          RunScript();
  // run the script (returns false for no scr)
  virtual bool          CompileScript(bool force = false);
  // #MENU #LABEL_Compile #MENU_ON_Actions #ARGC_0 compile script from source into internal runnable format;\n 'true' if compiled, 'false' if not or if deferred due to being in readline
  virtual void          StopScript();
  // stops the running script
  virtual void          CmdShellScript();
  // #MENU #LABEL_CmdShell #MENU_ON_Actions set the css command shell to operate on this script, so that you can interact with (run, debug, etc) this script
  virtual void          ExitShellScript();
  // #MENU #LABEL_ExitShell #MENU_ON_Actions if this script is currently being operated on by the command shell (via CmdShell function), exit from that shell (shell returns to previous script it was operating on)

  virtual TypeDef* GetThisTypeDef() const = 0;
  // #IGNORE overload this function to get the typedef of 'this' object, which must be taBase
  virtual void* GetThisPtr() = 0;
  // #IGNORE overload this function to get the 'this' pointer for object (must be taBase)

  AbstractScriptBase();
  virtual ~AbstractScriptBase();

protected:
  bool                  DoCompileScript(); // called directly, or in WaitProc
  virtual void          InitScriptObj_impl(); // #IGNORE called to initialize script object prior to compiling, create 'script' and 'ths'; can be extended to add more to script etc.
  virtual bool          PreCompileScript_impl(); // #IGNORE called before compile: base impl clears existing script: MUST CALL THIS BEFORE CHANGING ANY VARIABLE SPACES (e.g.,hard_vars, prog_vars)
  virtual bool          CompileScript_impl(); // #IGNORE
  virtual void          ScriptCompiled() {} // #IGNORE called when script is recompiled
private:
  AbstractScriptBase(const AbstractScriptBase& cp); // #IGNORE not allowed
  AbstractScriptBase& operator=(const AbstractScriptBase& cp); // #IGNORE not allowed
};

#endif // AbstractScriptBase_h
