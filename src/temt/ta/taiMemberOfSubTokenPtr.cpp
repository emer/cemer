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

#include "taiMemberOfSubTokenPtr.h"
#include <taiWidget>
#include <taiSubToken>
#include <taiMenu>

#include <taMisc>
#include <taiMisc>


int taiMemberOfSubTokenPtr::BidForMember(MemberDef* md, TypeDef* td) {
  if((md->type->IsPointer()) && (md->OptionAfter("SUBTYPE_") != ""))
    return (taiMember::BidForMember(md,td) + 1);
  return 0;
}

taiWidget* taiMemberOfSubTokenPtr::GetDataRep_impl(IWidgetHost* host_, taiWidget* par, QWidget* gui_parent_, int flags_, MemberDef*) {
  TypeDef* td = NULL;
  String typ_nm = mbr->OptionAfter("SUBTYPE_");
  if (!typ_nm.empty())
    td = TypeDef::FindGlobalTypeName(typ_nm, false);
  if (td == NULL)
    td = mbr->type;
  if (mbr->HasOption("NULL_OK"))
    flags_ |= taiWidget::flgNullOk;
  if (!mbr->HasOption("NO_EDIT"))
    flags_ |= taiWidget::flgEditOk;
  if(!mbr->HasOption(TypeItem::opt_NO_APPLY_IMMED))
    flags_ |= taiWidget::flgAutoApply; // default is to auto-apply!
  taiSubToken* rval =
    new taiSubToken( taiMenu::buttonmenu, taiMisc::fonSmall, td, host_, par, gui_parent_, flags_);
  return rval;
}

void taiMemberOfSubTokenPtr::GetImage_impl(taiWidget* dat, const void* base){
  void* new_base = mbr->GetOff(base);
  taiSubToken* rval = (taiSubToken*)dat;
//nn, done in GetImage  rval->UpdateMenu();
  rval->GetImage(base,*((void **)new_base));
  GetOrigVal(dat, base);
}

void taiMemberOfSubTokenPtr::GetMbrValue_impl(taiWidget* dat, void* base) {
  void* new_base = mbr->GetOff(base);
  taiSubToken* rval = (taiSubToken*)dat;
  if (!no_setpointer && mbr->type->IsTaBase())
    taBase::SetPointer((taBase**)new_base, (taBase*)rval->GetValue());
  else
    *((void**)new_base) = rval->GetValue();
}
