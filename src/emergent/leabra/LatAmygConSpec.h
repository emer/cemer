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

#ifndef LatAmygConSpec_h
#define LatAmygConSpec_h 1

// parent includes:
#include <LeabraConSpec>

// member includes:
#include <LeabraNetwork>

// declare all other types mentioned but not required to include:

eTypeDef_Of(LatAmygConSpec);

class E_API LatAmygConSpec : public LeabraConSpec {
  // simulates learning in the lateral amygdala, based on CS-specific input weights, with learning modulated by phasic dopamine from either da_p (positive-valence) or da_n (negative valence), but predominantly the positive values of these signals. To prevent CS self-training positive feedback, the CS must generally have been active in the prior trial, using act_q0.  there is no dependence on postsynaptic activation
INHERITED(LeabraConSpec)
public:
  float         neg_da_gain;  // #DEF_0 multiplicative gain factor applied to negative dopamine signals -- this value should be zero or close to zero to cause negative da to be much reduced relative to positive, thus reducing the level of unlearning and extinction in this pathway

  inline float  GetDa(float da)
  { return (da < 0.0f) ? neg_da_gain * da : da; }
  // get neg-modulated dopamine value
  
  inline void C_Compute_dWt_LatAmyg(float& dwt, const float su_act,
                                    const float da_p, const float da_n) {
    dwt += cur_lrate * su_act * (GetDa(da_p) + GetDa(da_n));
  }
  // #IGNORE dopamine multiplication

  inline void Compute_dWt(ConGroup* rcg, Network* rnet, int thr_no) override {
    LeabraNetwork* net = (LeabraNetwork*)rnet;
    if(!learn || (use_unlearnable && net->unlearnable_trial)) return;
    LeabraConGroup* cg = (LeabraConGroup*)rcg;
    LeabraUnitVars* su = (LeabraUnitVars*)cg->ThrOwnUnVars(net, thr_no);
    
    float su_act = su->act_q0;
    float* dwts = cg->OwnCnVar(DWT);

    const int sz = cg->size;
    for(int i=0; i<sz; i++) {
      LeabraUnitVars* ru = (LeabraUnitVars*)cg->UnVars(i, net);
      C_Compute_dWt_LatAmyg(dwts[i], su_act, ru->da_p, ru->da_n);
    }
  }

  TA_SIMPLE_BASEFUNS(LatAmygConSpec);
protected:
  SPEC_DEFAULTS;
private:
  void  Initialize();
  void  Destroy()     { };
  void	Defaults_init();
};

#endif // LatAmygConSpec_h
