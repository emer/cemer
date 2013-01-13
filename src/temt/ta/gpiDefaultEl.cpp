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

#include "gpiDefaultEl.h"
#include <gpiListEls>
#include <taList_impl>
#include <taGroup_impl>

#include <taiMisc>



int gpiDefaultEl::BidForMember(MemberDef* md, TypeDef* td) {
  if ((md->name == "el_def") && (td->InheritsFrom(TA_taList_impl)))
    return (taiMember::BidForMember(md,td) + 1);
  return 0;
}

taiData* gpiDefaultEl::GetDataRep_impl(IDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_, MemberDef*) {
  gpiListEls *rval = new gpiListEls(taiMenu::buttonmenu, taiMisc::fonSmall,
        NULL, typ, host_, par, gui_parent_, (flags_ | taiData::flgNullOk | taiData::flgNoList));
  return rval;
}

void gpiDefaultEl::GetImage_impl(taiData* dat, const void* base) {
  taList_impl* tl = (taList_impl*)base;
  taBase* tmp_ptr = tl->DefaultEl_();
  gpiListEls* rval = (gpiListEls*)dat;
  rval->GetImage((taGroup_impl*)base, tmp_ptr);
  GetOrigVal(dat, base);
}

void gpiDefaultEl::GetMbrValue(taiData* dat, void* base, bool& first_diff) {
  taList_impl* tl = (taList_impl*)base;
  gpiListEls* rval = (gpiListEls*)dat;
  taBase* tmp_ptr = rval->GetValue();
  tl->SetDefaultEl(tmp_ptr);
  CmpOrigVal(dat, base, first_diff);
}

