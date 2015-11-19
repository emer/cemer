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

eTypeDef_Of(BasAmygLearnSpec);

class E_API BasAmygLearnSpec : public SpecMemberBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra basal amygdala learning specs
INHERITED(SpecMemberBase)
public:
  bool          us_delta;       // engage delta rule learning equation when an unconditioned-stimulus (primary value) is present dW = s * (us - act) -- keeps BA units tracking actual US magnitude -- receives US as deep_raw_net from SendDeepRawConSpec from corresponding US unit
  bool          delta_da;       // #CONDSHOW_ON_us_delta multiply us_delta by absolute value of dopamine 
  float         burst_da_gain;  // #MIN_0 multiplicative gain factor applied to positive dopamine signals -- this operates on the raw dopamine signal prior to any effect of D2 receptors in reversing its sign!
  float         dip_da_gain;    // #MIN_0 multiplicative gain factor applied to negative dopamine signals -- this operates on the raw dopamine signal prior to any effect of D2 receptors in reversing its sign! should be small for acq, but roughly equal to burst_da_gain for ext 
  
  String       GetTypeDecoKey() const override { return "ConSpec"; }

  TA_SIMPLE_BASEFUNS(BasAmygLearnSpec);
protected:
  SPEC_DEFAULTS;
private:
  void  Initialize();
  void  Destroy()       { };
  void  Defaults_init();
};

eTypeDef_Of(BasAmygConSpec);

class E_API BasAmygConSpec : public LeabraConSpec {
  // simulates learning in the basal amygdala, with separate equations for acquisition vs. extinction subpoplations -- acquisition recv from LatAmyg, learn from da_p and postsynaptic activity -- extinction recv from context / pfc, and learn from ACQ up-state signal and da_p using D2 receptors
INHERITED(LeabraConSpec)
public:
  BasAmygLearnSpec      ba_learn; // basal amygdala learning specs

  inline float  GetDa(float da, bool d2r) {
    if(da < 0.0f) da *= ba_learn.dip_da_gain; else da *= ba_learn.burst_da_gain;
    if(d2r) da = -da;
    return da;
  }
  // get effective dopamine signal taking into account gains and reversal by D2R

  inline void C_Compute_dWt_BasAmyg_Acq
    (float& dwt, const float su_act, const float ru_act, const float us,
     const float da_p, const bool d2r, const float lrate_eff) {
    const float da = GetDa(da_p, d2r);
    if(ba_learn.us_delta) {
      if(us > 0.01f) {
        float delta = lrate_eff * su_act * (us - ru_act);
        if(ba_learn.delta_da) {
          delta *= fabsf(da);
        }
        dwt += delta;
      }
      else {
        dwt += lrate_eff * su_act * ru_act * da;
      }
    }
    else {
      dwt += lrate_eff * su_act * ru_act * da;
    }
  }
  // #IGNORE acquisition
  inline void C_Compute_dWt_BasAmyg_Ext
    (float& dwt, const float su_act, const float ru_act, const float us,
     const float da_p, const bool d2r, const float lrate_eff) {
    const float da = GetDa(da_p, d2r);
    if(ba_learn.us_delta) {
      if(us > 0.01f) {
        float delta = lrate_eff * su_act * ((1.0f-us) - ru_act); // inverse us..
        if(ba_learn.delta_da) {
          delta *= fabsf(da);
        }
        dwt += delta;
      }
      else {
        dwt += lrate_eff * su_act * ru_act * da;
      }
    }
    else {
      dwt += lrate_eff * su_act * ru_act * da;
    }
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

    float clrate, bg_lrate, fg_lrate;
    bool deep_on;
    GetLrates(cg, clrate, deep_on, bg_lrate, fg_lrate);

    const int sz = cg->size;
    for(int i=0; i<sz; i++) {
      LeabraUnitVars* ru = (LeabraUnitVars*)cg->UnVars(i, net);
      if(acq) {
        C_Compute_dWt_BasAmyg_Acq(dwts[i], su_act, ru->act_eq, ru->deep_raw_net,
                                  ru->da_p, d2r, clrate);
      }
      else {
        // this is the key for learning: up-state or actual ru activation
        const float ru_act_eff = MAX(ru->deep_lrn, ru->act_eq);
        C_Compute_dWt_BasAmyg_Ext(dwts[i], su_act, ru_act_eff, ru->deep_raw_net,
                                  ru->da_p, d2r, clrate);
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
