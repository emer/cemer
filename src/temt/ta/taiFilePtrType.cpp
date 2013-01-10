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

#include "taiFilePtrType.h"

int taiFilePtrType::BidForType(TypeDef* td) {
  if(td->DerivesFrom(TA_taFiler) && (td->ptr == 1))
    return (taiType::BidForType(td) +1);
  return 0;
}

taiData* taiFilePtrType::GetDataRep_impl(IDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_, MemberDef*) {
  return new taiFileButton(typ,  host_, par, gui_parent_, flags_);
}

void taiFilePtrType::GetImage_impl(taiData* dat, const void* base){
  taiFileButton* fbut = (taiFileButton*) dat;
  // note: we are a taFiler*
  fbut->SetFiler(*((taFiler**)base));
  fbut->GetImage();
}

void taiFilePtrType::GetValue_impl(taiData* dat, void* base) {
  taiFileButton* rval = (taiFileButton*)dat;
  // safely replace filer, using ref counting
  taRefN::SetRefDone(*((taRefN**)base), rval->GetFiler());
}

