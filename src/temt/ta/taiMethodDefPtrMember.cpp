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

#include "taiMethodDefPtrMember.h"
#include <taiData>
#include <taiMethodDefButton>


int taiMethodDefPtrMember::BidForMember(MemberDef* md, TypeDef* td) {
  if ((md->type->ptr == 1) && (md->type->DerivesFrom(TA_MethodDef)))
    return (inherited::BidForMember(md,td) + 1);
  return 0;
}

taiData* taiMethodDefPtrMember::GetDataRep_impl(IDataHost* host_, taiData* par,
  QWidget* gui_parent_, int flags_, MemberDef*)
{
  if(!mbr->HasOption(TypeItem::opt_NO_APPLY_IMMED))
    flags_ |= taiData::flgAutoApply; // default is to auto-apply!
  taiMethodDefButton* rval = new taiMethodDefButton(typ, host_,
    par, gui_parent_, flags_);
  return rval;
}

void taiMethodDefPtrMember::GetImage_impl(taiData* dat, const void* base){
  void* new_base = mbr->GetOff(base);
  taiMethodDefButton* rval = (taiMethodDefButton*)dat;
  MethodDef* cur_sel = *((MethodDef**)(new_base));
  rval->GetImage(cur_sel, GetTargetType(base));
  GetOrigVal(dat, base);
}

void taiMethodDefPtrMember::GetMbrValue_impl(taiData* dat, void* base) {
  void* new_base = mbr->GetOff(base);
  taiMethodDefButton* rval = (taiMethodDefButton*)dat;
  *((MethodDef**)new_base) = rval->GetValue();
}
