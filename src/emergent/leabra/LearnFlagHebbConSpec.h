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

#ifndef LearnFlagHebbConSpec_h
#define LearnFlagHebbConSpec_h 1

// parent includes:
#include <LeabraConSpec>

// member includes:
#include <LeabraNetwork>

// declare all other types mentioned but not required to include:

eTypeDef_Of(LearnFlagHebbConSpec);

class E_API LearnFlagHebbConSpec : public LeabraConSpec {
  // requires recv unit to have LEARN flag activated, and also includes optional dopamine modulation, on top of a basic hebbian rule -- used for gdPVLV learning in subcortical areas
INHERITED(LeabraConSpec)
public:
  enum DaModType {
    NO_DA_MOD,                  // do not modulate the learning by recv unit dopamine value (dav)
    DA_MOD,                     // modulate the learning by multiplying directly by the recv unit dopamine value (dav) -- this will change the sign of learning as a function of the sign of the dopamine value
    DA_MOD_ABS,                 // modulate the learning by multiplying by the absolute value of the recv unit dopamine (abs(dav)) -- this does not change the sign, only the magnitude of learning
  };

  enum SendAct {                // what var to use for sending activation
    ACT_M,                      // minus phase activation
    ACT_P,                      // plus phase activation
  };

#ifdef __MAKETA__
  SAvgCorSpec	savg_cor;	// #CAT_Learning for CPCA Hebbian learning: correction for sending average act levels (i.e., renormalization)
#endif
  DaModType     da_mod;         // how does receiving unit dopamine modulate learning (or not)?
  SendAct       send_act;       // what variable to use on the sending unit for activation to enter into the delta rule learning equation

  inline void	C_Compute_dWt_Hebb_NoDa(float& dwt, const float cg_savg_cor,
                                        const float lin_wt, const float ru_act,
                                        const float su_act) {
    dwt += cur_lrate * ru_act * 
      (su_act * (cg_savg_cor - lin_wt) - (1.0f - su_act) * lin_wt);
  }
  // #IGNORE no dopamine
  inline void	C_Compute_dWt_Hebb_Da(float& dwt, const float cg_savg_cor,
                                      const float lin_wt, const float ru_act,
                                      const float su_act, const float dav) {
    dwt += cur_lrate * dav * ru_act * 
      (su_act * (cg_savg_cor - lin_wt) - (1.0f - su_act) * lin_wt);
  }
  // #IGNORE with dopamine

  inline void Compute_dWt_CtLeabraXCAL(LeabraSendCons* cg, LeabraUnit* su,
                                       LeabraNetwork* net) override {
    Compute_SAvgCor(cg, su, net);
    if(((LeabraLayer*)cg->prjn->from.ptr())->acts_p.avg < savg_cor.thresh) return;

    float* lwts = cg->OwnCnVar(LWT);
    float* dwts = cg->OwnCnVar(DWT);
    float su_act;
    if(send_act == ACT_M)
      su_act = su->act_m;
    else
      su_act = su->act_p;

    const int sz = cg->size;
    if(da_mod == NO_DA_MOD) {
      for(int i=0; i<sz; i++) {
        LeabraUnit* ru = (LeabraUnit*)cg->Un(i,net);
        if(!ru->HasLearnFlag()) continue; // must have this flag to learn
        const float lin_wt = LinFmSigWt(lwts[i]);
        C_Compute_dWt_Hebb_NoDa(dwts[i], cg->savg_cor, lin_wt, ru->act_p, su_act);
      }
    }
    else if(da_mod == DA_MOD) {
      for(int i=0; i<sz; i++) {
        LeabraUnit* ru = (LeabraUnit*)cg->Un(i,net);
        if(!ru->HasLearnFlag()) continue; // must have this flag to learn
        const float lin_wt = LinFmSigWt(lwts[i]);
        C_Compute_dWt_Hebb_Da(dwts[i], cg->savg_cor, lin_wt, ru->act_p, su_act, ru->dav);
      }
    }
    else {                      // DA_MOD_ABS
      for(int i=0; i<sz; i++) {
        LeabraUnit* ru = (LeabraUnit*)cg->Un(i,net);
        if(!ru->HasLearnFlag()) continue; // must have this flag to learn
        const float lin_wt = LinFmSigWt(lwts[i]);
        C_Compute_dWt_Hebb_Da(dwts[i], cg->savg_cor, lin_wt, ru->act_p, su_act,
                                fabsf(ru->dav));
      }
    }
  }

  inline void Compute_dWt_LeabraCHL(LeabraSendCons* cg, LeabraUnit* su,
                                             LeabraNetwork* net) override {
    Compute_dWt_CtLeabraXCAL(cg, su, net);
  }

  inline void Compute_dWt_CtLeabraCAL(LeabraSendCons* cg, LeabraUnit* su,
                                               LeabraNetwork* net) override {
    Compute_dWt_CtLeabraXCAL(cg, su, net);
  }

  inline void Compute_Weights_LeabraCHL(LeabraSendCons* cg, LeabraUnit* su,
                                                 LeabraNetwork* net) override {
    Compute_Weights_CtLeabraXCAL(cg, su, net); // do soft bound here
  }

  TA_SIMPLE_BASEFUNS(LearnFlagHebbConSpec);
protected:
  SPEC_DEFAULTS;
private:
  void  Initialize();
  void  Destroy()     { };
  void	Defaults_init();
};

#endif // LearnFlagHebbConSpec_h
