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
#include "ta_datatable.h"

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
  String rval = cssMisc::Indent(indent_level) + "while (" + test.GetFullExpr() + ") {\n";
  if(IsVerbose()) {
    rval += cssMisc::Indent(indent_level+1) + "taMisc::Info(\"starting while(" +
      test.GetFullExpr().quote_esc() + ") loop...\");\n";
  }
  return rval;
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
  if(IsVerbose())
    rval += cssMisc::Indent(indent_level+1) + "taMisc::Info(\"starting do..while(" +
      test.GetFullExpr().quote_esc() + ") loop\");\n";
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

void ForLoop::InitLinks() { 
  inherited::InitLinks(); 
  InitLinks_taAuto(&TA_ForLoop);
  if (taMisc::is_loading || taMisc::is_duplicating) return;
  UpdateOnInsert_impl();
}

void ForLoop::UpdateAfterEdit_impl() {
  SetProgExprFlags();
  inherited::UpdateAfterEdit_impl();
  if(taMisc::is_loading) return;
  Program* prg = GET_MY_OWNER(Program);
  if(!prg || isDestroying() || prg->isDestroying()) return;
  if(init.expr == "_toolbox_tmp_") {
    init.expr = "i = 0";
    UpdateOnInsert_impl();
  }
  bool is_local;
  String loop_var = GetLoopVar(is_local);
  if(!is_local)
    MakeIndexVar(loop_var);	// make sure it exists
}

void ForLoop::UpdateOnInsert_impl() {
  bool is_local;
  String loop_var = GetLoopVar(is_local);
  if (is_local) return; // locals don't clash with above
  bool clashes = ParentForLoopVarClashes(loop_var);
  if (!clashes) return;
  String new_loop_var;
  while (clashes) {
    MorphVar(loop_var);
    clashes = ParentForLoopVarClashes(loop_var);
  }
  ChangeLoopVar(loop_var);
} 

void ForLoop::MorphVar(String& cur_loop_var) {
  char c;
  // if a single letter, then increment, otherwise pick a random letter
  if (cur_loop_var.length() == 1) {
    c = cur_loop_var[0] + 1;
    if (isalpha(c)) {
      cur_loop_var = c;
      return;
    }
  }
  // last resort: something random
  c = 'a' + (char)(rand() % 26);
  cur_loop_var = c;
}

bool ForLoop::ParentForLoopVarClashes(const String& loop_var) {
  ForLoop* outer_loop = GET_MY_OWNER(ForLoop);
  while (outer_loop) {
    bool is_local;
    String outer_loop_var = outer_loop->GetLoopVar(is_local);
    // note: is_local irrelevant because we will still clash
    if (loop_var == outer_loop_var) return true;
    outer_loop = (ForLoop*)outer_loop->GetOwner(&TA_ForLoop);
  }
  return false;
}

String ForLoop::GetLoopVar(bool& is_local) const {
  // note: this heuristic is going to work 99.9% of the time
  // get trimmed part before first =
  String loop_var = trim(init.expr.before("="));
  // there will only be any embedded spaces if there is a type declaration
  is_local = loop_var.contains(" ");
  if (is_local) {
    loop_var = trim(loop_var.after(" "));
  }
  return loop_var;
}
 
