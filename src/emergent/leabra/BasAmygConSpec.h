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
#include <BasAmygUnitSpec>

// declare all other types mentioned but not required to include:

eTypeDef_Of(BasAmygConSpec);

class E_API BasAmygConSpec : public LeabraConSpec {
  // simulates learning in the basal amygdala, with separate equations for acquisition vs. extinction subpoplations -- acquisition recv from LatAmyg, learn from da_p and postsynaptic activity -- extinction recv from context / pfc, and learn from ACQ up-state signal and da_p using D2 receptors
INHERITED(LeabraConSpec)
public:
  float         burst_da_gain;  // #MIN_0 multiplicative gain factor applied to positive dopamine signals -- this operates on the raw dopamine signal prior to any effect of D2 receptors in reversing its sign!
  float         dip_da_gain;    // #MIN_0 multiplicative gain factor applied to negative dopamine signals -- this operates on the raw dopamine signal prior to any effect of D2 receptors in reversing its sign!

//  inline float  GetDa(float da, bool d2r) {
//    if(da < 0.0f) da *= dip_da_gain; else da *= burst_da_gain;
//    if(d2r) da = -da;
//    return da;
//  }
//  // get effective dopamine signal taking into account gains and reversal by D2R

  inline float  GetDa(float da, bool d2r, float cwt) {
    if(da < 0.0f) da *= dip_da_gain; else da *= burst_da_gain;
    if(d2r) {
      da = -da;
//      if(da >=0.0f) {
//        da *= (1.0f - cwt);
//      }
//      else { da *= cwt; }
    }
    return da;
  }
  // get effective dopamine signal taking into account gains and reversal by D2R
  
  inline void C_Compute_dWt_BasAmyg_Acq(float& dwt, float cwt,
                                        const float su_act,
                                        const float ru_act, const float da_p,
                                        const bool d2r, const float lrate_eff) {
    dwt += lrate_eff * su_act * ru_act * GetDa(da_p, d2r, cwt);
  }
  // #IGNORE acquisition
  inline void C_Compute_dWt_BasAmyg_Ext(float& dwt, float cwt,
                                        const float su_act,
                                        const float ru_act, const float da_p,
                                        const bool d2r, const float lrate_eff) {
    dwt += lrate_eff * su_act * GetDa(da_p, d2r, cwt); // no ru_act!
  }
  // #IGNORE extinction

  inline void Compute_dWt(ConGroup* rcg, Network* rnet, int thr_no) override {
    LeabraNetwork* net = (LeabraNetwork*)rnet;
    if(!learn || (use_unlearnable && net->unlearnable_trial)) return;
    LeabraConGroup* cg = (LeabraConGroup*)rcg;
    LeabraUnitVars* su = (LeabraUnitVars*)cg->ThrOwnUnVars(net, thr_no);
    LeabraLayer* rlay = (LeabraLayer*)cg->prjn->layer;
    BasAmygUnitSpec* rus = (BasAmygUnitSpec*)rlay->GetUnitSpec();
    bool d2r = (rus->dar == BasAmygUnitSpec::D2R);
    bool acq = (rus->acq_ext == BasAmygUnitSpec::ACQ);
    
    float su_act = su->act_q0;  // previous trial
    float* dwts = cg->OwnCnVar(DWT);
    float* cwts = cg->OwnCnVar(WT);

    float clrate, bg_lrate, fg_lrate;
    bool deep_on;
    GetLrates(cg, clrate, deep_on, bg_lrate, fg_lrate);

    const int sz = cg->size;
    for(int i=0; i<sz; i++) {
      LeabraUnitVars* ru = (LeabraUnitVars*)cg->UnVars(i, net);
      float lrate_eff = clrate;
      if(deep_on) {
        lrate_eff *= (bg_lrate + fg_lrate * ru->deep_lrn);
      }
      if(acq) {
        C_Compute_dWt_BasAmyg_Acq(dwts[i], cwts[i], su_act, ru->act_eq, ru->da_p, d2r, lrate_eff);
      }
      else {
        C_Compute_dWt_BasAmyg_Ext(dwts[i], cwts[i], su_act, ru->act_eq, ru->da_p, d2r, lrate_eff);
      }
    }
  }

  bool  CheckConfig_RecvCons(ConGroup* cg, bool quiet=false) override;

  TA_SIMPLE_BASEFUNS(BasAmygConSpec);
protected:
  SPEC_DEFAULTS;
private:
  void  Initialize();
  void  Destroy()     { };
  void	Defaults_init();
};

#endif // BasAmygConSpec_h
