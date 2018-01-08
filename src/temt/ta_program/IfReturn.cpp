// Copyright 2013-2018, Regents of the University of Colorado,
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

#include "IfReturn.h"
#include <If>
#include <ReturnExpr>
#include <Program>
#include <taMisc>

TA_BASEFUNS_CTORS_DEFN(IfReturn);


void IfReturn::Initialize() {
}

void IfReturn::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  if(taMisc::is_loading) {
    ConvertToReturnExpr();
  }
}

void IfReturn::ConvertToReturnExpr() {
  if(!owner) return;
  if(!owner->InheritsFrom(&TA_ProgEl_List)) return;
  ProgEl_List* own = (ProgEl_List*)owner;
  if (cond.var_expr.empty()) {
    ReturnExpr* ret_stmt = new ReturnExpr;
    ret_stmt->flags = flags;         // get our flags
    own->ReplaceLater(this, ret_stmt);
  }
  else {
    If* if_stmt = new If;
    if_stmt->flags = flags;
    if_stmt->cond.Copy(cond);
    
    ReturnExpr* ret_stmt = new ReturnExpr;
    ret_stmt->flags = flags;
    if_stmt->true_code.Add(ret_stmt);
    
    own->ReplaceLater(this, if_stmt);
  }
}

void IfReturn::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
//   CheckError(cond.expr.empty(), quiet, rval,  "condition expression is empty");
  CheckEqualsError(cond.expr, quiet, rval);
}

bool IfReturn::GenCssBody_impl(Program* prog) {
  cond.ParseExpr();             // re-parse just to be sure!
  String fexp = cond.GetFullExpr();
  if(fexp.nonempty()) {
    prog->AddLine(this, "if(" + fexp + ") {", ProgLine::MAIN_LINE);
    prog->AddVerboseLine(this, true, "\"before if\"");
    prog->IncIndent();
    prog->AddLine(this, "return;");
    prog->AddVerboseLine(this, false, "\"inside if -- returning\"");
    prog->DecIndent();
    prog->AddLine(this, "}");
  }
  else {
    prog->AddLine(this, "return;", ProgLine::MAIN_LINE);
    prog->AddVerboseLine(this);
  }
  return true;
}

String IfReturn::GetDisplayName() const {
  if(cond.expr.empty())
    return "return";
  else
    return "if(" + cond.GetFullExpr() + ") return";
}

bool IfReturn::CanCvtFmCode(const String& code_str, ProgEl* scope_el) const {
  return false; // obsolete - gets replaced on load by separate if and return statements
//  String code = code_str; code.downcase();
//  if(code.startsWith("if") && code.contains("return")) return true;
//  if(code == "return") return true;
//  return false;
}

bool IfReturn::CvtFmCode(const String& code) {
  return false;
//  String cd = trim(code.after("if"));
//  cd = trim(cd.before("return"));
//  if(cd.startsWith('(')) {
//    cd = cd.after('(');
//    if(cd.endsWith(')'))
//      cd = cd.before(')', -1);
//  }
//  cond.SetExpr(cd);
//  return true;
}
