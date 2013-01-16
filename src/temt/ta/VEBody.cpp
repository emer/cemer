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

#include "VEBody.h"
#include <VEWorld>
#include <VEObject>

#include <ode/ode.h>

#include <taMisc>

void ODEDamping::Initialize() {
  on = false;
  lin = 0.0f;
  lin_thr = 0.0f;
  ang = 0.0f;
  ang_thr = 0.0f;
  ang_speed = 0.0f;
}

void ODEFiniteRotation::Initialize() {
  on = false;
}

void VEBody::Initialize() {
  body_id = NULL;
  geom_id = NULL;
  flags = (BodyFlags)(GRAVITY_ON | EULER_ROT);
  shape = CAPSULE;
  cur_shape = NO_SHAPE;
  cur_long_axis = (LongAxis)0;
  mass = 1.0f;
  radius = .2f;
  length = 1.0f;
  long_axis = LONG_X;
  box = 1.0f;
  set_color = true;
  color.Set(0.2f, 0.2f, .5f, .5f);      // transparent blue.. why not..
  full_colors = false;
  ambient_color.Set(0.2f, 0.2f, .2f);
  ambient_color.no_a = true;
  specular_color.Set(1.0f, 1.0f, 1.0f, 0.0f);
  emissive_color.Set(0.0f, 0.0f, 0.0f);
  emissive_color.no_a = true;
  fixed_joint_id = NULL;
}

void VEBody::Destroy() {
  CutLinks();
}

void VEBody::CutLinks() {
  DestroyODE();
  inherited::CutLinks();
}

void VEBody::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  if(shape == CAPSULE) {
    if(TestWarning(length < 2.0f * radius, "", "capsule length must be > 2 * radius!")) {
      length = 1.1f * 2.0f * radius;
    }
  }
  // keep synchronized..
  if(HasBodyFlag(EULER_ROT)) {
    init_quat = init_euler;
    init_quat.Normalize();
    init_quat.ToEulerVec(init_euler);
    init_rot = init_quat;
  }
  else {
    init_quat = init_rot;
    init_quat.Normalize();
    init_rot = init_quat;
    init_quat.ToEulerVec(init_euler);
  }

  if(!taMisc::is_loading) {
    if(shape == CAPSULE || shape == CYLINDER) {
      if(long_axis != cur_long_axis) {
        // first, undo old setting
        if(cur_long_axis == LONG_X) {
          cur_quat.RotateAxis(0.0f, 1.0f, 0.0f, 1.5708f);
        }
        else if(cur_long_axis == LONG_Y) {
          cur_quat.RotateAxis(1.0f, 0.0f, 0.0f, 1.5708f);
        }
        // next, set new one
        if(long_axis == LONG_X) {
          cur_quat.RotateAxis(0.0f, 1.0f, 0.0f, -1.5708f);
        }
        else if(long_axis == LONG_Y) {
          cur_quat.RotateAxis(1.0f, 0.0f, 0.0f, -1.5708f);
        }
      }
    }
  }
  cur_long_axis = long_axis;

  CurToODE();           // always update ODE with any changes!
}

VEWorld* VEBody::GetWorld() {
  return GET_MY_OWNER(VEWorld);
}

void* VEBody::GetWorldID() {
  VEWorld* wld = GetWorld();
  if(!wld) return NULL;
  return wld->world_id;
}

VEObject* VEBody::GetObject() {
  return GET_MY_OWNER(VEObject);
}

void* VEBody::GetObjSpaceID() {
  VEObject* obj = GetObject();
  if(!obj) return NULL;
  return obj->space_id;
}

bool VEBody::CreateODE() {
  if(HasBodyFlag(VEBody::OFF)) {
    DestroyODE();
    return true;
  }

  if(body_id)
    return true;

  dWorldID wid = (dWorldID)GetWorldID();
  if(TestError(!wid, "CreateODE", "no valid world id -- cannot create body!"))
    return false;

  if(!body_id)
    body_id = (dBodyID)dBodyCreate(wid);
  if(TestError(!body_id, "CreateODE", "could not create body!"))
    return false;
  return true;
}

