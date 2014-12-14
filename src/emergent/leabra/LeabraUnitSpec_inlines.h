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

inline void LeabraUnitSpec::Compute_SelfInhib(LeabraUnitVars* u, LeabraNetwork* net,
                                              int thr_no, LeabraLayerSpec* lspec) {
  float self = lspec->inhib_misc.self_fb * u->act;
  u->gi_self += lspec->inhib_misc.self_dt * (self - u->gi_self);
}

inline float LeabraUnitSpec::Compute_EThresh(LeabraUnitVars* u) {
  return ((g_bar.i * u->gc_i * (u->E_i - act.thr) + g_bar.l * e_rev_sub_thr.l - u->adapt) /
	  thr_sub_e_rev_e);
} 

inline float LeabraUnitSpec::Compute_EqVm(LeabraUnitVars* u) {
  float new_v_m = (((u->net * e_rev.e) + (g_bar.l * e_rev.l) + (g_bar.i * u->gc_i * u->E_i)
                    - u->adapt) / (u->net + g_bar.l + g_bar.i * u->gc_i));
  return new_v_m;
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
