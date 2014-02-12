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

#include "IfElse.h"
#include <Program>
#include <taMisc>

#include <css_machine.h>

TA_BASEFUNS_CTORS_DEFN(IfElse);


void IfElse::Initialize() {
  show_false_code = true;
}

void IfElse::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  if(taMisc::is_loading) {
    if(false_code.size == 0)
      show_false_code = false;
  }
  if(false_code.size > 0)
    show_false_code = true;
}

void IfElse::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  CheckError(cond.expr.empty(), quiet, rval,  "condition expression is empty");
  CheckEqualsError(cond.expr, quiet, rval);
}

void IfElse::CheckChildConfig_impl(bool quiet, bool& rval) {
  inherited::CheckChildConfig_impl(quiet, rval);
  true_code.CheckConfig(quiet, rval);
  false_code.CheckConfig(quiet, rval);
}

void IfElse::GenCssPre_impl(Program* prog) {
  cond.ParseExpr();             // re-parse just to be sure!
  prog->AddLine(this, "if(" + cond.GetFullExpr() + ") {", ProgLine::MAIN_LINE);
  prog->AddVerboseLine(this, true, "\"before if\"");
  prog->IncIndent();
  prog->AddVerboseLine(this, false, "\"inside if\"");
}

void IfElse::GenCssBody_impl(Program* prog) {
  true_code.GenCss(prog);
  // don't gen 'else' portion unless there are els
  if (false_code.size > 0) {
    prog->DecIndent();
    prog->AddLine(this, "} else {");
    prog->IncIndent();
    prog->AddVerboseLine(this, false, "\"inside else\"");
    false_code.GenCss(prog);
  }
}

void IfElse::GenCssPost_impl(Program* prog) {
  prog->DecIndent();
  prog->AddLine(this, "}");
}

const String IfElse::GenListing_children(int indent_level) {
  String rval = true_code.GenListing(indent_level + 1);
  if (false_code.size > 0) {
    rval += cssMisc::Indent(indent_level) + "else\n";
    rval += false_code.GenListing(indent_level + 1);
  }
  return rval;
}

void IfElse::PreGenChildren_impl(int& item_id) {
  inherited::PreGenChildren_impl(item_id);
  true_code.PreGen(item_id);
  false_code.PreGen(item_id);
}

ProgVar* IfElse::FindVarName(const String& var_nm) const {
  ProgVar* pv = inherited::FindVarName(var_nm);
  if (pv) return pv;
  pv = true_code.FindVarName(var_nm);
  if(pv) return pv;
  return false_code.FindVarName(var_nm);
}

String IfElse::GetDisplayName() const {
  return "if (" + cond.expr + ")";
}

bool IfElse::CanCvtFmCode(const String& code, ProgEl* scope_el) const {
  if(code.startsWith("if")) return true;
  return false;
}

bool IfElse::CvtFmCode(const String& code) {
  String cd = trim(code.after("if"));
  if(cd.startsWith('(')) {
    cd = cd.after('(');
    if(cd.endsWith(')'))
      cd = cd.before(')', -1);
  }
  if(cd.endsWith(';')) cd = cd.before(';',-1);
  cond.SetExpr(cd);
  return true;
}
