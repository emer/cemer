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

#ifndef Deep5bConSpec_h
#define Deep5bConSpec_h 1

// parent includes:
#include <LeabraConSpec>

// member includes:
#include <LeabraNetwork>

// declare all other types mentioned but not required to include:

eTypeDef_Of(Deep5bConSpec);

class E_API Deep5bConSpec : public LeabraConSpec {
  // deep layer 5b connection spec -- sends deep5b activation values instead of usual act values -- used e.g., in projections to thalamus
INHERITED(LeabraConSpec)
public:
  // special!
  bool  IsDeep5bCon() override { return true; }

  inline void Send_Deep5bNetin(LeabraSendCons* cg, LeabraNetwork* net,
                               const int thread_no, const float su_act) {
    const float su_act_eff = cg->scale_eff * su_act;
    float* wts = cg->OwnCnVar(WT);
    if(thread_no < 0) {
      CON_GROUP_LOOP(cg, C_Send_NetinDelta_NoThread(wts[i],
                                       ((LeabraUnit*)cg->Un(i,net))->deep5b_net,
                                        su_act_eff));
    }
    else {
      float* send_netin_vec = net->send_netin_tmp.el
        + net->send_netin_tmp.FastElIndex(0, thread_no);
      CON_GROUP_LOOP(cg, C_Send_NetinDelta_Thread(wts[i], send_netin_vec,
                                                  cg->UnIdx(i), su_act_eff));
    }
  }
  // #IGNORE sender-based activation net input for con group (send net input to receivers) -- always goes into tmp matrix (thread_no >= 0!) and is then integrated into net through Compute_NetinInteg function on units

  // don't send regular net inputs..
  inline void Send_NetinDelta(LeabraSendCons*, LeabraNetwork* net, int thread_no, 
                                       float su_act_delta_eff) override { };
  inline float Compute_Netin(RecvCons* cg, Unit* ru, Network* net) override
  { return 0.0f; }

  void  GetPrjnName(Projection& prjn, String& nm) override;

  TA_SIMPLE_BASEFUNS(Deep5bConSpec);
private:
  void Initialize();
  void Destroy()     { };
};

#endif // Deep5bConSpec_h
