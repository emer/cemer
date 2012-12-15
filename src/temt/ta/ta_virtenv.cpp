// Copyright, 1995-2007, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of The Emergent Toolkit
//
//   Emergent is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   Emergent is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.

#include "ta_virtenv.h"
#include "ta_math.h"

#include <Inventor/SbLinear.h>

// all objs should set this pointer while setting vals, for better err msgs
static taBase* VE_last_ve_set_vals_to_ode = NULL;

////////////////////////////////////////////////
//      parameters

void ODEIntParams::Initialize() {
  erp = 0.2f;
  cfm = 1.0e-5f;
}

void ODEIntParams::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  if(erp < .01f) erp = .01f;
  if(erp > .99f) erp = .99f;
  if(cfm < 1.0e-9f) cfm = 1.0e-9f;
  if(cfm > 1.0f) cfm = 1.0f;
}

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


////////////////////////////////////////////////
//              surfaces and textures

void VESurface::Initialize() {
  friction = 1.0e22f;
  bounce = 0.0f;
  bounce_vel = .01f;
}

void VETexture::Initialize() {
  mode = MODULATE;
  wrap_horiz = REPEAT;
  wrap_vert = REPEAT;
  offset = 0.0f;
  scale = 1.0f;
  rot = 0.0f;
  center = 0.0f;
  blend_color.r = 0.0f;
  blend_color.g = 0.0f;
  blend_color.b = 0.0f;
  blend_color.no_a = true;
  idx = -1;
}

// in ta_virtenv_qtso.cpp: void VETexture::SetTexture(SoTexture2* sotx)


bool VETexture::NeedsTransform() {
  if(offset == 0.0f && scale == 1.0f && rot == 0.0f) return false;
  return true;
}

// in ta_virtenv_qtso.cpp:  void VETexture::SetTransform(SoTexture2Transform* sotx)


////////////////////////////////////////////////
//              bodies (rigid object elements)

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
  VE_last_ve_set_vals_to_ode = this;

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

  DataChanged(DCR_ITEM_UPDATED); // update displays..
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

  VE_last_ve_set_vals_to_ode = this;

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
    DataChanged(DCR_ITEM_UPDATED); // update displays..
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
      dBodyAddRelForceAtPos(bid, fx, fy, fz, px, py, pz);
  }
}

void VEBody::CurToInit() {
  init_pos = cur_pos;
  init_lin_vel = cur_lin_vel;
  init_ang_vel = cur_ang_vel;
  InitRotFromCur();
}

static float ve_snap_val(float val, float grid_size) {
  int ival = (int)((val / grid_size) + .5f);
  return (float)ival * grid_size;
}