void ForLoop::MakeIndexVar(const String& var_nm) {
  if(var_nm.empty()) return;
  Function* my_fun = GET_MY_OWNER(Function);
  
  if(my_fun) {			// use function scope by default
    if(my_fun->FindVarName(var_nm)) return; // all good
    if(my_fun->fun_code.size > 0) {
      ProgEl* fvars = my_fun->fun_code[0]; // first code element is vars by default
      if(fvars->InheritsFrom(&TA_ProgVars)) {
	ProgVar* var = ((ProgVars*)fvars)->AddVar();
	var->name = var_nm;
	var->SetInt(0);
	var->ClearVarFlag(ProgVar::CTRL_PANEL);
	var->DataChanged(DCR_ITEM_UPDATED);
	// get the var ptrs in case someone changes them later!
	init.ParseExpr();
	test.ParseExpr();
	iter.ParseExpr();
	return;
      }
    }
    // fall through if no var in fun
  }

  // default is to use program
  Program* my_prog = GET_MY_OWNER(Program);
  if(!my_prog) return;
  if(!my_prog->vars.FindName(var_nm)) {
    ProgVar* var = (ProgVar*)my_prog->vars.New(1, &TA_ProgVar);
    var->name = var_nm;
    var->SetInt(0);
    var->ClearVarFlag(ProgVar::CTRL_PANEL);
    var->DataChanged(DCR_ITEM_UPDATED);
    // get the var ptrs in case someone changes them later!
    init.ParseExpr();
    test.ParseExpr();
    iter.ParseExpr();
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
  String full_expr = init.GetFullExpr() + "; " + test.GetFullExpr() + "; " + iter.GetFullExpr();
  rval = cssMisc::Indent(indent_level) + 
    "for (" + full_expr + ") {\n";
  if(IsVerbose()) {
    rval += cssMisc::Indent(indent_level+1) + "taMisc::Info(\"starting for(" + 
      full_expr.quote_esc() + ") loop\");\n";
  }
  return rval; 
}

const String ForLoop::GenCssPost_impl(int indent_level) {
  String rval = cssMisc::Indent(indent_level) + "}\n";
  return rval;
}

String ForLoop::GetDisplayName() const {
  return "for (" + init.expr + "; " + test.expr + "; " + iter.expr + ")";
}

void ForLoop::ChangeLoopVar(const String& to_var) {
  bool is_local;
  String fm_var = GetLoopVar(is_local);
  if (fm_var.empty()) return; // TODO: mebe should complain?
  init.expr.gsub(fm_var, to_var);
  test.expr.gsub(fm_var, to_var);
  iter.expr.gsub(fm_var, to_var);
  MakeIndexVar(to_var);      // have to make the new var *before* parsing!!
  // this is possibly redundant with make index var but not always..
  init.ParseExpr();
  test.ParseExpr();
  iter.ParseExpr();
  UpdateAfterEdit();
}


//////////////////////////
//  IfContinue		//
//////////////////////////

void IfContinue::Initialize() {
}

void IfContinue::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
//   CheckError(cond.expr.empty(), quiet, rval,  "condition expression is empty");
  CheckEqualsError(cond.expr, quiet, rval);
}

const String IfContinue::GenCssBody_impl(int indent_level) {
  cond.ParseExpr();		// re-parse just to be sure!
  String rval;
  rval = cssMisc::Indent(indent_level);
  String fexp = cond.GetFullExpr();
  if(fexp.nonempty()) rval += "if(" + fexp + ") ";
  if(IsVerbose())
    rval += "{ taMisc::Info(\"if(" + fexp.quote_esc() + ") == true, continuing\"); continue; }\n";
  else
    rval += "continue;\n";
  return rval; 
}

String IfContinue::GetDisplayName() const {
  if(cond.expr.empty())
    return "continue;";
  else
    return "if(" + cond.expr + ") continue;";
}


//////////////////////////
//  IfBreak		//
//////////////////////////

void IfBreak::Initialize() {
}

void IfBreak::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
//   CheckError(cond.expr.empty(), quiet, rval,  "condition expression is empty");
  CheckEqualsError(cond.expr, quiet, rval);
}

const String IfBreak::GenCssBody_impl(int indent_level) {
  cond.ParseExpr();		// re-parse just to be sure!
  String rval;
  rval = cssMisc::Indent(indent_level);
  String fexp = cond.GetFullExpr();
  if(fexp.nonempty()) rval += "if(" + fexp + ") ";
  if(IsVerbose())
    rval += "{ taMisc::Info(\"if=true, breaking\"); break; }\n";
  else
    rval += "break;\n";
  return rval; 
}

String IfBreak::GetDisplayName() const {
  if(cond.expr.empty())
    return "break;";
  else
    return "if(" + cond.expr + ") break;";
}

//////////////////////////
//  IfReturn		//
//////////////////////////

void IfReturn::Initialize() {
}

void IfReturn::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
//   CheckError(cond.expr.empty(), quiet, rval,  "condition expression is empty");
  CheckEqualsError(cond.expr, quiet, rval);
}

