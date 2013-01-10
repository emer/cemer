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

#include "VEJoint.h"

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
