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
  enum MtxConVars {
    SACT_LRN = SWT+1,           // sending activation for learning
  };

  MtxLearnMode  mtx_learn;       // how do the matrix units learn?  can either use a trace mechanism or pure PVLV dopamine
  float         dwt_remain;      // how much of the dwt value remains after the weights are updated (i.e., every time there is a PV trial)

  inline void Compute_SuLearnAct(LeabraRecvCons* cg, LeabraUnit* ru, LeabraNetwork* net) {
    const int sz = cg->size;
    for(int i=0; i<sz; i++) {
      LeabraUnit* su = (LeabraUnit*)cg->Un(i,net);
      cg->PtrCn(i, SACT_LRN, net) = su->act_eq;
    }
  }
  // #IGNORE RECV-based save current sender activation states to sact_lrn for subsequent learning -- call this at time of gating

  inline override void Compute_SRAvg(LeabraSendCons* cg, LeabraUnit* su,
                                     LeabraNetwork* net, const bool do_s) {
    // do NOT do this under any circumstances!!
  }

  // everything can use one dwt with post-soft-bound because no hebbian term
  inline void C_Compute_dWt_Matrix_Trace(float& dwt, 
                                         const float mtx_act, const float su_act) {
    dwt = cur_lrate * mtx_act * su_act;  // note: =, not += -- always learn last gating action
  }
  // #IGNORE

  inline void C_Compute_dWt_Matrix_LvDa(float& dwt, const float mtx_da,
                                        const float mtx_act, const float su_act) {
    dwt += cur_lrate * mtx_da * mtx_act * su_act;
  }
  // #IGNORE

  inline override void Compute_dWt_CtLeabraXCAL(LeabraSendCons* cg, LeabraUnit* su,
                                                LeabraNetwork* net) {
    if(ignore_unlearnable && net->unlearnable_trial) return;

    float* dwts = cg->OwnCnVar(DWT);
    float* sacts = cg->OwnCnVar(SACT_LRN);

    if(mtx_learn == TRACE) {
      const int sz = cg->size;
      for(int i=0; i<sz; i++) {
        LeabraUnit* ru = (LeabraUnit*)cg->Un(i,net);
        if(!ru->HasUnitFlag(Unit::LEARN)) continue; // signal for gating for this stripe
        C_Compute_dWt_Matrix_Trace(dwts[i], ru->act_mid, sacts[i]);
      }
    }
    else {                      // LV_DA
      const int sz = cg->size;
      for(int i=0; i<sz; i++) {
        LeabraUnit* ru = (LeabraUnit*)cg->Un(i,net);
        if(!ru->HasUnitFlag(Unit::LEARN)) continue; // signal for gating for this stripe
        C_Compute_dWt_Matrix_LvDa(dwts[i], ru->dav, ru->act_mid, sacts[i]);
      }
    }
  }

  inline override void Compute_dWt_LeabraCHL(LeabraSendCons* cg, LeabraUnit* su,
                                             LeabraNetwork* net) {
    Compute_dWt_CtLeabraXCAL(cg, su, net);
  }

  inline override void Compute_dWt_CtLeabraCAL(LeabraSendCons* cg, LeabraUnit* su,
                                               LeabraNetwork* net) {
    Compute_dWt_CtLeabraXCAL(cg, su, net);
  }

  inline void C_Compute_Weights_Matrix_Trace(float& wt, float& dwt, float& pdw,
                                             float& lwt, const float swt,
                                             const float ru_dav) {
    if(dwt != 0.0f) {
      // PV dav modulates the prior su product dwts -- skipping the LV middleman..
      float tmp = ru_dav * dwt;
      float lin_wt = LinFmSigWt(lwt);
      // always do soft bounding
      if(tmp > 0.0f)	tmp *= (1.0f - lin_wt);
      else		tmp *= lin_wt;
      lwt = SigFmLinWt(lin_wt + tmp);
      C_Compute_EffWt(wt, swt, lwt);
      pdw = tmp;
      dwt *= dwt_remain;    // gradually dissappears
    }
  }
  // #IGNORE

  inline override void Compute_Weights_CtLeabraXCAL(LeabraSendCons* cg, LeabraUnit* su,
                                                    LeabraNetwork* net) {
    if(mtx_learn == TRACE) {
      float* wts = cg->OwnCnVar(WT);
      float* dwts = cg->OwnCnVar(DWT);
      float* pdws = cg->OwnCnVar(PDW);
      float* lwts = cg->OwnCnVar(LWT);
      float* swts = cg->OwnCnVar(SWT);

      const int sz = cg->size;
      for(int i=0; i<sz; i++) {
        LeabraUnit* ru = (LeabraUnit*)cg->Un(i,net);
        if(ru->dav != 0.0f) {
          C_Compute_Weights_Matrix_Trace(wts[i], dwts[i], pdws[i], 
                                         lwts[i], swts[i], ru->dav);
        }
        //  ApplyLimits(cg, ru, net); limits are automatically enforced anyway
      }
    }
    else {
      inherited::Compute_Weights_CtLeabraXCAL(cg, su, net); // use std
    }
  }

  inline override void Compute_Weights_LeabraCHL(LeabraSendCons* cg, LeabraUnit* su,
                                                 LeabraNetwork* net) {
    Compute_Weights_CtLeabraXCAL(cg, su, net);
  }
  inline override void Compute_Weights_CtLeabraCAL(LeabraSendCons* cg, LeabraUnit* su,
                                                   LeabraNetwork* net) {
    Compute_Weights_CtLeabraXCAL(cg, su, net);
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
