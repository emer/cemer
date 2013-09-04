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

#ifndef XCalHebbConSpec_h
#define XCalHebbConSpec_h 1

// parent includes:
#include <LeabraConSpec>

// member includes:
#include <LeabraNetwork>

// declare all other types mentioned but not required to include:

eTypeDef_Of(XCalHebbConSpec);

class E_API XCalHebbConSpec : public LeabraConSpec {
  // xcal version of hebbian learning
INHERITED(LeabraConSpec)
public:
  float		hebb_mix;	// #DEF_0 #MIN_0 #MAX_1 amount of "pure" hebbian learning operating within the thr_l_mix BCM-like component -- actually the amount of fixed sending layer expected activity level to mulitply the recv long-term average activation by (remainder is multiplied by the current sending medium time scale activation, as is done in BCM) -- this is not useful for most cases, but is included for generality
  float		hebb_mix_c;	// #READ_ONLY 1 - hebb_mix -- amount of sending medium time scale activation to mulitply ru avg_l by
  float		su_act_min;	// #DEF_0 #MIN_0 #MAX_1 NOTE: this is only useful for hebb_mixminimum effective activation for sending units as entering into learning rule -- because the xcal curve returns to 0 when the S*R coproduct is 0, an inactive sending unit will never experience any weight change -- this is counter to the hebbian form of learning, where an active recv unit will decrease weights from inactive senders -- ensuring a minimal amount of activation avoids this issue, and reflects the low background rate of neural and synaptic activity that actually exists in the brain

  inline void Compute_dWt_CtLeabraXCAL(LeabraSendCons* cg, LeabraUnit* su,
                                       LeabraNetwork* net) {
    LeabraLayer* slay = (LeabraLayer*)cg->prjn->from.ptr();
    float su_avg_s = MAX(su->avg_s, su_act_min);
    float su_avg_m = MAX(su->avg_m, su_act_min);
    float su_act_mult = xcal.thr_l_mix * (hebb_mix * slay->kwta.pct + hebb_mix_c * su_avg_m);

    for(int i=0; i<cg->size; i++) {
      LeabraUnit* ru = (LeabraUnit*)cg->Un(i,net);
      C_Compute_dWt_CtLeabraXCAL_trial((LeabraCon*)cg->OwnCn(i), ru, su_avg_s, su_avg_m,
				       su_act_mult);
    }
  }

  TA_SIMPLE_BASEFUNS(XCalHebbConSpec);
protected:
  SPEC_DEFAULTS;
  void	UpdateAfterEdit_impl();
private:
  void 	Initialize();
  void	Destroy()		{ };
  void	Defaults_init() 	{ };
};

#endif // XCalHebbConSpec_h
