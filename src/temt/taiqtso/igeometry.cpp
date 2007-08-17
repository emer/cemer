// Copyright, 1995-2005, Regents of the University of Colorado,
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

#include "igeometry.h"

#ifdef TA_GUI
  #include "qpoint.h"
  #include "qsize.h"
  #include "qrect.h"
#endif

#ifdef TA_USE_INVENTOR
  #include <Inventor/SbLinear.h>
  #include <Inventor/SbBox.h>
#endif

#ifndef MAX
#define	MAX(a,b) (((a) > (b)) ? (a) : (b))
#define	MIN(a,b) (((a) < (b)) ? (a) : (b))
#endif

// NOTE: all Qt-related definitions are together below

//obs float iCoord::px_per_pr = 1.0;

//////////////////////////
//   iPoint		//
//////////////////////////

int iPoint::getArrayIndex(int x_, int y_) {
  if ((x_ >= x) || (y_ >= y)) return -1;
  else return (y_ * x) + x_;
}



iSize::iSize(): w(0), h(0) {}
iSize::iSize(int w_, int h_): w(w_), h(h_) {}

iRect::iRect(): x(0), y(0), w(0), h(0) {}
iRect::iRect(const iPoint& topLeft, const iPoint& bottomRight)
    : x(topLeft.x), y(topLeft.y), w(bottomRight.x - topLeft.x), h(bottomRight.y - topLeft.y) {}
iRect::iRect(const iPoint& topLeft, const iSize& size )
    : x(topLeft.x), y(topLeft.y), w(size.w), h(size.h) {}
iRect::iRect(int left, int top, int width, int height)
    : x(left), y(top), w(width), h(height) {}
//iRect::iRect(iCoord left, iCoord top, iCoord width, iCoord height)
//    : x(left.asPixel()), y(top.asPixel()), w(width.asPixel()), h(height.asPixel()) {}


/*nn void iBox3f::MaxOf(const iBox3f& val) {
  min.x = MIN(min.x, val.min.x);
  min.y = MIN(min.y, val.min.y);
  min.z = MIN(min.z, val.min.z);
  max.x = MAX(max.x, val.max.x);
  max.y = MAX(max.y, val.max.y);
  max.z = MAX(max.z, val.max.z);
} */

//////////////////////////
//   iBox3f		//
//////////////////////////

void iBox3f::MaxOf(const iBox3f& val1, const iBox3f& val2) {
  min.x = MIN(val1.min.x, val2.min.x);
  min.y = MIN(val1.min.y, val2.min.y);
  min.z = MIN(val1.min.z, val2.min.z);
  max.x = MAX(val1.max.x, val2.max.x);
  max.y = MAX(val1.max.y, val2.max.y);
  max.z = MAX(val1.max.z, val2.max.z);
}


#ifdef TA_GUI
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

iSize::iSize(const QSize& val)
: w(val.width()), h(val.height())
{
}

iSize& iSize::operator=(const QSize& val) {
  w = val.width();
  h = val.height();
  return *this;
}

iSize::operator QSize() const {
  return QSize(w, h);
}

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
#endif // def TA_GUI

// Note: all Inventor definitions are below:

#ifdef TA_USE_INVENTOR

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


iVec3i::iVec3i(const SbVec3s& src) {
  short xs; short ys; short zs;
  src.getValue(xs, ys, zs);
  x = xs; y = ys; z = zs;
}

iVec3i& iVec3i::operator=(const SbVec3s& src) {
  short xs; short ys; short zs;
  src.getValue(xs, ys, zs);
  x = xs; y = ys; z = zs;
  return *this;
}

iVec3i::operator SbVec3s() const {
  return SbVec3s((short)x, (short)y, (short)z);
}


iVec3f::iVec3f(const SbVec3f& src) {
  src.getValue(x, y, z);
}

iVec3f& iVec3f::operator=(const SbVec3f& src) {
  src.getValue(x, y, z);
  return *this;
}

iVec3f::operator SbVec3f() const {
  return SbVec3f(x, y, z);
}



iBox3f::operator SbBox3f() const {
  return SbBox3f(min.x, min.y, min.z, max.x, max.y, max.z);
}

/*
iTransform::iTransform(const SbMatrix& src) {
  src.getValue(vals);
}

iTransform& iTransform::operator=(const SbMatrix& src) {
  src.getValue(vals);
  return *this;
}

void iTransform::copyTo(SbMatrix& dst) const {
  dst.setValue(vals);
}

void iTransform::copyFrom(const SbMatrix& src) {
  src.getValue(vals);
}

iTransform::operator SbMatrix() const {
  return SbMatrix(vals);
}
*/

#endif  // def TA_USE_INVENTOR