void VEBody::DestroyODE() {
  if(geom_id) dGeomDestroy((dGeomID)geom_id);
  geom_id = NULL;
  if(body_id) dBodyDestroy((dBodyID)body_id);
  body_id = NULL;
  if(fixed_joint_id) dJointDestroy((dJointID)fixed_joint_id);
  fixed_joint_id = NULL;
}

void VEBody::Init() {
  VEWorld::last_to_set_ode = this;

  if(HasBodyFlag(VEBody::OFF)) {
    DestroyODE();
    return;
  }

  if(!body_id) CreateODE();
  if(!body_id) return;
  dWorldID wid = (dWorldID)GetWorldID();
  if(TestError(!wid, "Init", "no valid world id -- cannot create stuff!"))
    return;

  Init_Shape();
  Init_Pos();
  Init_Rotation();
  Init_Velocity();
  Init_Mass();
  Init_FiniteRotation();
  Init_Gravity();
  Init_Damping();
  
  DataItemUpdated(); // update displays..
}

void VEBody::Init_Shape() {
  dSpaceID sid = (dSpaceID)GetObjSpaceID();
  if(TestError(!sid, "CreateODE", "no valid space id -- cannot create body geom!"))
    return;

  if(geom_id) dGeomDestroy((dGeomID)geom_id);
  geom_id = NULL;

  if(shape == NO_SHAPE || HasBodyFlag(NO_COLLIDE)) {
    cur_shape = shape;
    SetBodyFlagState(CUR_FM_FILE, HasBodyFlag(FM_FILE));
    return;
  }

  switch(shape) {
  case SPHERE:
    geom_id = dCreateSphere(sid, radius);
    break;
  case CAPSULE:
    geom_id = dCreateCapsule(sid, radius, MAX(0.00001f,(length-(2.0f*radius))));
    break;
  case CYLINDER:
    geom_id = dCreateCylinder(sid, radius, length);
    break;
  case BOX:
    geom_id = dCreateBox(sid, box.x, box.y, box.z);
    break;
  case NO_SHAPE:                // compiler food
    break;
  }

  if(TestError(!geom_id, "CreateODE", "could not create body geom!"))
    return;

  cur_shape = shape;
  SetBodyFlagState(CUR_FM_FILE, HasBodyFlag(FM_FILE));

  dGeomSetBody((dGeomID)geom_id, (dBodyID)body_id);
  dGeomSetData((dGeomID)geom_id, (void*)this);
}

void VEBody::Init_Pos() {
  dBodyID bid = (dBodyID)body_id;
  cur_pos = init_pos;
  dBodySetPosition(bid, init_pos.x, init_pos.y, init_pos.z);

  if(HasBodyFlag(FIXED)) {
    if(!fixed_joint_id) {
      dWorldID wid = (dWorldID)GetWorldID();
      if(TestError(!wid, "Init", "no valid world id -- cannot create stuff!"))
        return;
      fixed_joint_id = dJointCreateFixed(wid, 0);
    }
    dJointAttach((dJointID)fixed_joint_id, bid, 0);     // 0 = attach to static object
  }
  else if(fixed_joint_id) {
    dJointDestroy((dJointID)fixed_joint_id);
    fixed_joint_id = NULL;
  }
}

void VEBody::Init_Rotation() {
  dBodyID bid = (dBodyID)body_id;

  // capsules and cylinders need to have extra rotation as they are always Z axis oriented!
  cur_quat = init_quat;
  if(shape == CAPSULE || shape == CYLINDER) {
    if(long_axis == LONG_X) {
      cur_quat.RotateAxis(0.0f, 1.0f, 0.0f, -1.5708f);
    }
    else if(long_axis == LONG_Y) {
      cur_quat.RotateAxis(1.0f, 0.0f, 0.0f, -1.5708f);
    }
  }

  cur_quat.ToAxisAngle(cur_rot);
  cur_quat.ToEulerVec(cur_euler);

  dQuaternion Q;
  cur_quat.ToODE(Q);
  dBodySetQuaternion(bid, Q);
}

