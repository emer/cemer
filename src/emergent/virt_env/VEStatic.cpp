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

#include "VEStatic.h"
#include <VEWorld>
#include <VESpace>

#include <ode/ode.h>

TA_BASEFUNS_CTORS_DEFN(VEStatic);
SMARTREF_OF_CPP(VEStatic); // VEStaticRef


void VEStatic::Initialize() {
  geom_id = NULL;
  flags = EULER_ROT;
  relative = false;
  shape = BOX;
  long_axis = LONG_X;
  cur_shape = BOX;
  radius = .2f;
  length = 1.0f;
  box = 1.0f;
  plane_norm = NORM_Y;
  plane_height = 0.0f;
  plane_vis_size = 100.0f;
  set_color = true;
  color.Set(0.4f, 0.3f, .1f, 1.0f);     // brownish..
  full_colors = false;
  ambient_color.Set(0.2f, 0.2f, .2f);
  ambient_color.no_a = true;
  specular_color.Set(1.0f, 1.0f, 1.0f, 0.0f);
  emissive_color.Set(0.0f, 0.0f, 0.0f);
  emissive_color.no_a = true;
}

void VEStatic::Destroy() {
  CutLinks();
}

void VEStatic::CutLinks() {
  DestroyODE();
  inherited::CutLinks();
}

void VEStatic::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  if(shape == CAPSULE) {
    if(TestWarning(length < 2.0f * radius, "", "capsule length must be > 2 * radius!")) {
      length = 1.1f * 2.0f * radius;
    }
  }

  GetInitFromRel();

  // keep synchronized..
  if(HasStaticFlag(EULER_ROT)) {
    rot_quat = rot_euler;
    rot = rot_quat;
  }
  else {
    rot_quat = rot;
    rot_quat.ToEulerVec(rot_euler);
  }
  if(shape == CAPSULE || shape == CYLINDER) {
    if(long_axis == LONG_X) {
      rot_quat.RotateAxis(0.0f, 1.0f, 0.0f, -1.5708f);
    }
    else if(long_axis == LONG_Y) {
      rot_quat.RotateAxis(1.0f, 0.0f, 0.0f, -1.5708f);
    }
  }

  if(!geom_id) return;
  dWorldID wid = (dWorldID)GetWorldID();
  if(!wid) return;
  Init();               // always update ODE with any changes!
}

void VEStatic::InitRotFromCur() {
  // capsules and cylinders need to have extra rotation as they are always Z axis oriented!
  taQuaternion eff_quat = rot_quat;
  if(shape == CAPSULE || shape == CYLINDER) {
    if(long_axis == LONG_X) {
      eff_quat.RotateAxis(0.0f, 1.0f, 0.0f, 1.5708f);
    }
    else if(long_axis == LONG_Y) {
      eff_quat.RotateAxis(1.0f, 0.0f, 0.0f, 1.5708f);
    }
  }
  eff_quat.ToAxisAngle(rot);
  eff_quat.ToEulerVec(rot_euler);
}

VEWorld* VEStatic::GetWorld() {
  return GET_MY_OWNER(VEWorld);
}

void* VEStatic::GetWorldID() {
  VEWorld* wld = GetWorld();
  if(!wld) return NULL;
  return wld->world_id;
}

VESpace* VEStatic::GetSpace() {
  return GET_MY_OWNER(VESpace);
}

void* VEStatic::GetSpaceID() {
  VESpace* obj = GetSpace();
  if(!obj) return NULL;
  return obj->space_id;
}

bool VEStatic::CreateODE() {
  if(HasStaticFlag(VEStatic::OFF)) {
    DestroyODE();
    return true;
  }
  dWorldID wid = (dWorldID)GetWorldID();
  if(TestError(!wid, "CreateODE", "no valid world id -- cannot create static item!"))
    return false;

  Init_Shape();
  return true;
}

void VEStatic::DestroyODE() {
  if(geom_id) dGeomDestroy((dGeomID)geom_id);
  geom_id = NULL;
}

