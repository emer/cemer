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

#ifndef PVConSpec_h
#define PVConSpec_h 1

// parent includes:
#include <LeabraConSpec>

// member includes:
#include <LeabraNetwork>

// declare all other types mentioned but not required to include:

eTypeDef_Of(PVConSpec);

class E_API PVConSpec : public LeabraConSpec {
  // pvlv connection spec: learns using delta rule from act_p - act_m values -- does not use hebb or err_sb parameters
INHERITED(LeabraConSpec)
public:
  enum SendActVal {
    ACT_P,                      // plus phase activation state
    ACT_M2,                     // mid-minus activation state (for PBWM)
  };

  SendActVal    send_act;       // what to use for the sending activation value

  inline void C_Compute_dWt_Delta(LeabraCon* cn, float lin_wt, LeabraUnit* ru, float su_act) {
    float dwt = (ru->act_p - ru->act_m) * su_act; // basic delta rule
    if(lmix.err_sb) {
      if(dwt > 0.0f)    dwt *= (1.0f - lin_wt);
      else              dwt *= lin_wt;
    }
    cn->dwt += cur_lrate * dwt;
  }

  inline override void Compute_dWt_LeabraCHL(LeabraSendCons* cg, LeabraUnit* su) {
    LeabraNetwork* net = (LeabraNetwork*)su->own_net();
    if(ignore_unlearnable && net && net->unlearnable_trial) return;

    float su_act;
    if(send_act == ACT_P)
      su_act = su->act_p;
    else                        // ACT_M2
      su_act = su->act_m2;

    for(int i=0; i<cg->size; i++) {
      LeabraUnit* ru = (LeabraUnit*)cg->Un(i);
      LeabraCon* cn = (LeabraCon*)cg->OwnCn(i);
      C_Compute_dWt_Delta(cn, LinFmSigWt(cn->wt), ru, su_act);
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

  TA_SIMPLE_BASEFUNS(PVConSpec);
protected:
  SPEC_DEFAULTS;
  void  UpdateAfterEdit_impl();
private:
  void  Initialize();
  void  Destroy()               { };
  void  Defaults_init();
};

#endif // PVConSpec_h