void VEBody::SnapPosToGrid(float grid_size, bool do_init_pos) {
  if(do_init_pos) {
    init_pos.x = ve_snap_val(init_pos.x, grid_size);
    init_pos.y = ve_snap_val(init_pos.y, grid_size);
    init_pos.z = ve_snap_val(init_pos.z, grid_size);
  }
  else {
    cur_pos.x = ve_snap_val(cur_pos.x, grid_size);
    cur_pos.y = ve_snap_val(cur_pos.y, grid_size);
    cur_pos.z = ve_snap_val(cur_pos.z, grid_size);
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

/////////////////////////////////////////////
//              Group

void VEBody_Group::Init() {
  FOREACH_ELEM_IN_GROUP(VEBody, ob, *this) {
    ob->Init();
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

void VEBody_Group::SnapPosToGrid(float grid_size, bool init_pos) {
  FOREACH_ELEM_IN_GROUP(VEBody, ob, *this) {
    ob->SnapPosToGrid(grid_size, init_pos);
  }
}

void VEBody_Group::Translate(float dx, float dy, float dz, bool init) {
  FOREACH_ELEM_IN_GROUP(VEBody, ob, *this) {
    ob->Translate(dx, dy, dz, init);
  }
}

void VEBody_Group::Scale(float sx, float sy, float sz) {
  FOREACH_ELEM_IN_GROUP(VEBody, ob, *this) {
    ob->Scale(sx, sy, sz);
  }
}

void VEBody_Group::RotateAxis(float x_ax, float y_ax, float z_ax, float rot, bool init) {
  FOREACH_ELEM_IN_GROUP(VEBody, ob, *this) {
    ob->RotateAxis(x_ax, y_ax, z_ax, rot, init);
  }
}

void VEBody_Group::RotateEuler(float euler_x, float euler_y, float euler_z, bool init) {
  FOREACH_ELEM_IN_GROUP(VEBody, ob, *this) {
    ob->RotateEuler(euler_x, euler_y, euler_z, init);
  }
}

void VEBody_Group::CopyColorFrom(VEBody* cpy_fm) {
  FOREACH_ELEM_IN_GROUP(VEBody, ob, *this) {
    ob->CopyColorFrom(cpy_fm);
  }
}


/////////////////////////////////////////////
//              Camera and Lights

void VELightParams::Initialize() {
  on = true;
  intensity = 1.0f;
  color.no_a = true;
  color.r = 1.0f; color.g = 1.0f; color.b = 1.0f;
}

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

void VECamera::Init() {
  inherited::Init();
  SbVec3f dn(0.0f, 0.0f, -1.0f);
  SbRotation sbrot(SbVec3f(cur_rot.x, cur_rot.y, cur_rot.z), cur_rot.rot);
  sbrot.multVec(dn, dn);
  dir_norm.x = dn[0]; dir_norm.y = dn[1]; dir_norm.z = dn[2];
}

void VECamera::CurFromODE(bool updt_disp) {
  inherited::CurFromODE(updt_disp);
  SbVec3f dn(0.0f, 0.0f, -1.0f);
  SbRotation sbrot(SbVec3f(cur_rot.x, cur_rot.y, cur_rot.z), cur_rot.rot);
  sbrot.multVec(dn, dn);
  dir_norm.x = dn[0]; dir_norm.y = dn[1]; dir_norm.z = dn[2];
}

// in ta_virtenv_qtso.cpp:  void VECamera::ConfigCamera(SoPerspectiveCamera* cam)

///////////////////////////////////////////////////
//              Lights

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

///////////////////////////////////////////////////
//              VEObjCarousel

void VEObjCarousel::Initialize() {
  cur_obj_no = -1;
  SetBodyFlag(FM_FILE);
  obj_switch = NULL;
}

// Destroy, LoadObjs are in in ta_virtenv_qtso

bool VEObjCarousel::ViewObjNo(int obj_no) {
  if(TestError(!(bool)obj_table, "ViewObjNo", "no obj_table data table set -- must set this first -- falling back on basic body render for now!"))
    return false;
  if(TestError(obj_no >= obj_table->rows, "ViewObjNo", "obj_no out of range -- only:",
               String(obj_table->rows), "rows in the obj_table data table"))
    return false;
  cur_obj_no = obj_no;
  obj_fname = obj_table->GetVal("FileName", cur_obj_no);
  DataChanged(DCR_ITEM_UPDATED); // update displays..
  return true;
}

bool VEObjCarousel::ViewObjName(const String& obj_nm) {
  if(TestError(!(bool)obj_table, "ViewObjName", "no obj_table data table set -- must set this first -- falling back on basic body render for now!"))
    return false;
  int obj_no = obj_table->FindVal(obj_nm, "FileName");
  if(TestError(obj_no < 0, "ViewObjName", "object file named:", obj_nm,
               "not found in the obj_table data table"))
    return false;
  cur_obj_no = obj_no;
  obj_fname = obj_nm;
  DataChanged(DCR_ITEM_UPDATED); // update displays..
  return true;
}


////////////////////////////////////////////////
//              Joints

void VEJointStops::Initialize() {
  stops_on = true;
  lo = -3.1415f;
  hi = 3.1415f;
  def = 0.0f;
  bounce = 0.0f;
  def_force = 0.0f;
}

void VEJointMotor::Initialize() {
  motor_on = false;
  vel = 0.0f;
  f_max = 1.0f;
  servo_on = false;
  trg_pos = 0.0f;
  gain = 0.1f;
}

void VEJointMotor::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  if(gain <= 0.0f) gain = 0.01f;
  if(!motor_on) servo_on = false;
}


void ODEJointParams::Initialize() {
  no_stop_cfm = 1.0e-5f;
  fudge = 1.0f;
}

void VEJoint::Initialize() {
  joint_id = NULL;
  flags = FEEDBACK;
  cur_type = NO_JOINT;
  joint_type = HINGE;
  axis.x = 1.0f;
  axis2.y = 1.0f;
  vis_size = 0.1f;
  pos = pos_norm = vel = pos2 = pos2_norm = vel2 = 0.0f;
}

void VEJoint::Destroy() {
  CutLinks();
}

void VEJoint::CutLinks() {
  DestroyODE();
  inherited::CutLinks();
}

void VEJoint::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  if(!HasTwoAxes()) {
    motor2.motor_on = false;
    motor2.servo_on = false;
  }
  if(motor.servo_on || motor2.servo_on) {
    SetJointFlag(FEEDBACK);
  }
}

VEWorld* VEJoint::GetWorld() {
  return GET_MY_OWNER(VEWorld);
}

void* VEJoint::GetWorldID() {
  VEWorld* wld = GetWorld();
  if(!wld) return wld;
  return wld->world_id;
}

bool VEJoint::CreateODE() {
  if(joint_id && joint_type == cur_type) return true;
  dWorldID wid = (dWorldID)GetWorldID();
  if(TestError(!wid, "CreateODE", "no valid world id -- cannot create joint!"))
    return false;
  if(joint_id)
    DestroyODE();

  if(!body1 || !body1->body_id || body1->HasBodyFlag(VEBody::OFF)) return true;
  if(!body2 || !body2->body_id || body2->HasBodyFlag(VEBody::OFF)) return true;

  if(HasJointFlag(OFF)) {
    return true;
  }

  switch(joint_type) {
  case BALL:
    joint_id = (dJointID)dJointCreateBall(wid, 0);
    break;
  case HINGE:
    joint_id = (dJointID)dJointCreateHinge(wid, 0);
    break;
  case SLIDER:
    joint_id = (dJointID)dJointCreateSlider(wid, 0);
    break;
  case UNIVERSAL:
    joint_id = (dJointID)dJointCreateUniversal(wid, 0);
    break;
  case HINGE2:
    joint_id = (dJointID)dJointCreateHinge2(wid, 0);
    break;
  case FIXED:
    joint_id = (dJointID)dJointCreateFixed(wid, 0);
    break;
//   case PR:
//     joint_id = (dJointID)dJointCreatePR(wid, 0);
//     break;
  case NO_JOINT:
    break;
  }
  cur_type = joint_type;
  return (bool)joint_id;
}

void VEJoint::DestroyODE() {
  if(joint_id) dJointDestroy((dJointID)joint_id);
  joint_id = NULL;
}

void VEJoint::Init() {
  VE_last_ve_set_vals_to_ode = this;

  if(!joint_id || joint_type != cur_type) CreateODE();
  if(!joint_id) return;
  dJointID jid = (dJointID)joint_id;

  // reset probe vals!
  pos = 0.0f;
  pos_norm = 0.0f;
  vel = 0.0f;
  pos2 = 0.0f;
  pos2_norm = 0.0f;
  vel2 = 0.0f;
  cur_force1 = 0.0f;
  cur_force2 = 0.0f;
  cur_torque1 = 0.0f;
  cur_torque2 = 0.0f;

  if(stops.stops_on) {
    pos_norm = stops.Normalize(pos);
  }
  if(HasTwoAxes() && stops2.stops_on) {
    pos2_norm = stops2.Normalize(pos2);
  }

  if(TestError(!body1 || !body1->body_id,
               "Init", "body1 of joint MUST be specified and already exist!"))
    return;
  if(TestError(!body2 || !body2->body_id,
               "Init", "body2 of joint MUST be specified and already exist -- use fixed field on body to set fixed bodies!"))
    return;

  if(HasJointFlag(OFF) || body1->HasBodyFlag(VEBody::OFF) || body2->HasBodyFlag(VEBody::OFF)) {
    DestroyODE();
    return;
  }

  dJointAttach(jid, (dBodyID)body1->body_id, (dBodyID)body2->body_id);

  Init_Anchor();
  Init_Stops();
  Init_Motor();
  Init_ODEParams();

  if(HasJointFlag(FEEDBACK)) {
    dJointSetFeedback(jid, &ode_fdbk_obj);
  }
}

void VEJoint::Init_Anchor() {
  dJointID jid = (dJointID)joint_id;
  taVector3f wanchor = body1->init_pos + anchor; // world anchor offset from body1 position

  switch(joint_type) {
  case BALL:
    dJointSetBallAnchor(jid, wanchor.x, wanchor.y, wanchor.z);
    break;
  case HINGE:
    dJointSetHingeAxis(jid, axis.x, axis.y, axis.z);
    dJointSetHingeAnchor(jid, wanchor.x, wanchor.y, wanchor.z);
    break;
  case SLIDER:
    dJointSetSliderAxis(jid, axis.x, axis.y, axis.z);
    break;
  case UNIVERSAL:
    dJointSetUniversalAxis1(jid, axis.x, axis.y, axis.z);
    dJointSetUniversalAxis2(jid, axis2.x, axis2.y, axis2.z);
    dJointSetUniversalAnchor(jid, wanchor.x, wanchor.y, wanchor.z);
    break;
  case HINGE2:
    dJointSetHinge2Axis1(jid, axis.x, axis.y, axis.z);
    dJointSetHinge2Axis2(jid, axis2.x, axis2.y, axis2.z);
    dJointSetHinge2Anchor(jid, wanchor.x, wanchor.y, wanchor.z);
    break;
  case FIXED:
    dJointSetFixed(jid);
    break;
//   case PR:
//     dJointSetPRAnchor(jid, wanchor.x, wanchor.y, wanchor.z);
//     dJointSetPRAxis1(jid, axis.x, axis.y, axis.z);
//     dJointSetPRAxis2(jid, axis2.x, axis2.y, axis2.z);
//     break;
  case NO_JOINT:
    break;
  }
}

void VEJoint::Init_Stops() {
  dJointID jid = (dJointID)joint_id;

  if(joint_type == HINGE2) {
    dJointSetHinge2Param(jid, dParamSuspensionERP, suspension.erp);
    dJointSetHinge2Param(jid, dParamSuspensionCFM, suspension.cfm);
  }

  // dInfinity turns off..
  switch(joint_type) {
  case HINGE:
    if(stops.stops_on) {
      dJointSetHingeParam(jid, dParamLoStop, stops.lo);
      dJointSetHingeParam(jid, dParamHiStop, stops.hi);
      dJointSetHingeParam(jid, dParamBounce, stops.bounce);
    }
    else {
      dJointSetHingeParam(jid, dParamLoStop, dInfinity);
      dJointSetHingeParam(jid, dParamHiStop, dInfinity);
    }
    break;
  case SLIDER:
    if(stops.stops_on) {
      dJointSetSliderParam(jid, dParamLoStop, stops.lo);
      dJointSetSliderParam(jid, dParamHiStop, stops.hi);
      dJointSetSliderParam(jid, dParamBounce, stops.bounce);
    }
    else {
      dJointSetSliderParam(jid, dParamLoStop, dInfinity);
      dJointSetSliderParam(jid, dParamHiStop, dInfinity);
    }
    break;
  case UNIVERSAL:
    if(stops.stops_on) {
      dJointSetUniversalParam(jid, dParamLoStop, stops.lo);
      dJointSetUniversalParam(jid, dParamHiStop, stops.hi);
      dJointSetUniversalParam(jid, dParamBounce, stops.bounce);
    }
    else {
      dJointSetUniversalParam(jid, dParamLoStop, dInfinity);
      dJointSetUniversalParam(jid, dParamHiStop, dInfinity);
    }
    if(stops2.stops_on) {
      dJointSetUniversalParam(jid, dParamLoStop2, stops2.lo);
      dJointSetUniversalParam(jid, dParamHiStop2, stops2.hi);
      dJointSetUniversalParam(jid, dParamBounce2, stops2.bounce);
    }
    else {
      dJointSetUniversalParam(jid, dParamLoStop2, dInfinity);
      dJointSetUniversalParam(jid, dParamHiStop2, dInfinity);
    }
    break;
  case HINGE2:
    if(stops.stops_on) {
      dJointSetHinge2Param(jid, dParamLoStop, stops.lo);
      dJointSetHinge2Param(jid, dParamHiStop, stops.hi);
      dJointSetHinge2Param(jid, dParamBounce, stops.bounce);
    }
    else {
      dJointSetHinge2Param(jid, dParamLoStop, dInfinity);
      dJointSetHinge2Param(jid, dParamHiStop, dInfinity);
    }
    if(stops2.stops_on) {
      dJointSetHinge2Param(jid, dParamLoStop2, stops2.lo);
      dJointSetHinge2Param(jid, dParamHiStop2, stops2.hi);
      dJointSetHinge2Param(jid, dParamBounce2, stops2.bounce);
    }
    else {
      dJointSetHinge2Param(jid, dParamLoStop2, dInfinity);
      dJointSetHinge2Param(jid, dParamHiStop2, dInfinity);
    }
    break;
  case FIXED:
    break;
  case BALL:
    break;
  case NO_JOINT:
    break;
  }
}

void VEJoint::Init_Motor() {
  dJointID jid = (dJointID)joint_id;

  if(motor.motor_on && motor.servo_on) {
    motor.vel = motor.gain * (motor.trg_pos - pos);
  }
  if(motor2.motor_on && motor2.servo_on) {
    motor2.vel = motor2.gain * (motor2.trg_pos - pos2);
  }

  switch(joint_type) {
  case HINGE:
    if(motor.motor_on) {
      dJointSetHingeParam(jid, dParamVel, motor.vel);
      dJointSetHingeParam(jid, dParamFMax, motor.f_max);
    }
    else {
      dJointSetHingeParam(jid, dParamFMax, 0.0f);
    }
    break;
  case SLIDER:
    if(motor.motor_on) {
      dJointSetSliderParam(jid, dParamVel, motor.vel);
      dJointSetSliderParam(jid, dParamFMax, motor.f_max);
    }
    else {
      dJointSetSliderParam(jid, dParamFMax, 0.0f);
    }
    break;
  case UNIVERSAL:
    if(motor.motor_on) {
      dJointSetUniversalParam(jid, dParamVel, motor.vel);
      dJointSetUniversalParam(jid, dParamFMax, motor.f_max);
    }
    else {
      dJointSetUniversalParam(jid, dParamFMax, 0.0f);
    }
    if(motor2.motor_on) {
      dJointSetUniversalParam(jid, dParamVel2, motor2.vel);
      dJointSetUniversalParam(jid, dParamFMax2, motor2.f_max);
    }
    else {
      dJointSetUniversalParam(jid, dParamFMax2, 0.0f);
    }
    break;
  case HINGE2:
    if(motor.motor_on) {
      dJointSetHinge2Param(jid, dParamVel, motor.vel);
      dJointSetHinge2Param(jid, dParamFMax, motor.f_max);
    }
    else {
      dJointSetHinge2Param(jid, dParamFMax, 0.0f);
    }
    if(motor2.motor_on) {
      dJointSetHinge2Param(jid, dParamVel2, motor2.vel);
      dJointSetHinge2Param(jid, dParamFMax2, motor2.f_max);
    }
    else {
      dJointSetHinge2Param(jid, dParamFMax2, 0.0f);
    }
    break;
  case FIXED:
    break;
  case BALL:
    break;
  case NO_JOINT:
    break;
  }
}

void VEJoint::Init_ODEParams() {
  dJointID jid = (dJointID)joint_id;
  if(!HasJointFlag(USE_ODE_PARAMS)) return;

  switch(joint_type) {
  case HINGE:
    dJointSetHingeParam(jid, dParamFudgeFactor, ode_params.fudge);
    dJointSetHingeParam(jid, dParamCFM, ode_params.no_stop_cfm);
    dJointSetHingeParam(jid, dParamStopERP, ode_params.erp);
    dJointSetHingeParam(jid, dParamStopCFM, ode_params.cfm);
    break;
  case SLIDER:
    dJointSetSliderParam(jid, dParamFudgeFactor, ode_params.fudge);
    dJointSetSliderParam(jid, dParamCFM, ode_params.no_stop_cfm);
    dJointSetSliderParam(jid, dParamStopERP, ode_params.erp);
    dJointSetSliderParam(jid, dParamStopCFM, ode_params.cfm);
    break;
  case UNIVERSAL:
    dJointSetUniversalParam(jid, dParamFudgeFactor, ode_params.fudge);
    dJointSetUniversalParam(jid, dParamCFM, ode_params.no_stop_cfm);
    dJointSetUniversalParam(jid, dParamStopERP, ode_params.erp);
    dJointSetUniversalParam(jid, dParamStopCFM, ode_params.cfm);
    dJointSetUniversalParam(jid, dParamFudgeFactor2, ode_params.fudge);
    dJointSetUniversalParam(jid, dParamCFM2, ode_params.no_stop_cfm);
    dJointSetUniversalParam(jid, dParamStopERP2, ode_params.erp);
    dJointSetUniversalParam(jid, dParamStopCFM2, ode_params.cfm);
    break;
  case HINGE2:
    dJointSetHinge2Param(jid, dParamFudgeFactor, ode_params.fudge);
    dJointSetHinge2Param(jid, dParamCFM, ode_params.no_stop_cfm);
    dJointSetHinge2Param(jid, dParamStopERP, ode_params.erp);
    dJointSetHinge2Param(jid, dParamStopCFM, ode_params.cfm);
    dJointSetHinge2Param(jid, dParamFudgeFactor2, ode_params.fudge);
    dJointSetHinge2Param(jid, dParamCFM2, ode_params.no_stop_cfm);
    dJointSetHinge2Param(jid, dParamStopERP2, ode_params.erp);
    dJointSetHinge2Param(jid, dParamStopCFM2, ode_params.cfm);
    break;
  case FIXED:
    break;
  case BALL:
    break;
  case NO_JOINT:
    break;
  }
}

static inline float get_val_no_nan(float val) {
  if (isnan(val)) return 0.0f;
  return val;
}

void VEJoint::CurFromODE(bool updt_disp) {
  if(!HasJointFlag(FEEDBACK)) return;
  if(!joint_id) CreateODE();
  if(!joint_id) return;
  dJointID jid = (dJointID)joint_id;

  dJointGetFeedback(jid);
  cur_force1.x = get_val_no_nan(ode_fdbk_obj.f1[0]);
  cur_force1.y = get_val_no_nan(ode_fdbk_obj.f1[1]);
  cur_force1.z = get_val_no_nan(ode_fdbk_obj.f1[2]);
  cur_force2.x = get_val_no_nan(ode_fdbk_obj.f2[0]);
  cur_force2.y = get_val_no_nan(ode_fdbk_obj.f2[1]);
  cur_force2.z = get_val_no_nan(ode_fdbk_obj.f2[2]);
  cur_torque1.x = get_val_no_nan(ode_fdbk_obj.t1[0]);
  cur_torque1.y = get_val_no_nan(ode_fdbk_obj.t1[1]);
  cur_torque1.z = get_val_no_nan(ode_fdbk_obj.t1[2]);
  cur_torque2.x = get_val_no_nan(ode_fdbk_obj.t2[0]);
  cur_torque2.y = get_val_no_nan(ode_fdbk_obj.t2[1]);
  cur_torque2.z = get_val_no_nan(ode_fdbk_obj.t2[2]);

  switch(joint_type) {
  case BALL:
    break;
  case HINGE:
    pos = get_val_no_nan(dJointGetHingeAngle(jid));
    vel = get_val_no_nan(dJointGetHingeAngleRate(jid));
    break;
  case SLIDER:
    pos = get_val_no_nan(dJointGetSliderPosition(jid));
    vel = get_val_no_nan(dJointGetSliderPositionRate(jid));
    break;
  case UNIVERSAL:
    pos = get_val_no_nan(dJointGetUniversalAngle1(jid));
    vel = get_val_no_nan(dJointGetUniversalAngle1Rate(jid));
    pos2 = get_val_no_nan(dJointGetUniversalAngle2(jid));
    vel2 = get_val_no_nan(dJointGetUniversalAngle2Rate(jid));
    break;
  case HINGE2:
    pos = get_val_no_nan(dJointGetHinge2Angle1(jid));
    vel = get_val_no_nan(dJointGetHinge2Angle1Rate(jid));
    vel2 = get_val_no_nan(dJointGetHinge2Angle2Rate(jid));
    break;
  case FIXED:
    break;
  case NO_JOINT:
    break;
  }

  if(stops.stops_on) {
    pos_norm = stops.Normalize(pos);
  }
  if(HasTwoAxes() && stops2.stops_on) {
    pos2_norm = stops2.Normalize(pos2);
  }

  if((motor.motor_on && motor.servo_on) || (motor2.motor_on && motor2.servo_on)) {
    Init_Motor();       // update the motor each step..
  }

  if(updt_disp)
    DataChanged(DCR_ITEM_UPDATED);
}

void VEJoint::ApplyForce(float force1, float force2) {
  if(!joint_id) CreateODE();
  if(!joint_id) return;
  dJointID jid = (dJointID)joint_id;

  if(stops.stops_on && stops.def_force > 0.0f)
    force1 += -stops.def_force * (pos - stops.def);
  if(stops2.stops_on && stops2.def_force > 0.0f)
    force2 += -stops2.def_force * (pos2 - stops2.def);

  switch(joint_type) {
  case BALL:
    break;
  case HINGE:
    dJointAddHingeTorque(jid, force1);
    break;
  case SLIDER:
    dJointAddSliderForce(jid, force1);
    break;
  case UNIVERSAL:
    dJointAddUniversalTorques(jid, force1, force2);
    break;
  case HINGE2:
    dJointAddHinge2Torques(jid, force1, force2);
    break;
  case FIXED:
    break;
  case NO_JOINT:
    break;
  }
}

void VEJoint::ApplyMotor(float vel1, float f_max1, float vel2, float f_max2) {
  if(!joint_id) CreateODE();
  if(!joint_id) return;

  SetJointFlag(FEEDBACK);

  motor.servo_on = false;
  if(f_max1 <= 0.0f) {
    motor.motor_on = false;
  }
  else {
    motor.motor_on = true;
  }
  motor2.servo_on = false;
  if(f_max2 <= 0.0f) {
    motor2.motor_on = false;
  }
  else {
    motor2.motor_on = true;
  }

  motor.vel = vel1;
  motor.f_max = f_max1;
  motor2.vel = vel2;
  motor2.f_max = f_max2;

  Init_Motor();
}

void VEJoint::ApplyServo(float trg_pos1, float trg_pos2) {
  if(!joint_id) CreateODE();
  if(!joint_id) return;

  SetJointFlag(FEEDBACK);

  motor.motor_on = true;
  motor.servo_on = true;
  motor.trg_pos = trg_pos1;
  if(HasTwoAxes()) {
    motor2.motor_on = true;
    motor2.servo_on = true;
    motor2.trg_pos = trg_pos2;
  }

  Init_Motor();
}

void VEJoint::ApplyServoNorm(float trg_norm_pos1, float trg_norm_pos2, float stop_buffer) {
  float trg_pos1 = trg_norm_pos1;
  float trg_pos2 = trg_norm_pos2;
  if(stops.stops_on) {
    trg_pos1 = stops.Project(trg_norm_pos1);
    trg_pos1 = MAX(stops.lo + stop_buffer, trg_pos1);
    trg_pos1 = MIN(stops.hi - stop_buffer, trg_pos1);
  }
  if(HasTwoAxes() && stops2.stops_on) {
    trg_pos2 = stops2.Project(trg_norm_pos2);
    trg_pos2 = MAX(stops2.lo + stop_buffer, trg_pos2);
    trg_pos2 = MIN(stops2.hi - stop_buffer, trg_pos2);
  }
  ApplyServo(trg_pos1, trg_pos2);
}


/////////////////////////////////////////////
//              Group

void VEJoint_Group::Init() {
  FOREACH_ELEM_IN_GROUP(VEJoint, ob, *this) {
    ob->Init();
  }
}

void VEJoint_Group::CurFromODE(bool updt_disp) {
  FOREACH_ELEM_IN_GROUP(VEJoint, ob, *this) {
    ob->CurFromODE(updt_disp);
  }
}

void VEJoint_Group::DestroyODE() {
  FOREACH_ELEM_IN_GROUP(VEJoint, ob, *this) {
    ob->DestroyODE();
  }
}

////////////////////////////////////////////////
//      Special VE stuff for robotic arm sims

void VELambdaMuscle::Initialize() {
  lambda_norm = 0.5f;
  lambda = 0.10f;
  co_contract_pct = 0.2f;
  extra_force = 0.0f;

  muscle_type = FLEXOR;
  moment_arm = .04f;
  len_range.min = 0.05f;
  len_range.max = 0.15f;
  co_contract_len = 0.04f;
  rest_len = 0.10f;

  step_size = 0.02f;
  vel_damp = 0.06f;
  reflex_delay = .025f;
  reflex_delay_idx = 5;
  m_rec_grad = 11.2f;
  m_mag = 2.1f;
  ca_dt = 0.015f;
  fv1 = 0.82f;
  fv2 = 0.50f;
  fv3 = 0.43f;
  fv4 = 0.58f;
  passive_k = 0.0f;

  len= lambda;
  dlen= 0.0f;
  act = 0.0f;
  m_act_force = 0.0f;
  force = 0.0f;
  torque = 0.0f;
}

void VELambdaMuscle::Init(float step_sz, float rest_norm_angle, float init_norm_angle,
                          float co_contract) {
  step_size = step_sz;
  lambda_norm = rest_norm_angle; // target is to go to rest
  rest_len = LenFmAngle(rest_norm_angle);
  len = LenFmAngle(init_norm_angle);
  co_contract_pct = co_contract;

  Compute_Lambda();             // get lambda from params

  dlen = 0.0f;
  act = 0.0f;
  m_act_force = 0.0f;
  m_force = 0.0f;
  force = 0.0f;
  torque = 0.0f;

  len_buf.Reset();
  dlen_buf.Reset();

  if(muscle_obj) {
    muscle_obj->length = len;
    muscle_obj->Init();
    muscle_obj->UpdateAfterEdit(); // update display
  }

  UpdateAfterEdit();
}

float VELambdaMuscle::LenFmAngle(float norm_angle) {
  // using a simple linear function here -- fairly accurate as shown in Andrew H. Fagg
  // tech report #00-03: A Model of Muscle Geometry for a Two Degree-Of-Freedom Planar Arm
  if(muscle_type == FLEXOR) {
    return len_range.Project(1.0f - norm_angle); // reversed sense
  }
  return len_range.Project(norm_angle);
}

void VELambdaMuscle::Compute_Lambda() {
  // ensure normalization
  lambda_norm = MIN(1.0f, lambda_norm); lambda_norm = MAX(0.0f, lambda_norm);
  co_contract_pct = MIN(1.0f, co_contract_pct); co_contract_pct = MAX(0.0f, co_contract_pct);
  lambda = len_range.Project(lambda_norm); // project norm force value into real coords
  lambda -= co_contract_pct * co_contract_len;
}

void VELambdaMuscle::Compute_Force(float cur_norm_angle) {
  Compute_Lambda();

  float cur_len = LenFmAngle(cur_norm_angle);
  cur_len = len_range.Clip(cur_len); // keep it in range -- else nonsensical

  dlen = (cur_len - len) / step_size;
  len = cur_len;

  len_buf.CircAddLimit(len, reflex_delay_idx);
  dlen_buf.CircAddLimit(dlen, reflex_delay_idx);

  if(len_buf.length < reflex_delay_idx) { // just starting out -- no history -- no activation
    act = 0.0f;
  }
  else {
    float del_len = len_buf.CircSafeEl((int)(reflex_delay-1));
    float del_dlen = dlen_buf.CircSafeEl((int)(reflex_delay-1));
    act = (del_len - lambda) + vel_damp * del_dlen;
    if(act < 0.0f) act = 0.0f;
  }
  m_act_force = m_mag * (expf(m_rec_grad * act) - 1.0f);
  m_force += ca_dt_cmp * (m_act_force - m_force); // first order low-pass filter, not 2nd order
  force = extra_force + m_force * (fv1 + fv2 * atanf(fv3 + fv4 * dlen)) + passive_k * (len - rest_len);
  torque = force * moment_arm;  // assume constant moment arm: could compute based on geom.

  if(muscle_obj) {
    muscle_obj->length = len;
    muscle_obj->Init();
    muscle_obj->UpdateAfterEdit(); // update display
  }
}

void VELambdaMuscle::SetTargAngle(float targ_norm_angle, float co_contract) {
  co_contract_pct = co_contract;
  lambda = LenFmAngle(targ_norm_angle);
  lambda = len_range.Clip(lambda);              // keep in range
  lambda_norm = len_range.Normalize(lambda);    // this is still key command
  Compute_Lambda();
}

void VELambdaMuscle::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  if(step_size > 0.0f) {
    reflex_delay_idx = (int)(0.5f + (reflex_delay / step_size));
    if(ca_dt > 0.0f)
      ca_dt_cmp = step_size / ca_dt;
  }
  if(co_contract_len > .95f * len_range.min)
    co_contract_len = .95f * len_range.min;
  Compute_Lambda();
}

///////////////////////////////
//      VEMuscleJoint

void VEMuscleJoint::Initialize() {
  joint_type = HINGE;
  SetJointFlag(FEEDBACK);

  extensor.moment_arm = -extensor.moment_arm; // extensor is negative
  flexor.muscle_type = VELambdaMuscle::FLEXOR;
  extensor.muscle_type = VELambdaMuscle::EXTENSOR;

  extensor2.moment_arm = -extensor2.moment_arm; // extensor is negative
  flexor2.muscle_type = VELambdaMuscle::FLEXOR;
  extensor2.muscle_type = VELambdaMuscle::EXTENSOR;

  targ_norm_angle = 0.0f;
  targ_angle = 0.0f;

  targ_norm_angle2 = 0.0f;
  targ_angle2 = 0.0f;

  co_contract_pct = 0.5f;

  motor.motor_on = true;
  stops.stops_on = true;        // need stops
}

void VEMuscleJoint::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  if(joint_type == BALL || joint_type == NO_JOINT) {
    taMisc::Warning("cannot use BALL or NO_JOINT joint_types for muscle joint -- setting to HINGE");
    joint_type = HINGE;
  }

  SetJointFlag(FEEDBACK);       // definitely need this
  if(HasTwoAxes()) {
    motor2.motor_on = true;
    stops2.stops_on = true;
  }
  else {
    motor2.motor_on = false;
    stops2.stops_on = false;
  }
}

