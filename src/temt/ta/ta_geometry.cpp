// Copyright, 1995-2007, Regents of the University of Colorado,
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


#include "ta_geometry.h"

#ifdef TA_USE_INVENTOR
#include <Inventor/SbLinear.h>
#include <Inventor/nodes/SoTransform.h>
#endif

TwoDCoord::TwoDCoord(const FloatTwoDCoord& cp) {
  Register(); Initialize(); x = (int)cp.x; y = (int)cp.y;
}


TwoDCoord& TwoDCoord::operator=(const FloatTwoDCoord& cp) {
  x = (int)cp.x; y = (int)cp.y;
  return *this;
}

void TwoDCoord::CopyToMatrixGeom(MatrixGeom& geom) {
// NOTE: TDCoord just replaces this whole routine, for simplicity
  geom.SetDims(2);
  geom.FastEl(0) = x;
  geom.FastEl(1) = y;
}

bool TwoDCoord::FitN(int n) {
  if((x * y) == n)	return false;
  y = (int)sqrtf((float)n);
  if(y < 1)
    y = 1;
  x = n / y;
  if(x*y == n) return true; // got lucky

  // next try a range of y's to fit evenly
  int sqrty = y;
  int lwy = y/2;  int hiy = y*2;
  if(lwy == 0) lwy = 1;
  for(y = lwy; y<=hiy; y++) {
    x = n / y;
    if(x*y == n) return true; // got lucky
  }
  if(n < 20) {
    x = n;    y = 1;		// just go linear for small values
    return true;
  }
  // else just go with an imperfect fit
  y = sqrty;
  x = n / y;
  while((x * y) < n)
    x++;
  return true;
}

bool TwoDCoord::WrapClipOne(bool wrap, int& c, int max) {
  bool out_of_range = false;
  if(wrap) {
    if(c >= max) {
      if(c > max + max/2) out_of_range = true; // wraps past half way to other side
      c = c % max;
    }
    else if(c < 0) {
      if(c < -max/2) out_of_range = true; // wraps past half way to other side
      c = max + (c % max);
    }
  }
  else {
    if(c >= max) {
      out_of_range = true;
      c = max-1;
    }
    else if(c < 0) {
      out_of_range = true;
      c = 0;
    }
  }
  return out_of_range;
}


void PosTwoDCoord::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  SetGtEq(0);
}

void XYNGeom::Initialize() {
  x = 1;
  y = 1;
  n_not_xy = false;
  n = 1;
  z = 0;
}

void XYNGeom::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  if(n_not_xy && n > x*y) { // only if not fitting, expand
    FitN(n);
  }
  if(n_not_xy) {
    if(x * y == n) n_not_xy = false; // no need for flag
  }
  else {
    n = x * y;			// always keep n up-to-date
  }
}

bool XYNGeom::FitN(int no) {
  bool rval = inherited::FitN(no);
  n = no;
  if(x * y == n) n_not_xy = false;
  else n_not_xy = true;
  return rval;
}

void XYNGeom::operator=(const TwoDCoord& cp) {
  x = cp.x; y = cp.y; UpdateAfterEdit();
}

TDCoord::TDCoord(const FloatTDCoord& cp) {
  Register(); Initialize();
  x = (int)cp.x; y = (int)cp.y; z = (int)cp.z;
}

TDCoord& TDCoord::operator=(const FloatTDCoord& cp) {
  x = (int)cp.x; y = (int)cp.y;	z = (int)cp.z;
  return *this;
}

void TDCoord::CopyToMatrixGeom(MatrixGeom& geom) {
// NOTE: TDCoord just replaces this whole routine, for simplicity
  geom.SetDims(3);
  geom.FastEl(0) = x;
  geom.FastEl(1) = y;
  geom.FastEl(2) = z;
}

bool TDCoord::FitNinXY(int n) {
  return FitN(n);
}

void PosTDCoord::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  SetGtEq(0);
}


FloatTwoDCoord::FloatTwoDCoord(const TwoDCoord& cp) {
  Register(); Initialize(); x = (float)cp.x; y = (float)cp.y;
}

FloatTwoDCoord& FloatTwoDCoord::operator=(const TwoDCoord& cp) {
  x = (float)cp.x; y = (float)cp.y;
  return *this;
}

FloatTDCoord::FloatTDCoord(const TDCoord& cp) {
  Register(); Initialize();
  x = (float)cp.x; y = (float)cp.y; z = (float)cp.z;
}

FloatTDCoord& FloatTDCoord::operator=(const TDCoord& cp) {
  x = (float)cp.x; y = (float)cp.y; z = (float)cp.z;
  return *this;
}

#ifdef TA_USE_INVENTOR

void FloatTransform::CopyTo(SoTransform* txfm) {
  if (!txfm) return;
  txfm->translation.setValue(SbVec3f(translate.x, translate.y, translate.z));
  txfm->rotation.setValue(SbVec3f(rotate.x, rotate.y, rotate.z), rotate.rot);
  txfm->scaleFactor.setValue(SbVec3f(scale.x, scale.y, scale.z));
}

#endif

const ValIdx ValIdx_Array::blank;

