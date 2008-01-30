// Copyright, 1995-2007, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of The Emergent Toolkit
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

#include "ta_program_els.h"

#include "css_machine.h"
// #include "css_basic_types.h"
// #include "css_c_ptr_types.h"
// #include "css_ta.h"
// #include "ta_project.h"

// #include <QCoreApplication>
// #include <QDir>

//////////////////////////
//  CodeBlock		//
//////////////////////////

void CodeBlock::Initialize() {
}

void CodeBlock::CheckChildConfig_impl(bool quiet, bool& rval) {
  inherited::CheckChildConfig_impl(quiet, rval);
  prog_code.CheckConfig(quiet, rval);
}

const String CodeBlock::GenCssPre_impl(int indent_level) {
  if(prog_code.size == 0) return _nilString;
  String rval = cssMisc::Indent(indent_level) + "{\n";
  return rval; 
}

const String CodeBlock::GenCssBody_impl(int indent_level) {
  if(prog_code.size == 0) return _nilString;
  return prog_code.GenCss(indent_level+1);
}

const String CodeBlock::GenCssPost_impl(int indent_level) {
  if(prog_code.size == 0) return _nilString;
  String rval = cssMisc::Indent(indent_level) + "}\n";
  return rval; 
}

const String CodeBlock::GenListing_children(int indent_level) {
  return prog_code.GenListing(indent_level + 1);
}

String CodeBlock::GetDisplayName() const {
  return "CodeBlock (" + String(prog_code.size) + " items)";
}

void CodeBlock::PreGenChildren_impl(int& item_id) {
  prog_code.PreGen(item_id);
}
ProgVar* CodeBlock::FindVarName(const String& var_nm) const {
  return prog_code.FindVarName(var_nm);
}

//////////////////////////
//  ProgVars		//
//////////////////////////

void ProgVars::Initialize() {
}

void ProgVars::Destroy() {
  CutLinks();
}

void ProgVars::CheckChildConfig_impl(bool quiet, bool& rval) {
  inherited::CheckChildConfig_impl(quiet, rval);
  local_vars.CheckConfig(quiet, rval);
}

const String ProgVars::GenCssBody_impl(int indent_level) {
  return local_vars.GenCss(indent_level);
}
const String ProgVars::GenListing_children(int indent_level) {
  return local_vars.GenListing(indent_level + 1);
}

String ProgVars::GetDisplayName() const {
  String rval;
  rval += "ProgVars (";
  rval += String(local_vars.size);
  rval += " vars)";
  return rval;
}

ProgVar* ProgVars::FindVarName(const String& var_nm) const {
  return local_vars.FindName(var_nm);
}

//////////////////////////
//    UserScript	//
//////////////////////////


// todo: remove after a while (4.0.10)
void UserScript::SetProgExprFlags() {
  script.SetExprFlag(ProgExpr::NO_VAR_ERRS); // don't report bad variable errors
  script.SetExprFlag(ProgExpr::FULL_STMT); // full statements for parsing
}

void UserScript::Initialize() {
  static String _def_user_script("// TODO: Add your CSS script code here.\n");
  script.expr = _def_user_script;
  SetProgExprFlags();
}

void UserScript::UpdateAfterEdit_impl() {
  SetProgExprFlags();
  inherited::UpdateAfterEdit_impl();
}

const String UserScript::GenCssBody_impl(int indent_level) {
  script.ParseExpr();		// re-parse just to be sure!
  String rval(cssMisc::IndentLines(script.GetFullExpr(), indent_level));
  // strip trailing non-newline ws, and make sure there is a trailing newline
  rval = trimr(rval);
  if (rval.lastchar() != '\n')
    rval += '\n';
  return rval;
}

String UserScript::GetDisplayName() const {
  return script.expr;
}

void UserScript::ImportFromFile(istream& strm) {
  script.expr = _nilString;
  char c;
  while((c = strm.get()) != EOF) {
    script.expr += c;
  }
  UpdateAfterEdit();
}

void UserScript::ImportFromFileName(const String& fnm) {
  String full_fnm = taMisc::FindFileOnLoadPath(fnm);
  fstream strm;
  strm.open(full_fnm, ios::in);
  ImportFromFile(strm);
  strm.close();
}

void UserScript::ExportToFile(ostream& strm) {
  strm << script.GetFullExpr();
}

void UserScript::ExportToFileName(const String& fnm) {
  fstream strm;
  strm.open(fnm, ios::out);
  ExportToFile(strm);
  strm.close();
}

//////////////////////////
//  WhileLoop		//
//////////////////////////

void WhileLoop::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  CheckError(test.expr.empty(), quiet, rval, "test expression is empty");
}

const String WhileLoop::GenCssPre_impl(int indent_level) {
  test.ParseExpr();		// re-parse just to be sure!
  return cssMisc::Indent(indent_level) + "while (" + test.GetFullExpr() + ") {\n";
}

const String WhileLoop::GenCssPost_impl(int indent_level) {
  return cssMisc::Indent(indent_level) + "}\n";
}

String WhileLoop::GetDisplayName() const {
  return "while (" + test.expr + ")";
}

//////////////////////////
//  DoLoop		//
//////////////////////////

void DoLoop::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  CheckError(test.expr.empty(), quiet, rval, "test expression is empty");
}

const String DoLoop::GenCssPre_impl(int indent_level) {
  String rval = cssMisc::Indent(indent_level) + "do {\n";
  return rval; 
}