const String IfReturn::GenCssBody_impl(int indent_level) {
  cond.ParseExpr();		// re-parse just to be sure!
  String rval;
  rval = cssMisc::Indent(indent_level);
  String fexp = cond.GetFullExpr();
  if(fexp.empty()) {
    rval += "return;\n";
  }
  else {
    rval += "if(" + fexp + ") ";
    if(IsVerbose())
      rval += "{ taMisc::Info(\"if(" + fexp.quote_esc() + ") == true, returning\"); return; }\n";
    else
      rval += "return;\n";
  }
  return rval; 
}

String IfReturn::GetDisplayName() const {
  if(cond.expr.empty())
    return "return;";
  else
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
  if(IsVerbose())
    rval += cssMisc::Indent(indent_level+1) + "taMisc::Info(\"if("
      + cond.GetFullExpr().quote_esc() + ") == true...\");\n";
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
    if(IsVerbose())
      rval += cssMisc::Indent(indent_level+1) + "taMisc::Info(\"if(" + 
	cond.GetFullExpr().quote_esc() + ") == false, in else...\");\n";
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
  //  CheckError(case_val.empty(), quiet, rval,  "case value expression is empty!");
}

const String CaseBlock::GenCssPre_impl(int indent_level) {
  case_val.ParseExpr();		// re-parse just to be sure!
  if(prog_code.size == 0) return _nilString;
  String il = cssMisc::Indent(indent_level);
  String expr = case_val.GetFullExpr();
  String rval;
  if(expr.empty())
    rval = il + "default: {\n";
  else
    rval = il + "case " + case_val.GetFullExpr() + ": {\n";
  if(IsVerbose())
    rval += cssMisc::Indent(indent_level+1) + "taMisc::Info(\"in case " +
      case_val.GetFullExpr().quote_esc() + "...\");\n";
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
  if(case_val.expr.empty()) return "default: (" + String(prog_code.size) + " items)";
  return "case: " + case_val.expr + " (" + String(prog_code.size) + " items)";
}

//////////////////////////
//  Switch		//
//////////////////////////

void Switch::Initialize() {
  cases.SetBaseType(&TA_CaseBlock);
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
  if(IsVerbose())
    rval += cssMisc::Indent(indent_level+1) + "taMisc::Info(\"in switch(" +
      switch_var->name + ")...\");\n";
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

void AssignExpr::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  CheckError(!result_var, quiet, rval, "result_var is NULL");
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
  if(IsVerbose())
    rval += cssMisc::Indent(indent_level) + "taMisc::Info(\"assigned " + result_var->name
      + " = " + expr.GetFullExpr().quote_esc() + "; now = \", " + result_var->name + ");\n";
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

void VarIncr::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  CheckError(!var, quiet, rval, "var is NULL");
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
  if(IsVerbose())
    rval += cssMisc::Indent(indent_level) + "taMisc::Info(\"incremented " + var->name
      + " += " + expr.GetFullExpr().quote_esc() + "; now = \", " + var->name + ");\n";
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

void MethodCall::Initialize() {
  method = NULL;
  obj_type = &TA_taBase; // placeholder
}

void MethodCall::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  if(obj)
    obj_type = obj->act_object_type();
  else obj_type = &TA_taBase; // placeholder

//  if(!taMisc::is_loading && method)
  if (method) { // needed to set required etc.
    if(meth_args.UpdateFromMethod(method)) { // changed
      if(taMisc::gui_active) {
	tabMisc::DelayedFunCall_gui(this, "BrowserExpandAll");
      }
    }
    meth_sig = method->prototype();
    meth_desc = method->desc;
  } else {
    meth_sig = _nilString;
    meth_desc = _nilString;
  }
}

void MethodCall::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  CheckError(!obj, quiet, rval, "obj is NULL");
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
  
  if(IsVerbose()) {
    String argstmp = meth_args.GenCssBody_impl(indent_level);
    rval += cssMisc::Indent(indent_level) + "taMisc::Info(\"calling method " +
      obj->name + "->" + method->name + argstmp.quote_esc() + "\");\n";
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
    rval += pa->expr.expr; // GetDisplayName();
  }
  rval += ")";
  return rval;
}

