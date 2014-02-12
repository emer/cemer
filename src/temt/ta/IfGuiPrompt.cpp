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

#include "IfGuiPrompt.h"
#include <Program>
#include <taMisc>

TA_BASEFUNS_CTORS_DEFN(IfGuiPrompt);


void IfGuiPrompt::Initialize() {
  prompt = "Do you want to...";
  yes_label = "Yes";
  no_label = "No";
}

void IfGuiPrompt::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  prompt.gsub("\"", "''");
}

void IfGuiPrompt::CheckChildConfig_impl(bool quiet, bool& rval) {
  inherited::CheckChildConfig_impl(quiet, rval);
  yes_code.CheckConfig(quiet, rval);
}

void IfGuiPrompt::GenCssPre_impl(Program* prog) {
  if(taMisc::gui_active) {
    prog->AddLine(this, "{ int chs = taMisc::Choice(\"" + prompt + "\", \""
                  + yes_label + "\", \"" + no_label + "\");", ProgLine::MAIN_LINE);
    prog->AddVerboseLine(this);
    prog->IncIndent();
    prog->AddLine(this, "if(chs == 0) {");
    prog->IncIndent();
    prog->AddVerboseLine(this, false, "\"inside choice == yes\"");
  }
  else {
    prog->AddLine(this, "{");           // just a block to run..
    prog->IncIndent();
  }
}

void IfGuiPrompt::GenCssBody_impl(Program* prog) {
  yes_code.GenCss(prog);
}

void IfGuiPrompt::GenCssPost_impl(Program* prog) {
  prog->DecIndent();
  prog->AddLine(this, "}");
  if(taMisc::gui_active) {      // extra close
    prog->DecIndent();
    prog->AddLine(this, "}");
  }
}

const String IfGuiPrompt::GenListing_children(int indent_level) {
  String rval = yes_code.GenListing(indent_level + 1 + (int)taMisc::gui_active);
  return rval;
}

String IfGuiPrompt::GetDisplayName() const {
  return "if (gui && " + prompt + ")";
}

void IfGuiPrompt::PreGenChildren_impl(int& item_id) {
  yes_code.PreGen(item_id);
}
ProgVar* IfGuiPrompt::FindVarName(const String& var_nm) const {
  ProgVar* pv = yes_code.FindVarName(var_nm);
  return pv;
}
