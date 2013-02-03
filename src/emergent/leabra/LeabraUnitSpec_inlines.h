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

inline void LeabraUnitSpec::Compute_ApplyInhib(LeabraUnit* u, LeabraNetwork*, float inhib_val) {
  // if you have a computed inhibition value, apply it full force, overwriting anything else
  u->g_i_raw = inhib_val;
  u->gc.i = inhib_val;
  u->prv_g_i = inhib_val;
}

inline void LeabraUnitSpec::Compute_ApplyInhib_LoserGain(LeabraUnit* u,
				 LeabraNetwork*, float inhib_thr, float inhib_top,
				 float inhib_loser) {
  if(u->i_thr >= inhib_thr) {
    u->g_i_raw = inhib_top;
    u->gc.i = inhib_top;
    u->prv_g_i = inhib_top;
  }
  else {
    u->g_i_raw = inhib_loser;
    u->gc.i = inhib_loser;
    u->prv_g_i = inhib_loser;
  }
}

inline float LeabraUnitSpec::Compute_IThreshStd(LeabraUnit* u, LeabraNetwork* net) {
  float non_bias_net = u->net;
  if(u->bias.size)		// subtract out bias weights so they can change k
    non_bias_net -= u->bias_scale * u->bias.OwnCn(0)->wt;
  // including the ga and gh terms
  return ((non_bias_net * e_rev_sub_thr.e + u->gc.l * e_rev_sub_thr.l
	   + u->gc.a * e_rev_sub_thr.a + u->gc.h * e_rev_sub_thr.h - u->adapt) /
	  thr_sub_e_rev_i);
} 

inline float LeabraUnitSpec::Compute_IThreshNoA(LeabraUnit* u, LeabraNetwork* net) {
  float non_bias_net = u->net;
  if(u->bias.size)		// subtract out bias weights so they can change k
    non_bias_net -= u->bias_scale * u->bias.OwnCn(0)->wt;
  // NOT including the ga term
  return ((non_bias_net * e_rev_sub_thr.e + u->gc.l * e_rev_sub_thr.l
	   + u->gc.h * e_rev_sub_thr.h) /
	  thr_sub_e_rev_i);
} 

inline float LeabraUnitSpec::Compute_IThreshNoH(LeabraUnit* u, LeabraNetwork* net) {
  float non_bias_net = u->net;
  if(u->bias.size)		// subtract out bias weights so they can change k
    non_bias_net -= u->bias_scale * u->bias.OwnCn(0)->wt;
  // NOT including the gh terms
  return ((non_bias_net * e_rev_sub_thr.e + u->gc.l * e_rev_sub_thr.l
	   + u->gc.a * e_rev_sub_thr.a - u->adapt) /
	  thr_sub_e_rev_i);
} 

inline float LeabraUnitSpec::Compute_IThreshNoAH(LeabraUnit* u, LeabraNetwork* net) {
  float non_bias_net = u->net;
  if(u->bias.size)		// subtract out bias weights so they can change k
    non_bias_net -= u->bias_scale * u->bias.OwnCn(0)->wt;
  // NOT including the ga and gh terms
  return ((non_bias_net * e_rev_sub_thr.e + u->gc.l * e_rev_sub_thr.l) /
	  thr_sub_e_rev_i);
} 

inline float LeabraUnitSpec::Compute_IThreshAll(LeabraUnit* u, LeabraNetwork* net) {
  // including the ga and gh terms and bias weights
  return ((u->net * e_rev_sub_thr.e + u->gc.l * e_rev_sub_thr.l
	   + u->gc.a * e_rev_sub_thr.a + u->gc.h * e_rev_sub_thr.h - u->adapt) /
	  thr_sub_e_rev_i);
} 

inline float LeabraUnitSpec::Compute_EThresh(LeabraUnit* u) {
  // including the ga and gh terms -- only way to affect anything
  return ((u->gc.i * e_rev_sub_thr.i + u->gc.l * e_rev_sub_thr.l
	   + u->gc.a * e_rev_sub_thr.a + u->gc.h * e_rev_sub_thr.h - u->adapt) /
	  thr_sub_e_rev_e);
} 

inline float LeabraUnitSpec::Compute_EqVm(LeabraUnit* u) {
  float new_v_m = (((u->net * e_rev.e) + (u->gc.l * e_rev.l) + (u->gc.i * e_rev.i) +
		   (u->gc.h * e_rev.h) + (u->gc.a * e_rev.a) - u->adapt) / 
		  (u->net + u->gc.l + u->gc.i + u->gc.h + u->gc.a));
  return new_v_m;
}

#endif // LeabraUnitSpec_inlines_h
