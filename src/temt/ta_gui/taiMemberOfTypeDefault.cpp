// Co2018ght 2013-2017, Regents of the University of Colorado,
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

#include "taiMemberOfTypeDefault.h"
#include <TypeDefault>
#include <taiWidgetPlusToggle>
#include <taiWidget>

#include <taMisc>


int taiMemberOfTypeDefault::BidForMember(MemberDef*, TypeDef*) {
  // TD_Default member does not bid, it is only applied in special cases.
  return 0;
}

taiWidget* taiMemberOfTypeDefault::GetWidgetRep(IWidgetHost* host_, taiWidget* par, QWidget* gui_parent_,
                                       taiType*, int flags_, MemberDef* mbr)
{
  taiWidgetPlusToggle* rval = new taiWidgetPlusToggle(typ, host_, par, gui_parent_, flags_);
  rval->InitLayout();
  taiWidget* rdat;
  if (HasLowerBidder()) {
    rdat = LowerBidder()->GetWidgetRep(host_, rval, rval->GetRep(), NULL, flags_, mbr);
  }
  else {
    rdat = taiMember::GetWidgetRep_impl(host_, rval, rval->GetRep(), flags_, mbr);
  }
  rval->AddChildWidget(rdat->GetRep());
  rval->EndLayout();
  return rval;
}

void taiMemberOfTypeDefault::GetImage(taiWidget* dat, const void* base) {
  taiWidgetPlusToggle* rval = (taiWidgetPlusToggle*)dat;
  taiWidget* sub_dat= rval->widget_el.FastEl(0);
  if (HasLowerBidder()) {
    LowerBidder()->GetImage(sub_dat, base);
  }
  else {
    taiMember::GetImage_impl(sub_dat, base);
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

void taiMemberOfTypeDefault::GetMbrValue(taiWidget* dat, void* base, bool& first_diff) {
  taiWidgetPlusToggle* rval = (taiWidgetPlusToggle*)dat;
  taiWidget* sub_dat= rval->widget_el.FastEl(0);
  if (HasLowerBidder()) {
    LowerBidder()->GetMbrValue(sub_dat, base, first_diff);
  }
  else {
    taiMember::GetMbrValue(sub_dat, base,first_diff);
  }
  if (tpdflt != NULL) {         // gotten by prev GetImage
    tpdflt->SetActive(mbr->idx, rval->GetValue());
  }
}
