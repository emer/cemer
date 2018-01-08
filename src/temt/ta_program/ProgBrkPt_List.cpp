// Copyright 2013-2017, Regents of the University of Colorado,
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

#include "ProgBrkPt_List.h"
#include <ProgBrkPt>
#include <ProgEl>
#include <ProgLine>
#include <tabMisc>
#include <Program>

#include <taMisc>

TA_BASEFUNS_CTORS_DEFN(ProgBrkPt_List);

void ProgBrkPt_List::Initialize() {
  SetBaseType(&TA_ProgBrkPt);
  setUseStale(true);
}

ProgBrkPt* ProgBrkPt_List::AddBrkPt(ProgEl* prog_el, ProgLine* pl) {
  if (!prog_el)
    return NULL;
  ProgBrkPt* bp = FindBrkPt(prog_el);
  if (bp == NULL) {
    bp = (ProgBrkPt*)New(1);
    bp->program = GET_OWNER(prog_el, Program);
    bp->prog_el = prog_el;
    bp->desc = bp->program->name + ":" + taMisc::LeadingZeros(pl->line_no, 3)
      + " " + pl->code.elidedTo(-1);
    bp->Enable();
    SigEmitUpdated();
//    tabMisc::DelayedFunCall_gui(this, "BrowserExpandAll");  // don't do this!! - will cause tree to scroll 
  }
  return bp;
}

void ProgBrkPt_List::DeleteBrkPt(ProgEl* prog_el) {
  if (!prog_el)
    return;
  for(int i = size-1; i >= 0; i--) {
    ProgBrkPt* bp = FastEl(i);
    if (prog_el == bp->prog_el) {
      this->RemoveEl(bp);
      break;
    }
  }
}

ProgBrkPt* ProgBrkPt_List::FindBrkPt(ProgEl* prog_el) const {
  for(int i = size-1; i >= 0; i--) {
    ProgBrkPt* bp = FastEl(i);
    if(prog_el == bp->prog_el) {
      return bp;
    }
  }
  return NULL;
}

bool ProgBrkPt_List::BrowserExpandAll() {
  return inherited::BrowserExpandAll();
}

void ProgBrkPt_List::EnableAll() {
  for(int i = 0; i < size; i++) {
    ProgBrkPt* bp = FastEl(i);
    bp->Enable();
  }
}

void ProgBrkPt_List::DisableAll() {
  for(int i = 0; i < size; i++) {
    ProgBrkPt* bp = FastEl(i);
    bp->Disable();
  }
}