void VEMuscleJoint::Init() {
  inherited::Init();

  VEWorld* wld = GetWorld();
  float step_sz = wld->stepsize;

  float rest_norm_angle = stops.Normalize(stops.def); // def = rest
  float init_norm_angle = stops.Normalize(pos);       // pos = cur position/angle

  targ_norm_angle = rest_norm_angle;
  targ_angle = stops.def;
  pos_norm = init_norm_angle;

  extensor.Init(step_sz, rest_norm_angle, init_norm_angle, co_contract_pct);
  flexor.Init(step_sz, rest_norm_angle, init_norm_angle, co_contract_pct);

  if(HasTwoAxes()) {
    float rest_norm_angle2 = stops2.Normalize(stops2.def); // def = rest
    float init_norm_angle2 = stops2.Normalize(pos2);          // pos = cur position/angle

    targ_norm_angle2 = rest_norm_angle2;
    targ_angle2 = stops2.def;
    pos2_norm = init_norm_angle2;

    extensor2.Init(step_sz, rest_norm_angle2, init_norm_angle2, co_contract_pct);
    flexor2.Init(step_sz, rest_norm_angle2, init_norm_angle2, co_contract_pct);
  }
}

void VEMuscleJoint::CurFromODE(bool updt_disp) {
  inherited::CurFromODE(updt_disp);

  flexor.Compute_Force(pos_norm);
  extensor.Compute_Force(pos_norm);

  float force1 = extensor.torque + flexor.torque;  // simple sum of torques..

  float moto_fmax1 = 0.0f;
  // motor idea is that guy pulling in opposite direction opposes motion..
  if(flexor.torque > -extensor.torque) // net flex dir
    moto_fmax1 = -extensor.torque; // apply positive valued extensor torque to stop motor
  else
    moto_fmax1 = flexor.torque; // apply positive valued extensor torque to stop motor

  float force2 = 0.0f;
  float moto_fmax2 = 0.0f;

  if(HasTwoAxes()) {
    flexor2.Compute_Force(pos2_norm);
    extensor2.Compute_Force(pos2_norm);

    force2 = extensor2.torque + flexor2.torque;  // simple sum of torques..

    // motor idea is that guy pulling in opposite direction opposes motion..
    if(flexor2.torque > -extensor2.torque) // net flex dir
      moto_fmax2 = -extensor2.torque; // apply positive valued extensor torque to stop motor
    else
      moto_fmax2 = flexor2.torque; // apply positive valued extensor torque to stop motor
  }

  ApplyForce(force1, force2);
  ApplyMotor(0.0f, moto_fmax1, 0.0f, moto_fmax2);
}

void VEMuscleJoint::SetTargAngle(float trg_angle, float co_contract, float trg_angle2) {
  co_contract_pct = co_contract;
  float norm_angle = stops.Normalize(targ_angle);

  targ_norm_angle = norm_angle;
  targ_angle = trg_angle;

  flexor.SetTargAngle(norm_angle, co_contract_pct);
  extensor.SetTargAngle(norm_angle, co_contract_pct);

  if(HasTwoAxes()) {
    float norm_angle2 = stops2.Normalize(targ_angle2);

    targ_norm_angle2 = norm_angle2;
    targ_angle2 = trg_angle2;

    flexor2.SetTargAngle(norm_angle2, co_contract_pct);
    extensor2.SetTargAngle(norm_angle2, co_contract_pct);
  }
}