const String DoLoop::GenCssPost_impl(int indent_level) {
  test.ParseExpr();		// re-parse just to be sure!
  String rval = cssMisc::Indent(indent_level) + "} while (" + test.GetFullExpr() + ");\n";
  return rval;
}

String DoLoop::GetDisplayName() const {
  return "do ... while (" + test.expr + ")";
}

//////////////////////////
//  ForLoop		//
//////////////////////////

// todo: remove after a while (4.0.10)
void ForLoop::SetProgExprFlags() {
  init.SetExprFlag(ProgExpr::NO_VAR_ERRS); // don't report bad variable errors
  test.SetExprFlag(ProgExpr::NO_VAR_ERRS); // don't report bad variable errors
  iter.SetExprFlag(ProgExpr::NO_VAR_ERRS); // don't report bad variable errors

  init.SetExprFlag(ProgExpr::FOR_LOOP_EXPR);// requires special parsing
  iter.SetExprFlag(ProgExpr::FOR_LOOP_EXPR);
}

void ForLoop::Initialize() {
  // the following are just default examples for the user
  init.expr = "i = 0";
  test.expr = "i < 10";
  iter.expr = "i++";
  SetProgExprFlags();
}

void ForLoop::UpdateAfterEdit_impl() {
  SetProgExprFlags();
  inherited::UpdateAfterEdit_impl();
  if(taMisc::is_loading) return;
  Program* prg = GET_MY_OWNER(Program);
  if(!prg || isDestroying() || prg->isDestroying()) return;
  GetIndexVar();
}

void ForLoop::GetIndexVar() {
  Program* my_prog = program();
  if(!my_prog) return;
  if(init.expr.at(0,2) == "i " && test.expr.at(0,2) == "i ") {
    if(!my_prog->vars.FindName("i")) {
      ProgVar* ivar = (ProgVar*)my_prog->vars.New(1, &TA_ProgVar);
      ivar->name = "i";
      ivar->SetInt(0);
      ivar->ClearVarFlag(ProgVar::CTRL_PANEL);
      ivar->DataChanged(DCR_ITEM_UPDATED);
      // get the var ptrs in case someone changes them later!
      init.ParseExpr();
      test.ParseExpr();
      iter.ParseExpr();
    }
  }
}

void ForLoop::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  CheckError(test.expr.empty(), quiet, rval, "test expression is empty");
  CheckError(iter.expr.empty(), quiet, rval, "iter expression is empty");
}

const String ForLoop::GenCssPre_impl(int indent_level) {
  init.ParseExpr();		// re-parse just to be sure!
  test.ParseExpr();		// re-parse just to be sure!
  iter.ParseExpr();		// re-parse just to be sure!
  String rval;
  rval = cssMisc::Indent(indent_level) + 
    "for (" + init.GetFullExpr() + "; " + test.GetFullExpr() + "; " + iter.GetFullExpr() + ") {\n";
  return rval; 
}

const String ForLoop::GenCssPost_impl(int indent_level) {
  String rval = cssMisc::Indent(indent_level) + "}\n";
  return rval;
}

String ForLoop::GetDisplayName() const {
  return "for (" + init.expr + "; " + test.expr + "; " + iter.expr + ")";
}


//////////////////////////
//  IfContinue		//
//////////////////////////

void IfContinue::Initialize() {
}

void IfContinue::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  CheckError(cond.expr.empty(), quiet, rval,  "condition expression is empty");
  CheckEqualsError(cond.expr, quiet, rval);
}

const String IfContinue::GenCssBody_impl(int indent_level) {
  cond.ParseExpr();		// re-parse just to be sure!
  String rval;
  rval = cssMisc::Indent(indent_level) + 
    "if(" + cond.GetFullExpr() + ") continue;\n";
  return rval; 
}

String IfContinue::GetDisplayName() const {
  return "if(" + cond.expr + ") continue;";
}


//////////////////////////
//  IfBreak		//
//////////////////////////

void IfBreak::Initialize() {
}

void IfBreak::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  CheckError(cond.expr.empty(), quiet, rval,  "condition expression is empty");
  CheckEqualsError(cond.expr, quiet, rval);
}

const String IfBreak::GenCssBody_impl(int indent_level) {
  cond.ParseExpr();		// re-parse just to be sure!
  String rval;
  rval = cssMisc::Indent(indent_level) + 
    "if(" + cond.GetFullExpr() + ") break;\n";
  return rval; 
}

String IfBreak::GetDisplayName() const {
  return "if(" + cond.expr + ") break;";
}

//////////////////////////
//  IfReturn		//
//////////////////////////

void IfReturn::Initialize() {
}

void IfReturn::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  CheckError(cond.expr.empty(), quiet, rval,  "condition expression is empty");
  CheckEqualsError(cond.expr, quiet, rval);
}

const String IfReturn::GenCssBody_impl(int indent_level) {
  cond.ParseExpr();		// re-parse just to be sure!
  String rval;
  rval = cssMisc::Indent(indent_level) + 
    "if(" + cond.GetFullExpr() + ") return;\n";
  return rval; 
}

String IfReturn::GetDisplayName() const {
  return "if(" + cond.expr + ") return;";
}

//////////////////////////
//  If			//
//////////////////////////

void If::Initialize() {
  //  cond.expr = "true";
}

void If::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  CheckError(cond.expr.empty(), quiet, rval,  "condition expression is empty");
  CheckEqualsError(cond.expr, quiet, rval);
}

void If::CheckChildConfig_impl(bool quiet, bool& rval) {
  inherited::CheckChildConfig_impl(quiet, rval);
  true_code.CheckConfig(quiet, rval);
}

