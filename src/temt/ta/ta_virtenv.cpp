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

  if(body_id && geom_id && cur_shape == shape && ((bool)fixed_joint_id == HasBodyFlag(FIXED)))
     return true;
  dWorldID wid = (dWorldID)GetWorldID();
  if(TestError(!wid, "CreateODE", "no valid world id -- cannot create body!"))
    return false;

  if(!body_id)
    body_id = (dBodyID)dBodyCreate(wid);
  if(TestError(!body_id, "CreateODE", "could not create body!"))
    return false;

  dBodyID bid = (dBodyID)body_id;
  if(HasBodyFlag(FIXED)) {
    if(!fixed_joint_id) {
      fixed_joint_id = dJointCreateFixed(wid, 0);
    }
    dJointAttach((dJointID)fixed_joint_id, bid, 0);	// 0 = attach to static object
  }
  else if(fixed_joint_id) {
    dJointDestroy((dJointID)fixed_joint_id);
    fixed_joint_id = NULL;
  }

  dSpaceID sid = (dSpaceID)GetObjSpaceID();
  if(TestError(!sid, "CreateODE", "no valid space id -- cannot create body geom!"))
    return false;

  if(!geom_id || cur_shape != shape) {
    if(geom_id) dGeomDestroy((dGeomID)geom_id);
    geom_id = NULL;
    switch(shape) {
    case SPHERE:
      geom_id = dCreateSphere(sid, radius);
      break;
    case CAPSULE:
      geom_id = dCreateCapsule(sid, radius, MAX(0.001f,length-2.0f*radius));
      break;
    case CYLINDER:
      geom_id = dCreateCylinder(sid, radius, length);
      break;
    case BOX:
      geom_id = dCreateBox(sid, box.x, box.y, box.z);
      break;
    case NO_SHAPE:
      break;
    }
  }
  
  if(TestError(!geom_id, "CreateODE", "could not create body geom!"))
    return false;

  cur_shape = shape;

  dGeomSetBody((dGeomID)geom_id, (dBodyID)body_id);
  dGeomSetData((dGeomID)geom_id, (void*)this);

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
  if(HasBodyFlag(VEBody::OFF)) {
    return;
  }
  if(!body_id || !geom_id || cur_shape != shape) CreateODE();
  if(!body_id) return;
  dWorldID wid = (dWorldID)GetWorldID();
  if(TestError(!wid, "SetValsToODE", "no valid world id -- cannot create stuff!"))
    return;

  cur_pos = init_pos;
  cur_rot = init_rot;

  dBodyID bid = (dBodyID)body_id;
  dBodySetPosition(bid, init_pos.x, init_pos.y, init_pos.z);

  dMatrix3 R;
  dRFromAxisAndAngle(R, init_rot.x, init_rot.y, init_rot.z, init_rot.rot);
  dBodySetRotation(bid, R);

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

  SetMassToODE();
}