void VEBody::InitRotFromCur() {
  init_quat = cur_quat;

  // capsules and cylinders need to have extra rotation as they are always Z axis oriented!
  if(shape == CAPSULE || shape == CYLINDER) {
    if(long_axis == LONG_X) {
      init_quat.RotateAxis(0.0f, 1.0f, 0.0f, 1.5708f);
    }
    else if(long_axis == LONG_Y) {
      init_quat.RotateAxis(1.0f, 0.0f, 0.0f, 1.5708f);
    }
  }
  init_quat.ToAxisAngle(init_rot);
  init_quat.ToEulerVec(init_euler);
}

void VEBody::Init_Velocity() {
  dBodyID bid = (dBodyID)body_id;
  cur_lin_vel = init_lin_vel;
  cur_ang_vel = init_ang_vel;

  if(HasBodyFlag(FIXED)) {
    if(!fixed_joint_id) CreateODE();
    dBodySetLinearVel(bid, 0.0f, 0.0f, 0.0f);
    dBodySetAngularVel(bid,  0.0f, 0.0f, 0.0f);
    dJointSetFixed((dJointID)fixed_joint_id);
  }
  else {
    if(fixed_joint_id) CreateODE(); // will destroy joint
    dBodySetLinearVel(bid, init_lin_vel.x, init_lin_vel.y, init_lin_vel.z);
    dBodySetAngularVel(bid, init_ang_vel.x, init_ang_vel.y, init_ang_vel.z);
  }
}

void VEBody::Init_Mass() {
  if(HasBodyFlag(VEBody::OFF)) {
    return;
  }
  if(!body_id) CreateODE();
  if(!body_id) return;
  dBodyID bid = (dBodyID)body_id;
  switch(shape) {
  case SPHERE:
    dMassSetSphereTotal(&mass_ode, mass, radius);
    break;
  case CAPSULE:
    dMassSetCapsuleTotal(&mass_ode, mass, long_axis, radius, MAX(0.00001f,length-2.0f*radius));
    break;
  case CYLINDER:
    dMassSetCylinderTotal(&mass_ode, mass, long_axis, radius, length);
    break;
  case BOX:
    dMassSetBoxTotal(&mass_ode, mass, box.x, box.y, box.z);
    break;
  case NO_SHAPE:
    break;
  }
  dBodySetMass(bid, &mass_ode);
}

void VEBody::Init_FiniteRotation() {
  if (finite_rotation.on) {
    SetFiniteRotationMode(1);
    if (finite_rotation.axis.x != 0.0 ||
        finite_rotation.axis.y != 0.0 ||
        finite_rotation.axis.z != 0.0)
      SetFiniteRotationAxis(finite_rotation.axis.x, finite_rotation.axis.y, finite_rotation.axis.z);
  }
  else
    SetFiniteRotationMode(0);
}

void VEBody::Init_Gravity() {
  if(HasBodyFlag(GRAVITY_ON))
    SetGravityMode(1);
  else
    SetGravityMode(0);
}

void VEBody::Init_Damping() {
  if (damp.on) {
    SetLinearDamping(damp.lin);
    SetLinearDampingThreshold(damp.lin_thr);
    SetAngularDamping(damp.ang);
    SetAngularDampingThreshold(damp.ang_thr);
    SetMaxAngularSpeed(damp.ang_speed);
  }
}

//////////////////////

void VEBody::CurToODE() {
  if(!body_id) return;
  dWorldID wid = (dWorldID)GetWorldID();
  if(!wid) return;

  VEWorld::last_to_set_ode = this;

  CurToODE_Pos();
  CurToODE_Rotation();
  CurToODE_Velocity();
}

void VEBody::CurToODE_Pos() {
  dBodyID bid = (dBodyID)body_id;
  dBodySetPosition(bid, cur_pos.x, cur_pos.y, cur_pos.z);
}

