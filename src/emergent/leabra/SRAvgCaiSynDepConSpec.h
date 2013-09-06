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

#ifndef SRAvgCaiSynDepConSpec_h
#define SRAvgCaiSynDepConSpec_h 1

// parent includes:
#include <LeabraConSpec>

// member includes:
#include <SRAvgCaiSynDepCon>
#include <CaiSynDepConSpec>

// declare all other types mentioned but not required to include:

eTypeDef_Of(SRAvgCaiSynDepConSpec);

class E_API SRAvgCaiSynDepConSpec : public LeabraConSpec {
  // send-recv average at the connection level learning in XCal, synaptic depression connection at the cycle level, based on synaptic integration of calcium
INHERITED(LeabraConSpec)
public:
  enum CaiSynDepConVars {
    EFFWT = SRAVG_M+1,          // effective weight value
    CAI,                        // cai intacelluarl calcium
  };

  CaiSynDepSpec		ca_dep;		// calcium-based depression of synaptic efficacy
  
  /////////////////////////////////////////////////////////////////////////////////////
  // 		Ca updating and synaptic depression

  inline void C_Compute_Cai(float& cai, const float ru_act, const float su_act)
  { ca_dep.CaUpdt(cai, ru_act, su_act); }
  // connection-level Cai update
  inline void Compute_Cai(LeabraSendCons* cg, LeabraUnit* su, LeabraNetwork* net)
  { float* cais = cg->OwnCnVar(CAI);
    const float su_act = su->act_eq;
    CON_GROUP_LOOP(cg, C_Compute_Cai(cais[i],
                                     ((LeabraUnit*)cg->Un(i,net))->act_eq, su_act));
  }
  // connection-level synaptic depression: syn dep direct

  inline void C_Compute_CycSynDep(float& effwt, const float wt, const float cai)
  { effwt = wt * ca_dep.SynDep(cai); }
  // connection-level synaptic depression: ca mediated
  inline override void Compute_CycSynDep(LeabraSendCons* cg, LeabraUnit* su,
                                         LeabraNetwork* net) {
    Compute_Cai(cg, su, net);
    float* wts = cg->OwnCnVar(WT);
    float* cais = cg->OwnCnVar(CAI);
    float* effs = cg->OwnCnVar(EFFWT);
    CON_GROUP_LOOP(cg, C_Compute_CycSynDep(effs[i], wts[i], cais[i]));
  }
  // connection-group level synaptic depression

  inline void C_Init_SdEffWt(float& effwt, const float wt, float& cai)
  { effwt = wt; cai = 0.0f;  }

  inline void Init_SdEffWt(LeabraRecvCons* cg, LeabraNetwork* net) {
    // receiver based
    CON_GROUP_LOOP(cg, C_Init_SdEffWt(cg->PtrCn(i,EFFWT,net), cg->PtrCn(i,WT,net),
                                      cg->PtrCn(i,CAI,net)));
  }
  // #CAT_Activation reset synaptic depression effective weight (remove any existing synaptic depression and associated variables) -- receiver based -- slow -- use sender if possible
  inline void Init_SdEffWt(LeabraSendCons* cg) {
    float* wts = cg->OwnCnVar(WT);
    float* cais = cg->OwnCnVar(CAI);
    float* effs = cg->OwnCnVar(EFFWT);
    CON_GROUP_LOOP(cg, C_Init_SdEffWt(effs[i], wts[i], cais[i]));
  }
  // #CAT_Activation reset synaptic depression effective weight (remove any existing synaptic depression and associated variables)

  inline override void C_Init_Weights_post(BaseCons* cg, const int idx,
                                           Unit* ru, Unit* su, Network* net) {
    inherited::C_Init_Weights_post(cg, idx, ru, su, net);
    cg->OwnCn(idx,EFFWT) = cg->OwnCn(idx,WT); cg->OwnCn(idx,CAI) = 0.0f;
  }

  inline override void Send_NetinDelta(LeabraSendCons* cg, LeabraNetwork* net,
                                const int thread_no, const float su_act_delta)
  { Send_NetinDelta_impl(cg, net, thread_no, su_act_delta, cg->OwnCnVar(EFFWT)); }
  // use effwt instead of wt

  inline override float Compute_Netin(RecvCons* cg, Unit* ru, Network* net) {
    // this is slow b/c going through the PtrCn
    float rval=0.0f;
    CON_GROUP_LOOP(cg, rval += C_Compute_Netin(cg->PtrCn(i,EFFWT,net), // effwt
                                               cg->Un(i,net)->act));
    return ((LeabraRecvCons*)cg)->scale_eff * rval;
  }

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

  inline void Compute_dWt_CtLeabraXCAL(LeabraSendCons* cg, LeabraUnit* su,
                                       LeabraNetwork* net) {
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

  override bool CheckConfig_RecvCons(RecvCons* cg, bool quiet=false);

  TA_SIMPLE_BASEFUNS(SRAvgCaiSynDepConSpec);
protected:
  SPEC_DEFAULTS;
  void 	UpdateAfterEdit_impl();

private:
  void 	Initialize();
  void	Destroy()		{ };
  void	Defaults_init() 	{ };
};

#endif // SRAvgCaiSynDepConSpec_h
