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

#include "ProgLine.h"
#include <ProgEl>
#include <Program>

#include <taMisc>

#include <css_machine.h>


void ProgLine::Initialize() {
  flags = PL_NONE;
  line_no = -1;
  indent = 0;
}

void ProgLine::Destroy() {
  prog_el.CutLinks();
}

const String ProgLine::Indent() {
  return cssMisc::Indent(indent);
}

const String ProgLine::CodeLineNo() {
  return taMisc::LeadingZeros(line_no, 5) + " " + Indent() + code;
}


void ProgLine::SetBreakpoint() {
  Program* prog = GET_MY_OWNER(Program);
  if(!prog) return;
  SetPLineFlag(ProgLine::BREAKPOINT);
  if((bool)prog_el && prog_el->InheritsFrom(&TA_ProgEl)) {
    ProgEl* pel = (ProgEl*)prog_el.ptr();
    pel->SetProgFlag(ProgEl::BREAKPOINT);
    pel->DataItemUpdated();
    prog->SetBreakpoint_impl(pel);
  }
}

void ProgLine::ClearBreakpoint() {
  Program* prog = GET_MY_OWNER(Program);
  if(!prog) return;
  ClearPLineFlag(ProgLine::BREAKPOINT);
  if((bool)prog_el && prog_el->InheritsFrom(&TA_ProgEl)) {
    ProgEl* pel = (ProgEl*)prog_el.ptr();
    pel->ClearProgFlag(ProgEl::BREAKPOINT);
    pel->DataItemUpdated();
    prog->ClearBreakpoint_impl(pel);
  }
}

void ProgLine::SetError() {
  SetPLineFlag(ProgLine::PROG_ERROR);
  if((bool)prog_el && prog_el->InheritsFrom(&TA_ProgEl)) {
    ProgEl* pel = (ProgEl*)prog_el.ptr();
    pel->SetProgFlag(ProgEl::PROG_ERROR);
    pel->DataItemUpdated();
    pel->BrowserSelectMe();
  }
}

void ProgLine::ClearError() {
  ClearPLineFlag(ProgLine::PROG_ERROR);
  if((bool)prog_el && prog_el->InheritsFrom(&TA_ProgEl)) {
    ProgEl* pel = (ProgEl*)prog_el.ptr();
    bool was_set = pel->HasProgFlag(ProgEl::PROG_ERROR);
    if(was_set) {
      pel->ClearProgFlag(ProgEl::PROG_ERROR);
      pel->DataItemUpdated();
    }
  }
}

void ProgLine::SetWarning() {
  SetPLineFlag(ProgLine::WARNING);
  if((bool)prog_el && prog_el->InheritsFrom(&TA_ProgEl)) {
    ProgEl* pel = (ProgEl*)prog_el.ptr();
    pel->SetProgFlag(ProgEl::WARNING);
    pel->DataItemUpdated();
  }
}

void ProgLine::ClearWarning() {
  ClearPLineFlag(ProgLine::WARNING);
  if((bool)prog_el && prog_el->InheritsFrom(&TA_ProgEl)) {
    ProgEl* pel = (ProgEl*)prog_el.ptr();
    bool was_set = pel->HasProgFlag(ProgEl::WARNING);
    if(was_set) {
      pel->ClearProgFlag(ProgEl::WARNING);
      pel->DataItemUpdated();
    }
  }
}

