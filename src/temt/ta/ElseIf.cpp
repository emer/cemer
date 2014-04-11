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

#include "ElseIf.h"
#include <Program>
#include <If>
#include <taMisc>

#include <css_machine.h>

TA_BASEFUNS_CTORS_DEFN(ElseIf);

void ElseIf::Initialize() {
}

void ElseIf::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  CheckAfterIf();
}

bool ElseIf::CheckAfterIf() {
  if(!owner) return false;
  if(!owner->InheritsFrom(&TA_ProgEl_List)) return false;
  ProgEl_List* own = (ProgEl_List*)owner;
  int idx = own->FindEl(this);
  if(TestError(idx == 0, "CheckAfterIf",
               "ElseIf statement is first element in code list -- must come after an if")) {
    return false;
  }
  ProgEl* prv = own->SafeEl(idx-1);
  if(TestError(!prv, "CheckAfterIf",
               "ElseIf statement comes after a NULL previous element -- must come after an if")) {
    return false;
  }
  if(TestError(!prv->InheritsFrom(&TA_If) && !prv->InheritsFrom(&TA_ElseIf),
               "CheckAfterIf",
               "ElseIf statement does not come after an If or ElseIf -- it must -- instead it comes after a:", prv->GetTypeDef()->name)) {
    return false;
  }
  return true;
}

void ElseIf::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  CheckError(cond.expr.empty(), quiet, rval,  "condition expression is empty");
  CheckEqualsError(cond.expr, quiet, rval);
  rval = CheckAfterIf();
}

void ElseIf::CheckChildConfig_impl(bool quiet, bool& rval) {
  inherited::CheckChildConfig_impl(quiet, rval);
  true_code.CheckConfig(quiet, rval);
}

void ElseIf::GenCssPre_impl(Program* prog) {
  cond.ParseExpr();             // re-parse just to be sure!
  prog->AddLine(this, "else if(" + cond.GetFullExpr() + ") {", ProgLine::MAIN_LINE);
  prog->AddVerboseLine(this, true, "\"before else if\"");
  prog->IncIndent();
  prog->AddVerboseLine(this, false, "\"inside else if\"");
}

void ElseIf::GenCssBody_impl(Program* prog) {
  true_code.GenCss(prog);
}

void ElseIf::GenCssPost_impl(Program* prog) {
  prog->DecIndent();
  prog->AddLine(this, "}");
}

const String ElseIf::GenListing_children(int indent_level) {
  String rval = true_code.GenListing(indent_level + 1);
  return rval;
}

void ElseIf::PreGenChildren_impl(int& item_id) {
  inherited::PreGenChildren_impl(item_id);
  true_code.PreGen(item_id);
}

ProgVar* ElseIf::FindVarName(const String& var_nm) const {
  ProgVar* pv = inherited::FindVarName(var_nm);
  if (pv) return pv;
  return true_code.FindVarName(var_nm);
}

String ElseIf::GetDisplayName() const {
  return "else if (" + cond.expr + ")";
}

bool ElseIf::CanCvtFmCode(const String& code_str, ProgEl* scope_el) const {
  String code = code_str; code.downcase();
  if(code.startsWith("else if")) return true;
  return false;
}

bool ElseIf::CvtFmCode(const String& code) {
  String cd = trim(code.after("else if"));
  if(cd.startsWith('(')) {
    cd = cd.after('(');
    if(cd.endsWith(')'))
      cd = cd.before(')', -1);
  }
  if(cd.endsWith(';')) cd = cd.before(';',-1);
  cond.SetExpr(cd);
  return true;
}
