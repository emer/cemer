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

#include "taiMemberOfTypePtr.h"
#include <taiData>
#include <taiTypeDefButton>

#include <taMisc>

int taiMemberOfTypePtr::BidForMember(MemberDef* md, TypeDef* td) {
  if ((md->type->IsPointer()) &&
     ((md->OptionAfter("TYPE_") != "") || (md->OptionAfter("TYPE_ON_") != "")
      || (md->HasOption("NULL_OK")))
     && md->type->DerivesFrom(TA_TypeDef))
    return (taiMember::BidForMember(md,td) + 1);
  return 0;
}

taiData* taiMemberOfTypePtr::GetDataRep_impl(IDataHost* host_, taiData* par, QWidget* gui_parent_,
  int flags_, MemberDef*)
{
  if (mbr->HasOption("NULL_OK"))
    flags_ |= taiData::flgNullOk;
  if(!mbr->HasOption(TypeItem::opt_NO_APPLY_IMMED))
    flags_ |= taiData::flgAutoApply; // default is to auto-apply!
  taiTypeDefButton* rval =
    new taiTypeDefButton(mbr->type, host_, par, gui_parent_, flags_);
  return rval;
}

void taiMemberOfTypePtr::GetImage_impl(taiData* dat, const void* base){
  void* new_base = mbr->GetOff(base);
  taiTypeDefButton* rval = (taiTypeDefButton*)dat;
  TypeDef* td = NULL;
  String mb_nm = mbr->OptionAfter("TYPE_ON_");
  if (mb_nm != "") {
    TypeDef* own_td = typ;
    ta_memb_ptr net_mbr_off = 0;    int net_base_off = 0;
    MemberDef* md = TypeDef::FindMemberPathStatic(own_td, net_base_off, net_mbr_off,
                                                  mb_nm, false); // no warn
    if (md && (md->type->name == "TypeDef_ptr")) {
      td = *(TypeDef**)(MemberDef::GetOff_static(base, net_base_off, net_mbr_off));
    }
  }
  else {
    mb_nm = mbr->OptionAfter("TYPE_");
    if(mb_nm == "this")
      td = typ;
    else if(mb_nm != "")
      td = TypeDef::FindGlobalTypeName(mb_nm);
  }
  TypeDef* targ_typ = (td) ? td : mbr->type;
  rval->GetImage((TypeDef*)*((void**)new_base), targ_typ);
  GetOrigVal(dat, base);
}

void taiMemberOfTypePtr::GetMbrValue_impl(taiData* dat, void* base) {
  void* new_base = mbr->GetOff(base);
  taiTypeDefButton* rval = (taiTypeDefButton*)dat;
  TypeDef* nw_typ = (TypeDef*)rval->GetValue();
  if (mbr->HasOption("NULL_OK"))
    *((void**)new_base) = nw_typ;
  else {
    if (nw_typ)
      *((void**)new_base) = nw_typ;
  }
}
