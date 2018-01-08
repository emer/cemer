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

#include "taSigLinkPropertySpace.h"
#include <PropertySpace>
#include <taiViewType>
#include <taBase>
#include <taSigLinkTypeItem>


taSigLinkPropertySpace::taSigLinkPropertySpace(PropertySpace* data_)
:inherited(TypeItem::TIK_PROPERTYSPACE, data_, data_->sig_link)
{
}

taiSigLink* taSigLinkPropertySpace::GetListChild(int itm_idx) {
  MemberDefBase* el = static_cast<MemberDefBase*>(data()->PosSafeEl(itm_idx)); 
  if (el == NULL) return NULL;
  
  taiSigLink* dl = taiViewType::StatGetSigLink(el, &TA_MemberDefBase);
  return dl;
}

int taSigLinkPropertySpace::NumListCols() const {
  return 2;
}

String taSigLinkPropertySpace::GetColHeading(const KeyString& key) const {
  static String prop_name_("Prop Name");
  static String prop_typ_("Memb Type");
  if (key == taBase::key_name) return prop_name_;
  else if (key == taBase::key_type) return prop_typ_;
  else return inherited::GetColHeading(key);
}

const KeyString taSigLinkPropertySpace::GetListColKey(int col) const {
  switch (col) {
  case 0: return taBase::key_name;
  case 1: return taBase::key_type;
  default: return _nilKeyString;
  }
}

String taSigLinkPropertySpace::ChildGetColText(taSigLink* child, const KeyString& key,
  int itm_idx) const
{
  String rval;
  if (child != NULL) {
    MemberDefBase* el = static_cast<MemberDefBase*>(static_cast<taSigLinkTypeItem*>(child)->data());
    if (key == taBase::key_name) { 
      if (el->is_static) rval = " static "; //note: sleazy leading space to sort before non-static
      rval += el->name; 
    } else if (key == taBase::key_type)  rval = el->type->Get_C_Name();
    else return inherited::ChildGetColText(child, key, itm_idx);
  }
  return rval;
}

