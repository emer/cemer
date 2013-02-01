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

#include "taiTypeOfBool.h"
#include <taiWidget>
#include <taiWidgetToggle>



int taiTypeOfBool::BidForType(TypeDef* td){
  if(td->IsBool())
    return (taiType::BidForType(td) +1);
  return 0;
}

taiWidget* taiTypeOfBool::GetDataRep_impl(IWidgetHost* host_, taiWidget* par, QWidget* gui_parent_, int flags_, MemberDef*){
  if(!typ->HasOption(TypeItem::opt_NO_APPLY_IMMED))
    flags_ |= taiWidget::flgAutoApply; // default is to auto-apply!
  taiWidgetToggle* rval = new taiWidgetToggle(typ, host_, par, gui_parent_, flags_);
  return rval;
}

void taiTypeOfBool::GetImage_impl(taiWidget* dat, const void* base) {
  bool val = *((bool*)base);
  taiWidgetToggle* rval = (taiWidgetToggle*)dat;
  rval->GetImage(val);
}

void taiTypeOfBool::GetValue_impl(taiWidget* dat, void* base) {
  taiWidgetToggle* rval = (taiWidgetToggle*)dat;
  *((bool*)base) = rval->GetValue();
}
