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

#include "EditMthItem.h"

TA_BASEFUNS_CTORS_DEFN(EditMthItem);


void EditMthItem::Initialize() {
  mth = NULL;
}

void EditMthItem::Destroy() {
}

void EditMthItem::Copy_(const EditMthItem& cp) {
  mth = cp.mth;
}

void EditMthItem::UpdateAfterEdit_impl() {
  // version 5.0.2 -- update to new Gui buttons for Run and Step
  if(mth) {
    if(((mth->name == "Run") || (mth->name == "Step")) && !mth->im) {
      MethodDef* nwmth = mth->owner->FindName(mth->name + "_Gui");
      if(nwmth) {
        mth = nwmth;
      }
    }
  }
  if(!cust_desc && mth) {
    desc = mth->desc;
    prv_desc = desc;
  }
  inherited::UpdateAfterEdit_impl();
}