void VEStatic::GetInitFromRel() {
  if(!relative) {
    SetStaticFlag(INIT_WAS_ABS);
    return;
  }
  if(TestWarning(!rel_static, "GetInitFromRel", "relative is on but rel_static is NULL -- no static to be relative to")) {
    SetStaticFlag(INIT_WAS_ABS);
    return;
  }

  if(HasStaticFlag(INIT_WAS_ABS)) {
    // use current init vals relative to rel body to set rels to be consistent therewith
    rel_pos = pos - rel_static->pos;

    taQuaternion tquat;
    if(HasStaticFlag(EULER_ROT)) {
      tquat = rot_euler;
    }
    else {
      tquat = rot;
    }
    tquat.Normalize();
    taQuaternion rquat;
    if(rel_static->HasStaticFlag(EULER_ROT)) {
      rquat = rel_static->rot_euler;
    }
    else {
      rquat = rel_static->rot;
    }
    tquat /= rquat;             // take away the rel rotation, what's left is relative
    tquat.ToEulerVec(rel_euler);
    rel_rot = tquat;
    rel_quat = tquat;
    ClearStaticFlag(INIT_WAS_ABS); // no longer..
  }

  pos = rel_static->pos + rel_pos;

  // get rel_quat and keep sync'd
  if(rel_static->HasStaticFlag(EULER_ROT)) {
    rel_quat = rel_euler;
    rel_quat.ToAxisAngle(rel_rot);
  }
  else {
    rel_quat = rel_rot;
    rel_quat.ToEulerVec(rot_euler);
  }

  taQuaternion tquat;
  if(rel_static->HasStaticFlag(EULER_ROT)) {
    tquat = rel_static->rot_euler;
  }
  else {
    tquat = rel_static->rot;
  }
  tquat.Normalize();
  if(HasStaticFlag(EULER_ROT)) {
    tquat.RotateEuler(rel_euler.x, rel_euler.y, rel_euler.z);
  }
  else {
    tquat.RotateAxis(rel_rot.x, rel_rot.y, rel_rot.z, rel_rot.rot);
  }
  tquat.ToEulerVec(rot_euler);
  rot = tquat;
}

void VEStatic::Init() {
  VEWorld::last_to_set_ode = this;

  GetInitFromRel();

  if(HasStaticFlag(VEStatic::OFF)) {
    DestroyODE();
    return;
  }
  dWorldID wid = (dWorldID)GetWorldID();
  if(TestError(!wid, "Init", "no valid world id -- cannot create stuff!"))
    return;

  Init_Shape();
  Init_PosRot();
}

void VEStatic::Init_Shape() {
  dSpaceID sid = (dSpaceID)GetSpaceID();
  if(TestError(!sid, "CreateODE", "no valid space id -- cannot create static item geom!"))
    return;

  if(geom_id) dGeomDestroy((dGeomID)geom_id);
  geom_id = NULL;

  if(shape == NO_SHAPE) return;

  switch(shape) {
  case SPHERE:
    geom_id = dCreateSphere(sid, radius);
    break;
  case CAPSULE:
    geom_id = dCreateCapsule(sid, radius, MAX(0.00001f,length-2.0f*radius));
    break;
  case CYLINDER:
    geom_id = dCreateCylinder(sid, radius, length);
    break;
  case BOX:
    geom_id = dCreateBox(sid, box.x, box.y, box.z);
    break;
  case PLANE: {
    switch(plane_norm) {
    case NORM_X:
      geom_id = dCreatePlane(sid, 1.0f, 0.0f, 0.0f, plane_height);
      break;
    case NORM_Y:
      geom_id = dCreatePlane(sid, 0.0f, 1.0f, 0.0f, plane_height);
      break;
    case NORM_Z:
      geom_id = dCreatePlane(sid, 0.0f, 0.0f, 1.0f, plane_height);
      break;
    }
    break;
  }
  case NO_SHAPE:
    break;
  }

  if(TestError(!geom_id, "CreateODE", "could not create static item geom!"))
    return;

  cur_shape = shape;
  SetStaticFlagState(CUR_FM_FILE, HasStaticFlag(FM_FILE));

  dGeomSetData((dGeomID)geom_id, (void*)this);
}

void VEStatic::Init_PosRot() {
  dGeomID gid = (dGeomID)geom_id;

  if(shape != PLANE) {
    dGeomSetPosition(gid, pos.x, pos.y, pos.z);
    dQuaternion Q;
    rot_quat.ToODE(Q);
    dGeomSetQuaternion(gid, Q);
  }
}

void VEStatic::SnapPosToGrid(float grid_size) {
  pos.x = VEWorld::SnapVal(pos.x, grid_size);
  pos.y = VEWorld::SnapVal(pos.y, grid_size);
  pos.z = VEWorld::SnapVal(pos.z, grid_size);
  SigEmitUpdated(); // update displays..
}

void VEStatic::SaveCurAsPrv() {
  prv_quat = rot_quat;
  prv_pos = pos;
}

void VEStatic::UpdateCurFromRel() {
  if(!relative || !rel_static) return;

  taQuaternion rdif = rel_static->rot_quat / rel_static->prv_quat;
  rdif.RotateVec(rel_pos);

  // don't need to compound extra additions at all..

  GetInitFromRel();
  UpdateAfterEdit();
}

void VEStatic::Translate(float dx, float dy, float dz, bool abs_pos) {
  VESpace* obj = GetSpace();
  if(obj) {
    obj->SaveCurAsPrv();
  }
  if(relative && rel_static) {
    if(abs_pos) {
      rel_pos.SetXYZ(dx, dy, dz);
    }
    else {
      rel_pos.x += dx;
      rel_pos.y += dy;
      rel_pos.z += dz;
    }
  }
  else {
    if(abs_pos) {
      pos.SetXYZ(dx, dy, dz);
    }
    else {
      pos.x += dx;
      pos.y += dy;
      pos.z += dz;
    }
  }
  UpdateAfterEdit();
  if(obj) {
    obj->UpdateCurToRels();
  }
}

