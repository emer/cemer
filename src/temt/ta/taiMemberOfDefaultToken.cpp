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

#include "taiMemberOfDefaultToken.h"
#include <MemberDef>
#include <taiEditOfDefault>
#include <taiWidgetEditButton>


int taiMemberOfDefaultToken::BidForMember(MemberDef* md, TypeDef* td) {
  TypeDef* mtd = md->type;
  if (((mtd->IsPointer()) && mtd->IsTaBase()) &&
      md->HasOption("DEFAULT_EDIT"))
    return taiMemberOfTokenPtr::BidForMember(md,td)+10;
  return 0;
}

taiWidget* taiMemberOfDefaultToken::GetWidgetRep_impl(IWidgetHost* host_, taiWidget* par, QWidget* gui_parent_, int flags_, MemberDef*) {
  taiEditOfDefault* tde = new taiEditOfDefault(mbr->type->GetNonPtrType());
  taiWidgetEditButton *rval = taiWidgetEditButton::New(NULL, tde, mbr->type->GetNonPtrType(),
     host_, par, gui_parent_, flags_);
  return rval;
}

void taiMemberOfDefaultToken::GetImage_impl(taiWidget* dat, const void* base) {
  taiWidgetEditButton* rval = (taiWidgetEditButton*)dat;
  taBase* token_ptr = GetTokenPtr(base);
  rval->GetImage_(token_ptr);
  if (token_ptr) {
    rval->typ = token_ptr->GetTypeDef();
  }
}

void taiMemberOfDefaultToken::GetMbrValue_impl(taiWidget*, void*) {
}