const String If::GenCssPre_impl(int indent_level) {
  cond.ParseExpr();		// re-parse just to be sure!
  String rval = cssMisc::Indent(indent_level);
  rval += "if(" + cond.GetFullExpr() + ") {\n";
  return rval; 
}

const String If::GenCssBody_impl(int indent_level) {
  String rval = true_code.GenCss(indent_level + 1);
  return rval;
}

const String If::GenListing_children(int indent_level) {
  String rval = true_code.GenListing(indent_level + 1);
  return rval;
}

const String If::GenCssPost_impl(int indent_level) {
  return cssMisc::Indent(indent_level) + "}\n";
}

String If::GetDisplayName() const {
  return "if (" + cond.expr + ")";
}

void If::PreGenChildren_impl(int& item_id) {
  true_code.PreGen(item_id);
}

ProgVar* If::FindVarName(const String& var_nm) const {
  ProgVar* pv = true_code.FindVarName(var_nm);
  return pv;
}

//////////////////////////
//  IfElse		//
//////////////////////////

void IfElse::Initialize() {
  //  cond.expr = "true";
}

void IfElse::CheckChildConfig_impl(bool quiet, bool& rval) {
  inherited::CheckChildConfig_impl(quiet, rval);
  false_code.CheckConfig(quiet, rval);
}

const String IfElse::GenCssBody_impl(int indent_level) {
  String rval = true_code.GenCss(indent_level + 1);
  // don't gen 'else' portion unless there are els
  if (false_code.size > 0) {
    rval += cssMisc::Indent(indent_level) + "} else {\n";
    rval += false_code.GenCss(indent_level + 1);
  }
  return rval;
}
const String IfElse::GenListing_children(int indent_level) {
  String rval = true_code.GenListing(indent_level + 1);
  if (false_code.size > 0) {
    rval += cssMisc::Indent(indent_level) + "else\n";
    rval += false_code.GenListing(indent_level + 1);
  }
  return rval;
}

void IfElse::PreGenChildren_impl(int& item_id) {
  inherited::PreGenChildren_impl(item_id);
  false_code.PreGen(item_id);
}

ProgVar* IfElse::FindVarName(const String& var_nm) const {
  ProgVar* pv = inherited::FindVarName(var_nm);
  if (pv) return pv;
  return false_code.FindVarName(var_nm);
}

//////////////////////////
//  IfGuiPrompt		//
//////////////////////////

void IfGuiPrompt::Initialize() {
  prompt = "Do you want to...";
  yes_label = "Yes";
  no_label = "No";
}

void IfGuiPrompt::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  prompt.gsub("\"", "''");
}

void IfGuiPrompt::CheckChildConfig_impl(bool quiet, bool& rval) {
  inherited::CheckChildConfig_impl(quiet, rval);
  yes_code.CheckConfig(quiet, rval);
}

const String IfGuiPrompt::GenCssPre_impl(int indent_level) {
  String il = cssMisc::Indent(indent_level);
  String rval;
  if(taMisc::gui_active) {
    rval = il + "{ int chs = taMisc::Choice(\"" + prompt + "\", \""
      + yes_label + "\", \""
      + no_label + "\");\n";
    rval += cssMisc::Indent(indent_level+1) + "if(chs == 0) {\n";
  }
  else {
    rval = il + "{\n";		// just a block to run..
  }
  return rval; 
}

const String IfGuiPrompt::GenCssBody_impl(int indent_level) {
  String rval = yes_code.GenCss(indent_level + 1 + (int)taMisc::gui_active);
  return rval;
}
const String IfGuiPrompt::GenListing_children(int indent_level) {
  String rval = yes_code.GenListing(indent_level + 1 + (int)taMisc::gui_active);
  return rval;
}

const String IfGuiPrompt::GenCssPost_impl(int indent_level) {
  if(taMisc::gui_active) {
    return cssMisc::Indent(indent_level+1) + "}\n" +
      cssMisc::Indent(indent_level) + "}\n"; // double close
  }
  else {
    return cssMisc::Indent(indent_level) + "}\n";
  }
}

String IfGuiPrompt::GetDisplayName() const {
  return "if (gui && " + prompt + ")";
}

void IfGuiPrompt::PreGenChildren_impl(int& item_id) {
  yes_code.PreGen(item_id);
}
ProgVar* IfGuiPrompt::FindVarName(const String& var_nm) const {
  ProgVar* pv = yes_code.FindVarName(var_nm);
  return pv;
}

//////////////////////////
//  CaseBlock		//
//////////////////////////

void CaseBlock::Initialize() {
}

void CaseBlock::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  CheckError(case_val.empty(), quiet, rval,  "case value expression is empty!");
}

const String CaseBlock::GenCssPre_impl(int indent_level) {
  case_val.ParseExpr();		// re-parse just to be sure!
  if(prog_code.size == 0) return _nilString;
  String il = cssMisc::Indent(indent_level);
  String rval = il + "case " + case_val.GetFullExpr() + ": {\n";
  return rval; 
}

const String CaseBlock::GenCssBody_impl(int indent_level) {
  if(prog_code.size == 0) return _nilString;
  return prog_code.GenCss(indent_level+1);
}

const String CaseBlock::GenCssPost_impl(int indent_level) {
  if(prog_code.size == 0) return _nilString;
  String rval = cssMisc::Indent(indent_level+1) + "break;\n"; // always break
  rval += cssMisc::Indent(indent_level) + "}\n";
  return rval; 
}

