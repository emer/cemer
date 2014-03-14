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

#include "ProgType.h"
#include <Program>
#include <css_machine.h>

TA_BASEFUNS_CTORS_DEFN(ProgType);
SMARTREF_OF_CPP(ProgType); // ProgTypeRef


void ProgType::Initialize() {
  setUseStale(true);
}

void ProgType::Destroy() {
  CutLinks();
}

void ProgType::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  if(Program::IsForbiddenName(name)) {
    name = "My" + name;
  }
}

void ProgType::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  CheckError(Program::IsForbiddenName(name, false), quiet, rval,
	     "Name:",name,"is a css reserved name used for something else -- please choose another name");
}

taBase* ProgType::FindTypeName(const String& nm) const {
  if(name == nm) return (taBase*)this;
  return NULL;
}

const String ProgType::GenCssType() const {
  return "";
}

void ProgType::GenCss(Program* prog) {
  prog->AddDescString(this, desc);
  GenCssPre_impl(prog);
  GenCssBody_impl(prog);
  GenCssPost_impl(prog);
}

const String ProgType::GenListing(int indent_level) {
  String rval = Program::GetDescString(desc, indent_level);
  rval += cssMisc::Indent(indent_level) + GetDisplayName() + "\n";
  return rval;
}

bool ProgType::BrowserEditSet(const String& code, int move_after) {
  String cd = CodeGetDesc(code);
  return inherited::BrowserEditSet(cd, move_after);
}

String ProgType::CodeGetDesc(const String& code) {
  if(code.contains("//")) {
    desc = trim(code.after("//"));
    return trim(code.before("//"));
  }
  if(code.contains("/*")) {
    desc = trim(code.after("/*"));
    if(desc.contains("*/"))
      desc = trim(desc.before("*/",-1));
    return trim(code.before("/*"));
  }
  return code;
}

bool ProgType::BrowserSelectMe() {
  Program* prog = GET_MY_OWNER(Program);
  if(!prog) return false;
  return prog->BrowserSelectMe_ProgItem(this);
}

bool ProgType::BrowserExpandAll() {
  Program* prog = GET_MY_OWNER(Program);
  if(!prog) return false;
  return prog->BrowserExpandAll_ProgItem(this);
}

bool ProgType::BrowserCollapseAll() {
  Program* prog = GET_MY_OWNER(Program);
  if(!prog) return false;
  return prog->BrowserCollapseAll_ProgItem(this);
}


