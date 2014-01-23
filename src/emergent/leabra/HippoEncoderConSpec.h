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

#ifndef HippoEncoderConSpec_h
#define HippoEncoderConSpec_h 1

// parent includes:
#include <LeabraConSpec>

// member includes:
#include <LeabraNetwork>

// declare all other types mentioned but not required to include:

eTypeDef_Of(HippoEncoderConSpec);

class E_API HippoEncoderConSpec : public LeabraConSpec {
  // for EC <-> CA1 connections: CHL learning on encoder variables (ru_act_p vs. ru_act_mid) -- soft bounding as specified in spec
INHERITED(LeabraConSpec)
public:
#ifdef __MAKETA__
  LearnMixSpec	lmix;		// #CAT_Learning mixture of hebbian & err-driven learning 
#endif

  inline void Compute_dWt_LeabraCHL(LeabraSendCons* cg, LeabraUnit* su,
                                             LeabraNetwork* net) override {
    if(ignore_unlearnable && net->unlearnable_trial) return;

    Compute_SAvgCor(cg, su, net);
    if(((LeabraLayer*)cg->prjn->from.ptr())->acts_p.avg < savg_cor.thresh) return;

    float* lwts = cg->OwnCnVar(LWT);
    float* dwts = cg->OwnCnVar(DWT);

    const int sz = cg->size;
    for(int i=0; i<sz; i++) {
      LeabraUnit* ru = (LeabraUnit*)cg->Un(i, net);
      const float lin_wt = LinFmSigWt(lwts[i]);
      C_Compute_dWt_LeabraCHL(dwts[i],
		    C_Compute_Hebb(cg->savg_cor, lin_wt, ru->act_p, su->act_p),
                              // only diff: replaces act_mid instead of act_m
                    C_Compute_Err_LeabraCHL(lin_wt, ru->act_p, ru->act_mid,
					    su->act_p, su->act_mid));  
    }
  }

  inline void Compute_dWt_CtLeabraXCAL(LeabraSendCons* cg, LeabraUnit* su,
                                                LeabraNetwork* net) override {
    Compute_dWt_LeabraCHL(cg, su, net);
  }
  inline void Compute_dWt_CtLeabraCAL(LeabraSendCons* cg, LeabraUnit* su,
                                               LeabraNetwork* net) override {
    Compute_dWt_LeabraCHL(cg, su, net);
  }
  inline void	Compute_Weights_CtLeabraXCAL(LeabraSendCons* cg, LeabraUnit* su,
                                                     LeabraNetwork* net) override {
    inherited::Compute_Weights_LeabraCHL(cg, su, net);
  }
  inline void	Compute_Weights_CtLeabraCAL(LeabraSendCons* cg, LeabraUnit* su,
                                                    LeabraNetwork* net) override {
    inherited::Compute_Weights_LeabraCHL(cg, su, net);
  }

  TA_BASEFUNS_NOCOPY(HippoEncoderConSpec);
protected:
  SPEC_DEFAULTS;
  void	UpdateAfterEdit_impl();
private:
  void 	Initialize();
  void	Destroy()		{ };
  void	Defaults_init() 	{ Initialize(); }
};

#endif // HippoEncoderConSpec_h
