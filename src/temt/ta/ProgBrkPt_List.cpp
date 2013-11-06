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

#include "ProgBrkPt_List.h"
#include <ProgBrkPt>
#include <ProgEl>

void ProgBrkPt_List::Initialize() {
  SetBaseType(&TA_ProgBrkPt);
  setUseStale(true);
}

void ProgBrkPt_List::AddBrkPt(ProgEl* prog_el) {
  if (!prog_el)
    return;
  if (FindBrkPt(prog_el) == NULL) {
    ProgBrkPt* bp = (ProgBrkPt*)New(1);
    bp->prog_el = prog_el;
  }
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
