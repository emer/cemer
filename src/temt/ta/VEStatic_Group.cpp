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

#include "VEStatic_Group.h"

void VEStatic_Group::Init() {
  FOREACH_ELEM_IN_GROUP(VEStatic, ob, *this) {
    ob->Init();
  }
}

void VEStatic_Group::DestroyODE() {
  FOREACH_ELEM_IN_GROUP(VEStatic, ob, *this) {
    ob->DestroyODE();
  }
}

void VEStatic_Group::SnapPosToGrid(float grid_size) {
  FOREACH_ELEM_IN_GROUP(VEStatic, ob, *this) {
    ob->SnapPosToGrid(grid_size);
  }
}

void VEStatic_Group::Translate(float dx, float dy, float dz) {
  FOREACH_ELEM_IN_GROUP(VEStatic, ob, *this) {
    ob->Translate(dx, dy, dz);
  }
}

void VEStatic_Group::Scale(float sx, float sy, float sz) {
  FOREACH_ELEM_IN_GROUP(VEStatic, ob, *this) {
    ob->Scale(sx, sy, sz);
  }
}

void VEStatic_Group::RotateAxis(float x_ax, float y_ax, float z_ax, float rot) {
  FOREACH_ELEM_IN_GROUP(VEStatic, ob, *this) {
    ob->RotateAxis(x_ax, y_ax, z_ax, rot);
  }
}

void VEStatic_Group::RotateEuler(float euler_x, float euler_y, float euler_z) {
  FOREACH_ELEM_IN_GROUP(VEStatic, ob, *this) {
    ob->RotateEuler(euler_x, euler_y, euler_z);
  }
}

void VEStatic_Group::CopyColorFrom(VEStatic* cpy_fm) {
  FOREACH_ELEM_IN_GROUP(VEStatic, ob, *this) {
    ob->CopyColorFrom(cpy_fm);
  }
}
