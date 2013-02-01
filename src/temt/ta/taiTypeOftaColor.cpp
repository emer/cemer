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

#include "taiTypeOftaColor.h"
#include <taiColor>
#include <taColor>


int taiTypeOftaColor::BidForType(TypeDef* td) {
  if (td->InheritsFrom(TA_taColor))
    return (inherited::BidForType(td) +1);
  return 0;
}

void taiTypeOftaColor::GetImage_impl(taiWidget* dat_, const void* base) {
  taiColor* dat = dynamic_cast<taiColor*>(dat_); // for safety
  if (!dat) return;
  const taColor* col = static_cast<const taColor*>(base);
  dat->GetImage(iColor(col->r, col->g, col->b, col->a));
}

void taiTypeOftaColor::GetValue_impl(taiWidget* dat_, void* base) {
  taiColor* dat = dynamic_cast<taiColor*>(dat_); // for safety
  if (!dat) return;
  iColor icol = dat->GetValue();
  taColor* col = static_cast<taColor*>(base);
  col->Set(icol.redf(), icol.greenf(), icol.bluef(), icol.alphaf());
}

