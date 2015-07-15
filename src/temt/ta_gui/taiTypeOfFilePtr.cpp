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

#include "taiTypeOfFilePtr.h"
#include <taiWidgetFileButton>
#include <taFiler>


int taiTypeOfFilePtr::BidForType(TypeDef* td) {
  if(td->DerivesFrom(TA_taFiler) && (td->IsPointer()))
    return (taiType::BidForType(td) +1);
  return 0;
}

taiWidget* taiTypeOfFilePtr::GetWidgetRep_impl(IWidgetHost* host_, taiWidget* par, QWidget* gui_parent_, int flags_, MemberDef*) {
  return new taiWidgetFileButton(typ,  host_, par, gui_parent_, flags_);
}

void taiTypeOfFilePtr::GetImage_impl(taiWidget* dat, const void* base){
  taiWidgetFileButton* fbut = (taiWidgetFileButton*) dat;
  // note: we are a taFiler*
  fbut->SetFiler(*((taFiler**)base));
  fbut->GetImage();
}

void taiTypeOfFilePtr::GetValue_impl(taiWidget* dat, void* base) {
  taiWidgetFileButton* rval = (taiWidgetFileButton*)dat;
  // safely replace filer, using ref counting
  taRefN::SetRefDone(*((taRefN**)base), rval->GetFiler());
}

