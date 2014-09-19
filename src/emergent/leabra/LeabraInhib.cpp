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

TA_BASEFUNS_CTORS_DEFN(LeabraInhibVals);

void LeabraInhibVals::Initialize() {
  InitVals();
}

void LeabraInhibVals::InitVals() {
  ffi = 0.0f;
  fbi = 0.0f;
  prv_trl_g_i = 0.0f;
  prv_phs_g_i = 0.0f;

  g_i = 0.0f;
  g_i_orig = 0.0f;
  lay_g_i = 0.0f;
  laygp_g_i = 0.0f;
}

void LeabraInhibVals::Copy_(const LeabraInhibVals& cp) {
  ffi = cp.ffi;
  fbi = cp.fbi;
  prv_trl_g_i = cp.prv_trl_g_i;
  prv_phs_g_i = cp.prv_phs_g_i;
  g_i = cp.g_i;
  g_i_orig = cp.g_i_orig;
  lay_g_i = cp.lay_g_i;
  laygp_g_i = cp.laygp_g_i;
}
 
void LeabraInhib::Inhib_Initialize() {
  i_val.InitVals();
  acts_m_avg = .1f;
  acts_p_avg = .1f;
  acts_m_avg_eff = 1.f;
  un_g_i.cmpt = false;          // don't compute by default
}

void LeabraInhib::Inhib_Init_Acts(LeabraLayerSpec*) {
  i_val.InitVals();
  netin.InitVals();
  acts.InitVals();
  un_g_i.InitVals();
}

void LeabraInhib::Inhib_Copy_(const LeabraInhib& cp) {
  acts_m_avg = cp.acts_m_avg;
  acts_m_avg_eff = cp.acts_m_avg_eff;
  acts_p_avg = cp.acts_p_avg;
  i_val = cp.i_val;
  netin = cp.netin;
  acts = cp.acts;
  acts_m = cp.acts_m;
  acts_p = cp.acts_p;
  acts_ctxt = cp.acts_ctxt;
  acts_mid = cp.acts_mid;
  un_g_i = cp.un_g_i;
}

