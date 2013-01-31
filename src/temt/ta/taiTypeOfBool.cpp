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
#include <taiData>
#include <taiToggle>



int taiTypeOfBool::BidForType(TypeDef* td){
  if(td->IsBool())
    return (taiType::BidForType(td) +1);
  return 0;
}

taiData* taiTypeOfBool::GetDataRep_impl(IDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_, MemberDef*){
  if(!typ->HasOption(TypeItem::opt_NO_APPLY_IMMED))
    flags_ |= taiData::flgAutoApply; // default is to auto-apply!
  taiToggle* rval = new taiToggle(typ, host_, par, gui_parent_, flags_);
  return rval;
}

void taiTypeOfBool::GetImage_impl(taiData* dat, const void* base) {
  bool val = *((bool*)base);
  taiToggle* rval = (taiToggle*)dat;
  rval->GetImage(val);
}

void taiTypeOfBool::GetValue_impl(taiData* dat, void* base) {
  taiToggle* rval = (taiToggle*)dat;
  *((bool*)base) = rval->GetValue();
}
