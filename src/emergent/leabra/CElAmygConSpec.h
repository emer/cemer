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

#ifndef CElAmygConSpec_h
#define CElAmygConSpec_h 1

// parent includes:
#include <LeabraConSpec>

// member includes:
#include <LeabraNetwork>

// declare all other types mentioned but not required to include:

eTypeDef_Of(CElAmygLearnSpec);

class E_API CElAmygLearnSpec : public SpecMemberBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS ##CAT_Leabra specifications for gain factors and other learning parameters in CEl central amygdala lateral learning
INHERITED(SpecMemberBase)
public:
  float         dalr_gain;      // gain multiplier on abs(da) learning rate multiplier
  float         dalr_base;      // constant baseline amount of learning prior to abs(da) factor -- should in general be small to preserve da-modulation
  
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
  // simulates learning in the central amygdala (lateral), with separate equations for acquisition vs. extinction subpoplations 
INHERITED(LeabraConSpec)
public:
  CElAmygLearnSpec   cel_learn; // central amygdala, lateral learning specs

  inline void C_Compute_dWt_CEl_Delta
    (float& dwt, const float su_act, const float ru_act, const float ru_act_prv,
     const float da_p, const float lrate_eff) {
    float delta = lrate_eff * su_act * (ru_act - ru_act_prv);
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
      C_Compute_dWt_CEl_Delta(dwts[i], su_act, ru->act_eq, ru->act_q0, ru->da_p, clrate);
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