void VEBody::CurToODE_Rotation() {
  dBodyID bid = (dBodyID)body_id;
  dQuaternion Q;
  cur_quat.ToODE(Q);
  dBodySetQuaternion(bid, Q);
}

void VEBody::CurToODE_Velocity() {
  dBodyID bid = (dBodyID)body_id;

  if(HasBodyFlag(FIXED)) {
    dBodySetLinearVel(bid, 0.0f, 0.0f, 0.0f);
    dBodySetAngularVel(bid,  0.0f, 0.0f, 0.0f);
  }
  else {
    dBodySetLinearVel(bid, cur_lin_vel.x, cur_lin_vel.y, cur_lin_vel.z);
    dBodySetAngularVel(bid, cur_ang_vel.x, cur_ang_vel.y, cur_ang_vel.z);
  }
}

///////////////

void VEBody::CurFromODE(bool updt_disp) {
  if(HasBodyFlag(VEBody::OFF)) {
    return;
  }
  if(!body_id) CreateODE();
  if(!body_id) return;
  dBodyID bid = (dBodyID)body_id;
  const dReal* opos = dBodyGetPosition(bid);
  cur_pos.x = opos[0]; cur_pos.y = opos[1]; cur_pos.z = opos[2];

  const dReal* quat = dBodyGetQuaternion(bid);
  cur_quat.FromODE(quat);
  UpdateCurRotFmQuat();

  const dReal* olv = dBodyGetLinearVel(bid);
  cur_lin_vel.x = olv[0]; cur_lin_vel.y = olv[1]; cur_lin_vel.z = olv[2];
  const dReal* oav = dBodyGetAngularVel(bid);
  cur_ang_vel.x = oav[0]; cur_ang_vel.y = oav[1]; cur_ang_vel.z = oav[2];

  if(updt_disp)
    DataItemUpdated(); // update displays..
}

void VEBody::UpdateCurRotFmQuat() {
  cur_quat.ToAxisAngle(cur_rot);
  cur_quat.ToEulerVec(cur_euler);
}

void VEBody::Translate(float dx, float dy, float dz, bool init) {
  if(init) {
    init_pos.x += dx;
    init_pos.y += dy;
    init_pos.z += dz;
  }
  else {
    cur_pos.x += dx;
    cur_pos.y += dy;
    cur_pos.z += dz;
  }
  UpdateAfterEdit();            // calls CurToODE and updates display
}

void VEBody::Scale(float sx, float sy, float sz) {
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
  case NO_SHAPE:
    break;
  }
  UpdateAfterEdit();
}

void VEBody::RotateAxis(float x_ax, float y_ax, float z_ax, float rot, bool init) {
  if(TestError((x_ax == 0.0f) && (y_ax == 0.0f) && (z_ax == 0.0f),
    "RotateAxis", "must specify a non-zero axis!"))
    return;

  if(init) {
    init_quat.RotateAxis(x_ax, y_ax, z_ax, rot);
    init_quat.ToAxisAngle(init_rot);
    init_quat.ToEulerVec(init_euler);
  }
  else {
    cur_quat.RotateAxis(x_ax, y_ax, z_ax, rot);
    cur_quat.ToAxisAngle(cur_rot);
    cur_quat.ToEulerVec(cur_euler);
  }
  UpdateAfterEdit();            // calls CurToODE and updates display
}

void VEBody::RotateEuler(float euler_x, float euler_y, float euler_z, bool init) {
  if(init) {
    init_quat.RotateEuler(euler_x, euler_y, euler_z);
    init_quat.ToAxisAngle(init_rot);
    init_quat.ToEulerVec(init_euler);
  }
  else {
    cur_quat.RotateEuler(euler_x, euler_y, euler_z);
    cur_quat.ToAxisAngle(cur_rot);
    cur_quat.ToEulerVec(cur_euler);
  }
  UpdateAfterEdit();            // calls CurToODE and updates display
}