void MethodCall::Help() {
  if(obj && (bool)obj->object_val) {
    obj->object_val->Help();
    // todo: add check for method and select that in class browser guy..
  }
  else {
    inherited::Help();
  }
}

/*const String MethodCall::statusTip(const KeyString& ks) const {
  if (method)
    return method->prototype();
  else return inherited::statusTip(ks);
}*/


//////////////////////////
//    MemberProgEl	//
//////////////////////////

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
}

// StringFieldLookupFun is in ta_program_qt.cpp

bool MemberProgEl::GetTypeFromPath(bool quiet) {
  if(!obj) {
    obj_type = &TA_taBase; // placeholder
    return false;
  }
  TypeDef* ot = obj->act_object_type();
  taBase* base_base = obj->object_val;
  MemberDef* md = NULL;
  bool rval = false;
  if(base_base) {
    taBase* mb_tab = base_base->FindFromPath(path, md);
    if(mb_tab) {
      obj_type = mb_tab->GetTypeDef();
      rval = true;
    }
  }
  else {
    int net_base_off = 0;
    ta_memb_ptr net_mbr_off = 0;
    md = TypeDef::FindMemberPathStatic(ot, net_base_off, net_mbr_off, path, false);
    // gets static path based just on member types, updates ot to point to owner type of md
    obj_type = ot;
    rval = (bool)md;
  }
  return rval;
}

void MemberProgEl::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  CheckError(!obj, quiet, rval, "obj is NULL");
  CheckError(path.empty(), quiet, rval, "path is empty");
}

void MemberProgEl::Help() {
  if(obj && (bool)obj->object_val) {
    obj->object_val->Help();
    // todo: add check for member and select that in class browser guy..
  }
  else {
    inherited::Help();
  }
}

//////////////////////////
//    MemberAssign	//
//////////////////////////

void MemberAssign::Initialize() {
  update_after = false;
}

void MemberAssign::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  if((bool)obj && (bool)obj->object_val && expr.empty()) { // assume ok to set default here based on obj
    update_after = obj->object_val->UAEProgramDefault();
  }
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
  if(IsVerbose())
    rval += cssMisc::Indent(indent_level) + "taMisc::Info(\"assigned " + obj->name + "->" + path
      + " = " + expr.GetFullExpr().quote_esc() + "; now = \", " +
      obj->name + "->" + path + ");\n";
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
  prv_obj = NULL;
}

void MemberFmArg::Copy_(const MemberFmArg& cp) {
  MemberFmArg::StatTypeDef(0)->CopyOnlySameType((void*)this, (void*)&cp);
  UpdateAfterCopy(cp);
  prv_obj = obj.ptr();
}

void MemberFmArg::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  if(prv_obj && (bool)obj && (obj.ptr() != prv_obj)) {
    if((bool)obj->object_val && (bool)prv_obj->object_val &&
       !obj->object_val->InheritsFrom(prv_obj->object_val->GetTypeDef())) {
      path = "";		// reset path
      arg_name = "";
    }
  }
  prv_obj = obj.ptr();
  if(arg_name.empty() && path.nonempty()) {
    if(path.contains("."))
      arg_name = path.after(".",-1);
    else
      arg_name = path;
  }
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

  if(!quiet || IsVerbose())
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
  
  if(IsVerbose()) {
    String argstmp = meth_args.GenCssBody_impl(indent_level);
    rval += cssMisc::Indent(indent_level) + "taMisc::Info(\"calling method " +
      obj->name + "->" + path + "->" + method->name + argstmp.quote_esc() + "\");\n";
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
    rval += pa->expr.expr;   // GetDisplayName();
  }
  rval += ")";
  return rval;
}

//////////////////////////
//      MathCall	//
//////////////////////////

