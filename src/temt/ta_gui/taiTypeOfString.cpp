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

#include "taiTypeOfString.h"
#include <taiWidgetField>
#include <MemberDef>


int taiTypeOfString::BidForType(TypeDef* td){
  if (td->IsString())
    return(taiType::BidForType(td) + 2); // outbid class
  return 0;
}

taiWidget* taiTypeOfString::GetWidgetRep_impl(IWidgetHost* host_, taiWidget* par, QWidget* gui_parent_, int flags_, MemberDef* md) {
  taiWidgetField* rval = new taiWidgetField(typ, host_, par, gui_parent_, flags_);
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

void taiTypeOfString::GetImage_impl(taiWidget* dat, const void* base) {
  ((taiWidgetField*)dat)->lookupfun_base = GetCurParObjBase(); // for lookup function
  dat->GetImage_(base);
}

void taiTypeOfString::GetValue_impl(taiWidget* dat, void* base) {
  dat->GetValue_(base); //noop for taiWidgetEditButton
}