void VEBody::CopyColorFrom(VEBody* cpy_fm) {
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

void VEBody::AddForce(float fx, float fy, float fz, bool torque, bool rel) {
  if(!body_id) return;
  dBodyID bid = (dBodyID)body_id;

  if(torque) {
    if(rel)
      dBodyAddRelTorque(bid, fx, fy, fz);
    else
      dBodyAddTorque(bid, fx, fy, fz);
  }
  else {
    if(rel)
      dBodyAddRelForce(bid, fx, fy, fz);
    else
      dBodyAddForce(bid, fx, fy, fz);
  }
}

void VEBody::AddForceAtPos(float fx, float fy, float fz, float px, float py, float pz,
                           bool rel_force, bool rel_pos) {
  if(!body_id) return;
  dBodyID bid = (dBodyID)body_id;

  if(rel_pos) {
    if(rel_force)
      dBodyAddRelForceAtRelPos(bid, fx, fy, fz, px, py, pz);
    else
      dBodyAddForceAtRelPos(bid, fx, fy, fz, px, py, pz);
  }
  else {
    if(rel_force)
      dBodyAddRelForceAtPos(bid, fx, fy, fz, px, py, pz);
    else
      dBodyAddForceAtPos(bid, fx, fy, fz, px, py, pz);
  }
}

void VEBody::CurToInit() {
  init_pos = cur_pos;
  init_lin_vel = cur_lin_vel;
  init_ang_vel = cur_ang_vel;
  InitRotFromCur();
}

void VEBody::SnapPosToGrid(float grid_size, bool do_init_pos) {
  if(do_init_pos) {
    init_pos.x = VEWorld::SnapVal(init_pos.x, grid_size);
    init_pos.y = VEWorld::SnapVal(init_pos.y, grid_size);
    init_pos.z = VEWorld::SnapVal(init_pos.z, grid_size);
  }
  else {
    cur_pos.x = VEWorld::SnapVal(cur_pos.x, grid_size);
    cur_pos.y = VEWorld::SnapVal(cur_pos.y, grid_size);
    cur_pos.z = VEWorld::SnapVal(cur_pos.z, grid_size);
  }
}


//////////////////////////////
//    Set Damping

void VEBody::SetLinearDamping(float ldamp) {
  if(!body_id) return;
  dBodyID bid = (dBodyID)body_id;
  dBodySetLinearDamping(bid, ldamp);
}

void VEBody::SetAngularDamping(float adamp) {
  if(!body_id) return;
  dBodyID bid = (dBodyID)body_id;
  dBodySetAngularDamping(bid, adamp);
}

void VEBody::SetLinearDampingThreshold(float ldampthresh) {
  if(!body_id) return;
  dBodyID bid = (dBodyID)body_id;
  dBodySetLinearDampingThreshold(bid, ldampthresh);
}

void VEBody::SetAngularDampingThreshold(float adampthresh) {
  if(!body_id) return;
  dBodyID bid = (dBodyID)body_id;
  dBodySetAngularDampingThreshold(bid, adampthresh);
}

void VEBody::SetMaxAngularSpeed(float maxaspeed) {
  if(!body_id) return;
  dBodyID bid = (dBodyID)body_id;
  dBodySetMaxAngularSpeed(bid, maxaspeed);
}

//////////////////////////////
//      Finite Rotation Mode

void VEBody::SetFiniteRotationMode(int rotmode) {
  if(!body_id) return;
  dBodyID bid = (dBodyID)body_id;
  dBodySetFiniteRotationMode(bid, rotmode);
}

void VEBody::SetFiniteRotationAxis(float xr, float yr, float zr) {
  if(!body_id) return;
  dBodyID bid = (dBodyID)body_id;
  dBodySetFiniteRotationAxis(bid, xr, yr, zr);
}

//////////////////////////////
//      Gravity mode

void VEBody::SetGravityMode(int mode) {
  if(!body_id) return;
  dBodyID bid = (dBodyID)body_id;
  dBodySetGravityMode(bid, mode);
}
