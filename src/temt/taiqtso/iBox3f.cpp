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

#include "iBox3f.h"

#ifdef TA_GUI
  #include "qpoint.h"
  #include "qsize.h"
  #include "qrect.h"
#endif

#ifdef TA_USE_INVENTOR
  #include <Inventor/SbBox.h>

iBox3f::operator SbBox3f() const {
  return SbBox3f(min.x, min.y, min.z, max.x, max.y, max.z);
}

#endif

#ifndef MAX
#define	MAX(a,b) (((a) > (b)) ? (a) : (b))
#define	MIN(a,b) (((a) < (b)) ? (a) : (b))
#endif

void iBox3f::MaxOf(const iBox3f& val1, const iBox3f& val2) {
  min.x = MIN(val1.min.x, val2.min.x);
  min.y = MIN(val1.min.y, val2.min.y);
  min.z = MIN(val1.min.z, val2.min.z);
  max.x = MAX(val1.max.x, val2.max.x);
  max.y = MAX(val1.max.y, val2.max.y);
  max.z = MAX(val1.max.z, val2.max.z);
}
