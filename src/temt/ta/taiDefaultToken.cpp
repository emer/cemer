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

#include "taiDefaultToken.h"

int taiDefaultToken::BidForMember(MemberDef* md, TypeDef* td) {
  TypeDef* mtd = md->type;
  if (((mtd->ptr == 1) && mtd->DerivesFrom(TA_taBase)) &&
     md->HasOption("DEFAULT_EDIT"))
    return taiTokenPtrMember::BidForMember(md,td)+10;
  return 0;
}

taiData* taiDefaultToken::GetDataRep_impl(IDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_, MemberDef*) {
  taiDefaultEdit* tde = new taiDefaultEdit(mbr->type->GetNonPtrType());
  taiEditButton *rval = taiEditButton::New(NULL, tde, mbr->type->GetNonPtrType(),
     host_, par, gui_parent_, flags_);
  return rval;
}

void taiDefaultToken::GetImage_impl(taiData* dat, const void* base) {
  taiEditButton* rval = (taiEditButton*)dat;
  taBase* token_ptr = GetTokenPtr(base);
  rval->GetImage_(token_ptr);
  if (token_ptr) {
    rval->typ = token_ptr->GetTypeDef();
  }
}

void taiDefaultToken::GetMbrValue_impl(taiData*, void*) {
}