void MathCall::Initialize() {
  min_type = &TA_taMath;
  object_type = &TA_taMath_float;
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

void PrintVar::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  //  CheckError(!print_var, quiet, rval, "print_var is NULL");
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

void StopStepPoint::InitLinks() { 
  inherited::InitLinks(); 
  InitLinks_taAuto(&TA_StopStepPoint);
  if (taMisc::is_loading || taMisc::is_duplicating) return;
  Program* prg = GET_MY_OWNER(Program);
  if(!prg || isDestroying() || prg->isDestroying()) return;
  prg->SetProgFlag(Program::SHOW_STEP);
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
  if(IsVerbose())
    rval += cssMisc::Indent(indent_level) + "taMisc::Info(\"returning value: "
      + expr.GetFullExpr().quote_esc() + ");\n";

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

void OtherProgramVar::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  CheckError(!other_prog, quiet, rval, "other_prog is NULL");
  if(other_prog) {
    ProgVar* pv = NULL;
    if(var_1) {
      pv = other_prog->FindVarName(var_1->name);
      CheckError(!pv, quiet, rval, "Could not find variable named:", var_1->name,
		 "in program:", other_prog->name, "path:", other_prog->GetPath_Long());
    }
    if(var_2) {
      pv = other_prog->FindVarName(var_2->name);
      CheckError(!pv, quiet, rval, "Could not find variable named:", var_2->name,
		 "in program:", other_prog->name, "path:", other_prog->GetPath_Long());
    }
    if(var_3) {
      pv = other_prog->FindVarName(var_3->name);
      CheckError(!pv, quiet, rval, "Could not find variable named:", var_3->name,
		 "in program:", other_prog->name, "path:", other_prog->GetPath_Long());
    }
    if(var_4) {
      pv = other_prog->FindVarName(var_4->name);
      CheckError(!pv, quiet, rval, "Could not find variable named:", var_4->name,
		 "in program:", other_prog->name, "path:", other_prog->GetPath_Long());
    }
  }
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
  TestError(!other_prog, "GetOtherProg", "Other program is NULL in OtherProgramVar:",
	    desc, "in program:", program()->name);
  return other_prog.ptr();
}

bool OtherProgramVar::GenCss_OneVar(String& rval, ProgVarRef& var,
				    const String& il, int var_no) {
  if(!var) return false;
  if(set_other)
    rval += il + "other_prog->SetVar(\"" + var->name + "\", " + var->name +");\n";
  else
    rval += il + var->name + " = other_prog->GetVar(\"" + var->name + "\");\n";
  if(IsVerbose()) {
    if(set_other)
      rval += il + "taMisc::Info(\"assigned var " + var->name
	+ " in other program: " + other_prog->name + " to same variable in this program, which has value:\"," + var->name + ");\n";
    else
      rval += il + "taMisc::Info(\"assigned var " + var->name
	+ " in this program to same variable in other program: " + other_prog->name + " -- now has value:\"," + var->name + ");\n";
  }
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
  String il = cssMisc::Indent(indent_level+1);
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

  if((bool)prog && var_name.nonempty()) {
    ProgVar* pv = prog->FindVarName(var_name);
    CheckError(!pv, quiet, rval, "Could not find variable named:", var_name, "in program:",
	       prog->name, "path:", prog->GetPath_Long());
  }
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
//		DataColsFmArgs
///////////////////////////////////////////////////////

void DataColsFmArgs::Initialize() {
  row_spec = CUR_ROW;
}

void DataColsFmArgs::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
}

void DataColsFmArgs::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  if(CheckError(!data_var, quiet, rval, "data_var is NULL")) return; // fatal
  // should be done by var, not us
  //  CheckError(!data_var->object_val, quiet, rval, "data_var variable is NULL");
  CheckError(data_var->object_type != &TA_DataTable, quiet, rval,
	     "data_var variable does not point to a DataTable object");
  CheckError(row_spec != CUR_ROW && !row_var, quiet, rval, "row_var is NULL but is required!");
}

String DataColsFmArgs::GetDisplayName() const {
  String rval = "Data Cols Fm Args";
  DataTable* dt = GetData();
  if(dt) {
    rval += " To: " + dt->name;
  }
  String row_var_name = "(ERR: not set!)";
  if((bool)row_var)
    row_var_name = row_var->name;
  if(row_spec  == CUR_ROW)
    rval += " cur_row";
  else if(row_spec == ROW_NUM)
    rval += " row_num: " + row_var_name;
  else
    rval += " row_val: " + row_var_name;
  return rval;
}

