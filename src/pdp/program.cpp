// Copyright, 1995-2005, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of TA/PDP++
//
//   TA/PDP++ is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   TA/PDP++ is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.

#include "program.h"

#include "css_machine.h"
#include "css_basic_types.h"

#ifdef TA_GUI
# include "ta_qt.h"
# include "ta_qtdialog.h"

# include <QMessageBox>
#endif

//////////////////////////
//  ProgEl		//
//////////////////////////

void ProgEl::Initialize() {
}

void ProgEl::DataChanged(int dcr, void* op1, void* op2) {
  inherited::DataChanged(dcr, op1, op2);
  if (!(dcr == DCR_ITEM_UPDATED)) return;
  Program* prog = GET_MY_OWNER(Program);
  if (prog) {
    prog->setDirty(true);
  }
}

const String ProgEl::GenCss(int indent_level) {
  String rval;
  rval = GenCssPre_impl(indent_level) + GenCssBody_impl(indent_level) + GenCssPost_impl(indent_level);
  return rval;
}


//////////////////////////
//  UserScriptEl	//
//////////////////////////

void UserScriptEl::Initialize() {
  user_script = "// TODO: Add your CSS script code here.\n";
}

void UserScriptEl::Copy_(const UserScriptEl& cp) {
  user_script = cp.user_script;
}

const String UserScriptEl::GenCssBody_impl(int indent_level) {
// TODO: maybe indent every line by the indent amount
  return user_script;
}


//////////////////////////
//  ProgEl_List		//
//////////////////////////

const String ProgEl_List::GenCss(int indent_level) {
  String rval;
  ProgEl* el;
  for (int i = 0; i < size; ++i) {
    el = FastEl(i);
    rval += el->GenCss(indent_level); 
  }
  return rval;;
}


//////////////////////////
//  ProgVars		//
//////////////////////////

void ProgVars::Initialize() {
}

void ProgVars::Destroy() {
  CutLinks();
}

void ProgVars::InitLinks() {
  inherited::InitLinks();
  taBase::Own(script_vars, this);
}

void ProgVars::CutLinks() {
  script_vars.CutLinks();
  inherited::CutLinks();
}

const String ProgVars::GenCssBody_impl(int indent_level) {
  return script_vars.GenCss(indent_level);
}


//////////////////////////
//  ProgList		//
//////////////////////////

void ProgList::Initialize() {
}

void ProgList::InitLinks() {
  inherited::InitLinks();
  taBase::Own(prog_els, this);
}

void ProgList::CutLinks() {
  prog_els.CutLinks();
  inherited::CutLinks();
}

void ProgList::Copy_(const ProgList& cp) {
  prog_els = cp.prog_els; //TODO: need to make sure this is a value copy
}

const String ProgList::GenCssBody_impl(int indent_level) {
  return prog_els.GenCss(indent_level);
}


//////////////////////////
//  LoopEl		//
//////////////////////////

void LoopEl::Initialize() {
  loop_var = "i"; // the loop variable
  init_val = "0"; // initial value of loop variable
  loop_test = "i < 20"; // the test each time
  loop_iter = "i += 1"; // the iteration operation
}

void LoopEl::Copy_(const LoopEl& cp) {
  loop_var = cp.loop_var;
  init_val = cp.init_val;
  loop_test = cp.loop_test;
  loop_iter = cp.loop_iter;
}

const String LoopEl::GenCssPre_impl(int indent_level) {
  String rval = cssMisc::Indent(indent_level);
  rval += "{Int " + loop_var + ";\n";
  rval += "for (" + loop_var + " = " + init_val + "; "
    + loop_test + "; " 
    + loop_iter + ") {\n";
  return rval; 
}

const String LoopEl::GenCssBody_impl(int indent_level) {
  return inherited::GenCssBody_impl(indent_level + 1);
}

const String LoopEl::GenCssPost_impl(int indent_level) {
  return cssMisc::Indent(indent_level) + "}}\n";
}


//////////////////////////
//  CondEl		//
//////////////////////////

void CondEl::Initialize() {
  cond_test = "true";
}

void CondEl::InitLinks() {
  inherited::InitLinks();
  taBase::Own(true_els, this);
  taBase::Own(false_els, this);
}

void CondEl::CutLinks() {
  false_els.CutLinks();
  true_els.CutLinks();
  inherited::CutLinks();
}

void CondEl::Copy_(const CondEl& cp) {
  cond_test = cp.cond_test;
  true_els = cp.true_els; //TODO: need to make sure this is a value copy
  false_els = cp.false_els; //TODO: need to make sure this is a value copy
}

const String CondEl::GenCssPre_impl(int indent_level) {
  String rval = cssMisc::Indent(indent_level);
  rval += "if (" + cond_test + ") {\n";
  return rval; 
}

const String CondEl::GenCssBody_impl(int indent_level) {
  String rval = true_els.GenCss(indent_level + 1);
  rval += cssMisc::Indent(indent_level) + "} else {\n";
  rval += false_els.GenCss(indent_level + 1);
  return rval;
}

const String CondEl::GenCssPost_impl(int indent_level) {
  return cssMisc::Indent(indent_level) + "}\n";
}



//////////////////////////
//  Program		//
//////////////////////////

void Program::Initialize() {
  m_dirty = true; 
  m_our_hardvar_base_index = -1; // flag that not set yet
}

void Program::Destroy()	{ 
  CutLinks();
}

void Program::InitLinks() {
  inherited::InitLinks();
  taBase::Own(global_vars, this);
}

void Program::CutLinks() {
  global_vars.CutLinks();
  inherited::CutLinks();
}


