// Copyright, 1995-2013, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of Emergent
//
//   Emergent is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   Emergent is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.

#include "ScriptPrjnSpec.h"
#include <Network>
#include <taFiler>

TA_BASEFUNS_CTORS_DEFN(ScriptPrjnSpec);


void ScriptPrjnSpec::Initialize() {
  prjn = NULL;
}

void ScriptPrjnSpec::Destroy() {
  prjn = NULL;
}

void ScriptPrjnSpec::InitLinks() {
  ProjectionSpec::InitLinks();
  taBase::Own(s_args, this);
  if(script_file->fname().empty())      // initialize only on startup up, not transfer
    SetScript("");
}

void ScriptPrjnSpec::Copy_(const ScriptPrjnSpec& cp) {
  s_args = cp.s_args;
  *script_file = *(cp.script_file);
  script_string = cp.script_string;
  script_filename = cp.script_filename;
}

void ScriptPrjnSpec::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);

  //make sure most recent compile succeeded
  CheckError(!script_compiled, quiet, rval,
    "The script did not compile -- please recompile and check console for errors");
}

void ScriptPrjnSpec::Connect_impl(Projection* prj) {
  prjn = prj;                   // set the arg for the script
  RunScript();
  prjn = NULL;
}

void ScriptPrjnSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  UpdateScriptFile();           // deals with all the script file management issues
  CompileScript(true); // force
/*v3 nuke  if(!script_file->fname().empty()) {
    //note: fname() is name only
    name = script_file->fname().before(".css");
    int i;
    for(i=0;i<s_args.size;i++)
      name += String("_") + s_args[i];
  }*/
}

void ScriptPrjnSpec::Compile() {
  UpdateScriptFile();           // deals with all the script file management issues
  CompileScript(true);          // force recompile!
}

