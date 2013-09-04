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

#ifndef CycleSynDepConSpec_h
#define CycleSynDepConSpec_h 1

// parent includes:
#include <LeabraConSpec>

// member includes:
#include <CycleSynDepCon>
#include <LeabraNetwork>

// declare all other types mentioned but not required to include:

eTypeDef_Of(CycleSynDepSpec);

class E_API CycleSynDepSpec : public SpecMemberBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra specs for synaptic depression
INHERITED(SpecMemberBase)
public:
  float		rec;		// #DEF_0.002 rate of recovery from depression
  float		asymp_act;	// #DEF_0.4 asymptotic activation value (as proportion of 1) for a fully active unit (determines depl rate value)
  float		depl;		// #READ_ONLY #SHOW rate of depletion of synaptic efficacy as a function of sender-receiver activations (computed from rec, asymp_act)

  inline void	Depress(float& effwt, float wt, float ru_act, float su_act) {
    float drive = ru_act * su_act * effwt;
    float deff = rec * (wt - effwt) - depl * drive;
    effwt += deff;
    if(effwt > wt) effwt = wt;
    if(effwt < 0.0f) effwt = 0.0f;
  }

  override String       GetTypeDecoKey() const { return "ConSpec"; }

  SIMPLE_COPY(CycleSynDepSpec);
  TA_BASEFUNS(CycleSynDepSpec);
protected:
  SPEC_DEFAULTS;
  void UpdateAfterEdit_impl();

private:
  void	Initialize();
  void 	Destroy()	{ };
  void	Defaults_init() { Initialize(); }
};

eTypeDef_Of(CycleSynDepConSpec);

class E_API CycleSynDepConSpec : public LeabraConSpec {
  // synaptic depression connection at the cycle level (as opposed to the trial level) -- this is the simpler version -- Ca_i based version below
INHERITED(LeabraConSpec)
public:
  CycleSynDepSpec	syn_dep;	// synaptic depression specifications

  inline void C_Compute_CycSynDep(CycleSynDepCon* cn, LeabraUnit* ru, LeabraUnit* su) {
    syn_dep.Depress(cn->effwt, cn->wt, ru->act_eq, su->act_eq);
  }
  inline override void Compute_CycSynDep(LeabraSendCons* cg, LeabraUnit* su,
                                         LeabraNetwork* net) {
    CON_GROUP_LOOP(cg, C_Compute_CycSynDep((CycleSynDepCon*)cg->OwnCn(i),
					   (LeabraUnit*)cg->Un(i,net), su));
  }

  void C_Reset_EffWt(CycleSynDepCon* cn) {
    cn->effwt = cn->wt;
  }
  virtual void Reset_EffWt(LeabraRecvCons* cg) {
    CON_GROUP_LOOP(cg, C_Reset_EffWt((CycleSynDepCon*)cg->PtrCn(i)));
  }
  virtual void Reset_EffWt(LeabraSendCons* cg) {
    CON_GROUP_LOOP(cg, C_Reset_EffWt((CycleSynDepCon*)cg->OwnCn(i)));
  }

  void 	C_Init_Weights_post(BaseCons* cg, Connection* cn, Unit* ru, Unit* su) {
    inherited::C_Init_Weights_post(cg, cn, ru, su);
    CycleSynDepCon* lcn = (CycleSynDepCon*)cn; lcn->effwt = lcn->wt;
  }

  inline void C_Send_NetinDelta_Thread(CycleSynDepCon* cn, float* send_netin_vec,
                                       int ru_idx, float su_act_delta_eff) {
    send_netin_vec[ru_idx] += cn->effwt * su_act_delta_eff;
  }

  inline void C_Send_NetinDelta_NoThread(CycleSynDepCon* cn, LeabraUnit* ru,
                                         float su_act_delta_eff) {
    ru->net_delta += cn->effwt * su_act_delta_eff;
  }

  override void Send_NetinDelta(LeabraSendCons* cg, LeabraNetwork* net,
                                int thread_no, float su_act_delta) {
    float su_act_delta_eff = cg->scale_eff * su_act_delta;
    if(net->NetinPerPrjn()) { // always uses send_netin_tmp -- thread_no auto set to 0 in parent call if no threads
      float* send_netin_vec = net->send_netin_tmp.el
	+ net->send_netin_tmp.FastElIndex(0, cg->recv_idx(), thread_no);
      CON_GROUP_LOOP(cg, C_Send_NetinDelta_Thread((CycleSynDepCon*)cg->OwnCn(i),
                                                  send_netin_vec,
                                                  cg->UnIdx(i), su_act_delta_eff));
    }
    else {
      if(thread_no < 0) {
	CON_GROUP_LOOP(cg, C_Send_NetinDelta_NoThread((CycleSynDepCon*)cg->OwnCn(i),
                                      (LeabraUnit*)cg->Un(i,net), su_act_delta_eff));
      }
      else {
	float* send_netin_vec = net->send_netin_tmp.el
	  + net->send_netin_tmp.FastElIndex(0, thread_no);
	CON_GROUP_LOOP(cg, C_Send_NetinDelta_Thread((CycleSynDepCon*)cg->OwnCn(i),
                                                    send_netin_vec,
                                                    cg->UnIdx(i), su_act_delta_eff));
      }
    }
  }

  float C_Compute_Netin(CycleSynDepCon* cn, LeabraUnit*, LeabraUnit* su) {
    return cn->effwt * su->act_eq;
  }
  float Compute_Netin(RecvCons* cg, Unit* ru, Network* net) {
    float rval=0.0f;
    CON_GROUP_LOOP(cg, rval += C_Compute_Netin((CycleSynDepCon*)cg->PtrCn(i),
                                               (LeabraUnit*)ru,
					       (LeabraUnit*)cg->Un(i,net)));
    return ((LeabraRecvCons*)cg)->scale_eff * rval;
  }

  TA_SIMPLE_BASEFUNS(CycleSynDepConSpec);
protected:
  SPEC_DEFAULTS;
  void 	UpdateAfterEdit_impl();

private:
  void 	Initialize();
  void	Destroy()		{ };
  void	Defaults_init() 	{ };
};

#endif // CycleSynDepConSpec_h
