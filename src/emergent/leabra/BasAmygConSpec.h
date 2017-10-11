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
  // ##INLINE ##NO_TOKENS ##CAT_Leabra basal amygdala learning specs
INHERITED(SpecMemberBase)
public:
  enum BaLearnRule {
    DELTA,                      // fully general delta rule learning: su * (ru->act_eq - ru->act_q0) -- delta relative to activation on prior trial -- use da mod of activation to also account for dopamine effects
    US_DELTA,                   // earlier form of delta rule based on difference between US (PV) and current activation -- only used when a PV signal is present  -- keeps BA units tracking actual US magnitude -- receives US as deep_raw_net from SendDeepRawConSpec from corresponding US unit
  };
    
  BaLearnRule   learn_rule;     // #DEF_DELTA learning rule -- delta is most general and deals with second-order conditioning and contrastive learning of CS-US associations -- US_DELTA is now obsolete but avail for backwards compatibility
  float         delta_neg_lrate; // #CONDSHOW_ON_learn_rule:DELTA learning rate factor for negative delta 
  float         delta_da_gain;   // #CONDSHOW_ON_learn_rule:DELTA how much does absolute value of dopamine contribute to learning
  float         delta_da_base;  // #CONDSHOW_ON_learn_rule:DELTA constant baseline learning rate -- da_gain adds to this
  bool          delta_da;       // #CONDSHOW_ON_learn_rule:US_DELTA multiply us_delta by absolute value of dopamine 
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

  inline void C_Compute_dWt_BasAmyg_Delta
    (float& dwt, const float su_act, const float ru_act, const float ru_act_prv,
     const float da_p, const bool d2r, const float lrate_eff) {
    const float da = GetDa(da_p, d2r);
    float delta = lrate_eff * su_act * (ru_act - ru_act_prv);
    if(delta < 0.0f)
      delta *= ba_learn.delta_neg_lrate;
    float da_lrate = ba_learn.delta_da_base + ba_learn.delta_da_gain * fabsf(da);
    dwt += da_lrate * delta;
  }
  // #IGNORE basic delta
  
  inline void C_Compute_dWt_BasAmyg_Acq_UsDelta
    (float& dwt, const float su_act, const float ru_act, const float us,
     const float da_p, const bool d2r, const float lrate_eff) {
    const float da = GetDa(da_p, d2r);
    if(us > 0.01f) {
      float delta = lrate_eff * su_act * (us - ru_act);
      if(ba_learn.delta_da) {
        delta *= fabsf(da);
      }
      dwt += delta;
    }
    else { // using us_delta, but no US present this timestep
      dwt += lrate_eff * su_act * ru_act * da;
    }
  }
  // #IGNORE: acq us delta

  inline void C_Compute_dWt_BasAmyg_Acq_DaSuRu
    (float& dwt, const float su_act, const float ru_act, const float us,
     const float da_p, const bool d2r, const float lrate_eff) {
    const float da = GetDa(da_p, d2r);
    dwt += lrate_eff * su_act * fmaxf(us, ru_act) * da;
  }
  // #IGNORE: not used -- here for reference

  inline void C_Compute_dWt_BasAmyg_Ext_UsDelta
    (float& dwt, const float su_act, const float ru_act, const float us,
     const float da_p, const bool d2r, const float lrate_eff) {
    const float da = GetDa(da_p, d2r);
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
  // #IGNORE ext us delta
  inline void C_Compute_dWt_BasAmyg_Ext_DaSuRu
    (float& dwt, const float su_act, const float ru_act, const float us,
     const float da_p, const bool d2r, const float lrate_eff) {
    const float da = GetDa(da_p, d2r);
    dwt += lrate_eff * su_act * ru_act * da;
  }
  // #IGNORE not used -- here for reference

  inline void Compute_dWt(ConState* rcg, Network* rnet, int thr_no) override {
    LeabraNetwork* net = (LeabraNetwork*)rnet;
    if(!learn || (use_unlearnable && net->unlearnable_trial)) return;
    LeabraConState_cpp* cg = (LeabraConState_cpp*)rcg;
    LeabraUnitState_cpp* su = (LeabraUnitState_cpp*)cg->ThrOwnUnState(net, thr_no);
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
    
    if(ba_learn.learn_rule == BasAmygLearnSpec::DELTA) {
      for(int i=0; i<sz; i++) {
        LeabraUnitState_cpp* ru = (LeabraUnitState_cpp*)cg->UnState(i, net);
        if(acq) {
          C_Compute_dWt_BasAmyg_Delta(dwts[i], su_act, ru->act_eq, ru->act_q0, ru->da_p, d2r, clrate);
        }
        else {
          const float ru_act_eff = fmaxf(ru->deep_lrn, ru->act_eq);
          C_Compute_dWt_BasAmyg_Delta(dwts[i], su_act, ru_act_eff, ru->act_q0, ru->da_p, d2r, clrate);
        }
      }
    }
    else {
      for(int i=0; i<sz; i++) {
        LeabraUnitState_cpp* ru = (LeabraUnitState_cpp*)cg->UnState(i, net);
        if(acq) {
          C_Compute_dWt_BasAmyg_Acq_UsDelta(dwts[i], su_act, ru->act_eq, ru->deep_raw_net,
                                            ru->da_p, d2r, clrate);
        }
        else {
          // this is the key for learning: up-state or actual ru activation
          const float ru_act_eff = fmaxf(ru->deep_lrn, ru->act_eq);
          C_Compute_dWt_BasAmyg_Ext_UsDelta(dwts[i], su_act, ru_act_eff, ru->deep_raw_net,
                                            ru->da_p, d2r, clrate);
        }
      }
    }
  }

  bool  CheckConfig_RecvCons(Projection* prjn, bool quiet=false) override;

  TA_SIMPLE_BASEFUNS(BasAmygConSpec);
protected:
  SPEC_DEFAULTS;
private:
  void  Initialize();
  void  Destroy()     { };
  void	Defaults_init();
};

#endif // BasAmygConSpec_h
