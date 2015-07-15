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

#include "taAxisAngle.h"
#include <taQuaternion>

TA_BASEFUNS_CTORS_LITE_DEFN(taAxisAngle);

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

