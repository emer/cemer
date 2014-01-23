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

  inline void C_Compute_dWt_Delta(float& dwt, const float ru_act_p, 
                                  const float ru_act_m, const float su_act) {
    dwt += cur_lrate * (ru_act_p - ru_act_m) * su_act;
  }
  // #IGNORE

  inline void Compute_dWt_CtLeabraXCAL(LeabraSendCons* cg, LeabraUnit* su,
                                                LeabraNetwork* net) override {
    if(ignore_unlearnable && net->unlearnable_trial) return;

    float su_act;
    if(send_act == ACT_P)
      su_act = su->act_p;
    else                        // ACT_M2
      su_act = su->act_m2;

    float* dwts = cg->OwnCnVar(DWT);

    const int sz = cg->size;
    for(int i=0; i<sz; i++) {
      LeabraUnit* ru = (LeabraUnit*)cg->Un(i, net);
      C_Compute_dWt_Delta(dwts[i], ru->act_p, ru->act_m, su_act);
    }
  }

  inline void Compute_dWt_LeabraCHL(LeabraSendCons* cg, LeabraUnit* su,
                                             LeabraNetwork* net) override {
    Compute_dWt_CtLeabraXCAL(cg, su, net);
  }

  inline void Compute_dWt_CtLeabraCAL(LeabraSendCons* cg, LeabraUnit* su,
                                               LeabraNetwork* net) override {
    Compute_dWt_CtLeabraXCAL(cg, su, net);
  }

  inline void C_Compute_Weights_CtLeabraXCAL(float& wt, float& dwt, float& pdw,
                                               float& lwt, const float swt) {
    if(dwt != 0.0f) {
      float lin_wt = LinFmSigWt(lwt);
      if(lmix.err_sb) {         // check for soft-bounding -- typically not enabled for pv
        if(dwt > 0.0f)	dwt *= (1.0f - lin_wt);
        else		dwt *= lin_wt;
      }
      lwt = SigFmLinWt(lin_wt + dwt);
      C_Compute_EffWt(wt, swt, lwt);
    }
    pdw = dwt;
    dwt = 0.0f;
  }
  // #IGNORE

  inline void Compute_Weights_CtLeabraXCAL(LeabraSendCons* cg, LeabraUnit* su,
                                                    LeabraNetwork* net) override {
    float* wts = cg->OwnCnVar(WT);
    float* dwts = cg->OwnCnVar(DWT);
    float* pdws = cg->OwnCnVar(PDW);
    float* lwts = cg->OwnCnVar(LWT);
    float* swts = cg->OwnCnVar(SWT);

    CON_GROUP_LOOP(cg, C_Compute_Weights_CtLeabraXCAL(wts[i], dwts[i], pdws[i], 
                                                      lwts[i], swts[i]));
    //  ApplyLimits(cg, ru, net); limits are automatically enforced anyway
  }

  inline void Compute_Weights_LeabraCHL(LeabraSendCons* cg, LeabraUnit* su,
                                                 LeabraNetwork* net) override {
    Compute_Weights_CtLeabraXCAL(cg, su, net);
  }
  inline void Compute_Weights_CtLeabraCAL(LeabraSendCons* cg, LeabraUnit* su,
                                                   LeabraNetwork* net) override {
    Compute_Weights_CtLeabraXCAL(cg, su, net);
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
