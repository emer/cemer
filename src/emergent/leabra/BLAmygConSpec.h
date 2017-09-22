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

#ifndef BLAmygConSpec_h
#define BLAmygConSpec_h 1

// parent includes:
#include <LeabraConSpec>

// member includes:
#include <LeabraNetwork>

// declare all other types mentioned but not required to include:

eTypeDef_Of(BLAmygLearnSpec);

class E_API BLAmygLearnSpec : public SpecMemberBase {
  // ##INLINE ##NO_TOKENS ##CAT_Leabra specifications for gain factors and other learning parameters in Basal Lateral Amygdala learning
INHERITED(SpecMemberBase)
public:
  float         dalr_gain;      // #DEF_1 gain multiplier on abs(da) learning rate multiplier
  float         dalr_base;      // #DEF_0 constant baseline amount of learning prior to abs(da) factor -- should be near zero otherwise offsets in activation will drive learning in the absence of da significance
  
  float         da_lrn_thr;     // #DEF_0.02 minimum threshold for phasic abs(da) signals to count as non-zero;  useful to screen out spurious da signals due to tiny VSPatch-to-LHb signals on t2 & t4 timesteps that can accumulate over many trials - 0.02f seems to work okay
  float         act_delta_thr; // minimum threshold for delta activation to count as non-zero;  useful to screen out spurious learning due to unintended delta activity - 0.05f seems to work okay for acquisition guys, 0.02 for extinction guys
  
  bool          deep_lrn_mod; // #DEF_true if true, recv unit deep_lrn value modulates learning
  
  float         deep_lrn_thr; // #CONDSHOW_ON_deep_lrn_mod #DEF_0.05 only ru->deep_lrn values > this get to learn - 0.05f seems to work okay
  
  String       GetTypeDecoKey() const override { return "ConSpec"; }

  TA_SIMPLE_BASEFUNS(BLAmygLearnSpec);
protected:
  SPEC_DEFAULTS;
  // void  UpdateAfterEdit_impl() override;
private:
  void	Initialize();
  void	Destroy()	{ };
  void	Defaults_init();
};


eTypeDef_Of(BLAmygConSpec);

class E_API BLAmygConSpec : public LeabraConSpec {
  // simulates learning in the Basal Lateral Amygdala using a simple delta-rule between this trial and previous trial, modulated also by absolute value of phasic dopamine -- delta influence comes from unit spec dopamine da_mod and US drive
INHERITED(LeabraConSpec)
public:
  BLAmygLearnSpec   bla_learn; // central amygdala, lateral learning specs

  inline void C_Compute_dWt_BLA_Delta
    (float& dwt, const float su_act, const float ru_act, const float ru_act_prv,
    const float da_p, const float lrate_eff, const float wt) {
      
    float ru_act_delta = ru_act - ru_act_prv;
     
    // filter tiny spurious delta act signals - needed especially for acq guys w/
    // non-zero dalr_base value
    if(fabsf(ru_act_delta) < bla_learn.act_delta_thr) { ru_act_delta = 0.0f; }
    float delta = lrate_eff * su_act * (ru_act_delta);
    
    float da_lrate = bla_learn.dalr_base + bla_learn.dalr_gain * fabsf(da_p);
    dwt += da_lrate * delta;
  }
  // #IGNORE abs(da) modulated delta learning

  inline void Compute_dWt(ConGroup* rcg, Network* rnet, int thr_no) override {
    LeabraNetwork* net = (LeabraNetwork*)rnet;
    if(!learn || (use_unlearnable && net->unlearnable_trial)) return;
    LeabraConGroup* cg = (LeabraConGroup*)rcg;
    LeabraUnitVars* su = (LeabraUnitVars*)cg->ThrOwnUnVars(net, thr_no);
    LeabraLayer* rlay = (LeabraLayer*)cg->prjn->layer;
    
    float su_act = su->act_q0;  // previous trial
    float* dwts = cg->OwnCnVar(DWT);
    float* wts = cg->OwnCnVar(WT);
    float* scales = cg->OwnCnVar(SCALE);

    float clrate, bg_lrate, fg_lrate;
    bool deep_on;
    GetLrates(cg, clrate, deep_on, bg_lrate, fg_lrate);

    const int sz = cg->size;
    
    for(int i=0; i<sz; i++) {
      LeabraUnitVars* ru = (LeabraUnitVars*)cg->UnVars(i, net);
      float lrate_eff = clrate; // start fresh each time thru for_loop
      
      // learning dependent on non-zero deep_lrn
      if(bla_learn.deep_lrn_mod) {
        //lrate_eff *= (bg_lrate + fg_lrate * ru->deep_lrn);
        float eff_deep_lrn = 0.0f;
        if(ru->deep_lrn > bla_learn.deep_lrn_thr) {
          eff_deep_lrn = 1.0f;
        }
        else {
          eff_deep_lrn = 0.0f;
        }
        lrate_eff *= eff_deep_lrn;
      }
      
      // filter any tiny spurious da signals on t2 & t4 trials - best for ext guys since
      // they have zero dalr_base value
      float ru_da_p = ru->da_p;
      if(fabsf(ru_da_p) < bla_learn.da_lrn_thr) { ru_da_p = 0.0f; }
      
      C_Compute_dWt_BLA_Delta(dwts[i], su_act, ru->act_eq, ru->act_q0, ru_da_p, lrate_eff,
                              wts[i] / scales[i]);
    }
  }

  bool  CheckConfig_RecvCons(Projection* prjn, bool quiet=false) override;

  TA_SIMPLE_BASEFUNS(BLAmygConSpec);
protected:
  SPEC_DEFAULTS;
private:
  void  Initialize();
  void  Destroy()     { };
  void	Defaults_init();
};

#endif // BLAmygConSpec_h
