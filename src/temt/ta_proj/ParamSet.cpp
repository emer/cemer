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

#include "ParamSet.h"

#include <taMisc>
#include <EditMbrItem>
#include <ParamSetItem>

TA_BASEFUNS_CTORS_DEFN(ParamSet);

void ParamSet::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
}

void ParamSet::CopyActiveToSaved() {
  FOREACH_ELEM_IN_GROUP(EditMbrItem, sei, mbrs) {
    sei->CopyActiveToSaved();
  }
  ReShowEdit(true);
}

void ParamSet::CopySavedToActive() {
  FOREACH_ELEM_IN_GROUP(EditMbrItem, sei, mbrs) {
    sei->CopySavedToActive();
  }
  ReShowEdit(true);
}

void ParamSet::CopyActiveToSaved_item(int idx) {
  EditMbrItem* item = mbrs.Leaf(idx);
  if(item && item->base) {
    item->CopyActiveToSaved();
    ReShowEdit(true);
  }
}

void ParamSet::CopySavedToActive_item(int idx) {
  EditMbrItem* item = mbrs.Leaf(idx);
  if(item && item->base) {
    item->CopySavedToActive();
    ReShowEdit(true);
  }
}

bool ParamSet::ActiveEqualsSaved(String member_name) {
  bool rval = false;
  EditMbrItem* emi = mbrs.FindLeafName(member_name);
  if (emi) {
    String active_value = emi->mbr->GetValStr(emi->base);
    String saved_value = emi->param_set_value.saved_value;
    if (active_value == saved_value)
      rval = true;
  }
  return rval;
}
