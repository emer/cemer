/* -*- C++ -*- */
/*=============================================================================
//									      //
// This file is part of the TypeAccess/C-Super-Script software package.	      //
//									      //
// Copyright (C) 1995 Randall C. O'Reilly, Chadley K. Dawson, 		      //
//		      James L. McClelland, and Carnegie Mellon University     //
//     									      //
// Permission to use, copy, modify, and distribute this software and its      //
// documentation for any purpose is hereby granted without fee, provided that //
// the above copyright notice and this permission notice appear in all copies //
// of the software and related documentation.                                 //
// 									      //
// Note that the PDP++ software package, which contains this package, has a   //
// more restrictive copyright, which applies only to the PDP++-specific       //
// portions of the software, which are labeled as such.			      //
//									      //
// Note that the taString class, which is derived from the GNU String class,  //
// is Copyright (C) 1988 Free Software Foundation, written by Doug Lea, and   //
// is covered by the GNU General Public License, see ta_string.h.             //
// The iv_graphic library and some iv_misc classes were derived from the      //
// InterViews morpher example and other InterViews code, which is             //
// Copyright (C) 1987, 1988, 1989, 1990, 1991 Stanford University             //
// Copyright (C) 1991 Silicon Graphics, Inc.				      //
//									      //
// THE SOFTWARE IS PROVIDED "AS-IS" AND WITHOUT WARRANTY OF ANY KIND,         //
// EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY 	      //
// WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.  	      //
// 									      //
// IN NO EVENT SHALL CARNEGIE MELLON UNIVERSITY BE LIABLE FOR ANY SPECIAL,    //
// INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY KIND, OR ANY DAMAGES  //
// WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER OR NOT     //
// ADVISED OF THE POSSIBILITY OF DAMAGE, AND ON ANY THEORY OF LIABILITY,      //
// ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS        //
// SOFTWARE. 								      //
==============================================================================*/

// ta_script.cc

#include "ta_script.h"

#include "ta_css.h"
#ifdef TA_GUI
  #include "ta_qt.h"
#include "ta_qtdata.h"
//#include "css_qt.h"
//#include "ta_qtgroup.h"
#endif


ScriptBase_List Script::recompile_scripts;

bool Script::Wait_RecompileScripts() {
  if(recompile_scripts.size == 0)
    return false;
  int i;
  for(i=0; i<recompile_scripts.size; i++)
    recompile_scripts[i]->LoadScript();
  recompile_scripts.Reset();
  return true;
}

//////////////////////////
// 	ScriptBase	//
//////////////////////////

//TODO: should always create the filer on startup, because inherited classes may not check if
// exists (in Iv, it was a value member)
ScriptBase::ScriptBase() {
  script=NULL;
  script_file = taFiler_CreateInstance(".","*.css*",false);
  script_file->compress = false;	// don't compress
  script_file->mode = taFiler::NO_AUTO;
  taRefN::Ref(script_file);
}

ScriptBase::~ScriptBase() {
  if(script != NULL) {
    if(script->DeleteOk())
      delete script;
    else
      script->DeferredDelete();
  }
  script = NULL;
  if (script_file != NULL) {
      taRefN::unRefDone(script_file);
      script_file = NULL;
  }
}

TypeDef* ScriptBase::GetThisTypeDef() {
  return &TA_Script;
}

bool ScriptBase::HasScript() {
  if(script_file->fname.empty() && script_string.empty())
    return false;
  return true;
}

void ScriptBase::SetScript(const char* nm) {
  script_file->select_only = true;	// just selecting a file name here
  script_file->fname = nm;
//  script_file->UpdateGF();
}

void ScriptBase::LoadScript(const char* nm) {
  if(nm != NULL)
    SetScript(nm);

  if(!HasScript()) {
    taMisc::Error("Cannot Load Script: No script file or string specified");
    return;
  }

  if(script == NULL) {
    script = new cssProgSpace();
    InstallThis();
  }

  if(script->in_readline) {
    Script::recompile_scripts.Add(this);
    script->ExitShell();
    return;
  }
  script->ClearAll();
  if(script_file->fname.empty())
    script->CompileCode(script_string);	// compile the string itself
  else
    script->Compile(script_file->fname);
}

void ScriptBase::LoadScriptString(const char* string) {
  if(string != NULL)
    script_string = string;
  script_file->fname = "";
  LoadScript();
}

void ScriptBase::InstallThis() {
  cssTA_Base* ths = new cssTA_Base(GetThisPtr(), 1, GetThisTypeDef(), "this");
  ths->InstallThis(script);
}

bool ScriptBase::RunScript() {
  if (script == NULL)   return false;
  script->Run();		// just run the script
  return true;
}

void ScriptBase::InteractScript() {
  if(script == NULL)   return;
  cssMisc::next_shell = script;
}

// todo: replace this with a time-stamp for recompiling
// maybe this should be done at a lower level within css itself

void ScriptBase::UpdateReCompile() {
  if(!HasScript())
    return;

  if((script == NULL) || script_file->file_selected || !script_string.empty()) {
    script_file->file_selected = false;
    LoadScript();
  }
}

///////////////////////////
// 	Script		//
//////////////////////////

void Script::Initialize() {
  recording = false;
  auto_run = false;
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
  auto_run = cp.auto_run;
  s_args = cp.s_args;
  script_file = cp.script_file;
  script_string = cp.script_string;
}

void Script::UpdateAfterEdit() {
  taNBase::UpdateAfterEdit();
  UpdateReCompile();
  if(!script_file->fname.empty()) {
    name = script_file->fname;
    if(name.contains(".css"))
      name = name.before(".css");
    if(name.contains('/'))
      name = name.after('/', -1);
  }
}

void Script::InstallThis() {
  ScriptBase::InstallThis();
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
  if(file_nm != NULL)
    SetScript(file_nm);
  if(script_file->fname.empty()) {
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

void Script::Interact() {
  if(script == NULL)   return;
  cssMisc::next_shell = script;
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
  LoadScript();
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
void Script_MGroup::Run_mc(taiMenuEl* sel) {
//TODO  if(win_owner == NULL) return;
  if((sel != NULL) && (sel->usr_data != NULL)) {
    Script* itm = (Script*)sel->usr_data;
    itm->Run();
    if(taMisc::record_script != NULL) {
      *taMisc::record_script << itm->GetPath() << "->Run();" << endl;
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

  mc.member = SLOT(Run_mc(taiMenuEl*));
  sub = menu->AddSubMenu("Run");
  itm_list->GetMenu(sub, &mc);

}
*/
