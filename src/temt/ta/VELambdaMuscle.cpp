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

#include "VELambdaMuscle.h"

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
