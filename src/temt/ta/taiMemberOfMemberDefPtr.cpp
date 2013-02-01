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

#include "taiMemberOfMemberDefPtr.h"
#include <taiWidget>
#include <taiWidgetMemberDefChooser>


int taiMemberOfMemberDefPtr::BidForMember(MemberDef* md, TypeDef* td) {
  if ((md->type->IsPointer()) && (md->type->DerivesFrom(TA_MemberDef)))
    return (inherited::BidForMember(md,td) + 1);
  return 0;
}

taiWidget* taiMemberOfMemberDefPtr::GetDataRep_impl(IWidgetHost* host_, taiWidget* par,
  QWidget* gui_parent_, int flags_, MemberDef*)
{
  if(!mbr->HasOption(TypeItem::opt_NO_APPLY_IMMED))
    flags_ |= taiWidget::flgAutoApply; // default is to auto-apply!
  taiWidgetMemberDefChooser* rval =  new taiWidgetMemberDefChooser(typ, host_, par, gui_parent_, flags_);
  return rval;
}

void taiMemberOfMemberDefPtr::GetImage_impl(taiWidget* dat, const void* base){
  void* new_base = mbr->GetOff(base);
  taiWidgetMemberDefChooser* rval = (taiWidgetMemberDefChooser*)dat;
  MemberDef* cur_sel = *((MemberDef**)(new_base));
  rval->GetImage(cur_sel, GetTargetType(base));
  GetOrigVal(dat, base);
}

void taiMemberOfMemberDefPtr::GetMbrValue_impl(taiWidget* dat, void* base) {
  void* new_base = mbr->GetOff(base);
  taiWidgetMemberDefChooser* rval = (taiWidgetMemberDefChooser*)dat;
  *((MemberDef**)new_base) = rval->GetValue();
}
