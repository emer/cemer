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

void ProgEl::ChildUpdateAfterEdit(TAPtr child, bool& handled) {
  inherited::ChildUpdateAfterEdit(child, handled);
  Program* prog = GET_MY_OWNER(Program);
  if (prog) {
    prog->setDirty(true);
  }
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
  // we could just return the string, but that wouldn't indent...
// TODO: indent every line by the indent amount
//  String rval(user_script.length() + 20, 0, '\0');
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
  loop_var_type = "Int"; // the loop variable
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
  rval += "{" + loop_var_type + " " + loop_var + ";\n";
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
//  MethodCallEl	//
//////////////////////////

void MethodCallEl::Initialize() {
  script_obj = NULL;
  method = NULL;
}

void MethodCallEl::CutLinks() {
  taBase::DelPointer((taBase**)&script_obj);
  inherited::CutLinks();
}

void MethodCallEl::Copy_(const MethodCallEl& cp) {
  taBase::SetPointer((taBase**)&script_obj, cp.script_obj);
  method = cp.method;
}

void MethodCallEl::UpdateAfterEdit() {
  //TODO: if object changed, we need to make sure MethodDef is still valid
  inherited::UpdateAfterEdit();
}

const String MethodCallEl::GenCssBody_impl(int indent_level) {
  if (!script_obj && !method) return _nilString;
  
  String rval(80, 0, '\0');
  //TODO: return value
  rval += cssMisc::Indent(indent_level);
  rval += script_obj->name;
  rval += "->";
  rval += method->name;
  rval += "(";
  //TODO: args
  rval += ");\n";
  
  return rval;
}


//////////////////////////
//  ProgramCallEl	//
//////////////////////////

const String ProgramCallEl::prfx = "__prog_";

void ProgramCallEl::Initialize() {
  target = NULL;
  old_target = NULL;
}

void ProgramCallEl::InitLinks() {
  inherited::InitLinks();
  taBase::Own(global_args, this);
  taBase::Own(fail_el, this);
  if (!taMisc::is_loading) {
    fail_el.user_script = "cerr << \"Program Call failed--Stopping\\n\";\nthis->StopScript();\n";
  }
}

void ProgramCallEl::CutLinks() {
  taBase::DelPointer((taBase**)&target);
  global_args.CutLinks();
  old_target = NULL;
  inherited::CutLinks();
}

void ProgramCallEl::Copy_(const ProgramCallEl& cp) {
  taBase::SetPointer((taBase**)&target, cp.target);
  global_args = cp.global_args;
  fail_el = cp.fail_el;
}

void ProgramCallEl::UpdateAfterEdit() {
  if (target != old_target) {
    old_target = target; // note: we don't ref, because we just need to check ptr addr
    UpdateGlobalArgs();
  }
  inherited::UpdateAfterEdit();
}

const String ProgramCallEl::GenCssPre_impl(int indent_level) {
  STRING_BUF(rval, 50);
  rval = cssMisc::Indent(indent_level);
  rval += "{ // call program: "; 
  if (target)
    rval += target->name;
  rval += "\n";
  return rval;
}

const String ProgramCallEl::GenCssBody_impl(int indent_level) {
  if (!target) return _nilString;
  STRING_BUF(rval, 250);
  rval += cssMisc::Indent(indent_level);
  rval += "Bool call_succeeded = false;\n";
  rval += cssMisc::Indent(indent_level);
  rval += "Program target = ";
  rval += target->GetPath();
  rval += ";\n";
  rval += cssMisc::Indent(indent_level);
  rval += "if (target->CompileScript()) {\n"; 
  ++indent_level;
  
  rval += cssMisc::Indent(indent_level);
  rval += "// set global vars of target\n";
  String nm;
  ScriptVar* ths_var;
  ScriptVar* prg_var;
  for (int i = 0; i < global_args.size; ++i) {
    ths_var = global_args.FastEl(i);
    if (ths_var->ignore) continue;
    nm = ths_var->name.after(prfx);
    prg_var = target->global_vars.FindName(nm);
    if (!prg_var || prg_var->ignore) continue;
    rval += cssMisc::Indent(indent_level);
    //TODO: need to be using ScriptArg instead of ScriptVar
    rval += "target->SetGlobalVar(\"" + prg_var->name + "\", "
      + ths_var->value.toCssLiteral() + ");\n";
  }
  
  rval += cssMisc::Indent(indent_level);
  rval += "call_succeeded = target->Run();\n";
  --indent_level;
  rval += cssMisc::Indent(indent_level);
  rval += "}\n";
  rval += cssMisc::Indent(indent_level);
  rval += "if (!call_succeeded) {\n";
  rval += fail_el.GenCss(indent_level + 1);
  rval += cssMisc::Indent(indent_level);
  rval += "}\n";
  
  return rval;
}

const String ProgramCallEl::GenCssPost_impl(int indent_level) {
  return cssMisc::Indent(indent_level) + "} // call program\n";
}

void ProgramCallEl::UpdateGlobalArgs() {
  if (!target) return; // just leave existing stuff for now
  // we prefix our local copies of prog args with following:
  int i;
  int t;
  ScriptVar* ths_var; // our copy
  ScriptVar* prg_var; // target's copy
  String nm;
  // delete defunct, in ours but not in target
  // when we find matches, we update our copy
  for (i = global_args.size - 1; i >= 0; --i) {
    ths_var = global_args.FastEl(i);
    nm = ths_var->name.after(prfx);
    t = target->global_vars.Find(nm);
    if (t >= 0) { 
      // found, but make sure they are of same type
      prg_var = target->global_vars.FastEl(t);
      if (ths_var->GetTypeDef() == prg_var->GetTypeDef()) {
        // ok, match, so update our copy and continue
        ths_var->Freshen(*prg_var);
        // we don't unignore, but will copy ignore
        if (prg_var->ignore)
          ths_var->ignore = true;
        ths_var->UpdateAfterEdit();
        continue; 
      }
    }
    // not found, or not same type
    global_args.Remove(i);
  }
  // add new
  for (i = 0; i < target->global_vars.size; ++i) {
    prg_var = target->global_vars.FastEl(i);
    nm = prfx + prg_var->name;
    t = global_args.Find(nm);
    if (t >= 0) 
      continue; 
    ths_var = (ScriptVar*)prg_var->Clone();
    global_args.Add(ths_var);
    ths_var->name = nm;
  }
}

//////////////////////////
//  Program		//
//////////////////////////

void Program::Initialize() {
  m_dirty = true; 
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
  setDirty(true);
  inherited::UpdateAfterEdit();
}

void Program::InitScriptObj_impl() {
  AbstractScriptBase::InitScriptObj_impl();
  //NOTE: nothing else, could be eliminated
}

void Program::PreCompileScript_impl() {
  AbstractScriptBase::PreCompileScript_impl();
  UpdateScriptVars();
}

bool Program::Run() {
  if (!script_compiled) {
    if (!CompileScript())
      return false; // deferred or error
  }
  return RunScript();
}

void Program::ScriptCompiled() {
  AbstractScriptBase::ScriptCompiled();
  m_dirty = false;
  script_compiled = true;
  DataChanged(DCR_ITEM_UPDATED);
}

void Program::setDirty(bool value) {
  if (m_dirty == value) return;
  m_dirty = value;
  script_compiled = false; // have to assume user changed something
  DirtyChanged_impl();
  DataChanged(DCR_ITEM_UPDATED);
}

bool Program::SetGlobalVar(const String& nm, const Variant& value) {
  cssElPtr& el_ptr = script->prog_vars.FindName(nm);
  if (el_ptr == cssMisc::VoidElPtr) return false;
  cssEl* el = el_ptr.El();
  *el = value;
  return true;
}

void  Program::UpdateScriptVars() {
  //NOTE: if we have to nuke any or change any types then we have to recompile!
  // but currently, we only do this before recompiling anyway, so no worries!
// easiest is just to nuke and recreate...
  // nuke existing
  script->prog_vars.Reset(); // removes/unref-deletes
    
  // add new
  for (int i = 0; i < global_vars.size; ++i) {
    ScriptVar* sv = global_vars.FastEl(i);
    if (sv->ignore) continue;
    cssEl* el = sv->NewCssEl();
    script->prog_vars.Push(el); //refs
  } 
/*old  int i = 0;
  ScriptVar* sv;
  cssEl* el;
  // update names and values of existing (if any)
  while ((m_our_hardvar_base_index + i) < script->hard_vars.size) {
    sv = global_vars.FastEl(i);
    if (sv->ignore) continue;
    el = script->hard_vars.FastEl(m_our_hardvar_base_index + i);
    // easiest (harmless) is to just update, even if not changed 
    el->name = sv->name;
    *el = sv->value;
    ++i;
  }
  
  // add new
  while (i < global_vars.size) {
    sv = global_vars.FastEl(i);
    if (sv->ignore) continue;
    el = sv->NewCssEl();
    script->hard_vars.Push(el); //refs
    ++i;
  } */
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

