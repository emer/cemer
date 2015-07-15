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

#include "taSigLinkMethodSpace.h"
#include <MethodSpace>
#include <MethodDef>
#include <taiViewType>
#include <taBase>
#include <taSigLinkTypeItem>


taSigLinkMethodSpace::taSigLinkMethodSpace(MethodSpace* data_)
:inherited(TypeItem::TIK_METHODSPACE, data_, data_->sig_link)
{
}

taiSigLink* taSigLinkMethodSpace::GetListChild(int itm_idx) {
  MethodDef* el = static_cast<MethodDef*>(data()->PosSafeEl(itm_idx)); 
  if (el == NULL) return NULL;
  
  taiSigLink* dl = taiViewType::StatGetSigLink(el, &TA_MethodDef);
  return dl;
}

int taSigLinkMethodSpace::NumListCols() const {
  return 3;
}

const KeyString taSigLinkMethodSpace::key_rval("rval");
const KeyString taSigLinkMethodSpace::key_params("params");

String taSigLinkMethodSpace::GetColHeading(const KeyString& key) const {
  static String meth_name_("Method Name");
  static String rval_("rval");
  static String params_("Params");
  if (key == taBase::key_name) return meth_name_;
  else if (key == key_rval) return rval_;
  else if (key == key_params) return params_;
  else return inherited::GetColHeading(key);
}

const KeyString taSigLinkMethodSpace::GetListColKey(int col) const {
  switch (col) {
  case 0: return taBase::key_name;
  case 1: return key_rval;
  case 2: return key_params;
  default: return _nilKeyString;
  }
}

String taSigLinkMethodSpace::ChildGetColText(taSigLink* child, const KeyString& key,
  int itm_idx) const 
{
  String rval;
  if (child != NULL) {
    MethodDef* el = static_cast<MethodDef*>(static_cast<taSigLinkTypeItem*>(child)->data());
    if (key == taBase::key_name) {
      if (el->is_static) rval = " static "; //note: sleazy leading space to sort before non-static
      rval += el->name; 
    } else if (key == key_rval) rval = el->type->Get_C_Name();
    else if (key == key_params) rval = el->ParamsAsString();
    else return inherited::ChildGetColText(child, key, itm_idx);
  }
  return rval;
}