void Program::Copy_(const Program& cp) {
  m_dirty = true; // require rebuild/refetch
  m_scriptCache = "";
  if (script)
    script->ClearAll();
}

void Program::UpdateAfterEdit() {
  script_compiled = false; // have to assume user changed something
  setDirty(true);
  inherited::UpdateAfterEdit();
}

void Program::InitScriptObj_impl() {
  AbstractScriptBase::InitScriptObj_impl();
  // if first time, find last intrinsic hardvar
  if (m_our_hardvar_base_index < 0) {
    m_our_hardvar_base_index = script->hard_vars.size;
  }
}

void Program::PreCompileScript_impl() {
  AbstractScriptBase::PreCompileScript_impl();
  UpdateScriptVars();
}

bool Program::Run() {
  if (!script_compiled) {
    if (!CompileScript())
      return false; // deferred or error
  } else {
    UpdateScriptVars();
  }
  return RunScript();
}

void Program::ScriptCompiled() {
  AbstractScriptBase::ScriptCompiled();
  //TODO: maybe inform gui
}

void Program::setDirty(bool value) {
  if (m_dirty == value) return;
  m_dirty = value;
  DirtyChanged_impl();
}

void  Program::UpdateScriptVars() {
// makes sure the global vars are in the script, and values are current
  // nuke any unnecessary ones (ex. user deleted a global_var)
  while ((script->hard_vars.size - m_our_hardvar_base_index) > 
    global_vars.size) 
  {
    script->hard_vars.DelPop(); // removes/unref-deletes
  }
    
  int i = 0;
  ScriptVar* sv;
  cssEl* el;
  // update names and values of existing (if any)
  while ((m_our_hardvar_base_index + i) < script->hard_vars.size) {
    sv = global_vars.FastEl(i);
    el = script->hard_vars.FastEl(m_our_hardvar_base_index + i);
    // easiest (harmless) is to just update, even if not changed 
    el->name = sv->name;
    *el = sv->value;
    ++i;
  }
  
  // add new
  while (i < global_vars.size) {
    sv = global_vars.FastEl(i);
    el = sv->NewCssEl();
    script->hard_vars.Push(el); //refs
    ++i;
  }
}

#ifdef TA_GUI
void Program::RunGui() {
  //TODO: need to put up a progress/cancel dialog
  // TODO: actually need to provide a global strategy for running!!!!!
  bool ran = Run();
  if (ran) 
    QMessageBox::information(NULL, QString("Operation Succeeded"),
      QString("The Program finished"), QMessageBox::Ok);
  else 
    QMessageBox::warning(NULL, QString("Operation Failed"),
      QString("The Program did not run -- check that there is script source, and that there were no compile errors"), QMessageBox::Ok, QMessageBox::NoButton);
 ; 
  
}

void Program::ViewScript() {
  ViewScript_impl();
}
#endif  // TA_GUI


//////////////////////////
//  Program_MGroup	//
//////////////////////////

void Program_MGroup::Initialize() {
  SetBaseType(&TA_Program);
}



//////////////////////////
//  ProgElProgram	//
//////////////////////////

void ProgElProgram::Initialize() {
}

void ProgElProgram::Destroy()	{ 
  CutLinks();
}

void ProgElProgram::InitLinks() {
  inherited::InitLinks();
  taBase::Own(prog_els, this);
}

void ProgElProgram::CutLinks() {
  prog_els.CutLinks();
  inherited::CutLinks();
}

void ProgElProgram::Copy_(const ProgElProgram& cp) {
  prog_els = cp.prog_els;
}

void ProgElProgram::UpdateAfterEdit() {
  inherited::UpdateAfterEdit();
}

void ProgElProgram::ChildUpdateAfterEdit(TAPtr child, bool& handled) {
  if (!handled && child->InheritsFrom(&TA_ProgEl)) {
    m_dirty = true;
    handled = true;
    UpdateAfterEdit();
  }
  inherited::ChildUpdateAfterEdit(child, handled);
} 

const String ProgElProgram::scriptString() {
  if (m_dirty) {
    m_scriptCache = "// ";
    m_scriptCache += GetName();
    m_scriptCache += "\n\n/* globals added to hardvars:\n";
    m_scriptCache += global_vars.GenCss(0);
    m_scriptCache += "*/\n\n";
    
    m_scriptCache += prog_els.GenCss(0);
    m_scriptCache += "\n";
    m_dirty = false;
  }
  return m_scriptCache;
}

#ifdef TA_GUI
void ProgElProgram::ViewScript_impl() {
  iTextEditDialog* dlg = new iTextEditDialog(true); // readonly
  dlg->setText(scriptString());
  dlg->exec();
}
#endif // TA_GUI

//////////////////////////
//  FileProgram	//
//////////////////////////

void FileProgram::Initialize() {
  script_file = taFiler_CreateInstance(".","*.css",false);
  script_file->compress = false;	// don't compress
  script_file->mode = taFiler::NO_AUTO;
  taRefN::Ref(script_file);
}


void FileProgram::Destroy() { 
  if (script_file) {
    taRefN::unRefDone(script_file);
    script_file = NULL;
  }
}

void FileProgram::Copy_(const FileProgram& cp) {
  if (script_file && cp.script_file)
    *script_file = *(cp.script_file);
}

const String FileProgram::scriptFilename() {
  return script_file->fname;
}

AbstractScriptBase::ScriptSource FileProgram::scriptSource() {
  if (script_file->fname.empty())
    return NoScript;
  else
    return ScriptFile;
}

#ifdef TA_GUI
void FileProgram::ViewScript_impl() {
  //TODO: edit in editor
}
#endif // TA_GUI

