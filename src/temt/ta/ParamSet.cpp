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
  FOREACH_ELEM_IN_GROUP(ControlPanelItem, sei, mbrs) {
    EditMbrItem* edit_mbr = dynamic_cast<EditMbrItem*>(sei);
    taBase* bs = sei->base;
    edit_mbr->param_set_value.saved_value = edit_mbr->mbr->GetValStr(bs);
    edit_mbr->UpdateAfterEdit();
  }
}

void ParamSet::CopySavedToActive() {
  FOREACH_ELEM_IN_GROUP(ControlPanelItem, sei, mbrs) {
    EditMbrItem* edit_mbr = dynamic_cast<EditMbrItem*>(sei);
    taBase* bs = sei->base;
    edit_mbr->mbr->SetValStr(edit_mbr->param_set_value.saved_value, bs);
    edit_mbr->UpdateAfterEdit();
  }
}

bool ParamSet::ActiveEqualsSaved(String member_name) {
  bool rval = false;
  EditMbrItem* emi = mbrs.FindName(member_name);
  if (emi) {
    String active_value = emi->mbr->GetValStr(emi->base);
    String saved_value = emi->param_set_value.saved_value;
    if (active_value == saved_value)
      rval = true;
  }
  return rval;
}
