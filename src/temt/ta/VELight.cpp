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

void VELight::Initialize() {
  light_type = SPOT_LIGHT;
  drop_off_rate = 0.0f;
  cut_off_angle = 45.0f;
  dir_norm.x = 0.0f; dir_norm.y = 0.0f; dir_norm.z = -1.0f;
  shape = NO_SHAPE;             // having a shape will interfere with light!  but might want to see where it is sometimes..
  long_axis = LONG_Z;
  mass = .01f;
  radius = .01f;
  length = .01f;
}

// in ta_virtenv_qtso.cpp: SoLight* VELight::CreateLight()

// in ta_virtenv_qtso.cpp: void VELight::ConfigLight(SoLight* lgt)

void VELight::Init() {
  inherited::Init();
  SbVec3f dn(0.0f, 0.0f, -1.0f);
  SbRotation sbrot(SbVec3f(cur_rot.x, cur_rot.y, cur_rot.z), cur_rot.rot);
  sbrot.multVec(dn, dn);
  dir_norm.x = dn[0]; dir_norm.y = dn[1]; dir_norm.z = dn[2];
}

void VELight::CurFromODE(bool updt_disp) {
  inherited::CurFromODE(updt_disp);
  SbVec3f dn(0.0f, 0.0f, -1.0f);
  SbRotation sbrot(SbVec3f(cur_rot.x, cur_rot.y, cur_rot.z), cur_rot.rot);
  sbrot.multVec(dn, dn);
  dir_norm.x = dn[0]; dir_norm.y = dn[1]; dir_norm.z = dn[2];
}

