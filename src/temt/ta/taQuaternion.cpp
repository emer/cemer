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

#include "taQuaternion.h"
#include <taAxisAngle>
#include <float_Matrix>
#include <taMisc>

#include <ode/ode.h>

void taQuaternion::ToMatrix(taMatrix& mat) const {
  mat.SetGeom(1,4); mat.SetFmVar(s,0); mat.SetFmVar(x,1); mat.SetFmVar(y,2);
  mat.SetFmVar(z,3);
}

void taQuaternion::FromMatrix(taMatrix& mat) {
  s = mat.SafeElAsVar(0).toFloat(); x = mat.SafeElAsVar(1).toFloat();
  y = mat.SafeElAsVar(2).toFloat(); z = mat.SafeElAsVar(3).toFloat();
}

void taQuaternion::FromAxisAngle(const taAxisAngle& axa) {
  float ang2 = axa.rot*0.5f; float sinang2 = sinf(ang2);
  s = cosf(ang2); x = axa.x * sinang2; y = axa.y * sinang2; z = axa.z * sinang2;
}

void taQuaternion::ToAxisAngle(taAxisAngle& axa) const {
  const float tol = 1.0e-4f;     // tolerance
  axa.rot = acosf(s);
  float sinangi = sinf(axa.rot);
  if(sinangi == 0.0f) {	// singularity -- angle == 0
    if(fabsf(axa.Mag() - 1.0f) > tol) { // only if it doesn't already have a valid axis
      axa.x = 0.0f; axa.y = 1.0f; axa.z = 0.0f; // provide a default axis..
    }
    return;
  }
  sinangi = 1.0f / sinangi;
  axa.x = x * sinangi; axa.y = y * sinangi; axa.z = z * sinangi;  axa.rot *= 2.0f;
}

void taQuaternion::ToRotMatrix(float_Matrix& mat) const {
  float mag = Mag();
  if(mag < 0.9999 || mag > 1.0001) {
    taMisc::Error("taQuaternion::ToMatrix -- must be normalized (Mag == 1.0), mag is:", 
                  String(mag));
    return;
  }
  mat.SetGeom(2,3,3);

  mat.Set(1.0f - 2.0f*(y*y+z*z), 0,0);
  mat.Set(2.0f*(x*y-s*z),1,0);
  mat.Set(2.0f*(x*z+s*y),2,0);

  mat.Set(2.0f*(x*y+s*z),0,1);
  mat.Set(1.0f-2.0f*(x*x+z*z), 1,1);
  mat.Set(2.0f*(y*z-s*x),2,1);
    
  mat.Set(2.0f*(x*z-s*y),0,2);
  mat.Set(2.0f*(y*z+s*x),1,2);
  mat.Set(1.0f-2.0f*(x*x+y*y),2,2);
}

taQuaternion& taQuaternion::operator=(const taAxisAngle& cp) {
  FromAxisAngle(cp); return *this;
}

taQuaternion taQuaternion::operator / (float scale) const {
  taQuaternion rv;
   if(scale != 0.0f) { rv.s = s / scale; rv.x = x / scale; rv.y = y / scale; rv.z = z / scale; }
   else	      { taMisc::Error("Quaternion -- division by 0 scalar"); }
return rv;
}

taQuaternion& taQuaternion::operator /= (float scale) {
  if(scale != 0.0f) { s /= scale; x /= scale; y /= scale; z /= scale; }
  else	      { taMisc::Error("Quaternion -- division by 0 scalar"); }
  return *this;
}

void taQuaternion::ToODE(dQuaternion dq) const{
  dq[0] = s; dq[1] = x; dq[2] = y; dq[3] = z;
}

void taQuaternion::FromODE(const dQuaternion dq) {
  s = dq[0]; x = dq[1]; y = dq[2]; z = dq[3];
}
