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
#include <MatrixCon>
#include <LeabraNetwork>

// declare all other types mentioned but not required to include:

eTypeDef_Of(MatrixConSpec);

class E_API MatrixConSpec : public LeabraConSpec {
  // Learning of matrix input connections based on dopamine modulation of activation -- for Matrix_Go connections only -- use MatrixNoGoConSpec for NoGo pathway
INHERITED(LeabraConSpec)
public:
  bool          trace_learn;      // use trace-based learning mechnism, instead of the previous LV-dopamine modulated learning mechanism -- this setting will be automatically coordinated with MatrixLayerSpec.matrix.trace_learn -- layerspec takes precidence and will reset this one to be consistent

  inline override void Compute_SRAvg(LeabraSendCons* cg, LeabraUnit* su, bool do_s) {
    // do NOT do this under any circumstances!!
  }

  inline void Compute_MidMinusAct(LeabraRecvCons* cg, LeabraUnit* ru) {
    for(int i=0; i<cg->size; i++) {
      LeabraUnit* su = (LeabraUnit*)cg->Un(i);
      MatrixCon* cn = (MatrixCon*)cg->PtrCn(i);
      cn->sact_lrn = su->act_eq;
    }
  }
  // RECV-based save current sender activation states to sravg_m for subsequent learning -- call this at time of gating..

  // everything can use one dwt with post-soft-bound because no hebbian term
  inline void C_Compute_dWt_Matrix_DA(LeabraCon* cn, float lin_wt, 
                                      float mtx_act_mid, float mtx_da, float su_act_lrn) {
    float dwt = mtx_da * mtx_act_mid * su_act_lrn;
    cn->dwt += cur_lrate * dwt;
  }
  inline void C_Compute_dWt_Matrix_Trace(LeabraCon* cn, float lin_wt, 
                                         float mtx_act_mid, float su_act_lrn) {
    float dwt = mtx_act_mid * su_act_lrn;
    cn->dwt += cur_lrate * dwt;
  }

  inline override void Compute_dWt_LeabraCHL(LeabraSendCons* cg, LeabraUnit* su) {
    if(trace_learn) {
      for(int i=0; i<cg->size; i++) {
        LeabraUnit* ru = (LeabraUnit*)cg->Un(i);
        MatrixCon* cn = (MatrixCon*)cg->OwnCn(i);
        if(ru->act_mid == 0.0f) continue; // if act_mid == 0 then was not -- in any case, dwt = 0
        C_Compute_dWt_Matrix_Trace(cn, LinFmSigWt(cn->wt), ru->act_mid, cn->sact_lrn);
        // note: using cn->sact_lrn as having saved sending activation in Compute_MidMinusAct
      }
    }
    else {
      for(int i=0; i<cg->size; i++) {
        LeabraUnit* ru = (LeabraUnit*)cg->Un(i);
        MatrixCon* cn = (MatrixCon*)cg->OwnCn(i);
        if(ru->act_mid == 0.0f) continue; // if act_mid == 0 then was not -- in any case, dwt = 0
        C_Compute_dWt_Matrix_DA(cn, LinFmSigWt(cn->wt), ru->act_mid, ru->dav, cn->sact_lrn);
        // note: using cn->sact_lrn as having saved sending activation in Compute_MidMinusAct
      }
    }
  }

  inline override void Compute_dWt_CtLeabraXCAL(LeabraSendCons* cg, LeabraUnit* su) {
    Compute_dWt_LeabraCHL(cg, su);
  }

  inline override void Compute_dWt_CtLeabraCAL(LeabraSendCons* cg, LeabraUnit* su) {
    Compute_dWt_LeabraCHL(cg, su);
  }

  inline void C_Compute_Weights_LeabraCHL_Trace(LeabraCon* cn, float ru_dav) {
    if(cn->dwt != 0.0f) {
      // always do soft bounding, at this point (post agg across processors, etc)
      // PV dav modulates the prior su product dwts -- skipping the LV middleman..
      float dwt = ru_dav * cn->dwt;
      float lin_wt = LinFmSigWt(cn->wt);
      // always do soft bounding
      if(dwt > 0.0f)	dwt *= (1.0f - lin_wt);
      else		dwt *= lin_wt;
      cn->wt = SigFmLinWt(lin_wt + dwt);
    }
    cn->pdw = cn->dwt;
    cn->dwt = 0.0f;
  }

  inline void C_Compute_Weights_LeabraCHL_DA(LeabraCon* cn) {
    if(cn->dwt != 0.0f) {
      // always do soft bounding, at this point (post agg across processors, etc)
      float dwt = cn->dwt;
      float lin_wt = LinFmSigWt(cn->wt);
      // always do soft bounding
      if(dwt > 0.0f)	dwt *= (1.0f - lin_wt);
      else		dwt *= lin_wt;
      cn->wt = SigFmLinWt(lin_wt + dwt);
    }
    cn->pdw = cn->dwt;
    cn->dwt = 0.0f;
  }

  inline void Compute_Weights_LeabraCHL(LeabraSendCons* cg, LeabraUnit* su) {
    if(trace_learn) {
      for(int i=0; i<cg->size; i++) {
        LeabraUnit* ru = (LeabraUnit*)cg->Un(i);
        MatrixCon* cn = (MatrixCon*)cg->OwnCn(i);
        C_Compute_Weights_LeabraCHL_Trace(cn, ru->dav);
        //  ApplyLimits(cg, ru); limits are automatically enforced anyway
      }
    }
    else {
      for(int i=0; i<cg->size; i++) {
        LeabraUnit* ru = (LeabraUnit*)cg->Un(i);
        MatrixCon* cn = (MatrixCon*)cg->OwnCn(i);
        C_Compute_Weights_LeabraCHL_DA(cn);
        //  ApplyLimits(cg, ru); limits are automatically enforced anyway
      }
    }
  }

  inline override void Compute_Weights_CtLeabraXCAL(LeabraSendCons* cg, LeabraUnit* su) {
    Compute_Weights_LeabraCHL(cg, su);
  }
  inline override void Compute_Weights_CtLeabraCAL(LeabraSendCons* cg, LeabraUnit* su) {
    Compute_Weights_LeabraCHL(cg, su);
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
