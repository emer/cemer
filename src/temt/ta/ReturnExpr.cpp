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

#include "ReturnExpr.h"
#include <Program>
#include <taMisc>

TA_BASEFUNS_CTORS_DEFN(ReturnExpr);


void ReturnExpr::Initialize() {
}

void ReturnExpr::CheckChildConfig_impl(bool quiet, bool& rval) {
  inherited::CheckChildConfig_impl(quiet, rval);
  expr.CheckConfig(quiet, rval);
}

void ReturnExpr::GenCssBody_impl(Program* prog) {
  expr.ParseExpr();             // re-parse just to be sure!
  prog->AddLine(this, "return " + expr.GetFullExpr() + ";", ProgLine::MAIN_LINE);
  prog->AddVerboseLine(this);
}

String ReturnExpr::GetDisplayName() const {
  String rval;
  rval += "return " + expr.expr;
  return rval;
}

bool ReturnExpr::CanCvtFmCode(const String& code_str, ProgEl* scope_el) const {
  String code = code_str; code.downcase();
  if(code.startsWith("return")) return true;
  return false;
}

bool ReturnExpr::CvtFmCode(const String& code) {
  String cd = trim(code.after("return"));
  if(cd.startsWith('(')) {
    cd = cd.after('(');
    if(cd.endsWith(')')) cd = cd.before(')', -1);
  }
  if(cd.endsWith(';')) cd = cd.before(';',-1);
  expr.SetExpr(cd);
  return true;
}

