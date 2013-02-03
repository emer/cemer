// Copyright, 1995-2013, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of Emergent
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

#include "MotorForceLayerSpec.h"
#include <LeabraNetwork>
#include <taMath_float>

void MotorForceSpec::Initialize() {
  pos_width = .2f;
  vel_width = .2f;
  norm_width = true;
  clip_vals = true;

  cur_pos = cur_vel = 0.0f;
  pos_min = vel_min = 0.0f;
  pos_range = vel_range = 1.0f;

  pos_width_eff = pos_width;
  vel_width_eff = vel_width;
}

void MotorForceSpec::InitRanges(float pos_min_, float pos_range_, float vel_min_, float vel_range_) {
  pos_min = pos_min_;
  pos_range = pos_range_;
  vel_min = vel_min_;
  vel_range = vel_range_;
  pos_width_eff = pos_width;
  vel_width_eff = vel_width;
  if(norm_width) {
    pos_width_eff *= pos_range;
    vel_width_eff *= vel_range;
  }
}

void MotorForceSpec::InitVals(float pos, int pos_size, float pos_min_, float pos_range_,
                              float vel, int vel_size, float vel_min_, float vel_range_) {
  InitRanges(pos_min_, pos_range_, vel_min_, vel_range_);
  cur_pos = pos;
  pos_incr = pos_range / (float)(pos_size-1);
  cur_vel = vel;
  vel_incr = vel_range / (float)(vel_size-1);
}

float MotorForceSpec::GetWt(int pos_gp_idx, int vel_gp_idx) {
  float ug_pos = pos_min + pos_incr * (float)pos_gp_idx;
  float pos_dist = (ug_pos - cur_pos) / pos_width_eff;
  float ug_vel = vel_min + vel_incr * (float)vel_gp_idx;
  float vel_dist = (ug_vel - cur_vel) / vel_width_eff;
  return taMath_float::exp_fast(-(pos_dist * pos_dist + vel_dist * vel_dist));
}

void MotorForceLayerSpec::Initialize() {
  pos_range.min = 0.0f;
  pos_range.max = 2.0f;
  vel_range.min = -.1f;
  vel_range.max = .1f;
  add_noise = true;
  force_noise.type = Random::GAUSSIAN;
  force_noise.var = .01f;
}

void MotorForceLayerSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  pos_range.UpdateAfterEdit_NoGui();
  vel_range.UpdateAfterEdit_NoGui();
  force_noise.UpdateAfterEdit_NoGui();
}

bool MotorForceLayerSpec::CheckConfig_Layer(Layer* ly, bool quiet) {
  LeabraLayer* lay = (LeabraLayer*)ly;
  bool rval = inherited::CheckConfig_Layer(lay, quiet);
  if(!rval) return rval;

  if(lay->CheckError(!lay->unit_groups, quiet, rval,
                "requires unit groups -- I just set it for you")) {
    lay->unit_groups = true;
  }
  if(lay->CheckError(lay->gp_geom.x < 3, quiet, rval,
                "requires at least 3 unit groups in x axis -- I just set it for you")) {
    lay->gp_geom.x = 5;
  }
  if(lay->CheckError(lay->gp_geom.y < 3, quiet, rval,
                "requires at least 3 unit groups in y axis -- I just set it for you")) {
    lay->gp_geom.y = 5;
  }
  return rval;
}

float MotorForceLayerSpec::ReadForce(LeabraLayer* lay, LeabraNetwork* net, float pos, float vel) {
  if(motor_force.clip_vals) {
    pos = pos_range.Clip(pos);
    vel = vel_range.Clip(vel);
  }
  motor_force.InitVals(pos, lay->gp_geom.x, pos_range.min, pos_range.range,
                       vel, lay->gp_geom.y, vel_range.min, vel_range.range);

  float force = 0.0f;
  float wt_sum = 0.0f;
  for(int y=0; y<lay->gp_geom.y; y++) {
    for(int x=0; x<lay->gp_geom.x; x++) {
      float wt = motor_force.GetWt(x,y);
      int gpidx = y * lay->gp_geom.x + x;
      LeabraUnit* un0 = (LeabraUnit*)lay->UnitAccess(Layer::ACC_GP, 0, gpidx);
      force += wt * un0->act_eq;
      wt_sum += wt;
    }
  }
  if(wt_sum > 0.0f)
    force /= wt_sum;
  if(add_noise)
    force += force_noise.Gen();
  return force;
}

void MotorForceLayerSpec::ClampForce(LeabraLayer* lay, LeabraNetwork* net, float force, float pos, float vel) {
  if(motor_force.clip_vals) {
    pos = pos_range.Clip(pos);
    vel = vel_range.Clip(vel);
  }
  motor_force.InitVals(pos, lay->gp_geom.x, pos_range.min, pos_range.range,
                       vel, lay->gp_geom.y, vel_range.min, vel_range.range);

  for(int y=0; y<lay->gp_geom.y; y++) {
    for(int x=0; x<lay->gp_geom.x; x++) {
      float wt = motor_force.GetWt(x,y);
      int gpidx = y * lay->gp_geom.x + x;
      LeabraUnit* un0 = (LeabraUnit*)lay->UnitAccess(Layer::ACC_GP, 0, gpidx);
      un0->ext = force;
      ClampValue_ugp(lay, Layer::ACC_GP, gpidx, net, wt);
    }
  }
  lay->SetExtFlag(Unit::EXT);
  lay->hard_clamped = clamp.hard;
  HardClampExt(lay, net);
  scalar.clamp_pat = true;      // must have this to keep this clamped val
  UNIT_GP_ITR(lay,
              LeabraUnit* u = (LeabraUnit*)lay->UnitAccess(acc_md, 0, gpidx);
              u->ext = 0.0f;            // must reset so it doesn't contribute!
              );
}

void MotorForceLayerSpec::Compute_BiasVal(LeabraLayer* lay, LeabraNetwork* net) {
  float vel_mid = .5f * (float)(lay->gp_geom.y-1);
  float pos_mid = .5f * (float)(lay->gp_geom.x-1);
  for(int y=0; y<lay->gp_geom.y; y++) {
    float vel_dist = -((float)y - vel_mid) / vel_mid;
    for(int x=0; x<lay->gp_geom.x; x++) {
      float pos_dist = -((float)x - pos_mid) / pos_mid;
      float sum_val = .5f * vel_dist + .5f * pos_dist;
      int gpidx = y * lay->gp_geom.x + x;

      if(bias_val.un != ScalarValBias::NO_UN) {
        Compute_UnBias_Val(lay, Layer::ACC_GP, gpidx, sum_val);
      }
      if(bias_val.wt == ScalarValBias::WT) {
        Compute_WtBias_Val(lay, Layer::ACC_GP, gpidx, sum_val);
      }
    }
  }
}

