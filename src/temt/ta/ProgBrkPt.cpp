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

#include "ProgBrkPt.h"
#include <Program>
#include <ProgLine>
#include <taSigLink>
#include <taMisc>
#include <SigLinkSignal>

TA_BASEFUNS_CTORS_DEFN(ProgBrkPt);
SMARTREF_OF_CPP(ProgBrkPt);

void ProgBrkPt::Initialize() {
  enabled = true;
}

void ProgBrkPt::InitLinks() {
  inherited::InitLinks();
  taBase::Own(prog_el, this);
}

void ProgBrkPt::CutLinks() {
  if(program && prog_el) {
    // start the clear the progline - not from the program
    int code_line = program->script_list.FindMainLine(prog_el);
    ProgLine* pl = program->script_list.SafeEl(code_line);
    prog_el->ClearBreakpoint();
    if (pl) {
      pl->ClearBreakpoint();
    }
  }
  prog_el.CutLinks();
  program.CutLinks();
  inherited::CutLinks();
}

void ProgBrkPt::Enable() {
  enabled = true;
  if(program) {
    int code_line = program->script_list.FindMainLine(prog_el);
    ProgLine* pl = program->script_list.SafeEl(code_line);
    pl->SetBreakpoint();
  }
  SigEmitUpdated();
}

void ProgBrkPt::Disable() {
  enabled = false;
  if (program) {
    int code_line = program->script_list.FindMainLine(prog_el);
    ProgLine* pl = program->script_list.SafeEl(code_line);
    pl->DisableBreakpoint();
  }
  SigEmitUpdated();
}

void ProgBrkPt::UpdateAfterEdit_impl() {
  if(!taMisc::is_loading) {
    if (enabled)  // did the state change?
      Enable();
    else
      Disable();
  }
}

String ProgBrkPt::GetStateDecoKey() const {
  String rval = inherited::GetStateDecoKey();
  if(rval.empty()) {
    if(!enabled)
      return "NotEnabled";
  }
  return rval;
}

String ProgBrkPt::GetDisplayName() const {
  String display = GetDesc();
  if (display.contains("//")) {
    display = trim(display.after("//"));
  }
  
  if (display.contains("/*")) {
    display = trim(display.after("/*"));
    if (display.contains("*/")) {
      display = trim(display.before("*/", -1));
    }
  }
  display = "brk on: " + display;
  return display;
}
