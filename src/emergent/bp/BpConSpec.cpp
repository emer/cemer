// Copyright 2017, Regents of the University of Colorado,
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

#include "BpConSpec.h"
#include <taMisc>

TA_BASEFUNS_CTORS_DEFN(BpConSpec);

eTypeDef_Of(BpCon);

void BpConSpec::Initialize() {
  min_obj_type = &TA_BpCon;
  lrate_sched.interpolate = false;
  Defaults_init();
}

void BpConSpec::Defaults_init() {
  Initialize_core();
  decay_fun = NULL;
}

void BpConSpec::InitLinks() {
  inherited::InitLinks();
  children.SetBaseType(&TA_BpConSpec); // allow any of this basic type here
  children.el_typ = GetTypeDef(); // but make the default to be me!
  taBase::Own(lrate_sched, this);
}

void BpConSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  lrate_sched.UpdateAfterEdit_NoGui();

  if(taMisc::is_loading && decay_fun) {
    if(decay_fun == Bp_Simple_WtDecay)
      decay_type = SIMPLE_DECAY;
    else if(decay_fun == Bp_WtElim_WtDecay)
      decay_type = ELIMINATION;
    decay_fun = NULL;
  }

  if(momentum > 0.0f && decay_type != NO_DECAY) {
    if(decay_type == SIMPLE_DECAY)
      wt_updt = WU_MOMENT_SIMPLE;
    else
      wt_updt = WU_MOMENT_ELIM;
  }
  else if(momentum > 0.0f) {
    wt_updt = WU_MOMENT;
  }
  else if(decay_type == SIMPLE_DECAY) {
    wt_updt = WU_SIMPLE_DECAY;
  }
  else if(decay_type == ELIMINATION) {
    wt_updt = WU_ELIMINATION;
  }
  else {
    wt_updt = WU_DWT_ONLY;
  }
}

void BpConSpec::SetCurLrate(BpNetworkState_cpp* net) {
  float prv_cur_lrate = cur_lrate;
  cur_lrate = lrate * lrate_sched.GetVal(net->epoch);
  if(cur_lrate != prv_cur_lrate) {
    net->lrate_updtd = true;
  }
}
// set current learning rate based on schedule given epoch

void BpConSpec::LogLrateSched(int epcs_per_step, int n_steps) {
  float log_ns[3] = {1, .5f, .2f};

  lrate_sched.SetSize(n_steps);
  for(int i=0;i<n_steps;i++) {
    lrate_sched[i]->start_ctr = i * epcs_per_step;
    lrate_sched[i]->start_val = log_ns[i%3] * powf(10.0f,-(i/3));
    lrate_sched[i]->UpdateAfterEdit();
  }
  UpdateAfterEdit();            // needed to update the sub guys
}


// these are both OBSOLETE as of 8.0, 8/2016 -- delete at some future point (
void Bp_Simple_WtDecay(BpConSpec* spec, float& wt, float& dwt) {
  dwt -= spec->decay * wt;
}

void Bp_WtElim_WtDecay(BpConSpec* spec, float& wt, float& dwt) {
  float denom = (1.0f + wt * wt);
  dwt -= (spec->decay * wt) / (denom * denom); // note: before 8.0, had wt_sq in numerator!  bad error -- only decays for positive weights!
}

