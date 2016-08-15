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

// declare all other types mentioned but not required to include:

eTypeDef_Of(TDRewPredConSpec);

class E_API TDRewPredConSpec : public LeabraConSpec {
  // Reward Prediction connections: for TD RewPred Unit, uses TD algorithm for predicting rewards -- learns on da_p (TD) * sending trace activation from prev timestep (act_q0)
INHERITED(LeabraConSpec)
public:
  inline void C_Compute_dWt_TD(float& dwt, const float ru_da_p, 
                                       const float su_trace) {
    dwt += cur_lrate * ru_da_p * su_trace;
  }
  // #IGNORE

  inline void Compute_dWt(ConGroup* scg, Network* rnet, int thr_no) override {
    LeabraNetwork* net = (LeabraNetwork*)rnet;
    if(!learn || (use_unlearnable && net->unlearnable_trial)) return;
    LeabraConGroup* cg = (LeabraConGroup*)scg;
    LeabraUnitVars* su = (LeabraUnitVars*)cg->ThrOwnUnVars(net, thr_no);

    const float su_act = su->act_q0; // todo: figure out a trace..
    float* dwts = cg->OwnCnVar(DWT);

    const int sz = cg->size;
    for(int i=0; i<sz; i++) {
      LeabraUnitVars* ru = (LeabraUnitVars*)cg->UnVars(i,net);
      C_Compute_dWt_TD(dwts[i], ru->da_p, su_act);
    }
  }

  inline void	C_Compute_Weights_LinNoBound(float& wt, float& dwt, float& fwt) {
    if(dwt != 0.0f) {
      wt += dwt;
      fwt = wt;
      dwt = 0.0f;
    }
  }
  // #IGNORE compute weights -- linear, no bounds

  inline void Compute_Weights(ConGroup* scg, Network* net, int thr_no) override {
    if(!learn) return;

    LeabraConGroup* cg = (LeabraConGroup*)scg;

    float* wts = cg->OwnCnVar(WT);
    float* dwts = cg->OwnCnVar(DWT);
    float* fwts = cg->OwnCnVar(FWT);

    const int sz = cg->size;
    for(int i=0; i<sz; i++) {
      C_Compute_Weights_LinNoBound(wts[i], dwts[i], fwts[i]);
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
