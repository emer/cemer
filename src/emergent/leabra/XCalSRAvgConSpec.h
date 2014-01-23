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

#ifndef XCalSRAvgConSpec_h
#define XCalSRAvgConSpec_h 1

// parent includes:
#include <LeabraConSpec>

// member includes:
#include <LeabraNetwork>

// declare all other types mentioned but not required to include:

eTypeDef_Of(XCalSRAvgConSpec);

class E_API XCalSRAvgConSpec : public LeabraConSpec {
  // send-recv average at the connection level learning in XCal
INHERITED(LeabraConSpec)
public:

  inline void C_Compute_dWt_CtLeabraXCAL_trial(float& dwt, const float sravg_s,
                                     const float sravg_m, const float sravg_s_nrm,
                                     const float sravg_m_nrm, float su_act_mult,
                                     const float ru_avg_l) {
    float srs = sravg_s * sravg_s_nrm;
    float srm = sravg_m * sravg_m_nrm;
    float sm_mix = xcal.s_mix * srs + xcal.m_mix * srm;
    float effthr = xcal.thr_m_mix * srm + su_act_mult * ru_avg_l;
    dwt += cur_lrate * xcal.dWtFun(sm_mix, effthr);
  }
  // #IGNORE

  inline void Compute_dWt_CtLeabraXCAL(LeabraSendCons* cg, LeabraUnit* su,
                                                LeabraNetwork* net) CPP11_OVERRIDE {
    if(ignore_unlearnable && net->unlearnable_trial) return;

    float su_avg_m = su->avg_m;
    float su_act_mult = xcal.thr_l_mix * su_avg_m;

    float* dwts = cg->OwnCnVar(DWT);
    float* srm = cg->OwnCnVar(SRAVG_M);
    float* srs = cg->OwnCnVar(SRAVG_S);
    // LeabraLayer* rlay = (LeabraLayer*)cg->prjn->layer;
    const float sravg_s_nrm = net->sravg_vals.s_nrm;
    const float sravg_m_nrm = net->sravg_vals.m_nrm;
    // todo: what is diff between using rlay vs. net???

    const int sz = cg->size;
    for(int i=0; i<sz; i++) {
      LeabraUnit* ru = (LeabraUnit*)cg->Un(i,net);
      C_Compute_dWt_CtLeabraXCAL_trial(dwts[i], srs[i], srm[i],
                                       sravg_s_nrm, sravg_m_nrm, su_act_mult,
                                       ru->avg_l);
    }
  }

  bool CheckConfig_RecvCons(RecvCons* cg, bool quiet=false) CPP11_OVERRIDE;

  TA_SIMPLE_BASEFUNS(XCalSRAvgConSpec);
protected:
  SPEC_DEFAULTS;
//   void 	UpdateAfterEdit_impl();

private:
  void 	Initialize();
  void	Destroy()		{ };
  void	Defaults_init() 	{ };
};

#endif // XCalSRAvgConSpec_h
