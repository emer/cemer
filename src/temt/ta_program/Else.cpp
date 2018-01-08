// Copyright 2014-2018, Regents of the University of Colorado,
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
  // if check happens in GenProgName
}

String Else::GenProgName() const {
  CondBase* prv = FindPriorIf(true); // this is called in UAE so this is source of errors
  String nm;
  if(prv) {
    nm = prv->name;
  }
  return GetTypeDef()->name + "_" + nm;
}

CondBase* Else::FindPriorIf(bool err_msgs) const {
  if(!owner) return NULL;
  if(!owner->InheritsFrom(&TA_ProgEl_List)) return NULL;
  ProgEl_List* own = (ProgEl_List*)owner;
  int idx = own->FindEl(this);
  if(idx == 0) {
    TestError(err_msgs, "FindPriorIf",      
              "else statement is first element in code list -- must come after an if");
    return NULL;
  }
  ProgEl* prv = own->SafeEl(idx-1);
  if(!prv) {
    TestError(err_msgs, "FindPriorIf",
              "else statement comes after a NULL previous element -- must come after an if");
    return NULL;
  }
  if(!(prv->InheritsFrom(&TA_If) || prv->InheritsFrom(&TA_ElseIf))) {
    TestError(err_msgs, "FindPriorIf",
              "else statement does not come after an If or ElseIf -- it must -- instead it comes after a:", prv->GetTypeDef()->name);
    return NULL;
  }
  ProgEl* post = own->SafeEl(idx+1);
  if (post) {
    if(post->InheritsFrom(&TA_Else) || post->InheritsFrom(&TA_ElseIf)) {
      TestError(err_msgs, "CheckNotBeforeElse",
                "Else can not precede another Else or ElseIf");
    }
  }
  return (CondBase*)prv;
}

void Else::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  CondBase* prv = FindPriorIf(true);
  if(!prv) {
    rval = false;
  }
}

void Else::CheckChildConfig_impl(bool quiet, bool& rval) {
  inherited::CheckChildConfig_impl(quiet, rval);
  true_code.CheckConfig(quiet, rval);
}

void Else::GenCssPre_impl(Program* prog) {
  prog->AddLine(this, "else {", ProgLine::MAIN_LINE);
  prog->AddVerboseLine(this, true, "\"before else\"");
  prog->IncIndent();
  prog->AddVerboseLine(this, false, "\"inside else\"");
}

bool Else::GenCssBody_impl(Program* prog) {
  true_code.GenCss(prog);
  return true;
}

void Else::GenCssPost_impl(Program* prog) {
  prog->DecIndent();
  prog->AddLine(this, "}");
}

const String Else::GenListing_children(int indent_level) const {
  String rval = true_code.GenListing(indent_level + 1);
  return rval;
}

void Else::PreGenChildren_impl(int& item_id) {
  inherited::PreGenChildren_impl(item_id);
  true_code.PreGen(item_id);
}

ProgVar* Else::FindVarName(const String& var_nm) const {
  ProgVar* pv = inherited::FindVarName(var_nm);
  if (pv) return pv;
  return true_code.FindVarName(var_nm);
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

bool Else::BrowserEditTest() {
  bool rval = inherited::BrowserEditTest();
  bool rv2 = true_code.BrowserEditTest();
  return rval && rv2;
}
