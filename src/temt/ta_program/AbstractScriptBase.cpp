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

#include "AbstractScriptBase.h"
#include <css_machine.h>
#include <css_ta.h>
#include <css_qtconsole.h>

#include <taMisc>


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
    ExitShellScript(false);     // no update prompt
    delete script;
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
  if(taMisc::gui_active) {
    QcssConsole* con = QcssConsole::getInstance();
    con->displayPrompt(true);   // force
  }
}

void AbstractScriptBase::ExitShellScript(bool updt_prompt) {
  if(!script || !cssMisc::TopShell) return;
  cssMisc::TopShell->PopSrcProg(script);
  if(updt_prompt && taMisc::gui_active) {
    QcssConsole* con = QcssConsole::getInstance();
    con->displayPrompt(true);   // force
  }
}
