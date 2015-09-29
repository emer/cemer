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

#include "CaseBlock.h"
#include <Program>
#include <Switch>
#include <taMisc>

TA_BASEFUNS_CTORS_DEFN(CaseBlock);


void CaseBlock::Initialize() {
  is_default = false;
  
  // these are the same as for Program prog_code
  prog_code.AddUnacceptableType("CaseBlock");
  prog_code.AddUnacceptableType("IfBreak");
  prog_code.AddUnacceptableType("IfContinue");
  prog_code.AddUnacceptableType("DataCalcAddDestRow");
  prog_code.AddUnacceptableType("DataCalcSetDestRow");
  prog_code.AddUnacceptableType("DataCalcSetSrcRow");
  prog_code.AddUnacceptableType("DataCalcCopyCommonCols");
  prog_code.check_with_parent = false; // stop here
}

void CaseBlock::Destroy() {
  CutLinks();
}

void CaseBlock::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  if(taMisc::is_loading) {
    taVersion v784(7, 8, 4);
    if (taMisc::loading_version < v784) {
      if (case_val.expr.empty()) {
        is_default = true;
      }
    }
  }
  // user can toggle the is_default case
  if (is_default) {
    case_val.expr = "";
  }
}

void CaseBlock::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  CheckError(!is_default && case_val.empty(), quiet, rval,
             "case value expression is empty for a non-default case!");
}

void CaseBlock::GenCssPre_impl(Program* prog) {
  case_val.ParseExpr();         // re-parse just to be sure!
  if(prog_code.size == 0) return;
  String expr = case_val.GetFullExpr();
  if(is_default)
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
  if(is_default) return "default:";
  return "case: " + case_val.expr;
}

bool CaseBlock::CanCvtFmCode(const String& code_str, ProgEl* scope_el) const {
  String code = code_str; code.downcase();
  if(code.startsWith("case") || code.startsWith("default")) return true;
  return false;
}

bool CaseBlock::CvtFmCode(const String& code) {
  String cd;
  if(code.startsWith("case:")) cd = trim(code.after("case:"));
  else if(code.startsWith("case")) cd = trim(code.after("case"));
  else if(code.startsWith("default:")) cd = trim(code.after("default:"));
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

bool CaseBlock::ChooseMe() {
  case_val.expr = "value"; // make into a regular case by making expression non-empty
  UpdateAfterEdit();
  Switch* sw = dynamic_cast<Switch*>(GetParent());
  if (sw) {
    if (sw->HasDefaultCase()) {
      // I'd better explain the next part
      // if it is a regular case it should precede the last item (i.e. the default case)
      CaseBlock* default_case = dynamic_cast<CaseBlock*>(sw->cases[sw->cases.size-2]);
      if (default_case) {
        sw->cases.MoveBefore(default_case, this);
      }
    }
    return true;
  }
  return false;
}
