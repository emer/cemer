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

#include "taiMemberOfLinkGroup.h"
#include <gpiLinkEditButton>
#include <taGroup_impl>



int taiMemberOfLinkGroup::BidForMember(MemberDef* md, TypeDef* td) {
  if ((md->type->InheritsFrom(TA_taGroup_impl)) &&
     (md->HasOption("LINK_GROUP")))
    return (taiMember::BidForMember(md,td) + 1);
  return 0;
}

taiWidget* taiMemberOfLinkGroup::GetDataRep_impl(IWidgetHost* host_, taiWidget* par, QWidget* gui_parent_, int flags_, MemberDef*) {
  gpiLinkEditButton* rval = new gpiLinkEditButton(NULL, mbr->type, host_, par, gui_parent_, flags_);
  return rval;
}

void taiMemberOfLinkGroup::GetImage_impl(taiWidget* dat, const void* base) {
  gpiLinkEditButton* rval = (gpiLinkEditButton*)dat;
  rval->GetImage_(mbr->GetOff(base));
}

void taiMemberOfLinkGroup::GetMbrValue(taiWidget*, void*, bool&) {
}