void VEBody::SetMassToODE() {
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
    //    dMassSetCappedCylinderTotal(&mass_ode, mass, long_axis, radius, length);
    dMassSetCapsuleTotal(&mass_ode, mass, long_axis, radius, MAX(0.001f,length-2.0*radius));
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
  color = true;
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

////////////////////////////////////////////////
//		Joints

void VEJointStops::Initialize() {
  lo = -3.1415f;
  hi = 3.1415f;
  bounce = 0.0f;
}

void VEJointMotor::Initialize() {
  vel = 0.0f;
  f_max = 0.0f;
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
  axis2.x = 1.0f;
  pos = vel = pos2 = vel2 = 0.0f;
}

void VEJoint::Destroy() {
  CutLinks();
}

void VEJoint::CutLinks() {
  DestroyODE();
  inherited::CutLinks();
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
  if(!joint_id || joint_type != cur_type) CreateODE();
  if(!joint_id) return;
  dJointID jid = (dJointID)joint_id;

  // reset probe vals!
  pos = 0.0f;
  vel = 0.0f;
  pos2 = 0.0f;
  vel2 = 0.0f;
  cur_force1 = 0.0f;
  cur_force2 = 0.0f;
  cur_torque1 = 0.0f;
  cur_torque2 = 0.0f;

  if(TestError(!body1 || !body1->body_id,
	       "SetValsToODE", "body1 of joint MUST be specified and already exist!"))
    return;
  if(TestError(!body2 || !body2->body_id,
	       "SetValsToODE", "body2 of joint MUST be specified and already exist -- use fixed field on body to set fixed bodies!"))
    return;

  if(body1->HasBodyFlag(VEBody::OFF) || body2->HasBodyFlag(VEBody::OFF)) return;

  dJointAttach(jid, (dBodyID)body1->body_id, (dBodyID)body2->body_id);

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

  if(HasJointFlag(USE_STOPS)) {
    switch(joint_type) {
    case HINGE:
      dJointSetHingeParam(jid, dParamLoStop, stops.lo);
      dJointSetHingeParam(jid, dParamHiStop, stops.hi);
      dJointSetHingeParam(jid, dParamBounce, stops.bounce);
      break;
    case SLIDER:
      dJointSetSliderParam(jid, dParamLoStop, stops.lo);
      dJointSetSliderParam(jid, dParamHiStop, stops.hi);
      dJointSetSliderParam(jid, dParamBounce, stops.bounce);
      break;
    case UNIVERSAL:
      dJointSetUniversalParam(jid, dParamLoStop, stops.lo);
      dJointSetUniversalParam(jid, dParamHiStop, stops.hi);
      dJointSetUniversalParam(jid, dParamBounce, stops.bounce);
      dJointSetUniversalParam(jid, dParamLoStop2, stops2.lo);
      dJointSetUniversalParam(jid, dParamHiStop2, stops2.hi);
      dJointSetUniversalParam(jid, dParamBounce2, stops2.bounce);
      break;
    case HINGE2:
      dJointSetHinge2Param(jid, dParamLoStop, stops.lo);
      dJointSetHinge2Param(jid, dParamHiStop, stops.hi);
      dJointSetHinge2Param(jid, dParamBounce, stops.bounce);
      dJointSetHinge2Param(jid, dParamLoStop2, stops2.lo);
      dJointSetHinge2Param(jid, dParamHiStop2, stops2.hi);
      dJointSetHinge2Param(jid, dParamBounce2, stops2.bounce);
      break;
    case FIXED:
      break;
    case BALL:
      break;
    case NO_JOINT:
      break;
    }
  }

  if(joint_type == HINGE2) {
    dJointSetHinge2Param(jid, dParamSuspensionERP, suspension.erp);
    dJointSetHinge2Param(jid, dParamSuspensionCFM, suspension.cfm);
  }

  if(HasJointFlag(USE_MOTOR)) {
    switch(joint_type) {
    case HINGE:
      dJointSetHingeParam(jid, dParamVel, motor.vel);
      dJointSetHingeParam(jid, dParamFMax, motor.f_max);
      break;
    case SLIDER:
      dJointSetSliderParam(jid, dParamVel, motor.vel);
      dJointSetSliderParam(jid, dParamFMax, motor.f_max);
      break;
    case UNIVERSAL:
      dJointSetUniversalParam(jid, dParamVel, motor.vel);
      dJointSetUniversalParam(jid, dParamFMax, motor.f_max);
      dJointSetUniversalParam(jid, dParamVel2, motor2.vel);
      dJointSetUniversalParam(jid, dParamFMax2, motor2.f_max);
      break;
    case HINGE2:
      dJointSetHinge2Param(jid, dParamVel, motor.vel);
      dJointSetHinge2Param(jid, dParamFMax, motor.f_max);
      dJointSetHinge2Param(jid, dParamVel2, motor2.vel);
      dJointSetHinge2Param(jid, dParamFMax2, motor2.f_max);
      break;
    case FIXED:
      break;
    case BALL:
      break;
    case NO_JOINT:
      break;
    }
  }

  if(HasJointFlag(USE_ODE_PARAMS)) {
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

  if(HasJointFlag(FEEDBACK)) {
    dJointSetFeedback(jid, &ode_fdbk_obj);
  }
}

static float get_val_no_nan(float val) {
  // note: this may not be sufficiently cross-platform (e.g., windows)
  // could potentially try this: if(val != val) return 0.0f;  // supposed to be true of nans
  if(isnan(val)) return 0.0f;
  return val;
}

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

  if(updt_disp)
    DataChanged(DCR_ITEM_UPDATED);
}

void VEJoint::ApplyForce(float force1, float force2) {
  if(!joint_id) CreateODE();
  if(!joint_id) return;
  dJointID jid = (dJointID)joint_id;

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
  if(geom_id && cur_shape == shape)
     return true;
  dWorldID wid = (dWorldID)GetWorldID();
  if(TestError(!wid, "CreateODE", "no valid world id -- cannot create static item!"))
    return false;

  dSpaceID sid = (dSpaceID)GetSpaceID();
  if(TestError(!sid, "CreateODE", "no valid space id -- cannot create static item geom!"))
    return false;

  if(!geom_id || cur_shape != shape) {
    if(geom_id) dGeomDestroy((dGeomID)geom_id);
    geom_id = NULL;
    switch(shape) {
    case SPHERE:
      geom_id = dCreateSphere(sid, radius);
      break;
    case CAPSULE:
      geom_id = dCreateCapsule(sid, radius, MAX(0.001f,length-2.0f*radius));
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
  }
  
  if(TestError(!geom_id, "CreateODE", "could not create static item geom!"))
    return false;

  cur_shape = shape;

  dGeomSetData((dGeomID)geom_id, (void*)this);

  return true;
}

void VEStatic::DestroyODE() {
  if(geom_id) dGeomDestroy((dGeomID)geom_id);
  geom_id = NULL;
}

void VEStatic::SetValsToODE() {
  if(HasStaticFlag(VEStatic::OFF)) {
    return;
  }
  if(!geom_id || cur_shape != shape) CreateODE();
  if(!geom_id) return;
  dWorldID wid = (dWorldID)GetWorldID();
  if(TestError(!wid, "SetValsToODE", "no valid world id -- cannot create stuff!"))
    return;

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
  static_els.SetValsToODE();	// bodies first!
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
  return true;
}

void VEWorld::DestroyODE() {
  if(cgp_id) dJointGroupDestroy((dJointGroupID)cgp_id);
  cgp_id = NULL;
  if(space_id) dSpaceDestroy((dSpaceID)space_id);
  space_id = NULL;
  if(world_id) dWorldDestroy((dWorldID)world_id);
  world_id = NULL;
}

void VEWorld::SetValsToODE() {
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
}

void VEWorld::GetValsFmODE() {
  objects.GetValsFmODE(updt_display);
  if(updt_display)
    DataChanged(DCR_ITEM_UPDATED); // update displays..
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
}

// in ta_virtenv_qtso.cpp:  QImage VEWorld::GetCameraImage(int cam_no)
