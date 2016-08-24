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

#ifndef LatAmygConSpec_h
#define LatAmygConSpec_h 1

// parent includes:
#include <LeabraConSpec>

// member includes:
#include <LeabraNetwork>

// declare all other types mentioned but not required to include:

eTypeDef_Of(LatAmygGains);

class E_API LatAmygGains : public SpecMemberBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS ##CAT_Leabra specifications for lateral amygdala learning
INHERITED(SpecMemberBase)
public:
  float         burst_da_gain;  // #MIN_0 multiplicative gain factor applied to positive dopamine signals -- this operates on the raw dopamine signal prior to any effect of D2 receptors in reversing its sign!
  float         dip_da_gain;    // #MIN_0 multiplicative gain factor applied to negative dopamine signals -- this operates on the raw dopamine signal prior to any effect of D2 receptors in reversing its sign!
  float         wt_decay_rate; // decay rate (each AlphaTrial) as percentage of the pos-rectified difference between the existing weight less the wt_decay_floor
  float         wt_decay_floor;  // #DEF_0.5 minimum weight value below which no decay occurs

  String       GetTypeDecoKey() const override { return "ConSpec"; }

  TA_SIMPLE_BASEFUNS(LatAmygGains);
protected:
  SPEC_DEFAULTS;
  // void  UpdateAfterEdit_impl() override;
private:
  void	Initialize();
  void	Destroy()	{ };
  void	Defaults_init();
};

eTypeDef_Of(LatAmygConSpec);

class E_API LatAmygConSpec : public LeabraConSpec {
  // simulates learning in the lateral amygdala, based on CS-specific input weights, with learning modulated by phasic dopamine from either da_p (positive-valence) or da_n (negative valence), but predominantly the positive values of these signals. To prevent CS self-training positive feedback, the CS must generally have been active in the prior trial, using act_q0.  there is no dependence on postsynaptic activation
INHERITED(LeabraConSpec)
public:
  LatAmygGains          lat_amyg;

  inline float  GetDa(float da)
  { da = (da < 0.0f) ? lat_amyg.dip_da_gain * da : lat_amyg.burst_da_gain * da;
    da = fabsf(da);
    return da;
    //if(fwt > lat_amyg.wt_decay_floor) da -= lat_amyg.wt_decay_rate * fwt;
  }
  // get overall dopamine value
  
  inline void C_Compute_dWt_LatAmyg(float& dwt, const float su_act, const float da_p, 
                                    const float fwt)
  {
    float wt_above_floor = fwt - lat_amyg.wt_decay_floor;
    wt_above_floor = MAX(wt_above_floor,0.0f); // positive-rectify
    dwt += (cur_lrate * su_act * GetDa(da_p)) - lat_amyg.wt_decay_rate * wt_above_floor;
    //dwt += cur_lrate * su_act * GetDa(da_p, fwt);
  }
  // #IGNORE dopamine multiplication

  inline void Compute_dWt(ConGroup* rcg, Network* rnet, int thr_no) override {
    LeabraNetwork* net = (LeabraNetwork*)rnet;
    if(!learn || (use_unlearnable && net->unlearnable_trial)) return;
    LeabraConGroup* cg = (LeabraConGroup*)rcg;
    LeabraUnitVars* su = (LeabraUnitVars*)cg->ThrOwnUnVars(net, thr_no);
    
    float su_act = su->act_q0;
    float* dwts = cg->OwnCnVar(DWT);
    float* fwts = cg->OwnCnVar(FWT);

    const int sz = cg->size;
    for(int i=0; i<sz; i++) {
      LeabraUnitVars* ru = (LeabraUnitVars*)cg->UnVars(i, net);
      C_Compute_dWt_LatAmyg(dwts[i], su_act, ru->da_p, fwts[i]);
    }
  }

  TA_SIMPLE_BASEFUNS(LatAmygConSpec);
protected:
  SPEC_DEFAULTS;
private:
  void  Initialize();
  void  Destroy()     { };
  void	Defaults_init();
};

#endif // LatAmygConSpec_h
