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

#ifndef MatrixConSpec_h
#define MatrixConSpec_h 1

// parent includes:
#include <LeabraConSpec>

// member includes:
#include <LeabraNetwork>
#include <MatrixCon>

// declare all other types mentioned but not required to include:

eTypeDef_Of(MatrixConSpec);

class E_API MatrixConSpec : public LeabraConSpec {
  // Learning of matrix input connections based on dopamine modulation of activation -- for both Matrix_Go and NoGo connections
INHERITED(LeabraConSpec)
public:
  enum MtxLearnMode {           // how does the matrix learn?
    TRACE,                      // learn from trace of gating activation modulated by PV dopamine
    LV_DA,                      // use PVLV LV dopamine, which anticipates positive outcomes
  };

  MtxLearnMode  mtx_learn;       // how do the matrix units learn?  can either use a trace mechanism or pure PVLV dopamine
  float         dwt_remain;      // how much of the dwt value remains after the weights are updated (i.e., every time there is a PV trial)

  inline void Compute_SuLearnAct(LeabraRecvCons* cg, LeabraUnit* ru) {
    for(int i=0; i<cg->size; i++) {
      LeabraUnit* su = (LeabraUnit*)cg->Un(i);
      MatrixCon* cn = (MatrixCon*)cg->PtrCn(i);
      cn->sact_lrn = su->act_eq;
    }
  }
  // RECV-based save current sender activation states to sact_lrn for subsequent learning -- call this at time of gating

  inline override void Compute_SRAvg(LeabraSendCons* cg, LeabraUnit* su, bool do_s) {
    // do NOT do this under any circumstances!!
  }

  // everything can use one dwt with post-soft-bound because no hebbian term
  inline void C_Compute_dWt_Matrix_Trace(LeabraCon* cn,
                                         float mtx_act, float su_act) {
    float dwt = mtx_act * su_act;
    cn->dwt = cur_lrate * dwt;  // note: =, not += -- always learn last gating action
  }

  inline void C_Compute_dWt_Matrix_LvDa(LeabraCon* cn, 
                                        float mtx_da, float mtx_act, float su_act) {
    float dwt = mtx_da * mtx_act * su_act;
    cn->dwt += cur_lrate * dwt;
  }

  inline override void Compute_dWt_CtLeabraXCAL(LeabraSendCons* cg, LeabraUnit* su) {
    LeabraNetwork* net = (LeabraNetwork*)su->own_net();
    if(ignore_unlearnable && net && net->unlearnable_trial) return;

    if(mtx_learn == TRACE) {
      for(int i=0; i<cg->size; i++) {
        LeabraUnit* ru = (LeabraUnit*)cg->Un(i);
        MatrixCon* cn = (MatrixCon*)cg->OwnCn(i);
        if(ru->misc_1 == 0.0f) continue; // signal for gating for this stripe
        C_Compute_dWt_Matrix_Trace(cn, ru->act_mid, cn->sact_lrn);
      }
    }
    else {                      // LV_DA
      for(int i=0; i<cg->size; i++) {
        LeabraUnit* ru = (LeabraUnit*)cg->Un(i);
        MatrixCon* cn = (MatrixCon*)cg->OwnCn(i);
        if(ru->misc_1 == 0.0f) continue; // signal for gating for this stripe
        C_Compute_dWt_Matrix_LvDa(cn, ru->dav, ru->act_mid, cn->sact_lrn);
      }
    }
  }

  inline override void Compute_dWt_LeabraCHL(LeabraSendCons* cg, LeabraUnit* su) {
    Compute_dWt_CtLeabraXCAL(cg, su);
  }

  inline override void Compute_dWt_CtLeabraCAL(LeabraSendCons* cg, LeabraUnit* su) {
    Compute_dWt_CtLeabraXCAL(cg, su);
  }

  inline void C_Compute_Weights_Matrix_Trace(LeabraCon* cn, float ru_dav) {
    if(cn->dwt != 0.0f) {
      // always do soft bounding, at this point (post agg across processors, etc)
      // PV dav modulates the prior su product dwts -- skipping the LV middleman..
      float dwt = ru_dav * cn->dwt;
      float lin_wt = LinFmSigWt(cn->lwt);
      // always do soft bounding
      if(dwt > 0.0f)	dwt *= (1.0f - lin_wt);
      else		dwt *= lin_wt;
      cn->lwt = SigFmLinWt(lin_wt + dwt);
      Compute_EffWt(cn);
      cn->pdw = dwt;
      cn->dwt *= dwt_remain;    // gradually dissappears
    }
  }

  inline override void Compute_Weights_CtLeabraXCAL(LeabraSendCons* cg, LeabraUnit* su) {
    if(mtx_learn == TRACE) {
      for(int i=0; i<cg->size; i++) {
        LeabraUnit* ru = (LeabraUnit*)cg->Un(i);
        if(ru->dav != 0.0f) {
          LeabraCon* cn = (LeabraCon*)cg->OwnCn(i);
          C_Compute_Weights_Matrix_Trace(cn, ru->dav);
        }
        //  ApplyLimits(cg, ru); limits are automatically enforced anyway
      }
    }
    else {
      inherited::Compute_Weights_CtLeabraXCAL(cg, su); // use std
    }
  }

  inline override void Compute_Weights_LeabraCHL(LeabraSendCons* cg, LeabraUnit* su) {
    Compute_Weights_CtLeabraXCAL(cg, su);
  }
  inline override void Compute_Weights_CtLeabraCAL(LeabraSendCons* cg, LeabraUnit* su) {
    Compute_Weights_CtLeabraXCAL(cg, su);
  }

  TA_SIMPLE_BASEFUNS(MatrixConSpec);
protected:
  SPEC_DEFAULTS;
  void	UpdateAfterEdit_impl();
private:
  void 	Initialize();
  void	Destroy()		{ };
  void	Defaults_init();
};

#endif // MatrixConSpec_h
