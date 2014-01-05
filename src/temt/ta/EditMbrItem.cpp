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
}

void EditMbrItem::Copy_(const EditMbrItem& cp) {
  mbr = cp.mbr;
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
    param_search.srch = EditParamSearch::NO;
  }
  else if(!is_numeric && param_search.srch == EditParamSearch::SRCH) {
    param_search.srch = EditParamSearch::SET;
  }
}

String EditMbrItem::GetColText(const KeyString& key, int itm_idx) const {
  if (key == "mbr_type")
    return (mbr) ? mbr->type->name : String("NULL");
  else return inherited::GetColText(key, itm_idx);
}

bool EditMbrItem::PSearchValidTest() {
  if(TestError(!mbr, "PSearchValidTest", "item does not have member def set -- not valid parameter search item"))
    return false;
  if(TestError(!is_numeric, "PSearchValidTest", "item is not numeric and thus not a valid parameter search item.  member name:", mbr->name, "label:", label))
    return false;
  return true;
}

Variant EditMbrItem::PSearchCurVal() {
  if(!PSearchValidTest()) return 0.0;
  return mbr->type->GetValVar(mbr->GetOff(base), mbr);
}

String EditMbrItem::CurValAsString() {
  if(!mbr) return _nilString;
  
  if(base && base->InheritsFrom(&TA_DynEnum)) {
    String nmval = ((DynEnum*)base)->NameVal();
    if(nmval.nonempty())
      return nmval;             // special case for program enum -- use string
  }
  return mbr->type->GetValStr(mbr->GetOff(base), NULL, mbr, TypeDef::SC_STREAMING, true);
}

bool EditMbrItem::PSearchCurVal_Set(const Variant& cur_val) {
  //  if(!PSearchValidTest()) return false;
  if(TestError(!mbr, "PSearchCurVal_Set", "item does not have member def set -- not valid parameter search item"))
    return false;
  if(TestError(!is_single, "PSearchCurVal_Set", "item is not a single atomic value and thus not a valid parameter search item.  member name:", mbr->name, "label:", label))
    return false;
  mbr->type->SetValVar(cur_val, mbr->GetOff(base), NULL, mbr);
  base->UpdateAfterEdit();
  return true;
}

bool EditMbrItem::PSearchMinToCur() {
  if(!PSearchValidTest()) return false;
  mbr->type->SetValVar(param_search.min_val, mbr->GetOff(base), NULL, mbr);
  base->UpdateAfterEdit();
  return true;
}

bool EditMbrItem::PSearchNextIncr() {
  if(!PSearchValidTest()) return false;
  double cur_val = PSearchCurVal().toDouble();
  param_search.next_val = cur_val + param_search.incr;
  if(param_search.next_val > param_search.max_val) {
    param_search.next_val = param_search.min_val;
    return false;
  }
  return true;
}

bool EditMbrItem::PSearchNextToCur() {
  if(!PSearchValidTest()) return false;
  mbr->type->SetValVar(param_search.next_val, mbr->GetOff(base), NULL, mbr);
  base->UpdateAfterEdit();
  return true;
}
