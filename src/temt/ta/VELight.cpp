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

#include "VELight.h"

#include <Inventor/SbLinear.h>

void VELight::Initialize() {
  flags = (BodyFlags)(FIXED | NO_COLLIDE | EULER_ROT);
  light_type = DIRECTIONAL_LIGHT;
  drop_off_rate = 0.1f;
  cut_off_angle = 30.0f;
  dir_norm.x = 0.0f; dir_norm.y = 0.0f; dir_norm.z = -1.0f;
  shape = CYLINDER;  // having a shape will interfere with spotlight!  but might want to see where it is sometimes..
  long_axis = LONG_Z;
  mass = .1f;
  radius = .1f;
  length = .2f;
}

// in VEWorldView.cpp: SoLight* VELight::CreateLight()

// in VEWorldView.cpp: void VELight::ConfigLight(SoLight* lgt)

void VELight::UpdtDirNorm() {
  taVector3f dn(0.0f, 0.0f, -1.0f);
  cur_quat.RotateVec(dn);
  dir_norm = dn;
}

void VELight::Init() {
  inherited::Init();
  UpdtDirNorm();
}

void VELight::CurFromODE(bool updt_disp) {
  inherited::CurFromODE(updt_disp);
  UpdtDirNorm();
}

void VELight::CurToODE() {
  inherited::CurToODE();
  UpdtDirNorm();
}
