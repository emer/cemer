// Copyright, 1995-2013, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of Emergent
//
//   Emergent is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   Emergent is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.

#include "WtInitPrompt.h"
#include <ProgVar>

#include <taMisc>

void WtInitPrompt::Initialize() {
  prompt = "Do you want to Initialize Network Weights? (WARNING: this will erase all previous training!)";
  yes_label = "Yes";
  no_label = "No";
}

void WtInitPrompt::GenCssPre_impl(Program* prog) {
  if(taMisc::gui_active && !taMisc::server_active) {
    prog->AddLine(this, "{ int chs = 0;");
    prog->IncIndent();
    prog->AddLine(this, String("if(network->epoch > 0) chs = taMisc::Choice(\"")
                  + prompt + "\", \"" + yes_label + "\", \"" + no_label + "\");",
                  ProgLine::MAIN_LINE);
    prog->AddVerboseLine(this);
    prog->AddLine(this, "if(chs == 0) {");
    prog->IncIndent();
    prog->AddVerboseLine(this, false, "\"inside choice == yes\"");
  }
  else {
    prog->AddLine(this, "{");           // just a block to run..
    prog->IncIndent();
  }
}

void WtInitPrompt::GenCssPost_impl(Program* prog) {
  prog->DecIndent();
  prog->AddLine(this, "}");
  if(taMisc::gui_active && !taMisc::server_active) {    // extra close
    prog->DecIndent();
    prog->AddLine(this, "}");
  }
}

