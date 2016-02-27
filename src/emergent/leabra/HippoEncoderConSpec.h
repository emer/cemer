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

#ifndef HippoEncoderConSpec_h
#define HippoEncoderConSpec_h 1

// parent includes:
#include <LeabraConSpec>

// member includes:
#include <LeabraNetwork>

// declare all other types mentioned but not required to include:

eTypeDef_Of(HippoEncoderConSpec);

class E_API HippoEncoderConSpec : public LeabraConSpec {
  // for EC <-> CA1 connections: CHL learning on encoder variables (act_p vs. act_q1)
INHERITED(LeabraConSpec)
public:

  inline void C_Compute_dWt_CHL(float& dwt, const float ru_act_p, 
                                const float ru_act_m, const float su_act_p,
                                const float su_act_m) {
    dwt += cur_lrate * ((ru_act_p * su_act_p) - (ru_act_m * su_act_m));
  }
  // #IGNORE

  inline void Compute_dWt(ConGroup* rcg, Network* rnet, int thr_no) override {
    LeabraNetwork* net = (LeabraNetwork*)rnet;
    if(!learn || (use_unlearnable && net->unlearnable_trial)) return;
    LeabraConGroup* cg = (LeabraConGroup*)rcg;
    LeabraUnitVars* su = (LeabraUnitVars*)cg->ThrOwnUnVars(net, thr_no);
    const float su_act_p = su->act_p;
    const float su_act_m = su->act_q1;
    float* dwts = cg->OwnCnVar(DWT);

    const int sz = cg->size;
    for(int i=0; i<sz; i++) {
      LeabraUnitVars* ru = (LeabraUnitVars*)cg->UnVars(i, net);
      C_Compute_dWt_CHL(dwts[i], ru->act_p, ru->act_q1, su_act_p, su_act_m);
    }
  }

  TA_BASEFUNS_NOCOPY(HippoEncoderConSpec);
protected:
  SPEC_DEFAULTS;
private:
  void 	Initialize();
  void	Destroy()		{ };
  void	Defaults_init() 	{ Initialize(); }
};

#endif // HippoEncoderConSpec_h
