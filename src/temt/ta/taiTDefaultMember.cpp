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

#include "taiTDefaultMember.h"
#include <TypeDefault>
#include <taiPlusToggle>
#include <taiData>

#include <taMisc>


int taiTDefaultMember::BidForMember(MemberDef*, TypeDef*) {
  // TD_Default member does not bid, it is only applied in special cases.
  return 0;
}

taiData* taiTDefaultMember::GetDataRep(IDataHost* host_, taiData* par, QWidget* gui_parent_,
                                       taiType*, int flags_, MemberDef* mbr)
{
  taiPlusToggle* rval = new taiPlusToggle(typ, host_, par, gui_parent_, flags_);
  rval->InitLayout();
  taiData* rdat;
  if (HasLowerBidder()) {
    rdat = LowerBidder()->GetDataRep(host_, rval, rval->GetRep(), NULL, flags_, mbr);
  }
  else {
    rdat = taiMember::GetDataRep_impl(host_, rval, rval->GetRep(), flags_, mbr);
  }
  rval->data = rdat;
  rval->AddChildWidget(rdat->GetRep());
  rval->EndLayout();
  return rval;
}

void taiTDefaultMember::GetImage(taiData* dat, const void* base) {
  QCAST_MBR_SAFE_EXIT(taiPlusToggle*, rval, dat)
  if (HasLowerBidder()) {
    LowerBidder()->GetImage(rval->data, base);
  }
  else {
    taiMember::GetImage_impl(rval->data, base);
  }
  taBase_List* gp = typ->defaults;
  tpdflt = NULL;
  if (gp != NULL) {
    for (int i = 0; i < gp->size; ++i) {
      TypeDefault* td = (TypeDefault*)gp->FastEl(i);
      if (td->token == (taBase*)base) {
        tpdflt = td;
        break;
      }
    }
  }
  if (tpdflt != NULL)
    rval->GetImage(tpdflt->GetActive(mbr->idx));
  GetOrigVal(dat, base);
}

void taiTDefaultMember::GetMbrValue(taiData* dat, void* base, bool& first_diff) {
  //note: we don't call the inherited, or use the impls
  QCAST_MBR_SAFE_EXIT(taiPlusToggle*, rval, dat)
  if (HasLowerBidder()) {
    LowerBidder()->GetMbrValue(rval->data, base, first_diff);
  }
  else {
    taiMember::GetMbrValue(rval->data, base,first_diff);
  }
  if (tpdflt != NULL) {         // gotten by prev GetImage
    tpdflt->SetActive(mbr->idx, rval->GetValue());
  }
}
