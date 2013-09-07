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

  inline void C_Compute_dWt_Delta(float& dwt, const float ru_act_p, 
                                  const float ru_act_m, const float su_act) {
    float tmp = (ru_act_p - ru_act_m) * su_act;
    if(tmp < 0.0f)      tmp *= wt_dec_mult;
    dwt += cur_lrate * tmp;
  }
  // #IGNORE

  inline override void Compute_dWt_CtLeabraXCAL(LeabraSendCons* cg, LeabraUnit* su,
                                                LeabraNetwork* net) {
    if(ignore_unlearnable && net->unlearnable_trial) return;

    float su_act = su->act_p;
    float* dwts = cg->OwnCnVar(DWT);
    const int sz = cg->size;
    for(int i=0; i<sz; i++) {
      LeabraUnit* ru = (LeabraUnit*)cg->Un(i, net);
      C_Compute_dWt_Delta(dwts[i], ru->act_p, ru->act_m, su_act);
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

  // compute weights already correct from PVConSpec

  TA_SIMPLE_BASEFUNS(PVrConSpec);
protected:
  SPEC_DEFAULTS;
private:
  void  Initialize();
  void  Destroy()               { };
  void  Defaults_init() { Initialize(); }
};

#endif // PVrConSpec_h
