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

#include "EditMthItem_Group.h"
#include <SelectEdit>

#include <taMisc>


void EditMthItem_Group::Initialize() {
  SetBaseType(&TA_EditMthItem);
  is_root = true;
  group_type = GT_BUTTONS; // for root only
}

void EditMthItem_Group::Copy_(const EditMthItem_Group& cp) {
  group_type = cp.group_type;
}

void EditMthItem_Group::InitLinks() {
  inherited::InitLinks();
  is_root = IsRoot();
  if (!is_root)
    group_type = GT_MENU_BUTTON;
}

void EditMthItem_Group::DataChanged(int dcr, void* op1, void* op2)
{
  inherited::DataChanged(dcr, op1, op2);
  SelectEdit::StatDataChanged_Group(this, dcr, op1, op2);
}


taBase* EditMthItem_Group::GetBase_Flat(int idx) const {
  taBase* rval = NULL;
  SelectEditItem::StatGetBase_Flat(this, idx, rval);
  return rval;
}

String EditMthItem_Group::GetColHeading(const KeyString& key) const {
  if (key == "base_name") return "Base Name";
  else if (key == "base_type") return "Base Type";
  else if (key == "item_name") return "Method Name";
  else if (key == "label") return "Label";
  else return inherited::GetColHeading(key);
}

const KeyString EditMthItem_Group::GetListColKey(int col) const {
  switch (col) {
  case 0: return "base_name";
  case 1: return "base_type";
  case 2: return "item_name"; // mbr or mth
  case 4: return "label";
  default: break;
  }
  return inherited::GetListColKey(col);
}

void EditMthItem_Group::SetGroupType(MthGroupType group_type_) {
  if (is_root && (group_type_ != GT_BUTTONS))
    taMisc::Error("The root group may only show buttons -- create a subgroup for menus or button groups");
  group_type = group_type_;
}