String CaseBlock::GetDisplayName() const {
  return "case: " + case_val.expr + " (" + String(prog_code.size) + " items)";
}

//////////////////////////
//  Switch		//
//////////////////////////

void Switch::Initialize() {
  cases.SetBaseType(&TA_CaseBlock);
}

void Switch::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  UpdateProgVarRef_NewOwner(switch_var);
}

void Switch::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  CheckError(!switch_var, quiet, rval, "switch_var is NULL");
}

void Switch::CheckChildConfig_impl(bool quiet, bool& rval) {
  inherited::CheckChildConfig_impl(quiet, rval);
  cases.CheckConfig(quiet, rval);
}

const String Switch::GenCssPre_impl(int indent_level) {
  if(!switch_var) return _nilString;
  String rval = cssMisc::Indent(indent_level);
  rval += "switch(" + switch_var->name + ") {\n";
  return rval; 
}

const String Switch::GenCssBody_impl(int indent_level) {
  if(!switch_var) return _nilString;
  return cases.GenCss(indent_level+1);
}

const String Switch::GenCssPost_impl(int indent_level) {
  if(!switch_var) return _nilString;
  return cssMisc::Indent(indent_level) + "}\n";
}

const String Switch::GenListing_children(int indent_level) {
  return cases.GenListing(indent_level+1);
}

String Switch::GetDisplayName() const {
  if(switch_var)
    return "switch(" + switch_var->name + ")";
  return "switch( VAR NOT SET!)";
}

void Switch::PreGenChildren_impl(int& item_id) {
  cases.PreGen(item_id);
}

ProgVar* Switch::FindVarName(const String& var_nm) const {
  return cases.FindVarName(var_nm);
}

void Switch::CasesFmEnum() {
  if(TestError(!switch_var, "CasesFmEnum", "switch_var not set!"))
    return;
  if(TestError(((switch_var->var_type != ProgVar::T_DynEnum) && 
		(switch_var->var_type != ProgVar::T_HardEnum)), "CasesFmEnum", "switch_var is not an enumerated type (either hard-coded enum or a dynamic enum)!"))
    return;

  if(switch_var->var_type == ProgVar::T_HardEnum)
    CasesFmEnum_hard();
  else
    CasesFmEnum_dyn();

  if(taMisc::gui_active) {
    tabMisc::DelayedFunCall_gui(this, "BrowserExpandAll");
  }
}

void Switch::CasesFmEnum_hard() {
  if(TestError(!switch_var->hard_enum_type, "CasesFmEnum", "switch_var hard_enum_type not set!"))
    return;
  TypeDef* et = switch_var->hard_enum_type;
  String enm = et->name + "::";
  int i;  int ti;
  CaseBlock* pe;
  EnumDef* ei;
  // delete ones that no longer exist
  for (i = cases.size - 1; i >= 0; --i) {
    pe = (CaseBlock*)cases.FastEl(i);
    ei = et->enum_vals.FindName(pe->case_val.expr);
    if(!ei) {
      cases.RemoveIdx(i);
    }
  }
  // add new ones and put in order
  for (ti = 0; ti < et->enum_vals.size; ++ti) {
    ei = et->enum_vals.FastEl(ti);
    String einm = enm + ei->name;
    for(i=0;i<cases.size;i++) {
      pe = (CaseBlock*)cases.FastEl(i);
      if(pe->case_val.expr == einm) break;
    }
    if(i==cases.size) {
      pe = new CaseBlock();
      pe->case_val.SetExpr(einm);
      cases.Insert(pe, ti);
    } else if (i != ti) {
      cases.MoveIdx(i, ti);
    }
  }
}

void Switch::CasesFmEnum_dyn() {
  if(TestError(!switch_var->dyn_enum_val.enum_type.ptr(), "CasesFmEnum", "switch_var dyn enum_type not set!"))
    return;
  DynEnumType* et = switch_var->dyn_enum_val.enum_type.ptr();
  int i;  int ti;
  CaseBlock* pe;
  DynEnumItem* ei;
  // delete ones that no longer exist
  for (i = cases.size - 1; i >= 0; --i) {
    pe = (CaseBlock*)cases.FastEl(i);
    ei = et->enums.FindName(pe->case_val.expr);
    if(!ei) {
      cases.RemoveIdx(i);
    }
  }
  // add new ones and put in order
  for (ti = 0; ti < et->enums.size; ++ti) {
    ei = et->enums.FastEl(ti);
    for(i=0;i<cases.size;i++) {
      pe = (CaseBlock*)cases.FastEl(i);
      if(pe->case_val.expr == ei->name) break;
    }
    if(i==cases.size) {
      pe = new CaseBlock();
      pe->case_val.SetExpr(ei->name);
      cases.Insert(pe, ti);
    } else if (i != ti) {
      cases.MoveIdx(i, ti);
    }
  }
}

//////////////////////////
//    AssignExpr	//
//////////////////////////

void AssignExpr::Initialize() {
}

void AssignExpr::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  UpdateProgVarRef_NewOwner(result_var);
}

void AssignExpr::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  CheckError(!result_var, quiet, rval, "result_var is NULL");
  CheckProgVarRef(result_var, quiet, rval);
  expr.CheckConfig(quiet, rval);
}

const String AssignExpr::GenCssBody_impl(int indent_level) {
  expr.ParseExpr();		// re-parse just to be sure!
  String rval;
  rval += cssMisc::Indent(indent_level);
  if (!result_var) {
    rval += "//WARNING: AssignExpr not generated here -- result_var not specified\n";
    return rval;
  }
  
  rval += result_var->name + " = " + expr.GetFullExpr() + ";\n";
  return rval;
}

