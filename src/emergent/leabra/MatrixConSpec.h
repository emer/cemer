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

// declare all other types mentioned but not required to include:

eTypeDef_Of(MatrixConSpec);

class E_API MatrixConSpec : public LeabraConSpec {
  // #AKA_MatrixNoGoConSpec Learning of matrix input connections based on dopamine modulation of activation -- for both Matrix_Go and NoGo connections
INHERITED(LeabraConSpec)
public:

  inline override void Compute_SRAvg(LeabraSendCons* cg, LeabraUnit* su, bool do_s) {
    // do NOT do this under any circumstances!!
  }

  // everything can use one dwt with post-soft-bound because no hebbian term
  inline void C_Compute_dWt_Matrix_Trace(LeabraCon* cn, float lin_wt, 
                                         float mtx_act, float su_act) {
    float dwt = mtx_act * su_act;
    cn->dwt += cur_lrate * dwt;
  }

  inline override void Compute_dWt_LeabraCHL(LeabraSendCons* cg, LeabraUnit* su) {
    for(int i=0; i<cg->size; i++) {
      LeabraUnit* ru = (LeabraUnit*)cg->Un(i);
      LeabraCon* cn = (LeabraCon*)cg->OwnCn(i);
      if(ru->act_p == 0.0f) continue;
      C_Compute_dWt_Matrix_Trace(cn, LinFmSigWt(cn->wt), ru->act_p, su->act_p);
    }
  }

  inline override void Compute_dWt_CtLeabraXCAL(LeabraSendCons* cg, LeabraUnit* su) {
    Compute_dWt_LeabraCHL(cg, su);
  }

  inline override void Compute_dWt_CtLeabraCAL(LeabraSendCons* cg, LeabraUnit* su) {
    Compute_dWt_LeabraCHL(cg, su);
  }

  inline void C_Compute_Weights_Matrix_Trace(LeabraCon* cn, float ru_dav) {
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

  inline void Compute_Weights_LeabraCHL(LeabraSendCons* cg, LeabraUnit* su) {
    for(int i=0; i<cg->size; i++) {
      LeabraUnit* ru = (LeabraUnit*)cg->Un(i);
      LeabraCon* cn = (LeabraCon*)cg->OwnCn(i);
      C_Compute_Weights_Matrix_Trace(cn, ru->dav);
      //  ApplyLimits(cg, ru); limits are automatically enforced anyway
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
