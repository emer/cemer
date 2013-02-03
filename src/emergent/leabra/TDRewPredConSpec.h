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

#ifndef TDRewPredConSpec_h
#define TDRewPredConSpec_h 1

// parent includes:
#include <LeabraConSpec>

// member includes:
#include <LeabraNetwork>
#include <LeabraTdUnit>

// declare all other types mentioned but not required to include:

TypeDef_Of(TDRewPredConSpec);

class LEABRA_API TDRewPredConSpec : public LeabraConSpec {
  // Reward Prediction connections: for TD RewPred Layer, uses TD algorithm for predicting rewards
INHERITED(LeabraConSpec)
public:
  inline void C_Compute_dWt_Delta(LeabraCon* cn, float lin_wt, LeabraTdUnit* ru, LeabraTdUnit* su) {
    float err = (ru->act_p - ru->act_m) * su->trace;
    if(lmix.err_sb) {
      if(err > 0.0f)	err *= (1.0f - lin_wt);
      else		err *= lin_wt;	
    }
    cn->dwt += cur_lrate * err;
  }

  inline void C_Compute_dWt_Delta_NoSB(LeabraCon* cn, LeabraTdUnit* ru, LeabraTdUnit* su) {
    float err = (ru->act_p - ru->act_m) * su->trace;
    cn->dwt += cur_lrate * err;
  }


  // this computes weight changes based on sender at time t-1
  inline void Compute_dWt_LeabraCHL(LeabraSendCons* cg, LeabraUnit* su) {
    LeabraTdUnit* lsu = (LeabraTdUnit*)su;
    for(int i=0; i<cg->size; i++) {
      LeabraTdUnit* ru = (LeabraTdUnit*)cg->Un(i);
      LeabraCon* cn = (LeabraCon*)cg->OwnCn(i);
      C_Compute_dWt_Delta(cn, LinFmSigWt(cn->wt), ru, lsu);
    }
  }

  inline void Compute_dWt_CtLeabraXCAL(LeabraSendCons* cg, LeabraUnit* su) {
    LeabraTdUnit* lsu = (LeabraTdUnit*)su;
    for(int i=0; i<cg->size; i++) {
      LeabraTdUnit* ru = (LeabraTdUnit*)cg->Un(i);
      LeabraCon* cn = (LeabraCon*)cg->OwnCn(i);
      C_Compute_dWt_Delta_NoSB(cn, ru, lsu);
    }
  }

  inline void Compute_dWt_CtLeabraCAL(LeabraSendCons* cg, LeabraUnit* su) {
    Compute_dWt_CtLeabraXCAL(cg, su);
  }

  TA_BASEFUNS_NOCOPY(TDRewPredConSpec);
protected:
  SPEC_DEFAULTS;
private:
  void 	Initialize();
  void	Destroy()		{ };
  void	Defaults_init() { Initialize(); }
};

#endif // TDRewPredConSpec_h
