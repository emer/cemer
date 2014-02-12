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

#include "NetCounterIncr.h"
#include <ProgVar>
#include <MemberDef>
#include <Program>

TA_BASEFUNS_CTORS_DEFN(NetCounterIncr);

String NetCounterIncr::GetDisplayName() const {
  String rval = "Net Counter Incr: ";
  if(counter) rval += counter->name;
  return rval;
}

void NetCounterIncr::GenCssBody_impl(Program* prog) {
  if(!counter || !network_var) {
    prog->AddLine(this, "// NetCounterIncr ERROR: vars not set!", ProgLine::MAIN_LINE);
    return;
  }
  prog->AddLine(this, counter->name + "++;");
  prog->AddLine(this, network_var->name + "->" + counter->name + " = " + counter->name + ";",
                ProgLine::MAIN_LINE);
  prog->AddVerboseLine(this);
  prog->AddVerboseLine(this, false, "\"new value:\", String(" + counter->name + ")");
  if(update_after)
    prog->AddLine(this, network_var->name + "->UpdateAfterEdit();");
}

