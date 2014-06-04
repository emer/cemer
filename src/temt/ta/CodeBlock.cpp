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

#include "CodeBlock.h"
#include <Program>
#include <taMisc>

TA_BASEFUNS_CTORS_DEFN(CodeBlock);


void CodeBlock::Initialize() {
}

void CodeBlock::CheckChildConfig_impl(bool quiet, bool& rval) {
  inherited::CheckChildConfig_impl(quiet, rval);
  prog_code.CheckConfig(quiet, rval);
}

void CodeBlock::GenCssPre_impl(Program* prog) {
  if(prog_code.size == 0) return;
  prog->AddLine(this, "{", ProgLine::MAIN_LINE);
  prog->IncIndent();
}

void CodeBlock::GenCssBody_impl(Program* prog) {
  if(prog_code.size == 0) return;
  prog_code.GenCss(prog);
}

void CodeBlock::GenCssPost_impl(Program* prog) {
  if(prog_code.size == 0) return;
  prog->DecIndent();
  prog->AddLine(this, "}");
}

const String CodeBlock::GenListing_children(int indent_level) {
  return prog_code.GenListing(indent_level + 1);
}

String CodeBlock::GetDisplayName() const {
  return "CodeBlock (" + String(prog_code.size) + " items)";
}

void CodeBlock::PreGenChildren_impl(int& item_id) {
  prog_code.PreGen(item_id);
}
ProgVar* CodeBlock::FindVarName(const String& var_nm) const {
  return prog_code.FindVarName(var_nm);
}

bool CodeBlock::CanCvtFmCode(const String& code, ProgEl* scope_el) const {
  if(code.startsWith("{")) return true;
  return false;
}

bool CodeBlock::CvtFmCode(const String& code) {
  // just open bracket is all there is!
  return true;
}

bool CodeBlock::BrowserEditTest() {
  bool rval = inherited::BrowserEditTest();
  bool rv2 = prog_code.BrowserEditTest();
  return rval | rv2;
}
