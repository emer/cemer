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

#include "ScriptBase.h"

ScriptBase::ScriptBase() {
  script_file = taFiler::New("Script", ".css");
  taRefN::Ref(script_file);
}

ScriptBase::~ScriptBase() {
  if (script_file) {
    taRefN::unRefDone(script_file);
    script_file = NULL;
  }
}

void ScriptBase::Copy_(const ScriptBase& cp) {
  if (script_file && cp.script_file)
    *script_file = *(cp.script_file);
  script_string = cp.script_string;
}

void ScriptBase::LoadScript(const String& file_nm) {
  SetScript(file_nm);

  CompileScript(true);
}

void ScriptBase::LoadScriptString(const String& string) {
  script_string = string;
  script_file->SetFname("");
  script_filename = "";
  script_compiled = false;
  
  CompileScript(true);
}

const String ScriptBase::scriptFilename() {
  return script_file->FileName();
}

AbstractScriptBase::ScriptSource ScriptBase::scriptSource() {
  if (!script_string.empty()) 
    return ScriptString;
  else  if (!script_file->FileName().empty())
    return ScriptFile;
  else return NoScript;
}

void ScriptBase::SetScript(const String& file_nm) {
  script_filename = file_nm;
  script_file->select_only = true;	// just selecting a file name here
  script_file->SetFileName(file_nm);
  script_string = _nilString; // either/or
  script_compiled = false;
}

void ScriptBase::UpdateScriptFile() {
  if(taMisc::is_loading) {
    if(script_filename.nonempty())
      script_file->SetFileName(script_filename);
    script_compiled = false;
  }
  else {
    script_filename = script_file->FileName();
  }
}
