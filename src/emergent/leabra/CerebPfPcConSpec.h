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

#ifndef CerebPfPcConSpec_h
#define CerebPfPcConSpec_h 1

// parent includes:
#include <LeabraConSpec>

// member includes:
#include <LeabraNetwork>

// declare all other types mentioned but not required to include:

eTypeDef_Of(CerebPfPcConSpec);

class E_API CerebPfPcConSpec : public LeabraConSpec {
  // The parallel-fiber to Purkinje cell connection spec -- special learning rule driven by IO error values clamped onto the Purkinje cell -- if targ value is 0, then no error (weights slowly increase) else an error and LTD occurs
INHERITED(LeabraConSpec)
public:
  float         nerr_lrate;      // no-error learning rate value -- weights increase at this rate * cur_lrate (i.e., this is relative to the basic learning rate -- typically smaller)

  // everything can use one dwt with post-soft-bound because no hebbian term
  inline void C_Compute_dWt_PfPc(float& dwt, const float gran_act,
                                 const float purk_minus, const float purk_plus)
  { if(purk_plus != 0.0f) dwt += -cur_lrate * purk_plus * gran_act * purk_minus;
    else dwt += cur_lrate * nerr_lrate * gran_act;  }
  // #IGNORE

  inline void Compute_dWt_CtLeabraXCAL(LeabraSendCons* cg, LeabraUnit* su,
                                                LeabraNetwork* net) CPP11_OVERRIDE
  { if(su->act_lrn == 0.0f) return; // if sender is not active, bail
    if(ignore_unlearnable && net->unlearnable_trial) return;

    const float gran_act = su->act_lrn;
    float* dwts = cg->OwnCnVar(DWT);

    const int sz = cg->size;
    for(int i=0; i<sz; i++) {
      LeabraUnit* ru = (LeabraUnit*)cg->Un(i,net);
      C_Compute_dWt_PfPc(dwts[i], gran_act, ru->act_eq, ru->targ);
      // target activation trains relative to act_eq
    }
  }
  // #IGNORE 

  inline void Compute_dWt_LeabraCHL(LeabraSendCons* cg, LeabraUnit* su,
                                             LeabraNetwork* net) CPP11_OVERRIDE {
    Compute_dWt_CtLeabraXCAL(cg, su, net);
  }
  // #IGNORE 

  inline void Compute_dWt_CtLeabraCAL(LeabraSendCons* cg, LeabraUnit* su,
                                               LeabraNetwork* net) CPP11_OVERRIDE {
    Compute_dWt_CtLeabraXCAL(cg, su, net);
  }
  // #IGNORE 

  inline void Compute_Weights_LeabraCHL(LeabraSendCons* cg, LeabraUnit* su,
                                        LeabraNetwork* net) {
    Compute_Weights_CtLeabraXCAL(cg, su, net); // do soft bound here
  }
  // #IGNORE 

  TA_SIMPLE_BASEFUNS(CerebPfPcConSpec);
private:
  void Initialize();
  void Destroy()     { };
};

#endif // CerebPfPcConSpec_h
