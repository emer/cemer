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

#ifndef LeabraStableConSpec_h
#define LeabraStableConSpec_h 1

// parent includes:
#include <LeabraConSpec>

// member includes:
#include <LeabraNetwork>
#include <LeabraStableCon>

// declare all other types mentioned but not required to include:

eTypeDef_Of(StableMixSpec);

class E_API StableMixSpec : public SpecMemberBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra stable weight mixing specs
INHERITED(SpecMemberBase)
public:
  float		stable_pct;	// #MIN_0 #MAX_1 proportion (0..1) of the stable weight value contributing to the overall weight value
  bool          updt_to_lwt;    // update the stable swt values to the current lwt learned weight values -- otherwise updates to the current effective weights (which contain a component of the stable weights) -- updt to learn causes learning to be faster but more prone to positive feedback phenomena

  float         learn_pct;       // #READ_ONLY #SHOW proportion that learned weight contributes to the overall weight value -- automatically computed as 1 - stable_pct

  inline float	EffWt(const float swt, const float lwt)
  { return stable_pct * swt + learn_pct * lwt; }

  override String       GetTypeDecoKey() const { return "ConSpec"; }

  TA_SIMPLE_BASEFUNS(StableMixSpec);
protected:
  SPEC_DEFAULTS;
  void	UpdateAfterEdit_impl();
private:
  void	Initialize();
  void	Destroy()	{ };
  void	Defaults_init();
};

eTypeDef_Of(LeabraStableConSpec);

class E_API LeabraStableConSpec : public LeabraConSpec {
  // specs for a stable synaptic connection -- wt = effective net weight = mix of lwt that learns and a stable weight swt that is only updated periodically -- reflects short vs. long term LTP dynamics and synaptic consolidation process, and can prevent pervasive positive feedback loop dynamics that can lead to degenerate weights -- both weights are set to the saved wt value when weights are reloaded from a file
INHERITED(LeabraConSpec)
public:
  StableMixSpec         stable_mix; // mixing parameters for stable (swt) vs. learned weight (lwt) to compute the overall effective weight value (wt)

  inline void Compute_EffWt(LeabraStableCon* cn) {
    cn->wt = stable_mix.EffWt(cn->swt, cn->lwt);
  }
  // compute the effective weight from the stable and learned weights

  // note: following is called after loading weights too
  inline void   C_Init_Weights_post(BaseCons* cg, Connection* pcn, Unit* ru, Unit* su) {
    LeabraStableCon* cn = (LeabraStableCon*)pcn;
    cn->swt = cn->lwt = cn->wt;    
  }


  inline void Compute_StableWeights(LeabraSendCons* cg, LeabraUnit* su) {
    if(stable_mix.updt_to_lwt) {
      for(int i=0; i<cg->size; i++) {
        LeabraStableCon* cn = (LeabraStableCon*)cg->OwnCn(i);
        cn->swt = cn->lwt;
      }
    }
    else {
      for(int i=0; i<cg->size; i++) {
        LeabraStableCon* cn = (LeabraStableCon*)cg->OwnCn(i);
        cn->swt = cn->wt;
      }
    }
  }

  inline void Compute_dWt_LeabraCHL(LeabraSendCons* cg, LeabraUnit* su) {
    LeabraNetwork* net = (LeabraNetwork*)su->own_net();
    if(ignore_unlearnable && net && net->unlearnable_trial) return;

    Compute_SAvgCor(cg, su);
    if(((LeabraLayer*)cg->prjn->from.ptr())->acts_p.avg < savg_cor.thresh) return;

    for(int i=0; i<cg->size; i++) {
      LeabraUnit* ru = (LeabraUnit*)cg->Un(i);
      LeabraStableCon* cn = (LeabraStableCon*)cg->OwnCn(i);
      float lin_wt = LinFmSigWt(cn->lwt);
      C_Compute_dWt(cn, ru, 
                    C_Compute_Hebb(cn, cg, lin_wt, ru->act_p, su->act_p),
                    C_Compute_Err_LeabraCHL(cn, lin_wt, ru->act_p, ru->act_m,
                                            su->act_p, su->act_m));
    }
  }

  inline void C_Compute_Weights_LeabraCHL(LeabraStableCon* cn) {
    if(cn->dwt != 0.0f) {
      cn->lwt = SigFmLinWt(LinFmSigWt(cn->lwt) + cn->dwt);
      Compute_EffWt(cn);
    }
    cn->pdw = cn->dwt;
    cn->dwt = 0.0f;
  }

  inline void Compute_Weights_LeabraCHL(LeabraSendCons* cg, LeabraUnit* su) {
    CON_GROUP_LOOP(cg, C_Compute_Weights_LeabraCHL((LeabraStableCon*)cg->OwnCn(i)));
    //  ApplyLimits(cg, ru); limits are automatically enforced anyway
  }


  inline void C_Compute_Weights_CtLeabraXCAL(LeabraStableCon* cn) {
    if(cn->dwt != 0.0f) {
      // always do soft bounding, at this point (post agg across processors, etc)
      float lin_wt = LinFmSigWt(cn->lwt);
      if(cn->dwt > 0.0f) cn->dwt *= (1.0f - lin_wt);
      else		 cn->dwt *= lin_wt;
      cn->lwt = SigFmLinWt(lin_wt + cn->dwt);
      Compute_EffWt(cn);
    }
    cn->pdw = cn->dwt;
    cn->dwt = 0.0f;
  }

  inline void Compute_Weights_CtLeabraXCAL(LeabraSendCons* cg, LeabraUnit* su) {
    CON_GROUP_LOOP(cg, C_Compute_Weights_CtLeabraXCAL((LeabraStableCon*)cg->OwnCn(i)));
    //  ApplyLimits(cg, ru); limits are automatically enforced anyway
  }

  // NOTE: this is incompatible with Stable at this point..
  // inline void C_Compute_Weights_CtLeabraCAL(LeabraSRAvgCon* cn) {
  //   if(cn->dwt != 0.0f) {
  //     // always do soft bounding, at this point (post agg across processors, etc)
  //     float lin_wt = LinFmSigWt(cn->lwt);
  //     if(cn->dwt > 0.0f) cn->dwt *= (1.0f - lin_wt);
  //     else		 cn->dwt *= lin_wt;
  //     cn->lwt = SigFmLinWt(lin_wt + cn->dwt);
  //     Compute_EffWt(cn);
  //   }
  //   cn->pdw = cn->dwt;
  //   cn->dwt = 0.0f;
  // }

  // inline void Compute_Weights_CtLeabraCAL(LeabraSendCons* cg, LeabraUnit* su) {
  //   CON_GROUP_LOOP(cg, C_Compute_Weights_CtLeabraCAL((LeabraSRAvgCon*)cg->OwnCn(i)));
  //   //  ApplyLimits(cg, ru); limits are automatically enforced anyway
  // }

  // NOTE: bias weights could also have stable if incorporated into base


  TA_SIMPLE_BASEFUNS(LeabraStableConSpec);
protected:
  void	UpdateAfterEdit_impl();
private:
  void Initialize();
  void Destroy()     { };
};

#endif // LeabraStableConSpec_h
