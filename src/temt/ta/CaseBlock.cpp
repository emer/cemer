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
#include <taMisc>

TA_BASEFUNS_CTORS_DEFN(CaseBlock);


void CaseBlock::Initialize() {
}

void CaseBlock::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  //  CheckError(case_val.empty(), quiet, rval,  "case value expression is empty!");
}

void CaseBlock::GenCssPre_impl(Program* prog) {
  case_val.ParseExpr();         // re-parse just to be sure!
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

