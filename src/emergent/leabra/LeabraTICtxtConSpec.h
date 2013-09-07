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

#ifndef LeabraTICtxtConSpec_h
#define LeabraTICtxtConSpec_h 1

// parent includes:
#include <LeabraConSpec>

// member includes:
#include <LeabraNetwork>

// declare all other types mentioned but not required to include:

eTypeDef_Of(LeabraTICtxtConSpec);

class E_API LeabraTICtxtConSpec : public LeabraConSpec {
  // leabra TI (temporal integration) context con spec -- use for SELF projection in a layer to implement LeabraTI context activation and learning, and in projections to/from other layers -- control relative contribution to net input by setting wt_scale.rel, just like regular connections
INHERITED(LeabraConSpec)
public:
  // special!
  override bool  IsTICtxtCon() { return true; }

  inline void Send_CtxtNetin(LeabraSendCons* cg, LeabraNetwork* net,
                             const int thread_no, const float su_act) {
    const float su_act_eff = cg->scale_eff * su_act;
    float* wts = cg->OwnCnVar(WT);
    if(thread_no < 0) {
      CON_GROUP_LOOP(cg, C_Send_NetinDelta_NoThread(wts[i],
                                       ((LeabraUnit*)cg->Un(i,net))->net_ctxt,
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
  inline override void Send_NetinDelta(LeabraSendCons*, LeabraNetwork* net, int thread_no, 
                                       float su_act_delta_eff) { };
  inline override float Compute_Netin(RecvCons* cg, Unit* ru, Network* net)
  { return 0.0f; }

  // everything can use one dwt with post-soft-bound because no hebbian term
  inline void C_Compute_dWt_Delta(float& dwt, const float ru_act_p, const float ru_act_m,
                                  const float su_p_act_p)
  { dwt += cur_lrate * (ru_act_p - ru_act_m) * su_p_act_p; }
  // #IGNORE

  inline override void Compute_dWt_LeabraCHL(LeabraSendCons* cg, LeabraUnit* su,
                                      LeabraNetwork* net) {
    if(ignore_unlearnable && net->unlearnable_trial) return;

    float* dwts = cg->OwnCnVar(DWT);
    const float su_p_act_p = su->p_act_p;

    const int sz = cg->size;
    for(int i=0; i<sz; i++) {
      LeabraUnit* ru = (LeabraUnit*)cg->Un(i,net);
      C_Compute_dWt_Delta(dwts[i], ru->act_p, ru->act_m, su_p_act_p);  
    }
  }

  inline override void Compute_dWt_CtLeabraXCAL(LeabraSendCons* cg, LeabraUnit* su,
                                                LeabraNetwork* net)
  { Compute_dWt_LeabraCHL(cg, su, net); }

  inline override void Compute_dWt_CtLeabraCAL(LeabraSendCons* cg, LeabraUnit* su,
                                               LeabraNetwork* net)
  { Compute_dWt_LeabraCHL(cg, su, net); }

  inline override void Compute_Weights_LeabraCHL(LeabraSendCons* cg, LeabraUnit* su,
                                                 LeabraNetwork* net) {
    Compute_Weights_CtLeabraXCAL(cg, su, net);
    // CHL uses XCAL with aggregate soft weight bounding, b/c no hebbian term
  }

  override void  GetPrjnName(Projection& prjn, String& nm);

  TA_SIMPLE_BASEFUNS(LeabraTICtxtConSpec);
private:
  void Initialize();
  void Destroy()     { };
};

#endif // LeabraTICtxtConSpec_h
