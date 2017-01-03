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

#ifndef BLAmygConSpec_h
#define BLAmygConSpec_h 1

// parent includes:
#include <LeabraConSpec>

// member includes:
#include <LeabraNetwork>

// declare all other types mentioned but not required to include:

eTypeDef_Of(BLAmygLearnSpec);

class E_API BLAmygLearnSpec : public SpecMemberBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS ##CAT_Leabra specifications for gain factors and other learning parameters in Basal Lateral Amygdala learning
INHERITED(SpecMemberBase)
public:
  float         dalr_gain;      // gain multiplier on abs(da) learning rate multiplier
  float         dalr_base;      // constant baseline amount of learning prior to abs(da) factor -- should be near zero otherwise offsets in activation will drive learning in the absence of da significance
  float         wt_decay;       // decay rate (applied each weight change, typically AlphaTrial) as proportion of the weight value above the weight floor
  float         wt_floor;       // #DEF_0.5 minimum weight value below which no decay occurs
  
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
    float delta = lrate_eff * su_act * (ru_act - ru_act_prv);
    float da_lrate = bla_learn.dalr_base + bla_learn.dalr_gain * fabsf(da_p);
    float wt_decay_base = fmaxf(0.0f, (wt - bla_learn.wt_floor));
    dwt += da_lrate * delta - wt_decay_base * bla_learn.wt_decay;
  }
  // #IGNORE abs(da) modulated delta learning with weight decay

  inline void Compute_dWt(ConGroup* rcg, Network* rnet, int thr_no) override {
    LeabraNetwork* net = (LeabraNetwork*)rnet;
    if(!learn || (use_unlearnable && net->unlearnable_trial)) return;
    LeabraConGroup* cg = (LeabraConGroup*)rcg;
    LeabraUnitVars* su = (LeabraUnitVars*)cg->ThrOwnUnVars(net, thr_no);
    LeabraLayer* rlay = (LeabraLayer*)cg->prjn->layer;
    
    float su_act = su->act_q0;  // previous trial
    float* dwts = cg->OwnCnVar(DWT);
    float* wts = cg->OwnCnVar(WT);

    float clrate, bg_lrate, fg_lrate;
    bool deep_on;
    GetLrates(cg, clrate, deep_on, bg_lrate, fg_lrate);

    const int sz = cg->size;
    
    for(int i=0; i<sz; i++) {
      LeabraUnitVars* ru = (LeabraUnitVars*)cg->UnVars(i, net);
      C_Compute_dWt_BLA_Delta(dwts[i], su_act, ru->act_eq, ru->act_q0, ru->da_p, clrate,
                              wts[i]);
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
