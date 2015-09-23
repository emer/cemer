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

#include "AssignExpr.h"
#include <Program>
#include <taMisc>

#include <taiWidgetTokenChooser>

TA_BASEFUNS_CTORS_DEFN(AssignExpr);

void AssignExpr::Initialize() {
}

void AssignExpr::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  CheckError(!result_var, quiet, rval, "result_var is NULL");
  expr.CheckConfig(quiet, rval);
}

void AssignExpr::GenCssBody_impl(Program* prog) {
  expr.ParseExpr();             // re-parse just to be sure!
  if (!result_var) {
    prog->AddLine(this, "// WARNING: AssignExpr not generated here -- result_var not specified", ProgLine::MAIN_LINE);
    return;
  }

  prog->AddLine(this, result_var->name + " = " + expr.GetFullExpr() + ";", ProgLine::MAIN_LINE);
  prog->AddVerboseLine(this, true, "\"prev value:\", String(" + result_var->name + ")"); // moved above
  prog->AddVerboseLine(this, false, "\"new  value:\", String(" + result_var->name + ")"); // after
}

String AssignExpr::GetDisplayName() const {
  String rval;
  if(result_var)
    rval = result_var->name;
  else
    rval = "?";
  rval += " = " + expr.GetFullExpr();
  return rval;
}

bool AssignExpr::CanCvtFmCode(const String& code, ProgEl* scope_el) const {
  // note: AssignExpr is specifically excluded if multiple matches, so no need to exclude
  // all the other things that might have an = in them -- it is just a fallback default
  if(code.endsWith(';')) return false; // don't pick up css exprs
  if(code.freq('=') >= 1) {
    String lhs = code.before('=');
    if(lhs.nonempty() && !lhs.contains('.') && !lhs.contains('-') && !lhs.contains('['))
      // no path, matrix
      return true;
  }
  return false;
}

bool AssignExpr::CvtFmCode(const String& code) {
  String lhs = trim(code.before('='));
  String rhs = trim(code.after('='));
  if(rhs.endsWith(';')) rhs = rhs.before(';',-1);

  if(lhs == "?") return false;
  result_var = FindVarNameInScope(lhs, true); // option to make
  expr.SetExpr(rhs);

  return true;
}

bool AssignExpr::ChooseMe() {
  // first get the object
  if (!result_var) {
    taiWidgetTokenChooser* chooser =  new taiWidgetTokenChooser(&TA_ProgVar, NULL, NULL, NULL, 0, "");
    chooser->SetTitleText("Choose the variable you want to set");
    Program* scope_program = GET_MY_OWNER(Program);
    chooser->GetImageScoped(NULL, &TA_ProgVar, scope_program, &TA_Program); // scope to this guy
    bool okc = chooser->OpenChooser();
    if(okc && chooser->token()) {
      result_var = (ProgVar*)chooser->token();
      UpdateAfterEdit();
    }
    delete chooser;
  }
  return true;
}