void VEMuscleJoint::SetTargNormAngle(float trg_norm_angle, float co_contract,
                                     float trg_norm_angle2) {
  co_contract_pct = co_contract;

  targ_norm_angle = trg_norm_angle;
  targ_angle = stops.Project(targ_norm_angle);

  flexor.SetTargAngle(targ_norm_angle, co_contract_pct);
  extensor.SetTargAngle(targ_norm_angle, co_contract_pct);

  if(HasTwoAxes()) {
    targ_norm_angle2 = trg_norm_angle2;
    targ_angle2 = stops2.Project(targ_norm_angle2);

    flexor2.SetTargAngle(targ_norm_angle2, co_contract_pct);
    extensor2.SetTargAngle(targ_norm_angle2, co_contract_pct);
  }
}

///////////////////////////////////////////////////////////////
//  Arm: bodies and joints representing an arm (and a torso too)

void VEArm::Initialize() {
  // just the default initial values here -- note that VEObject parent initializes all the space stuff in its Initialize, so you don't need to do that here
  arm_side = RIGHT_ARM;
  // note: torso ref is self initializing
  La = 0.3f;
  Lf = 0.33f;
  elbow_gap = 0.03f; // space left between bodies so joint can rotate
  wrist_gap = 0.03f;
        //VEWorld* Worldly = GetWorld(); // these lines crash Emergent on Startup
        //WorldStep = Worldly->stepsize;

// Initializing the insertion point matrices, assuming RIGHT_ARM and shoulder at origin
// Should copy this to ConfigArm

  // Here are all the muscle insertion points in the trunk and shoulder, as
  // labeled in (46) starting with point e
  float ShouldIP_f[] = { -0.05f,  0,      0,       -0.11f,  0,     -0.07f, // e,g
                         -0.08f,  0.01f,  0,       -0.08f, -0.01f,  0,     // i,k
                         -0.14f,  0.06f,  0.01f,   -0.10f, -0.05f,  0.01f, // m,o
                         -0.14f,  0.06f, -0.12f,   -0.10f, -0.05f, -0.12f, // q,s
                         -0.02f,  0.02f,  0.01f };                         // t

  ShouldIP = float_Matrix(2,3,9);     // the matrix has two indices, 3 columns and 9 rows
  ShouldIP.InitFromFloats(ShouldIP_f);

  // Here are all the muscle insertion points in the arm, as labeled in (46),
  // starting with point d
  float ArmIP_f[] = {  0.02f,  0,      -0.05f,   -0.02f,  0,      -0.05f,   // d,f
                       0,      0.02f,   0,        0,     -0.02f,   0,      // h,j
                      -0.01f,  0.01f,  -0.06f,   -0.01f, -0.01f,  -0.06f,   // l,n
                      -0.01f,  0.01f,  -0.05f,   -0.01f, -0.01f,  -0.05f,   // p,r
                       0,     -0.015f, -0.06f,    0,      0.015f, -0.15f }; // v,x

  ArmIP = float_Matrix(2,3,10);
  ArmIP.InitFromFloats(ArmIP_f);

  //String aout;
  //ArmIP.Print(aout);
  //taMisc::Info("init ArmIP:\n", aout);

  // Here are the muscle insertion points in the forearm, corresponding to the
  // biceps, the triceps, and the brachialis.
  float FarmIP_f[] = {  0,      0.015f, -La - 0.05f,
                        0,     -0.005f, -La + 0.03f,
                       -0.01f,  0.015f, -La - 0.04f };
  FarmIP = float_Matrix(2,3,3);
  FarmIP.InitFromFloats(FarmIP_f);

  // Here are the initial and final points of the restricted bending lines for
  // each one of the muscles 1-8. The values for muscles 3,4 are not used
  float p1_f[] = {  0.03f, -0.02f,  0.02f,    0.01f,  0.03f, -0.02f,
                    0,      0.02f,  0.01f,    0,     -0.02f,  0.01f,
                   -0.02f,  0.03f,  0.02f,   -0.01f, -0.05f, -0.07f,
                   -0.02f,  0.03f,  0.02f,   -0.01f, -0.05f, -0.07f };
  p1 = float_Matrix(2,3,8);
  p1.InitFromFloats(p1_f);

  float p2_f[] = {  0.03f,  0.02f,  0.02f,    0.01f, -0.03f, -0.02f,
                    0,      0.02f, -0.01f,    0,     -0.02f, -0.01f,
                   -0.03f,  0.03f, -0.06f,    0,     -0.04f,  0.02f,
                   -0.03f,  0.03f, -0.06f,    0,     -0.04f,  0.02f };
  p2 = float_Matrix(2,3,8);
  p2.InitFromFloats(p2_f);

// Initializing the muscles
  // the number of muscles is 1/2 the sum of points in ShoulderIP+ArmIP+FarmIP
        Nmusc = (int)((FarmIP.count() + ArmIP.count() + ShouldIP.count())/6);
  VELinearMuscle* musl = muscles.NewEl(Nmusc); // creating VELinearMuscle objects to populate group

  //UpdateIPs(); // attaching muscles to their corresponding insertion points
                 // crashes the program at VEBody* humerus = bodies[HUMERUS];
        //for(int i=0; i<Nmusc; i++) // initializing past muscle lengths
                //muscles[i]->OldLength2 = muscles[i]->OldLength1 = muscles[i]->Length();

}

bool VEArm::UpdateIPs() {

// We update the muscles' past lengths before setting new IPs
        for(int i=0; i<Nmusc; i++) {
                muscles[i]->OldLength2 = muscles[i]->OldLength1;
                muscles[i]->OldLength1 = muscles[i]->Length();
        }

// To set new IPs, first we'll find the coordinates of the rotated IPs
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  VEBody* humerus = bodies[HUMERUS];
  VEBody* ulna = bodies[ULNA];
  VEJoint* elbow = joints[ELBOW];

  float_Matrix R(2,3,3);
  humerus->cur_quat.ToRotMatrix(R);

  float_Matrix RT(2,3,3); // the transpose of R
  taMath_float::mat_transpose(&RT, &R);

  // rotating the humerus' insertion points
  float_Matrix RotArmIP;
  taMath_float::mat_mult(&RotArmIP, &ArmIP, &RT);

  String out;
  RotArmIP.Print(out);
  taMisc::Info("rotated ArmIP:\n", out);

  // rotating the ulna's insertion points
        //elbow->CurFromODE(true);      // so the angle we get is actualized
  float delta = elbow->pos;     // DON'T KNOW IF THIS WILL WORK
  taMisc::Info("elbow pos at UpdateIPs: ", String(delta), "\n");
  float UlnaShift_f[] = {0, 0, -La,
                         0, 0, -La,
                         0, 0, -La}; // should have one row per forearm IP
  float T_elbowRot_f[] = {1, 0, 0,
                0, cos(delta),  sin(delta),
                0, -sin(delta), cos(delta)};
  // T_elbowRot is the TRANSPOSED rotation matrix for the delta rotation
  float_Matrix UlnaShift(2,3,3);
  UlnaShift.InitFromFloats(UlnaShift_f);
  float_Matrix T_elbowRot(2,3,3);
  T_elbowRot.InitFromFloats(T_elbowRot_f);

// first we shift the FarmIP's so the origin is at the elbow
  float_Matrix ShiftedIP(2,3,3);
  ShiftedIP = FarmIP - UlnaShift;

// we rotate the shifted IPs by the elbow bend (delta rotation)
  float_Matrix Rot1FarmIP(2,3,3);
  taMath_float::mat_mult(&Rot1FarmIP, &ShiftedIP, &T_elbowRot);
/*
  String ruout;
  Rot1FarmIP.Print(ruout);
  taMisc::Info("rotated ulna before translation:\n", ruout);
*/

// now we set the origin at the shoulder
  float_Matrix ReshiftedIP(2,3,3);
  ReshiftedIP = Rot1FarmIP + UlnaShift;

// finally we apply the shoulder rotation
  float_Matrix RotFarmIP(2,3,3);
  taMath_float::mat_mult(&RotFarmIP, &ReshiftedIP, &RT);

  String ripout;
  RotFarmIP.Print(ripout);
  taMisc::Info("rotated ulna IPs:\n", ripout);
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// Now we assign the rotated IPs to the corresponding muscles
//-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_
  taVector3f c1, c2, shoulderIP, humerIP, pv1, pv2, p3;
  for(int i=0; i<8; i++)  // the 8 shoulder to humerus muscles
    {
      muscles[i]->IPprox.x = ShouldIP.FastElAsFloat(0,i);
      muscles[i]->IPprox.y = ShouldIP.FastElAsFloat(1,i);
      muscles[i]->IPprox.z = ShouldIP.FastElAsFloat(2,i);
      muscles[i]->IPdist.x = RotArmIP.FastElAsFloat(0,i);
      muscles[i]->IPdist.y = RotArmIP.FastElAsFloat(1,i);
      muscles[i]->IPdist.z = RotArmIP.FastElAsFloat(2,i);

      humerIP = muscles[i]->IPdist;
      shoulderIP = muscles[i]->IPprox;
      pv1.x = p1.FastElAsFloat(0,i); pv2.x = p2.FastElAsFloat(0,i);
      pv1.y = p1.FastElAsFloat(1,i); pv2.y = p2.FastElAsFloat(1,i);
      pv1.z = p1.FastElAsFloat(2,i); pv2.z = p2.FastElAsFloat(2,i);

      if(bender(p3,shoulderIP,humerIP,pv1,pv2) && i!=2 && i!=3) {
	// if muscle wraps around bending line (except for muscles 3 and 4)
	muscles[i]->p3 = p3;
	muscles[i]->bend = true;
      } else {
	muscles[i]->bend = false;
      }
    }
  // next muscle is the biceps, from shoulder to forearm
  muscles[8]->IPprox.x = ShouldIP.FastElAsFloat(0,8);
  muscles[8]->IPprox.y = ShouldIP.FastElAsFloat(1,8);
  muscles[8]->IPprox.z = ShouldIP.FastElAsFloat(2,8);
  muscles[8]->IPdist.x = RotFarmIP.FastElAsFloat(0,0);
  muscles[8]->IPdist.y = RotFarmIP.FastElAsFloat(1,0);
  muscles[8]->IPdist.z = RotFarmIP.FastElAsFloat(2,0);
  muscles[8]->bend = false;
  // the triceps and the brachialis connect from humerus to ulna
  for(int i=1; i<=2; i++) {
    muscles[8+i]->IPprox.x = RotArmIP.FastElAsFloat(0,7+i);
    muscles[8+i]->IPprox.y = RotArmIP.FastElAsFloat(1,7+i);
    muscles[8+i]->IPprox.z = RotArmIP.FastElAsFloat(2,7+i);
    muscles[8+i]->IPdist.x = RotFarmIP.FastElAsFloat(0,i);
    muscles[8+i]->IPdist.y = RotFarmIP.FastElAsFloat(1,i);
    muscles[8+i]->IPdist.z = RotFarmIP.FastElAsFloat(2,i);
    muscles[8+i]->bend = false;
  }
//-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_

  return true;
}

void VEArm::Destroy() {
  CutLinks();
}

bool VEArm::CheckArm(bool quiet) {
  bool rval = true;
  if(!torso) {
    rval = false;
    TestError(!quiet, "CheckArm", "torso not set -- must specify a body in another object to serve as the torso");
  }
  if(bodies.size < N_ARM_BODIES) {
    rval = false;
    TestError(!quiet, "CheckArm", "number of bodies < N_ARM_BODIES -- run ConfigArm");
  }
  if(joints.size < N_ARM_JOINTS) {
    rval = false;
    TestError(!quiet, "CheckArm", "number of joints < N_ARM_JOINTS -- run ConfigArm");
  }
  if(Nmusc != (int)((FarmIP.count() + ArmIP.count() + ShouldIP.count())/6)) {
    rval = false;
    TestError(!quiet, "CheckArm", "number of muscles doesn't match number of insertion points -- run ConfigArm");
  }
  // probably that's sufficient for a quick-and-dirty sanity check
  // Also: Nmuscl equal to # of elements in muscles
  return rval;
}

