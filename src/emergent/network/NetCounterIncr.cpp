// Co2018ght 2013-2017, Regents of the University of Colorado,
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
  if(counter)
    rval += counter->name;
  else
    rval += "?";
  return rval;
}

bool NetCounterIncr::GenCssBody_impl(Program* prog) {
  if(!counter || !network_var) {
    prog->AddLine(this, "// NetCounterIncr ERROR: vars not set!", ProgLine::MAIN_LINE);
    return false;
  }
  TypeDef* netype = network_var->act_object_type();
  if((counter->name == "cycle") && netype->InheritsFromName("LeabraNetwork")) {
    prog->AddLine(this,
                  counter->name + " = " + network_var->name + "->" + counter->name + ";",
                  ProgLine::MAIN_LINE);
    prog->AddVerboseLine(this);
    prog->AddVerboseLine(this, false, "\"new value:\", String(" + counter->name + ")");
  }
  else {
    prog->AddLine(this, counter->name + "++;");
    prog->AddLine(this,
                  network_var->name + "->" + counter->name + " = " + counter->name + ";",
                  ProgLine::MAIN_LINE);
    prog->AddVerboseLine(this);
    prog->AddVerboseLine(this, false, "\"new value:\", String(" + counter->name + ")");
    if(update_after)
      prog->AddLine(this, network_var->name + "->UpdateAfterEdit();");
  }
  return true;
}

bool NetCounterIncr::CanCvtFmCode(const String& code, ProgEl* scope_el) const {
  if (CvtFmCodeCheckNames(code))
    return true;
  
  String dc = code;  dc = trim(dc.downcase());
  int pos = GetDisplayName().index(':');
  String disp = GetDisplayName().before(pos + 1); disp.downcase();
  
  if(dc.startsWith(disp))
    return true;
  return false;
}

bool NetCounterIncr::CvtFmCode(const String& code) {
  String remainder = trim(code.after(":"));
  if(remainder.empty())
    return true;
  if(network_var && network_var->object_val) {
    counter = network_var->object_val->FindMemberName(remainder);
    SigEmitUpdated();
  }
  return true;

}
