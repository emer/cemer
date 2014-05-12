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

#include "If.h"
#include <Program>
#include <Else>
#include <taMisc>

#include <css_machine.h>

TA_BASEFUNS_CTORS_DEFN(If);

void If::Initialize() {
}

void If::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  if(taMisc::is_loading) {
    if(false_code.size > 0) {
      ConvertFromIfElse();
    }
  }
}

void If::ConvertFromIfElse() {
  if(!owner) return;
  if(!owner->InheritsFrom(&TA_ProgEl_List)) return;
  ProgEl_List* own = (ProgEl_List*)owner;
  int idx = own->FindEl(this);
  Else* nelse = new Else;
  own->Insert(nelse, idx+1);
  while(false_code.size > 0) {
    nelse->else_code.Transfer(false_code[0]);
  }
  nelse->flags = flags;         // get our flags
}

void If::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  CheckError(cond.expr.empty(), quiet, rval,  "condition expression is empty");
  CheckEqualsError(cond.expr, quiet, rval);
}

void If::CheckChildConfig_impl(bool quiet, bool& rval) {
  inherited::CheckChildConfig_impl(quiet, rval);
  true_code.CheckConfig(quiet, rval);
}

void If::GenCssPre_impl(Program* prog) {
  cond.ParseExpr();             // re-parse just to be sure!
  prog->AddLine(this, "if(" + cond.GetFullExpr() + ") {", ProgLine::MAIN_LINE);
  prog->AddVerboseLine(this, true, "\"before if\"");
  prog->IncIndent();
  prog->AddVerboseLine(this, false, "\"inside if\"");
}

void If::GenCssBody_impl(Program* prog) {
  true_code.GenCss(prog);
}

void If::GenCssPost_impl(Program* prog) {
  prog->DecIndent();
  prog->AddLine(this, "}");
}

const String If::GenListing_children(int indent_level) {
  String rval = true_code.GenListing(indent_level + 1);
  return rval;
}

void If::PreGenChildren_impl(int& item_id) {
  inherited::PreGenChildren_impl(item_id);
  true_code.PreGen(item_id);
}

ProgVar* If::FindVarName(const String& var_nm) const {
  ProgVar* pv = inherited::FindVarName(var_nm);
  if (pv) return pv;
  return true_code.FindVarName(var_nm);
}

String If::GetDisplayName() const {
  return "if (" + cond.expr + ")";
}

bool If::CanCvtFmCode(const String& code_str, ProgEl* scope_el) const {
  String code = code_str; code.downcase();
  if(code.startsWith("if(") || code.startsWith("if ")) return true;
  return false;
}

bool If::CvtFmCode(const String& code) {
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
