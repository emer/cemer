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

#include "VEObject.h"
#include <VEWorld>


void VEObject::Initialize() {
  space_id = NULL;
  space_type = SIMPLE_SPACE;
  cur_space_type = SIMPLE_SPACE;
  hash_levels.min = -3;  hash_levels.max = 10;
}
void VEObject::Destroy() {
  CutLinks();
}

void VEObject::CutLinks() {
  bodies.CutLinks();
  joints.CutLinks();
  DestroyODE();
  inherited::CutLinks();
}


VEWorld* VEObject::GetWorld() {
  return GET_MY_OWNER(VEWorld);
}

void* VEObject::GetWorldID() {
  VEWorld* wld = GetWorld();
  if(!wld) return NULL;
  return wld->world_id;
}

void* VEObject::GetWorldSpaceID() {
  VEWorld* wld = GetWorld();
  if(!wld) return NULL;
  return wld->space_id;
}

bool VEObject::CreateODE() {
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

void VEObject::DestroyODE() {
  bodies.DestroyODE();  // bodies first!
  joints.DestroyODE();
  if(space_id) dSpaceDestroy((dSpaceID)space_id);
  space_id = NULL;
}

void VEObject::Init() {
  if(!space_id || space_type != cur_space_type) CreateODE();
  if(!space_id) return;
  dSpaceID sid = (dSpaceID)space_id;
  if(space_type == HASH_SPACE) {
    dHashSpaceSetLevels(sid, hash_levels.min, hash_levels.max);
  }
  bodies.Init();        // bodies first!
  joints.Init();
}

void VEObject::CurToODE() {
  bodies.CurToODE();
}

void VEObject::CurFromODE(bool updt_disp) {
  bodies.CurFromODE(updt_disp);       // bodies first!
  joints.CurFromODE(updt_disp);
}

void VEObject::CurToInit() {
  bodies.CurToInit();
}

void VEObject::SnapPosToGrid(float grid_size, bool init_pos) {
  bodies.SnapPosToGrid(grid_size, init_pos);
}

void VEObject::Translate(float dx, float dy, float dz, bool init) {
  bodies.Translate(dx, dy, dz, init);
}

void VEObject::Scale(float sx, float sy, float sz) {
  bodies.Scale(sx, sy, sz);
}

void VEObject::RotateAxis(float x_ax, float y_ax, float z_ax, float rot, bool init) {
  bodies.RotateAxis(x_ax, y_ax, z_ax, rot, init);
}

void VEObject::RotateEuler(float euler_x, float euler_y, float euler_z, bool init) {
  bodies.RotateEuler(euler_x, euler_y, euler_z, init);
}

void VEObject::CopyColorFrom(VEBody* cpy_fm) {
  bodies.CopyColorFrom(cpy_fm);
}


