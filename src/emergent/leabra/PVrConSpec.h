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

#ifndef PVrConSpec_h
#define PVrConSpec_h 1

// parent includes:
#include <PVConSpec>

// member includes:

// declare all other types mentioned but not required to include:

eTypeDef_Of(PVrConSpec);

class E_API PVrConSpec : public PVConSpec {
  // primary value connection spec with asymmetrical learning rates -- used for reward detection connections -- have asymmetric weight decrease to lock in expectations for longer
INHERITED(PVConSpec)
public:
  float         wt_dec_mult;   // multiplier for weight decrease rate relative to basic lrate used for weight increases

  inline void C_Compute_dWt_Delta(LeabraCon* cn, float lin_wt, LeabraUnit* ru, LeabraUnit* su) {
    float dwt = (ru->act_p - ru->act_m) * su->act_p; // basic delta rule
    if(dwt < 0.0f)      dwt *= wt_dec_mult;
    if(lmix.err_sb) {
      if(dwt > 0.0f)    dwt *= (1.0f - lin_wt);
      else              dwt *= lin_wt;
    }
    cn->dwt += cur_lrate * dwt;
  }

  inline override void Compute_dWt_LeabraCHL(LeabraSendCons* cg, LeabraUnit* su) {
    for(int i=0; i<cg->size; i++) {
      LeabraUnit* ru = (LeabraUnit*)cg->Un(i);
      LeabraCon* cn = (LeabraCon*)cg->OwnCn(i);
      C_Compute_dWt_Delta(cn, LinFmSigWt(cn->wt), ru, su);
    }
  }

  inline override void Compute_dWt_CtLeabraXCAL(LeabraSendCons* cg, LeabraUnit* su) {
    Compute_dWt_LeabraCHL(cg, su);
  }

  inline override void Compute_dWt_CtLeabraCAL(LeabraSendCons* cg, LeabraUnit* su) {
    Compute_dWt_LeabraCHL(cg, su);
  }

  inline override void Compute_Weights_CtLeabraXCAL(LeabraSendCons* cg, LeabraUnit* su) {
    // just run chl version through-and-through
    CON_GROUP_LOOP(cg, C_Compute_Weights_LeabraCHL((LeabraCon*)cg->OwnCn(i)));
    //  ApplyLimits(cg, ru); limits are automatically enforced anyway
  }
  inline override void Compute_Weights_CtLeabraCAL(LeabraSendCons* cg, LeabraUnit* su) {
    // just run chl version through-and-through
    CON_GROUP_LOOP(cg, C_Compute_Weights_LeabraCHL((LeabraCon*)cg->OwnCn(i)));
    //  ApplyLimits(cg, ru); limits are automatically enforced anyway
  }


  TA_SIMPLE_BASEFUNS(PVrConSpec);
protected:
  SPEC_DEFAULTS;
private:
  void  Initialize();
  void  Destroy()               { };
  void  Defaults_init() { Initialize(); }
};

#endif // PVrConSpec_h
