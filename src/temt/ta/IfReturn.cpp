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

#include "IfReturn.h"
#include <Program>
#include <taMisc>

TA_BASEFUNS_CTORS_DEFN(IfReturn);


void IfReturn::Initialize() {
}

void IfReturn::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
//   CheckError(cond.expr.empty(), quiet, rval,  "condition expression is empty");
  CheckEqualsError(cond.expr, quiet, rval);
}

void IfReturn::GenCssBody_impl(Program* prog) {
  cond.ParseExpr();             // re-parse just to be sure!
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

bool IfReturn::CanCvtFmCode(const String& code_str, ProgEl* scope_el) const {
  String code = code_str; code.downcase();
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
