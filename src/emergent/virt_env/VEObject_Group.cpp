// Copyright 2015, Regents of the University of Colorado,
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

#include "VEObject_Group.h"

TA_BASEFUNS_CTORS_DEFN(VEObject_Group);

void VEObject_Group::Init() {
  FOREACH_ELEM_IN_GROUP(VEObject, ob, *this) {
    ob->Init();
  }
}

void VEObject_Group::Step_pre() {
  FOREACH_ELEM_IN_GROUP(VEObject, ob, *this) {
    ob->Step_pre();
  }
}

void VEObject_Group::CurToODE() {
  FOREACH_ELEM_IN_GROUP(VEObject, ob, *this) {
    ob->CurToODE();
  }
}

void VEObject_Group::CurFromODE(bool updt_disp) {
  FOREACH_ELEM_IN_GROUP(VEObject, ob, *this) {
    ob->CurFromODE(updt_disp);
  }
}

void VEObject_Group::DestroyODE() {
  FOREACH_ELEM_IN_GROUP(VEObject, ob, *this) {
    ob->DestroyODE();
  }
}

void VEObject_Group::CurToInit() {
  FOREACH_ELEM_IN_GROUP(VEObject, ob, *this) {
    ob->CurToInit();
  }
}

void VEObject_Group::SnapPosToGrid(float grid_size, bool init_pos) {
  FOREACH_ELEM_IN_GROUP(VEObject, ob, *this) {
    ob->SnapPosToGrid(grid_size, init_pos);
  }
}

void VEObject_Group::Translate(float dx, float dy, float dz, bool init) {
  FOREACH_ELEM_IN_GROUP(VEObject, ob, *this) {
    ob->Translate(dx, dy, dz, init);
  }
}

void VEObject_Group::Scale(float sx, float sy, float sz) {
  FOREACH_ELEM_IN_GROUP(VEObject, ob, *this) {
    ob->Scale(sx, sy, sz);
  }
}

void VEObject_Group::RotateAxis(float x_ax, float y_ax, float z_ax, float rot, bool init) {
  FOREACH_ELEM_IN_GROUP(VEObject, ob, *this) {
    ob->RotateAxis(x_ax, y_ax, z_ax, rot, init);
  }
}

void VEObject_Group::RotateEuler(float euler_x, float euler_y, float euler_z, bool init) {
  FOREACH_ELEM_IN_GROUP(VEObject, ob, *this) {
    ob->RotateEuler(euler_x, euler_y, euler_z, init);
  }
}

void VEObject_Group::CopyColorFrom(VEBody* cpy_fm) {
  FOREACH_ELEM_IN_GROUP(VEObject, ob, *this) {
    ob->CopyColorFrom(cpy_fm);
  }
}

