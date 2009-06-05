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
//	parameters

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

////////////////////////////////////////////////
//		surfaces and textures

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
//		bodies (ridid object elements)


#include <Inventor/SbLinear.h>

void VEBody::Initialize() {
  body_id = NULL;
  geom_id = NULL;
  flags = BF_NONE;
  shape = CAPSULE;
  cur_shape = BOX;
  mass = 1.0f;
  radius = .2f;
  length = 1.0f;
  long_axis = LONG_X;
  box = 1.0f;
  set_color = true;
  color.Set(0.2f, 0.2f, .5f, .5f);	// transparent blue.. why not..
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

void VEBody::SetValsToODE() {
  VE_last_ve_set_vals_to_ode = this;

  if(HasBodyFlag(VEBody::OFF)) {
    DestroyODE();
    return;
  }

  if(!body_id) CreateODE();
  if(!body_id) return;
  dWorldID wid = (dWorldID)GetWorldID();
  if(TestError(!wid, "SetValsToODE", "no valid world id -- cannot create stuff!"))
    return;

  SetValsToODE_Shape();
  SetValsToODE_InitPos();
  SetValsToODE_Rotation();
  SetValsToODE_Velocity();
  SetValsToODE_Mass();
}

void VEBody::SetValsToODE_Shape() {
  dSpaceID sid = (dSpaceID)GetObjSpaceID();
  if(TestError(!sid, "CreateODE", "no valid space id -- cannot create body geom!"))
    return;

  if(geom_id) dGeomDestroy((dGeomID)geom_id);
  geom_id = NULL;

  if(shape == NO_SHAPE || HasBodyFlag(NO_COLLIDE)) {
    cur_shape = shape;
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
  case NO_SHAPE:		// compiler food
    break;
  }
  
  if(TestError(!geom_id, "CreateODE", "could not create body geom!"))
    return;

  cur_shape = shape;

  dGeomSetBody((dGeomID)geom_id, (dBodyID)body_id);
  dGeomSetData((dGeomID)geom_id, (void*)this);
}

void VEBody::SetValsToODE_InitPos() {
  dBodyID bid = (dBodyID)body_id;
  cur_pos = init_pos;
  dBodySetPosition(bid, init_pos.x, init_pos.y, init_pos.z);


  if(HasBodyFlag(FIXED)) {
    if(!fixed_joint_id) {
      dWorldID wid = (dWorldID)GetWorldID();
      if(TestError(!wid, "SetValsToODE", "no valid world id -- cannot create stuff!"))
	return;
      fixed_joint_id = dJointCreateFixed(wid, 0);
    }
    dJointAttach((dJointID)fixed_joint_id, bid, 0);	// 0 = attach to static object
  }
  else if(fixed_joint_id) {
    dJointDestroy((dJointID)fixed_joint_id);
    fixed_joint_id = NULL;
  }
}

void VEBody::SetValsToODE_Rotation() {
  dBodyID bid = (dBodyID)body_id;
  cur_rot = init_rot;

  // capsules and cylinders need to have extra rotation as they are always Z axis oriented!
  dMatrix3 R;
  if(shape == CAPSULE || shape == CYLINDER) {
    SbRotation irot;
    irot.setValue(SbVec3f(init_rot.x, init_rot.y, init_rot.z), init_rot.rot);
    if(long_axis == LONG_X) {
      SbRotation sbrot;
      sbrot.setValue(SbVec3f(0.0f, 1.0f, 0.0f), 1.5708f);
      irot *= sbrot;
    }
    else if(long_axis == LONG_Y) {
      SbRotation sbrot;
      sbrot.setValue(SbVec3f(1.0f, 0.0f, 0.0f), 1.5708f);
      irot *= sbrot;
    }
    SbVec3f rot_ax;    float rangle;
    irot.getValue(rot_ax, rangle);
    dRFromAxisAndAngle(R, rot_ax[0], rot_ax[1], rot_ax[2], rangle);
  }
  else {
    dRFromAxisAndAngle(R, init_rot.x, init_rot.y, init_rot.z, init_rot.rot);
  }
  dBodySetRotation(bid, R);
}

void VEBody::SetValsToODE_Velocity() {
  dBodyID bid = (dBodyID)body_id;

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

void VEBody::SetValsToODE_Mass() {
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

void VEBody::GetValsFmODE(bool updt_disp) {
  if(HasBodyFlag(VEBody::OFF)) {
    return;
  }
  if(!body_id) CreateODE();
  if(!body_id) return;
  dBodyID bid = (dBodyID)body_id;
  const dReal* opos = dBodyGetPosition(bid);
  cur_pos.x = opos[0]; cur_pos.y = opos[1]; cur_pos.z = opos[2];

  // ODE quaternion = w,x,y,z; Inventor = x,y,z,w
  const dReal* quat = dBodyGetQuaternion(bid);
  SbRotation sbrot;
  sbrot.setValue(quat[1], quat[2], quat[3], quat[0]);

  // capsules and cylinders need to have extra rotation as they are always Z axis oriented: undo!
  if(shape == CAPSULE || shape == CYLINDER) {
    if(long_axis == LONG_X) {
      SbVec3f yaxis(0.0f, 1.0f, 0.0f);
      SbVec3f new_yaxis;
      sbrot.multVec(yaxis, new_yaxis);
      SbRotation ubrot;
      ubrot.setValue(new_yaxis, -1.5708f); // undo along the new yaxis, not the original one..
      sbrot *= ubrot;
    }
    else if(long_axis == LONG_Y) {
      SbVec3f xaxis(1.0f, 0.0f, 0.0f);
      SbVec3f new_xaxis;
      sbrot.multVec(xaxis, new_xaxis);
      SbRotation ubrot;
      ubrot.setValue(new_xaxis, -1.5708f); // undo along the new xaxis, not the original one..
      sbrot *= ubrot;
    }
  }
  SbVec3f rot_ax;
  sbrot.getValue(rot_ax, cur_rot.rot);
  cur_rot.x = rot_ax[0]; cur_rot.y = rot_ax[1]; cur_rot.z = rot_ax[2];

  const dReal* olv = dBodyGetLinearVel(bid);
  cur_lin_vel.x = olv[0]; cur_lin_vel.y = olv[1]; cur_lin_vel.z = olv[2];
  const dReal* oav = dBodyGetAngularVel(bid);
  cur_ang_vel.x = oav[0]; cur_ang_vel.y = oav[1]; cur_ang_vel.z = oav[2];

  if(updt_disp)
    DataChanged(DCR_ITEM_UPDATED); // update displays..
  // not necc (nonrelativistic!)
  //  dBodyGetMass(bid, &mass);
}

/////////////////////////////////////////////
//		Group

void VEBody_Group::SetValsToODE() {
  VEBody* ob;
  taLeafItr i;
  FOR_ITR_EL(VEBody, ob, this->, i) {
    ob->SetValsToODE();
  }
}

void VEBody_Group::GetValsFmODE(bool updt_disp) {
  VEBody* ob;
  taLeafItr i;
  FOR_ITR_EL(VEBody, ob, this->, i) {
    ob->GetValsFmODE(updt_disp);
  }
}

void VEBody_Group::DestroyODE() {
  VEBody* ob;
  taLeafItr i;
  FOR_ITR_EL(VEBody, ob, this->, i) {
    ob->DestroyODE();
  }
}

/////////////////////////////////////////////
//		Camera and Lights

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
  antialias_scale = 2;
  light.intensity = .2f;		// keep it not so bright relative to the sun..
  mass = .01f;
  radius = .01f;
  length = .01f;
  shape = CYLINDER;
  long_axis = LONG_Z;		// default orientation: must remain!
  dir_norm.x = 0.0f; dir_norm.y = 0.0f; dir_norm.z = -1.0f;
}

void VECamera::SetValsToODE() {
  inherited::SetValsToODE();
  SbVec3f dn(0.0f, 0.0f, -1.0f);
  SbRotation sbrot(SbVec3f(cur_rot.x, cur_rot.y, cur_rot.z), cur_rot.rot);
  sbrot.multVec(dn, dn);
  dir_norm.x = dn[0]; dir_norm.y = dn[1]; dir_norm.z = dn[2];
}

void VECamera::GetValsFmODE(bool updt_disp) {
  inherited::GetValsFmODE(updt_disp);
  SbVec3f dn(0.0f, 0.0f, -1.0f);
  SbRotation sbrot(SbVec3f(cur_rot.x, cur_rot.y, cur_rot.z), cur_rot.rot);
  sbrot.multVec(dn, dn);
  dir_norm.x = dn[0]; dir_norm.y = dn[1]; dir_norm.z = dn[2];
}

// in ta_virtenv_qtso.cpp:  void VECamera::ConfigCamera(SoPerspectiveCamera* cam)

///////////////////////////////////////////////////
//		Lights

void VELight::Initialize() {
  light_type = SPOT_LIGHT;
  drop_off_rate = 0.0f;
  cut_off_angle = 45.0f;
  dir_norm.x = 0.0f; dir_norm.y = 0.0f; dir_norm.z = -1.0f;
  shape = NO_SHAPE;		// having a shape will interfere with light!  but might want to see where it is sometimes..
  long_axis = LONG_Z;
  mass = .01f;
  radius = .01f;
  length = .01f;
}

// in ta_virtenv_qtso.cpp: SoLight* VELight::CreateLight()

// in ta_virtenv_qtso.cpp: void VELight::ConfigLight(SoLight* lgt)

void VELight::SetValsToODE() {
  inherited::SetValsToODE();
  SbVec3f dn(0.0f, 0.0f, -1.0f);
  SbRotation sbrot(SbVec3f(cur_rot.x, cur_rot.y, cur_rot.z), cur_rot.rot);
  sbrot.multVec(dn, dn);
  dir_norm.x = dn[0]; dir_norm.y = dn[1]; dir_norm.z = dn[2];
}

void VELight::GetValsFmODE(bool updt_disp) {
  inherited::GetValsFmODE(updt_disp);
  SbVec3f dn(0.0f, 0.0f, -1.0f);
  SbRotation sbrot(SbVec3f(cur_rot.x, cur_rot.y, cur_rot.z), cur_rot.rot);
  sbrot.multVec(dn, dn);
  dir_norm.x = dn[0]; dir_norm.y = dn[1]; dir_norm.z = dn[2];
}

///////////////////////////////////////////////////
//		VEObjCarousel

void VEObjCarousel::Initialize() {
  cur_obj_no = -1;
}

bool VEObjCarousel::ViewObjNo(int obj_no) {
  if(TestError(!(bool)obj_table, "ViewObjNo", "no obj_table data table set -- must set this first!"))
    return false;
  if(TestError(obj_no >= obj_table->rows, "ViewObjNo", "obj_no out of range -- only:",
	       String(obj_table->rows), "rows in the obj_table data table"))
    return false;
  cur_obj_no = obj_no;
  cur_obj_name = obj_table->GetVal("FileName", cur_obj_no);
  DataChanged(DCR_ITEM_UPDATED); // update displays..
  return true;
}

bool VEObjCarousel::ViewObjName(const String& obj_nm) {
  if(TestError(!(bool)obj_table, "ViewObjName", "no obj_table data table set -- must set this first!"))
    return false;
  int obj_no = obj_table->FindVal(obj_nm, "FileName");
  if(TestError(obj_no < 0, "ViewObjName", "object file named:", obj_nm,
	       "not found in the obj_table data table"))
    return false;
  cur_obj_no = obj_no;
  cur_obj_name = obj_nm;
  DataChanged(DCR_ITEM_UPDATED); // update displays..
  return true;
}


////////////////////////////////////////////////
//		Joints

void VEJointStops::Initialize() {
  stops_on = false;
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

void VEJoint::SetValsToODE() {
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

  if(HasJointFlag(OFF) || body1->HasBodyFlag(VEBody::OFF) || body2->HasBodyFlag(VEBody::OFF)) {
    DestroyODE();
    return;
  }

  if(TestError(!body1 || !body1->body_id,
	       "SetValsToODE", "body1 of joint MUST be specified and already exist!"))
    return;
  if(TestError(!body2 || !body2->body_id,
	       "SetValsToODE", "body2 of joint MUST be specified and already exist -- use fixed field on body to set fixed bodies!"))
    return;

  dJointAttach(jid, (dBodyID)body1->body_id, (dBodyID)body2->body_id);

  SetValsToODE_Anchor();
  SetValsToODE_Stops();
  SetValsToODE_Motor();
  SetValsToODE_ODEParams();

  if(HasJointFlag(FEEDBACK)) {
    dJointSetFeedback(jid, &ode_fdbk_obj);
  }
}

void VEJoint::SetValsToODE_Anchor() {
  dJointID jid = (dJointID)joint_id;
  FloatTDCoord wanchor = body1->init_pos + anchor; // world anchor offset from body1 position

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

void VEJoint::SetValsToODE_Stops() {
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

void VEJoint::SetValsToODE_Motor() {
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

void VEJoint::SetValsToODE_ODEParams() {
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

// #ifdef 0 // __GNUC__
// static inline float get_val_no_nan(float val) {
//   // note: this may not be sufficiently cross-platform (e.g., windows)
//   // could potentially try this: if(val != val) return 0.0f;  // supposed to be true of nans
//   if(isnan(val)) return 0.0f;
//   return val;
// }
// #else
static inline float get_val_no_nan(float val) {
  return val;
}
// #endif

void VEJoint::GetValsFmODE(bool updt_disp) {
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
    SetValsToODE_Motor();	// update the motor each step..
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

  SetValsToODE_Motor();
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

  SetValsToODE_Motor();
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
//		Group

void VEJoint_Group::SetValsToODE() {
  VEJoint* ob;
  taLeafItr i;
  FOR_ITR_EL(VEJoint, ob, this->, i) {
    ob->SetValsToODE();
  }
}

void VEJoint_Group::GetValsFmODE(bool updt_disp) {
  VEJoint* ob;
  taLeafItr i;
  FOR_ITR_EL(VEJoint, ob, this->, i) {
    ob->GetValsFmODE(updt_disp);
  }
}

void VEJoint_Group::DestroyODE() {
  VEJoint* ob;
  taLeafItr i;
  FOR_ITR_EL(VEJoint, ob, this->, i) {
    ob->DestroyODE();
  }
}

////////////////////////////////////////////////
//	Special VE stuff for robotic arm sims

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

  Compute_Lambda();		// get lambda from params

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
    muscle_obj->SetValsToODE();
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
    float del_len = len_buf.CircSafeEl(reflex_delay-1);
    float del_dlen = dlen_buf.CircSafeEl(reflex_delay-1);
    act = (del_len - lambda) + vel_damp * del_dlen;
    if(act < 0.0f) act = 0.0f;
  }
  m_act_force = m_mag * (expf(m_rec_grad * act) - 1.0f);
  m_force += ca_dt_cmp * (m_act_force - m_force); // first order low-pass filter, not 2nd order 
  force = extra_force + m_force * (fv1 + fv2 * atanf(fv3 + fv4 * dlen)) + passive_k * (len - rest_len);
  torque = force * moment_arm;	// assume constant moment arm: could compute based on geom.

  if(muscle_obj) {
    muscle_obj->length = len;
    muscle_obj->SetValsToODE();
    muscle_obj->UpdateAfterEdit(); // update display
  }
}

void VELambdaMuscle::SetTargAngle(float targ_norm_angle, float co_contract) {
  co_contract_pct = co_contract;
  lambda = LenFmAngle(targ_norm_angle);
  lambda = len_range.Clip(lambda);		// keep in range
  lambda_norm = len_range.Normalize(lambda); 	// this is still key command
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
//	VEMuscleJoint

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
  stops.stops_on = true;	// need stops
}

void VEMuscleJoint::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  if(joint_type == BALL || joint_type == NO_JOINT) {
    taMisc::Warning("cannot use BALL or NO_JOINT joint_types for muscle joint -- setting to HINGE");
    joint_type = HINGE;
  }

  SetJointFlag(FEEDBACK);	// definitely need this
  if(HasTwoAxes()) {
    motor2.motor_on = true;
    stops2.stops_on = true;
  }
  else {
    motor2.motor_on = false;
    stops2.stops_on = false;
  }
}

void VEMuscleJoint::SetValsToODE() {
  inherited::SetValsToODE();

  VEWorld* wld = GetWorld();
  float step_sz = wld->stepsize;

  float rest_norm_angle = stops.Normalize(stops.def); // def = rest
  float init_norm_angle = stops.Normalize(pos);	      // pos = cur position/angle

  targ_norm_angle = rest_norm_angle;
  targ_angle = stops.def;
  pos_norm = init_norm_angle;

  extensor.Init(step_sz, rest_norm_angle, init_norm_angle, co_contract_pct);
  flexor.Init(step_sz, rest_norm_angle, init_norm_angle, co_contract_pct);

  if(HasTwoAxes()) {
    float rest_norm_angle2 = stops2.Normalize(stops2.def); // def = rest
    float init_norm_angle2 = stops2.Normalize(pos2);	      // pos = cur position/angle

    targ_norm_angle2 = rest_norm_angle2;
    targ_angle2 = stops2.def;
    pos2_norm = init_norm_angle2;

    extensor2.Init(step_sz, rest_norm_angle2, init_norm_angle2, co_contract_pct);
    flexor2.Init(step_sz, rest_norm_angle2, init_norm_angle2, co_contract_pct);
  }
}

void VEMuscleJoint::GetValsFmODE(bool updt_disp) {
  inherited::GetValsFmODE(updt_disp);

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
//	Object: collection of bodies and joints

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
  bodies.DestroyODE();	// bodies first!
  joints.DestroyODE();
  if(space_id) dSpaceDestroy((dSpaceID)space_id);
  space_id = NULL;
}

void VEObject::SetValsToODE() {
  if(!space_id || space_type != cur_space_type) CreateODE();
  if(!space_id) return;
  dSpaceID sid = (dSpaceID)space_id;
  if(space_type == HASH_SPACE) {
    dHashSpaceSetLevels(sid, hash_levels.min, hash_levels.max);
  }
  bodies.SetValsToODE();	// bodies first!
  joints.SetValsToODE();
}

void VEObject::GetValsFmODE(bool updt_disp) {
  bodies.GetValsFmODE(updt_disp);	// bodies first!
  joints.GetValsFmODE(updt_disp);
}

/////////////////////////////////////////////
//		Group

void VEObject_Group::SetValsToODE() {
  VEObject* ob;
  taLeafItr i;
  FOR_ITR_EL(VEObject, ob, this->, i) {
    ob->SetValsToODE();
  }
}

void VEObject_Group::GetValsFmODE(bool updt_disp) {
  VEObject* ob;
  taLeafItr i;
  FOR_ITR_EL(VEObject, ob, this->, i) {
    ob->GetValsFmODE(updt_disp);
  }
}

void VEObject_Group::DestroyODE() {
  VEObject* ob;
  taLeafItr i;
  FOR_ITR_EL(VEObject, ob, this->, i) {
    ob->DestroyODE();
  }
}



////////////////////////////////////////////////
//	Static bodies

void VEStatic::Initialize() {
  geom_id = NULL;
  flags = SF_NONE;
  shape = BOX;
  cur_shape = BOX;
  radius = .2f;
  length = 1.0f;
  box = 1.0f;
  plane_norm = NORM_Y;
  plane_height = 0.0f;
  plane_vis_size = 100.0f;
  set_color = true;
  color.Set(0.4f, 0.3f, .1f, 1.0f);	// brownish..
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

  SetValsToODE_Shape();
  return true;
}

void VEStatic::DestroyODE() {
  if(geom_id) dGeomDestroy((dGeomID)geom_id);
  geom_id = NULL;
}

void VEStatic::SetValsToODE() {
  VE_last_ve_set_vals_to_ode = this;

  if(HasStaticFlag(VEStatic::OFF)) {
    DestroyODE();
    return;
  }
  dWorldID wid = (dWorldID)GetWorldID();
  if(TestError(!wid, "SetValsToODE", "no valid world id -- cannot create stuff!"))
    return;

  SetValsToODE_Shape();
  SetValsToODE_PosRot();
}

void VEStatic::SetValsToODE_Shape() {
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

  dGeomSetData((dGeomID)geom_id, (void*)this);
}

void VEStatic::SetValsToODE_PosRot() {
  dGeomID gid = (dGeomID)geom_id;

  if(shape != PLANE) {
    dGeomSetPosition(gid, pos.x, pos.y, pos.z);
    dMatrix3 R;
    dRFromAxisAndAngle(R, rot.x, rot.y, rot.z, rot.rot);
    dGeomSetRotation(gid, R);
  }
}

/////////////////////////////////////////////
//		HeightField

void VEHeightField::Initialize() {
  // todo: do this
}

void VEHeightField::SetValsToODE() {
  TestError(true, "SetValsToODE", "Sorry, HeightField is not yet implemented -- soon!");
}

/////////////////////////////////////////////
//		Group

void VEStatic_Group::SetValsToODE() {
  VEStatic* ob;
  taLeafItr i;
  FOR_ITR_EL(VEStatic, ob, this->, i) {
    ob->SetValsToODE();
  }
}

void VEStatic_Group::DestroyODE() {
  VEStatic* ob;
  taLeafItr i;
  FOR_ITR_EL(VEStatic, ob, this->, i) {
    ob->DestroyODE();
  }
}

////////////////////////////////////////////////
//	Space: collection of static elements

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

void VESpace::SetValsToODE() {
  if(!space_id || space_type != cur_space_type) CreateODE();
  if(!space_id) return;
  dSpaceID sid = (dSpaceID)space_id;
  if(space_type == HASH_SPACE) {
    dHashSpaceSetLevels(sid, hash_levels.min, hash_levels.max);
  }
  static_els.SetValsToODE();
}

/////////////////////////////////////////////
//		Group

void VESpace_Group::SetValsToODE() {
  VESpace* ob;
  taLeafItr i;
  FOR_ITR_EL(VESpace, ob, this->, i) {
    ob->SetValsToODE();
  }
}

void VESpace_Group::DestroyODE() {
  VESpace* ob;
  taLeafItr i;
  FOR_ITR_EL(VESpace, ob, this->, i) {
    ob->DestroyODE();
  }
}

///////////////////////////////////////////////////////////////
//	Error handling!!

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
		"last VE item doing SetValsToODE:", set_vals_nm,
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
		"last VE item doing SetValsToODE:", set_vals_nm,
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
		"last VE item doing SetValsToODE:", set_vals_nm);
}

///////////////////////////////////////////////////////////////
//	World!

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
  DestroyODE();			// do this last!
  inherited::CutLinks();
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

void VEWorld::SetValsToODE() {
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
  objects.SetValsToODE();
  spaces.SetValsToODE();
  StructUpdate(false);		// trigger full rebuild!

  VE_last_ve_set_vals_to_ode = NULL; // turn off!
}

void VEWorld::GetValsFmODE() {
  bool updt_disp = updt_display;
  if(!taMisc::gui_active)
    updt_disp = false;
  objects.GetValsFmODE();
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
    //    cerr << num_contact << " contacts for objs: " << b1->name << " " << b2->name << endl;
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

  GetValsFmODE();

  VE_last_ve_stepped = NULL;
}

// in ta_virtenv_qtso.cpp:  QImage VEWorld::GetCameraImage(int cam_no)

