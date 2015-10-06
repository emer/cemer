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

#ifndef DeepCtxtConSpec_h
#define DeepCtxtConSpec_h 1

// parent includes:
#include <LeabraConSpec>

// member includes:
#include <LeabraNetwork>

// declare all other types mentioned but not required to include:

eTypeDef_Of(DeepCtxtConSpec);

class E_API DeepCtxtConSpec : public LeabraConSpec {
  // #AKA_LeabraTICtxtConSpec sends deep layer deep_raw activation values to deep_ctxt_net variable on receiving units -- typically used to integrate across the local context within a layer, providing both temporal integration (TI) learning, and the basis for normalizing attentional signals -- use for SELF projection in a layer -- wt_scale should be set to 1, 1
INHERITED(LeabraConSpec)
public:
  // special!
  bool  DoesStdNetin() override { return false; }
  bool  DoesStdDwt() override { return false; }
  bool  IsDeepCtxtCon() override { return true; }
  void  Trial_Init_Specs(LeabraNetwork* net) override;
 
  inline void Send_DeepCtxtNetDelta(LeabraConGroup* cg, LeabraNetwork* net,
                                    int thr_no, const float su_act_delta) {
    const float su_act_delta_eff = cg->scale_eff * su_act_delta;
    float* wts = cg->OwnCnVar(WT);
    float* send_deepnet_vec = net->ThrSendDeepNetTmp(thr_no);
#ifdef TA_VEC_USE
    Send_NetinDelta_vec(cg, su_act_delta_eff, send_deepnet_vec, wts);
#else
    CON_GROUP_LOOP(cg, C_Send_NetinDelta(wts[i], send_deepnet_vec,
                                         cg->UnIdx(i), su_act_delta_eff));
#endif
  }
  // #IGNORE sender-based activation net input for con group (send net input to receivers) -- always goes into tmp matrix (thread_no >= 0!) and is then integrated into net through Compute_NetinInteg function on units

  // don't send regular net inputs..
  inline void Send_NetinDelta(LeabraConGroup* cg, LeabraNetwork* net, int thr_no, 
                              float su_act_delta) override { };
  inline float Compute_Netin(ConGroup* cg, Network* net, int thr_no) override
  { return 0.0f; }

  inline void C_Compute_dWt_Delta(float& dwt, const float ru_avg_s, const float ru_avg_m,
                                  const float su_deep_prv)
  { dwt += cur_lrate * (ru_avg_s - ru_avg_m) * su_deep_prv; }
  // #IGNORE

  inline void Compute_dWt(ConGroup* rcg, Network* rnet, int thr_no) override {
    LeabraNetwork* net = (LeabraNetwork*)rnet;
    if(!learn || (ignore_unlearnable && net->unlearnable_trial)) return;
    LeabraConGroup* cg = (LeabraConGroup*)rcg;
    LeabraUnitVars* su = (LeabraUnitVars*)cg->ThrOwnUnVars(net, thr_no);

    float* dwts = cg->OwnCnVar(DWT);
    const float su_deep_prv = su->deep_raw_prv; // this is the value sent

    const int sz = cg->size;
    for(int i=0; i<sz; i++) {
      LeabraUnitVars* ru = (LeabraUnitVars*)cg->UnVars(i,net);
      C_Compute_dWt_Delta(dwts[i], ru->avg_s, ru->avg_m, su_deep_prv); // using avgs..
    }
  }

  void  GetPrjnName(Projection& prjn, String& nm) override;

  TA_SIMPLE_BASEFUNS(DeepCtxtConSpec);
private:
  void Initialize();
  void Destroy()     { };
};

#endif // DeepCtxtConSpec_h
