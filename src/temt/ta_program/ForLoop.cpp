// Copyright 2013-2017, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of The Emergent Toolkit
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

#include "ForLoop.h"
#include <Program>
#include <Function>
#include <ProgExpr>
#include <LocalVars>

#include <taMisc>

TA_BASEFUNS_CTORS_DEFN(ForLoop);

void ForLoop::SetProgExprFlags() {
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
  // note: no need to call UAE here or anything -- indeed it is bad!
}

void ForLoop::UpdateAfterEdit_impl() {
  SetProgExprFlags();
  inherited::UpdateAfterEdit_impl();
  if(taMisc::is_loading) return;
  Program* prg = GET_MY_OWNER(Program);
  if(!prg || isDestroying()) return;
  if(init.expr == "_toolbox_tmp_") { // special code-word for toolbox version
    init.expr = "i = 0";
    init.ParseExpr();
    if(!UpdateVarClashes()) {
      MakeIndexVar("i");
    }
  }
  else {
    UpdateVarClashes();
  }
  // whatever we do, make sure these guys are parsed!
  init.ParseExpr();
  test.ParseExpr();
  iter.ParseExpr();
}

ProgVar* ForLoop::GetLoopVar() {
  // use init expression for the loop variable -- most likely to just contain 1 item, and in any case should be first item
  ProgExprBase* peb = &init;
  peb->ParseExpr();
  if(peb->vars.size == 0) {     // try iter then
    peb = &iter;
    peb->ParseExpr();
    if(peb->vars.size == 0) {
      peb = &test;
      peb->ParseExpr();
    }
  }
  if(peb->vars.size == 0) {     // couldn't get nothing! could do err msg
    return NULL;
  }
  ProgVar* rval = NULL;
  for(int i=0; i<peb->vars.size; i++) {
    ProgVarRef* vrf = peb->vars.FastEl(i);
    if(!vrf->ptr()) continue;   // shouldn't happen..
    // todo: could do some things here to test but for now just take the first guy!
    rval = vrf->ptr();
    break;                      // get the first one!
  }
  return rval;
}

bool ForLoop::UpdateVarClashes() {
  ProgVar* var = GetLoopVar();
  if(!var) return false;
  bool clashes = ParentForLoopVarClashes(var->name);
  if (!clashes) return false;
  String new_loop_var = var->name;
  while (clashes) {
    MorphVar(new_loop_var);
    clashes = ParentForLoopVarClashes(new_loop_var);
  }
  ChangeLoopVar(new_loop_var);
  return true;
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
    ProgVar* outer_loop_var = outer_loop->GetLoopVar();
    if (loop_var == outer_loop_var->name) return true;
    outer_loop = (ForLoop*)outer_loop->GetOwner(&TA_ForLoop);
  }
  return false;
}

ProgVar* ForLoop::MakeIndexVar(const String& var_nm) {
  if(var_nm.empty()) return NULL;

  Program* my_prog = GET_MY_OWNER(Program);
  if(!my_prog) return NULL;
  Function* my_fun = GET_MY_OWNER(Function);

  ProgVar* rval = NULL;
  if(my_fun) {                  // use function scope by default
    rval = my_fun->FindVarName(var_nm);
    if(rval) return rval;
  }
  rval = my_prog->FindVarName(var_nm);
  if(rval) return rval;

  LocalVars* locvars = FindLocalVarList();
  if(locvars) {
    rval = locvars->AddVar();
  }
  else {
    rval = (ProgVar*)my_prog->vars.New(1, &TA_ProgVar);
  }
  if(rval) {
    rval->name = var_nm;
    rval->SetInt(0);
    rval->ClearVarFlag(ProgVar::CTRL_PANEL);
    rval->SigEmitUpdated();
  }
  return rval;
}

void ForLoop::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  CheckError(test.expr.empty(), quiet, rval, "test expression is empty");
  CheckError(iter.expr.empty(), quiet, rval, "iter expression is empty");
}

void ForLoop::GenCssPre_impl(Program* prog) {
  init.ParseExpr();             // re-parse just to be sure!
  test.ParseExpr();             // re-parse just to be sure!
  iter.ParseExpr();             // re-parse just to be sure!
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
  ProgVar* cur_var = GetLoopVar();
  ProgVar* new_var = MakeIndexVar(to_var);
  if(cur_var && new_var) {
    init.ReplaceVar(cur_var, new_var);
    test.ReplaceVar(cur_var, new_var);
    iter.ReplaceVar(cur_var, new_var);
  }
  SigEmitUpdated();
}

bool ForLoop::CanCvtFmCode(const String& code_str, ProgEl* scope_el) const {
  String code = code_str; code.downcase();
  if(code.startsWith("for")) {
    String remainder_code = trim(code.after("for"));
    if (remainder_code.startsWith('(')) {
      return true;
    }
  }
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
  init.expr = trim(cd.before(';')); // IMPORTANT: don't use SetExpr here --- will trigger full UAE before rest of expressions are updated!
  String rest = cd.after(';');
  test.expr = trim(rest.before(';'));
  iter.expr = trim(rest.after(';'));
  // NOW re-parse everything
  init.ReParseExpr();
  test.ReParseExpr();
  iter.ReParseExpr();
  UpdateVarClashes();
  SigEmitUpdated();
  // uae will be auto-called after this point, to update us
  return true;
}
