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

taVector2i::taVector2i(const taVector2f& cp) {
  Register(); Initialize(); x = (int)cp.x; y = (int)cp.y;
}


taVector2i& taVector2i::operator=(const taVector2f& cp) {
  x = (int)cp.x; y = (int)cp.y;
  return *this;
}

void taVector2i::CopyToMatrixGeom(MatrixGeom& geom) {
// NOTE: taVector3i just replaces this whole routine, for simplicity
  geom.SetDims(2);
  geom.Set(0, x);
  geom.Set(1, y);
}

void taVector2i::CopyToMatrixIndex(MatrixIndex& idx) {
// NOTE: taVector3i just replaces this whole routine, for simplicity
  idx.SetDims(2);
  idx.Set(0, x);
  idx.Set(1, y);
}

bool taVector2i::FitN(int n) {
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

bool taVector2i::WrapClipOne(bool wrap, int& c, int max) {
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


void PosVector2i::UpdateAfterEdit_impl() {
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

void XYNGeom::operator=(const taVector2i& cp) {
  x = cp.x; y = cp.y; UpdateAfterEdit_NoGui();
}

taVector3i::taVector3i(const taVector3f& cp) {
  Register(); Initialize();
  x = (int)cp.x; y = (int)cp.y; z = (int)cp.z;
}

taVector3i& taVector3i::operator=(const taVector3f& cp) {
  x = (int)cp.x; y = (int)cp.y;	z = (int)cp.z;
  return *this;
}

void taVector3i::CopyToMatrixGeom(MatrixGeom& geom) {
  geom.SetDims(3);
  geom.Set(0, x);
  geom.Set(1, y);
  geom.Set(2, z);
}

void taVector3i::CopyToMatrixIndex(MatrixIndex& idx) {
  idx.SetDims(3);
  idx.Set(0, x);
  idx.Set(1, y);
  idx.Set(2, z);
}

bool taVector3i::FitNinXY(int n) {
  return FitN(n);
}

void PosVector3i::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  SetGtEq(0);
}


taVector2f::taVector2f(const taVector2i& cp) {
  Register(); Initialize(); x = (float)cp.x; y = (float)cp.y;
}

taVector2f& taVector2f::operator=(const taVector2i& cp) {
  x = (float)cp.x; y = (float)cp.y;
  return *this;
}

taVector3f::taVector3f(const taVector3i& cp) {
  Register(); Initialize();
  x = (float)cp.x; y = (float)cp.y; z = (float)cp.z;
}

taVector3f& taVector3f::operator=(const taVector3i& cp) {
  x = (float)cp.x; y = (float)cp.y; z = (float)cp.z;
  return *this;
}

#ifdef TA_USE_INVENTOR

void taTransform::CopyTo(SoTransform* txfm) {
  if (!txfm) return;
  txfm->translation.setValue(SbVec3f(translate.x, translate.y, translate.z));
  txfm->rotation.setValue(SbVec3f(rotate.x, rotate.y, rotate.z), rotate.rot);
  txfm->scaleFactor.setValue(SbVec3f(scale.x, scale.y, scale.z));
}

#endif

const ValIdx ValIdx_Array::blank;

taAxisAngle& taAxisAngle::operator=(const taQuaternion& cp) {
  cp.ToAxisAngle(*this);
  return *this;
}

void taAxisAngle::FromEuler(float theta_x, float theta_y, float theta_z) {
  taQuaternion q(theta_x, theta_y, theta_z);
  q.ToAxisAngle(*this);
}

void taAxisAngle::RotateAxis(float x_axis, float y_axis, float z_axis, float rot_ang) {
  taQuaternion q(*this);
  q.RotateAxis(x_axis, y_axis, z_axis, rot_ang);
  q.ToAxisAngle(*this);
}

void taAxisAngle::RotateEuler(float theta_x, float theta_y, float theta_z) {
  taQuaternion q(*this);
  q.RotateEuler(theta_x, theta_y, theta_z);
  q.ToAxisAngle(*this);
}

void taAxisAngle::RotateXYZ(float& x, float& y, float& z) {
  taQuaternion q(*this);
  q.RotateXYZ(x, y, z);
}

void taAxisAngle::RotateVec(taVector3f& vec) {
  taQuaternion q(*this);
  q.RotateVec(vec);
}

