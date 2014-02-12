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

#include "VESpace.h"
#include <VEWorld>

#include <ode/ode.h>

TA_BASEFUNS_CTORS_DEFN(VESpace);



void VESpace::Initialize() {
  space_id = NULL;
  space_type = HASH_SPACE;
  cur_space_type = HASH_SPACE;
  hash_levels.min = -3;  hash_levels.max = 10;
}
void VESpace::Destroy() {
  CutLinks();
}

void VESpace::CutLinks() {
  static_els.CutLinks();
  DestroyODE();
  inherited::CutLinks();
}


VEWorld* VESpace::GetWorld() {
  return GET_MY_OWNER(VEWorld);
}

void* VESpace::GetWorldID() {
  VEWorld* wld = GetWorld();
  if(!wld) return NULL;
  return wld->world_id;
}

void* VESpace::GetWorldSpaceID() {
  VEWorld* wld = GetWorld();
  if(!wld) return NULL;
  return wld->space_id;
}

bool VESpace::CreateODE() {
  if(space_id && space_type == cur_space_type) return true;
  dSpaceID wsid = (dSpaceID)GetWorldSpaceID();
  if(TestError(!wsid, "CreateODE", "no valid world id -- cannot create space!"))
    return false;
  if(space_id)
    DestroyODE();
  switch(space_type) {
  case SIMPLE_SPACE:
    space_id = (dSpaceID)dSimpleSpaceCreate(wsid);
    break;
  case HASH_SPACE:
    space_id = (dSpaceID)dHashSpaceCreate(wsid);
    break;
  }
  if(TestError(!space_id, "CreateODE", "cannot create space!"))
    return false;
  cur_space_type = space_type;
  return true;
}

void VESpace::DestroyODE() {
  static_els.DestroyODE();
  if(space_id) dSpaceDestroy((dSpaceID)space_id);
  space_id = NULL;
}

void VESpace::Init() {
  if(!space_id || space_type != cur_space_type) CreateODE();
  if(!space_id) return;
  dSpaceID sid = (dSpaceID)space_id;
  if(space_type == HASH_SPACE) {
    dHashSpaceSetLevels(sid, hash_levels.min, hash_levels.max);
  }
  static_els.Init();
}

void VESpace::SnapPosToGrid(float grid_size) {
  static_els.SnapPosToGrid(grid_size);
}

void VESpace::UpdateCurToRels() {
  static_els.UpdateCurToRels();
}

void VESpace::SaveCurAsPrv() {
  static_els.SaveCurAsPrv();
}

void VESpace::Translate(float dx, float dy, float dz) {
  static_els.Translate(dx, dy, dz);
}

void VESpace::Scale(float sx, float sy, float sz) {
  static_els.Scale(sx, sy, sz);
}

void VESpace::RotateAxis(float x_ax, float y_ax, float z_ax, float rot) {
  static_els.RotateAxis(x_ax, y_ax, z_ax, rot);
}

void VESpace::RotateEuler(float euler_x, float euler_y, float euler_z) {
  static_els.RotateEuler(euler_x, euler_y, euler_z);
}

void VESpace::CopyColorFrom(VEStatic* cpy_fm) {
  static_els.CopyColorFrom(cpy_fm);
}

