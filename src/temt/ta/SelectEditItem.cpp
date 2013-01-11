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

#include "SelectEditItem.h"


SelectEditItem* SelectEditItem::StatFindItemBase(const taGroup_impl* grp,
   taBase* base, TypeItem* ti, int& idx)
{
  idx = 0;
  FOREACH_ELEM_IN_GROUP(SelectEditItem, rval, *grp) {
    if ((rval->base == base) && (rval->typeItem() == ti))
      return rval;
    ++idx;
  }
  idx = -1;
  return NULL;
}

bool SelectEditItem::StatGetBase_Flat(const taGroup_impl* grp, int idx,
  taBase*& base)
{
  SelectEditItem* sei = dynamic_cast<SelectEditItem*>(grp->Leaf_(idx));
  if (sei) {
    base = sei->base;
    return true;
  }
  return false;
}

bool SelectEditItem::StatHasBase(taGroup_impl* grp, taBase* base) {
  FOREACH_ELEM_IN_GROUP(SelectEditItem, ei, *grp) {
    if (ei->base == base) return true;
  }
  return false;
}

bool SelectEditItem::StatRemoveItemBase(taGroup_impl* grp, taBase* base) {
  bool rval = false;
  FOREACH_ELEM_IN_GROUP_REV(SelectEditItem, ei, *grp) {
    if (ei->base == base) {
      rval = true;
      ei->Close();
    }
  }
  return rval;
}


void SelectEditItem::Initialize() {
  base = NULL;
  cust_desc = false;
}

void SelectEditItem::Destroy() {
}

void SelectEditItem::Copy_(const SelectEditItem& cp) {
  label = cp.label;
  desc = cp.desc;
  cust_desc = cp.cust_desc;
  prv_desc = desc;             // no change here
  base = cp.base;
  item_nm = cp.item_nm;
}

void SelectEditItem::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  if(!cust_desc && !taMisc::is_loading && desc != prv_desc) {
    cust_desc = true;
  }
  prv_desc = desc;
  label = taMisc::StringCVar(label); // keep as safe c variables at all times..
}

String SelectEditItem::caption() const {
  return label;
}

String SelectEditItem::GetDesc() const {
  if (desc.nonempty()) return desc;
  return (typeItem()) ? typeItem()->desc : _nilString;
}

String SelectEditItem::GetName() const {
  return label;
}

String SelectEditItem::GetColText(const KeyString& key, int itm_idx) const {
  if (key == "base_name") return (base) ? base->GetName() : String("NULL");
  else if (key == "base_type") return (base) ? base->GetTypeDef()->name : String("NULL");
  else if (key == "item_name")
    return (typeItem()) ? typeItem()->name : String("NULL");
  else if (key == "label") return label;
  else return inherited::GetColText(key, itm_idx);
}

