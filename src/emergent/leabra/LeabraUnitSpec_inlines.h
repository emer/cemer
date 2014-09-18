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

#ifndef LeabraUnitSpec_inlines_h
#define LeabraUnitSpec_inlines_h 1

// parent includes:

// member includes:

// declare all other types mentioned but not required to include:

inline void LeabraUnitSpec::Compute_SelfInhib(LeabraUnit* u, LeabraLayerSpec* lspec,
                                              LeabraNetwork* net) {
  float nw_fbi = lspec->inhib_misc.self_fb * u->act_eq;
  u->g_i_self = lspec->lay_inhib.fb_dt * nw_fbi +
    lspec->lay_inhib.fb_dt_c * u->g_i_self;
}

inline void LeabraUnitSpec::Compute_ApplyInhib(LeabraUnit* u, LeabraLayerSpec* lspec,
                                               LeabraNetwork* net,
                                               float inhib_val) {
  Compute_SelfInhib(u, lspec, net);
  u->gc_i = inhib_val + u->g_i_syn + u->g_i_self; // add synaptic and imposed inhibition
}

inline float LeabraUnitSpec::Compute_EThresh(LeabraUnit* u) {
  return ((u->gc_i * e_rev_sub_thr.i + u->gc_l * e_rev_sub_thr.l - u->adapt) /
	  thr_sub_e_rev_e);
} 

inline float LeabraUnitSpec::Compute_EqVm(LeabraUnit* u) {
  float new_v_m = (((u->net * e_rev.e) + (u->gc_l * e_rev.l) + (u->gc_i * e_rev.i)
                    - u->adapt) / (u->net + u->gc_l + u->gc_i));
  return new_v_m;
}

inline void LeabraUnitSpec::Compute_Conduct(LeabraUnit* u, LeabraNetwork* net) {
  u->gc_i *= g_bar.i;
  //  u->net *= g_bar_e_val; // do NOT do this here -- keep in original units for dnet stuff -- g_bar.e is multiplied later when net is actually used..
  u->gc_l = g_bar.l;
}

inline LeabraInhib* LeabraUnitSpec::GetInhib(LeabraUnit* u) {
  LeabraLayer* lay = u->own_lay();
  LeabraLayerSpec* ls = (LeabraLayerSpec*)lay->GetLayerSpec();
  if(ls->HasUnitGpInhib(lay))
    return (LeabraInhib*)lay->UnGpDataUn(u);
  else
    return (LeabraInhib*)lay;
}

#endif // LeabraUnitSpec_inlines_h
