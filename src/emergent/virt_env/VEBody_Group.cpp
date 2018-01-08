// Copyright 2013-2018, Regents of the University of Colorado,
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

#include "VEBody_Group.h"
#include <VEObject>

TA_BASEFUNS_CTORS_DEFN(VEBody_Group);

void VEBody_Group::Init() {
  FOREACH_ELEM_IN_GROUP(VEBody, ob, *this) {
    ob->Init();
  }
}

void VEBody_Group::Step_pre() {
  FOREACH_ELEM_IN_GROUP(VEBody, ob, *this) {
    ob->Step_pre();
  }
}

void VEBody_Group::CurToODE() {
  FOREACH_ELEM_IN_GROUP(VEBody, ob, *this) {
    ob->CurToODE();
  }
}

void VEBody_Group::CurFromODE(bool updt_disp) {
  FOREACH_ELEM_IN_GROUP(VEBody, ob, *this) {
    ob->CurFromODE(updt_disp);
  }
}

void VEBody_Group::DestroyODE() {
  FOREACH_ELEM_IN_GROUP(VEBody, ob, *this) {
    ob->DestroyODE();
  }
}

void VEBody_Group::CurToInit() {
  FOREACH_ELEM_IN_GROUP(VEBody, ob, *this) {
    ob->CurToInit();
  }
}

void VEBody_Group::UpdateInitToRels() {
  FOREACH_ELEM_IN_GROUP(VEBody, ob, *this) {
    ob->GetInitFromRel();
  }
}

void VEBody_Group::UpdateCurToRels() {
  FOREACH_ELEM_IN_GROUP(VEBody, ob, *this) {
    ob->UpdateCurFromRel();
  }
}

void VEBody_Group::SaveCurAsPrv() {
  FOREACH_ELEM_IN_GROUP(VEBody, ob, *this) {
    ob->SaveCurAsPrv();
  }
}

void VEBody_Group::SnapPosToGrid(float grid_size, bool init_pos) {
  FOREACH_ELEM_IN_GROUP(VEBody, ob, *this) {
    ob->SnapPosToGrid(grid_size, init_pos);
  }
}

void VEBody_Group::Translate(float dx, float dy, float dz, bool init) {
  VEObject* obj = GET_MY_OWNER(VEObject);
  FOREACH_ELEM_IN_GROUP(VEBody, ob, *this) {
    if(!(obj && obj->auto_updt_rels && ob->init_rel && ob->rel_body)) { // rels will be dealt with in updt
      ob->Translate(dx, dy, dz, init);
    }
  }
}

void VEBody_Group::Scale(float sx, float sy, float sz) {
  FOREACH_ELEM_IN_GROUP(VEBody, ob, *this) {
    ob->Scale(sx, sy, sz);
  }
}

void VEBody_Group::RotateAxis(float x_ax, float y_ax, float z_ax, float rot, bool init) {
  VEObject* obj = GET_MY_OWNER(VEObject);
  FOREACH_ELEM_IN_GROUP(VEBody, ob, *this) {
    if(!(obj && obj->auto_updt_rels && ob->init_rel && ob->rel_body)) { // rels will be dealt with in updt
      ob->RotateAxis(x_ax, y_ax, z_ax, rot, init);
    }
  }
}

void VEBody_Group::RotateEuler(float euler_x, float euler_y, float euler_z, bool init) {
  VEObject* obj = GET_MY_OWNER(VEObject);
  FOREACH_ELEM_IN_GROUP(VEBody, ob, *this) {
    if(!(obj && obj->auto_updt_rels && ob->init_rel && ob->rel_body)) { // rels will be dealt with in updt
      ob->RotateEuler(euler_x, euler_y, euler_z, init);
    }
  }
}

void VEBody_Group::CopyColorFrom(VEBody* cpy_fm) {
  FOREACH_ELEM_IN_GROUP(VEBody, ob, *this) {
    ob->CopyColorFrom(cpy_fm);
  }
}