bool VEArm::ConfigArm(const String& name_prefix,
                      float humerus_length, float humerus_radius,
                      float ulna_length, float ulna_radius,
                      float hand_length, float hand_radius) {
  // note: keeping torso out of it, so the arm is fully modular and can attach to anything


  if(TestError(!torso, "ConfigArm", "torso not set -- must specify a body in another object to serve as the torso before running ConfigArm"))
    return false;

  if(bodies.size < N_ARM_BODIES)
    bodies.SetSize(N_ARM_BODIES); // auto-creates the VEBody objects up to specified size
  if(joints.size < N_ARM_JOINTS)
    joints.SetSize(N_ARM_JOINTS); // auto-creates the VEJoint objects up to specified size

  // Getting the stepsize of VEWorld containing this Arm
  VEWorld* Worldly = GetWorld();
  WorldStep = Worldly->stepsize;

  // this is how you access the bodies -- very fast and efficient
  VEBody* humerus = bodies[HUMERUS];
  VEBody* ulna = bodies[ULNA];
  VEBody* hand = bodies[HAND];

  VEJoint* shoulder = joints[SHOULDER];
  VEJoint* elbow = joints[ELBOW];
  VEJoint* wrist = joints[WRIST];

  //------- Setting standard geometry -------
  // todo: just need to know where to attach to torso..
  // torso.shape = VEBody::BOX;
  // torso.length = 0.4;
  // torso.init_pos.x = -0.25; torso.init_pos.y = 0; torso.init_pos.z = -0.2;
  // torso.cur_pos.x = -0.25; torso.cur_pos.y = 0; torso.cur_pos.z = -0.2;
  // torso.box.x = 0.35; torso.box.y = 0.1; torso.box.z = 0.4;
  // torso.SetValsToODE();

  // todo: need to make all this conditional on arm_side!
  humerus->name = name_prefix + "Humerus";
  humerus->shape = VEBody::CAPSULE; humerus->long_axis = VEBody::LONG_Z;
  humerus->length = humerus_length; humerus->radius = humerus_radius;
  humerus->init_pos.x = 0; humerus->init_pos.y = 0; humerus->init_pos.z = -humerus_length/2;
  humerus->init_rot.x = 0; humerus->init_rot.y = 0; humerus->init_rot.z = 0;
  humerus->cur_pos.x = 0; humerus->cur_pos.y = 0; humerus->cur_pos.z = humerus->init_pos.z;
  humerus->cur_rot.x = 0; humerus->cur_rot.y = 0; humerus->cur_rot.z = 0;

  ulna->name = name_prefix + "Ulna";
  ulna->shape = VEBody::CAPSULE; ulna->long_axis = VEBody::LONG_Z;
  ulna->length = ulna_length; ulna->radius = ulna_radius;
  ulna->init_pos.x = 0; ulna->init_pos.y = 0; ulna->init_pos.z =-humerus_length-(ulna_length/2)-elbow_gap;
  ulna->init_rot.x = 0; ulna->init_rot.y = 0; ulna->init_rot.z = 0;
  ulna->cur_pos.x = 0; ulna->cur_pos.y = 0; ulna->cur_pos.z = ulna->init_pos.z;
  ulna->cur_rot.x = 0; ulna->cur_rot.y = 0; ulna->cur_rot.z = 0;

  hand->name = name_prefix + "Hand";
  hand->shape = VEBody::CAPSULE; hand->long_axis = VEBody::LONG_Z;
  hand->length = hand_length; hand->radius = hand_radius;
  hand->init_pos.x = 0; hand->init_pos.y = 0;
  hand->init_pos.z = -(humerus_length+ulna_length+elbow_gap+wrist_gap+(hand_length/2));
  hand->init_rot.x = 0; hand->init_rot.y = 0; hand->init_rot.z = 0;
  hand->cur_pos.x = 0; hand->cur_pos.y = 0; hand->cur_pos.z = hand->init_pos.z;
  hand->cur_rot.x = 0; hand->cur_rot.y = 0; hand->cur_rot.z = 0;

  La = humerus_length + elbow_gap/2;
  Lf = ulna_length + (elbow_gap/2) + wrist_gap + (hand_length/2);

  //-------- Creating initial joints -------

  //-------- Setting joint locations -------
  shoulder->name = name_prefix + "Shoulder";
  shoulder->joint_type = VEJoint::BALL;
  shoulder->body1 = torso; shoulder->body2 = humerus;

  elbow->name = name_prefix + "Elbow";
  elbow->joint_type = VEJoint::HINGE2;
  elbow->body1 = humerus; elbow->body2 = ulna;

  wrist->name = name_prefix + "Wrist";
  wrist->joint_type = VEJoint::FIXED;
  wrist->body1 = ulna; wrist->body2 = hand;

  // the shoulder anchor is wrt to torso's CM, should lie near one corner
  shoulder->anchor.x = 0.2f; shoulder->anchor.y = 0; shoulder->anchor.z = 0.2f;

  elbow->anchor.x = 0;  // set elbow joint's anchor point wrt humerus' CM
  elbow->anchor.y = 0;
  elbow->anchor.z = -(humerus->length/2 +(elbow_gap/2));

  wrist->anchor.x = 0; // set wrist joint's anchor point wrt ulna's CM
  wrist->anchor.y = 0;
  wrist->anchor.z = -(ulna->length/2 + (wrist_gap/2));

  elbow->axis.x = -1;  // setting elbow joint's axes
  elbow->axis.y = 0;
  elbow->axis.z = 0;
  elbow->axis2.x = 0;
  elbow->axis2.y = 0;
  elbow->axis2.z = -1;

// Initializing the insertion point matrices, assuming RIGHT_ARM and shoulder at origin
// Should copy this to ConfigArm

  // Here are all the muscle insertion points in the trunk and shoulder, as
  // labeled in (46) starting with point e
  float ShouldIP_f[] = { -0.05f,  0,      0,       -0.11f,  0,     -0.07f, // e,g
                         -0.08f,  0.01f,  0,       -0.08f, -0.01f,  0,     // i,k
                         -0.14f,  0.06f,  0.01f,   -0.10f, -0.05f,  0.01f, // m,o
                         -0.14f,  0.06f, -0.12f,   -0.10f, -0.05f, -0.12f, // q,s
                         -0.02f,  0.02f,  0.01f };                         // t

  ShouldIP = float_Matrix(2,3,9);     // the matrix has two indices, 3 columns and 9 rows
  ShouldIP.InitFromFloats(ShouldIP_f);

  // Here are all the muscle insertion points in the arm, as labeled in (46),
  // starting with point d
  float ArmIP_f[] = {  0.02f,  0,      -0.05f,   -0.02f,  0,      -0.05f,   // d,f
                       0,      0.02f,   0,        0,     -0.02f,   0,       // h,j
                      -0.01f,  0.01f,  -0.06f,   -0.01f, -0.01f,  -0.06f,   // l,n
                      -0.01f,  0.01f,  -0.05f,   -0.01f, -0.01f,  -0.05f,   // p,r
                       0,     -0.015f, -0.06f,    0,      0.015f, -0.15f }; // v,x

  ArmIP = float_Matrix(2,3,10);
  ArmIP.InitFromFloats(ArmIP_f);

  //String aout;
  //ArmIP.Print(aout);
  //taMisc::Info("init ArmIP:\n", aout);

  // Here are the muscle insertion points in the forearm, corresponding to the
  // biceps, the triceps, and the brachialis.
  float FarmIP_f[] = {  0,      0.015f, -La - 0.05f,
                        0,     -0.005f, -La + 0.03f,
                       -0.01f,  0.015f, -La - 0.04f };
  FarmIP = float_Matrix(2,3,3);
  FarmIP.InitFromFloats(FarmIP_f);

  // Here are the initial and final points of the restricted bending lines for
  // each one of the muscles 1-8. The values for muscles 3,4 are not used
  float p1_f[] = {  0.03f, -0.02f,  0.02f,    0.01f,  0.03f, -0.02f,
                    0,      0.02f,  0.01f,    0,     -0.02f,  0.01f,
                   -0.02f,  0.03f,  0.02f,   -0.01f, -0.05f, -0.07f,
                   -0.02f,  0.03f,  0.02f,   -0.01f, -0.05f, -0.07f };
  p1 = float_Matrix(2,3,8);
  p1.InitFromFloats(p1_f);

  float p2_f[] = {  0.03f,  0.02f,  0.02f,    0.01f, -0.03f, -0.02f,
                    0,      0.02f, -0.01f,    0,     -0.02f, -0.01f,
                   -0.03f,  0.03f, -0.06f,    0,     -0.04f,  0.02f,
                   -0.03f,  0.03f, -0.06f,    0,     -0.04f,  0.02f };
  p2 = float_Matrix(2,3,8);
  p2.InitFromFloats(p2_f);

// Initializing the muscles
  // the number of muscles is 1/2 the sum of points in ShoulderIP+ArmIP+FarmIP
        Nmusc = (int)((FarmIP.count() + ArmIP.count() + ShouldIP.count())/6);
        muscles.SetSize(Nmusc);

  UpdateIPs(); // attaching muscles to their corresponding insertion points

  for(int i=0; i<Nmusc; i++) // initializing past muscle lengths
    muscles[i]->OldLength2 = muscles[i]->OldLength1 = muscles[i]->Length();

  Init();                       // this will attempt to init everything just created..

  DataChanged(DCR_ITEM_UPDATED); // this will in theory update the display

  return true;
}

