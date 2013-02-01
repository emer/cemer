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

#include "taiMemberOfMethodDefPtr.h"
#include <taiWidget>
#include <taiMethodDefButton>
#include <MethodDef>

int taiMemberOfMethodDefPtr::BidForMember(MemberDef* md, TypeDef* td) {
  if ((md->type->IsPointer()) && (md->type->DerivesFrom(TA_MethodDef)))
    return (inherited::BidForMember(md,td) + 1);
  return 0;
}

taiWidget* taiMemberOfMethodDefPtr::GetDataRep_impl(IWidgetHost* host_, taiWidget* par,
  QWidget* gui_parent_, int flags_, MemberDef*)
{
  if(!mbr->HasOption(TypeItem::opt_NO_APPLY_IMMED))
    flags_ |= taiWidget::flgAutoApply; // default is to auto-apply!
  taiMethodDefButton* rval = new taiMethodDefButton(typ, host_,
    par, gui_parent_, flags_);
  return rval;
}

void taiMemberOfMethodDefPtr::GetImage_impl(taiWidget* dat, const void* base){
  void* new_base = mbr->GetOff(base);
  taiMethodDefButton* rval = (taiMethodDefButton*)dat;
  MethodDef* cur_sel = *((MethodDef**)(new_base));
  rval->GetImage(cur_sel, GetTargetType(base));
  GetOrigVal(dat, base);
}

void taiMemberOfMethodDefPtr::GetMbrValue_impl(taiWidget* dat, void* base) {
  void* new_base = mbr->GetOff(base);
  taiMethodDefButton* rval = (taiMethodDefButton*)dat;
  *((MethodDef**)new_base) = rval->GetValue();
}
