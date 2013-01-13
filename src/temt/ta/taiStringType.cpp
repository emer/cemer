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

#include "taiStringType.h"
#include <taiField>
#include <MemberDef>


int taiStringType::BidForType(TypeDef* td){
  if (td->InheritsFrom(TA_taString))
    return(taiType::BidForType(td) + 2); // outbid class
  return 0;
}

taiData* taiStringType::GetDataRep_impl(IDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_, MemberDef* md) {
  taiField* rval = new taiField(typ, host_, par, gui_parent_, flags_);
  rval->lookupfun_md = md;              // for lookup function
  if(md) {
    String ew = md->OptionAfter("EDIT_WIDTH_");
    if(ew.nonempty()) {
      int ewi = (int)ew;
      if(ewi > 0) {
        rval->setMinCharWidth(ewi);
      }
    }
  }
  return rval;
}

void taiStringType::GetImage_impl(taiData* dat, const void* base) {
  ((taiField*)dat)->lookupfun_base = GetCurParObjBase(); // for lookup function
  dat->GetImage_(base);
}

void taiStringType::GetValue_impl(taiData* dat, void* base) {
  dat->GetValue_(base); //noop for taiEditButton
}
