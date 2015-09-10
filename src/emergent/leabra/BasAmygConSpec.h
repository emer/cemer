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

#ifndef BasAmygConSpec_h
#define BasAmygConSpec_h 1

// parent includes:
#include <LeabraConSpec>

// member includes:
#include <LeabraNetwork>

// declare all other types mentioned but not required to include:

eTypeDef_Of(BasAmygConSpec);

class E_API BasAmygConSpec : public LeabraConSpec {
  // simulates learning in the basal amygdala, with separate equations for acquisition vs. extinction subpoplations -- acquisition recv from LatAmyg, learn from da_p or da_n and postsynaptic activity -- extinction recv from context / pfc, and learn from acq (as lrnmod input) and -(da_p or da_n) -- each layer should recv from appropriate valence of da signals (pos or neg)
INHERITED(LeabraConSpec)
public:
  enum BasAmygType {
    ACQ,                        // acquisition neurons -- learn from LatAmyg and (da_p + da_n)
    EXT,                        // extinction neurons -- learn from context / pfc input and ACQ (as learn_mod) and -(da_p + da_n)
  };

  BasAmygType   ba_type;        // type of basal amgydala neuron
  float         neg_da_gain;    // multiplicative gain factor applied to negative dopamine signals -- this value should always be >= 0 and should be < 1 to cause negative da to be reduced relative to positive, thus reducing the level of unlearning in this pathway -- applies to negative of da for the extinction pathway (i.e., to positive da values)
  bool          invert_da;   // if true, wt changes go in opposite direction to the standard case; analogous to NoGo guys in striatum; generally, for PosLV_BasAmyg layers/units should be false; for NegPV_ guys should be true

  inline float  GetDa(float da) {
    if(!invert_da) { return (da < 0.0f) ? neg_da_gain * da : da; }
    else { return (da >= 0.0f) ? neg_da_gain * da : da; }
  }
  // get neg-modulated dopamine value

  inline void C_Compute_dWt_BasAmyg_Acq(float& dwt, const float su_act,
                                        const float ru_act, const float da_p,
                                        const float da_n) {
      dwt += cur_lrate * su_act * ru_act * (GetDa(da_p) +  GetDa(da_n));
      if(invert_da) dwt = -dwt;
  }
  // #IGNORE acquisition
  inline void C_Compute_dWt_BasAmyg_Ext(float& dwt, const float su_act,
                                        const float lrnmod, const float da_p,
                                        const float da_n) {
      dwt += cur_lrate * su_act * lrnmod * (GetDa(-da_p) + GetDa(-da_n));
//      dwt += cur_lrate * su_act * (GetDa(-da_p) + GetDa(-da_n));
    if(invert_da) dwt = -dwt;
  }
  // #IGNORE extinction

  inline void Compute_dWt(ConGroup* rcg, Network* rnet, int thr_no) override {
    LeabraNetwork* net = (LeabraNetwork*)rnet;
    if(!learn || (ignore_unlearnable && net->unlearnable_trial)) return;
    LeabraConGroup* cg = (LeabraConGroup*)rcg;
    LeabraUnitVars* su = (LeabraUnitVars*)cg->ThrOwnUnVars(net, thr_no);
    
    float su_act = su->act_q0;  // previous trial
    float* dwts = cg->OwnCnVar(DWT);

    const int sz = cg->size;
    if(ba_type == ACQ) {
      for(int i=0; i<sz; i++) {
        LeabraUnitVars* ru = (LeabraUnitVars*)cg->UnVars(i, net);
        C_Compute_dWt_BasAmyg_Acq(dwts[i], su_act, ru->act_eq, ru->da_p, ru->da_n);
      }
    }
    else {
      for(int i=0; i<sz; i++) {
        LeabraUnitVars* ru = (LeabraUnitVars*)cg->UnVars(i, net);
        C_Compute_dWt_BasAmyg_Ext(dwts[i], su_act, ru->lrnmod, ru->da_p, ru->da_n);
      }
    }
  }

  TA_SIMPLE_BASEFUNS(BasAmygConSpec);
protected:
  SPEC_DEFAULTS;
private:
  void  Initialize();
  void  Destroy()     { };
  void	Defaults_init();
};

#endif // BasAmygConSpec_h
