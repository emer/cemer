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
#include "ta_seledit.h"
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

void CodeBlock::GenCssPre_impl(Program* prog) {
  if(prog_code.size == 0) return;
  prog->AddLine(this, "{", ProgLine::MAIN_LINE);
  prog->IncIndent();
}

void CodeBlock::GenCssBody_impl(Program* prog) {
  if(prog_code.size == 0) return;
  prog_code.GenCss(prog);
}

void CodeBlock::GenCssPost_impl(Program* prog) {
  if(prog_code.size == 0) return;
  prog->DecIndent();
  prog->AddLine(this, "}");
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

bool CodeBlock::CanCvtFmCode(const String& code, ProgEl* scope_el) const {
  if(code.startsWith("{")) return true;
  return false;
}

bool CodeBlock::CvtFmCode(const String& code) {
  // just open bracket is all there is!
  return true;
}

//////////////////////////
//  LocalVars		//
//////////////////////////

void LocalVars::Initialize() {
}

void LocalVars::Destroy() {
  CutLinks();
}

void LocalVars::CheckChildConfig_impl(bool quiet, bool& rval) {
  inherited::CheckChildConfig_impl(quiet, rval);
  local_vars.CheckConfig(quiet, rval);
}

void LocalVars::GenCssBody_impl(Program* prog) {
  if(local_vars.size > 0) {
    prog->AddLine(this, "// local variables", ProgLine::MAIN_LINE); // best we've got
    local_vars.GenCss_ProgVars(prog);
  }
}

const String LocalVars::GenListing_children(int indent_level) {
  return local_vars.GenListing(indent_level + 1);
}

String LocalVars::GetDisplayName() const {
  String rval;
  rval += "LocalVars (";
  rval += String(local_vars.size);
  rval += " vars)";
  return rval;
}

ProgVar* LocalVars::FindVarName(const String& var_nm) const {
  return local_vars.FindName(var_nm);
}

ProgVar* LocalVars::AddVar() {
  return (ProgVar*)local_vars.New_gui(1);
}

ProgVar* LocalVars::AddFloatMatrix() {
  ProgVar* rval = (ProgVar*)local_vars.New_gui(1);
  rval->SetObjectType(&TA_float_Matrix);
  rval->ClearVarFlag(ProgVar::NULL_CHECK);
  return rval;
}
ProgVar* LocalVars::AddDoubleMatrix() {
  ProgVar* rval = (ProgVar*)local_vars.New_gui(1);
  rval->SetObjectType(&TA_double_Matrix);
  rval->ClearVarFlag(ProgVar::NULL_CHECK);
  return rval;
}
ProgVar* LocalVars::AddIntMatrix() {
  ProgVar* rval = (ProgVar*)local_vars.New_gui(1);
  rval->SetObjectType(&TA_int_Matrix);
  rval->ClearVarFlag(ProgVar::NULL_CHECK);
  return rval;
}
ProgVar* LocalVars::AddStringMatrix() {
  ProgVar* rval = (ProgVar*)local_vars.New_gui(1);
  rval->SetObjectType(&TA_String_Matrix);
  rval->ClearVarFlag(ProgVar::NULL_CHECK);
  return rval;
}
ProgVar* LocalVars::AddVarMatrix() {
  ProgVar* rval = (ProgVar*)local_vars.New_gui(1);
  rval->SetObjectType(&TA_Variant_Matrix);
  rval->ClearVarFlag(ProgVar::NULL_CHECK);
  return rval;
}

bool LocalVars::CanCvtFmCode(const String& code, ProgEl* scope_el) const {
  if(!code.contains(' ')) return false; // must have at least one space
  String vartyp = trim(code.before(' '));
  if(vartyp.endsWith('*')) vartyp = vartyp.before('*',-1);
  TypeDef* td = taMisc::types.FindName(vartyp);
  if(td != NULL) return true;	// yep.
  return false;
}

bool LocalVars::CvtFmCode(const String& code) {
  String vartyp = trim(code.before(' '));
  if(vartyp.endsWith('*')) vartyp = vartyp.before('*',-1);
  String varnm = trim(code.after(' '));
  if(varnm.endsWith(';')) varnm = varnm.before(';',-1);
  TypeDef* td = taMisc::types.FindName(vartyp);
  if(td == NULL) return false; // shouldn't happen
  ProgVar* var = AddVar();
  var->SetName(varnm);
  if(vartyp == "int")
    var->SetInt(0);
  else if((vartyp == "float") || (vartyp == "double"))
    var->SetReal(0.0);
  else if(vartyp == "String")
    var->SetString("");
  else if(vartyp == "bool")
    var->SetBool(false);
  else {
    var->SetObjectType(td);	// catch all.
  }
  return true;
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

void UserScript::GenCssBody_impl(Program* prog) {
  script.ParseExpr();		// re-parse just to be sure!
  prog->AddLine(this, script.GetFullExpr(), ProgLine::MAIN_LINE);
  prog->AddVerboseLine(this);
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

void WhileLoop::GenCssPre_impl(Program* prog) {
  test.ParseExpr();		// re-parse just to be sure!
  prog->AddLine(this, String("while (") + test.GetFullExpr() + ") {", ProgLine::MAIN_LINE);
  prog->AddVerboseLine(this, true, "\"before entering loop\""); // move to start
  prog->IncIndent();
  prog->AddVerboseLine(this, false, "\"starting in loop\""); // don't move to out
}

void WhileLoop::GenCssPost_impl(Program* prog) {
  prog->DecIndent();
  prog->AddLine(this, "}");
}

String WhileLoop::GetDisplayName() const {
  return "while (" + test.expr + ")";
}

bool WhileLoop::CanCvtFmCode(const String& code, ProgEl* scope_el) const {
  if(code.startsWith("while")) return true;
  return false;
}

bool WhileLoop::CvtFmCode(const String& code) {
  String cd = trim(code.after("while"));
  if(cd.startsWith('(')) {
    cd = cd.after('(');
    if(cd.endsWith(')')) cd = cd.before(')', -1);
  }
  if(cd.endsWith(';')) cd = cd.before(';',-1);
  test.SetExpr(cd);
  return true;
}

//////////////////////////
//  DoLoop		//
//////////////////////////

void DoLoop::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  CheckError(test.expr.empty(), quiet, rval, "test expression is empty");
}

void DoLoop::GenCssPre_impl(Program* prog) {
  prog->AddLine(this, "do {", ProgLine::MAIN_LINE);
  prog->AddVerboseLine(this, true, "\"before entering loop\""); // move to start
  prog->IncIndent();
  prog->AddVerboseLine(this, false, "\"starting in loop\""); // don't move to out
}

void DoLoop::GenCssPost_impl(Program* prog) {
  test.ParseExpr();		// re-parse just to be sure!
  prog->DecIndent();
  prog->AddLine(this, String("} while (") + test.GetFullExpr() + ");");
}

String DoLoop::GetDisplayName() const {
  return "do ... while (" + test.expr + ")";
}

bool DoLoop::CanCvtFmCode(const String& code, ProgEl* scope_el) const {
  if(code.startsWith("do")) return true;
  return false;
}

bool DoLoop::CvtFmCode(const String& code) {
  String cd = trim(code.after("do"));
  if(cd.startsWith('(')) {
    cd = cd.after('(');
    if(cd.endsWith(')'))
      cd = cd.before(')', -1);
  }
  if(cd.endsWith(';')) cd = cd.before(';',-1);
  test.SetExpr(cd);
  return true;
}

//////////////////////////
//  ForLoop		//
//////////////////////////

// todo: remove after a while (4.0.10)
void ForLoop::SetProgExprFlags() {
  // init.SetExprFlag(ProgExpr::NO_VAR_ERRS); // don't report bad variable errors
  // test.SetExprFlag(ProgExpr::NO_VAR_ERRS); // don't report bad variable errors
  // iter.SetExprFlag(ProgExpr::NO_VAR_ERRS); // don't report bad variable errors

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
  if(!prg || isDestroying()) return;
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

  Program* my_prog = GET_MY_OWNER(Program);
  if(!my_prog) return;
  Function* my_fun = GET_MY_OWNER(Function);
  
  if(my_fun) {			// use function scope by default
    if(my_fun->FindVarName(var_nm)) return; // all good
  }
  if(my_prog->FindVarName(var_nm)) return; // still good

  ProgVar* var = NULL;
  LocalVars* locvars = FindLocalVarList();
  if(locvars) {
    var = locvars->AddVar();
  }
  else {
    var = (ProgVar*)my_prog->vars.New(1, &TA_ProgVar);
  }
  if(var) {
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

void ForLoop::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  CheckError(test.expr.empty(), quiet, rval, "test expression is empty");
  CheckError(iter.expr.empty(), quiet, rval, "iter expression is empty");
}

void ForLoop::GenCssPre_impl(Program* prog) {
  init.ParseExpr();		// re-parse just to be sure!
  test.ParseExpr();		// re-parse just to be sure!
  iter.ParseExpr();		// re-parse just to be sure!
  String full_expr = init.GetFullExpr() + "; " + test.GetFullExpr() + "; " + iter.GetFullExpr();
  prog->AddLine(this, String("for(") + full_expr + ") {", ProgLine::MAIN_LINE);
  prog->AddVerboseLine(this, true, "\"before entering loop\""); // move to start
  prog->IncIndent();
  prog->AddVerboseLine(this, false, "\"starting in loop\""); // don't move to out
}

void ForLoop::GenCssPost_impl(Program* prog) {
  prog->DecIndent();
  prog->AddLine(this, "}");
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

bool ForLoop::CanCvtFmCode(const String& code, ProgEl* scope_el) const {
  if(code.startsWith("for(") || code.startsWith("for (")) return true;
  return false;
}

bool ForLoop::CvtFmCode(const String& code) {
  String cd = trim(code.after("for"));
  if(cd.startsWith('(')) {
    cd = cd.after('(');
    if(cd.endsWith(')'))
      cd = cd.before(')', -1);
  }
  if(cd.endsWith(';')) cd = cd.before(';',-1);
  init.SetExpr(cd.before(';'));
  String rest = cd.after(';');
  test.SetExpr(rest.before(';'));
  iter.SetExpr(rest.after(';'));
  UpdateAfterEdit_impl();	// make local var
  return true;
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

void IfContinue::GenCssBody_impl(Program* prog) {
  cond.ParseExpr();		// re-parse just to be sure!
  String fexp = cond.GetFullExpr();
  if(fexp.nonempty()) {
    prog->AddLine(this, "if(" + fexp + ") {", ProgLine::MAIN_LINE);
    prog->AddLine(this, "continue;");
    prog->AddVerboseLine(this, true, "\"before if\"");
    prog->IncIndent();
    prog->AddVerboseLine(this, false, "\"inside if -- continuing\"");
    prog->DecIndent();
    prog->AddLine(this, "}");
  }
  else {
    prog->AddLine(this, "continue;", ProgLine::MAIN_LINE);
    prog->AddVerboseLine(this);
  }
}

String IfContinue::GetDisplayName() const {
  if(cond.expr.empty())
    return "continue;";
  else
    return "if(" + cond.expr + ") continue;";
}

bool IfContinue::CanCvtFmCode(const String& code, ProgEl* scope_el) const {
  if(code.startsWith("if") && code.contains("continue")) return true;
  return false;
}

bool IfContinue::CvtFmCode(const String& code) {
  String cd = trim(code.after("if"));
  cd = trim(cd.before("continue"));
  if(cd.startsWith('(')) {
    cd = cd.after('(');
    if(cd.endsWith(')'))
      cd = cd.before(')', -1);
  }
  cond.SetExpr(cd);
  return true;
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

void IfBreak::GenCssBody_impl(Program* prog) {
  cond.ParseExpr();		// re-parse just to be sure!
  String fexp = cond.GetFullExpr();
  if(fexp.nonempty()) {
    prog->AddLine(this, "if(" + fexp + ") {", ProgLine::MAIN_LINE);
    prog->AddLine(this, "break;");
    prog->AddVerboseLine(this, true, "\"before if\"");
    prog->IncIndent();
    prog->AddVerboseLine(this, false, "\"inside if -- breaking\"");
    prog->DecIndent();
    prog->AddLine(this, "}");
  }
  else {
    prog->AddLine(this, "break;", ProgLine::MAIN_LINE);
    prog->AddVerboseLine(this);
  }
}

String IfBreak::GetDisplayName() const {
  if(cond.expr.empty())
    return "break;";
  else
    return "if(" + cond.expr + ") break;";
}

bool IfBreak::CanCvtFmCode(const String& code, ProgEl* scope_el) const {
  if(code.startsWith("if") && code.contains("break")) return true;
  return false;
}

bool IfBreak::CvtFmCode(const String& code) {
  String cd = trim(code.after("if"));
  cd = trim(cd.before("break"));
  if(cd.startsWith('(')) {
    cd = cd.after('(');
    if(cd.endsWith(')'))
      cd = cd.before(')', -1);
  }
  cond.SetExpr(cd);
  return true;
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

void IfReturn::GenCssBody_impl(Program* prog) {
  cond.ParseExpr();		// re-parse just to be sure!
  String fexp = cond.GetFullExpr();
  if(fexp.nonempty()) {
    prog->AddLine(this, "if(" + fexp + ") {", ProgLine::MAIN_LINE);
    prog->AddLine(this, "return;");
    prog->AddVerboseLine(this, true, "\"before if\"");
    prog->IncIndent();
    prog->AddVerboseLine(this, false, "\"inside if -- returning\"");
    prog->DecIndent();
    prog->AddLine(this, "}");
  }
  else {
    prog->AddLine(this, "return;", ProgLine::MAIN_LINE);
    prog->AddVerboseLine(this);
  }
}

String IfReturn::GetDisplayName() const {
  if(cond.expr.empty())
    return "return;";
  else
    return "if(" + cond.expr + ") return;";
}

bool IfReturn::CanCvtFmCode(const String& code, ProgEl* scope_el) const {
  if(code.startsWith("if") && code.contains("return")) return true;
  return false;
}

bool IfReturn::CvtFmCode(const String& code) {
  String cd = trim(code.after("if"));
  cd = trim(cd.before("return"));
  if(cd.startsWith('(')) {
    cd = cd.after('(');
    if(cd.endsWith(')'))
      cd = cd.before(')', -1);
  }
  cond.SetExpr(cd);
  return true;
}

//////////////////////////
//  IfElse		//
//////////////////////////

void IfElse::Initialize() {
  show_false_code = true;
}

void IfElse::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  if(taMisc::is_loading) {
    if(false_code.size == 0)
      show_false_code = false;
  }
  if(false_code.size > 0)
    show_false_code = true;
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

void IfElse::GenCssPre_impl(Program* prog) {
  cond.ParseExpr();		// re-parse just to be sure!
  prog->AddLine(this, "if(" + cond.GetFullExpr() + ") {", ProgLine::MAIN_LINE);
  prog->AddVerboseLine(this, true, "\"before if\"");
  prog->IncIndent();
  prog->AddVerboseLine(this, false, "\"inside if\"");
}

void IfElse::GenCssBody_impl(Program* prog) {
  true_code.GenCss(prog);
  // don't gen 'else' portion unless there are els
  if (false_code.size > 0) {
    prog->DecIndent();
    prog->AddLine(this, "} else {");
    prog->IncIndent();
    prog->AddVerboseLine(this, false, "\"inside else\"");
    false_code.GenCss(prog);
  }
}

void IfElse::GenCssPost_impl(Program* prog) {
  prog->DecIndent();
  prog->AddLine(this, "}");
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
  true_code.PreGen(item_id);
  false_code.PreGen(item_id);
}

ProgVar* IfElse::FindVarName(const String& var_nm) const {
  ProgVar* pv = inherited::FindVarName(var_nm);
  if (pv) return pv;
  pv = true_code.FindVarName(var_nm);
  if(pv) return pv;
  return false_code.FindVarName(var_nm);
}

String IfElse::GetDisplayName() const {
  return "if (" + cond.expr + ")";
}

bool IfElse::CanCvtFmCode(const String& code, ProgEl* scope_el) const {
  if(code.startsWith("if")) return true;
  return false;
}

bool IfElse::CvtFmCode(const String& code) {
  String cd = trim(code.after("if"));
  if(cd.startsWith('(')) {
    cd = cd.after('(');
    if(cd.endsWith(')'))
      cd = cd.before(')', -1);
  }
  if(cd.endsWith(';')) cd = cd.before(';',-1);
  cond.SetExpr(cd);
  return true;
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

void IfGuiPrompt::GenCssPre_impl(Program* prog) {
  if(taMisc::gui_active) {
    prog->AddLine(this, "{ int chs = taMisc::Choice(\"" + prompt + "\", \""
		  + yes_label + "\", \"" + no_label + "\");", ProgLine::MAIN_LINE);
    prog->AddVerboseLine(this);
    prog->IncIndent();
    prog->AddLine(this, "if(chs == 0) {");
    prog->IncIndent();
    prog->AddVerboseLine(this, false, "\"inside choice == yes\"");
  }
  else {
    prog->AddLine(this, "{");		// just a block to run..
    prog->IncIndent();
  }
}

void IfGuiPrompt::GenCssBody_impl(Program* prog) {
  yes_code.GenCss(prog);
}

void IfGuiPrompt::GenCssPost_impl(Program* prog) {
  prog->DecIndent();
  prog->AddLine(this, "}");
  if(taMisc::gui_active) {	// extra close
    prog->DecIndent();
    prog->AddLine(this, "}");
  }
}

const String IfGuiPrompt::GenListing_children(int indent_level) {
  String rval = yes_code.GenListing(indent_level + 1 + (int)taMisc::gui_active);
  return rval;
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

void CaseBlock::GenCssPre_impl(Program* prog) {
  case_val.ParseExpr();		// re-parse just to be sure!
  if(prog_code.size == 0) return;
  String expr = case_val.GetFullExpr();
  if(expr.empty())
    prog->AddLine(this, "default: {", ProgLine::MAIN_LINE);
  else
    prog->AddLine(this, "case " + case_val.GetFullExpr() + ": {", ProgLine::MAIN_LINE);
  prog->IncIndent();
  prog->AddVerboseLine(this, false, "\"inside case\"");
}

void CaseBlock::GenCssBody_impl(Program* prog) {
  if(prog_code.size == 0) return;
  prog_code.GenCss(prog);
}

void CaseBlock::GenCssPost_impl(Program* prog) {
  if(prog_code.size == 0) return;
  prog->AddLine(this, "break;"); // always break
  prog->DecIndent();
  prog->AddLine(this, "}");
}

String CaseBlock::GetDisplayName() const {
  if(case_val.expr.empty()) return "default: (" + String(prog_code.size) + " items)";
  return "case: " + case_val.expr + " (" + String(prog_code.size) + " items)";
}

bool CaseBlock::CanCvtFmCode(const String& code, ProgEl* scope_el) const {
  if(code.startsWith("case") || code.startsWith("default")) return true;
  return false;
}

bool CaseBlock::CvtFmCode(const String& code) {
  String cd;
  if(code.startsWith("case")) cd = trim(code.after("case"));
  else if(code.startsWith("default")) cd = trim(code.after("default"));
  if(cd.startsWith('(')) {
    cd = cd.after('(');
    if(cd.endsWith(')'))
      cd = cd.before(')', -1);
  }
  if(cd.endsWith(':'))
    cd = cd.before(':', -1);
  case_val.SetExpr(cd);
  return true;
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

void Switch::GenCssPre_impl(Program* prog) {
  if(!switch_var) return;
  prog->AddLine(this, "switch(" + switch_var->name + ") {", ProgLine::MAIN_LINE);
  prog->AddVerboseLine(this);
  prog->IncIndent();
}

void Switch::GenCssBody_impl(Program* prog) {
  if(!switch_var) return;
  cases.GenCss(prog);
}

void Switch::GenCssPost_impl(Program* prog) {
  if(!switch_var) return;
  prog->DecIndent();
  prog->AddLine(this, "}");
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

bool Switch::CanCvtFmCode(const String& code, ProgEl* scope_el) const {
  if(code.startsWith("switch")) return true;
  return false;
}

bool Switch::CvtFmCode(const String& code) {
  String cd = trim(code.after("switch"));
  if(cd.startsWith('(')) {
    cd = cd.after('(');
    if(cd.endsWith(')'))
      cd = cd.before(')', -1);
  }
  if(cd.endsWith(';')) cd = cd.before(';',-1);
  switch_var = FindVarNameInScope(cd, true); // prompt to make if not found
  return true;
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

void AssignExpr::GenCssBody_impl(Program* prog) {
  expr.ParseExpr();		// re-parse just to be sure!
  if (!result_var) {
    prog->AddLine(this, "// WARNING: AssignExpr not generated here -- result_var not specified", ProgLine::MAIN_LINE);
    return;
  }

  prog->AddLine(this, result_var->name + " = " + expr.GetFullExpr() + ";", ProgLine::MAIN_LINE);
  prog->AddVerboseLine(this, true, "\"prev value:\", String(" + result_var->name + ")"); // moved above
  prog->AddVerboseLine(this, false, "\"new  value:\", String(" + result_var->name + ")"); // after
}

String AssignExpr::GetDisplayName() const {
  if(!result_var)
    return "(result_var not selected)";
  
  String rval;
  rval += result_var->name + "=" + expr.GetFullExpr();
  return rval;
}

bool AssignExpr::CanCvtFmCode(const String& code, ProgEl* scope_el) const {
  // note: AssignExpr is specifically excluded if multiple matches, so no need to exclude
  // all the other things that might have an = in them -- it is just a fallback default
  if(code.freq('=') == 1) {
    String lhs = code.before('=');
    if(lhs.nonempty() && !lhs.contains('.') && !lhs.contains('-')) // no path
      return true;
  }
  return false;
}

bool AssignExpr::CvtFmCode(const String& code) {
  String lhs = trim(code.before('='));
  String rhs = trim(code.after('='));
  if(rhs.endsWith(';')) rhs = rhs.before(';',-1);
  
  result_var = FindVarNameInScope(lhs, true); // option to make
  expr.SetExpr(rhs);
  
  return true;
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

void VarIncr::GenCssBody_impl(Program* prog) {
  expr.ParseExpr();		// re-parse just to be sure!
  if (!var) {
    prog->AddLine(this, "// WARNING: VarIncr not generated here -- var not specified", ProgLine::MAIN_LINE);
    return;
  }
  
  prog->AddLine(this, var->name + " = " + var->name + " + " + expr.GetFullExpr() + ";", ProgLine::MAIN_LINE);
  prog->AddVerboseLine(this, true, "\"prev value:\", String(" + var->name + ")"); // moved above
  prog->AddVerboseLine(this, false, "\"new  value:\", String(" + var->name + ")"); // after
}

String VarIncr::GetDisplayName() const {
  if(!var)
    return "(var not selected)";
  
  String rval;
  rval += var->name + "+=" + expr.GetFullExpr();
  return rval;
}

bool VarIncr::CanCvtFmCode(const String& code, ProgEl* scope_el) const {
  if(code.freq("+=") == 1 || code.freq("-=") == 1) return true;
  return false;
}

bool VarIncr::CvtFmCode(const String& code) {
  String lhs, rhs;
  bool neg = false;
  if(code.contains("+=")) {
    lhs = trim(code.before("+="));
    rhs = trim(code.after("+="));
  }
  else {
    lhs = trim(code.before("-="));
    rhs = trim(code.after("-="));
    neg = true;
  }
  if(rhs.endsWith(';')) rhs = rhs.before(';',-1);
  
  var = FindVarNameInScope(lhs, true); // option to make
  if(neg)
    expr.SetExpr("-" + rhs);
  else
    expr.SetExpr(rhs);
  
  return true;
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

void MethodCall::GenCssBody_impl(Program* prog) {
  if (!((bool)obj && method)) {
    prog->AddLine(this, "// WARNING: MethodCall not generated here -- obj or method not specified", ProgLine::MAIN_LINE);
    return;
  }
  
  String rval;
  if(result_var)
    rval += result_var->name + " = ";
  rval += obj->name;
  rval += "->";
  rval += method->name;
  rval += meth_args.GenCssArgs();
  rval += ";";

  prog->AddLine(this, rval, ProgLine::MAIN_LINE);
  prog->AddVerboseLine(this);
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

bool MethodCall::CanCvtFmCode(const String& code, ProgEl* scope_el) const {
  // fmt: [result = ]obj[.|->]method(args...
  if(!code.contains('(')) return false;
  String lhs = code.before('(');
  String mthobj = lhs;
  if(lhs.contains('='))
    mthobj = trim(lhs.after('='));
  if((mthobj.freq('.') + mthobj.freq("->")) != 1) return false;
  String objnm;
  if(mthobj.contains('.'))
    objnm = mthobj.before('.');
  else 
    objnm = mthobj.before("->");
  if(objnm.nonempty() && !objnm.contains('[')) return true;
  // syntax above should be enough to rule in -- no [ ] paths either tho -- nowhere to put
  return false;
}

bool MethodCall::CvtFmCode(const String& code) {
  String lhs = trim(code.before('('));
  String mthobj = lhs;
  String rval;
  if(lhs.contains('=')) {
    mthobj = trim(lhs.after('='));
    rval = trim(lhs.before('='));
  }
  String objnm;
  String methnm;
  if(mthobj.contains('.')) {
    objnm = mthobj.before('.');
    methnm = mthobj.after('.');
  }
  else {
    objnm = mthobj.before("->");
    methnm = mthobj.after("->");
  }
  ProgVar* pv = FindVarNameInScope(objnm, true); // true = give option to make one
  if(!pv) return false;
  obj = pv;
  if(rval.nonempty())
    result_var = FindVarNameInScope(rval, true); // true = give option to make one
  UpdateAfterEdit_impl();			   // update based on obj
  MethodDef* md = obj_type->methods.FindName(methnm);
  if(md) {
    method = md;
    UpdateAfterEdit_impl();			   // update based on obj
  }
  // now tackle the args
  String args = trim(code.after('('));
  if(args.endsWith(')')) args = trim(args.before(')',-1));
  if(args.endsWith(';')) args = trim(args.before(';',-1));
  for(int i=0; i<meth_args.size; i++) {
    ProgArg* pa = meth_args.FastEl(i);
    String arg;
    if(args.contains(',')) {
      arg = trim(args.before(','));
      args = trim(args.after(','));
    }
    else {
      arg = args;
      args = "";		// all done
    }
    pa->expr.SetExpr(arg);
    if(args.empty()) break;
  }
  return true;
}



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
  path = trim(path);					   // keep it clean
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
  if(!rval) {			// didn't get it yet, try with static
    int net_base_off = 0;
    ta_memb_ptr net_mbr_off = 0;
    md = TypeDef::FindMemberPathStatic(ot, net_base_off, net_mbr_off, path, false);
    // gets static path based just on member types..
    if(md) {
      obj_type = md->type;	// it is the type of the member, not the owner type.
    }
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

// bool MemberProgEl::CanCvtFmCode(const String& code, ProgEl* scope_el) const {
//   return false;
// }

// bool MemberProgEl::CvtFmCode(const String& code) {
//   return false;
// }


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

void MemberAssign::GenCssBody_impl(Program* prog) {
  expr.ParseExpr();		// re-parse just to be sure!
  if (!(bool)obj || path.empty() || expr.empty()) {
    prog->AddLine(this, "// WARNING: MemberAssign not generated here -- obj or path not specified or expr empty", ProgLine::MAIN_LINE);
    return;
  }

  String path_term = path;
  String path_pre = path;
  if(path_term.contains('.')) {
    path_term = path_term.after('.', -1);
    path_pre = path.before('.', -1);
  }
  String opath;
  if(path_term != path) {
    if(path_pre.startsWith('['))
      opath = obj->name + path_pre;
    else
      opath = obj->name + "->" + path_pre;
  }
  else {
    opath = obj->name;
  }
  opath = trim(opath);
  String fpath = opath + "->" + path_term;
  String rval;
  if(opath.endsWith(']')) {	// itr expression
    rval = "set(" + opath + ", \"" + path_term + "\", " + expr.GetFullExpr() + ");";
  }
  else {
    rval = fpath + " = " + expr.GetFullExpr() + ";";
  }
  prog->AddLine(this, rval, ProgLine::MAIN_LINE);
  if (update_after) {
    prog->AddLine(this, obj->name + "->UpdateAfterEdit();");
    if(path_term != path) {
      // also do uae on immediate owner!
      prog->AddLine(this, opath + "->UpdateAfterEdit();");
    }
  }
  prog->AddVerboseLine(this, true, "\"prev value:\", String(" + fpath + ")"); // moved above
  prog->AddVerboseLine(this, false, "\"new  value:\", String(" + fpath + ")"); // after
}

String MemberAssign::GetDisplayName() const {
  if (!obj || path.empty())
    return "(object or path not selected)";
  
  String rval;
  rval = obj->name;
  if(path.startsWith('['))
    rval += path;
  else
    rval += "->" + path;
  rval += " = ";
  rval += expr.GetFullExpr();
  return rval;
}

bool MemberAssign::CanCvtFmCode(const String& code, ProgEl* scope_el) const {
  if(!code.contains('=')) return false;
  String lhs = code.before('=');
  if(!(lhs.contains('.') || lhs.contains("->"))) return false;
  return true;			// probably enough?
}

bool MemberAssign::CvtFmCode(const String& code) {
  String lhs = trim(code.before('='));
  if(lhs.contains('(') || lhs.contains(' ')) return false; // exclude others
  String objnm;
  String pathnm;
  if(lhs.contains('.')) {
    objnm = lhs.before('.');
    pathnm = lhs.after('.');
  }
  else {
    objnm = lhs.before("->");
    pathnm = lhs.after("->");
  }
  if(objnm.contains('[')) {
    if(pathnm.nonempty())
      pathnm = objnm.from('[') + "." + pathnm;
    else 
      pathnm = objnm.from('[');
    objnm = objnm.before('[');
  }
  ProgVar* pv = FindVarNameInScope(objnm, true); // true = give option to make one
  if(!pv) return false;
  obj = pv;
  path = pathnm;
  String rhs = trim(code.after('='));
  if(rhs.endsWith(';')) rhs = rhs.before(';',-1);
  expr.SetExpr(rhs);
  UpdateAfterEdit_impl();
  return true;
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

void MemberFmArg::GenCssBody_impl(Program* prog) {
  if (!(bool)obj || path.empty() || arg_name.empty()) {
    prog->AddLine(this, "// WARNING: MemberFmArg not generated here -- obj or path not specified or expr empty", ProgLine::MAIN_LINE);
    return;
  }

  String flpth = obj->name + "->" + path;
  
  prog->AddLine(this, "{ String arg_str = taMisc::FindArgByName(\"" + arg_name + "\");",
		ProgLine::MAIN_LINE);
  prog->IncIndent();
  prog->AddLine(this, "if(arg_str.nonempty()) {");
  prog->IncIndent();
  prog->AddLine(this, flpth + " = arg_str;");
  if(update_after) {
    prog->AddLine(this, obj->name + "->UpdateAfterEdit();");
    if(path.contains('.')) {
      // also do uae on immediate owner!
      prog->AddLine(this, obj->name + "->" + path.before('.',-1) + "->UpdateAfterEdit();");
    }
  }

  if(!quiet || IsVerbose())	// special case
    prog->AddLine(this, String("taMisc::Info(\"Set ") + flpth + " to:\"," + flpth + ");");
  prog->DecIndent();
  prog->AddLine(this, "}");
  prog->DecIndent();
  prog->AddLine(this, "}");
}

void MemberFmArg::GenRegArgs(Program* prog) {
  prog->AddLine(this, String("taMisc::AddEqualsArgName(\"") + arg_name + "\");");
  prog->AddLine(this, String("taMisc::AddArgNameDesc(\"") + arg_name
		+ "\", \"MemberFmArg: obj = " + (((bool)obj) ? obj->name : "NOT SET")
		+ " path = " + path + "\");");
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

void MemberMethodCall::GenCssBody_impl(Program* prog) {
  if (!((bool)obj && method)) {
    prog->AddLine(this, "// WARNING: MemberMethodCall not generated here -- obj or method not specified");
    return;
  }

  String rval;
  if(result_var)
    rval += result_var->name + " = ";

  if(path.endsWith(']')) {    // is using list comprehension access of some sort
    // use call to iterate over elements on the list
    rval += "call(" + obj->name;
    if(path.startsWith('['))
      rval += path;
    else
      rval += "->" + path;
    rval += ", \"" + method->name + "\"";
    if(meth_args.size > 0) {
      String targs = meth_args.GenCssArgs();
      rval += ", " + targs.after('(');
    }
    else {
      rval += ")";
    }
    rval += ";";
  }
  else {
    rval += obj->name + "->" + path + "->";
    rval += method->name;
    rval += meth_args.GenCssArgs();
    rval += ";";
  }

  prog->AddLine(this, rval, ProgLine::MAIN_LINE);
  prog->AddVerboseLine(this);
}

String MemberMethodCall::GetDisplayName() const {
  if (!obj || !method)
    return "(object or method not selected)";
  
  String rval;
  if(result_var)
    rval += result_var->name + "=";
  rval += obj->name;
  if(path.startsWith('['))
    rval += path;
  else
    rval += "->" + path;
  rval += "->" + method->name + "(";
  for(int i=0;i<meth_args.size;i++) {
    ProgArg* pa = meth_args[i];
    if (i > 0)
      rval += ", ";
    rval += pa->expr.expr;   // GetDisplayName();
  }
  rval += ")";
  return rval;
}

bool MemberMethodCall::CanCvtFmCode(const String& code, ProgEl* scope_el) const {
  if(!code.contains('(')) return false;
  String lhs = code.before('(');
  int mbfreq = lhs.freq('.') + lhs.freq("->");
  if(mbfreq <= 1) {
    if(!(mbfreq == 1 && lhs.contains('[') && lhs.contains(']')))
      return false;
  }
  String mthobj = lhs;
  if(lhs.contains('='))
    mthobj = trim(lhs.after('='));
  String objnm;
  if(mthobj.contains('.'))
    objnm = mthobj.before('.');
  else 
    objnm = mthobj.before("->");
  if(objnm.nonempty()) return true; // syntax above should be enough to rule in..
  return false;
}

bool MemberMethodCall::CvtFmCode(const String& code) {
  String lhs = trim(code.before('('));
  String mthobj = lhs;
  String rval;
  if(lhs.contains('=')) {
    mthobj = trim(lhs.after('='));
    rval = trim(lhs.before('='));
  }
  String objnm;
  String pathnm;
  if(mthobj.contains('.')) {
    objnm = mthobj.before('.');
    pathnm = mthobj.after('.');
  }
  else {
    objnm = mthobj.before("->");
    pathnm = mthobj.after("->");
  }
  if(objnm.contains('[')) {
    if(pathnm.nonempty())
      pathnm = objnm.from('[') + "." + pathnm;
    else 
      pathnm = objnm.from('[');
    objnm = objnm.before('[');
  }
  ProgVar* pv = FindVarNameInScope(objnm, true); // true = give option to make one
  if(!pv) return false;
  obj = pv;
  if(rval.nonempty())
    result_var = FindVarNameInScope(rval, true); // true = give option to make one
  String methnm;
  if(pathnm.contains('.')) {
    methnm = pathnm.after('.',-1);
    pathnm = pathnm.before('.', -1);
  }
  else {
    methnm = pathnm.after("->",-1);
    pathnm = pathnm.before("->", -1);
  }
  path = pathnm;
  UpdateAfterEdit_impl();			   // update based on obj and path
  MethodDef* md = obj_type->methods.FindName(methnm);
  if(md) {
    method = md;
    UpdateAfterEdit_impl();			   // update based on obj
  }
  // now tackle the args
  String args = trim(code.after('('));
  if(args.endsWith(')')) args = trim(args.before(')',-1));
  if(args.endsWith(';')) args = trim(args.before(';',-1));
  for(int i=0; i<meth_args.size; i++) {
    ProgArg* pa = meth_args.FastEl(i);
    String arg;
    if(args.contains(',')) {
      arg = trim(args.before(','));
      args = trim(args.after(','));
    }
    else {
      arg = args;
      args = "";		// all done
    }
    pa->expr.SetExpr(arg);
    if(args.empty()) break;
  }
  return true;
}

//////////////////////////
//      MathCall	//
//////////////////////////

void MathCall::Initialize() {
  min_type = &TA_taMath;
  object_type = &TA_taMath_float;
}

bool MathCall::CanCvtFmCode(const String& code, ProgEl* scope_el) const {
  if(!code.contains("::")) return false;
  if(!code.contains('(')) return false;
  String lhs = code.before('(');
  String mthobj = lhs;
  if(lhs.contains('='))
    mthobj = trim(lhs.after('='));
  String objnm = mthobj.before("::");
  TypeDef* td = taMisc::types.FindName(objnm);
  if(!td) return false;
  if(objnm.contains("taMath")) return true;
  return false;
}

void RandomCall::Initialize() {
  min_type = &TA_Random;
  object_type = &TA_Random;
}

bool RandomCall::CanCvtFmCode(const String& code, ProgEl* scope_el) const {
  if(!code.contains("::")) return false;
  if(!code.contains('(')) return false;
  String lhs = code.before('(');
  String mthobj = lhs;
  if(lhs.contains('='))
    mthobj = trim(lhs.after('='));
  String objnm = mthobj.before("::");
  TypeDef* td = taMisc::types.FindName(objnm);
  if(!td) return false;
  if(objnm == "Random") return true;
  return false;
}

void MiscCall::Initialize() {
  min_type = &TA_taMisc;
  object_type = &TA_taMisc;
}

bool MiscCall::CanCvtFmCode(const String& code, ProgEl* scope_el) const {
  if(!code.contains("::")) return false;
  if(!code.contains('(')) return false;
  String lhs = code.before('(');
  String mthobj = lhs;
  if(lhs.contains('='))
    mthobj = trim(lhs.after('='));
  String objnm = mthobj.before("::");
  TypeDef* td = taMisc::types.FindName(objnm);
  if(!td) return false;
  if(objnm == "taMisc") return true;
  return false;
}

//////////////////////////
//      PrintVar	//
//////////////////////////

void PrintVar::Initialize() {
  nogui = false;
}

void PrintVar::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  Program* my_prog = program();
  if(!my_prog) return;
  if(!debug_level) {
    debug_level = my_prog->vars.FindName("debug_level"); 
  }
}

void PrintVar::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  //  CheckError(!print_var, quiet, rval, "print_var is NULL");
}

void PrintVar::GenCssBody_impl(Program* prog) {
  if(message.empty() && !print_var && !print_var2 && !print_var3 && !print_var4 && !print_var5 && !print_var6)
    return;
  if(!nogui && !taMisc::gui_active) // don't generate anything
    return;

  String rval = "cerr ";
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
  rval += " << endl;";

  if(my_mask && debug_level) {
    prog->AddLine(this, "if(" + my_mask->name + " & " + debug_level->name + ") {");
    prog->IncIndent();
    prog->AddLine(this, rval, ProgLine::MAIN_LINE);
    prog->AddVerboseLine(this);
    prog->DecIndent();
    prog->AddLine(this, "}");
  }
  else {
    prog->AddLine(this, rval, ProgLine::MAIN_LINE);
    prog->AddVerboseLine(this);
  }
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

  if(debug_level && my_mask) {
    rval += " (dbg mask: " + my_mask->name + ")";
  }
  return rval;
}

bool PrintVar::CanCvtFmCode(const String& code, ProgEl* scope_el) const {
  if(!(code.startsWith("print ") || code.startsWith("cerr << ") || code.startsWith("cout << ")))
    return false;
  String exprstr;
  if(code.startsWith("print ")) exprstr = trim(code.after("print "));
  else if(code.startsWith("cerr << ")) exprstr = trim(code.after("cerr << "));
  else if(code.startsWith("cout << ")) exprstr = trim(code.after("cout << "));
  if(exprstr.contains('"')) {
    if(exprstr.freq('"') != 2) return false; // only one message
    exprstr = trim(exprstr.after('"',-1));
    if(exprstr.contains(',')) exprstr = trim(exprstr.after(','));
    if(exprstr.contains("<<")) exprstr = trim(exprstr.after("<<"));
  }
  String varnm = exprstr;
  if(exprstr.contains(',')) varnm = trim(exprstr.before(','));
  if(exprstr.contains("<<")) varnm = trim(exprstr.before("<<"));
  if(exprstr.contains(' ')) varnm = trim(exprstr.before(' '));
  if(varnm.nonempty()) return true; // cannot look it up -- have to go on nonempty status
  return false;
}

bool PrintVar::CvtFmCode(const String& code) {
  String exprstr;
  if(code.startsWith("print ")) exprstr = trim(code.after("print "));
  else if(code.startsWith("cerr << ")) exprstr = trim(code.after("cerr << "));
  else if(code.startsWith("cout << ")) exprstr = trim(code.after("cout << "));
  String msg;
  String vars;
  if(exprstr.contains('"')) {
    msg = trim(exprstr.before('"',-1));
    msg = msg.after('"');
    message = msg;
    exprstr = trim(exprstr.after('"',-1));
    if(exprstr.contains(',')) exprstr = trim(exprstr.after(','));
    if(exprstr.contains("<<")) exprstr = trim(exprstr.after("<<"));
  }
  if(exprstr.empty()) return true; // ok, we'll take it -- just a meassage
  String varnms = exprstr;
  int idx = 0;
  ProgVarRef* refs[6] = {&print_var, &print_var2, &print_var3, &print_var4, &print_var5,
			 &print_var6};
  do {
    String varnm = varnms;
    if(varnms.contains(',')) {
      varnm = trim(varnms.before(',')); varnms = trim(varnms.after(',')); }
    if(varnms.contains("<<")) {
      varnm = trim(varnms.before("<<")); varnms = trim(varnms.after("<<")); }
    if(varnms.contains(' ')) {
      varnm = trim(varnms.before(' ')); varnms = trim(varnms.after(' ')); }
    if(varnm == varnms)
      varnms = "";		// done
    ProgVar* pv = FindVarNameInScope(varnm, false);
    if(pv) {
      *(refs[idx]) = pv;
    }
    idx++;
  } while(varnms.nonempty());
  return true;
}

//////////////////////////
//      PrintExpr	//
//////////////////////////

void PrintExpr::Initialize() {
  nogui = false;
}

void PrintExpr::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  Program* my_prog = program();
  if(!my_prog) return;
  if(!debug_level) {
    debug_level = my_prog->vars.FindName("debug_level"); 
  }
}

void PrintExpr::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  expr.CheckConfig(quiet, rval);
}

void PrintExpr::GenCssBody_impl(Program* prog) {
  if(!nogui && !taMisc::gui_active) // don't generate anything
    return;
  expr.ParseExpr();		// re-parse just to be sure!
  String rval = String("cerr << ") + expr.GetFullExpr() + " << endl;";

  if(my_mask && debug_level) {
    prog->AddLine(this, "if(" + my_mask->name + " & " + debug_level->name + ") {");
    prog->IncIndent();
    prog->AddLine(this, rval, ProgLine::MAIN_LINE);
    prog->AddVerboseLine(this);
    prog->DecIndent();
    prog->AddLine(this, "}");
  }
  else {
    prog->AddLine(this, rval, ProgLine::MAIN_LINE);
    prog->AddVerboseLine(this);
  }
}

String PrintExpr::GetDisplayName() const {
  String rval;
  rval += "Print: " + expr.GetFullExpr();
  if(debug_level && my_mask) {
    rval += " (dbg mask: " + my_mask->name + ")";
  }
  return rval;
}

bool PrintExpr::CanCvtFmCode(const String& code, ProgEl* scope_el) const {
  if(!(code.startsWith("print ") || code.startsWith("cerr << ") || code.startsWith("cout << ")))
    return false;
  String exprstr;
  if(code.startsWith("print ")) exprstr = trim(code.after("print "));
  else if(code.startsWith("cerr << ")) exprstr = trim(code.after("cerr << "));
  else if(code.startsWith("cout << ")) exprstr = trim(code.after("cout << "));
  if(exprstr.freq('"') > 2) return true; // not a var guy
  return false;
}

bool PrintExpr::CvtFmCode(const String& code) {
  String exprstr;
  if(code.startsWith("print ")) exprstr = trim(code.after("print "));
  else if(code.startsWith("cerr << ")) exprstr = trim(code.after("cerr << "));
  else if(code.startsWith("cout << ")) exprstr = trim(code.after("cout << "));
  expr.SetExpr(exprstr);
  return true;
}


//////////////////////////
//      Comment 	//
//////////////////////////

void Comment::Initialize() {
  static String _def_comment("TODO: Add your program comment here (multi-lines ok).");
  desc = _def_comment;
}

void Comment::GenCssBody_impl(Program* prog) {
  prog->AddLine(this, "/*******************************************************************",
		ProgLine::MAIN_LINE);
  prog->AddDescString(this, desc);
  prog->AddLine(this, "*******************************************************************/");
}

String Comment::GetDisplayName() const {
  return desc;
}

bool Comment::CanCvtFmCode(const String& code, ProgEl* scope_el) const {
  if(code.startsWith("//") || code.startsWith("/*")) return true;
  return false;
}

bool Comment::CvtFmCode(const String& code) {
  if(code.startsWith("//")) desc = trim(code.after("//"));
  else if(code.startsWith("/*")) trim(desc = code.after("/*"));
  if(code.endsWith("*/")) desc = trim(desc.before("*/",-1));
  return true;
}


//////////////////////////
//      StopStepPoint 	//
//////////////////////////

void StopStepPoint::Initialize() {
}

void StopStepPoint::GenCssBody_impl(Program* prog) {
  prog->AddLine(this, "StopCheck(); // check for Stop or Step button", ProgLine::MAIN_LINE);
  prog->AddVerboseLine(this);
}

String StopStepPoint::GetDisplayName() const {
  return "Stop/Step Point";
}

void StopStepPoint::InitLinks() { 
  inherited::InitLinks(); 
  InitLinks_taAuto(&TA_StopStepPoint);
}

void StopStepPoint::PreGenMe_impl(int item_id) {
  // register as a subproc
  Program* prog = program();
  if (!prog) return; // shouldn't normally happen
  prog->SetProgFlag(Program::SELF_STEP);
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

void ReturnExpr::GenCssBody_impl(Program* prog) {
  expr.ParseExpr();		// re-parse just to be sure!
  prog->AddLine(this, "return " + expr.GetFullExpr() + ";", ProgLine::MAIN_LINE);
  prog->AddVerboseLine(this);
}

String ReturnExpr::GetDisplayName() const {
  String rval;
  rval += "return " + expr.expr;
  return rval;
}

bool ReturnExpr::CanCvtFmCode(const String& code, ProgEl* scope_el) const {
  if(code.startsWith("return")) return true;
  return false;
}

bool ReturnExpr::CvtFmCode(const String& code) {
  String cd = trim(code.after("return"));
  if(cd.startsWith('(')) {
    cd = cd.after('(');
    if(cd.endsWith(')')) cd = cd.before(')', -1);
  }
  if(cd.endsWith(';')) cd = cd.before(';',-1);
  expr.SetExpr(cd);
  return true;
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
		 "in program:", other_prog->name, "path:", other_prog->GetPathNames());
    }
    if(var_2) {
      pv = other_prog->FindVarName(var_2->name);
      CheckError(!pv, quiet, rval, "Could not find variable named:", var_2->name,
		 "in program:", other_prog->name, "path:", other_prog->GetPathNames());
    }
    if(var_3) {
      pv = other_prog->FindVarName(var_3->name);
      CheckError(!pv, quiet, rval, "Could not find variable named:", var_3->name,
		 "in program:", other_prog->name, "path:", other_prog->GetPathNames());
    }
    if(var_4) {
      pv = other_prog->FindVarName(var_4->name);
      CheckError(!pv, quiet, rval, "Could not find variable named:", var_4->name,
		 "in program:", other_prog->name, "path:", other_prog->GetPathNames());
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

bool OtherProgramVar::GenCss_OneVar(Program* prog, ProgVarRef& var, int var_no) {
  if(!var) return false;
  if(set_other) {
    prog->AddVerboseLine(this, false, "\"setting other prog's variable named: "+var->name +
			 " to value:\", String(" + var->name + ")");
    prog->AddLine(this, String("other_prog->SetVar(\"") + var->name + "\", " + var->name +");");
  }
  else {
    prog->AddVerboseLine(this, false, "\"setting my variable named: "+var->name +
			 " current value:\", String(" + var->name + ")");
    prog->AddLine(this, var->name + " = other_prog->GetVar(\"" + var->name + "\");");
    prog->AddVerboseLine(this, false, "\"new value:\", String(" + var->name + ")");
  }
  return true;
}

void OtherProgramVar::GenCssPre_impl(Program* prog) {
  String rval = "{ // other program var: "; 
  if (other_prog)
    rval += other_prog->name;
  prog->AddLine(this, rval, ProgLine::MAIN_LINE);
  prog->AddVerboseLine(this);
  prog->IncIndent();
}

void OtherProgramVar::GenCssBody_impl(Program* prog) {
  if (!other_prog) return;
  prog->AddLine(this, String("Program* other_prog = this") + GetPath(NULL, program())
		+ "->GetOtherProg();");
  GenCss_OneVar(prog, var_1, 0);
  GenCss_OneVar(prog, var_2, 1);
  GenCss_OneVar(prog, var_3, 2);
  GenCss_OneVar(prog, var_4, 3);
}

void OtherProgramVar::GenCssPost_impl(Program* prog) {
  prog->DecIndent();
  prog->AddLine(this, "} // other program var");
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
	       prog->name, "path:", prog->GetPathNames());
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

void ProgVarFmArg::GenCssBody_impl(Program* prog) {
  if (!prog) return;
  prog->AddLine(this, String("{ // prog var fm arg: ") + prog->name, ProgLine::MAIN_LINE);
  prog->AddVerboseLine(this);
  prog->IncIndent();
  prog->AddLine(this, String("Program* other_prog = this") + GetPath(NULL, program())
		+ "->GetOtherProg();");
  prog->AddLine(this, "other_prog->SetVarFmArg(\"" + arg_name + "\", \"" + var_name + "\");");
  prog->DecIndent();
  prog->AddLine(this, "} // prog var fm arg");
}

void ProgVarFmArg::GenRegArgs(Program* prog) {
  prog->AddLine(this, String("taMisc::AddEqualsArgName(\"") + arg_name + "\");");
  prog->AddLine(this, String("taMisc::AddArgNameDesc(\"") + arg_name
			     + "\", \"ProgVarFmArg: prog = " + (((bool)prog) ? prog->name : "NOT SET")
			     + " var_name = " + var_name + "\");");
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

void DataColsFmArgs::GenCssBody_impl(Program* prog) {
  DataTable* dt = GetData();
  if(!dt) {
    prog->AddLine(this, "// DataColsFmArgs: data_var not set!", ProgLine::MAIN_LINE);
    return;
  }
  prog->AddLine(this, "{ // DataColsFmArgs fm: " + dt->name, ProgLine::MAIN_LINE);
  prog->AddVerboseLine(this);
  prog->IncIndent();
  prog->AddLine(this, "String dcfma_colnm, dcfma_argval;");
  prog->AddLine(this, "for(int j=0;j<" + data_var->name + ".cols();j++) {");
  prog->IncIndent();
  prog->AddLine(this, "dcfma_colnm = " + data_var->name + ".data[j].name;");
  prog->AddLine(this, "dcfma_argval = taMisc::FindArgByName(dcfma_colnm);");
  prog->AddLine(this, "if(dcfma_argval.empty()) continue;");
  if(row_spec == CUR_ROW) {
    prog->AddLine(this, data_var->name + ".SetDataByName(dcfma_argval, dcfma_colnm);");
  }
  else if(row_spec == ROW_NUM) {
    prog->AddLine(this, data_var->name + ".SetValColName(dcfma_argval, dcfma_colnm, "
		  + row_var->name + ");");
  }
  else if(row_spec == ROW_VAL) {
    prog->AddLine(this, data_var->name + ".SetValColRowName(dcfma_argval, dcfma_colnm, \""
		  + row_var->name + "\", " + row_var->name + ");");
  }
  if(taMisc::dmem_proc == 0) {
    prog->AddLine(this, String("taMisc::Info(\"Set column: \",dcfma_colnm,\"in data table:\",\"") +
		  dt->name + "\",\"to val:\",dcfma_argval);");
  }
  prog->DecIndent();
  prog->AddLine(this, "}");
  prog->DecIndent();
  prog->AddLine(this, "}");
}

void DataColsFmArgs::GenRegArgs(Program* prog) {
  DataTable* dt = GetData();
  if(dt) {
    for(int j=0;j<dt->cols();j++) {
      DataCol* dc = dt->data[j];
      prog->AddLine(this, "taMisc::AddEqualsArgName(\"" + dc->name + "\");");
      prog->AddLine(this, "taMisc::AddArgNameDesc(\"" + dc->name
		    + "\", \"DataColsFmArgs: data_table = " + dt->name + "\");");
    }
  }
}

///////////////////////////////////////////////////////
//		SelectEditsFmArgs
///////////////////////////////////////////////////////

void SelectEditsFmArgs::Initialize() {
}

void SelectEditsFmArgs::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
}

void SelectEditsFmArgs::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  if(CheckError(!sel_edit_var, quiet, rval, "sel_edit_var is NULL")) return; // fatal
  CheckError(sel_edit_var->object_type != &TA_SelectEdit, quiet, rval,
	     "sel_edit_var variable does not point to a SelectEdit object");
}

String SelectEditsFmArgs::GetDisplayName() const {
  String rval = "Select Edits Fm Args";
  SelectEdit* se = GetSelectEdit();
  if(se) {
    rval += " To: " + se->name;
  }
  return rval;
}

SelectEdit* SelectEditsFmArgs::GetSelectEdit() const {
  if(!sel_edit_var) return NULL;
  if(sel_edit_var->object_type != &TA_SelectEdit) return NULL;
  return (SelectEdit*)sel_edit_var->object_val.ptr();
}

void SelectEditsFmArgs::GenCssBody_impl(Program* prog) {
  SelectEdit* se = GetSelectEdit();
  if(!se) {
    prog->AddLine(this, "// SelectEditsFmArgs: sel_edit_var not set!", ProgLine::MAIN_LINE);
    return;
  }

  prog->AddLine(this, "{ // SelectEditsFmArgs fm: " + se->name, ProgLine::MAIN_LINE);
  prog->AddVerboseLine(this);
  prog->IncIndent();
  prog->AddLine(this, "String sefma_lbl, sefma_argval;");
  prog->AddLine(this, "for(int j=0;j<" + se->name + ".mbrs.leaves;j++) {");
  prog->IncIndent();
  prog->AddLine(this, "EditMbrItem* sei = " + se->name + ".mbrs.Leaf(j);");
  prog->AddLine(this, "if(!sei->is_numeric) continue;");
  prog->AddLine(this, "sefma_lbl = sei->label;");
  prog->AddLine(this, "sefma_argval = taMisc::FindArgByName(sefma_lbl);");
  prog->AddLine(this, "if(sefma_argval.empty()) continue;");
  prog->AddLine(this, "sei->PSearchCurVal_Set(sefma_argval);");
  if(taMisc::dmem_proc == 0) {
    prog->AddLine(this, String("taMisc::Info(\"Set select edit item: \",sefma_lbl,\" in select edit: \",\"") +
		  se->name + "\",\"to val:\",sefma_argval);");
  }
  prog->DecIndent();
  prog->AddLine(this, "}");
  prog->DecIndent();
  prog->AddLine(this, "}");
}

void SelectEditsFmArgs::GenRegArgs(Program* prog) {
  SelectEdit* se = GetSelectEdit();
  if(se) {
    for(int j=0;j<se->mbrs.leaves;j++) {
      EditMbrItem* sei = se->mbrs.Leaf(j);
      if(!sei->is_numeric) continue;
      prog->AddLine(this, "taMisc::AddEqualsArgName(\"" + sei->label + "\");");
      prog->AddLine(this, "taMisc::AddArgNameDesc(\"" + sei->label
		    + "\", \"SelectEditsFmArgs: sel_edit = " + se->name + "\");");
    }
  }
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

void RegisterArgs::GenCssBody_impl(Program* prog) {
  prog->AddLine(this, "// Register Args:", ProgLine::MAIN_LINE);
  prog->AddVerboseLine(this);
  AddArgsFmCode(prog, prog->prog_code);
  prog->AddLine(this, "taMisc::UpdateArgs();");
  prog->AddLine(this, "if(taMisc::CheckArgByName(\"Help\")) taMisc::HelpMsg();"); // extra help!
}

void RegisterArgs::AddArgsFmCode(Program* prog, ProgEl_List& progs) {
  for(int i=0;i<progs.size;i++) {
    ProgEl* pel = progs[i];
    if(pel->InheritsFrom(&TA_ProgVarFmArg)) {
      ProgVarFmArg* pva = (ProgVarFmArg*)pel;
      pva->GenRegArgs(prog);
    }
    else if(pel->InheritsFrom(&TA_MemberFmArg)) {
      MemberFmArg* mfa = (MemberFmArg*)pel;
      mfa->GenRegArgs(prog);
    }
    else if(pel->InheritsFrom(&TA_DataColsFmArgs)) {
      DataColsFmArgs* dca = (DataColsFmArgs*)pel;
      dca->GenRegArgs(prog);
    }
    else if(pel->InheritsFrom(&TA_SelectEditsFmArgs)) {
      SelectEditsFmArgs* sea = (SelectEditsFmArgs*)pel;
      sea->GenRegArgs(prog);
    }
    else {			// look for sub-lists
      TypeDef* td = pel->GetTypeDef();
      for(int j=0;j<td->members.size;j++) {
	MemberDef* md = td->members[j];
	if(md->type->InheritsFrom(&TA_ProgEl_List)) {
	  ProgEl_List* nxt_prgs = (ProgEl_List*)md->GetOff(pel);
	  AddArgsFmCode(prog, *nxt_prgs);
	}
      }
    }
  }
}