bool VEArm::MoveToTarget(float trg_x, float trg_y, float trg_z) {
// at some point I should set the arm to its initial position here
  if(!CheckArm()) return false;

  if(trg_x == 0.0 && trg_y == 0.0) {
    trg_y = 0.000001f;  // this is to avoid a Gimble lock
  }

  VEBody* humerus = bodies[HUMERUS];
  VEBody* ulna = bodies[ULNA];
  VEBody* hand = bodies[HAND];
        VEJoint* elbow = joints[ELBOW];
  VEJoint* wrist = joints[WRIST];

  // target coordinates
  float t_f[] = {trg_x, trg_y, trg_z};
  float_Matrix T(1,3);
  T.InitFromFloats(t_f);
  float D = taMath_float::vec_norm(&T);

  if(D < 0.1) {
    taMisc::Info("Target too close \n");
    // moving the target away, maintaining direction
    float Lfactor = 0.15/D;
    trg_x *= Lfactor; trg_y *= Lfactor; trg_z *= Lfactor;
    T.SetFmVar(trg_x,0); T.SetFmVar(trg_y,1); T.SetFmVar(trg_z,2);
    D = taMath_float::vec_norm(&T);
  }
  else if( D >= (La+Lf)) {
      taMisc::Info("Target too far \n");
    // bringing the target closer, maintaining direction
      float Lfactor = (La+Lf-0.01)/D;
      trg_x *= Lfactor; trg_y *= Lfactor; trg_z *= Lfactor;
      T.SetFmVar(trg_x,0); T.SetFmVar(trg_y,1); T.SetFmVar(trg_z,2);
      D = taMath_float::vec_norm(&T);
  }

  // From coordinates to angles as in (44)

  float delta = taMath_float::pi - acos((La*La + Lf*Lf - D*D) / (2.0*La*Lf));
  float gamma = 0;
  float beta = acos(-trg_z/D) - acos((D*D + La*La - Lf*Lf)/(2.0*D*La));
  float alpha = asin(-trg_x/sqrt(trg_x*trg_x + trg_y*trg_y));

  if(trg_y < 0) { // if the target is behind
    alpha = taMath_float::pi - alpha;
  }

  taMisc::Info("alpha:", String(alpha), "beta:", String(beta), "gamma:", String(gamma));

  // Now we'll rotate the insertion points by the Euler angles in reverse order
  // This magic R matrix (from (42)) does it all in one step
  float sa = sin(alpha); float ca = cos(alpha);
  float sb = sin(beta);  float cb = cos(beta);
  float sc = sin(gamma); float cc = cos(gamma);

  float R_f[] = {ca*cc-sa*cb*sc, -ca*sc-sa*cb*cc, sa*sb,
                 sa*cc+ca*cb*sc, -sa*sc+ca*cb*cc, -ca*sb,
                 sb*sc,           sb*cc,          cb};
  float_Matrix R(2,3,3);
  R.InitFromFloats(R_f);

/*
  String Rout;
  R.Print(Rout);
  taMisc::Info("rotation matrix:\n", Rout, "\n");

  //  taQuaternion quat(alpha, beta, gamma);
  // this matches your matrix -- for some reason beta and gamma are reversed???
  //taQuaternion quat(beta, gamma, alpha);

  taQuaternion quat(beta,gamma,alpha);
  float_Matrix qR;
  quat.ToRotMatrix(qR);

  String qRout;
  qR.Print(qRout);
  taMisc::Info("quat rotation matrix:\n", qRout, "\n");

  // test direct quat rotation of first coords, against quat rotation matrix
  taVector3f arm1;
  arm1.FromMatrix(ArmIP);       // grab first coords
  quat.RotateVec(arm1);
  taMisc::Info("quat rotate first ArmIP coords:\n", arm1.GetStr(), "\n");


  // compute: RotArmIP = (R*ArmIP')';
  float_Matrix armipt;
  taMath_float::mat_transpose(&armipt, &ArmIP);

  String tout;
  armipt.Print(tout);
  taMisc::Info("transposed ArmIP:\n", tout);

  float_Matrix armipr;
  taMath_float::mat_mult(&armipr, &R, &armipt);

  String rout;
  armipr.Print(rout);
  taMisc::Info("rot pretransposed ArmIP:\n", rout);

  float_Matrix RotArmIP;
  taMath_float::mat_transpose(&RotArmIP, &armipr);

  String out;
  RotArmIP.Print(out);
  taMisc::Info("rotated ArmIP:\n", out);
*/


//------ Rotating humerus -------
  //humerus->RotateEuler(beta,gamma,alpha,true); // may substitute by Init_Pos();
  //humerus->Init_Pos();
  //humerus->Init_Rotation();
  //Init();
  humerus->RotateEuler(beta,gamma,alpha,false);

  float HumCM_f[] = {0.0f,0.0f,(-La+(elbow_gap/2))/2};  // humerus' geometrical center at rest
  //float Elbow_f[] = {0.0f,0.0f,(-La+(elbow_gap/2))};  // elbow coordinates at rest
  float Elbow_f[] = {0.0f,0.0f,-La};  // elbow coordinates at rest

  float_Matrix HumCM(2,1,3);
  HumCM.InitFromFloats(HumCM_f);
  float_Matrix Elbow(2,1,3);
  Elbow.InitFromFloats(Elbow_f);

  float_Matrix RotHumCM(2,1,3);
  taMath_float::mat_mult(&RotHumCM, &R, &HumCM);  // rotating geometrical center
  float_Matrix RotElbow(2,1,3);
  taMath_float::mat_mult(&RotElbow, &R, &Elbow);  // rotating elbow

humerus->Translate(RotHumCM.FastEl(0),RotHumCM.FastEl(1),RotHumCM.FastEl(2)-(humerus->init_pos.z),false);


//------ Rotating ulna -------
/*
  taVector3f UlnaCM(0.0f, 0.0f, -Lf/2); // Ulna CM with origin at elbow
  taQuaternion flex(0.0f,delta,0.0f); // elbow flexion rotation
  flex.RotateVec(UlnaCM);   // Ulna CM after elbow flexion with origin at elbow
  UlnaCM.SetXYZ(UlnaCM.x,UlnaCM.y,UlnaCM.z-La);  // Ulna CM after elbow flexion
  quat.RotateVec(UlnaCM);       // Ulna CM after elbow flexion, origin at shoulder

  ulna->RotateEuler(delta, 0.0, 0.0, false);
  ulna->RotateEuler(beta,gamma,alpha,false);
  ulna->Translate(UlnaCM.x,UlnaCM.y,UlnaCM.z+La+Lf/2,false);
*/
  float UlnaCM_f[] = {0,0,-(ulna->length/2 + elbow_gap/2)};  // Ulna 'CM' with origin at elbow
  float Wrist_f[] = {0,0,-(ulna->length + elbow_gap/2 + wrist_gap/2)};  // wrist coords with origin at elbow
  float elbow_rot_f[] = {1 , 0, 0,
                0, cos(delta), -sin(delta),
                0, sin(delta), cos(delta)};
  float_Matrix UlnaCM(2,1,3);
  UlnaCM.InitFromFloats(UlnaCM_f);
  float_Matrix Wrist(2,1,3);
  Wrist.InitFromFloats(Wrist_f);
  float_Matrix elbow_rot(2,3,3);
  elbow_rot.InitFromFloats(elbow_rot_f);

  String erout;
  elbow_rot.Print(erout);
  taMisc::Info("elbow rotation matrix :\n", erout);

  float_Matrix Rot1UlnaCM(2,1,3);
  taMath_float::mat_mult(&Rot1UlnaCM, &elbow_rot, &UlnaCM);
  float_Matrix Rot1Wrist(2,1,3);
  taMath_float::mat_mult(&Rot1Wrist, &elbow_rot, &Wrist);

  String ruout;
  Rot1UlnaCM.Print(ruout);
  taMisc::Info("rotated ulna before translation:\n", ruout);

  Rot1UlnaCM.Set(Rot1UlnaCM.FastEl(2)-La,2); // setting origin at shoulder
  Rot1Wrist.Set(Rot1Wrist.FastEl(2)-La,2);

  String rudout;
  Rot1UlnaCM.Print(rudout);
  taMisc::Info("rotated ulna after translation:\n", rudout);

  float_Matrix Rot2UlnaCM(2,1,3);
  taMath_float::mat_mult(&Rot2UlnaCM, &R, &Rot1UlnaCM); // applying shoulder rotation
  float_Matrix Rot2Wrist(2,1,3);
  taMath_float::mat_mult(&Rot2Wrist, &R, &Rot1Wrist);

/*
  taVector3f Rot2UlnaCM;
  Rot2UlnaCM.FromMatrix(Rot1UlnaCM);
  quat.RotateVec(Rot2UlnaCM);
*/
  ulna->RotateEuler(beta+delta,gamma,alpha,false);
  ulna->Translate(Rot2UlnaCM.FastEl(0),Rot2UlnaCM.FastEl(1),Rot2UlnaCM.FastEl(2)-(ulna->init_pos.z),false);

  hand->RotateEuler(beta+delta,gamma,alpha,false);
  hand->Translate(trg_x,trg_y,trg_z-(hand->init_pos.z),false);

  // sending the values of the bodies to ODE
  CurToODE();

  // calculating and updating the joint values
  // setting the axes for the elbow joint
  elbow->axis.x = cos(alpha);
  elbow->axis.y = sin(alpha);
  elbow->axis.z = 0.0f;
  elbow->axis2.x = -sin(alpha)*sin(beta+delta); // sin(beta+delta) normalizes the norm of axis2
  elbow->axis2.y = cos(alpha)*sin(beta+delta);
  elbow->axis2.z = -cos(beta+delta);

  // sending the joint axes and anchor points to ODE
  //elbow->anchor.FromMatrix(RotElbow); // set elbow joint's anchor point
  // wrist->anchor.FromMatrix(Rot2Wrist); // not necessary since relative coordinates are used
  wrist->Init_Anchor();
  elbow->Init_Anchor();

  // looking at the values of the joints in ODE
  dJointID ejid = (dJointID)elbow->joint_id;
  //dJointSetHinge2Anchor(ejid,   elbow->anchor.x, elbow->anchor.y, elbow->anchor.z);
  float angl = (float)dJointGetHinge2Angle1(ejid);
  float ax1[] = {0.0f, 0.0f, 0.0f, 0.0f}, ax2[] = {0.0f, 0.0f, 0.0f, 0.0f};
  float anch[] = {0.0f, 0.0f, 0.0f, 0.0f};
  dJointGetHinge2Anchor(ejid, anch);
  dJointGetHinge2Axis1(ejid, ax1);
  dJointGetHinge2Axis2(ejid, ax2);
  float_Matrix max1(1,4), max2(1,4), manch(1,4);
  String smax1, smax2, smanch;
  max1.InitFromFloats(ax1); max2.InitFromFloats(ax2); manch.InitFromFloats(anch);
  max1.Print(smax1); max2.Print(smax2); manch.Print(smanch);
  taMisc::Info("elbow angle: ", String(angl), "\n");
  taMisc::Info("elbow anchor: ", smanch, "\n");
  taMisc::Info("elbow axis1: ", smax1, "\n");
  taMisc::Info("elbow axis2: ", smax2, "\n");
  //elbow->Init_Anchor();
  //CurFromODE(true);

  DataChanged(DCR_ITEM_UPDATED); // this will in theory update the display

  return true;
}

bool VEArm::bender(taVector3f &p3, taVector3f a, taVector3f c,
		   taVector3f p1, taVector3f p2) {
  // This function is the C++ equivalent of piece5.m.
  // The points a and c are insertion points, whereas p1 and p2 are the extremes of the bending line.
  // If the muscle wraps around the bending line, bender returns true, and inserts the value of
  // the point of intersection with the muscle in the vector i.

  //---- declaring the variables to be used for bend test ----
  //taVector3f a(-10,-5,-12), c(-1,-1,-5), p1(-3,-4,-7), p2(-2,-4,2), p3(0,0,0);
  taVector3f b, r, v, s;
  float gam;

  //---- preliminary calculations ----
  b = p2 - p1;  // b points from p1 to p2
  b.MagNorm();  // b is now a unit vector
  r = p1 - ((p1*b).Sum())*b;  // shortest line from origin to bending line
  //taMisc::Info("r = ", r.GetStr(), "\n");
  //----------------------------------

  // calculating side %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  v = p1 - a - (((p1-a)*b).Sum())*b;  // shortest vector from 'a' to bending line
  s.x = b.y*v.z - v.y*b.z;
  s.y = v.x*b.z - b.x*v.z;
  s.z = b.x*v.y - v.x*b.y;
  gam = ((c-r)*s).Sum();
  //taMisc::Info("v = ", v.GetStr(), ", s = ", s.GetStr(), "\n");
  //taMisc::Info("gam = ", String(gam), "\n");
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

  // get bending point depending on side ()()()()()()()()()()()
  //()()()()()()()()()()()()()()()()()()()()()()()()()()()()()()
  if(gam > 0) {  // if muscle should wrap around bending line
    taVector3f ar,cr,abs_b;
    float f1,f2,f3,f4,ap,bp,cp,k,k1,k2,n1,n2,ind;

    ar = a-r;  // I shift so b can be considered to intersect the origin
    cr = c-r;
    f1 = ar.SqMag();
    f2 = cr.SqMag();
    f3 = (ar*b).Sum();
    f4 = (cr*b).Sum();
    ap = f2 + (f3*f3) - f1 - (f4*f4);
    bp = 2*(f1*f4 - f2*f3 + f3*f4*(f4-f3));
    cp = f2*f3*f3 - f1*f4*f4;

    // selecting the root with the smallest sum of distances
    k1 = (-bp + sqrt(bp*bp - 4*ap*cp))/(2*ap);
    k2 = (-bp - sqrt(bp*bp - 4*ap*cp))/(2*ap);
    n1 = (ar-k1*b).Mag() + (cr-k1*b).Mag();
    n2 = (ar-k2*b).Mag() + (cr-k2*b).Mag();
    if(n1 < n2) {
      k = k1;
    }
    else {
      k = k2;
    }

    // limiting p3 between p1 and p2
    abs_b = b;  abs_b.Abs();
    // this is because b may have entries equal to zero
    if(abs_b.x > abs_b.y && abs_b.x > abs_b.z) {
      ind = 0;
    }
    else if(abs_b.y > abs_b.x && abs_b.y > abs_b.z) {
      ind = 1;
    }
    else {
      ind = 2;
    }

    if(ind == 0) {
      k1 = (p1.x - r.x)/b.x;
      k2 = (p2.x - r.x)/b.x;
    }
    else if(ind == 1) {
      k1 = (p1.y - r.y)/b.y;
      k2 = (p2.y - r.y)/b.y;
    }
    else if(ind == 2) {
      k1 = (p1.z - r.z)/b.z;
      k2 = (p2.z - r.z)/b.z;
    }

    k = MIN(k,MAX(k1,k2));
    k = MAX(k,MIN(k1,k2)); // k is now restricted between p1 and p2
    p3 = r + k*b;           // the point of intersection between muscle and bending line

    //taMisc::Info("p3 = ", p3.GetStr());
    return true;
  }
  //taMisc::Info("p3 = ", p3.GetStr());
  return false;
  //()()()()()()()()()()()()()()()()()()()()()()()()()()()()()()
}

