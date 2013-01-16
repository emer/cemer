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

#include "NetCounterInit.h"

String NetCounterInit::GetDisplayName() const {
  String rval = "Net Counter Init: ";
  if(counter) rval += counter->name;
  return rval;
}

void NetCounterInit::GenCssBody_impl(Program* prog) {
  if(!counter || !network_var) {
    prog->AddLine(this, "// NetCounterInit ERROR: vars not set!", ProgLine::MAIN_LINE);
    return;
  }
  prog->AddLine(this, counter->name + " = 0;");
  prog->AddLine(this, network_var->name + "->" + counter->name + " = " + counter->name + ";",
                ProgLine::MAIN_LINE);
  prog->AddVerboseLine(this);
  prog->AddVerboseLine(this, false, "\"new value:\", String(" + counter->name + ")");
  if(update_after)
    prog->AddLine(this, network_var->name + "->UpdateAfterEdit();");
}

