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

#include "EditMbrItem_Group.h"
#include <ControlPanel>
#include <DataTable>

#include <taMisc>

TA_BASEFUNS_CTORS_DEFN(EditMbrItem_Group);


void EditMbrItem_Group::SigEmit(int sls, void* op1, void* op2)
{
  inherited::SigEmit(sls, op1, op2);
  ControlPanel::StatSigEmit_Group(this, sls, op1, op2);
}

taBase* EditMbrItem_Group::GetBase_Flat(int idx) const {
  taBase* rval = NULL;
  ControlPanelItem::StatGetBase_Flat(this, idx, rval);
  return rval;
}

String EditMbrItem_Group::GetColHeading(const KeyString& key) const {
  if (key == "base_name") return "Base Name";
  else if (key == "base_type") return "Base Type";
  else if (key == "item_name") return "Member Name";
  else if (key == "mbr_type") return "Member Type";
  else if (key == "label") return "Label";
  else return inherited::GetColHeading(key);
}

const KeyString EditMbrItem_Group::GetListColKey(int col) const {
  switch (col) {
  case 0: return "base_name";
  case 1: return "base_type";
  case 2: return "item_name"; // mbr or mth
  case 3: return "mbr_type";
  case 4: return "label";
  default: break;
  }
  return inherited::GetListColKey(col);
}

EditMbrItem* EditMbrItem_Group::FindMbrName(const String& mbr_nm, const String& label) {
  FOREACH_ELEM_IN_GROUP(EditMbrItem, sei, *this) {
    if(!sei->mbr) continue;
    if(sei->mbr->name == mbr_nm) {
      if(label.nonempty()) {
        if(sei->label.contains(label)) {
          return sei;
        }
      }
    }
  }
  return NULL;
}

