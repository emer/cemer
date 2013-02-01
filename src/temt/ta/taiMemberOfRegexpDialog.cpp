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

#include "taiMemberOfRegexpDialog.h"
#include <iDialogRegexpPopulator>
#include <taiWidgetFieldRegexp>

#include <taMisc>


int taiMemberOfRegexpDialog::BidForMember(MemberDef* md, TypeDef* td) {
  if (md->type->IsString() && md->HasOption("REGEXP_DIALOG"))
    return (taiMember::BidForMember(md,td) + 1);
  return 0;
}

taiWidget* taiMemberOfRegexpDialog::GetDataRep_impl(IWidgetHost* host_, taiWidget* par, QWidget* gui_parent_, int flags_, MemberDef*) {
  // Get the iDialogRegexpPopulator instance that should be used for this field.
  iDialogRegexpPopulator *populator = 0;
  String pop_type = mbr->OptionAfter("TYPE_");
  if (!pop_type.empty()) {
    if (TypeDef *type = TypeDef::FindGlobalTypeName(pop_type, false)) {
      if (void *pv_inst = type->GetInstance()) {
        populator = reinterpret_cast<iDialogRegexpPopulator *>(pv_inst);
      }
    }
  }

  return new taiWidgetFieldRegexp(mbr->type, host_, par, gui_parent_, flags_, populator);
}

void taiMemberOfRegexpDialog::GetImage_impl(taiWidget* dat, const void* base){
  void* new_base = mbr->GetOff(base);
  taiWidgetFieldRegexp* rval = (taiWidgetFieldRegexp*)dat;
  // The 'base' pointer is the owner of the regexp field.
  rval->SetFieldOwner(base);
  rval->GetImage(*((String*)new_base));
}

void taiMemberOfRegexpDialog::GetMbrValue_impl(taiWidget* dat, void* base) {
  void* new_base = mbr->GetOff(base);
  taiWidgetFieldRegexp* rval = (taiWidgetFieldRegexp*)dat;
  *((String*)new_base) = rval->GetValue();
}