String AssignExpr::GetDisplayName() const {
  if(!result_var)
    return "(result_var not selected)";
  
  String rval;
  rval += result_var->name + "=" + expr.GetFullExpr();
  return rval;
}

//////////////////////////
//      VarIncr 	//
//////////////////////////

void VarIncr::Initialize() {
  expr.expr = "1";
}

void VarIncr::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  UpdateProgVarRef_NewOwner(var);
}

void VarIncr::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  CheckError(!var, quiet, rval, "var is NULL");
  CheckProgVarRef(var, quiet, rval);
  expr.CheckConfig(quiet, rval);
}

const String VarIncr::GenCssBody_impl(int indent_level) {
  expr.ParseExpr();		// re-parse just to be sure!
  String rval;
  rval += cssMisc::Indent(indent_level);
  if (!var) {
    rval += "//WARNING: VarIncr not generated here -- var not specified\n";
    return rval;
  }
  
  rval += var->name + " = " + var->name + " + " + expr.GetFullExpr() + ";\n";
  return rval;
}

String VarIncr::GetDisplayName() const {
  if(!var)
    return "(var not selected)";
  
  String rval;
  rval += var->name + "+=" + expr.GetFullExpr();
  return rval;
}

//////////////////////////
//    MethodCall	//
//////////////////////////

bool MethodCall::ShowVarFilter(void* var_) {
  ProgVar* var = static_cast<ProgVar*>(var_);
  return (var->var_type == ProgVar::T_Object);
}

void MethodCall::Initialize() {
  method = NULL;
  obj_type = &TA_taBase; // placeholder
}

void MethodCall::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  if(obj)
    obj_type = obj->act_object_type();
  else obj_type = &TA_taBase; // placeholder

  UpdateProgVarRef_NewOwner(result_var);
  UpdateProgVarRef_NewOwner(obj);

//  if(!taMisc::is_loading && method)
  if (method) { // needed to set required etc.
    if(meth_args.UpdateFromMethod(method)) { // changed
      if(taMisc::gui_active) {
	tabMisc::DelayedFunCall_gui(this, "BrowserExpandAll");
      }
    }
  }
}

void MethodCall::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  CheckError(!obj, quiet, rval, "obj is NULL");
  CheckProgVarRef(result_var, quiet, rval);
  CheckProgVarRef(obj, quiet, rval);
  CheckError(!method, quiet, rval, "method is NULL");
}

void MethodCall::CheckChildConfig_impl(bool quiet, bool& rval) {
  inherited::CheckChildConfig_impl(quiet, rval);
  meth_args.CheckConfig(quiet, rval);
}

const String MethodCall::GenCssBody_impl(int indent_level) {
  String rval;
  rval += cssMisc::Indent(indent_level);
  if (!((bool)obj && method)) {
    rval += "//WARNING: MethodCall not generated here -- obj or method not specified\n";
   return rval;
  }
  
  if(result_var)
    rval += result_var->name + " = ";
  rval += obj->name;
  rval += "->";
  rval += method->name;
  rval += meth_args.GenCssBody_impl(indent_level);
  rval += ";\n";
  
  return rval;
}

String MethodCall::GetDisplayName() const {
  if (!obj || !method)
    return "(object or method not selected)";
  
  String rval;
  if(result_var)
    rval += result_var->name + "=";
  rval += obj->name;
  rval += "->";
  rval += method->name;
  rval += "(";
  for(int i=0;i<meth_args.size;i++) {
    ProgArg* pa = meth_args[i];
    if (i > 0)
      rval += ", ";
    rval += pa->GetDisplayName();
  }
  rval += ")";
  return rval;
}

//////////////////////////
//    MemberProgEl	//
//////////////////////////

bool MemberProgEl::ShowVarFilter(void* var_) {
  ProgVar* var = static_cast<ProgVar*>(var_);
  return (var->var_type == ProgVar::T_Object);
}

void MemberProgEl::Initialize() {
  obj_type = &TA_taBase; // placeholder
  member_lookup = NULL;
}

void MemberProgEl::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  if(member_lookup) {
    if(!path.empty() && (path.lastchar() != '.')) {
      if(path.contains('.')) {
	path = path.through('.',-1);
      }
      else {
	path = "";
      }
    }
    path += member_lookup->name;
    if(member_lookup->type->InheritsFormal(&TA_class) &&
       !member_lookup->type->InheritsFrom(&TA_taString))
      path += ".";
    member_lookup = NULL;
  }

  GetTypeFromPath();

  UpdateProgVarRef_NewOwner(obj);
}

bool MemberProgEl::GetTypeFromPath(bool quiet) {
  if(!obj) {
    obj_type = &TA_taBase; // placeholder
    return false;
  }
  TypeDef* ot = obj->act_object_type();
  MemberDef* md = TypeDef::FindMemberPathStatic(ot, path, false);
  // gets static path based just on member types, updates ot to point to owner type of md
  obj_type = ot;
  return (bool)md;
}

void MemberProgEl::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  CheckError(!obj, quiet, rval, "obj is NULL");
  CheckProgVarRef(obj, quiet, rval);
  CheckError(path.empty(), quiet, rval, "path is empty");
}

//////////////////////////
//    MemberAssign	//
//////////////////////////

void MemberAssign::Initialize() {
  update_after = false;
}

void MemberAssign::CheckChildConfig_impl(bool quiet, bool& rval) {
  inherited::CheckChildConfig_impl(quiet, rval);
  expr.CheckConfig(quiet, rval);
}

