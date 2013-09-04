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

#ifndef TrialSynDepConSpec_h
#define TrialSynDepConSpec_h 1

// parent includes:
#include <LeabraConSpec>

// member includes:
#include <TrialSynDepCon>
#include <LeabraNetwork>

// declare all other types mentioned but not required to include:

eTypeDef_Of(TrialSynDepSpec);

class E_API TrialSynDepSpec : public SpecMemberBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra specs for synaptic depression
INHERITED(SpecMemberBase)
public:
  float		rec;		// #DEF_1 rate of recovery from depression
  float		depl;		// #DEF_1.1 rate of depletion of synaptic efficacy as a function of sender-receiver activations

  override String       GetTypeDecoKey() const { return "ConSpec"; }

  SIMPLE_COPY(TrialSynDepSpec);
  TA_BASEFUNS(TrialSynDepSpec);
protected:
  SPEC_DEFAULTS;
private:
  void	Initialize();
  void 	Destroy()	{ };
  void	Defaults_init() { };
};

eTypeDef_Of(TrialSynDepConSpec);

class E_API TrialSynDepConSpec : public LeabraConSpec {
  // synaptic depression connection at the trial level (as opposed to cycle level)
INHERITED(LeabraConSpec)
public:
  TrialSynDepSpec	syn_dep;	// synaptic depression specifications

  void C_Depress_Wt(TrialSynDepCon* cn, LeabraUnit*, LeabraUnit* su) {
    // NOTE: fctn of sender act and wt: could be just sender or sender*recv.. 
    float deff = syn_dep.rec * (cn->wt - cn->effwt) - syn_dep.depl * su->act_eq * cn->wt;
    cn->effwt += deff;
    if(cn->effwt > cn->wt) cn->effwt = cn->wt;
    if(cn->effwt < wt_limits.min) cn->effwt = wt_limits.min;
  }
  virtual void Depress_Wt(LeabraSendCons* cg, LeabraUnit* su,
                          LeabraNetwork* net) {
    CON_GROUP_LOOP(cg, C_Depress_Wt((TrialSynDepCon*)cg->OwnCn(i),
                                    (LeabraUnit*)cg->Un(i,net), su));
  }

  override void Compute_dWt_LeabraCHL(LeabraSendCons* cg, LeabraUnit* su,
                                      LeabraNetwork* net) {
    inherited::Compute_dWt_LeabraCHL(cg, su, net);
    Depress_Wt(cg, su, net);
  }
  override void Compute_dWt_CtLeabraXCAL(LeabraSendCons* cg, LeabraUnit* su,
                                         LeabraNetwork* net) {
    inherited::Compute_dWt_CtLeabraXCAL(cg, su, net);
    Depress_Wt(cg, su, net);
  }
  override void Compute_dWt_CtLeabraCAL(LeabraSendCons* cg, LeabraUnit* su,
                                        LeabraNetwork* net) {
    inherited::Compute_dWt_CtLeabraCAL(cg, su, net);
    Depress_Wt(cg, su, net);
  }

  void C_Reset_EffWt(TrialSynDepCon* cn) {
    cn->effwt = cn->wt;
  }
  virtual void Reset_EffWt(LeabraRecvCons* cg) {
    CON_GROUP_LOOP(cg, C_Reset_EffWt((TrialSynDepCon*)cg->PtrCn(i)));
  }
  virtual void Reset_EffWt(LeabraSendCons* cg) {
    CON_GROUP_LOOP(cg, C_Reset_EffWt((TrialSynDepCon*)cg->OwnCn(i)));
  }

  void 	C_Init_Weights_post(BaseCons* cg, Connection* cn, Unit* ru, Unit* su) {
    inherited::C_Init_Weights_post(cg, cn, ru, su);
    TrialSynDepCon* lcn = (TrialSynDepCon*)cn; lcn->effwt = lcn->wt;
  }

  inline void C_Send_NetinDelta_Thread(TrialSynDepCon* cn, float* send_netin_vec,
                                       int ru_idx, float su_act_delta_eff) {
    send_netin_vec[ru_idx] += cn->effwt * su_act_delta_eff;
  }

  inline void C_Send_NetinDelta_NoThread(TrialSynDepCon* cn, LeabraUnit* ru,
                                         float su_act_delta_eff) {
    ru->net_delta += cn->effwt * su_act_delta_eff;
  }

  inline void C_Send_InhibDelta_Thread(TrialSynDepCon* cn, float* send_inhib_vec,
                                       int ru_idx, float su_act_delta_eff) {
    send_inhib_vec[ru_idx] += cn->effwt * su_act_delta_eff;
  }

  inline void C_Send_InhibDelta_NoThread(TrialSynDepCon* cn, LeabraUnit* ru,
                                         float su_act_delta_eff) {
    ru->g_i_delta += cn->effwt * su_act_delta_eff;
  }

  override void Send_NetinDelta(LeabraSendCons* cg, LeabraNetwork* net,
			      int thread_no, float su_act_delta) {
    float su_act_delta_eff = cg->scale_eff * su_act_delta;
    if(net->NetinPerPrjn()) { // always uses send_netin_tmp -- thread_no auto set to 0 in parent call if no threads
      float* send_netin_vec = net->send_netin_tmp.el
	+ net->send_netin_tmp.FastElIndex(0, cg->recv_idx(), thread_no);
      CON_GROUP_LOOP(cg, C_Send_NetinDelta_Thread((TrialSynDepCon*)cg->OwnCn(i),
                                                  send_netin_vec,
                                                  cg->UnIdx(i), su_act_delta_eff));
    }
    else {
      if(thread_no < 0) {
	CON_GROUP_LOOP(cg, C_Send_NetinDelta_NoThread((TrialSynDepCon*)cg->OwnCn(i),
                                          (LeabraUnit*)cg->Un(i,net), su_act_delta_eff));
      }
      else {
	float* send_netin_vec = net->send_netin_tmp.el
	  + net->send_netin_tmp.FastElIndex(0, thread_no);
	CON_GROUP_LOOP(cg, C_Send_NetinDelta_Thread((TrialSynDepCon*)cg->OwnCn(i),
                                                    send_netin_vec,
                                                    cg->UnIdx(i), su_act_delta_eff));
      }
    }
  }

  float C_Compute_Netin(TrialSynDepCon* cn, LeabraUnit*, LeabraUnit* su) {
    return cn->effwt * su->act_eq;
  }
  float Compute_Netin(RecvCons* cg, Unit* ru, Network* net) {
    float rval=0.0f;
    CON_GROUP_LOOP(cg, rval += C_Compute_Netin((TrialSynDepCon*)cg->PtrCn(i),
                                               (LeabraUnit*)ru,
					       (LeabraUnit*)cg->Un(i,net)));
    return ((LeabraRecvCons*)cg)->scale_eff * rval;
  }

  override String       GetTypeDecoKey() const { return "ConSpec"; }

  TA_SIMPLE_BASEFUNS(TrialSynDepConSpec);
protected:
  SPEC_DEFAULTS;
  void 	UpdateAfterEdit_impl();

private:
  void 	Initialize();
  void	Destroy()		{ };
  void	Defaults_init() 	{ };
};

#endif // TrialSynDepConSpec_h
