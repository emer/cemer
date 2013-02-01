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

#include "taSigLinkMemberSpace.h"
#include <MemberSpace>
#include <taiViewType>
#include <taBase>
#include <taSigLinkTypeItem>


taSigLinkMemberSpace::taSigLinkMemberSpace(MemberSpace* data_)
:inherited(TypeItem::TIK_MEMBERSPACE, data_, data_->sig_link)
{
}

taiSigLink* taSigLinkMemberSpace::GetListChild(int itm_idx) {
  MemberDef* el = static_cast<MemberDef*>(data()->PosSafeEl(itm_idx)); 
  if (el == NULL) return NULL;
  
  taiSigLink* dl = taiViewType::StatGetSigLink(el, &TA_MemberDef);
  return dl;
}

int taSigLinkMemberSpace::NumListCols() const {
  return 2;
}

String taSigLinkMemberSpace::GetColHeading(const KeyString& key) const {
  static String memb_name_("Memb Name");
  static String memb_typ_("Memb Type");
  if (key == taBase::key_name) return memb_name_;
  else if (key == taBase::key_type) return memb_typ_;
  else return inherited::GetColHeading(key);
}

const KeyString taSigLinkMemberSpace::GetListColKey(int col) const {
  switch (col) {
  case 0: return taBase::key_name;
  case 1: return taBase::key_type;
  default: return _nilKeyString;
  }
}

String taSigLinkMemberSpace::ChildGetColText(taSigLink* child, const KeyString& key,
  int itm_idx) const
{
  String rval;
  if (child != NULL) {
    MemberDef* el = static_cast<MemberDef*>(static_cast<taSigLinkTypeItem*>(child)->data());
    if (key == taBase::key_name) { 
      if (el->is_static) rval = " static "; //note: sleazy leading space to sort before non-static
      rval += el->name; 
    } else if (key == taBase::key_type)  rval = el->type->Get_C_Name();
    else return inherited::ChildGetColText(child, key, itm_idx);
  }
  return rval;
}

