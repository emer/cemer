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
//              bodies (ridid object elements)


#include <Inventor/SbLinear.h>

void VEBody::Initialize() {
  body_id = NULL;
  geom_id = NULL;
  flags = (BodyFlags)(GRAVITY_ON | EULER_ROT);
  shape = CAPSULE;
  cur_shape = BOX;
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

  CurToODE();		// always update ODE with any changes!
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
  taQuaternion eff_quat = init_quat;
  if(shape == CAPSULE || shape == CYLINDER) {
    if(long_axis == LONG_X) {
      eff_quat.RotateAxis(0.0f, 1.0f, 0.0f, -1.5708f);
    }
    else if(long_axis == LONG_Y) {
      eff_quat.RotateAxis(1.0f, 0.0f, 0.0f, -1.5708f);
    }
  }

  cur_quat = eff_quat;
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
  UpdateAfterEdit();		// calls CurToODE and updates display
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
  UpdateAfterEdit();		// calls CurToODE and updates display
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
  UpdateAfterEdit();		// calls CurToODE and updates display
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
  flags = JF_NONE;
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
  Init();		// always update ODE with any changes!
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

