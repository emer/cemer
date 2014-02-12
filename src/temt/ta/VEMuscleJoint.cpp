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

#include "VEMuscleJoint.h"
#include <VEWorld>

#include <taMisc>

TA_BASEFUNS_CTORS_DEFN(VEMuscleJoint);


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
