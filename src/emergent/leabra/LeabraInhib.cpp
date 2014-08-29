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

#include "LeabraInhib.h"

TA_BASEFUNS_CTORS_DEFN(KWTAVals);
TA_BASEFUNS_CTORS_DEFN(InhibVals);

void KWTAVals::Initialize() {
  k = 12;
  pct = .25f;
  pct_c = .75f;

  InitVals();
}

void KWTAVals::InitVals() {
  k_ithr = 0.0f;
  k1_ithr = 0.0f;
  ithr_diff = 0.0f;
  ffi = 0.0f;
  fbi = 0.0f;
  prv_trl_ffi = 0.0f;
  prv_phs_ffi = 0.0f;
}

void KWTAVals::Copy_(const KWTAVals& cp) {
  k = cp.k;
  pct = cp.pct;
  pct_c = cp.pct_c;
  k_ithr = cp.k_ithr;
  k1_ithr = cp.k1_ithr;
  ithr_diff = cp.ithr_diff;
  ffi = cp.ffi;
  fbi = cp.fbi;
  prv_trl_ffi = cp.prv_trl_ffi;
  prv_phs_ffi = cp.prv_phs_ffi;
}

void KWTAVals::Compute_Pct(int n_units) {
  if(n_units > 0)
    pct = (float)k / (float)n_units;
  else
    pct = 0.0f;
  pct_c = 1.0f - pct;
}

void InhibVals::Initialize() {
  kwta = 0.0f;
  g_i = 0.0f;
  gp_g_i = 0.0f;
  g_i_orig = 0.0f;
}

void InhibVals::Copy_(const InhibVals& cp) {
  kwta = cp.kwta;
  g_i = cp.g_i;
  gp_g_i = cp.gp_g_i;
  g_i_orig = cp.g_i_orig;
}

void LeabraInhib::Inhib_Initialize() {
  kwta.k = 1;
  kwta.pct = .25;
  kwta.pct_c = .75;
  i_val.Init();
  phase_dif_ratio = 1.0f;
  maxda = 0.0f;
  un_g_i.cmpt = false;          // don't compute by default
}

void LeabraInhib::Inhib_Init_Acts(LeabraLayerSpec*) {
  kwta.InitVals();
  i_val.Init();
  netin.InitVals();
  i_thrs.InitVals();
  acts.InitVals();
  un_g_i.InitVals();
  maxda = 0.0f;
}

void LeabraInhib::Inhib_Copy_(const LeabraInhib& cp) {
  netin = cp.netin;
  i_thrs = cp.i_thrs;
  acts = cp.acts;
  acts_p = cp.acts_p;
  acts_m = cp.acts_m;
  acts_m_avg = cp.acts_m_avg;
  acts_m2 = cp.acts_m2;
  acts_ctxt = cp.acts_ctxt;
  phase_dif_ratio = cp.phase_dif_ratio;
  kwta = cp.kwta;
  i_val = cp.i_val;
  un_g_i = cp.un_g_i;
}

