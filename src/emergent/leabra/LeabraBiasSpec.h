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

#ifndef LeabraBiasSpec_h
#define LeabraBiasSpec_h 1

// parent includes:
#include <LeabraConSpec>

// member includes:

// declare all other types mentioned but not required to include:
class LeabraCon; // 
class LeabraUnit; // 
class LeabraLayer; // 

eTypeDef_Of(LeabraBiasSpec);

class E_API LeabraBiasSpec : public LeabraConSpec {
  // Leabra bias-weight connection specs (bias wts are a little bit special)
INHERITED(LeabraConSpec)
public:

#ifdef __MAKETA__
  bool		learn;		// #CAT_Learning #DEF_false individual control over whether learning takes place in bias weights -- if false, no learning will take place regardless of any other settings -- if true, learning will take place if it is enabled at the network and other relevant levels -- default is OFF for bias weights, because they are generally not needed for larger models, and can only really get in the way -- turning them off also results in a small speed improvement
#endif

  float		dwt_thresh;  // #CONDSHOW_ON_learn #DEF_0.1 #MIN_0 #CAT_Learning don't change if dwt < thresh, prevents buildup of small changes

  inline void B_Compute_dWt(UnitVars* u, Network* net, int thr_no) override {
    if(!learn) return;
    LeabraUnitVars* uv = (LeabraUnitVars*)u;
    // only err is useful contributor to this learning
    float dw = uv->avg_s - uv->avg_m;
    if(fabsf(dw) >= dwt_thresh) {
      uv->bias_dwt += cur_lrate * dw;
    }
  }

  void	Trial_Init_Specs(LeabraNetwork* net) override;

  bool	CheckObjectType_impl(taBase* obj);

  TA_SIMPLE_BASEFUNS(LeabraBiasSpec);
protected:
  SPEC_DEFAULTS;
private:
  void 	Initialize();
  void	Destroy()		{ };
  void	Defaults_init();
};

TA_SMART_PTRS(E_API, LeabraBiasSpec)

#endif // LeabraBiasSpec_h
