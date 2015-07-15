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

#include "iRect.h"

#ifdef TA_GUI
  #include "qrect.h"
iRect::iRect(const QRect& val)
: x(val.left()), y(val.top()), w(val.width()), h(val.height())
{
}

iRect& iRect::operator=(const QRect& val) {
  x = val.left();
  y = val.top();
  w = val.width();
  h = val.height();
  return *this;
}

iRect::operator QRect() const {
  return QRect(x, y, w, h);
}
#endif

iRect::iRect(): x(0), y(0), w(0), h(0) {}
iRect::iRect(const iVec2i& topLeft, const iVec2i& bottomRight)
    : x(topLeft.x), y(topLeft.y), w(bottomRight.x - topLeft.x), h(bottomRight.y - topLeft.y) {}
iRect::iRect(const iVec2i& topLeft, const iSize& size )
    : x(topLeft.x), y(topLeft.y), w(size.w), h(size.h) {}
iRect::iRect(int left, int top, int width, int height)
    : x(left), y(top), w(width), h(height) {}
