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

#include "ForeachLoop.h"
#include <Program>
#include <taMisc>


void ForeachLoop::SetProgExprFlags() {
  // in.SetExprFlag(ProgExpr::FOR_LOOP_EXPR);// requires special parsing
  // iter.SetExprFlag(ProgExpr::FOR_LOOP_EXPR);
}

void ForeachLoop::Initialize() {
  SetProgExprFlags();
}

void ForeachLoop::UpdateAfterEdit_impl() {
  SetProgExprFlags();
  inherited::UpdateAfterEdit_impl();
  if(taMisc::is_loading) return;
  Program* prg = GET_MY_OWNER(Program);
  if(!prg || isDestroying()) return;
  bool clashes = ParentForeachLoopVarClashes();
  if(clashes) {
    taMisc::Warning("foreach variable is same as one used in outer loop, which is usually not a good idea.  variable name is:",
                    el_var->name);
  }
}

bool ForeachLoop::ParentForeachLoopVarClashes() {
  if(!el_var) return false;
  ForeachLoop* outer_loop = GET_MY_OWNER(ForeachLoop);
  while (outer_loop) {
    if(outer_loop->el_var && outer_loop->el_var->name == el_var->name) {
      return true;
    }
    outer_loop = (ForeachLoop*)outer_loop->GetOwner(&TA_ForeachLoop);
  }
  return false;
}

void ForeachLoop::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  CheckError(!el_var, quiet, rval, "el_var variable is not set");
  CheckError(in.expr.empty(), quiet, rval, "in expression is empty");
  CheckError(ParentForeachLoopVarClashes(), quiet, rval, "parent foreach variable is the same, which is usually not a good idea");
}

void ForeachLoop::GenCssPre_impl(Program* prog) {
  in.ParseExpr();               // re-parse just to be sure!
  if(!el_var) return;
  String full_expr = el_var->name + " in " + in.GetFullExpr();
  prog->AddLine(this, String("foreach(") + full_expr + ") {", ProgLine::MAIN_LINE);
  prog->AddVerboseLine(this, true, "\"before entering loop\""); // move to start
  prog->IncIndent();
  prog->AddVerboseLine(this, false, "\"starting in loop\""); // don't move to out
}

void ForeachLoop::GenCssPost_impl(Program* prog) {
  if(!el_var) return;
  prog->DecIndent();
  prog->AddLine(this, "}");
}

String ForeachLoop::GetDisplayName() const {
  String elnm = "<el_var not set>";
  if(el_var)
    elnm = el_var->name;
  return "foreach(" + elnm + " in " + in.expr + ")";
}

bool ForeachLoop::CanCvtFmCode(const String& code, ProgEl* scope_el) const {
  if(code.startsWith("foreach(") || code.startsWith("foreach (")) return true;
  return false;
}

bool ForeachLoop::CvtFmCode(const String& code) {
  String cd = trim(code.after("foreach"));
  if(cd.startsWith('(')) {
    cd = cd.after('(');
    if(cd.endsWith(')'))
      cd = cd.before(')', -1);
  }
  String inexpr = trim(cd.after("in"));
  in.SetExpr(inexpr);
  String varexpr = trim(cd.before("in"));
  el_var = FindVarNameInScope(varexpr, true); // option to make
  return true;
}