const String MemberAssign::GenCssBody_impl(int indent_level) {
  expr.ParseExpr();		// re-parse just to be sure!
  STRING_BUF(rval, 80);
  rval += cssMisc::Indent(indent_level);
  if (!(bool)obj || path.empty() || expr.empty()) {
    rval += "//WARNING: MemberAssign not generated here -- obj or path not specified or expr empty\n";
   return rval;
  }
  
  rval += obj->name + "->" + path + " = ";
  rval += expr.GetFullExpr() + ";\n";
  if (update_after) {
    rval += cssMisc::Indent(indent_level);
    rval += obj->name + "->UpdateAfterEdit();\n";
  }
  return rval;
}

String MemberAssign::GetDisplayName() const {
  if (!obj || path.empty())
    return "(object or path not selected)";
  
  String rval;
  rval = obj->name + "->" + path + " = ";
  rval += expr.GetFullExpr();
  return rval;
}

//////////////////////////
//    MemberFmArg	//
//////////////////////////

void MemberFmArg::Initialize() {
  update_after = true;
  quiet = false;
}

void MemberFmArg::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  CheckError(arg_name.empty(), quiet, rval, "arg_name is empty");
}

const String MemberFmArg::GenCssBody_impl(int indent_level) {
  STRING_BUF(rval, 80);
  String il = cssMisc::Indent(indent_level);
  String il1 = cssMisc::Indent(indent_level+1);
  String il2 = cssMisc::Indent(indent_level+2);
  rval += il;
  if (!(bool)obj || path.empty() || arg_name.empty()) {
    rval += "//WARNING: MemberFmArg not generated here -- obj or path not specified or expr empty\n";
   return rval;
  }

  String flpth = obj->name + "->" + path;
  
  rval += "{ String arg_str = taMisc::FindArgByName(\"" + arg_name + "\");\n";
  rval += il1 + "if(arg_str.nonempty()) {\n";
  rval += il2 + flpth + " = ";
  rval += "arg_str;\n";
  if (update_after) {
    rval += il2 + obj->name + "->UpdateAfterEdit();\n";
  }

  if(!quiet)
    rval += il2 + "taMisc::Info(\"Set " + flpth + " to:\"," + flpth + ");\n";
  rval += il1 + "}\n";
  rval += il + "}\n";
  return rval;
}

String MemberFmArg::GetDisplayName() const {
  if (!obj || path.empty())
    return "(object or path not selected)";
  
  String rval;
  rval = obj->name + "->" + path + " = ";
  rval += "Arg: " + arg_name;
  return rval;
}

//////////////////////////
//    MemberMethodCall	//
//////////////////////////

void MemberMethodCall::Initialize() {
  method = NULL;
}

void MemberMethodCall::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  UpdateProgVarRef_NewOwner(result_var);

//  if(!taMisc::is_loading && method)
  if (method) { // needed to set required etc.
    if(meth_args.UpdateFromMethod(method)) { // changed
      if(taMisc::gui_active) {
	tabMisc::DelayedFunCall_gui(this, "BrowserExpandAll");
      }
    }
  }
}

void MemberMethodCall::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  CheckProgVarRef(result_var, quiet, rval);
  CheckError(!method, quiet, rval, "method is NULL");
}

void MemberMethodCall::CheckChildConfig_impl(bool quiet, bool& rval) {
  inherited::CheckChildConfig_impl(quiet, rval);
  meth_args.CheckConfig(quiet, rval);
}

const String MemberMethodCall::GenCssBody_impl(int indent_level) {
  String rval;
  rval += cssMisc::Indent(indent_level);
  if (!((bool)obj && method)) {
    rval += "//WARNING: MemberMethodCall not generated here -- obj or method not specified\n";
   return rval;
  }
  
  if(result_var)
    rval += result_var->name + " = ";
  rval += obj->name + "->" + path + "->";
  rval += method->name;
  rval += meth_args.GenCssBody_impl(indent_level);
  rval += ";\n";
  
  return rval;
}

String MemberMethodCall::GetDisplayName() const {
  if (!obj || !method)
    return "(object or method not selected)";
  
  String rval;
  if(result_var)
    rval += result_var->name + "=";
  rval += obj->name + "->" + path + "->";
  rval += method->name;
  rval += "(";
  for(int i=0;i<meth_args.size;i++) {
    ProgArg* pa = meth_args[i];
    if (i > 0)
      rval += ", ";
    rval += pa->GetDisplayName();
  }
  rval += ")";
  return rval;
}

//////////////////////////
//      MathCall	//
//////////////////////////

void MathCall::Initialize() {
  min_type = &TA_taMath;
  object_type = &TA_taMath;
}

void RandomCall::Initialize() {
  min_type = &TA_Random;
  object_type = &TA_Random;
}

void MiscCall::Initialize() {
  min_type = &TA_taMisc;
  object_type = &TA_taMisc;
}

//////////////////////////
//      PrintVar	//
//////////////////////////

void PrintVar::Initialize() {
}

void PrintVar::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  UpdateProgVarRef_NewOwner(print_var);
  UpdateProgVarRef_NewOwner(print_var2);
  UpdateProgVarRef_NewOwner(print_var3);
  UpdateProgVarRef_NewOwner(print_var4);
  UpdateProgVarRef_NewOwner(print_var5);
  UpdateProgVarRef_NewOwner(print_var6);
}

