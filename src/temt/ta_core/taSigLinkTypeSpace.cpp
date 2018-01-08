// Copyright 2013-2017, Regents of the University of Colorado,
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

#include "taSigLinkTypeSpace.h"
#include <TypeSpace>
#include <taiViewType>
#include <taBase>
#include <taSigLinkTypeItem>


taSigLinkTypeSpace::taSigLinkTypeSpace(TypeSpace* data_)
:inherited(TypeItem::TIK_TYPESPACE, data_, data_->sig_link)
{
  dm = DM_DefaultRoot; // default for root, generally we override for other cases
}

taiSigLink* taSigLinkTypeSpace::GetListChild(int itm_idx) {
  TypeDef* el = static_cast<TypeDef*>(data()->PosSafeEl(itm_idx)); 
  if (el == NULL) return NULL;
  
  taiSigLink* dl = taiViewType::StatGetSigLink(el, &TA_TypeDef);
  return dl;
}

int taSigLinkTypeSpace::NumListCols() const {
  return 2;
}

String taSigLinkTypeSpace::GetColHeading(const KeyString& key) const {
  static String typ_name_("Type Name");
  static String typ_desc_("Type Name");
  if (key == taBase::key_name) return typ_name_;
  else if (key == taBase::key_desc) return typ_desc_;
  else return inherited::GetColHeading(key);
}

const KeyString taSigLinkTypeSpace::GetListColKey(int col) const {
  switch (col) {
  case 0: return taBase::key_name;
  case 1: return taBase::key_desc;
  default: return _nilKeyString;
  }
}

String taSigLinkTypeSpace::ChildGetColText(taSigLink* child, const KeyString& key,
  int itm_idx) const 
{
  if (child != NULL) {
    TypeDef* el = static_cast<TypeDef*>(static_cast<taSigLinkTypeItem*>(child)->data());
    if (key == taBase::key_name) return el->GetPathName();
    else if (key == taBase::key_desc) return el->desc;
    return inherited::ChildGetColText(child, key, itm_idx);
  }
  return _nilString;
}

bool taSigLinkTypeSpace::ShowChild(TypeDef* td) const {
  bool rval = false; // have to find at least one show condition
  if (dm & DM_ShowRoot) {
    if (td->IsActualClassNoEff()) return false;
    else rval = true;
  }
  if (dm & DM_ShowNonRoot) {
    if (!(td->IsActualClassNoEff())) return false;
    else rval = true;
  }
  if (dm & DM_ShowEnums) {
    if (td->enum_vals.size == 0) return false;
    else rval = true;
  }
  if (dm & DM_ShowNonEnums) {
    if (td->enum_vals.size > 0) return false;
    else rval = true;
  }
  return rval;
}
