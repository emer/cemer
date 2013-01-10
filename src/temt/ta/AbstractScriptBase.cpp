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

#include "AbstractScriptBase.h"

ScriptBase_List AbstractScriptBase::recompile_scripts;

bool AbstractScriptBase::Wait_RecompileScripts() {
  if (recompile_scripts.size == 0)
    return false;
  int i;
  for (i=0; i<recompile_scripts.size; ++i)
    recompile_scripts[i]->DoCompileScript();
  recompile_scripts.Reset();
  return true;
}

AbstractScriptBase::AbstractScriptBase() {
  script=NULL;
  script_compiled = false;
  ths = NULL;
}

AbstractScriptBase::~AbstractScriptBase() {
  ths = NULL;
  if (script) {
    ExitShellScript();
    delete script;		// todo: just added 2/16/07: might cause crashing!
    script = NULL;
  }
}

bool AbstractScriptBase::CompileScript(bool force) {
  if (script_compiled && (bool)script && !force) return true;
  if (!HasScript()) {
    taMisc::Error("** Cannot Compile Script: No script file or string specified");
    return false;
  }
  InitScriptObj_impl();
  return DoCompileScript();
}

bool AbstractScriptBase::PreCompileScript_impl() {
  // new impls must call this first!!
  script->ClearAll();
  return true;
}

bool AbstractScriptBase::CompileScript_impl() {
  bool rval = false;
  script->name = ((taBase*)GetThisPtr())->GetName();
  switch (scriptSource()) {
  case NoScript: break; //nothing to do
  case ScriptString:
    rval = script->CompileCode(scriptString());
    break;
  case ScriptFile:
    rval = script->Compile(scriptFilename());
    break;
  default: break;// shouldn't happen
  }
  script_compiled = rval;
  if (rval) {
    ScriptCompiled();
  }
  return rval;
}

bool AbstractScriptBase::DoCompileScript() {
  if(!PreCompileScript_impl()) return false;
  return CompileScript_impl();
}

void AbstractScriptBase::InitScriptObj_impl() {
  if (script == NULL) {
    script = new cssProgSpace();
    ths = new cssTA_Base(GetThisPtr(), 1, GetThisTypeDef(), "this");
    ths->InstallThis(script);
  }
}

bool AbstractScriptBase::RunScript() {
  if (!CompileScript()) return false;
  script->Run();		// just run the script
  return true;
}

void AbstractScriptBase::StopScript() {
  if(script)
    script->Stop();
}

void AbstractScriptBase::CmdShellScript() {
  if(!script || !cssMisc::TopShell) return;
  cssMisc::TopShell->PushSrcProg(script);
}

void AbstractScriptBase::ExitShellScript() {
  if(!script || !cssMisc::TopShell) return;
  cssMisc::TopShell->PopSrcProg(script);
}
