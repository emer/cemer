// Copyright 2017, Regents of the University of Colorado,
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

#include "taiMemberOfEnumTypePtr.h"
#include <taiWidget>
#include <taiWidgetEnumTypeDefChooser>



int taiMemberOfEnumTypePtr::BidForMember(MemberDef* md, TypeDef* td) {
  if (md->HasOption("ENUM_TYPE"))
    return (inherited::BidForMember(md,td) + 1);
  return 0;
}

taiWidget* taiMemberOfEnumTypePtr::GetWidgetRep_impl(IWidgetHost* host_, taiWidget* par, QWidget* gui_parent_,
  int flags_, MemberDef*)
{
  if (mbr->HasOption("NULL_OK"))
    flags_ |= taiWidget::flgNullOk;
  taiWidgetEnumTypeDefChooser* rval =
    new taiWidgetEnumTypeDefChooser(mbr->type, host_, par, gui_parent_, flags_);
  return rval;
}
