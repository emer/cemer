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

#include "EditMbrItem.h"
#include <BuiltinTypeDefs>
#include <DynEnum>

TA_BASEFUNS_CTORS_DEFN(EditMbrItem);

void EditMbrItem::Initialize() {
  mbr = NULL;
  is_numeric = false;
  is_single = false;
}

void EditMbrItem::Destroy() {
}

void EditMbrItem::InitLinks() {
  inherited::InitLinks();
  taBase::Own(param_search, this);
  taBase::Own(param_set_value, this);
  taBase::Own(notes, this);
}

void EditMbrItem::Copy_(const EditMbrItem& cp) {
  mbr = cp.mbr;
  is_numeric = cp.is_numeric;
  is_single = cp.is_single;
  param_search = cp.param_search;
  param_set_value = cp.param_set_value;
  notes = cp.notes;
}

void EditMbrItem::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  if (!cust_desc && mbr) {
    desc = _nilString;
    MemberDef::GetMembDesc(mbr, desc, "");
    prv_desc = desc;
  }
  is_numeric = false;
  is_single = false;
  if(mbr && !mbr->HasOption("READ_ONLY") && !mbr->HasOption("GUI_READ_ONLY")) {
    if(mbr->type->IsAtomic() || mbr->type->IsAtomicEff()) {
      is_single = true;
      if(mbr->type->IsInt() || mbr->type->IsFloat()) {
        is_numeric = true;
      }
      if(base && base->InheritsFrom(&TA_DynEnum)) {
        is_numeric = false;     // nix
      }
    }
  }
  if(!is_single) {
    param_search.search = false;
    param_search.record = false;
  }
  else if(!is_numeric && param_search.search) {
    param_search.search = false;
    param_search.record = true;
  }
}

String EditMbrItem::GetColText(const KeyString& key, int itm_idx) const {
  if (key == "mbr_type")
    return (mbr) ? mbr->type->name : String("NULL");
  else return inherited::GetColText(key, itm_idx);
}

String EditMbrItem::CurValAsString() {
  if(!mbr) return _nilString;
  
  if(base && base->InheritsFrom(&TA_DynEnum)) {
    String nmval = ((DynEnum*)base)->NameVal();
    if(nmval.nonempty())
      return nmval;             // special case for program enum -- use string
  }
  return mbr->GetValStr(base, TypeDef::SC_STREAMING, true);
}

bool EditMbrItem::SetCurVal(const Variant& cur_val) {
  if(TestError(!mbr, "SetCurVal", "item does not have member def set -- not valid control panel item"))
    return false;
  if(TestError(!is_single, "SetCurVal", "item is not a single atomic value and thus not a valid control panel item to set from a command line.  member name:", mbr->name, "label:", label))
    return false;
  mbr->type->SetValVar(cur_val, mbr->GetOff(base), NULL, mbr);
  base->UpdateAfterEdit();
  return true;
}