DataTable* DataColsFmArgs::GetData() const {
  if(!data_var) return NULL;
  if(data_var->object_type != &TA_DataTable) return NULL;
  return (DataTable*)data_var->object_val.ptr();
}

const String DataColsFmArgs::GenCssBody_impl(int indent_level) {
  String il = cssMisc::Indent(indent_level);
  String il1 = cssMisc::Indent(indent_level+1);
  String il2 = cssMisc::Indent(indent_level+2);
  DataTable* dt = GetData();
  if(!dt) return il + "// DataColsFmArgs: data_var not set!\n";
  String rval = il + "{ // DataColsFmArgs fm: " + dt->name + "\n";
  rval += il1 + "String dcfma_colnm, dcfma_argval;\n";
  rval += il1 + "for(int j=0;j<" + dt->name + ".cols();j++) {\n";
  rval += il2 + "dcfma_colnm = " + dt->name + ".data[j].name;\n";
  rval += il2 + "dcfma_argval = taMisc::FindArgByName(dcfma_colnm);\n";
  rval += il2 + "if(dcfma_argval.empty()) continue;\n";
  if(row_spec == CUR_ROW) {
    rval += il2 + data_var->name + ".SetDataByName(dcfma_argval, dcfma_colnm);\n";
  }
  else if(row_spec == ROW_NUM) {
    rval +=  il2 + data_var->name + ".SetValColName(dcfma_argval, dcfma_colnm, "
      + row_var->name + ");\n";
  }
  else if(row_spec == ROW_VAL) {
    rval +=  il2 + data_var->name + ".SetValColRowName(dcfma_argval, dcfma_colnm, \""
      + row_var->name + "\", " + row_var->name + ");\n";
  }
  if(taMisc::dmem_proc == 0) {
    rval += il2 + "cerr << \"Set column: \" << dcfma_colnm << \" in data table: \" << \"" +
      dt->name + "\" << \" to val: \" << dcfma_argval << endl;\n";
  }
  rval += il1 + "}\n";
  rval += il + "}\n";
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
  rval += "if(taMisc::CheckArgByName(\"Help\")) taMisc::HelpMsg();\n"; // extra help!
  return rval;
}

void RegisterArgs::AddArgsFmCode(String& gen_code, ProgEl_List& progs, int indent_level) {
  String il = cssMisc::Indent(indent_level);
  for(int i=0;i<progs.size;i++) {
    ProgEl* pel = progs[i];
    if(pel->InheritsFrom(&TA_ProgVarFmArg)) {
      ProgVarFmArg* pva = (ProgVarFmArg*)pel;
      gen_code += il + "taMisc::AddEqualsArgName(\"" + pva->arg_name + "\");\n";
      gen_code += il + "taMisc::AddArgNameDesc(\"" + pva->arg_name
	+ "\", \"ProgVarFmArg: prog = " + (((bool)pva->prog) ? pva->prog->name : "NOT SET")
	+ " var_name = " + pva->var_name + "\");\n";
    }
    else if(pel->InheritsFrom(&TA_MemberFmArg)) {
      MemberFmArg* mfa = (MemberFmArg*)pel;
      gen_code += il + "taMisc::AddEqualsArgName(\"" + mfa->arg_name + "\");\n";
      gen_code += il + "taMisc::AddArgNameDesc(\"" + mfa->arg_name
	+ "\", \"MemberFmArg: obj = " + (((bool)mfa->obj) ? mfa->obj->name : "NOT SET")
	+ " path = " + mfa->path + "\");\n";
    }
    else if(pel->InheritsFrom(&TA_DataColsFmArgs)) {
      DataColsFmArgs* mfa = (DataColsFmArgs*)pel;
      DataTable* dt = mfa->GetData();
      if(dt) {
	for(int j=0;j<dt->cols();j++) {
	  DataCol* dc = dt->data[j];
	  gen_code += il + "taMisc::AddEqualsArgName(\"" + dc->name + "\");\n";
	  gen_code += il + "taMisc::AddArgNameDesc(\"" + dc->name
	    + "\", \"DataColsFmArgs: data_table = " + dt->name + "\");\n";
	}
      }
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

