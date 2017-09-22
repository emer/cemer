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

#ifndef CElAmygConSpec_h
#define CElAmygConSpec_h 1

// parent includes:
#include <LeabraConSpec>

// member includes:
#include <LeabraNetwork>

// declare all other types mentioned but not required to include:

eTypeDef_Of(CElAmygLearnSpec);

class E_API CElAmygLearnSpec : public SpecMemberBase {
  // ##INLINE ##NO_TOKENS ##CAT_Leabra specifications for gain factors and other learning parameters in CEl central amygdala lateral learning
INHERITED(SpecMemberBase)
public:
  float         dalr_gain;      // #DEF_1 gain multiplier on abs(da) learning rate multiplier
  float         dalr_base;      // #DEF_0 constant baseline amount of learning prior to abs(da) factor -- should be near zero otherwise offsets in activation will drive learning in the absence of da significance
  
  // NEW GUYS
  float         da_lrn_thr;    // #DEF_0.02 minimum threshold for phasic abs(da) signals to count as non-zero;  useful to screen out spurious da signals due to tiny VSPatch-to-LHb signals on t2 & t4 timesteps that can accumulate over many trials - 0.02f seems to work okay
  float         act_delta_thr; // #DEF_0.02 minimum threshold for delta activation to count as non-zero;  useful to screen out spurious learning due to unintended delta activity - 0.02f seems to work okay for both acquisition and extinction guys
  // END NEW GUYS
  
  String       GetTypeDecoKey() const override { return "ConSpec"; }

  TA_SIMPLE_BASEFUNS(CElAmygLearnSpec);
protected:
  SPEC_DEFAULTS;
  // void  UpdateAfterEdit_impl() override;
private:
  void	Initialize();
  void	Destroy()	{ };
  void	Defaults_init();
};


eTypeDef_Of(CElAmygConSpec);

class E_API CElAmygConSpec : public LeabraConSpec {
  // simulates learning in the central amygdala (lateral) using a simple delta-rule between this trial and previous trial, modulated also by absolute value of phasic dopamine -- delta influence comes from unit spec dopamine da_mod and US drive
INHERITED(LeabraConSpec)
public:
  CElAmygLearnSpec   cel_learn; // central amygdala, lateral learning specs

  inline void C_Compute_dWt_CEl_Delta
    (float& dwt, const float su_act, const float ru_act, const float ru_act_prv,
     const float da_p, const float lrate_eff) {
    
    float ru_act_delta = ru_act - ru_act_prv;
    if(fabsf(ru_act_delta) < cel_learn.act_delta_thr) { ru_act_delta = 0.0f; }
    float delta = lrate_eff * su_act * (ru_act_delta);
    // dopamine signal further modulates learning
    float da_lrate = cel_learn.dalr_base + cel_learn.dalr_gain * fabsf(da_p);
    dwt += da_lrate * delta;
  }
  // #IGNORE abs(da) modulated delta learning

  inline void Compute_dWt(ConGroup* rcg, Network* rnet, int thr_no) override {
    LeabraNetwork* net = (LeabraNetwork*)rnet;
    if(!learn || (use_unlearnable && net->unlearnable_trial)) return;
    LeabraConGroup* cg = (LeabraConGroup*)rcg;
    LeabraUnitVars* su = (LeabraUnitVars*)cg->ThrOwnUnVars(net, thr_no);
    LeabraLayer* rlay = (LeabraLayer*)cg->prjn->layer;
    // BasAmygUnitSpec* rus = (BasAmygUnitSpec*)rlay->GetUnitSpec();
    // bool d2r = (rus->dar == BasAmygUnitSpec::D2R);
    // bool acq = (rus->acq_ext == BasAmygUnitSpec::ACQ);
    
    float su_act = su->act_q0;  // previous trial
    float* dwts = cg->OwnCnVar(DWT);

    float clrate, bg_lrate, fg_lrate;
    bool deep_on;
    GetLrates(cg, clrate, deep_on, bg_lrate, fg_lrate);

    const int sz = cg->size;
    
    for(int i=0; i<sz; i++) {
      LeabraUnitVars* ru = (LeabraUnitVars*)cg->UnVars(i, net);
      // CElAmygConSpec* rus = ru->TODO...;
      
      // screen out spurious da signals due to tiny VSPatch-to-LHb signals on t2 & t4 timesteps
      float ru_da_p = ru->da_p;
      if(fabsf(ru_da_p) < cel_learn.da_lrn_thr) { ru_da_p = 0.0f; }
      
      // TODO: need if(rus->cel_da_mod.lrn_fm_act_eq) ...
      // use US value as current ru act, if present
      // now redundant since done in Compute_ActFun_Rate() with da_mod
//      float ru_act = ru->act_eq;
//      if(ru->deep_raw_net > 0.01f) { ru_act = ru->deep_raw_net; }
//      C_Compute_dWt_CEl_Delta(dwts[i], su_act, ru_act, ru->act_q0, ru_da_p, clrate);
      //
      C_Compute_dWt_CEl_Delta(dwts[i], su_act, ru->act_eq, ru->act_q0, ru_da_p, clrate);
    }
  }

  bool  CheckConfig_RecvCons(Projection* prjn, bool quiet=false) override;

  TA_SIMPLE_BASEFUNS(CElAmygConSpec);
protected:
  SPEC_DEFAULTS;
private:
  void  Initialize();
  void  Destroy()     { };
  void	Defaults_init();
};


#endif // CElAmygConSpec_h
