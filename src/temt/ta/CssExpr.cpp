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

#include "CssExpr.h"
#include <Program>
#include <taMisc>

TA_BASEFUNS_CTORS_DEFN(CssExpr);


void CssExpr::Initialize() {
  expr.SetExprFlag(ProgExpr::FULL_STMT); // full statements for parsing
}

void CssExpr::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  expr.CheckConfig(quiet, rval);
}

void CssExpr::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  // must always end with a semicolon!!
  if(!expr.expr.endsWith(';')) {
    taMisc::Info("note: css expr must always end with a semicolon");
    expr.expr += ';';
  }
}

void CssExpr::GenCssBody_impl(Program* prog) {
  expr.ParseExpr();             // re-parse just to be sure!
  String rval = expr.GetFullExpr();
  if(!rval.endsWith(';'))
    rval += ';';
  if(rval.endsWith("};"))
    rval = rval.before(rval.length()-1); // cut off last ;
  prog->AddLine(this, rval, ProgLine::MAIN_LINE);
  // no verbose here!
}

String CssExpr::GetDisplayName() const {
  return expr.GetFullExpr();
}

bool CssExpr::CanCvtFmCode(const String& code, ProgEl* scope_el) const {
  if(code.endsWith(';')) return true; // only criterion -- if you include the ;
  return false;
}

bool CssExpr::CvtFmCode(const String& code) {
  expr.SetExpr(code);
  return true;
}
