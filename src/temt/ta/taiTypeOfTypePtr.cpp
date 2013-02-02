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

#include "taiTypeOfTypePtr.h"
#include <taiWidget>
#include <taiWidgetTypeDefChooser>


void taiTypeOfTypePtr::Initialize() {
  orig_typ = NULL;
}

int taiTypeOfTypePtr::BidForType(TypeDef* td) {
  if(td->DerivesFrom(TA_TypeDef) && (td->IsPointer()))
    return taiType::BidForType(td) + 1;
  return 0;
}

// todo: the problem is that it doesn't know at this point what the base is
// and can't therefore figure out what kind of datarep to use..
// need to have a datarep that is a "string or type menu" kind of thing..

taiWidget* taiTypeOfTypePtr::GetWidgetRep_impl(IWidgetHost* host_, taiWidget* par, QWidget* gui_parent_, int flags_, MemberDef* mbr_) {
  if(!typ->HasOption(TypeItem::opt_NO_APPLY_IMMED))
    flags_ |= taiWidget::flgAutoApply; // default is to auto-apply!

  if (orig_typ == NULL)
    return taiType::GetWidgetRep_impl(host_, par, gui_parent_, flags_, mbr_);

  taiWidgetTypeDefChooser* rval =
    new taiWidgetTypeDefChooser(typ, host_, par, gui_parent_, flags_);
  return rval;
}

void taiTypeOfTypePtr::GetImage_impl(taiWidget* dat, const void* base) {
  if (orig_typ == NULL) {
    taiType::GetImage_impl(dat, base);
    return;
  }

  taiWidgetTypeDefChooser* rval = (taiWidgetTypeDefChooser*)dat;
  TypeDef* typ_ = (TypeDef*)*((void**)base);
  rval->GetImage((TypeDef*)*((void**)base), typ_);
}

void taiTypeOfTypePtr::GetValue_impl(taiWidget* dat, void* base) {
  if (orig_typ == NULL) {
    taiType::GetValue_impl(dat, base);
    return;
  }

  taiWidgetTypeDefChooser* rval = (taiWidgetTypeDefChooser*)dat;
  *((void**)base) = rval->GetValue();
}
