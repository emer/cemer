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

#include "IfContinue.h"
#include <Program>
#include <taMisc>

TA_BASEFUNS_CTORS_DEFN(IfContinue);


void IfContinue::Initialize() {
}

void IfContinue::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
//   CheckError(cond.expr.empty(), quiet, rval,  "condition expression is empty");
  CheckEqualsError(cond.expr, quiet, rval);
}

void IfContinue::GenCssBody_impl(Program* prog) {
  cond.ParseExpr();             // re-parse just to be sure!
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

bool IfContinue::CanCvtFmCode(const String& code_str, ProgEl* scope_el) const {
  String code = code_str; code.downcase();
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

