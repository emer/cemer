// Copyright 2013-2018, Regents of the University of Colorado,
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

#include "ControlPanelItem.h"
#include <taGroup_impl>
#include <MemberDef>

#include <taMisc>

TA_BASEFUNS_CTORS_DEFN(ControlPanelItem);


bool ControlPanelItem::StatCheckBase(ControlPanelItem* itm, const taBase* base) {
  if(itm->base == base)
    return true;
  TypeItem* ti = itm->typeItem();
  if(!ti) return false;
  if(ti->TypeInfoKind() == TypeItem::TIK_MEMBER) {
    if(((MemberDef*)ti)->GetOff(itm->base) == base) {
      return true;
    }
  }
  return false;
}

ControlPanelItem* ControlPanelItem::StatFindItemBase
(const taGroup_impl* grp, const taBase* base, TypeItem* ti, int& idx)
{
  idx = 0;
  FOREACH_ELEM_IN_GROUP(ControlPanelItem, rval, *grp) {
    if ((rval->base == base) && (rval->typeItem() == ti))
      return rval;
    ++idx;
  }
  idx = -1;
  return NULL;
}

ControlPanelItem* ControlPanelItem::StatFindItemBase_List
(const taGroup_impl* grp, const taBase* base, TypeItem* ti, int& idx)
{
  idx = -1;
  for(int i=0; i<grp->size; i++) {
    ControlPanelItem* rval = (ControlPanelItem*)grp->FastEl_(i);
    if ((rval->base == base) && (rval->typeItem() == ti)) {
      idx = i;
      return rval;
    }
  }
  return NULL;
}

bool ControlPanelItem::StatGetBase_Flat(const taGroup_impl* grp, int idx,
  taBase*& base)
{
  ControlPanelItem* sei = dynamic_cast<ControlPanelItem*>(grp->Leaf_(idx));
  if (sei) {
    base = sei->base;
    return true;
  }
  return false;
}

bool ControlPanelItem::StatHasBase(taGroup_impl* grp, const taBase* base) {
  FOREACH_ELEM_IN_GROUP(ControlPanelItem, ei, *grp) {
    if (StatCheckBase(ei, base)) return true;
  }
  return false;
}

bool ControlPanelItem::StatRemoveItemBase(taGroup_impl* grp, taBase* base) {
  bool rval = false;
  FOREACH_ELEM_IN_GROUP_REV(ControlPanelItem, ei, *grp) {
    if (StatCheckBase(ei, base)) {
      rval = true;
      ei->Close();
    }
  }
  return rval;
}


void ControlPanelItem::Initialize() {
  base = NULL;
  cust_desc = false;
  cust_label = false;
  short_label = false;
}

void ControlPanelItem::Destroy() {
}

void ControlPanelItem::Copy_(const ControlPanelItem& cp) {
  label = cp.label;
  short_label = cp.short_label;
  cust_label = cp.cust_label;
  desc = cp.desc;
  cust_desc = cp.cust_desc;
  prv_desc = desc;             // no change here
  prv_label = label;
  base = cp.base;
}

void ControlPanelItem::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  // each sub-class should include code like this -- needs to be specific to member vs meth
  // if(!taMisc::is_loading) {
  //   if(!cust_desc && desc != prv_desc) {
  //     cust_desc = true;
  //   }
  //   if(!cust_label && label != prv_label) {
  //     cust_label = true;
  //   }
  // }
  // prv_desc = desc;
  // label = taMisc::StringCVar(label); // keep as safe c variables at all times..
  // prv_label = label;
}

bool ControlPanelItem::SetName(const String& nm) {
  label = nm;
  return true;
}

void ControlPanelItem::SetLabel(const String& new_label, bool custom_lbl, bool sht_label) {
  label = new_label;
  short_label = sht_label;
  cust_label = custom_lbl;
  prv_label = label;
}

void ControlPanelItem::SetDesc(const String& new_desc, bool custom_desc) {
  desc = new_desc;
  cust_desc = custom_desc;
  prv_desc = desc;
}

String ControlPanelItem::caption() const {
  return label;
}

String ControlPanelItem::GetDesc() const {
  if (desc.nonempty()) return desc;
  return (typeItem()) ? typeItem()->desc : _nilString;
}

String ControlPanelItem::GetColText(const KeyString& key, int itm_idx) const {
  if (key == "base_name") {
    if(base) {
      return base->DisplayPath();
    }
    return String("NULL");
  }
  else if (key == "base_type") {
    return (base) ? base->GetTypeDef()->name : String("NULL");
  }
  else if (key == "item_name") {
    return (typeItem()) ? typeItem()->name : String("NULL");
  }
  else if (key == "label") {
    return label;
  }
  else return inherited::GetColText(key, itm_idx);
}