void PrintVar::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  //  CheckError(!print_var, quiet, rval, "print_var is NULL");
  CheckProgVarRef(print_var, quiet, rval);
  CheckProgVarRef(print_var2, quiet, rval);
  CheckProgVarRef(print_var3, quiet, rval);
  CheckProgVarRef(print_var4, quiet, rval);
  CheckProgVarRef(print_var5, quiet, rval);
  CheckProgVarRef(print_var6, quiet, rval);
}

const String PrintVar::GenCssBody_impl(int indent_level) {
  String rval;
  if(message.empty() && !print_var && !print_var2 && !print_var3 && !print_var4 && !print_var5 && !print_var6)
    return _nilString;
  rval += cssMisc::Indent(indent_level) + "cerr ";
  if(message.nonempty()) 
    rval += "<< \"" + message + "\"";
  if((bool)print_var)
    rval += "<< \"  " + print_var->name + " = \" << " + print_var->name;
  if((bool)print_var2)
    rval += "<< \"  " + print_var2->name + " = \" << " + print_var2->name;
  if((bool)print_var3)
    rval += "<< \"  " + print_var3->name + " = \" << " + print_var3->name;
  if((bool)print_var4)
    rval += "<< \"  " + print_var4->name + " = \" << " + print_var4->name;
  if((bool)print_var5)
    rval += "<< \"  " + print_var5->name + " = \" << " + print_var5->name;
  if((bool)print_var6)
    rval += "<< \"  " + print_var6->name + " = \" << " + print_var6->name;
  rval += " << endl;\n";
  return rval;
}

String PrintVar::GetDisplayName() const {
  String rval = "Print:";
  if(message.nonempty())
    rval += " " + message;
  if(print_var)
    rval += " " + print_var->name;
  if((bool)print_var2)
    rval += " " + print_var2->name;
  if((bool)print_var3)
    rval += " " + print_var3->name;
  if((bool)print_var4)
    rval += " " + print_var4->name;
  if((bool)print_var5)
    rval += " " + print_var5->name;
  if((bool)print_var6)
    rval += " " + print_var6->name;
  return rval;
}


//////////////////////////
//      PrintExpr	//
//////////////////////////

void PrintExpr::Initialize() {
}

void PrintExpr::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  expr.CheckConfig(quiet, rval);
}

const String PrintExpr::GenCssBody_impl(int indent_level) {
  expr.ParseExpr();		// re-parse just to be sure!
  String rval;
  rval += cssMisc::Indent(indent_level);
  rval += "cerr << " + expr.GetFullExpr() + " << endl;\n";
  return rval;
}

String PrintExpr::GetDisplayName() const {
  String rval;
  rval += "Print: " + expr.GetFullExpr();
  return rval;
}


//////////////////////////
//      Comment 	//
//////////////////////////

void Comment::Initialize() {
  static String _def_comment("TODO: Add your program comment here (multi-lines ok).\n");
  desc = _def_comment;
}

const String Comment::GenCssBody_impl(int indent_level) {
  STRING_BUF(rval, desc.length() + 160);
  rval += cssMisc::Indent(indent_level);
  rval += "/*******************************************************************\n";
  rval += cssMisc::IndentLines(desc, indent_level);
  rval = trimr(rval);
  if (rval.lastchar() != '\n')
    rval += '\n';
  rval += cssMisc::Indent(indent_level);
  rval += "*******************************************************************/\n";
  return rval;
}

String Comment::GetDisplayName() const {
  return desc;
}


//////////////////////////
//      StopStepPoint 	//
//////////////////////////

void StopStepPoint::Initialize() {
}

const String StopStepPoint::GenCssBody_impl(int indent_level) {
  String rval;
  rval += cssMisc::Indent(indent_level) + "StopCheck(); // check for Stop or Step button\n";
  return rval;
}

String StopStepPoint::GetDisplayName() const {
  return "Stop/Step Point";
}


//////////////////////////
//    ReturnExpr	//
//////////////////////////

void ReturnExpr::Initialize() {
}

void ReturnExpr::CheckChildConfig_impl(bool quiet, bool& rval) {
  inherited::CheckChildConfig_impl(quiet, rval);
  expr.CheckConfig(quiet, rval);
}

const String ReturnExpr::GenCssBody_impl(int indent_level) {
  expr.ParseExpr();		// re-parse just to be sure!
  String rval;
  rval += cssMisc::Indent(indent_level);
  rval += "return " + expr.GetFullExpr() + ";\n";
  return rval;
}

String ReturnExpr::GetDisplayName() const {
  String rval;
  rval += "return " + expr.expr;
  return rval;
}


///////////////////////////////////////////////////////
//		OtherProgramVar
///////////////////////////////////////////////////////


void OtherProgramVar::Initialize() {
  set_other = false;
}

void OtherProgramVar::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  UpdateProgVarRef_NewOwner(var_1);
  UpdateProgVarRef_NewOwner(var_2);
  UpdateProgVarRef_NewOwner(var_3);
  UpdateProgVarRef_NewOwner(var_4);
}

void OtherProgramVar::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  CheckError(!other_prog, quiet, rval, "other_prog is NULL");
  CheckProgVarRef(var_1, quiet, rval);
  CheckProgVarRef(var_2, quiet, rval);
  CheckProgVarRef(var_3, quiet, rval);
  CheckProgVarRef(var_4, quiet, rval);
}

