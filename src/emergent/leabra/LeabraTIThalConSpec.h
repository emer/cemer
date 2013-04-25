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

#ifndef LeabraTIThalConSpec_h
#define LeabraTIThalConSpec_h 1

// parent includes:
#include <LeabraStableConSpec>

// member includes:
// #include <LeabraNetwork>

// declare all other types mentioned but not required to include:

eTypeDef_Of(LeabraTIThalConSpec);

class E_API LeabraTIThalConSpec : public LeabraStableConSpec {
  // leabra TI (temporal integration) thalamus con spec -- use for feed-forward trans-thalamic weights -- only sends netinput values in the plus phase -- should typically also change the learning and prjn specs to avoid the intrinsic positive feedback loop of feedforward weights in TI
INHERITED(LeabraStableConSpec)
public:
  StableMixSpec        stable_mix_p; // plus-phase mixing parameters for stable (swt) vs. learned weight (lwt) to compute the overall effective weight value (wt) -- can increase the proportion of stable in plus phase to simulate thalamic pathway

  override bool  IsTIThalCon() { return true; }

  inline void Send_NetinDelta(LeabraSendCons* cg, LeabraNetwork* net,
                              int thread_no, float su_act_delta) {
    if(net->phase != LeabraNetwork::PLUS_PHASE) return;
    const float su_act_delta_eff = cg->scale_eff * su_act_delta;
    if(net->NetinPerPrjn()) { // always uses send_netin_tmp -- thread_no auto set to 0 in parent call if no threads
      float* send_netin_vec = net->send_netin_tmp.el
        + net->send_netin_tmp.FastElIndex(0, cg->recv_idx(), thread_no);
      CON_GROUP_LOOP(cg, C_Send_NetinDelta_Thread(cg->OwnCn(i), send_netin_vec,
                                                  (LeabraUnit*)cg->Un(i), su_act_delta_eff));
    }
    else {
      if(thread_no < 0) {
        CON_GROUP_LOOP(cg, C_Send_NetinDelta_NoThread(cg->OwnCn(i), (LeabraUnit*)cg->Un(i),
                                                      su_act_delta_eff));
      }
      else {
        float* send_netin_vec = net->send_netin_tmp.el
          + net->send_netin_tmp.FastElIndex(0, thread_no);
        CON_GROUP_LOOP(cg, C_Send_NetinDelta_Thread(cg->OwnCn(i), send_netin_vec,
                                                    (LeabraUnit*)cg->Un(i), su_act_delta_eff));
      }
    }
  }

  override void  GetPrjnName(Projection& prjn, String& nm);

  TA_SIMPLE_BASEFUNS(LeabraTIThalConSpec);
private:
  void Initialize();
  void Destroy()     { };
};

#endif // LeabraTIThalConSpec_h
