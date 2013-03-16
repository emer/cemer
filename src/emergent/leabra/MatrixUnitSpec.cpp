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

#include "MatrixUnitSpec.h"
#include <LeabraNetwork>
#include <MatrixConSpec>
#include <MatrixBiasSpec>


void MatrixUnitSpec::Initialize() {
  SetUnique("bias_spec", true);
  bias_spec.type = &TA_MatrixBiasSpec;

  Defaults_init();
}

void MatrixUnitSpec::Defaults_init() {
  SetUnique("act", true);
  act.i_thr = LeabraActFunExSpec::NO_AH; // key for dopamine effects

  SetUnique("g_bar", true);
  g_bar.a = .03f;
  g_bar.h = .01f;

  SetUnique("maxda", true);
  maxda.val = MaxDaSpec::NO_MAX_DA;

  // SetUnique("noise_type", true);
  noise_type = NETIN_NOISE;

  // SetUnique("noise", true);
  noise.var = 0.0005f;
  // SetUnique("noise_adapt", true);
  noise_adapt.mode = NoiseAdaptSpec::FIXED_NOISE;
  noise_adapt.trial_fixed = true;
  noise_adapt.k_pos_noise = true;
}

void MatrixUnitSpec::InitLinks() {
  inherited::InitLinks();
  bias_spec.type = &TA_MatrixBiasSpec;
}

void MatrixUnitSpec::Compute_Weights(Unit* u, Network* nt, int thread_no) {
  LeabraNetwork* net = (LeabraNetwork*)nt;
  bool er_avail = net->ext_rew_avail || net->pv_detected; // either is good
  if(!er_avail) return; // ONLY update weights on pv trials!!
  inherited::Compute_Weights(u, net, thread_no);
}