String OtherProgramVar::GetDisplayName() const {
  String rval;
  if(set_other)
    rval = "To: ";
  else
    rval = "Fm: ";
  if(other_prog)
    rval += other_prog->name;
  else
    rval += "(ERROR: other_prog not set!)";
  if(set_other)
    rval += " Fm Vars: ";
  else
    rval += " To Vars: ";
  if(var_1) rval += var_1->name + " ";
  if(var_2) rval += var_2->name + " ";
  if(var_3) rval += var_3->name + " ";
  if(var_4) rval += var_4->name + " ";
  return rval;
}

Program* OtherProgramVar::GetOtherProg() {
  if(!other_prog) {
    taMisc::CheckError("Other program is NULL in OtherProgramVar:",
		       desc, "in program:", program()->name);
  }
  return other_prog.ptr();
}

bool OtherProgramVar::GenCss_OneVar(String& rval, ProgVarRef& var,
				    const String& il, int var_no) {
  if(!var) return false;
  if(set_other)
    rval += il + "other_prog->SetVar(\"" + var->name + "\", " + var->name +");\n";
  else
    rval += il + var->name + " = other_prog->GetVar(\"" + var->name + "\");\n";
  return true;
}

const String OtherProgramVar::GenCssPre_impl(int indent_level) {
  String rval;
  rval = cssMisc::Indent(indent_level);
  rval += "{ // other program var: "; 
  if (other_prog)
    rval += other_prog->name;
  rval += "\n";
  return rval;
}

const String OtherProgramVar::GenCssBody_impl(int indent_level) {
  if (!other_prog) return _nilString;
  String il = cssMisc::Indent(indent_level);
  String rval;
  rval += il + "Program* other_prog = this" + GetPath(NULL, program())+ "->GetOtherProg();\n";
  GenCss_OneVar(rval, var_1, il, 0);
  GenCss_OneVar(rval, var_2, il, 1);
  GenCss_OneVar(rval, var_3, il, 2);
  GenCss_OneVar(rval, var_4, il, 3);
  return rval;
}

const String OtherProgramVar::GenCssPost_impl(int indent_level) {
  return cssMisc::Indent(indent_level) + "} // other program var\n";
}

///////////////////////////////////////////////////////
//		ProgVarFmArg
///////////////////////////////////////////////////////


void ProgVarFmArg::Initialize() {
}

void ProgVarFmArg::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  if(arg_name.empty())
    arg_name = var_name;
  if(var_name.empty())
    var_name = arg_name;
}

void ProgVarFmArg::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  CheckError(!prog, quiet, rval, "prog is NULL");
  CheckError(var_name.empty(), quiet, rval, "var_name is empty");
  CheckError(arg_name.empty(), quiet, rval, "arg_name is empty");
}

String ProgVarFmArg::GetDisplayName() const {
  String rval;
  rval = "Set Var: " + var_name + " in: ";
  if(prog)
    rval += prog->name;
  else
    rval += "(ERROR: prog not set!)";
  rval += " Fm Arg: " + arg_name;
  return rval;
}

Program* ProgVarFmArg::GetOtherProg() {
  if(!prog) {
    taMisc::CheckError("Program is NULL in ProgVarFmArg:",
		       desc, "in program:", program()->name);
  }
  return prog.ptr();
}

const String ProgVarFmArg::GenCssBody_impl(int indent_level) {
  if (!prog) return _nilString;
  String il = cssMisc::Indent(indent_level);
  String rval = il + "{ // prog var fm arg: " + prog->name;
  rval += "\n";
  rval += il + "  Program* other_prog = this" + GetPath(NULL, program())+ "->GetOtherProg();\n";
  rval += il + "  other_prog->SetVarFmArg(\"" + arg_name + "\", \"" + var_name + "\");\n";
  rval += il + "} // prog var fm arg\n";
  return rval;
}


///////////////////////////////////////////////////////
//		RegisterArgs
///////////////////////////////////////////////////////


void RegisterArgs::Initialize() {
}

String RegisterArgs::GetDisplayName() const {
  String rval = "Register Args";
  return rval;
}

const String RegisterArgs::GenCssBody_impl(int indent_level) {
  String rval;
  Program* prog = program();
  if(!prog) return rval;
  rval += cssMisc::Indent(indent_level) + "// Register Args:\n";
  AddArgsFmCode(rval, prog->prog_code, indent_level);
  rval += cssMisc::Indent(indent_level) + "taMisc::UpdateArgs();\n";
  return rval;
}

void RegisterArgs::AddArgsFmCode(String& gen_code, ProgEl_List& progs, int indent_level) {
  String il = cssMisc::Indent(indent_level);
  for(int i=0;i<progs.size;i++) {
    ProgEl* pel = progs[i];
    if(pel->InheritsFrom(&TA_ProgVarFmArg)) {
      ProgVarFmArg* pva = (ProgVarFmArg*)pel;
      gen_code += il + "taMisc::AddEqualsArgName(\"" + pva->arg_name + "\");\n";
    }
    else if(pel->InheritsFrom(&TA_MemberFmArg)) {
      MemberFmArg* mfa = (MemberFmArg*)pel;
      gen_code += il + "taMisc::AddEqualsArgName(\"" + mfa->arg_name + "\");\n";
    }
    else {			// look for sub-lists
      TypeDef* td = pel->GetTypeDef();
      for(int j=0;j<td->members.size;j++) {
	MemberDef* md = td->members[j];
	if(md->type->InheritsFrom(&TA_ProgEl_List)) {
	  ProgEl_List* nxt_prgs = (ProgEl_List*)md->GetOff(pel);
	  AddArgsFmCode(gen_code, *nxt_prgs, indent_level);
	}
      }
    }
  }
}

