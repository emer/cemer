// Copyright 2017, Regents of the University of Colorado,
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
  bool          use_trace_act_avg;
  // if true, use act_avg value as sending activation in learning rule -- else uses prior activation state, act_q0

  inline void C_Compute_dWt_TD(float& dwt, const float ru_da_p, 
                                       const float su_act) {
    dwt += cur_lrate * ru_da_p * su_act;
  }
  // #IGNORE

  inline void Compute_dWt(ConState* scg, Network* rnet, int thr_no) override {
    LeabraNetwork* net = (LeabraNetwork*)rnet;
    if(!learn || (use_unlearnable && net->unlearnable_trial)) return;
    LeabraConState_cpp* cg = (LeabraConState_cpp*)scg;
    LeabraUnitState_cpp* su = (LeabraUnitState_cpp*)cg->ThrOwnUnState(net, thr_no);

    float su_act;
    if(use_trace_act_avg)
      su_act = su->act_avg;
    else
      su_act = su->act_q0;
    float* dwts = cg->OwnCnVar(DWT);

    const int sz = cg->size;
    for(int i=0; i<sz; i++) {
      LeabraUnitState_cpp* ru = (LeabraUnitState_cpp*)cg->UnState(i,net);
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

  inline void Compute_Weights(ConState* scg, Network* net, int thr_no) override {
    if(!learn) return;

    LeabraConState_cpp* cg = (LeabraConState_cpp*)scg;

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
