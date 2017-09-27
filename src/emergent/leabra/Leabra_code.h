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

#ifndef Leabra_code_h
#define Leabra_code_h 1

// this is a single master file with all-inline functions that define everything
// in Leabra!

// how to deal with parameters!?  to share code, need exact same memory footprint!


inline void LeabraUnitSpec_Init_Vars(LeabraUnitVars_gen* u, int thr_no) {
  u->bias_fwt = 0.0f;
  u->bias_swt = 0.0f;
  u->ext_orig = 0.0f;
  u->act_eq = 0.0f;
  u->act_nd = 0.0f;
  u->spike = 0.0f;
  u->act_q0 = 0.0f;
  u->act_q1 = 0.0f;
  u->act_q2 = 0.0f;
  u->act_q3 = 0.0f;
  u->act_q4 = 0.0f;
  u->act_g = 0.0f;
  u->act_m = 0.0f;
  u->act_p = 0.0f;
  u->act_dif = 0.0f;
  u->net_prv_q = 0.0f;
  u->net_prv_trl = 0.0f;
  u->da = 0.0f;
  u->avg_ss = 0.15f;
  u->avg_s = 0.15f;
  u->avg_s_eff = 0.15f;
  u->avg_m = 0.15f;
  u->avg_l = avg_l.init;
  u->avg_l_lrn = avg_l.GetLrn(u->avg_l);
  u->act_avg = 0.15f;
  u->margin = 0.0f;
  u->act_raw = 0.0f;
  u->deep_raw = 0.0f;
  u->deep_raw_prv = 0.0f;
  u->deep_mod = 1.0f;
  u->deep_lrn = 1.0f;
  u->deep_ctxt = 0.0f;
  u->deep_mod_net = 0.0f;
  u->deep_raw_net = 0.0f;
  u->thal = 0.0f;
  u->thal_gate = 0.0f;
  u->thal_cnt = -1.0f;
  u->gc_i = 0.0f;
  u->I_net = 0.0f;
  u->v_m = 0.0f;
  u->v_m_eq = 0.0f;
  u->adapt = 0.0f;
  u->gi_syn = 0.0f;
  u->gi_self = 0.0f;
  u->gi_ex = 0.0f;
  u->E_i = 0.25f;
  u->syn_tr = 1.0f;
  u->syn_nr = 1.0f;
  u->syn_pr = 0.2f;
  u->syn_kre = 0.0f;
  u->noise = 0.0f;
  u->da_p = 0.0f;
  u->da_n = 0.0f;
  u->sev = 0.0f;
  u->ach = 0.0f;
  u->shunt = 0.0f;
  u->misc_1 = 0.0f;
  u->misc_2 = 0.0f;
  u->spk_t = -1;

  u->bias_scale = 0.0f;
  u->act_sent = 0.0f;
  u->net_raw = 0.0f;
  u->gi_raw = 0.0f;
  u->deep_raw_sent = 0.0f;

  // CircBufferIndex::Reset(u->spike_e_st, u->spike_e_len);
  // CircBufferIndex::Reset(u->spike_i_st, u->spike_i_len);
  
  // act_buf = NULL;
  // spike_e_buf = NULL;
  // spike_i_buf = NULL;
}

inline void LeabraUnitSpec_Init_Weights(LeabraUnitVars_gen* ru, int thr_no) {
  u->net_prv_q = 0.0f;
  u->net_prv_trl = 0.0f;
  if(act_misc.avg_trace)
    u->act_avg = 0.0f;
  else
    u->act_avg = act_misc.avg_init;
  u->misc_1 = 0.0f;
  u->misc_2 = 0.0f;
  // Init_ActAvg(u, net, thr_no);
}

inline void LeabraUnitSpec_Compute_SelfInhib(LeabraUnitVars_gen* u, int thr_no,
                                              LeabraLayerSpec* lspec) {
  float self = lspec->inhib_misc.self_fb * u->act;
  u->gi_self += lspec->inhib_misc.self_dt * (self - u->gi_self);
}


inline void LeabraUnitSpec_Compute_ApplyInhib
(LeabraUnitVars_gen* u, int thr_no, float lay_adapt_gi,
 LeabraLayerSpec* lspec, LeabraInhib* thr, float ival) {
  LeabraUnitSpoec_Compute_SelfInhib(u, thr_no, lspec);
  float gi_ex = 0.0f;
  if(lspec->del_inhib.on) {
    gi_ex = lspec->del_inhib.prv_trl * u->net_prv_trl + 
      lspec->del_inhib.prv_q * u->net_prv_q;
  }
  u->gc_i = ival + lay_adapt_gi * (u->gi_syn + u->gi_self) + gi_ex;
  if(taMisc::gui_active) {
    u->gi_ex = gi_ex;
  }
}



#endif // Leabra_code_h
