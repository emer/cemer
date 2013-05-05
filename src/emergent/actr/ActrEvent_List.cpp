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

#include "ActrEvent_List.h"
#include <ActrModule>

void ActrEvent_List::Initialize() {
  SetBaseType(&TA_ActrEvent);
}

int ActrEvent_List::InsertInOrder(ActrEvent* ev) {
  for(int i=0; i<size; i++) {
    ActrEvent* oe = FastEl(i);
    if(ev->time > oe->time) continue;
    if(ev->priority < oe->priority) continue;
    Insert(ev, i);              // this is our spot
    return i;
  }
  Add(ev);
  return size-1;
}
