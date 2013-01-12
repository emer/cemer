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

#include "iPoint.h"

#ifdef TA_USE_INVENTOR
  #include <Inventor/SbLinear.h>

iPoint::iPoint(const SbVec2s& src) {
  short xs; short ys;
  src.getValue(xs, ys);
  x = xs; y = ys;
}

iPoint& iPoint::operator=(const SbVec2s& src) {
  short xs; short ys;
  src.getValue(xs, ys);
  x = xs; y = ys;
  return *this;
}

iPoint::operator SbVec2s() const {
  return SbVec2s((short)x, (short)y);
}

#endif

#ifdef TA_GUI
  #include "qpoint.h"

iPoint::iPoint(const QPoint& val)
: x(val.x()), y(val.y())
{
}

iPoint& iPoint::operator=(const QPoint& val) {
  x = val.x();
  y = val.y();
  return *this;
}

iPoint::operator QPoint() const {
  return QPoint(x, y);
}
#endif

int iPoint::getArrayIndex(int x_, int y_) {
  if ((x_ >= x) || (y_ >= y)) return -1;
  else return (y_ * x) + x_;
}

