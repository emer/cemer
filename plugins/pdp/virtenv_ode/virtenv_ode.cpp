#include "virtenv_ode.h"
#include <QtPlugin>
#include <iostream>

const taVersion VEOdePlugin::version(1,0,0,0);

VEOdePlugin::VEOdePlugin(QObject*){}

int VEOdePlugin::InitializeTypes() {
  ta_Init_virtenv_ode();  // call the maketa-generated type initialization routine
  taMisc::Init_Types();		// this is required if plugin defines any new So/Inventor types
  return 0;
}

int VEOdePlugin::InitializePlugin() {
  return 0;
}

const char* VEOdePlugin::url() {
  return "http://grey.colorado.edu/cgi-bin/trac.cgi";
}

Q_EXPORT_PLUGIN2(virtenv_ode, VEOdePlugin)

/////////////////////////////////////////////////////////////////////////////////
//		Actual ODE Code

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
//		bodies (ridid object elements)


void VESurface::Initialize() {
  friction = 1.0e22f;
  bounce = 0.0f;
  bounce_vel = .01f;
}

#include <Inventor/SbLinear.h>

void VEBody::Initialize() {
  body_id = NULL;
  geom_id = NULL;
  flags = BF_NONE;
  shape = CYLINDER;
  cur_shape = BOX;
  mass = 1.0f;
  radius = .5f;
  length = 1.0f;
  box = 1.0f;
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
      geom_id = dCreateCapsule(sid, radius, MAX(0.0f,length-2.0f*radius));
      break;
    case CYLINDER:
      geom_id = dCreateCylinder(sid, radius, length);
      break;
    case BOX:
      geom_id = dCreateBox(sid, box.x, box.y, box.z);
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
  if(!body_id) CreateODE();
  if(!body_id) return;
  dBodyID bid = (dBodyID)body_id;
  switch(shape) {
  case SPHERE:
    dMassSetSphereTotal(&mass_ode, mass, radius);
    break;
  case CAPSULE:
    //    dMassSetCappedCylinderTotal(&mass_ode, mass, long_axis, radius, length);
    dMassSetCapsuleTotal(&mass_ode, mass, long_axis, radius, MAX(0.0f,length-2.0*radius));
    break;
  case CYLINDER:
    dMassSetCylinderTotal(&mass_ode, mass, long_axis, radius, length);
    break;
  case BOX:
    dMassSetBoxTotal(&mass_ode, mass, box.x, box.y, box.z);
    break;
  }
  dBodySetMass(bid, &mass_ode);
}

void VEBody::GetValsFmODE(bool updt_disp) {
  if(!body_id) CreateODE();
  if(!body_id) return;
  dBodyID bid = (dBodyID)body_id;
  const dReal* opos = dBodyGetPosition(bid);
  cur_pos.x = opos[0]; cur_pos.y = opos[1]; cur_pos.z = opos[2];

  // ODE quaternion = w,x,y,z; Inventor = x,y,z,w
  const dReal* quat = dBodyGetQuaternion(bid);
  SbRotation sbrot;
  sbrot.setValue(-quat[3], quat[0], quat[1], -quat[2]);
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

void VECamera::Initialize() {
  img_size.x = 320;
  img_size.y = 240;
  color = true;
  focal_dist = 5.0f;
  field_of_view = .7853f;
}

void VELight::Initialize() {
  light_type = DIRECTIONAL_LIGHT;
  light_on = true;
  intensity = 1.0f;
  drop_off_rate = 0.0f;
  cut_off_angle = .7853f;
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

  if(TestError(!body1 || !body1->body_id,
	       "SetValsToODE", "body1 of joint MUST be specified and already exist!"))
    return;
  if(TestError(!body2 || !body2->body_id,
	       "SetValsToODE", "body2 of joint MUST be specified and already exist -- use fixed field on body to set fixed bodies!"))
    return;

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
    }
  }

  if(HasJointFlag(FEEDBACK)) {
    dJointSetFeedback(jid, &ode_fdbk_obj);
  }
}

