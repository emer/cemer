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
  // The parallel-fiber to Purkinje cell connection spec -- special learning rule
INHERITED(LeabraConSpec)
public:

  // everything can use one dwt with post-soft-bound because no hebbian term
  inline void C_Compute_dWt_PfPc(LeabraCon* cn, float gran_act,
                                 float purk_minus, float purk_plus) {
    float dwt = gran_act * (purk_plus - purk_minus);
    cn->dwt += cur_lrate * dwt; 
  }

  inline override void Compute_dWt_LeabraCHL(LeabraSendCons* cg, LeabraUnit* su) {
    if(su->act_lrn == 0.0f) return; // if sender is not active, bail
    LeabraNetwork* net = (LeabraNetwork*)su->own_net();
    if(ignore_unlearnable && net && net->unlearnable_trial) return;

    for(int i=0; i<cg->size; i++) {
      LeabraUnit* ru = (LeabraUnit*)cg->Un(i);
      LeabraCon* cn = (LeabraCon*)cg->OwnCn(i);
      C_Compute_dWt_PfPc(cn, su->act_lrn, ru->act_m, ru->act_p);
    }
  }

  inline override void Compute_dWt_CtLeabraXCAL(LeabraSendCons* cg, LeabraUnit* su) {
    Compute_dWt_LeabraCHL(cg, su);
  }

  inline override void Compute_dWt_CtLeabraCAL(LeabraSendCons* cg, LeabraUnit* su) {
    Compute_dWt_LeabraCHL(cg, su);
  }


  TA_SIMPLE_BASEFUNS(CerebPfPcConSpec);
private:
  void Initialize()  { };
  void Destroy()     { };
};

#endif // CerebPfPcConSpec_h
