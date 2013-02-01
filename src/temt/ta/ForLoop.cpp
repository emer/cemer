// Copyright, 1995-2013, Regents of the University of Colorado,
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
    MakeIndexVar(loop_var);     // make sure it exists
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

  if(my_fun) {                  // use function scope by default
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
    var->SigEmitUpdated();
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
  UpdateAfterEdit_impl();       // make local var
  return true;
}