void VEJoint::GetValsFmODE(bool updt_disp) {
  if(!HasJointFlag(FEEDBACK)) return;
  if(!joint_id) CreateODE();
  if(!joint_id) return;
  dJointID jid = (dJointID)joint_id;

  dJointGetFeedback(jid);
  cur_force1.x = ode_fdbk_obj.f1[0]; cur_force1.y = ode_fdbk_obj.f1[1]; cur_force1.z = ode_fdbk_obj.f1[2];
  cur_force2.x = ode_fdbk_obj.f2[0]; cur_force2.y = ode_fdbk_obj.f2[1]; cur_force2.z = ode_fdbk_obj.f2[2];
  cur_torque1.x = ode_fdbk_obj.t1[0]; cur_torque1.y = ode_fdbk_obj.t1[1]; cur_torque1.z = ode_fdbk_obj.t1[2];
  cur_torque2.x = ode_fdbk_obj.t2[0]; cur_torque2.y = ode_fdbk_obj.t2[1]; cur_torque2.z = ode_fdbk_obj.t2[2];

  switch(joint_type) {
  case BALL:
    break;
  case HINGE:
    pos = dJointGetHingeAngle(jid);
    vel = dJointGetHingeAngleRate(jid);
    break;
  case SLIDER:
    pos = dJointGetSliderPosition(jid);
    vel = dJointGetSliderPositionRate(jid);
    break;
  case UNIVERSAL:
    pos = dJointGetUniversalAngle1(jid);
    vel = dJointGetUniversalAngle1Rate(jid);
    pos2 = dJointGetUniversalAngle2(jid);
    vel2 = dJointGetUniversalAngle2Rate(jid);
    break;
  case HINGE2:
    pos = dJointGetHinge2Angle1(jid);
    vel = dJointGetHinge2Angle1Rate(jid);
    vel2 = dJointGetHinge2Angle2Rate(jid);
    break;
  case FIXED:
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
  hash_levels.min = 1;  hash_levels.max = 4;
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
  hash_levels.min = 1;  hash_levels.max = 4;
  gravity.y = -9.81f;
  step_type = STD_STEP;
  stepsize = .01f;
  quick_iters = 20;
}

void VEWorld::Destroy() {
  CutLinks();
}

void VEWorld::CutLinks() {
  objects.CutLinks();
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


/////////////////////////////////////////////////////////////////////////
//		So Inventor objects

#include <QGroupBox>
#include <qlayout.h>
#include <qpushbutton.h>
#include <qwidget.h>
#include <QImage>
#include <QPixmap>
#include <QLabel>

#include <Inventor/SoInput.h>
#include <Inventor/SoDB.h>
#include <Inventor/nodes/SoCube.h>
#include <Inventor/nodes/SoFont.h>
#include <Inventor/nodes/SoSphere.h>
#include <Inventor/nodes/SoCylinder.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/nodes/SoTranslation.h>
#include <Inventor/nodes/SoSwitch.h>
#include <Inventor/nodes/SoDirectionalLight.h>
#include <Inventor/nodes/SoPerspectiveCamera.h>
#include <Inventor/SoOffscreenRenderer.h>
#include <Inventor/SbViewportRegion.h>
 
SO_NODE_SOURCE(T3VEWorld);

void T3VEWorld::initClass()
{
  SO_NODE_INIT_CLASS(T3VEWorld, T3NodeParent, "T3NodeParent");
}

T3VEWorld::T3VEWorld(void* world)
:inherited(world)
{
  SO_NODE_CONSTRUCTOR(T3VEWorld);

  sun_light = new SoDirectionalLight;
  insertChildBefore(topSeparator(), sun_light, childNodes());
  camera_switch = new SoSwitch;
  insertChildBefore(topSeparator(), camera_switch, childNodes());
}

T3VEWorld::~T3VEWorld()
{
  
}

void T3VEWorld::setSunLightOn(bool on) {
  sun_light->on = on;
}

void T3VEWorld::setSunLightDir(float x_dir, float y_dir, float z_dir) {
  sun_light->direction = SbVec3f(x_dir, y_dir, z_dir);
}

/////////////////////////////////////////////
//	Object

SO_NODE_SOURCE(T3VEObject);

void T3VEObject::initClass()
{
  SO_NODE_INIT_CLASS(T3VEObject, T3NodeParent, "T3NodeParent");
}

T3VEObject::T3VEObject(void* world)
:inherited(world)
{
  SO_NODE_CONSTRUCTOR(T3VEObject);
}

T3VEObject::~T3VEObject()
{
  
}

/////////////////////////////////////////////
//	Body

SO_NODE_SOURCE(T3VEBody);

void T3VEBody::initClass()
{
  SO_NODE_INIT_CLASS(T3VEBody, T3NodeLeaf, "T3NodeLeaf");
}

T3VEBody::T3VEBody(void* bod, bool show_drag)
:inherited(bod)
{
  SO_NODE_CONSTRUCTOR(T3VEBody);

  show_drag_ = show_drag;
  drag_ = NULL;
}

T3VEBody::~T3VEBody()
{
  
}

///////////////////////////////////////////////////////////////////////
//		T3 DataView Guys

void VEBodyView::Initialize(){
  data_base = &TA_VEBody;
}

void VEBodyView::Copy_(const VEBodyView& cp) {
  name = cp.name;
}

void VEBodyView::Destroy() {
  CutLinks();
}

bool VEBodyView::SetName(const String& value) { 
  name = value;  
  return true; 
} 

void VEBodyView::SetBody(VEBody* ob) {
  if (Body() == ob) return;
  SetData(ob);
  if(ob) {
    if (name != ob->name) {
      name = ob->name;
    }
  }
}

void VEBodyView::Render_pre() {
  m_node_so = new T3VEBody(this);

  SoSeparator* ssep = m_node_so->shapeSeparator();

  VEBody* ob = Body();
  if(ob) {
    if(ob->HasBodyFlag(VEBody::FM_FILE) && !ob->obj_fname.empty()) {
      SoInput in;
      if (in.openFile(ob->obj_fname)) {
	SoSeparator* root = SoDB::readAll(&in);
	if (root) {
	  ssep->addChild(root);
	  SoTransform* tx = m_node_so->txfm_shape();
	  ob->obj_xform.CopyTo(tx);
	  goto finish;
	}
      }
      taMisc::Error("object file:", ob->obj_fname, "not found, reverting to shape");
    }
    switch(ob->shape) {
    case VEBody::SPHERE: {
      SoSphere* sp = new SoSphere;
      sp->radius = ob->radius;
      ssep->addChild(sp);
      break;
    }
    case VEBody::CAPSULE:
    case VEBody::CYLINDER: {
      SoCylinder* sp = new SoCylinder;
      sp->radius = ob->radius;
      sp->height = ob->length;
      ssep->addChild(sp);
      SoTransform* tx = m_node_so->txfm_shape();
      if(ob->long_axis == VEBody::LONG_X)
	tx->rotation.setValue(SbVec3f(0.0f, 0.0f, 1.0f), 1.5708);
      else if(ob->long_axis == VEBody::LONG_Z)
	tx->rotation.setValue(SbVec3f(1.0f, 0.0f, 0.0f), 1.5708);
      break;
    }
    case VEBody::BOX: {
      SoCube* sp = new SoCube;
      sp->width = ob->box.x;
      sp->depth = ob->box.z;
      sp->height = ob->box.y;
      ssep->addChild(sp);
      break;
    }
    }
  }      
 finish:

  inherited::Render_pre();
}

void VEBodyView::Render_impl() {
  inherited::Render_impl();

  T3VEBody* node_so = (T3VEBody*)this->node_so(); // cache
  if(!node_so) return;
  VEBody* ob = Body();
  if(!ob) return;

  SoTransform* tx = node_so->transform();
  tx->translation.setValue(ob->cur_pos.x, ob->cur_pos.y, ob->cur_pos.z);
  tx->rotation.setValue(SbVec3f(ob->cur_rot.x, ob->cur_rot.y, ob->cur_rot.z), ob->cur_rot.rot);

  SoMaterial* mat = node_so->material();
  mat->diffuseColor.setValue(ob->color.r, ob->color.g, ob->color.b);
  mat->transparency.setValue(1.0f - ob->color.a);
}

//////////////////////////
//   VEObjectView	//
//////////////////////////

void VEObjectView::Initialize(){
  data_base = &TA_VEObject;
}

void VEObjectView::Copy_(const VEObjectView& cp) {
  name = cp.name;
}

void VEObjectView::Destroy() {
  CutLinks();
}

bool VEObjectView::SetName(const String& value) { 
  name = value;  
  return true; 
} 

void VEObjectView::SetObject(VEObject* ob) {
  if (Object() == ob) return;
  SetData(ob);
  if(ob) {
    if (name != ob->name) {
      name = ob->name;
    }
  }
}

void VEObjectView::BuildAll() {
  Reset();
  VEObject* obj = Object();
  if(!obj) return;

  VEBody* bod;
  taLeafItr i;
  FOR_ITR_EL(VEBody, bod, obj->bodies., i) {
    VEBodyView* ov = new VEBodyView();
    ov->SetBody(bod);
    children.Add(ov);
    ov->BuildAll();
  }
}

void VEObjectView::Render_pre() {
  m_node_so = new T3VEObject(this);
  inherited::Render_pre();
}

void VEObjectView::Render_impl() {
  inherited::Render_impl();

  T3VEObject* node_so = (T3VEObject*)this->node_so(); // cache
  if(!node_so) return;
  VEObject* ob = Object();
  if(!ob) return;
}

//////////////////////////
//   VEWorldView	//
//////////////////////////

VEWorldView* VEWorld::NewView(T3DataViewFrame* fr) {
  return VEWorldView::New(this, fr);
}

VEWorldView* VEWorldView::New(VEWorld* wl, T3DataViewFrame*& fr) {
  if (!wl) return NULL;
  if (fr) {
    //note: even if fr specified, need to insure it is right proj for object
    if (!wl->SameScope(fr, &TA_taProject)) {
      taMisc::Error("The viewer you specified is not in the same Project as the world.");
      return NULL;
    }
    // check if already viewing this obj there, warn user
    // no, because we want to be able to graph and grid in same view!
//     T3DataView* dv = fr->FindRootViewOfData(wl);
//     if (dv) {
//       if (taMisc::Choice("This table is already shown in that frame -- would you like"
//           " to show it in a new frame?", "&Ok", "&Cancel") != 0) return NULL;
//       fr = NULL; // make a new one
//     }
  } 
  if (!fr) {
    fr = T3DataViewer::GetBlankOrNewT3DataViewFrame(wl);
  }
  if (!fr) return NULL; // unexpected...
  
  VEWorldView* vw = new VEWorldView;
  fr->AddView(vw);
  vw->SetWorld(wl);
  // make sure we get it all setup!
  vw->BuildAll();
  fr->Render();
  fr->ViewAll();
  fr->GetCameraPosOrient();
  return vw;
}

void VEWorldView::Initialize() {
  data_base = &TA_VEWorld;
  children.SetBaseType(&TA_VEObjectView);
  cam_renderer = NULL;
}

void VEWorldView::InitLinks() {
  inherited::InitLinks();
//   taBase::Own(table_orient, this);
}

void VEWorldView::CutLinks() {
  inherited::CutLinks();
  if(cam_renderer) delete cam_renderer;
  cam_renderer = NULL;
}

void VEWorldView::Copy_(const VEWorldView& cp) {
  display_on = cp.display_on;
}

void VEWorldView::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
}

String VEWorldView::GetLabel() const {
  VEWorld* wl = World(); 
  if(wl) return wl->GetName();
  return "(no world)";
}

String VEWorldView::GetName() const {
  VEWorld* wl = World(); 
  if(wl) return wl->GetName();
  return "(no world)";
}

const String VEWorldView::caption() const {
  VEWorld* wl = World(); 
  String rval;
  if (wl) {
    rval = wl->GetDisplayName();
  } else rval = "(no world)";
  return rval;
}

void VEWorldView::SetWorld(VEWorld* wl) {
  if (World() == wl) return;
  if (wl) {
    SetData(wl);
    if (m_wvp) m_wvp->Refresh(); // to update name
  } else {
    Unbind(); // also does kids
  }
}

void VEWorldView::OnWindowBind_impl(iT3DataViewFrame* vw) {
  inherited::OnWindowBind_impl(vw);
  if (!m_wvp) {
    m_wvp = new VEWorldViewPanel(this);
    vw->viewerWindow()->AddPanel(m_wvp, false); // no tab yet
    vw->RegisterPanel(m_wvp);
  }
}

void VEWorldView::InitDisplay(bool init_panel) {
  if (init_panel) {
    InitPanel();
    UpdatePanel();
  }
  // descend into sub items
//   LayerView* lv;
//   taListItr i;
//   FOR_ITR_EL(LayerView, lv, children., i) {
//     InitDisplay_Layer(lv, false);
//   }
}

void VEWorldView::UpdateDisplay(bool update_panel) {
  if (update_panel) UpdatePanel();
  Render_impl();
}

void VEWorldView::InitPanel() {
  if (m_wvp)
    m_wvp->InitPanel();
}

void VEWorldView::UpdatePanel() {
  if (m_wvp)
    m_wvp->UpdatePanel();
}

bool VEWorldView::isVisible() const {
  return (taMisc::gui_active && isMapped());
}

void VEWorldView::BuildAll() {
  Reset();
  VEWorld* wl = World();
  if(!wl) return;

  VEObject* obj;
  taLeafItr i;
  FOR_ITR_EL(VEObject, obj, wl->objects., i) {
    VEObjectView* ov = new VEObjectView();
    ov->SetObject(obj);
    children.Add(ov);
    ov->BuildAll();
  }
}

void VEWorldView::Render_pre() {
  InitPanel();

  m_node_so = new T3VEWorld(this);

  inherited::Render_pre();
}

void VEWorldView::Render_impl() {
  inherited::Render_impl();

  T3VEWorld* node_so = (T3VEWorld*)this->node_so(); // cache
  if(!node_so) return;

  VEWorld* wl = World();
  if(!wl) return;

  SoFont* font = node_so->captionFont(true);
  float font_size = 0.4f;
  font->size.setValue(font_size); // is in same units as geometry units of network
  node_so->setCaption(caption().chars());

  SoSwitch* cam_switch = node_so->getCameraSwitch();
  int n_cam = 0;
  if(wl->camera_0) {
    if(cam_switch->getNumChildren() == 0) {
      SoPerspectiveCamera* cam = new SoPerspectiveCamera;
      cam_switch->addChild(cam);
    }
    n_cam++;
  }
  if(n_cam == 1 && wl->camera_1) {
    if(cam_switch->getNumChildren() == 1) {
      SoPerspectiveCamera* cam = new SoPerspectiveCamera;
      cam_switch->addChild(cam);
    }
  }
  if(n_cam == 0) {
    cam_switch->removeAllChildren();
  }

  UpdatePanel();
}

// void VEWorldView::setDisplay(bool value) {
//   if (display_on == value) return;
//   display_on = value;
//   UpdateDisplay(false);		// 
// }

QImage VEWorldView::RenderCamera(int cam_no) {
  QImage img;
  VEWorld* wl = World();
  if(!wl) return img;

  T3VEWorld* node_so = (T3VEWorld*)this->node_so(); // cache
  if(!node_so) return img;

  SoSwitch* cam_switch = node_so->getCameraSwitch();
  if(cam_switch->getNumChildren() <= cam_no) return img; // not ready yet

  VECamera* vecam = NULL;
  if(cam_no == 0) {
    if(TestError(!wl->camera_0, "RenderCamera", "camera_0 not set -- cannot be rendered!"))
      return img;
    vecam = wl->camera_0.ptr();
  }
  else if(cam_no == 1) {
    if(TestError(!wl->camera_1, "RenderCamera", "camera_1 not set -- cannot be rendered!"))
      return img;
    vecam = wl->camera_1.ptr();
  }
  else {
    TestError(true, "RenderCamera", "only 2 cameras (0 or 1) supported!");
    return img;
  }

  SbViewportRegion vpreg;
  vpreg.setWindowSize(vecam->img_size.x, vecam->img_size.y); 
  
  if(!cam_renderer)
    cam_renderer = new SoOffscreenRenderer(vpreg);

  cam_renderer->setViewportRegion(vpreg);
  cam_renderer->setComponents(SoOffscreenRenderer::RGB_TRANSPARENCY);

  cam_switch->whichChild = cam_no;

  SoPerspectiveCamera* cam = (SoPerspectiveCamera*)cam_switch->getChild(cam_no);
  cam->position.setValue(vecam->cur_pos.x, vecam->cur_pos.y, vecam->cur_pos.z);
  cam->orientation.setValue(SbVec3f(vecam->cur_rot.x, vecam->cur_rot.y, vecam->cur_rot.z),
			     vecam->cur_rot.rot);
  // todo: compute these from the scene
  cam->nearDistance = 0.1f;
  cam->farDistance = 10.0f;
  cam->focalDistance = vecam->focal_dist;
  cam->heightAngle = vecam->field_of_view;

  bool ok = cam_renderer->render(node_so);

  cam_switch->whichChild = -1;	// switch off for regular viewing!

  if(TestError(!ok, "RenderCamera", "offscreen render failed!")) return img;
  
  img = QImage((uchar*)cam_renderer->getBuffer(), vecam->img_size.x, vecam->img_size.y,
 	       QImage::Format_ARGB32);
//   img = QImage(vecam->img_size.x, vecam->img_size.y, QImage::Format_RGB32);

//   int* gbuf = (int*)cam_renderer->getBuffer();

//   int idx = 0;
//   for(int y=0;y<vecam->img_size.y;y++) {
//     for(int x=0;x<vecam->img_size.x;x++) {
//       img.setPixel(x,y, gbuf[idx]);
//       idx++;
//     }
//   }
  return img;
}


////////////////////////////////////////////////////////////

VEWorldViewPanel::VEWorldViewPanel(VEWorldView* dv_)
:inherited(dv_)
{
  int font_spec = taiMisc::fonMedium;
  updating = 0;
  QWidget* widg = new QWidget();
  //note: we don't set the values of all controls here, because dv does an immediate refresh
  layOuter = new QVBoxLayout(widg);
  layOuter->setSpacing(taiM->vsep_c);

  ////////////////////////////////////////////////////////////////////////////
  layDispCheck = new QHBoxLayout(layOuter);

  labcam0 = new QLabel(widg);
  layDispCheck->addWidget(labcam0);

  labcam1 = new QLabel(widg);
  layDispCheck->addWidget(labcam1);
  
  setCentralWidget(widg);
}

VEWorldViewPanel::~VEWorldViewPanel() {
  VEWorldView* wv_ = wv();
  if (wv_) {
    wv_->m_wvp = NULL;
  }
}

void VEWorldViewPanel::GetImage_impl() {
  inherited::GetImage_impl();
  VEWorldView* wv_ = wv();
  
  VEWorld* wl = wv_->World();
  if(!wl) return;
  
  if(wl->camera_0) {
    QPixmap pm = QPixmap::fromImage(wv_->RenderCamera(0));
    if(!pm.isNull()) {
      labcam0->setPixmap(pm);
    }
    else {
      labcam0->setText("Camera 0 Image Render Failed!");
    }
  }
  else {
    labcam0->setText("No Camera 0 Set");
  }

  if(wl->camera_1) {
    QPixmap pm = QPixmap::fromImage(wv_->RenderCamera(1));
    if(!pm.isNull()) {
      labcam1->setPixmap(pm);
    }
    else {
      labcam1->setText("Camera 1 Image Render Failed!");
    }
  }
  else {
    labcam1->setText("No Camera 1 Set");
  }
}

void VEWorldViewPanel::InitPanel() {
  VEWorldView* wv_ = wv();
  if(!wv_) return;
  ++updating;
  // fill monitor values
//   GetVars();
  --updating;
}