bool VEArm::TargetLengths(float_Matrix &trgLen, float trg_x, float trg_y, float trg_z) {

  if(TestWarning(trgLen.count() != Nmusc, "","The matrix provided to TargetLengths doesn't match the number of muscles \n"))
                return false;

  if(!CheckArm()) return false;

  if(trg_x == 0.0 && trg_y == 0.0) {
    trg_y = 0.000001f;  // this is to avoid a Gimble lock
  }

  VEBody* humerus = bodies[HUMERUS];
  VEBody* ulna = bodies[ULNA];
  VEBody* hand = bodies[HAND];

  // target coordinates
  float t_f[] = {trg_x, trg_y, trg_z};
  float_Matrix T(1,3);
  T.InitFromFloats(t_f);
  float D = taMath_float::vec_norm(&T);

  if(D < 0.1) {
    taMisc::Info("Target too close \n");
    // moving the target away, maintaining direction
    float Lfactor = 0.15/D;
    trg_x *= Lfactor; trg_y *= Lfactor; trg_z *= Lfactor;
    T.SetFmVar(trg_x,0); T.SetFmVar(trg_y,1); T.SetFmVar(trg_z,2);
    D = taMath_float::vec_norm(&T);
  }
  else if( D >= (La+Lf) ) {
    taMisc::Info("Target too far \n");
    // bringing the target closer, maintaining direction
    float Lfactor = (La+Lf-0.01)/D;
    trg_x *= Lfactor; trg_y *= Lfactor; trg_z *= Lfactor;
    T.SetFmVar(trg_x,0); T.SetFmVar(trg_y,1); T.SetFmVar(trg_z,2);
    D = taMath_float::vec_norm(&T);
  }

  // From coordinates to angles as in (44)

  float delta = taMath_float::pi - acos((La*La + Lf*Lf - D*D) / (2.0*La*Lf));
  float gamma = 0;
  float beta = acos(-trg_z/D) - acos((D*D + La*La - Lf*Lf)/(2.0*D*La));
  float alpha = asin(-trg_x/sqrt(trg_x*trg_x + trg_y*trg_y));

  if(trg_y < 0) {  // if the target is behind
    alpha = taMath_float::pi - alpha;
  }

  //taMisc::Info("alpha:", String(alpha), "beta:", String(beta), "gamma:", String(gamma));

  // Now we'll rotate the insertion points by the Euler angles in reverse order
  // This magic R matrix (from (42)) does it all in one step
  float sa = sin(alpha); float ca = cos(alpha);
  float sb = sin(beta);  float cb = cos(beta);
  float sc = sin(gamma); float cc = cos(gamma);

  float R_f[] = {ca*cc-sa*cb*sc, -ca*sc-sa*cb*cc, sa*sb,
                 sa*cc+ca*cb*sc, -sa*sc+ca*cb*cc, -ca*sb,
                 sb*sc,           sb*cc,          cb};
  float_Matrix R(2,3,3);
  R.InitFromFloats(R_f);
  float_Matrix RT(2,3,3); // the transpose of R
  taMath_float::mat_transpose(&RT, &R);

  // rotating the humerus' insertion points
  float_Matrix RotArmIP;
  taMath_float::mat_mult(&RotArmIP, &ArmIP, &RT);

  String out;
  RotArmIP.Print(out);
  taMisc::Info("rotated ArmIP:\n", out);

// rotating the ulna's insertion points
  //float UlnaCM_f[] = {0,0,-(ulna->length/2 + elbow_gap/2)};  // Ulna CM with origin at elbow
  float UlnaShift_f[] = {0, 0, -La,
                         0, 0, -La,
                         0, 0, -La}; // should have one row per forearm IP
  float T_elbowRot_f[] = {1 , 0, 0,
                0, cos(delta),  sin(delta),
                0, -sin(delta), cos(delta)};
  // T_elbowRot is the TRANSPOSED rotation matrix for the delta rotation
  float_Matrix UlnaShift(2,3,3);
  UlnaShift.InitFromFloats(UlnaShift_f);
  float_Matrix T_elbowRot(2,3,3);
  T_elbowRot.InitFromFloats(T_elbowRot_f);

// first we shift the FarmIP's so the origin is at the elbow
  float_Matrix ShiftedIP(2,3,3);
  ShiftedIP = FarmIP - UlnaShift;

// we rotate the shifted IPs by the elbow bend (delta rotation)
  float_Matrix Rot1FarmIP(2,3,3);
  taMath_float::mat_mult(&Rot1FarmIP, &ShiftedIP, &T_elbowRot);
/*
  String ruout;
  Rot1FarmIP.Print(ruout);
  taMisc::Info("rotated ulna before translation:\n", ruout);
*/

// now we set the origin at the shoulder
  float_Matrix ReshiftedIP(2,3,3);
  ReshiftedIP = Rot1FarmIP + UlnaShift;

// finally we apply the shoulder rotation
  float_Matrix RotFarmIP(2,3,3);
  taMath_float::mat_mult(&RotFarmIP, &ReshiftedIP, &RT);

  String ripout;
  RotFarmIP.Print(ripout);
  taMisc::Info("rotated ulna IPs:\n", ripout);

// next we obtain the distance between the rotated IPs and the original IPs
// this code is highly dependent on the muscle geometry
  taVector3f c1, c2, shoulderIP, humerIP, pv1, pv2, p3;
  for(int i=0; i<8; i++) { // the 8 shoulder to humerus muscles
    shoulderIP.x =  ShouldIP.FastElAsFloat(0,i);
    shoulderIP.y =  ShouldIP.FastElAsFloat(1,i);
    shoulderIP.z =  ShouldIP.FastElAsFloat(2,i);
    humerIP.x = RotArmIP.FastElAsFloat(0,i);
    humerIP.y = RotArmIP.FastElAsFloat(1,i);
    humerIP.z = RotArmIP.FastElAsFloat(2,i);
    pv1.x = p1.FastElAsFloat(0,i); pv2.x = p2.FastElAsFloat(0,i);
    pv1.y = p1.FastElAsFloat(1,i); pv2.y = p2.FastElAsFloat(1,i);
    pv1.z = p1.FastElAsFloat(2,i); pv2.z = p2.FastElAsFloat(2,i);

    if(bender(p3,shoulderIP,humerIP,pv1,pv2) && i!=2 && i!=3) {
      // if muscle wraps around bending line (except for muscles 3 and 4)
      c1 = shoulderIP - p3; c2 = p3 - humerIP;
      trgLen.Set(c1.Mag()+c2.Mag(),i);
    } else {
      c1 = shoulderIP - humerIP;
      trgLen.Set(c1.Mag(),i);
    }
  }
  // next muscle is the biceps, from shoulder to forearm
  c1.x = ShouldIP.FastElAsFloat(0,8) - RotFarmIP.FastElAsFloat(0,0);
  c1.y = ShouldIP.FastElAsFloat(1,8) - RotFarmIP.FastElAsFloat(1,0);
  c1.z = ShouldIP.FastElAsFloat(2,8) - RotFarmIP.FastElAsFloat(2,0);
  trgLen.Set(c1.Mag(),8);
  // the triceps and the brachialis connect from humerus to ulna
  for(int i=1; i<=2; i++) {
    c1.x = RotArmIP.FastElAsFloat(0,7+i) - RotFarmIP.FastElAsFloat(0,i);
    c1.y = RotArmIP.FastElAsFloat(1,7+i) - RotFarmIP.FastElAsFloat(1,i);
    c1.z = RotArmIP.FastElAsFloat(2,7+i) - RotFarmIP.FastElAsFloat(2,i);
    trgLen.Set(c1.Mag(),8+i);
  }

  String trLout;
  trgLen.Print(trLout);
  taMisc::Info("target lengths: ", trLout, "\n");
  return true;
}

bool VEArm::Lengths(float_Matrix &Len) {
  if(TestWarning(Len.count() != Nmusc, "","The matrix provided to Lengts() doesn't match the number of muscles \n"))
    return false;

  for(int i=0; i<Nmusc; i++) {
    Len.Set(muscles[i]->Length(),i);
  }
  return true;
}

bool VEArm::Speeds(float_Matrix &Vel) {
  if(TestWarning(Vel.count() != Nmusc, "","The matrix provided to Speeds() doesn't match the number of muscles \n"))
    return false;

  for(int i=0; i<Nmusc; i++) {
    Vel.Set(muscles[i]->Speed(),i);
  }
  return true;
}

bool VEArm::ApplyStim(float_Matrix stims, float_Matrix &forces) {
  if(TestWarning(stims.count() != Nmusc, "","The stimulus matrix doesn't match the number of muscles \n"))
    return false;
  else if(TestWarning(forces.count() != 3*Nmusc, "","The forces matrix doesn't match the number of muscles \n"))
    return false;

  VEBody* humerus = bodies[HUMERUS];
  VEBody* ulna = bodies[ULNA];

  taVector3f daforce(0.0f, 0.0f, 0.0f);
  for(int i=0; i<Nmusc; i++) {
    daforce = muscles[i]->Contract(stims.FastElAsFloat(i));
    forces.Set(daforce.x, i, 0);
    forces.Set(daforce.y, i, 1);
    forces.Set(daforce.z, i, 2);

    if(i < 8) {  // muscles from shoulder to humerus
      humerus->AddForceAtPos(daforce.x,daforce.y,daforce.z,muscles[i]->IPdist.x,muscles[i]->IPdist.y,muscles[i]->IPdist.z,false,false);
    } else if(i == 8) {  // biceps
      ulna->AddForceAtPos(daforce.x,daforce.y,daforce.z,muscles[i]->IPdist.x,muscles[i]->IPdist.y,muscles[i]->IPdist.z,false,false);
    } else { // triceps and brachialis
      humerus->AddForceAtPos(-daforce.x,-daforce.y,-daforce.z,muscles[i]->IPprox.x,muscles[i]->IPprox.y,muscles[i]->IPprox.z,false,false);
      ulna->AddForceAtPos(daforce.x,daforce.y,daforce.z,muscles[i]->IPdist.x,muscles[i]->IPdist.y,muscles[i]->IPdist.z,false,false);
    }

  }
  return true;
}


///////////////////////////////////////////////////////////////
//   Linear Muscle: exerts force proportional to its input.

void VELinearMuscle::Initialize() {
  gain = 1;
  IPprox.SetXYZ(0.0f,0.0f,0.0f);
  IPdist.SetXYZ(0.1f,0.1f,0.1f);
  p3.SetXYZ(0.0f,0.0f,0.0f);
  bend = false;
  OldLength2 = OldLength1 = Length();
}

void VELinearMuscle::Initialize(taVector3f prox, taVector3f dist, float MrG) {
  gain = MrG;
  IPprox = prox;
  IPdist = dist;
  p3.SetXYZ(0.0f,0.0f,0.0f);
  bend = false;
  OldLength2 = OldLength1 = Length();
}

void VELinearMuscle::Initialize(taVector3f prox, taVector3f dist, float MrG, taVector3f pp3, bool bending) {
  gain = MrG;
  IPprox = prox;
  IPdist = dist;
  p3 = pp3;
  bend = bending;
  OldLength2 = OldLength1 = Length();
}

void VELinearMuscle::Destroy() { }

VEArm* VELinearMuscle::GetArm() {
  return GET_MY_OWNER(VEArm); // somehow Randy's macro does the trick
}

taVector3f VELinearMuscle::Contract(float stim) {
  taVector3f force_vec;
  if(bend)
    force_vec = IPprox - p3;
  else
    force_vec = IPprox - IPdist;  // vector points from distal to proximal

  force_vec.MagNorm();  // force_vec has now magnitude = 1
  return force_vec*gain*MAX(stim,0);
}

float VELinearMuscle::Length() {
  if(bend) // if muscle wraps around bending line
    return (IPprox - p3).Mag() + (p3 - IPdist).Mag();
  else
    return (IPprox - IPdist).Mag();
}

float VELinearMuscle::Speed() {
  float length = Length();
  VEArm* army = GetArm();
  float step = army->WorldStep; // copy of VEWorld stepsize
  return (length - OldLength2)/(2*step);  // 3 point rule
}


////////////////////////////////////////////////
//      Object: collection of bodies and joints

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



/////////////////////////////////////////////
//              Group

