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

#ifndef SendDeepRawConSpec_h
#define SendDeepRawConSpec_h 1

// parent includes:
#include <LeabraConSpec>

// member includes:
#include <LeabraNetwork>
#include "ta_vector_ops.h"

// declare all other types mentioned but not required to include:

eTypeDef_Of(SendDeepRawConSpec);

class E_API SendDeepRawConSpec : public LeabraConSpec {
  // #AKA_Deep5bConSpec sends deep_raw activation values instead of usual act values -- stored into deep_net var on recv unit -- used e.g., in projections to thalamus
INHERITED(LeabraConSpec)
public:

  // special!
  bool  DoesStdNetin() override { return false; }
  bool  IsDeepRawCon() override { return true; }
  void  Trial_Init_Specs(LeabraNetwork* net) override;

#ifdef TA_VEC_USE
  inline void Send_DeepRawNetDelta_vec
    (LeabraConGroup* cg, const float su_act_delta_eff,
     float* send_deepnet_vec, const float* wts)
  {
    VECF sa(su_act_delta_eff);
    const int sz = cg->size;
    const int parsz = cg->vec_chunked_size;
    int i;
    for(i=0; i<parsz; i += TA_VEC_SIZE) {
      VECF wt;  wt.load(wts+i);
      VECF dp = wt * sa;
      VECF rnet;
      float* stnet = send_deepnet_vec + cg->UnIdx(i);
      rnet.load(stnet);
      rnet += dp;
      rnet.store(stnet);
    }

    // remainder of non-vector chunkable ones
    for(; i<sz; i++) {
      send_deepnet_vec[cg->UnIdx(i)] += wts[i] * su_act_delta_eff;
    }
  }
#endif

  inline void 	C_Send_DeepRawNetDelta(const float wt, float* send_deepnet_vec,
                                       const int ru_idx, const float su_act_delta_eff)
  { send_deepnet_vec[ru_idx] += wt * su_act_delta_eff; }
  // #IGNORE

  inline void Send_DeepRawNetDelta(LeabraConGroup* cg, LeabraNetwork* net,
                                   int thr_no, const float su_act_delta) {
    const float su_act_delta_eff = cg->scale_eff * su_act_delta;
    float* wts = cg->OwnCnVar(WT);
    float* send_deepnet_vec = net->ThrSendDeepRawNetTmp(thr_no);
#ifdef TA_VEC_USE
    Send_DeepRawNetDelta_vec(cg, su_act_delta_eff, send_deepnet_vec, wts);
#else
    CON_GROUP_LOOP(cg, C_Send_DeepRawNetDelta(wts[i], send_deepnet_vec,
                                              cg->UnIdx(i), su_act_delta_eff));
#endif
  }
  // #IGNORE sender-based activation net input for con group (send net input to receivers) -- always goes into tmp matrix (thr_no >= 0!) and is then integrated into net through Compute_NetinInteg function on units

  // don't send regular net inputs..
  inline void Send_NetinDelta(LeabraConGroup* cg, LeabraNetwork* net, int thr_no, 
                              float su_act_delta) override { };
  inline float Compute_Netin(ConGroup* cg, Network* net, int thr_no) override
  { return 0.0f; }

  void   Init_Weights_sym_s(ConGroup* cg, Network* net, int thr_no) override;

  void  GetPrjnName(Projection& prjn, String& nm) override;

  TA_SIMPLE_BASEFUNS(SendDeepRawConSpec);
private:
  void Initialize();
  void Destroy()     { };
};

#endif // SendDeepRawConSpec_h
