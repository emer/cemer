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

#include "KwtaSortBuff_List.h"

TA_BASEFUNS_CTORS_DEFN(KwtaSortBuff_List);

void KwtaSortBuff_List::ResetAllBuffs() {
  for(int i=0; i < size; i++) {
    FastEl(i)->ResetAll();
  }
}

void KwtaSortBuff_List::AllocAllBuffs(int nunits, int ngps) {
  for(int i=0; i < size; i++) {
    FastEl(i)->Alloc(nunits, ngps);
  }
}

void KwtaSortBuff_List::CreateStdBuffs() {
  SetSize(N_BUFFS);
}

