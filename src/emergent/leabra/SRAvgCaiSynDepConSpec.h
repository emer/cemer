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
  CaiSynDepSpec		ca_dep;		// calcium-based depression of synaptic efficacy
  
  /////////////////////////////////////////////////////////////////////////////////////
  // 		Ca updating and synaptic depression

  inline void C_Compute_Cai(SRAvgCaiSynDepCon* cn, LeabraUnit* ru, LeabraUnit* su) {
    ca_dep.CaUpdt(cn->cai, ru->act_eq, su->act_eq);
  }
  // connection-level Cai update
  inline void Compute_Cai(LeabraSendCons* cg, LeabraUnit* su) {
    CON_GROUP_LOOP(cg, C_Compute_Cai((SRAvgCaiSynDepCon*)cg->OwnCn(i), (LeabraUnit*)cg->Un(i), su));
  }

  // connection-level synaptic depression: syn dep direct
  inline void C_Compute_CycSynDep(SRAvgCaiSynDepCon* cn, LeabraUnit* ru, LeabraUnit* su) {
    cn->effwt = cn->wt * ca_dep.SynDep(cn->cai);
  }
  // connection-level synaptic depression: ca mediated
  inline override void Compute_CycSynDep(LeabraSendCons* cg, LeabraUnit* su) {
    Compute_Cai(cg, su);
    CON_GROUP_LOOP(cg, C_Compute_CycSynDep((SRAvgCaiSynDepCon*)cg->OwnCn(i),
					   (LeabraUnit*)cg->Un(i), su));
  }
  // connection-group level synaptic depression

  inline void C_Init_SdEffWt(SRAvgCaiSynDepCon* cn) {
    cn->effwt = cn->wt; cn->cai = 0.0f; 
  }
  inline void Init_SdEffWt(LeabraRecvCons* cg) {
    CON_GROUP_LOOP(cg, C_Init_SdEffWt((SRAvgCaiSynDepCon*)cg->PtrCn(i)));
  }
  // #CAT_Activation reset synaptic depression effective weight (remove any existing synaptic depression and associated variables)
  inline void Init_SdEffWt(LeabraSendCons* cg) {
    CON_GROUP_LOOP(cg, C_Init_SdEffWt((SRAvgCaiSynDepCon*)cg->OwnCn(i)));
  }
  // #CAT_Activation reset synaptic depression effective weight (remove any existing synaptic depression and associated variables)

  override void C_Init_Weights_post(BaseCons* cg, Connection* cn, Unit* ru, Unit* su) {
    inherited::C_Init_Weights_post(cg, cn, ru, su);
    SRAvgCaiSynDepCon* lcn = (SRAvgCaiSynDepCon*)cn; lcn->effwt = lcn->wt;
    lcn->cai = 0.0f; 
  }

  inline void C_Send_NetinDelta_Thrd(SRAvgCaiSynDepCon* cn, float* send_netin_vec,
				    LeabraUnit* ru, float su_act_delta_eff) {
    send_netin_vec[ru->flat_idx] += cn->effwt * su_act_delta_eff;
  }

  inline void C_Send_NetinDelta_NoThrd(SRAvgCaiSynDepCon* cn, LeabraUnit* ru, float su_act_delta_eff) {
    ru->net_delta += cn->effwt * su_act_delta_eff;
  }

  override void Send_NetinDelta(LeabraSendCons* cg, LeabraNetwork* net,
			      int thread_no, float su_act_delta) {
    float su_act_delta_eff = cg->scale_eff * su_act_delta;
    if(net->NetinPerPrjn()) { // always uses send_netin_tmp -- thread_no auto set to 0 in parent call if no threads
      float* send_netin_vec = net->send_netin_tmp.el
	+ net->send_netin_tmp.FastElIndex(0, cg->recv_idx(), thread_no);
      CON_GROUP_LOOP(cg, C_Send_NetinDelta_Thrd((SRAvgCaiSynDepCon*)cg->OwnCn(i), send_netin_vec,
					(LeabraUnit*)cg->Un(i), su_act_delta_eff));
    }
    else {
      if(thread_no < 0) {
	CON_GROUP_LOOP(cg, C_Send_NetinDelta_NoThrd((SRAvgCaiSynDepCon*)cg->OwnCn(i),
						   (LeabraUnit*)cg->Un(i), su_act_delta_eff));
      }
      else {
	float* send_netin_vec = net->send_netin_tmp.el
	  + net->send_netin_tmp.FastElIndex(0, thread_no);
	CON_GROUP_LOOP(cg, C_Send_NetinDelta_Thrd((SRAvgCaiSynDepCon*)cg->OwnCn(i), send_netin_vec,
						 (LeabraUnit*)cg->Un(i), su_act_delta_eff));
      }
    }
  }

  float C_Compute_Netin(SRAvgCaiSynDepCon* cn, LeabraUnit*, LeabraUnit* su) {
    return cn->effwt * su->act_eq;
  }
  float Compute_Netin(RecvCons* cg, Unit* ru) {
    float rval=0.0f;
    CON_GROUP_LOOP(cg, rval += C_Compute_Netin((SRAvgCaiSynDepCon*)cg->PtrCn(i), 
					       (LeabraUnit*)ru, (LeabraUnit*)cg->Un(i)));
    return ((LeabraRecvCons*)cg)->scale_eff * rval;
  }

  inline void C_Compute_dWt_CtLeabraXCAL_trial(LeabraSRAvgCon* cn, LeabraUnit* ru,
			       float sravg_s_nrm, float sravg_m_nrm, float su_act_mult) {
    float srs = cn->sravg_s * sravg_s_nrm;
    float srm = cn->sravg_m * sravg_m_nrm;
    float sm_mix = xcal.s_mix * srs + xcal.m_mix * srm;
    float effthr = xcal.thr_m_mix * srm + su_act_mult * ru->avg_l;
    cn->dwt += cur_lrate * xcal.dWtFun(sm_mix, effthr);
  }

  inline void Compute_dWt_CtLeabraXCAL(LeabraSendCons* cg, LeabraUnit* su) {
    LeabraLayer* rlay = (LeabraLayer*)cg->prjn->layer;
    LeabraNetwork* net = (LeabraNetwork*)rlay->own_net;
    float su_avg_m = su->avg_m;
    float su_act_mult = xcal.thr_l_mix * su_avg_m;

    for(int i=0; i<cg->size; i++) {
      LeabraUnit* ru = (LeabraUnit*)cg->Un(i);
      C_Compute_dWt_CtLeabraXCAL_trial((LeabraSRAvgCon*)cg->OwnCn(i), ru, 
				       net->sravg_vals.s_nrm, net->sravg_vals.m_nrm,
				       su_act_mult);
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
