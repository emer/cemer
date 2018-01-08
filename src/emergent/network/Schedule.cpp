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

#include "Schedule.h"
#include <taProject>

TA_BASEFUNS_CTORS_DEFN(Schedule);
TA_BASEFUNS_CTORS_DEFN(SchedItem);

void SchedItem::Initialize() {
  start_ctr = 0;
  start_val = 0.0f;
  duration = 0;
  step = .01f;
}

String SchedItem::GetDesc() const {
  String rval = String(start_ctr) + ": " + String(start_val);
  return rval;
}

String SchedItem::GetDisplayName() const {
  return GetDesc();
}

bool SchedItem::BrowserEditEnable() const {
  return true;
}

bool SchedItem::BrowserEditSet(const String& new_val_str, int move_after) {
  if(move_after != -11) {
    taProject* proj = GetMyProj();
    if(proj) {
      proj->undo_mgr.SaveUndo(this, "BrowserEditSet", this);
    }
  }
  String start_str = new_val_str.before(':');
  if(start_str.nonempty()) {
    start_ctr = start_str.toInt();
    String val_str = new_val_str.after(": ");
    start_val = val_str.toFloat();
  }
  UpdateAfterEdit();            // need full UAE
  return true;
}

void Schedule::Initialize() {
  last_ctr = -1;
  default_val = 1.0f;
  cur_val = 0.0f;
  interpolate = true;
  SetBaseType(&TA_SchedItem);
}

void Schedule::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();

  SortItems();
  UpdateDurations();
}

void Schedule::SortItems() {
  last_ctr = -1;
  cur_val = 0;
  int lst_ctr = -1;
  Schedule temp;
  for(int i=0; i < size; i++) {
    SchedItem* itm = FastEl(i);
    if(itm->start_ctr < lst_ctr) {
      temp.Transfer(itm);
      i--;
    }
    else {
      lst_ctr = itm->start_ctr;
    }
  }
  for (; temp.size > 0 ;) {
    SchedItem* itm = temp.FastEl(0);
    int j;
    for(j=0; (j < size) && (itm->start_ctr > FastEl(j)->start_ctr); j++);
    Insert(itm,j);              // always insert item in new spot
    temp.RemoveIdx(0);
  }
}

void Schedule::UpdateDurations() {
  if(size > 0)
    FastEl(0)->start_ctr = 0;

  for(int i=0; i < size; i++) {
    SchedItem* itm = FastEl(i);
    if(i == (size - 1)) {
      itm->duration = 1;
      itm->step = 1.0;
      break;
    }
    SchedItem* nxt = FastEl(i+1);
    itm->duration = nxt->start_ctr - itm->start_ctr;
    itm->duration = MAX(itm->duration, 1);
    itm->step = (nxt->start_val - itm->start_val) / (float)itm->duration;
  }
}

float Schedule::GetVal(int ctr) {
  if((size <  1) || (ctr < 0)) {
    cur_val = default_val;
    return default_val;
  }

  if(ctr == last_ctr)
    return cur_val;

  last_ctr = ctr;
  int i;
  for(i=0; i < size; i++) {
    SchedItem* itm = FastEl(i);
    if((ctr >= itm->start_ctr) && (ctr < (itm->start_ctr + itm->duration))) {
      if(i == size-1)
        cur_val = itm->start_val;
      else {
        if(interpolate)
          cur_val = itm->GetVal(ctr);
        else
          cur_val = itm->start_val;
      }
      return cur_val;
    }
  }
  cur_val = FastEl(size-1)->start_val;
  return cur_val;
}

