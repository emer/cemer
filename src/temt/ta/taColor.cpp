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

#include "taColor.h"

#include <taMisc>

TA_BASEFUNS_CTORS_LITE_DEFN(taColor);

const iColor taColor::color() const {
  iColor rval(r,g,b,a);
  return rval;
}

void taColor::setColor(const iColor& cp) {
  Set(cp.redf(), cp.greenf(), cp.bluef(), cp.alphaf());
}

void taColor::setColorName(const String& nm) {
  if(!(iColor::find((char*)nm,r,g,b))){
    taMisc::Error("Color: " , nm , " not found for this display");
  }
}