void VEStatic::Scale(float sx, float sy, float sz) {
  switch(shape) {
  case SPHERE:
    radius *= sx;
    break;
  case CAPSULE:
  case CYLINDER:
    if(long_axis == LONG_X) {
      length *= sx; if(sy > 0.0f) radius *= sy; else radius *= sx;
    }
    if(long_axis == LONG_Y) {
      if(sy > 0.0f) length *= sy; else length *= sx; radius *= sx;
    }
    if(long_axis == LONG_Z) {
      if(sz > 0.0f) length *= sz; else length *= sx; radius *= sx;
     }
    break;
  case BOX:
    box.x *= sx;
    if(sy > 0.0f) box.y *= sy; else box.y *= sx;
    if(sz > 0.0f) box.z *= sz; else box.z *= sx;
    break;
  case PLANE:
    break;
  case NO_SHAPE:
    break;
  }
  SigEmitUpdated(); // update displays..
}

void VEStatic::RotateAxis(float x_ax, float y_ax, float z_ax, float rt, bool abs_rot) {
  if(TestError((x_ax == 0.0f) && (y_ax == 0.0f) && (z_ax == 0.0f),
    "RotateAxis", "must specify a non-zero axis!"))
    return;

  VESpace* obj = GetSpace();
  if(obj) {
    obj->SaveCurAsPrv();
  }

  if(relative && rel_static) {
    taQuaternion tquat;
    if(abs_rot) {
      if(rel_static->HasStaticFlag(EULER_ROT)) {
        tquat.FromAxisAngle(x_ax, y_ax, z_ax, rt);
        tquat.Normalize();
        tquat.ToEulerVec(rel_euler);
      }
      else {
        rel_rot.SetXYZR(x_ax, y_ax, z_ax, rt);
      }
    }
    else {
      if(rel_static->HasStaticFlag(EULER_ROT)) {
        tquat.FromEulerVec(rel_euler);
      }
      else {
        tquat.FromAxisAngle(rel_rot);
      }
      tquat.RotateAxis(x_ax, y_ax, z_ax, rt);
      if(rel_static->HasStaticFlag(EULER_ROT)) {
        tquat.ToEulerVec(rel_euler);
      }
      else {
        tquat.ToAxisAngle(rel_rot);
      }
    }
  }
  else {
    if(abs_rot) {
      rot_quat.FromAxisAngle(x_ax, y_ax, z_ax, rt);
    }
    else {
      rot_quat.RotateAxis(x_ax, y_ax, z_ax, rt);
    }
    InitRotFromCur();
  }

  UpdateAfterEdit();
  if(obj) {
    obj->UpdateCurToRels();
  }
}

void VEStatic::RotateEuler(float euler_x, float euler_y, float euler_z, bool abs_rot) {
  VESpace* obj = GetSpace();
  if(obj) {
    obj->SaveCurAsPrv();
  }

  if(relative && rel_static) {
    taQuaternion tquat;
    if(abs_rot) {
      if(rel_static->HasStaticFlag(EULER_ROT)) {
        rel_euler.SetXYZ(euler_x, euler_y, euler_z);
      }
      else {
        tquat.FromEuler(euler_x, euler_y, euler_z);
        tquat.Normalize();
        tquat.ToAxisAngle(rel_rot);
      }
    }
    else {
      if(rel_static->HasStaticFlag(EULER_ROT)) {
        tquat.FromEulerVec(rel_euler);
      }
      else {
        tquat.FromAxisAngle(rel_rot);
      }
      tquat.RotateEuler(euler_x, euler_y, euler_z);
      if(rel_static->HasStaticFlag(EULER_ROT)) {
        tquat.ToEulerVec(rel_euler);
      }
      else {
        tquat.ToAxisAngle(rel_rot);
      }
    }
  }
  else {
    if(abs_rot) {
      rot_quat.FromEuler(euler_x, euler_y, euler_z);
    }
    else {
      rot_quat.RotateEuler(euler_x, euler_y, euler_z);
    }
    InitRotFromCur();
  }

  UpdateAfterEdit();
  if(obj) {
    obj->UpdateCurToRels();
  }
}

void VEStatic::CopyColorFrom(VEStatic* cpy_fm) {
  if(!cpy_fm) return;
  set_color = cpy_fm->set_color;
  color = cpy_fm->color;
  full_colors = cpy_fm->full_colors;
  ambient_color = cpy_fm->ambient_color;
  specular_color = cpy_fm->specular_color;
  emissive_color = cpy_fm->emissive_color;
  texture = cpy_fm->texture;
  UpdateAfterEdit();
}

