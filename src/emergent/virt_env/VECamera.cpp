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

#include "VECamera.h"

#include <taMath_float>

#ifdef TA_QT3D

#else // TA_QT3D
#include <Inventor/SbLinear.h>
#include <Inventor/nodes/SoPerspectiveCamera.h>
#endif // TA_QT3D

TA_BASEFUNS_CTORS_DEFN(VECamera);
SMARTREF_OF_CPP(VECamera); // VECameraRef

TA_BASEFUNS_CTORS_DEFN(VECameraDists);

void VECameraDists::Initialize() {
  near = 0.1f;
  focal = 1.0f;
  far = 10.0f;
}

void VECamera::Initialize() {
  img_size.x = 320;
  img_size.y = 240;
  color_cam = true;
  field_of_view = 90.0f;
  light.intensity = .2f;                // keep it not so bright relative to the sun..
  mass = .01f;
  radius = .01f;
  length = .01f;
  shape = CYLINDER;
  long_axis = LONG_Z;           // default orientation: must remain!
  dir_norm.x = 0.0f; dir_norm.y = 0.0f; dir_norm.z = -1.0f;
}

void VECamera::UpdtDirNorm() {
  taVector3f dn(0.0f, 0.0f, -1.0f);
  cur_quat.RotateVec(dn);
  dir_norm = dn;
}

void VECamera::Init() {
  inherited::Init();
  UpdtDirNorm();
}

void VECamera::CurFromODE(bool updt_disp) {
  inherited::CurFromODE(updt_disp);
  UpdtDirNorm();
}

void VECamera::CurToODE() {
  inherited::CurToODE();
  UpdtDirNorm();
}

#ifdef near
#undef near
#endif
#ifdef far
#undef far
#endif

#ifdef TA_QT3D

#else // TA_QT3D

void VECamera::ConfigCamera(SoPerspectiveCamera* cam) {
  cam->position.setValue(cur_pos.x, cur_pos.y, cur_pos.z);
  cam->orientation.setValue(cur_quat.x, cur_quat.y, cur_quat.z, cur_quat.s);
  // SbVec3f(cur_rot.x, cur_rot.y, cur_rot.z), cur_rot.rot);
  cam->nearDistance = this->view_dist.near;
  cam->focalDistance = view_dist.focal;
  cam->farDistance = view_dist.far;
  cam->heightAngle = field_of_view * taMath_float::rad_per_deg;
}

#endif // TA_QT3D
