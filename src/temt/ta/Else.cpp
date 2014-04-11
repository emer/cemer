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

#include "Else.h"
#include <If>
#include <ElseIf>
#include <Program>
#include <taMisc>

#include <css_machine.h>

TA_BASEFUNS_CTORS_DEFN(Else);

void Else::Initialize() {
}

void Else::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  CheckAfterIf();
}

bool Else::CheckAfterIf() {
  if(!owner) return false;
  if(!owner->InheritsFrom(&TA_ProgEl_List)) return false;
  ProgEl_List* own = (ProgEl_List*)owner;
  int idx = own->FindEl(this);
  if(TestError(idx == 0, "CheckAfterIf",
               "else statement is first element in code list -- must come after an if")) {
    return false;
  }
  ProgEl* prv = own->SafeEl(idx-1);
  if(TestError(!prv, "CheckAfterIf",
               "else statement comes after a NULL previous element -- must come after an if")) {
    return false;
  }
  if(TestError(!prv->InheritsFrom(&TA_If) && !prv->InheritsFrom(&TA_ElseIf),
               "CheckAfterIf",
               "else statement does not come after an If or ElseIf -- it must -- instead it comes after a:", prv->GetTypeDef()->name)) {
    return false;
  }
  return true;
}

void Else::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  rval = CheckAfterIf();
}

void Else::CheckChildConfig_impl(bool quiet, bool& rval) {
  inherited::CheckChildConfig_impl(quiet, rval);
  else_code.CheckConfig(quiet, rval);
}

void Else::GenCssPre_impl(Program* prog) {
  prog->AddLine(this, "else {", ProgLine::MAIN_LINE);
  prog->AddVerboseLine(this, true, "\"before else\"");
  prog->IncIndent();
  prog->AddVerboseLine(this, false, "\"inside else\"");
}

void Else::GenCssBody_impl(Program* prog) {
  else_code.GenCss(prog);
}

void Else::GenCssPost_impl(Program* prog) {
  prog->DecIndent();
  prog->AddLine(this, "}");
}

const String Else::GenListing_children(int indent_level) {
  String rval = else_code.GenListing(indent_level + 1);
  return rval;
}

void Else::PreGenChildren_impl(int& item_id) {
  inherited::PreGenChildren_impl(item_id);
  else_code.PreGen(item_id);
}

ProgVar* Else::FindVarName(const String& var_nm) const {
  ProgVar* pv = inherited::FindVarName(var_nm);
  if (pv) return pv;
  return else_code.FindVarName(var_nm);
}

String Else::GetDisplayName() const {
  return "else";
}

bool Else::CanCvtFmCode(const String& code_str, ProgEl* scope_el) const {
  String code = code_str; code.downcase();
  if(code.startsWith("else") && !code.contains("if")) return true;
  return false;
}

bool Else::CvtFmCode(const String& code) {
  return true;
}
