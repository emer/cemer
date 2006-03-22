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
//   ScriptVar		//
//////////////////////////

void ScriptVar::Copy_(const ScriptVar& cp) {
  value = cp.value;
}

void ScriptVar::UpdateAfterEdit() {
  if (!cssMisc::IsNameValid(name)) {
    taMisc::Error("'", name, "' is not a valid name in css scripts; must be alphanums or underscores");
//TODO: should revert
  }
  inherited::UpdateAfterEdit();
}

const String ScriptVar::GenCssArg_impl() {
  String rval(0, 80, '\0'); //note: buffer will extend if needed
  rval += "Variant ";
  rval += name;
  bool init = !value.isDefault();
  if (init) {
    rval += " = ";
    rval += value.toCssLiteral();
  }
  return rval;
}

const String ScriptVar::GenCssVar_impl(bool make_new, TypeDef* val_type) {
  String rval(0, 80, '\0'); //note: buffer will extend if needed
  rval += "Variant ";
  rval += name;
  if (make_new && val_type) { //note: val_type should always be supplied if make_new true
    rval += " = new ";
    rval += val_type->name + "()";
  } else { 
    bool init = !value.isDefault();
    if (init) {
      rval += " = ";
      rval += value.toCssLiteral();
    } else if (!value.isInvalid()) {
      rval += ";  " + name + ".setType(" + String((int)value.type()) + ")";
    }
  }
  rval += ";\n";
  return rval;
}

cssEl* ScriptVar::NewCssEl() {
  cssVariant* rval = new cssVariant(value, name);
  return rval;
}

//////////////////////////
//   EnumScriptVar	//
//////////////////////////

void EnumScriptVar::Initialize() {
  value.setType(Variant::T_Int);
  enum_type = NULL;
  init = true;
}

void EnumScriptVar::Destroy() {
}

void EnumScriptVar::Copy_(const EnumScriptVar& cp) {
  enum_type = cp.enum_type;
  init = cp.init;
}

const String EnumScriptVar::enumName() {
  if (enum_type) {
    //TODO: need to make sure it is type::enum
    return enum_type->Get_C_Name();
  } else return _nilString;
}

const String EnumScriptVar::GenCssArg_impl() {
  String rval(0, 80, '\0'); //note: buffer will extend if needed
  rval += enumName() + " ";
  rval += name;
  if (init) {
    rval += " = ";
    rval += ValToId(value.toInt());
  }
  return rval;
}

const String EnumScriptVar::GenCssVar_impl(bool, TypeDef*) {
  String rval = GenCssArg_impl();
  rval += ";\n";
  return rval;
}

cssEl* EnumScriptVar::NewCssEl() {
  cssEnum* rval = new cssEnum(value.toInt(), name);
  return rval;
}

const String EnumScriptVar::ValToId(int val) {
  String rval(0, 80, '\0'); //note: buffer will extend if needed
  //TODO
  return rval;
}


//////////////////////////
//   ObjectScriptVar	//
//////////////////////////

void ObjectScriptVar::Initialize() {
  val_type = &TA_taOBase; //note: < taOBase generally not interesting
  make_new = false;
}

void ObjectScriptVar::Destroy() {
}

void ObjectScriptVar::Copy_(const ObjectScriptVar& cp) {
  val_type = cp.val_type;
  make_new = cp.make_new;
}

//////////////////////////
//   ScriptVar_List	//
//////////////////////////

void ScriptVar_List::Initialize() {
  SetBaseType(&TA_ScriptVar);
  var_context = VC_ProgVars;
}

void ScriptVar_List::El_SetIndex_(void* it_, int idx) {
  ScriptVar* it = (ScriptVar*)it_;
  if (it->name.empty()) {
    it->name = "Var_" + (String)idx;
  }
}

const String ScriptVar_List::GenCss(int indent_level) const {
  String rval(0, 40 * size, '\0'); // buffer with typical-ish room
  ScriptVar* el;
  for (int i = 0; i < size; ++i) {
    el = FastEl(i);
    bool is_arg = (var_context == VC_FuncArgs);
    if (is_arg) {
      if (i > 0)
        rval += ", ";
    } else {
      rval += cssMisc::Indent(indent_level); 
    }
    rval += el->GenCss(is_arg); 
  }
  return rval;
}



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
  script->ClearAll();
  switch (scriptSource()) {
  case NoScript: return false; //nothing to do
  case ScriptString:
    script->CompileCode(scriptString());
    break;
  case ScriptFile:
    script->Compile(scriptFilename());
    break;
  default: break;// shouldn't happen
  }
  script_compiled = true;
  ScriptCompiled();
  return true;
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

void AbstractScriptBase::InteractScript() {
  if (!CompileScript()) return;
  cssMisc::next_shell = script;
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
