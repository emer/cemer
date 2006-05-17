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


// ta_script.cc

#include "ta_script.h"

#include "ta_css.h"
#include "ta_platform.h"
#include "css_basic_types.h"

#ifdef TA_GUI
  #include "ta_qt.h"
#include "ta_qtdata.h"
//#include "css_qt.h"
//#include "ta_qtgroup.h"
#endif



//////////////////////////
//   AbstractScriptBase	//
//////////////////////////

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

//////////////////////////
//   AbstractScriptBase	//
//////////////////////////

//TODO: should always create the filer on startup, because inherited classes may not check if
// exists (in Iv, it was a value member)
AbstractScriptBase::AbstractScriptBase() {
  script=NULL;
  script_compiled = false;
  ths = NULL;
}

AbstractScriptBase::~AbstractScriptBase() {
  ths = NULL;
  if (script) {
    if (script->DeleteOk())
      delete script;
    else
      script->DeferredDelete();
    script = NULL;
  }
}

bool AbstractScriptBase::CompileScript(bool force) {
  if (script_compiled && !force) return true;
  if (!HasScript()) {
    taMisc::Error("** Cannot Compile Script: No script file or string specified");
    return false;
  }
  InitScriptObj_impl();

  if (script->in_readline) {
    Script::recompile_scripts.AddUnique(this);
    script->ExitShell();
    return false;
  }
  
  return DoCompileScript();
}

bool AbstractScriptBase::CompileScript_impl() {
  bool rval = false;
  script->ClearAll();
  switch (scriptSource()) {
  case NoScript: return false; //nothing to do
  case ScriptString:
    rval = script->CompileCode(scriptString());
    break;
  case ScriptFile:
    rval = script->Compile(scriptFilename());
    break;
  default: break;// shouldn't happen
  }
  if (rval) {
    script_compiled = true;
    ScriptCompiled();
  }
  return rval;
}

bool AbstractScriptBase::DoCompileScript() {
  PreCompileScript_impl();
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
  if (script)
    script->Stop();
}


///////////////////////////
//  ScriptBase		//
//////////////////////////

ScriptBase::ScriptBase() {
  script_file = taFiler_CreateInstance(".","*.css*",false);
  script_file->compress = false;	// don't compress
  script_file->mode = taFiler::NO_AUTO;
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

  CompileScript();
}

void ScriptBase::LoadScriptString(const String& string) {
  script_string = string;
  script_file->fname = "";
  script_compiled = false;
  
  CompileScript();
}

const String ScriptBase::scriptFilename() {
  return script_file->fname;
}

AbstractScriptBase::ScriptSource ScriptBase::scriptSource() {
  if (!script_file->fname.empty())
    return ScriptFile;
  else if (!script_string.empty()) 
    return ScriptString;
  else return NoScript;
}

void ScriptBase::SetScript(const String& file_nm) {
  script_file->select_only = true;	// just selecting a file name here
  script_file->fname = file_nm;
  script_string = _nilString; // either/or
  script_compiled = false;
//  script_file->UpdateGF();
}


///////////////////////////
// 	Script		//
//////////////////////////

void Script::Initialize() {
  recording = false;
  auto_run = false;
}

void Script::Destroy() {
  CutLinks();
}

void Script::InitLinks() {
  taNBase::InitLinks();
//filer not a taBase  taBase::Own(script_file, this);
  taBase::Own(s_args, this);
  if(script_file->fname.empty())	// initialize only on startup up, not transfer
    SetScript("");
}

void Script::CutLinks() {
  StopRecording();
  taNBase::CutLinks();
}

void Script::Copy_(const Script& cp) {
  ScriptBase::Copy_(cp);
  auto_run = cp.auto_run;
  s_args = cp.s_args;
  script_compiled = false; // redo compile
}

void Script::UpdateAfterEdit() {
  // have to assume user changed something
  script_compiled = false;
  // if user supplied a string, then nuke the filename
  if (!script_string.empty()) {
    script_file->file_selected = false;
    script_file->fname = _nilString;
  }
  // if based on a script, we automatically update our name
  else if (!script_file->fname.empty()) {
    name = script_file->fname;
    if(name.contains(".css"))
      name = name.before(".css");
    name = taPlatform::getFileName(name); // strip path
  }
  taNBase::UpdateAfterEdit();
  if (HasScript())
    CompileScript();
}

