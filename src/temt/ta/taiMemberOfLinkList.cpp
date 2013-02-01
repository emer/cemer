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

#include "taiMemberOfLinkList.h"
#include <gpiListLinkEditButton>

TypeDef_Of(taList_impl);


int taiMemberOfLinkList::BidForMember(MemberDef* md, TypeDef* td) {
  if ((md->type->InheritsFrom(TA_taList_impl)) &&
     (md->HasOption("LINK_GROUP")))
    return (taiMember::BidForMember(md,td) + 1);
  return 0;
}

taiWidget* taiMemberOfLinkList::GetDataRep_impl(IWidgetHost* host_, taiWidget* par, QWidget* gui_parent_, int flags_, MemberDef*) {
  gpiListLinkEditButton* rval = new gpiListLinkEditButton(NULL, mbr->type, host_, par, gui_parent_, flags_);
  return rval;
}

void taiMemberOfLinkList::GetImage_impl(taiWidget* dat, const void* base) {
  gpiListLinkEditButton* rval = (gpiListLinkEditButton*)dat;
  rval->GetImage_(mbr->GetOff(base));
}

void taiMemberOfLinkList::GetMbrValue(taiWidget*, void*, bool&) {
}
