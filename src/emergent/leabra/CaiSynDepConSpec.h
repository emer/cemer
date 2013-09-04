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

#ifndef CaiSynDepConSpec_h
#define CaiSynDepConSpec_h 1

// parent includes:
#include <LeabraConSpec>

// member includes:
#include <CaiSynDepCon>
#include <LeabraNetwork>

// declare all other types mentioned but not required to include:

eTypeDef_Of(CaiSynDepSpec);

class E_API CaiSynDepSpec : public SpecMemberBase {
  // ##INLINE ##NO_TOKENS ##CAT_Leabra specs for synaptic depression based in synaptic integration of calcium
INHERITED(SpecMemberBase)
public:
  float		ca_inc;		// #DEF_0.2 time constant for increases in Ca_i (from NMDA etc currents) -- default base 
  // value is .01 per cycle -- multiply by network->ct_learn.syndep_int to get this value (default = 20)
  float		ca_dec;		// #DEF_0.2 time constant for decreases in Ca_i (from Ca pumps pushing Ca back out into 
  // the synapse) -- default base value is .01 per cycle -- multiply by network->ct_learn.syndep_int to get this value 
  // (default = 20)

  float		sd_ca_thr;	// #DEF_0.2 synaptic depression ca threshold: only when ca_i has increased by this 
  // amount (thus synaptic ca depleted) does it affect firing rates and thus synaptic depression
  float		sd_ca_gain;	// #DEF_0.3 multiplier on cai value for computing synaptic depression -- modulates 
  // overall level of depression independent of rate parameters
  float		sd_ca_thr_rescale; // #READ_ONLY rescaling factor taking into account sd_ca_gain and sd_ca_thr 
  // (= sd_ca_gain/(1 - sd_ca_thr))

  inline void	CaUpdt(float& cai, float ru_act, float su_act) {
    float drive = ru_act * su_act;
    cai += ca_inc * (1.0f - cai) * drive - ca_dec * cai;
  }

  inline float	SynDep(float cai) {
    float cao_thr = (cai > sd_ca_thr) ? (1.0 - sd_ca_thr_rescale * (cai - sd_ca_thr)) : 1.0f;
    return cao_thr * cao_thr;
  }

  override String       GetTypeDecoKey() const { return "ConSpec"; }

  SIMPLE_COPY(CaiSynDepSpec);
  TA_BASEFUNS(CaiSynDepSpec);
protected:
  SPEC_DEFAULTS;
  void UpdateAfterEdit_impl();

private:
  void	Initialize();
  void 	Destroy()	{ };
  void	Defaults_init() { Initialize(); }
};

eTypeDef_Of(CaiSynDepConSpec);

class E_API CaiSynDepConSpec : public LeabraConSpec {
  // synaptic depression connection at the cycle level, based on synaptic integration of calcium
INHERITED(LeabraConSpec)
public:
  CaiSynDepSpec	ca_dep;		// calcium-based depression of synaptic efficacy
  
  /////////////////////////////////////////////////////////////////////////////////////
  // 		Ca updating and synaptic depression

  inline void C_Compute_Cai(CaiSynDepCon* cn, LeabraUnit* ru, LeabraUnit* su) {
    ca_dep.CaUpdt(cn->cai, ru->act_eq, su->act_eq);
  }
  // connection-level Cai update
  inline void Compute_Cai(LeabraSendCons* cg, LeabraUnit* su, LeabraNetwork* net) {
    CON_GROUP_LOOP(cg, C_Compute_Cai((CaiSynDepCon*)cg->OwnCn(i),
                                     (LeabraUnit*)cg->Un(i,net), su));
  }

  // connection-level synaptic depression: syn dep direct
  inline void C_Compute_CycSynDep(CaiSynDepCon* cn, LeabraUnit* ru, LeabraUnit* su) {
    cn->effwt = cn->wt * ca_dep.SynDep(cn->cai);
  }
  // connection-level synaptic depression: ca mediated
  inline override void Compute_CycSynDep(LeabraSendCons* cg, LeabraUnit* su,
                                         LeabraNetwork* net) {
    Compute_Cai(cg, su, net);
    CON_GROUP_LOOP(cg, C_Compute_CycSynDep((CaiSynDepCon*)cg->OwnCn(i),
					   (LeabraUnit*)cg->Un(i,net), su));
  }
  // connection-group level synaptic depression