bool Script::Run() {
  return ScriptBase::RunScript();
}

void Script::Record(const char* file_nm) {
#ifdef DMEM_COMPILE
  if ((taMisc::dmem_nprocs > 1) && (taMisc::record_script != NULL)) {
    taMisc::Error("Record: Cannot record a script under DMEM with the gui -- record script is used to communicate between processes");
    return;
  }
#endif
  Script_MGroup* mg = GET_MY_OWNER(Script_MGroup);
  if(mg != NULL)
    mg->StopRecording();
  if (file_nm != NULL)
    SetScript(file_nm);
  if (script_file->fname.empty()) {
    taMisc::Error("Record: No script file selected.\n Open a Script file and press Apply");
    return;
  }

  ostream* strm = script_file->open_append();
  if((strm == NULL) || strm->bad()) {
    taMisc::Error("Record: Script file could not be opened:", script_file->fname);
    script_file->Close();
    return;
  }

#ifdef TA_GUI
  taMisc::StartRecording(strm);
#endif
  recording = true;
}

void Script::StopRecording() {
  if(!recording)
    return;
#ifdef TA_GUI
  taMisc::StopRecording();
#endif
  script_file->Close();
  recording = false;
}

void Script::Clear() {
  if(recording) {
    script_file->Close();
    taMisc::record_script = script_file->open_write();
    return;
  }
  if(script_file->fname.empty()) {
    taMisc::Error("Clear: No Script File Selected\n Open a Script file and press Apply");
    return;
  }

  ostream* strm = script_file->open_write();
  if((strm == NULL) || strm->bad()) {
    taMisc::Error("Clear: Script file could not be opened:", script_file->fname);
  }
  script_file->Close();
}

void Script::Compile() {
  CompileScript();
}

void Script::AutoRun() {
  if(!auto_run)
    return;
  Run();
}

void Script::ScriptAllWinPos() {
  //TODO: call some central thingy to script the win pos
/*TODO:fixup  TAPtr scp = GetScopeObj();
  if ((scp == NULL) || !scp->InheritsFrom(TA_ViewBase)) {
    taMisc::Error("ScriptAllWinPos:: Could not locate appropriate scoping object");
    return;
  }
  ((ViewBase*)scp)->ScriptAllWinPos(); */
}



//////////////////////////
// 	Script_MGroup	//
//////////////////////////

void Script_MGroup::Initialize() {
  SetBaseType(&TA_Script);
//  SetAdapter(new Script_MGroupAdapter(this));
}

void Script_MGroup::StopRecording() {
  taLeafItr i;
  Script* sb;
  FOR_ITR_EL(Script, sb, this->, i)
    sb->StopRecording();
}

void Script_MGroup::AutoRun() {
  taLeafItr i;
  Script* sb;
  FOR_ITR_EL(Script, sb, this->, i)
    sb->AutoRun();
}

#ifdef TA_GUI
void Script_MGroup::Run_mc(taiAction* sel) {
//TODO  if(win_owner == NULL) return;
//TODO Qt4: verify that usr_data gets set to a taBase obj
  if((sel != NULL)) {
    Script* itm = (Script*)sel->usr_data.toBase(); // NULL if not a Base type, or NULL
    if (itm != NULL) {
      itm->Run();
      if(taMisc::record_script != NULL) {
        *taMisc::record_script << itm->GetPath() << "->Run();" << endl;
      }
    }
  }
}
#endif
/*
void Script_MGroup::GenMenu_impl(taiMenu* menu) {
  MenuGroup_impl::GenMenu_impl(menu);

  // don't put in the groups or the stats processes within menu
  itm_list->SetFlag(taiData::flgNoInGroup | taiData::flgNoList, true);

  menu->AddSep();

  taiMenu* sub;

  mc.member = SLOT(Run_mc(taiAction*));
  sub = menu->AddSubMenu("Run");
  itm_list->GetMenu(sub, &mc);

}
*/
