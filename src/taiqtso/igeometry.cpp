/* -*- C++ -*- */
/*=============================================================================
//									      //
// This file is part of the PDP++ software package.			      //
//									      //
// Copyright (C) 1995 Randall C. O'Reilly, Chadley K. Dawson, 		      //
//		      James L. McClelland, and Carnegie Mellon University     //
//     									      //
// Permission to use, copy, and modify this software and its documentation    //
// for any purpose other than distribution-for-profit is hereby granted	      //
// without fee, provided that the above copyright notice and this permission  //
// notice appear in all copies of the software and related documentation.     //
//									      //
// Permission to distribute the software or modified or extended versions     //
// thereof on a not-for-profit basis is explicitly granted, under the above   //
// conditions. 	HOWEVER, THE RIGHT TO DISTRIBUTE THE SOFTWARE OR MODIFIED OR  //
// EXTENDED VERSIONS THEREOF FOR PROFIT IS *NOT* GRANTED EXCEPT BY PRIOR      //
// ARRANGEMENT AND WRITTEN CONSENT OF THE COPYRIGHT HOLDERS.                  //
// 									      //
// Note that the taString class, which is derived from the GNU String class,  //
// is Copyright (C) 1988 Free Software Foundation, written by Doug Lea, and   //
// is covered by the GNU General Public License, see ta_string.h.             //
// The iv_graphic library and some iv_misc classes were derived from the      //
// InterViews morpher example and other InterViews code, which is             //
// Copyright (C) 1987, 1988, 1989, 1990, 1991 Stanford University             //
// Copyright (C) 1991 Silicon Graphics, Inc.				      //
//									      //
// THE SOFTWARE IS PROVIDED "AS-IS" AND WITHOUT WARRANTY OF ANY KIND,         //
// EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY 	      //
// WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.  	      //
// 									      //
// IN NO EVENT SHALL CARNEGIE MELLON UNIVERSITY BE LIABLE FOR ANY SPECIAL,    //
// INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY KIND, OR ANY DAMAGES  //
// WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER OR NOT     //
// ADVISED OF THE POSSIBILITY OF DAMAGE, AND ON ANY THEORY OF LIABILITY,      //
// ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS        //
// SOFTWARE. 								      //
==============================================================================*/

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

//////////////////////////
//   iTransform		//
//////////////////////////
/*
static SbMat unity_mat = {
  {1.0f, 0.0f, 0.0f, 0.0f},
  {0.0f, 1.0f, 0.0f, 0.0f},
  {0.0f, 0.0f, 1.0f, 0.0f},
  {0.0f, 0.0f, 0.0f, 1.0f}
};

iTransform::iTransform() {
  memset(vals, 0, sizeof(vals));
  a11 = 1.0f; a22 = 1.0f; a33 = 1.0f; a44 = 1.0f;
}

iTransform::iTransform(const iTransform& src) {
  memcpy(vals, src.vals, sizeof(vals));
}

bool iTransform::isUnity() const {
  return (vals == unity_mat);
}

void iTransform::rotate(const iVec3f &axis, const float angle) {
  //TODO:
}

void iTransform::rotate(const iVec3f &rotateFrom, const iVec3f &rotateTo) {
}

void iTransform::scale(float s) {
  scale(s, s, s);
}

void iTransform::scale(float sx, float sy, float sz) {
  a11 *= sx;  a22 *= sy;  a33 *= sz;
}

void iTransform::translate (float dx, float dy, float dz) {
  a41 += dx;  a42 += dy;  a43 += dz;
}

*/

#ifdef TA_USE_INVENTOR
#endif


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

