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

#ifndef TDRewPredConSpec_h
#define TDRewPredConSpec_h 1

// parent includes:
#include <LeabraConSpec>

// member includes:
#include <LeabraNetwork>
#include <LeabraTdUnit>

// declare all other types mentioned but not required to include:

eTypeDef_Of(TDRewPredConSpec);

class E_API TDRewPredConSpec : public LeabraConSpec {
  // Reward Prediction connections: for TD RewPred Layer, uses TD algorithm for predicting rewards
INHERITED(LeabraConSpec)
public:
  inline void C_Compute_dWt_Delta_NoSB(float& dwt, const float ru_act_p, 
                                  const float ru_act_m, const float su_trace) {
    const float err = (ru_act_p - ru_act_m) * su_trace;
    dwt += cur_lrate * err;
  }
  // #IGNORE

  inline void Compute_dWt(LeabraConGroup* cg, LeabraUnit* su, LeabraNetwork* net) override {
    if(use_unlearnable && net->unlearnable_trial) return;

    LeabraTdUnit* lsu = (LeabraTdUnit*)su;
    const float su_trace = lsu->trace;
    float* dwts = cg->OwnCnVar(DWT);
    float* wts = cg->OwnCnVar(WT);

    for(int i=0; i<cg->size; i++) {
      LeabraTdUnit* ru = (LeabraTdUnit*)cg->Un(i,net);
      C_Compute_dWt_Delta_NoSB(dwts[i], ru->act_p, ru->act_m, su_trace);
    }
  }

  TA_BASEFUNS_NOCOPY(TDRewPredConSpec);
protected:
  SPEC_DEFAULTS;
private:
  void 	Initialize();
  void	Destroy()		{ };
  void	Defaults_init() { Initialize(); }
};

#endif // TDRewPredConSpec_h