  inline void C_Init_SdEffWt(CaiSynDepCon* cn) {
    cn->effwt = cn->wt; cn->cai = 0.0f; 
  }
  inline void Init_SdEffWt(LeabraRecvCons* cg) {
    CON_GROUP_LOOP(cg, C_Init_SdEffWt((CaiSynDepCon*)cg->PtrCn(i)));
  }
  // #CAT_Activation reset synaptic depression effective weight (remove any existing synaptic depression and associated variables)
  inline void Init_SdEffWt(LeabraSendCons* cg) {
    CON_GROUP_LOOP(cg, C_Init_SdEffWt((CaiSynDepCon*)cg->OwnCn(i)));
  }
  // #CAT_Activation reset synaptic depression effective weight (remove any existing synaptic depression and associated variables)

  override void C_Init_Weights_post(BaseCons* cg, Connection* cn, Unit* ru, Unit* su) {
    inherited::C_Init_Weights_post(cg, cn, ru, su);
    CaiSynDepCon* lcn = (CaiSynDepCon*)cn; lcn->effwt = lcn->wt;
    lcn->cai = 0.0f; 
  }

  inline void C_Send_NetinDelta_Thread(CaiSynDepCon* cn, float* send_netin_vec,
                                       int ru_idx, float su_act_delta_eff) {
    send_netin_vec[ru_idx] += cn->effwt * su_act_delta_eff;
  }

  inline void C_Send_NetinDelta_NoThread(CaiSynDepCon* cn, LeabraUnit* ru, float su_act_delta_eff) {
    ru->net_delta += cn->effwt * su_act_delta_eff;
  }

  override void Send_NetinDelta(LeabraSendCons* cg, LeabraNetwork* net,
			      int thread_no, float su_act_delta) {
    float su_act_delta_eff = cg->scale_eff * su_act_delta;
    if(net->NetinPerPrjn()) { // always uses send_netin_tmp -- thread_no auto set to 0 in parent call if no threads
      float* send_netin_vec = net->send_netin_tmp.el
	+ net->send_netin_tmp.FastElIndex(0, cg->recv_idx(), thread_no);
      CON_GROUP_LOOP(cg, C_Send_NetinDelta_Thread((CaiSynDepCon*)cg->OwnCn(i),
                                                  send_netin_vec,
                                                  cg->UnIdx(i), su_act_delta_eff));
    }
    else {
      if(thread_no < 0) {
	CON_GROUP_LOOP(cg, C_Send_NetinDelta_NoThread((CaiSynDepCon*)cg->OwnCn(i),
                                                      (LeabraUnit*)cg->Un(i,net),
                                                      su_act_delta_eff));
      }
      else {
	float* send_netin_vec = net->send_netin_tmp.el
	  + net->send_netin_tmp.FastElIndex(0, thread_no);
	CON_GROUP_LOOP(cg, C_Send_NetinDelta_Thread((CaiSynDepCon*)cg->OwnCn(i),
                                                    send_netin_vec,
                                                    cg->UnIdx(i), su_act_delta_eff));
      }
    }
  }

  float C_Compute_Netin(CaiSynDepCon* cn, LeabraUnit*, LeabraUnit* su) {
    return cn->effwt * su->act_eq;
  }
  float Compute_Netin(RecvCons* cg, Unit* ru, Network* net) {
    float rval=0.0f;
    CON_GROUP_LOOP(cg, rval += C_Compute_Netin((CaiSynDepCon*)cg->PtrCn(i), 
					       (LeabraUnit*)ru,
                                               (LeabraUnit*)cg->Un(i,net)));
    return ((LeabraRecvCons*)cg)->scale_eff * rval;
  }

  TA_SIMPLE_BASEFUNS(CaiSynDepConSpec);
protected:
  SPEC_DEFAULTS;
  void 	UpdateAfterEdit_impl();

private:
  void 	Initialize();
  void	Destroy()		{ };
  void	Defaults_init() 	{ };
};

#endif // CaiSynDepConSpec_h
