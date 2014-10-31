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
#include "ta_vector_ops.h"

// declare all other types mentioned but not required to include:

eTypeDef_Of(Deep5bConSpec);

class E_API Deep5bConSpec : public LeabraConSpec {
  // deep layer 5b connection spec -- sends deep5b activation values instead of usual act values -- used e.g., in projections to thalamus
INHERITED(LeabraConSpec)
public:
  // special!
  bool  DoesStdNetin() override { return false; }
  bool  IsDeep5bCon() override { return true; }
  void  Trial_Init_Specs(LeabraNetwork* net) override;

#ifdef TA_VEC_USE
  inline void Send_D5bNetDelta_vec
    (LeabraSendCons* cg, const float su_act_delta_eff,
     float* send_d5bnet_vec, const float* wts)
  {
    VECF sa(su_act_delta_eff);
    const int sz = cg->size;
    const int parsz = cg->vec_chunked_size;
    int i;
    for(i=0; i<parsz; i += TA_VEC_SIZE) {
      VECF wt;  wt.load(wts+i);
      VECF dp = wt * sa;
      VECF rnet;
      float* stnet = send_d5bnet_vec + cg->UnIdx(i);
      rnet.load(stnet);
      rnet += dp;
      rnet.store(stnet);
    }

    // remainder of non-vector chunkable ones
    for(; i<sz; i++) {
      send_d5bnet_vec[cg->UnIdx(i)] += wts[i] * su_act_delta_eff;
    }
  }
#endif

  inline void 	C_Send_D5bNetDelta_Thread(const float wt, float* send_d5bnet_vec,
                                          const int ru_idx, const float su_act_delta_eff)
  { send_d5bnet_vec[ru_idx] += wt * su_act_delta_eff; }
  // #IGNORE

  inline void Send_D5bNetDelta(LeabraSendCons* cg, LeabraNetwork* net,
                               const int thread_no, const float su_act_delta) {
    const float su_act_delta_eff = cg->scale_eff * su_act_delta;
    float* wts = cg->OwnCnVar(WT);
    float* send_d5bnet_vec = net->send_d5bnet_tmp.el
      + net->send_d5bnet_tmp.FastElIndex(0, thread_no);
#ifdef TA_VEC_USE
    Send_D5bNetDelta_vec(cg, su_act_delta_eff, send_d5bnet_vec, wts);
#else
    CON_GROUP_LOOP(cg, C_Send_D5bNetDelta_Thread(wts[i], send_d5bnet_vec,
                                                 cg->UnIdx(i), su_act_delta_eff));
#endif
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