void VEObject_Group::Init() {
  FOREACH_ELEM_IN_GROUP(VEObject, ob, *this) {
    ob->Init();
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




////////////////////////////////////////////////
//      Static bodies

void VEStatic::Initialize() {
  geom_id = NULL;
  flags = EULER_ROT;
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

void VEStatic::Init() {
  VE_last_ve_set_vals_to_ode = this;

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
  pos.x = ve_snap_val(pos.x, grid_size);
  pos.y = ve_snap_val(pos.y, grid_size);
  pos.z = ve_snap_val(pos.z, grid_size);
  DataChanged(DCR_ITEM_UPDATED); // update displays..
}

void VEStatic::Translate(float dx, float dy, float dz) {
  pos.x += dx;
  pos.y += dy;
  pos.z += dz;
  DataChanged(DCR_ITEM_UPDATED); // update displays..
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
  DataChanged(DCR_ITEM_UPDATED); // update displays..
}

void VEStatic::RotateAxis(float x_ax, float y_ax, float z_ax, float rt) {
  if(TestError((x_ax == 0.0f) && (y_ax == 0.0f) && (z_ax == 0.0f),
    "RotateBody", "must specify a non-zero axis!"))
    return;

  rot_quat.RotateAxis(x_ax, y_ax, z_ax, rt);
  rot = rot_quat;
  rot_quat.ToEulerVec(rot_euler);
  DataChanged(DCR_ITEM_UPDATED); // update displays..
}

void VEStatic::RotateEuler(float euler_x, float euler_y, float euler_z) {
  rot_quat.RotateEuler(euler_x, euler_y, euler_z);
  rot = rot_quat;
  rot_quat.ToEulerVec(rot_euler);
  DataChanged(DCR_ITEM_UPDATED); // update displays..
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


/////////////////////////////////////////////
//              HeightField

void VEHeightField::Initialize() {
  // todo: do this
}

void VEHeightField::Init() {
  TestError(true, "Init", "Sorry, HeightField is not yet implemented");
}

/////////////////////////////////////////////
//              Group

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

////////////////////////////////////////////////
//      Space: collection of static elements

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


/////////////////////////////////////////////
//              Group

void VESpace_Group::Init() {
  FOREACH_ELEM_IN_GROUP(VESpace, ob, *this) {
    ob->Init();
  }
}

void VESpace_Group::DestroyODE() {
  FOREACH_ELEM_IN_GROUP(VESpace, ob, *this) {
    ob->DestroyODE();
  }
}

void VESpace_Group::SnapPosToGrid(float grid_size) {
  FOREACH_ELEM_IN_GROUP(VESpace, ob, *this) {
    ob->SnapPosToGrid(grid_size);
  }
}

void VESpace_Group::Translate(float dx, float dy, float dz) {
  FOREACH_ELEM_IN_GROUP(VESpace, ob, *this) {
    ob->Translate(dx, dy, dz);
  }
}

void VESpace_Group::Scale(float sx, float sy, float sz) {
  FOREACH_ELEM_IN_GROUP(VESpace, ob, *this) {
    ob->Scale(sx, sy, sz);
  }
}

void VESpace_Group::RotateAxis(float x_ax, float y_ax, float z_ax, float rot) {
  FOREACH_ELEM_IN_GROUP(VESpace, ob, *this) {
    ob->RotateAxis(x_ax, y_ax, z_ax, rot);
  }
}

void VESpace_Group::RotateEuler(float euler_x, float euler_y, float euler_z) {
  FOREACH_ELEM_IN_GROUP(VESpace, ob, *this) {
    ob->RotateEuler(euler_x, euler_y, euler_z);
  }
}

void VESpace_Group::CopyColorFrom(VEStatic* cpy_fm) {
  FOREACH_ELEM_IN_GROUP(VESpace, ob, *this) {
    ob->CopyColorFrom(cpy_fm);
  }
}


///////////////////////////////////////////////////////////////
//      Error handling!!

#include <ode/error.h>

static VEWorld* VE_last_ve_stepped = NULL;

// this is the default message guy from ODE
static void VE_Err_printMessage (int num, const char *msg1, const char *msg2,
                                 va_list ap)
{
  fflush (stderr);
  fflush (stdout);
  if (num) fprintf (stderr,"\n%s %d: ",msg1,num);
  else fprintf (stderr,"\n%s: ",msg1);
  vfprintf (stderr,msg2,ap);
  fprintf (stderr,"\n");
  fflush (stderr);
}

extern "C" void VE_ErrorHandler(int errnum, const char* msg, va_list ap) {
  String step_nm = "n/a";
  String set_vals_nm = "n/a";
  if(VE_last_ve_stepped) {
    step_nm = VE_last_ve_stepped->name;
  }
  if(VE_last_ve_set_vals_to_ode) {
    set_vals_nm = VE_last_ve_set_vals_to_ode->GetName();
  }

  VE_Err_printMessage(errnum, "ODE Error", msg, ap); // provide ap stuff

  taMisc::Error("ODE Fatal Error (details on console) number:", String(errnum), msg,
                "last VEWorld Step'd:", step_nm,
                "last VE item doing Init:", set_vals_nm,
                "DO NOT ATTEMPT TO RUN VIRTUAL ENV without restarting");
}

extern "C" void VE_DebugHandler(int errnum, const char* msg, va_list ap) {
  String step_nm = "n/a";
  String set_vals_nm = "n/a";
  if(VE_last_ve_stepped) {
    step_nm = VE_last_ve_stepped->name;
  }
  if(VE_last_ve_set_vals_to_ode) {
    set_vals_nm = VE_last_ve_set_vals_to_ode->GetName();
  }

  VE_Err_printMessage(errnum, "ODE INTERNAL ERROR", msg, ap); // provide ap stuff

  taMisc::Error("ODE Debug Error (details on console) number:", String(errnum), msg,
                "last VEWorld Step'd:", step_nm,
                "last VE item doing Init:", set_vals_nm,
                "DO NOT ATTEMPT TO RUN VIRTUAL ENV without restarting");
}

extern "C" void VE_MessageHandler(int errnum, const char* msg, va_list ap) {
  String step_nm = "n/a";
  String set_vals_nm = "n/a";
  if(VE_last_ve_stepped) {
    step_nm = VE_last_ve_stepped->name;
  }
  if(VE_last_ve_set_vals_to_ode) {
    set_vals_nm = VE_last_ve_set_vals_to_ode->GetName();
  }

  VE_Err_printMessage(errnum, "ODE Message", msg, ap); // provide ap stuff

  taMisc::Warning("ODE Message number:", String(errnum), msg,
                "last VEWorld Step'd:", step_nm,
                "last VE item doing Init:", set_vals_nm);
}

///////////////////////////////////////////////////////////////
//      World!

void ODEWorldParams::Initialize() {
  max_cor_vel = 1.0e6f;
  contact_depth = 0.001f;
  max_col_pts = 4;
}

void ODEWorldParams::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  if(max_col_pts > 64) max_col_pts = 64;
  if(max_col_pts < 0) max_col_pts = 1;
}

void VEWorld::Initialize() {
  world_id = NULL;
  space_id = NULL;
  cgp_id = NULL;
  space_type = HASH_SPACE;
  cur_space_type = HASH_SPACE;
  hash_levels.min = -3;  hash_levels.max = 10;
  gravity.y = -9.81f;
  updt_display = true;
  step_type = STD_STEP;
  stepsize = .01f;
  quick_iters = 20;
  bg_color.no_a = true;
  bg_color.r = 0.8f;
  bg_color.g = 0.8f;
  bg_color.b = 0.8f;
}

void VEWorld::Destroy() {
  CutLinks();
}

void VEWorld::CutLinks() {
  objects.CutLinks();
  spaces.CutLinks();
  DestroyODE();                 // do this last!
  inherited::CutLinks();
}

void VEWorld::Copy_(const VEWorld& cp) {
  TA_VEWorld.CopyOnlySameType((void*)this, (void*)&cp);
  UpdatePointers_NewPar((taBase*)&cp, this); // update all the pointers
}

bool VEWorld::CreateODE() {
  if(!world_id)
    world_id = (dWorldID)dWorldCreate();
  if(TestError(!world_id, "CreateODE", "could not create world id!"))
    return false;
  if(space_id && space_type == cur_space_type) return true;
  if(space_id) {
    dSpaceDestroy((dSpaceID)space_id);
    space_id = NULL;
  }
  switch(space_type) {
  case SIMPLE_SPACE:
    space_id = (dSpaceID)dSimpleSpaceCreate(NULL);
    break;
  case HASH_SPACE:
    space_id = (dSpaceID)dHashSpaceCreate(NULL);
    break;
  }
  if(TestError(!space_id, "CreateODE", "could not create space id!"))
    return false;
  cur_space_type = space_type;

  if(!cgp_id)
    cgp_id = dJointGroupCreate(0); // 0 = max_size = not used
  if(TestError(!cgp_id, "CreateODE", "could not create contact group id!"))
    return false;

  dSetErrorHandler(VE_ErrorHandler);
  dSetDebugHandler(VE_DebugHandler);
  dSetMessageHandler(VE_MessageHandler);

  return true;
}

void VEWorld::DestroyODE() {
  objects.DestroyODE();
  spaces.DestroyODE();

  if(cgp_id) dJointGroupDestroy((dJointGroupID)cgp_id);
  cgp_id = NULL;
  if(space_id) dSpaceDestroy((dSpaceID)space_id);
  space_id = NULL;
  if(world_id) dWorldDestroy((dWorldID)world_id);
  world_id = NULL;
}

void VEWorld::Init() {
  VE_last_ve_set_vals_to_ode = this;

  if(!world_id || !space_id || space_type != cur_space_type || !cgp_id) CreateODE();
  if(!world_id || !space_id || !cgp_id) return;
  dWorldID wid = (dWorldID)world_id;
  dSpaceID sid = (dSpaceID)space_id;

  dWorldSetGravity(wid, gravity.x, gravity.y, gravity.z);
  dWorldSetERP(wid, ode_params.erp);
  dWorldSetCFM(wid, ode_params.cfm);
  dWorldSetContactMaxCorrectingVel(wid, ode_params.max_cor_vel);
  dWorldSetContactSurfaceLayer(wid, ode_params.contact_depth);

  if(space_type == HASH_SPACE) {
    dHashSpaceSetLevels(sid, hash_levels.min, hash_levels.max);
  }

  StructUpdate(true);
  objects.Init();
  spaces.Init();
  StructUpdate(false);          // trigger full rebuild!

  VE_last_ve_set_vals_to_ode = NULL; // turn off!
}

void VEWorld::CurToODE() {
  objects.CurToODE();
}

void VEWorld::CurFromODE() {
  bool updt_disp = updt_display;
  if(!taMisc::gui_active)
    updt_disp = false;
  objects.CurFromODE();
  if(updt_disp)
    DataChanged(DCR_ITEM_UPDATED); // update displays..

  if(updt_display && taMisc::gui_no_win) {
    // normal data changed stuff doesn't work in gui_no_win, so we use a manual call
    // only if cameras are set
    if((bool)camera_0 || (bool)camera_1) {
      UpdateView();
    }
  }
  // not really nec
  // void dWorldGetGravity (dWorldID, dVector3 gravity);
}

void VEWorld::CurToInit() {
  objects.CurToInit();
}

void VEWorld::SnapPosToGrid(float grid_size, bool init_pos) {
  objects.SnapPosToGrid(grid_size, init_pos);
  spaces.SnapPosToGrid(grid_size);
}

void VEWorld::CollisionCallback(dGeomID o1, dGeomID o2) {
  static dContactGeom cgs[64];
  dWorldID wid = (dWorldID)world_id;
  dJointGroupID gid = (dJointGroupID)cgp_id;
  int num_contact = dCollide(o1,o2, ode_params.max_col_pts, cgs, sizeof(dContactGeom));
  // add these contact points to the simulation ...
  VEBody* b1 = (VEBody*)dGeomGetData(o1);
  VEBody* b2 = (VEBody*)dGeomGetData(o2);
  dContact cont;
  cont.surface.mode = dContactBounce | dContactSoftERP | dContactSoftCFM;
  cont.surface.mu = .5f * (b1->surface.friction + b2->surface.friction);
  cont.surface.bounce = .5f * (b1->surface.bounce + b2->surface.bounce);
  cont.surface.bounce_vel = .5f * (b1->surface.bounce_vel + b2->surface.bounce_vel);
  cont.surface.soft_erp = .5f * (b1->softness.erp + b2->softness.erp);
  cont.surface.soft_cfm = .5f * (b1->softness.cfm + b2->softness.cfm);
  // todo: not seting slip1 or second directions (as in tires)
  if(num_contact > 0) {
    for(int i=0;i<num_contact;i++) {
      cont.geom = cgs[i];
      dJointID c = dJointCreateContact(wid, gid, &cont);
      dJointAttach(c, dGeomGetBody(cont.geom.g1), dGeomGetBody(cont.geom.g2));
    }
  }
}

void nearCallback(void *data, dGeomID o1, dGeomID o2) {
  if (dGeomIsSpace (o1) || dGeomIsSpace (o2)) { // colliding a space with something
    dSpaceCollide2 (o1,o2,data,&nearCallback); // collide all geoms internal to the space(s)
    if (dGeomIsSpace (o1)) dSpaceCollide ((dSpaceID)o1,data,&nearCallback);
    if (dGeomIsSpace (o2)) dSpaceCollide ((dSpaceID)o2,data,&nearCallback);
  } else {
    ((VEWorld*)data)->CollisionCallback(o1, o2);
  }
}

void VEWorld::Step() {
  if(!world_id || !space_id || !cgp_id) return;

  VE_last_ve_stepped = this;

  dWorldID wid = (dWorldID)world_id;
  dSpaceID sid = (dSpaceID)space_id;
  dJointGroupID gid = (dJointGroupID)cgp_id;

  dSpaceCollide(sid, (void*)this, &nearCallback);

  if(step_type == STD_STEP) {
    dWorldStep(wid, stepsize);
  }
  else {
    dWorldQuickStep(wid, stepsize);
  }

  dJointGroupEmpty(gid);

  CurFromODE();

  VE_last_ve_stepped = NULL;
}

// in ta_virtenv_qtso.cpp:  QImage VEWorld::GetCameraImage(int cam_no)

