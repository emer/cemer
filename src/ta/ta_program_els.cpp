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

const String CodeBlock::GenCssBody_impl(int indent_level) {
  return prog_code.GenCss(indent_level);
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

void UserScript::Initialize() {
  static String _def_user_script("// TODO: Add your CSS script code here.\n");
  script.expr = _def_user_script;
  script.SetExprFlag(ProgExpr::NO_VAR_ERRS); // don't report bad variable errors
}

const String UserScript::GenCssBody_impl(int indent_level) {
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
  String rval = cssMisc::Indent(indent_level) + "} while (" + test.GetFullExpr() + ");\n";
  return rval;
}

String DoLoop::GetDisplayName() const {
  return "do ... while (" + test.expr + ")";
}

//////////////////////////
//  ForLoop		//
//////////////////////////

void ForLoop::Initialize() {
  // the following are just default examples for the user
  init.expr = "int i = 0";
  test.expr = "i < 10";
  iter.expr = "i++";
  init.SetExprFlag(ProgExpr::NO_VAR_ERRS); // don't report bad variable errors
  test.SetExprFlag(ProgExpr::NO_VAR_ERRS); // don't report bad variable errors
  iter.SetExprFlag(ProgExpr::NO_VAR_ERRS); // don't report bad variable errors
}

void ForLoop::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  CheckError(test.expr.empty(), quiet, rval, "test expression is empty");
  CheckError(iter.expr.empty(), quiet, rval, "iter expression is empty");
}

const String ForLoop::GenCssPre_impl(int indent_level) {
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
  String rval;
  rval = cssMisc::Indent(indent_level) + 
    "if(" + cond.GetFullExpr() + ") return;\n";
  return rval; 
}

String IfReturn::GetDisplayName() const {
  return "if(" + cond.expr + ") return;";
}

//////////////////////////
//  IfElse		//
//////////////////////////

void IfElse::Initialize() {
  //  cond.expr = "true";
}

void IfElse::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  CheckError(cond.expr.empty(), quiet, rval,  "condition expression is empty");
  CheckEqualsError(cond.expr, quiet, rval);
}

void IfElse::CheckChildConfig_impl(bool quiet, bool& rval) {
  inherited::CheckChildConfig_impl(quiet, rval);
  true_code.CheckConfig(quiet, rval);
  false_code.CheckConfig(quiet, rval);
}

const String IfElse::GenCssPre_impl(int indent_level) {
  String rval = cssMisc::Indent(indent_level);
  rval += "if (" + cond.GetFullExpr() + ") {\n";
  return rval; 
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

const String IfElse::GenCssPost_impl(int indent_level) {
  return cssMisc::Indent(indent_level) + "}\n";
}

String IfElse::GetDisplayName() const {
  return "if (" + cond.expr + ")";
}

void IfElse::PreGenChildren_impl(int& item_id) {
  true_code.PreGen(item_id);
  false_code.PreGen(item_id);
}
ProgVar* IfElse::FindVarName(const String& var_nm) const {
  ProgVar* pv = true_code.FindVarName(var_nm);
  if(pv) return pv;
  return false_code.FindVarName(var_nm);
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

  UpdateProgVarRef_NewOwner(result_var);
  UpdateProgVarRef_NewOwner(obj);

//  if(!taMisc::is_loading && method)
  if (method) // needed to set required etc.
    meth_args.UpdateFromMethod(method);
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
//    MemberAssign	//
//////////////////////////

void MemberAssign::Initialize() {
  obj_type = &TA_taBase; // placeholder
  member = NULL;
  update_after = false;
}

void MemberAssign::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  if(obj)
    obj_type = obj->act_object_type();
  else obj_type = &TA_taBase; // placeholder

  UpdateProgVarRef_NewOwner(obj);

//  if(!taMisc::is_loading && member)
}

void MemberAssign::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  CheckError(!obj, quiet, rval, "obj is NULL");
  CheckProgVarRef(obj, quiet, rval);
  CheckError(!member, quiet, rval, "member is NULL");
}

void MemberAssign::CheckChildConfig_impl(bool quiet, bool& rval) {
  inherited::CheckChildConfig_impl(quiet, rval);
  expr.CheckConfig(quiet, rval);
}

const String MemberAssign::GenCssBody_impl(int indent_level) {
  STRING_BUF(rval, 80);
  rval += cssMisc::Indent(indent_level);
  if (!((bool)obj && member) || expr.empty()) {
    rval += "//WARNING: MemberAssign not generated here -- obj or member not specified or expr empty\n";
   return rval;
  }
  
  rval += obj->name;
  rval += "->";
  rval += member->name + " = ";
  rval += expr.GetFullExpr();
  rval += ";\n";
  if (update_after) {
    rval += cssMisc::Indent(indent_level);
    rval += obj->name.cat("->UpdateAfterEdit();\n");
  }
  return rval;
}

String MemberAssign::GetDisplayName() const {
  if (!obj || !member)
    return "(object or member not selected)";
  
  String rval;
  rval += obj->name;
  rval += "->";
  rval += member->name + " = ";
  rval += expr.GetFullExpr();
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
}

void PrintVar::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  CheckError(!print_var, quiet, rval, "print_var is NULL");
  CheckProgVarRef(print_var, quiet, rval);
}

const String PrintVar::GenCssBody_impl(int indent_level) {
  String rval;
  rval += cssMisc::Indent(indent_level);
  if (!print_var) {
    rval += "//WARNING: PrintVar not generated here -- print_var not specified\n";
    return rval;
  }
  
  rval += "cerr << \"" + print_var->name + " = \" << " + print_var->name + " << endl;\n";
  return rval;
}

String PrintVar::GetDisplayName() const {
  if(!print_var)
    return "(print_var not selected)";
  
  String rval;
  rval += "Print: " + print_var->name;
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

