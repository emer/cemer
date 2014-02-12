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

#include "VarIncr.h"
#include <Program>
#include <taMisc>

TA_BASEFUNS_CTORS_DEFN(VarIncr);


void VarIncr::Initialize() {
  expr.expr = "1";
}

void VarIncr::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  CheckError(!var, quiet, rval, "var is NULL");
  expr.CheckConfig(quiet, rval);
}

void VarIncr::GenCssBody_impl(Program* prog) {
  expr.ParseExpr();             // re-parse just to be sure!
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
