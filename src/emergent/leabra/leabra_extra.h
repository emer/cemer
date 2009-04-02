// Copyright, 1995-2007, Regents of the University of Colorado,
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

// leabra.h 

#ifndef leabra_extra_h
#define leabra_extra_h

#include "leabra.h"
#include "netstru_extra.h"
#include "ta_imgproc.h"
#include "ta_virtenv.h"

// extra specialized classes for variations on the Leabra algorithm

class LEABRA_API MarkerConSpec : public LeabraConSpec {
  // connection spec that marks special projections: doesn't send netin or adapt weights
INHERITED(LeabraConSpec)
public:
  // don't send regular net inputs or learn!
  inline void 	Send_NetinDelta(LeabraRecvCons*, LeabraNetwork* net, int thread_no, 
				float su_act_delta_eff) { };
  inline void 	Compute_dWt(RecvCons*, Unit*) { };
  inline void 	Compute_dWt_LeabraCHL(LeabraRecvCons*, LeabraUnit*) { };
  inline void 	Compute_dWt_CtLeabraCAL(LeabraRecvCons*, LeabraUnit*) { };
  inline void 	Compute_dWt_CtLeabraXCAL(LeabraRecvCons*, LeabraUnit*) { };
  inline void 	Compute_SRAvg(LeabraRecvCons*, LeabraUnit*, bool do_s) { };
  inline void 	Trial_Init_SRAvg(LeabraRecvCons*, LeabraUnit*) { };
  inline void	Compute_Weights(RecvCons*, Unit*) { };
  inline void	Compute_Weights_LeabraCHL(LeabraRecvCons*, LeabraUnit*) { };
  inline void	Compute_Weights_CtLeabraCAL(LeabraRecvCons*, LeabraUnit*) { };
  inline void	Compute_Weights_CtLeabraXCAL(LeabraRecvCons*, LeabraUnit*) { };

  bool	 DMem_AlwaysLocal() { return true; }
  // these connections always need to be there on all nodes..

  TA_BASEFUNS_NOCOPY(MarkerConSpec);
private:
  void 	Initialize();
  void	Destroy()		{ };
};


//////////////////////////////////////////
// 	Context Layer for Sequential	//
//////////////////////////////////////////

class LEABRA_API CtxtUpdateSpec : public taOBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra context updating specifications
INHERITED(taOBase)
public:
  float		fm_hid;		// from hidden (inputs to context layer)
  float		fm_prv;		// from previous context layer values (maintenance)
  float		to_out;		// outputs from context layer

  SIMPLE_COPY(CtxtUpdateSpec);
  TA_BASEFUNS(CtxtUpdateSpec);
private:
  void	Initialize();
  void 	Destroy()	{ };
};

class LEABRA_API LeabraContextLayerSpec : public LeabraLayerSpec {
  // context layer that copies from its recv projection (like an input layer)
INHERITED(LeabraLayerSpec)
public:
  CtxtUpdateSpec updt;		// ctxt updating constants: from hidden, from previous values (hysteresis), outputs from context (n/a on simple gate layer)

  override void	Compute_HardClamp(LeabraLayer* lay, LeabraNetwork* net);
  // clamp from act_p values of sending layer
  override bool  CheckConfig_Layer(LeabraLayer* lay, bool quiet=false);

  // don't do any learning:
  override bool	Compute_SRAvg_Test(LeabraLayer* lay, LeabraNetwork* net)  { return false; }
  override bool	Compute_dWt_FirstPlus_Test(LeabraLayer* lay, LeabraNetwork* net) { return false; }
  override bool	Compute_dWt_SecondPlus_Test(LeabraLayer* lay, LeabraNetwork* net) { return false; }
  override bool	Compute_dWt_Nothing_Test(LeabraLayer* lay, LeabraNetwork* net) { return false; }

  void	Defaults();

  TA_SIMPLE_BASEFUNS(LeabraContextLayerSpec);
  
protected:
  virtual void Compute_Context(LeabraLayer* lay, LeabraUnit* u, LeabraNetwork* net);
  // get context source value for given context unit

private:
  void 	Initialize();
  void	Destroy()		{ };
};

//////////////////////////////////////////
// 	Misc Special Objects		//
//////////////////////////////////////////

class LEABRA_API LeabraLinUnitSpec : public LeabraUnitSpec {
  // a pure linear unit (suitable for an AC unit spec unit)
INHERITED(LeabraUnitSpec)
public:
  void 	Compute_ActFmVm(LeabraUnit* u, LeabraNetwork* net);
  
  void	Defaults();

  TA_BASEFUNS(LeabraLinUnitSpec);
private:
  void 	Initialize();
  void	Destroy()		{ };
};

class LEABRA_API LeabraNegBiasSpec : public LeabraBiasSpec {
  // only learns negative bias changes, not positive ones (decay restores back to zero)
INHERITED(LeabraBiasSpec)
public:
  float		decay;		// rate of weight decay towards zero 
  bool		updt_immed;	// update weights immediately when weights are changed

  inline void	B_Compute_Weights(LeabraCon* cn, LeabraUnit* ru) {
    if(cn->dwt > 0.0f)		// positive only
      cn->dwt = 0.0f;
    cn->dwt -= decay * cn->wt;
    cn->pdw = cn->dwt;
    cn->wt += cn->dwt;
    cn->dwt = 0.0f;
    C_ApplyLimits(cn, ru, NULL);
  }

  inline void	B_Compute_dWt_LeabraCHL(LeabraCon* cn, LeabraUnit* ru) {
    LeabraBiasSpec::B_Compute_dWt_LeabraCHL(cn, ru);
    if(updt_immed) B_Compute_Weights(cn, ru);
  }
  inline void	B_Compute_dWt_CtLeabraXCAL(LeabraCon* cn, LeabraUnit* ru, LeabraLayer* rlay) {
    LeabraBiasSpec::B_Compute_dWt_CtLeabraXCAL(cn, ru, rlay);
    if(updt_immed) B_Compute_Weights(cn, ru);
  }
  inline void	B_Compute_dWt_CtLeabraCAL(LeabraCon* cn, LeabraUnit* ru, LeabraLayer* rlay) {
    LeabraBiasSpec::B_Compute_dWt_CtLeabraCAL(cn, ru, rlay);
    if(updt_immed) B_Compute_Weights(cn, ru);
  }

  SIMPLE_COPY(LeabraNegBiasSpec);
  TA_BASEFUNS(LeabraNegBiasSpec);
private:
  void 	Initialize();
  void	Destroy()		{ };
};

//////////////////////////////////////////
// 	Synaptic Depression: Trial Level

class LEABRA_API TrialSynDepCon : public LeabraCon {
  // synaptic depression connection at the trial level (as opposed to cycle level)
INHERITED(LeabraCon)
public:
  float		effwt;		// #NO_SAVE effective weight value (can be depressed) -- used for sending ativation

  TrialSynDepCon() { effwt = 0.0f; }
};

class LEABRA_API TrialSynDepSpec : public taOBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra specs for synaptic depression
INHERITED(taOBase)
public:
  float		rec;		// #DEF_1 rate of recovery from depression
  float		depl;		// #DEF_1.1 rate of depletion of synaptic efficacy as a function of sender-receiver activations

  SIMPLE_COPY(TrialSynDepSpec);
  TA_BASEFUNS(TrialSynDepSpec);
private:
  void	Initialize();
  void 	Destroy()	{ };
};

class LEABRA_API TrialSynDepConSpec : public LeabraConSpec {
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
  virtual void Depress_Wt(LeabraRecvCons* cg, LeabraUnit* ru) {
    CON_GROUP_LOOP(cg, C_Depress_Wt((TrialSynDepCon*)cg->Cn(i), ru, (LeabraUnit*)cg->Un(i)));
  }

  void Compute_dWt_LeabraCHL(LeabraRecvCons* cg, LeabraUnit* ru) {
    inherited::Compute_dWt_LeabraCHL(cg, ru);
    Depress_Wt(cg, ru);
  }
  void Compute_dWt_CtLeabraXCAL(LeabraRecvCons* cg, LeabraUnit* ru) {
    inherited::Compute_dWt_CtLeabraXCAL(cg, ru);
    Depress_Wt(cg, ru);
  }
  void Compute_dWt_CtLeabraCAL(LeabraRecvCons* cg, LeabraUnit* ru) {
    inherited::Compute_dWt_CtLeabraCAL(cg, ru);
    Depress_Wt(cg, ru);
  }

  void C_Reset_EffWt(TrialSynDepCon* cn) {
    cn->effwt = cn->wt;
  }
  virtual void Reset_EffWt(LeabraRecvCons* cg) {
    CON_GROUP_LOOP(cg, C_Reset_EffWt((TrialSynDepCon*)cg->Cn(i)));
  }
  virtual void Reset_EffWt(LeabraSendCons* cg) {
    CON_GROUP_LOOP(cg, C_Reset_EffWt((TrialSynDepCon*)cg->Cn(i)));
  }

  void 	C_Init_Weights_post(RecvCons* cg, Connection* cn, Unit* ru, Unit* su) {
    inherited::C_Init_Weights_post(cg, cn, ru, su);
    TrialSynDepCon* lcn = (TrialSynDepCon*)cn; lcn->effwt = lcn->wt;
  }

  inline void C_Send_NetinDelta_Thrd(TrialSynDepCon* cn, float* send_netin_vec,
				    LeabraUnit* ru, float su_act_delta_eff) {
    send_netin_vec[ru->flat_idx] += cn->effwt * su_act_delta_eff;
  }

  inline void C_Send_NetinDelta_NoThrd(TrialSynDepCon* cn, LeabraUnit* ru, float su_act_delta_eff) {
    ru->net_delta += cn->effwt * su_act_delta_eff;
  }

  inline void C_Send_InhibDelta_Thrd(TrialSynDepCon* cn, float* send_inhib_vec,
				    LeabraUnit* ru, float su_act_delta_eff) {
    send_inhib_vec[ru->flat_idx] += cn->effwt * su_act_delta_eff;
  }

  inline void C_Send_InhibDelta_NoThrd(TrialSynDepCon* cn, LeabraUnit* ru, float su_act_delta_eff) {
    ru->g_i_delta += cn->effwt * su_act_delta_eff;
  }

  inline void Send_NetinDelta(LeabraSendCons* cg, LeabraNetwork* net,
			      int thread_no, float su_act_delta) {
    Unit* ru = cg->Un(0);
    float su_act_delta_eff = ((LeabraRecvCons*)ru->recv.FastEl(cg->recv_idx))->scale_eff
      * su_act_delta;
    if(inhib && net->inhib_cons_used) { // both must agree that inhib is ok
      if(thread_no < 0) {
	CON_GROUP_LOOP(cg, C_Send_InhibDelta_NoThrd((TrialSynDepCon*)cg->Cn(i),
						   (LeabraUnit*)cg->Un(i), su_act_delta_eff));
      }
      else {
	float* send_inhib_vec = net->send_inhib_tmp.el
	  + net->send_inhib_tmp.FastElIndex(0, thread_no);
	CON_GROUP_LOOP(cg, C_Send_InhibDelta_Thrd((TrialSynDepCon*)cg->Cn(i),
						 send_inhib_vec, (LeabraUnit*)cg->Un(i),
						 su_act_delta_eff));
      }
    }
    else {
      if(thread_no < 0) {
	CON_GROUP_LOOP(cg, C_Send_NetinDelta_NoThrd((TrialSynDepCon*)cg->Cn(i),
						   (LeabraUnit*)cg->Un(i), su_act_delta_eff));
      }
      else {
	float* send_netin_vec = net->send_netin_tmp.el
	  + net->send_netin_tmp.FastElIndex(0, thread_no);
	CON_GROUP_LOOP(cg, C_Send_NetinDelta_Thrd((TrialSynDepCon*)cg->Cn(i), send_netin_vec,
						 (LeabraUnit*)cg->Un(i), su_act_delta_eff));
      }
    }
  }

  float C_Compute_Netin(TrialSynDepCon* cn, LeabraUnit*, LeabraUnit* su) {
    return cn->effwt * su->act_eq;
  }
  float Compute_Netin(RecvCons* cg, Unit* ru) {
    float rval=0.0f;
    CON_GROUP_LOOP(cg, rval += C_Compute_Netin((TrialSynDepCon*)cg->Cn(i), (LeabraUnit*)ru,
					       (LeabraUnit*)cg->Un(i)));
    return ((LeabraRecvCons*)cg)->scale_eff * rval;
  }


  TA_SIMPLE_BASEFUNS(TrialSynDepConSpec);
protected:
  void 	UpdateAfterEdit_impl();

private:
  void 	Initialize();
  void	Destroy()		{ };
};

////////////////////////////////////////////////////////////////
//      Synaptic Depression: Cycle Level, simple

class LEABRA_API CycleSynDepCon : public LeabraCon {
  // synaptic depression connection at the cycle level (as opposed to the trial level) -- this is the simpler version -- Ca_i based version below
INHERITED(LeabraCon)
public:
  float		effwt;		// #NO_SAVE effective weight value (subject to synaptic depression) -- used for sending activation

  CycleSynDepCon() { effwt = 0.0f; }
};

class LEABRA_API CycleSynDepSpec : public taOBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra specs for synaptic depression
INHERITED(taOBase)
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

  SIMPLE_COPY(CycleSynDepSpec);
  TA_BASEFUNS(CycleSynDepSpec);
protected:
  void UpdateAfterEdit_impl();

private:
  void	Initialize();
  void 	Destroy()	{ };
};

class LEABRA_API CycleSynDepConSpec : public LeabraConSpec {
  // synaptic depression connection at the cycle level (as opposed to the trial level) -- this is the simpler version -- Ca_i based version below
INHERITED(LeabraConSpec)
public:
  CycleSynDepSpec	syn_dep;	// synaptic depression specifications

  inline void C_Compute_CycSynDep(CycleSynDepCon* cn, LeabraUnit* ru, LeabraUnit* su) {
    syn_dep.Depress(cn->effwt, cn->wt, ru->act_eq, su->act_eq);
  }
  inline override void Compute_CycSynDep(LeabraRecvCons* cg, LeabraUnit* ru) {
    CON_GROUP_LOOP(cg, C_Compute_CycSynDep((CycleSynDepCon*)cg->Cn(i), ru, (LeabraUnit*)cg->Un(i)));
  }

  void C_Reset_EffWt(CycleSynDepCon* cn) {
    cn->effwt = cn->wt;
  }
  virtual void Reset_EffWt(LeabraRecvCons* cg) {
    CON_GROUP_LOOP(cg, C_Reset_EffWt((CycleSynDepCon*)cg->Cn(i)));
  }
  virtual void Reset_EffWt(LeabraSendCons* cg) {
    CON_GROUP_LOOP(cg, C_Reset_EffWt((CycleSynDepCon*)cg->Cn(i)));
  }

  void 	C_Init_Weights_post(RecvCons* cg, Connection* cn, Unit* ru, Unit* su) {
    inherited::C_Init_Weights_post(cg, cn, ru, su);
    CycleSynDepCon* lcn = (CycleSynDepCon*)cn; lcn->effwt = lcn->wt;
  }

  inline void C_Send_NetinDelta_Thrd(CycleSynDepCon* cn, float* send_netin_vec,
				    LeabraUnit* ru, float su_act_delta_eff) {
    send_netin_vec[ru->flat_idx] += cn->effwt * su_act_delta_eff;
  }

  inline void C_Send_NetinDelta_NoThrd(CycleSynDepCon* cn, LeabraUnit* ru, float su_act_delta_eff) {
    ru->net_delta += cn->effwt * su_act_delta_eff;
  }

  inline void C_Send_InhibDelta_Thrd(CycleSynDepCon* cn, float* send_inhib_vec,
				    LeabraUnit* ru, float su_act_delta_eff) {
    send_inhib_vec[ru->flat_idx] += cn->effwt * su_act_delta_eff;
  }

  inline void C_Send_InhibDelta_NoThrd(CycleSynDepCon* cn, LeabraUnit* ru, float su_act_delta_eff) {
    ru->g_i_delta += cn->effwt * su_act_delta_eff;
  }

  inline void Send_NetinDelta(LeabraSendCons* cg, LeabraNetwork* net,
			      int thread_no, float su_act_delta) {
    Unit* ru = cg->Un(0);
    float su_act_delta_eff = ((LeabraRecvCons*)ru->recv.FastEl(cg->recv_idx))->scale_eff
      * su_act_delta;
    if(inhib && net->inhib_cons_used) { // both must agree that inhib is ok
      if(thread_no < 0) {
	CON_GROUP_LOOP(cg, C_Send_InhibDelta_NoThrd((CycleSynDepCon*)cg->Cn(i),
						   (LeabraUnit*)cg->Un(i), su_act_delta_eff));
      }
      else {
	float* send_inhib_vec = net->send_inhib_tmp.el
	  + net->send_inhib_tmp.FastElIndex(0, thread_no);
	CON_GROUP_LOOP(cg, C_Send_InhibDelta_Thrd((CycleSynDepCon*)cg->Cn(i),
						 send_inhib_vec, (LeabraUnit*)cg->Un(i),
						 su_act_delta_eff));
      }
    }
    else {
      if(thread_no < 0) {
	CON_GROUP_LOOP(cg, C_Send_NetinDelta_NoThrd((CycleSynDepCon*)cg->Cn(i),
						   (LeabraUnit*)cg->Un(i), su_act_delta_eff));
      }
      else {
	float* send_netin_vec = net->send_netin_tmp.el
	  + net->send_netin_tmp.FastElIndex(0, thread_no);
	CON_GROUP_LOOP(cg, C_Send_NetinDelta_Thrd((CycleSynDepCon*)cg->Cn(i), send_netin_vec,
						 (LeabraUnit*)cg->Un(i), su_act_delta_eff));
      }
    }
  }

  float C_Compute_Netin(CycleSynDepCon* cn, LeabraUnit*, LeabraUnit* su) {
    return cn->effwt * su->act_eq;
  }
  float Compute_Netin(RecvCons* cg, Unit* ru) {
    float rval=0.0f;
    CON_GROUP_LOOP(cg, rval += C_Compute_Netin((CycleSynDepCon*)cg->Cn(i), (LeabraUnit*)ru,
					       (LeabraUnit*)cg->Un(i)));
    return ((LeabraRecvCons*)cg)->scale_eff * rval;
  }

  TA_SIMPLE_BASEFUNS(CycleSynDepConSpec);
protected:
  void 	UpdateAfterEdit_impl();

private:
  void 	Initialize();
  void	Destroy()		{ };
};

////////////////////////////////////////////////////////////////
//      Synaptic Depression: Cycle Level, Ca-Based

class LEABRA_API CaiSynDepCon : public LeabraCon {
  // synaptic depression connection at the cycle level, based on synaptic integration of calcium
INHERITED(LeabraCon)
public:
  float		effwt;		// #NO_SAVE effective weight value (subject to synaptic depression) -- used for sending activation
  float		cai;		// #NO_SAVE intracellular postsynaptic calcium current integrated over cycles, used for synaptic depression

  CaiSynDepCon() { effwt = 0.0f; cai = 0.0f; }
};

class LEABRA_API CaiSynDepSpec : public taOBase {
  // ##INLINE ##NO_TOKENS ##CAT_Leabra specs for synaptic depression based in synaptic integration of calcium
INHERITED(taOBase)
public:
  float		ca_inc;		// #DEF_0.2 time constant for increases in Ca_i (from NMDA etc currents) -- default base value is .01 per cycle -- multiply by network->ct_learn.syndep_int to get this value (default = 20)
  float		ca_dec;		// #DEF_0.2 time constant for decreases in Ca_i (from Ca pumps pushing Ca back out into the synapse) -- default base value is .01 per cycle -- multiply by network->ct_learn.syndep_int to get this value (default = 20)

  float		sd_ca_thr;	// #DEF_0.2 synaptic depression ca threshold: only when ca_i has increased by this amount (thus synaptic ca depleted) does it affect firing rates and thus synaptic depression
  float		sd_ca_gain;	// #DEF_0.3 multiplier on cai value for computing synaptic depression -- modulates overall level of depression independent of rate parameters
  float		sd_ca_thr_rescale; // #READ_ONLY rescaling factor taking into account sd_ca_gain and sd_ca_thr (= sd_ca_gain/(1 - sd_ca_thr))

  inline void	CaUpdt(float& cai, float ru_act, float su_act) {
    float drive = ru_act * su_act;
    cai += ca_inc * (1.0f - cai) * drive - ca_dec * cai;
  }

  inline float	SynDep(float cai) {
    float cao_thr = (cai > sd_ca_thr) ? (1.0 - sd_ca_thr_rescale * (cai - sd_ca_thr)) : 1.0f;
    return cao_thr * cao_thr;
  }

  SIMPLE_COPY(CaiSynDepSpec);
  TA_BASEFUNS(CaiSynDepSpec);
protected:
  void UpdateAfterEdit_impl();

private:
  void	Initialize();
  void 	Destroy()	{ };
};

class LEABRA_API CaiSynDepConSpec : public LeabraConSpec {
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
  inline void Compute_Cai(LeabraRecvCons* cg, LeabraUnit* ru) {
    CON_GROUP_LOOP(cg, C_Compute_Cai((CaiSynDepCon*)cg->Cn(i), ru, (LeabraUnit*)cg->Un(i)));
  }

  // connection-level synaptic depression: syn dep direct
  inline void C_Compute_CycSynDep(CaiSynDepCon* cn, LeabraUnit* ru, LeabraUnit* su) {
    cn->effwt = cn->wt * ca_dep.SynDep(cn->cai);
  }
  // connection-level synaptic depression: ca mediated
  inline override void Compute_CycSynDep(LeabraRecvCons* cg, LeabraUnit* ru) {
    Compute_Cai(cg, ru);
    CON_GROUP_LOOP(cg, C_Compute_CycSynDep((CaiSynDepCon*)cg->Cn(i), ru, (LeabraUnit*)cg->Un(i)));
  }
  // connection-group level synaptic depression

  inline void C_Init_SdEffWt(CaiSynDepCon* cn) {
    cn->effwt = cn->wt; cn->cai = 0.0f; 
  }
  inline void Init_SdEffWt(LeabraRecvCons* cg) {
    CON_GROUP_LOOP(cg, C_Init_SdEffWt((CaiSynDepCon*)cg->Cn(i)));
  }
  // #CAT_Activation reset synaptic depression effective weight (remove any existing synaptic depression and associated variables)
  inline void Init_SdEffWt(LeabraSendCons* cg) {
    CON_GROUP_LOOP(cg, C_Init_SdEffWt((CaiSynDepCon*)cg->Cn(i)));
  }
  // #CAT_Activation reset synaptic depression effective weight (remove any existing synaptic depression and associated variables)

  override void C_Init_Weights_post(RecvCons* cg, Connection* cn, Unit* ru, Unit* su) {
    inherited::C_Init_Weights_post(cg, cn, ru, su);
    CaiSynDepCon* lcn = (CaiSynDepCon*)cn; lcn->effwt = lcn->wt;
    lcn->cai = 0.0f; 
  }

  inline void C_Send_NetinDelta_Thrd(CaiSynDepCon* cn, float* send_netin_vec,
				    LeabraUnit* ru, float su_act_delta_eff) {
    send_netin_vec[ru->flat_idx] += cn->effwt * su_act_delta_eff;
  }

  inline void C_Send_NetinDelta_NoThrd(CaiSynDepCon* cn, LeabraUnit* ru, float su_act_delta_eff) {
    ru->net_delta += cn->effwt * su_act_delta_eff;
  }

  inline void C_Send_InhibDelta_Thrd(CaiSynDepCon* cn, float* send_inhib_vec,
				    LeabraUnit* ru, float su_act_delta_eff) {
    send_inhib_vec[ru->flat_idx] += cn->effwt * su_act_delta_eff;
  }

  inline void C_Send_InhibDelta_NoThrd(CaiSynDepCon* cn, LeabraUnit* ru, float su_act_delta_eff) {
    ru->g_i_delta += cn->effwt * su_act_delta_eff;
  }

  inline void Send_NetinDelta(LeabraSendCons* cg, LeabraNetwork* net,
			      int thread_no, float su_act_delta) {
    Unit* ru = cg->Un(0);
    float su_act_delta_eff = ((LeabraRecvCons*)ru->recv.FastEl(cg->recv_idx))->scale_eff
      * su_act_delta;
    if(inhib && net->inhib_cons_used) { // both must agree that inhib is ok
      if(thread_no < 0) {
	CON_GROUP_LOOP(cg, C_Send_InhibDelta_NoThrd((CaiSynDepCon*)cg->Cn(i),
						   (LeabraUnit*)cg->Un(i), su_act_delta_eff));
      }
      else {
	float* send_inhib_vec = net->send_inhib_tmp.el
	  + net->send_inhib_tmp.FastElIndex(0, thread_no);
	CON_GROUP_LOOP(cg, C_Send_InhibDelta_Thrd((CaiSynDepCon*)cg->Cn(i),
						 send_inhib_vec, (LeabraUnit*)cg->Un(i),
						 su_act_delta_eff));
      }
    }
    else {
      if(thread_no < 0) {
	CON_GROUP_LOOP(cg, C_Send_NetinDelta_NoThrd((CaiSynDepCon*)cg->Cn(i),
						   (LeabraUnit*)cg->Un(i), su_act_delta_eff));
      }
      else {
	float* send_netin_vec = net->send_netin_tmp.el
	  + net->send_netin_tmp.FastElIndex(0, thread_no);
	CON_GROUP_LOOP(cg, C_Send_NetinDelta_Thrd((CaiSynDepCon*)cg->Cn(i), send_netin_vec,
						 (LeabraUnit*)cg->Un(i), su_act_delta_eff));
      }
    }
  }

  float C_Compute_Netin(CaiSynDepCon* cn, LeabraUnit*, LeabraUnit* su) {
    return cn->effwt * su->act_eq;
  }
  float Compute_Netin(RecvCons* cg, Unit* ru) {
    float rval=0.0f;
    CON_GROUP_LOOP(cg, rval += C_Compute_Netin((CaiSynDepCon*)cg->Cn(i), 
					       (LeabraUnit*)ru, (LeabraUnit*)cg->Un(i)));
    return ((LeabraRecvCons*)cg)->scale_eff * rval;
  }

  TA_SIMPLE_BASEFUNS(CaiSynDepConSpec);
protected:
  void 	UpdateAfterEdit_impl();

private:
  void 	Initialize();
  void	Destroy()		{ };
};

/////////////////////////////////////////////////
//		Fast Weights

// todo: this code needs a lot of work!

class LEABRA_API FastWtCon : public LeabraCon {
  // fast weight connection: standard wt learns fast, but decays toward slow weight value
public:
  float		swt;		// slow weight value
  float		sdwt;		// #NO_SAVE slow weight delta-weight change

  FastWtCon() { swt = sdwt = 0.0f; }
};

class LEABRA_API FastWtSpec : public taOBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra specificiations for fast weights
INHERITED(taOBase)
public:
  enum DecayMode {
    ALWAYS,			// always decay all weights toward slow weight (swt)
    SU_PROP,			// decay in proportion to the sending unit activation: decay only happens after sending activity dissipates
    SU_THR			// decay only weights with sending unit activations below sending threshold (nom .1): decay only happens after sending activity dissipates
  };

  float		lrate;		// learning rate
  bool		use_lrs;	// #DEF_false use learning rate schedule to modify cur_lrate learning rate?
  float		cur_lrate;	// #READ_ONLY #NO_INHERIT #SHOW current learning rate with lrate schedule factored in
  float		decay;		// rate of decay toward the slow weight values
  bool		slw_sat;	// #DEF_true does fast weight contribute to saturation of slow weights?
  DecayMode	dk_mode;	// how to apply the decay of fast weights back to the slow weight (swt) value

  SIMPLE_COPY(FastWtSpec);
  TA_BASEFUNS(FastWtSpec);
private:
  void	Initialize();
  void	Destroy()	{ };
};

// TODO: the decay in this code cannot be parallelized over dwt's because the dynamics will
// be different!!!
// need to have a dmem small_batch over sequences type mode that does a SyncWts using sum_dwts = false
// and calls Compute_Weights after each trial..

// TODO: following code is not dealing with contrast enhancement on the swt vals!!!!

class LEABRA_API FastWtConSpec : public LeabraConSpec {
  // fast weight connection: standard wt learns fast, but decays toward slow weight value
INHERITED(LeabraConSpec)
public:
  FastWtSpec	fast_wt;	// fast weight specs: fast weights are added in separately to overall weight value as an increment (

  void 		C_Init_Weights_post(RecvCons* cg, Connection* cn, Unit* ru, Unit* su) {
    inherited::C_Init_Weights_post(cg, cn, ru, su);
    FastWtCon* lcn = (FastWtCon*)cn; lcn->swt = lcn->wt;
  }

  void 		C_Init_dWt(RecvCons* cg, Connection* cn, Unit* ru, Unit* su)
  { ConSpec::C_Init_dWt(cg, cn, ru, su); ((FastWtCon*)cn)->sdwt=0.0f; }

  override void SetCurLrate(LeabraNetwork* net, int epoch);

  inline float C_Compute_SlowHebb(FastWtCon* cn, LeabraRecvCons* cg,
				  float lin_wt, float ru_act, float su_act)
  {
    float swt_eff = (fast_wt.slw_sat) ? lin_wt : cn->swt;
    return ru_act * (su_act * (cg->savg_cor - swt_eff) - (1.0f - su_act) * swt_eff);
  }

  // todo: somewhat inefficient to do this computation twice..

  // generec error term with sigmoid activation function, and soft bounding
  inline float C_Compute_SlowErr(FastWtCon* cn, float lin_wt, float ru_act_p, float ru_act_m, float su_act_p, float su_act_m) {
    float err = (ru_act_p * su_act_p) - (ru_act_m * su_act_m);
    float swt_eff = (fast_wt.slw_sat) ? lin_wt : cn->swt;
    if(err > 0.0f)	err *= (1.0f - swt_eff);
    else		err *= swt_eff;	
    return err;
  }

  inline void C_Compute_SlowdWt(FastWtCon* cn, LeabraUnit*, float heb, float err) {
    float sdwt = lmix.err * err + lmix.hebb * heb;
    cn->sdwt += cur_lrate * sdwt; // standard lrate is slow one..
  }

  // combine hebbian and error-driven
  inline void C_Compute_dWt(LeabraCon* cn, LeabraUnit*, float heb, float err) {
    float dwt = lmix.err * err + lmix.hebb * heb;
    cn->dwt += fast_wt.cur_lrate * dwt;	// the regular weight is the fast one!
  }

  inline void C_Compute_FastDecay(FastWtCon* cn, LeabraUnit*, LeabraUnit* su) {
    if((fast_wt.dk_mode == FastWtSpec::SU_THR) && (su->act_p > .1f)) return;
    if(fast_wt.dk_mode == FastWtSpec::SU_PROP)
      cn->wt += (1.0f - su->act_p) * fast_wt.decay * (-cn->swt - cn->wt); // decay toward slow weights..
    else
      cn->wt += fast_wt.decay * (-cn->swt - cn->wt); // decay toward slow weights..
  }

  // todo: do CtLeabra_XCal and cal

  inline void Compute_dWt_LeabraCHL(LeabraRecvCons* cg, LeabraUnit* ru) {
    Compute_SAvgCor(cg, ru);
    if(((LeabraLayer*)cg->prjn->from.ptr())->acts_p.avg >= savg_cor.thresh) {
      for(int i=0; i<cg->cons.size; i++) {
	LeabraUnit* su = (LeabraUnit*)cg->Un(i);
	FastWtCon* cn = (FastWtCon*)cg->Cn(i);
	if(!(su->in_subgp &&
	     (((LeabraUnit_Group*)su->owner)->acts_p.avg < savg_cor.thresh))) {
	  C_Compute_FastDecay(cn, ru, su);
	  float lin_wt = LinFmSigWt(cn->wt);
	  C_Compute_dWt(cn, ru, 
			C_Compute_Hebb(cn, cg, lin_wt, ru->act_p, su->act_p),
			C_Compute_Err_LeabraCHL(cn, lin_wt, ru->act_p, ru->act_m, su->act_p, su->act_m));  
	  C_Compute_SlowdWt(cn, ru, 
			    C_Compute_SlowHebb(cn, cg, lin_wt, ru->act_p, su->act_p),
			    C_Compute_SlowErr(cn, lin_wt, ru->act_p, ru->act_m, su->act_p, su->act_m));  
	}
      }
    }
  }

  inline void C_Compute_Weights_LeabraCHL(FastWtCon* cn) {
    if(cn->sdwt != 0.0f) {
      cn->swt += cn->sdwt; // wt is not negative!
      if(cn->swt < wt_limits.min) cn->swt = wt_limits.min;
      if(cn->swt > wt_limits.max) cn->swt = wt_limits.max;
    }
    if(cn->dwt != 0.0f) {
      // always do this because of the decay term..
      cn->wt = SigFmLinWt(LinFmSigWt(cn->wt) + cn->dwt);
    }
    cn->pdw = cn->dwt;
    cn->dwt = 0.0f;
    cn->sdwt = 0.0f;
  }

  inline void Compute_Weights_LeabraCHL(LeabraRecvCons* cg, LeabraUnit* ru) {
    CON_GROUP_LOOP(cg, C_Compute_Weights_LeabraCHL((FastWtCon*)cg->Cn(i)));
    //  ApplyLimits(cg, ru); limits are automatically enforced anyway
  }

  TA_SIMPLE_BASEFUNS(FastWtConSpec);
private:
  void 	Initialize();
  void	Destroy()		{ };
};


//////////////////////////////////////////////////////////////////////////
//	Activation Trace Hebbian learning (Foldiak, Rolls etc)

class LEABRA_API ActAvgHebbMixSpec : public taOBase {
  // ##INLINE ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra mixture of average activation hebbian learning and regular hebbian learning (on current act value)
INHERITED(taOBase)
public:
  float		act_avg;	// what proportion of average activation to include in hebbian receiving unit activation learning term
  float		cur_act;	// #READ_ONLY #SHOW 1.0 - act_avg -- proportion of current activation for hebbian learning

  SIMPLE_COPY(ActAvgHebbMixSpec);
  TA_BASEFUNS(ActAvgHebbMixSpec);
protected:
  void	UpdateAfterEdit_impl();
private:
  void	Initialize();
  void	Destroy()	{ };
};

class LEABRA_API ActAvgHebbConSpec : public LeabraConSpec {
  // hebbian learning that includes a proportion of average activation over time, in addition to standard current unit activation;  produces a trace-based learning effect for learning over trajectories -- only for Leabra_CHL
INHERITED(LeabraConSpec)
public:
  ActAvgHebbMixSpec	act_avg_hebb; // mixture of current and average activations to use in hebbian learning

 inline float C_Compute_Hebb(LeabraCon* cn, LeabraRecvCons* cg, float lin_wt,
			     float ru_act, float su_act, float ru_avg_act)
  {
    // wt is negative in linear form, so using opposite sign of usual here
    float eff_ru_act = act_avg_hebb.act_avg * ru_avg_act + act_avg_hebb.cur_act * ru_act;
    return eff_ru_act * (su_act * (cg->savg_cor - lin_wt) - (1.0f - su_act) * lin_wt);
  }

  // this computes weight changes based on sender at time t-1
  inline void Compute_dWt_LeabraCHL(LeabraRecvCons* cg, LeabraUnit* ru) {
    Compute_SAvgCor(cg, ru);
    if(((LeabraLayer*)cg->prjn->from.ptr())->acts_p.avg >= savg_cor.thresh) {
      for(int i=0; i<cg->cons.size; i++) {
	LeabraUnit* su = (LeabraUnit*)cg->Un(i);
	LeabraCon* cn = (LeabraCon*)cg->Cn(i);
	if(!(su->in_subgp &&
	     (((LeabraUnit_Group*)su->owner)->acts_p.avg < savg_cor.thresh))) {
	  float lin_wt = LinFmSigWt(cn->wt);
	  C_Compute_dWt(cn, ru, 
			C_Compute_Hebb(cn, cg, lin_wt, ru->act_p, su->act_p, ru->act_avg),
			C_Compute_Err_LeabraCHL(cn, lin_wt, ru->act_p, ru->act_m, su->act_p, su->act_m));  
	}
      }
    }
  }

  TA_SIMPLE_BASEFUNS(ActAvgHebbConSpec);
protected:
  void	UpdateAfterEdit_impl();
private:
  void 	Initialize();
  void	Destroy()		{ };
};

////////////////////////////////////////////////////////////////////////
//	Limited precision weights: for hardware impl testing

class LEABRA_API LeabraLimPrecConSpec : public LeabraConSpec {
  // ##CAT_Leabra Leabra limited precision connection specs: limits weight values to specified level of precision between 0-1
INHERITED(LeabraConSpec)
public:
  float		prec_levels;	// number of levels of precision available in the weight values

  inline float	PrecLimitVal(float val) {
    int tmp = (int)((prec_levels * val) + .5f); // integerize with rounding -- val 0-1
    float rval = (float)tmp / prec_levels;
    if(rval > 1.0f) rval = 1.0f;
    if(rval < 0.0f) rval = 0.0f;
    return rval;
  }

  inline void C_Compute_LimPrecWts(LeabraCon* cn) {
    cn->wt = PrecLimitVal(cn->wt);
  }
  inline void Compute_LimPrecWts(LeabraRecvCons* cg, LeabraUnit* ru) {
    CON_GROUP_LOOP(cg, C_Compute_LimPrecWts((LeabraCon*)cg->Cn(i)));
  }

  inline override void	Compute_Weights_LeabraCHL(LeabraRecvCons* cg, LeabraUnit* ru) {
    inherited::Compute_Weights_LeabraCHL(cg, ru);
    Compute_LimPrecWts(cg, ru);
  }

  inline override void	Compute_Weights_CtLeabraXCAL(LeabraRecvCons* cg, LeabraUnit* ru) {
    inherited::Compute_Weights_CtLeabraXCAL(cg, ru);
    Compute_LimPrecWts(cg, ru);
  }

  inline override void	Compute_Weights_CtLeabraCAL(LeabraRecvCons* cg, LeabraUnit* ru) {
    inherited::Compute_Weights_CtLeabraCAL(cg, ru);
    Compute_LimPrecWts(cg, ru);
  }

  // NOTE: bias weights typically not subject to limited precision!

  SIMPLE_COPY(LeabraLimPrecConSpec);
  TA_BASEFUNS(LeabraLimPrecConSpec);
// protected:
//   void	UpdateAfterEdit_impl();
private:
  void 	Initialize();
  void	Destroy()		{ };
};

///////////////////////////////////////////////////////////////////////
//	dwt normalization -- enforced zero-sum learning

class LEABRA_API LeabraDwtNorm : public taBase {
  // ##INLINE ##NO_TOKENS ##CAT_Leabra renormalize weight changes -- makes zero sum
INHERITED(taBase)
public:
  bool		on;		// whether to do normalized dwt Compute_Weights function: makes weight changes zero sum
  float		norm_pct;	// #CONDEDIT_ON_on what proportion of full normalization to apply to the delta weights (0 = no norm, 1 = full norm)

  SIMPLE_COPY(LeabraDwtNorm);
  TA_BASEFUNS(LeabraDwtNorm);
  //protected:
  //  void UpdateAfterEdit_impl();

private:
  void	Initialize();
  void 	Destroy()	{ };
};

class LEABRA_API LeabraDwtNormConSpec : public LeabraConSpec {
  // ##CAT_Leabra delta-weight normalization: weight changes are zero-sum.  also includes hebbian learning for CtLeabraCAL learning rule
INHERITED(LeabraConSpec)
public:
#ifdef __MAKETA__
  LearnMixSpec	lmix;		// #CAT_Learning #CONDSHOW_OFF_learn_rule:CTLEABRA_XCAL mixture of hebbian & err-driven learning (note: no hebbian for CTLEABRA_XCAL, but yes for CTLEABRA_CAL)
#endif

  LeabraDwtNorm	dwt_norm;	// renormalize weight changes to compensate for overal mean changes (zero sum weight changes)

  inline float	Compute_dWtMean(LeabraRecvCons* cg, LeabraUnit* ru) {
    float dwt_mean = 0.0f;
    CON_GROUP_LOOP(cg, dwt_mean += ((LeabraCon*)cg->Cn(i))->dwt);
    return dwt_mean / (float)cg->cons.size;
  }
  // #CAT_Learning compute dwt_mean -- mean of all weight changes, for dwt_norm

  inline void C_Compute_Weights_Norm_LeabraCHL(LeabraCon* cn, float dwnorm) {
    float eff_dw = cn->dwt + dwnorm;
    if(eff_dw != 0.0f) {
      // weight increment happens in linear weights
      cn->wt = SigFmLinWt(LinFmSigWt(cn->wt) + eff_dw);
    }
    cn->pdw = cn->dwt;
    cn->dwt = 0.0f;
  }

  inline void Compute_Weights_LeabraCHL(LeabraRecvCons* cg, LeabraUnit* ru) {
    if(dwt_norm.on) {
      float dwnorm = -dwt_norm.norm_pct * Compute_dWtMean(cg, ru);
      CON_GROUP_LOOP(cg, C_Compute_Weights_Norm_LeabraCHL((LeabraCon*)cg->Cn(i), dwnorm));
    }
    else {
      CON_GROUP_LOOP(cg, C_Compute_Weights_LeabraCHL((LeabraCon*)cg->Cn(i)));
    }
  }

  inline void C_Compute_dWt_CtLeabraCAL_hebb(LeabraCon* cn,
					     float sravg_s_nrm, float sravg_m_nrm, float hebb) {
    cn->dwt += cur_lrate * (lmix.err * (sravg_s_nrm * cn->sravg_s - sravg_m_nrm * cn->sravg_m)
			    + lmix.hebb * hebb);
  }

  inline void Compute_dWt_CtLeabraCAL(LeabraRecvCons* cg, LeabraUnit* ru) {
    Compute_SAvgCor(cg, ru);
    LeabraLayer* rlay = (LeabraLayer*)cg->prjn->layer;
    LeabraNetwork* net = (LeabraNetwork*)rlay->own_net;
    for(int i=0; i<cg->cons.size; i++) {
      LeabraUnit* su = (LeabraUnit*)cg->Un(i);
      LeabraCon* cn = (LeabraCon*)cg->Cn(i);
      C_Compute_dWt_CtLeabraCAL_hebb
	(cn, net->sravg_vals.s_nrm, net->sravg_vals.m_nrm,
	 C_Compute_Hebb(cn, cg, cn->wt, ru->act_p, su->act_p));
    }
  }

  inline void C_Compute_Weights_Norm_CtLeabraCAL(LeabraCon* cn, float dwnorm) {
    // do soft bounding now so it can include actreg, agg from dm, dwtnorm, 
    cn->dwt += dwnorm;
    if(cn->dwt > 0.0f)	cn->dwt *= (1.0f - cn->wt);
    else		cn->dwt *= cn->wt;

    cn->wt += cn->dwt;	// weights always linear
    // optimize: don't bother with this if always doing soft bounding above
    //     wt_limits.ApplyMinLimit(cn->wt); wt_limits.ApplyMaxLimit(cn->wt);
    cn->pdw = cn->dwt;
    cn->dwt = 0.0f;
  }

  inline void Compute_Weights_CtLeabraCAL(LeabraRecvCons* cg, LeabraUnit* ru) {
    if(dwt_norm.on) {
      float dwnorm = -dwt_norm.norm_pct * Compute_dWtMean(cg, ru);
      CON_GROUP_LOOP(cg, C_Compute_Weights_Norm_CtLeabraCAL((LeabraCon*)cg->Cn(i), dwnorm));
    }
    else {
      CON_GROUP_LOOP(cg, C_Compute_Weights_CtLeabraCAL((LeabraCon*)cg->Cn(i)));
    }
    //  ApplyLimits(cg, ru); limits are automatically enforced anyway
  }

  TA_SIMPLE_BASEFUNS(LeabraDwtNormConSpec);
// protected:
//   void	UpdateAfterEdit_impl();
private:
  void 	Initialize();
  void	Destroy()		{ };
};

////////////////////////////////////////////////////////////////////////
//	da-noise modulated learning as in MazzoniAndersenJordan91

class LEABRA_API LeabraDaNoise : public taBase {
  // ##INLINE ##NO_TOKENS ##CAT_Leabra da-noise modulated learning as in MazzoniAndersenJordan91
INHERITED(taBase)
public:
  float		da_noise;	// #DEF_0:1 amount to add of additional reinforcement-learning term based on unit dopamine value (dav) and TRIAL_VM_NOISE noise value, as in MazzoniAndersenJordan91: dwt = dav * (ru_act_p+noise - ru_act_p-noise) * su_act -- activation with noise (std acts as computed) minus activation without noise (specially computed given noise value) times sending activation times dopamine value -- if it does better and noise made unit more active, then make it more active next time (and so on for all other permutations)
  float		std_leabra;	// #DEF_0:1 how much of standard leabra learning to include in addition to the da_noise term

  SIMPLE_COPY(LeabraDaNoise);
  TA_BASEFUNS(LeabraDaNoise);
  //protected:
  //  void UpdateAfterEdit_impl();
private:
  void	Initialize();
  void 	Destroy()	{ };
};

class LEABRA_API LeabraDaNoiseConSpec : public LeabraConSpec {
  // ##CAT_Leabra da-noise modulated learning as in MazzoniAndersenJordan91
INHERITED(LeabraConSpec)
public:
  LeabraDaNoise	da_noise;	// how much da_noise based learning to include relative to std leabra

  inline void C_Compute_dWt_DaNoise(LeabraCon* cn, float lin_wt, float dav,
				    float ru_act, float ru_act_nonoise,
				    float su_act) {
    float err = dav * (ru_act - ru_act_nonoise) * su_act;
    // std leabra requires separate softbounding on all terms.. see XCAL for its version
    if(lmix.err_sb) {
      if(err > 0.0f)	err *= (1.0f - lin_wt);
      else		err *= lin_wt;
    }
    cn->dwt += cur_lrate * err;
  }

  inline void Compute_dWt_DaNoise(LeabraRecvCons* cg, LeabraUnit* ru) {
    // compute what activation value would be if we subtract out noise -- note that
    // we don't save v_m by phase so this is necessarily on the current v_m val, assumed
    // to be plus-phase value
    float ru_act_nonoise = ru->Compute_ActValFmVmVal_rate(ru->v_m - ru->noise);
    float dav = ru->dav * da_noise.da_noise;

    for(int i=0; i<cg->cons.size; i++) {
      LeabraUnit* su = (LeabraUnit*)cg->Un(i);
      LeabraCon* cn = (LeabraCon*)cg->Cn(i);
      float lin_wt = LinFmSigWt(cn->wt);
      C_Compute_dWt_DaNoise(cn, lin_wt, dav, ru->act_p, ru_act_nonoise, su->act_p);
    }
  }

  inline void C_Compute_dWt(LeabraCon* cn, LeabraUnit*, float heb, float err) {
    float dwt = lmix.err * err + lmix.hebb * heb;
    cn->dwt += da_noise.std_leabra * cur_lrate * dwt;
  }

  inline void Compute_dWt_LeabraCHL(LeabraRecvCons* cg, LeabraUnit* ru) {
    if(da_noise.std_leabra > 0.0f) {
      // this is a copy of the main fun, but uses above C_Compute_dWt which mults dwt
      Compute_SAvgCor(cg, ru);
      if(((LeabraLayer*)cg->prjn->from.ptr())->acts_p.avg >= savg_cor.thresh) {
	for(int i=0; i<cg->cons.size; i++) {
	  LeabraUnit* su = (LeabraUnit*)cg->Un(i);
	  LeabraCon* cn = (LeabraCon*)cg->Cn(i);
	  if(!(su->in_subgp &&
	       (((LeabraUnit_Group*)su->owner)->acts_p.avg < savg_cor.thresh))) {
	    float lin_wt = LinFmSigWt(cn->wt);
	    C_Compute_dWt(cn, ru, 
			  C_Compute_Hebb(cn, cg, lin_wt, ru->act_p, su->act_p),
			  C_Compute_Err_LeabraCHL(cn, lin_wt, ru->act_p, ru->act_m,
						  su->act_p, su->act_m));  
	  }
	}
      }
    }
    if(da_noise.da_noise > 0.0f) {
      Compute_dWt_DaNoise(cg, ru);
    }
  }

  // todo: add xcal version perhaps if promising..  not.. so maybe not..

  TA_SIMPLE_BASEFUNS(LeabraDaNoiseConSpec);
// protected:
//   void	UpdateAfterEdit_impl();
private:
  void 	Initialize();
  void	Destroy()		{ };
};


//////////////////////////////////
// 	Scalar Value Layer	//
//////////////////////////////////

// NOTE: got rid of sum_bar -- never worked and requires lots of different params
// to support in the future it should probably be a subclass of the main guy
// misc todo: items for SUM_BAR:
// to do equivalent of "clamp_value" (e.g., LV units at end of settle), add a special
// mode where asymptotic Vm is computed based on current params, and act from that, etc.

// also possible: unit subgroups that all have the same gc.i value, but different random
// connectivity from inputs: sending units all send to a fixed # (permute) of these 
// group units, producing a random sensory representation.  not really necc. for S2 
// spikes, because of syndep..

class LEABRA_API ScalarValSpec : public taOBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra specs for scalar values
INHERITED(taOBase)
public:
  enum	RepType {
    GAUSSIAN,			// gaussian bump, with value = weighted average of tuned unit values
    LOCALIST,			// each unit represents a distinct value; intermediate values represented by graded activity of neighbors; overall activity is weighted-average across all units
  };

  RepType	rep;		// #APPLY_IMMED type of representation of scalar value to use
  float		un_width;	// #CONDEDIT_ON_rep:GAUSSIAN sigma parameter of a gaussian specifying the tuning width of the coarse-coded units (in unit_range min-max units, unless norm_width is true, meaning use normalized 0-1 proportion of unit range)
  bool		norm_width;	// un_width is specified in normalized 0-1 proportion of unit range
  bool		clamp_pat;	// #DEF_false if true, environment provides full set of values to clamp over entire layer (instead of providing single scalar value to clamp on 1st unit, which then generates a corresponding distributed pattern)
  float		min_sum_act;	// #DEF_0.2 minimum total activity of all the units representing a value: when computing weighted average value, this is used as a minimum for the sum that you divide by
  bool		clip_val;	// ensure that value remains within specified range
  bool		send_thr;	// use unitspec.opt_thresh.send threshold to cut off small activation contributions to overall average value (i.e., if unit's activation is below this threshold, it doesn't contribute to weighted average computation)
  bool		init_nms;	// initialize unit names when weights are initialized

  float		min;		// #READ_ONLY #NO_SAVE #NO_INHERIT minimum unit value
  float		range;		// #READ_ONLY #NO_SAVE #NO_INHERIT range of unit values
  float		val;		// #READ_ONLY #NO_SAVE #NO_INHERIT current val being represented (implementational, computed in InitVal())
  float		incr;		// #READ_ONLY #NO_SAVE #NO_INHERIT increment per unit (implementational, computed in InitVal())
  float		un_width_eff;	// #READ_ONLY #NO_SAVE #NO_INHERIT effective unit range

  virtual void	InitRange(float umin, float urng);
  // initialize range values (also sets un_width_eff)
  virtual void	InitVal(float sval, int ugp_size, float umin, float urng);
  // initialize implementational values for subsequently computing GetUnitAct to represent scalar val sval over unit group of ugp_size
  virtual float	GetUnitAct(int unit_idx);
  // get activation under current representation for unit at given index: MUST CALL InitVal first!
  virtual float	GetUnitVal(int unit_idx);
  // get target value associated with unit at given index: MUST CALL InitVal first!

  TA_SIMPLE_BASEFUNS(ScalarValSpec);
private:
  void	Initialize();
  void 	Destroy()	{ };
};

class LEABRA_API ScalarValBias : public taOBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra initial bias for given activation value for scalar value units
INHERITED(taOBase)
public:
  enum UnitBias {		// bias on individual units
    NO_UN,			// no unit bias
    GC,				// bias value enters as a conductance in gc.h or gc.a
    BWT				// bias value enters as a bias.wt
  };

  enum BiasShape {		// shape of the bias pattern
    VAL,			// bias as a particular value representation
    NEG_SLP,			// bias as an increasingly negative slope (gc.a or -bwt) as unit values increase
    POS_SLP			// bias as an increasingly positive slope (gc.h or +bwt) as unit values increase
  };

  enum WeightBias {		// bias on weights into units
    NO_WT,			// no weight bias
    WT				// input weights
  };

  UnitBias	un;		// #APPLY_IMMED bias on individual units
  BiasShape	un_shp;		// shape of unit bias
  float		un_gain;	// #CONDEDIT_OFF_un:NO_UN #DEF_1 gain multiplier (strength) of bias to apply for units.  WT = .03 as basic weight multiplier
  WeightBias	wt;		// #APPLY_IMMED bias on weights: always uses a val-shaped bias
  float		wt_gain;	// #CONDEDIT_OFF_wt:NO_WT #DEF_1 gain multiplier (strength) of bias to apply for weights (gain 1 = .03 wt value)
  float		val;		// value location (center of gaussian bump)

  SIMPLE_COPY(ScalarValBias);
  TA_BASEFUNS(ScalarValBias);
private:
  void	Initialize();
  void 	Destroy()	{ };
};

class LEABRA_API ScalarValLayerSpec : public LeabraLayerSpec {
  // represents a scalar value using a coarse-coded distributed code over units.  first unit represents scalar value.
INHERITED(LeabraLayerSpec)
public:
  ScalarValSpec	 scalar;	// specifies how values are represented in terms of distributed patterns of activation across the layer
  MinMaxRange	 unit_range;	// range of values represented across the units; for GAUSSIAN, add extra values above and below true useful range to prevent edge effects.
  ScalarValBias	 bias_val;	// specifies bias for given value (as gaussian bump) 
  MinMaxRange	 val_range;	// #READ_ONLY #NO_INHERIT actual range of values (scalar.min/max taking into account un_range)

  virtual void	Settle_Init_Unit0(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_ScalarVal call Settle_Init_Unit on first unit in each group (the value unit) -- this is necessary b/c it is excluded from units_flat list and thus Compute_NetinScale, which is used for the global netin scale for the entire projection in Send_NetinDelta, and Init_TargFlags

  virtual void	ClampValue_ugp(Unit_Group* ugp, LeabraNetwork* net, float rescale=1.0f);
  // #CAT_ScalarVal clamp value in the first unit's ext field to the units in the group
  virtual float	ClampAvgAct(int ugp_size);
  // #CAT_ScalarVal computes the average activation for a clamped unit pattern (for computing rescaling)
  virtual void	ReadValue(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_ScalarVal read out current value represented by activations in layer
    virtual float ReadValue_ugp(LeabraLayer* lay, Unit_Group* ugp, LeabraNetwork* net);
    // #CAT_ScalarVal unit group version: read out current value represented by activations in layer
  virtual void 	Compute_ExtToPlus_ugp(Unit_Group* ugp, LeabraNetwork* net);
  // #CAT_ScalarVal copy ext values to act_p -- used for internally-generated training signals for learning in several subclasses
  virtual void HardClampExt(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_ScalarVal hard clamp current ext values (on all units, after ClampValue called) to all the units (calls ResetAfterClamp)
    virtual void ResetAfterClamp(LeabraLayer* lay, LeabraNetwork* net);
    // #CAT_ScalarVal reset activation of first unit(s) after hard clamping

  virtual void	LabelUnits(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_ScalarVal label units in given layer with their underlying values
    virtual void	LabelUnits_ugp(Unit_Group* ugp);
    // #CAT_ScalarVal label units with their underlying values
  virtual void	LabelUnitsNet(LeabraNetwork* net);
  // #BUTTON #CAT_ScalarVal label all layers in given network using this spec

  virtual void	Compute_BiasVal(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_ScalarVal initialize the bias value 
    virtual void Compute_WtBias_Val(Unit_Group* ugp, float val);
    // #IGNORE
    virtual void Compute_UnBias_Val(Unit_Group* ugp, float val);
    // #IGNORE
    virtual void Compute_UnBias_NegSlp(Unit_Group* ugp);
    // #IGNORE
    virtual void Compute_UnBias_PosSlp(Unit_Group* ugp);
    // #IGNORE

  override void BuildUnits_Threads(LeabraLayer* lay, LeabraNetwork* net);
    virtual void BuildUnits_Threads_ugp(LeabraLayer* lay, Unit_Group* ug, LeabraNetwork* net);
  override void Init_Weights(LeabraLayer* lay, LeabraNetwork* net);
  override void	Settle_Init_Layer(LeabraLayer* lay, LeabraNetwork* net);
  override void	Compute_HardClamp(LeabraLayer* lay, LeabraNetwork* net);
  override void	Compute_CycleStats(LeabraLayer* lay, LeabraNetwork* net);

  // don't include first unit in averages..
  override void Compute_AvgMaxVals_ugp(LeabraLayer* lay, Unit_Group* ug,
				       AvgMaxVals& vals, ta_memb_ptr mb_off);

  override float Compute_SSE(LeabraLayer* lay, LeabraNetwork* net, int& n_vals,
			     bool unit_avg = false, bool sqrt = false);
    virtual float Compute_SSE_ugp(Unit_Group* ugp, LeabraLayer* lay, int& n_vals);
    // #IGNORE
  override float Compute_NormErr(LeabraLayer* lay, LeabraNetwork* net);
    override float Compute_NormErr_ugp(LeabraLayer* lay, Unit_Group* ug, LeabraInhib* thr,
				       LeabraNetwork* net);
    // #IGNORE

  virtual void	ReConfig(Network* net, int n_units = -1);
  // #BUTTON #CAT_ScalarVal reconfigure layer and associated specs for current scalar.rep type; if n_units > 0, changes number of units in layer to specified value

  void	HelpConfig();	// #BUTTON get help message for configuring this spec
  bool  CheckConfig_Layer(LeabraLayer* lay, bool quiet=false);

  TA_SIMPLE_BASEFUNS(ScalarValLayerSpec);
protected:
  void	UpdateAfterEdit_impl();
private:
  void 	Initialize();
  void	Destroy()		{ };
};

class LEABRA_API ScalarValSelfPrjnSpec : public ProjectionSpec {
  // special projection for making self-connection that establishes neighbor similarity in scalar val
INHERITED(ProjectionSpec)
public:
  int	width;			// width of neighborhood, in units (i.e., connect width units to the left, and width units to the right)
  float	wt_width;		// width of the sigmoid for providing initial weight values
  float	wt_max;			// maximum weight value (of 1st neighbor -- not of self unit!)

  virtual void	Connect_UnitGroup(Unit_Group* gp, Projection* prjn);
  void		Connect_impl(Projection* prjn);
  void		C_Init_Weights(Projection* prjn, RecvCons* cg, Unit* ru);
  // uses weight values as specified in the tesselel's

  TA_SIMPLE_BASEFUNS(ScalarValSelfPrjnSpec);
private:
  void	Initialize();
  void 	Destroy()		{ };
};

class LEABRA_API MotorForceSpec : public taOBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra specs for scalar values
INHERITED(taOBase)
public:
  float		pos_width;	// sigma parameter of a gaussian specifying the tuning width of the coarse-coded integration over position (in pos_range or normalized units, depending on norm_width)
  float		vel_width;	// sigma parameter of a gaussian specifying the tuning width of the coarse-coded integration over velocity (in vel_range or normalized units, depending on norm_width)
  bool		norm_width;	// #DEF_true if true, use normalized 0-1 width parameters instead of raw parameters
  bool		clip_vals;	// #DEF_true clip the pos and velocity values within their ranges, for purposes of computing values

  float		cur_pos;	// #READ_ONLY #NO_SAVE #NO_INHERIT current pos val
  float		cur_vel;	// #READ_ONLY #NO_SAVE #NO_INHERIT current vel val
  float		pos_min;	// #READ_ONLY #NO_SAVE #NO_INHERIT current pos min
  float		vel_min;	// #READ_ONLY #NO_SAVE #NO_INHERIT current vel min
  float		pos_range;	// #READ_ONLY #NO_SAVE #NO_INHERIT current pos range
  float		vel_range;	// #READ_ONLY #NO_SAVE #NO_INHERIT current vel range
  float		pos_incr;	// #READ_ONLY #NO_SAVE #NO_INHERIT current pos increment
  float		vel_incr;	// #READ_ONLY #NO_SAVE #NO_INHERIT current vel increment
  float		pos_width_eff;	// #READ_ONLY #NO_SAVE #NO_INHERIT effective position width
  float		vel_width_eff;	// #READ_ONLY #NO_SAVE #NO_INHERIT effective velocity width

  virtual void	InitRanges(float pos_min, float pos_range, float vel_min, float vel_range);
  // #CAT_MotorForce initialize range parameters
  virtual void	InitVals(float pos, int pos_size, float pos_min, float pos_range, 
			float vel, int vel_size, float vel_min, float vel_range);
  // #CAT_MotorForce intiailize value and range parameters for subsequent calls
  virtual float	GetWt(int pos_gp_idx, int vel_gp_idx);
  // #CAT_MotorForce get weighting factor for position & velocity group at given indexes: MUST CALL InitVal first!

  TA_SIMPLE_BASEFUNS(MotorForceSpec);
private:
  void	Initialize();
  void 	Destroy()	{ };
};

class LEABRA_API MotorForceLayerSpec : public ScalarValLayerSpec {
  // represents motor force as a function of joint position and velocity using scalar val layer spec: layer uses unit groups -- each group represents a force (typically localist), and groups are organized in X axis by position, Y axis by velocity.  Overall value is weighted average from neighboring unit groups
INHERITED(ScalarValLayerSpec)
public:
  MotorForceSpec motor_force;   // misc specs for motor force representation
  MinMaxRange	 pos_range;	// range of position values encoded over the X axis of unit groups in the layer
  MinMaxRange	 vel_range;	// range of velocity values encoded over the Y axis of unit groups in the layer
  bool		 add_noise;	// add some noise after computing value from layer
  Random	 force_noise;	// #CONDEDIT_ON_add_noise parameters for random added noise to forces

  virtual float	ReadForce(LeabraLayer* lay, LeabraNetwork* net, float pos, float vel);
  // #CAT_MotorForce read the force value from the layer, as a gaussian weighted average over units near the current position and velocity values
  virtual void	ClampForce(LeabraLayer* lay, LeabraNetwork* net, float force, float pos, float vel);
  // #CAT_MotorForce clamp the force value to the layer, as a gaussian weighted average over units near the current position and velocity values

  override void	Compute_BiasVal(LeabraLayer* lay, LeabraNetwork* net);
  
  bool  CheckConfig_Layer(LeabraLayer* lay, bool quiet=false);

  TA_SIMPLE_BASEFUNS(MotorForceLayerSpec);
protected:
  void	UpdateAfterEdit_impl();
private:
  void 	Initialize();
  void	Destroy()		{ };
};


//////////////////////////////////
// 	TwoD Value Layer	//
//////////////////////////////////

class LEABRA_API TwoDValSpec : public taOBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra specs for two-dimensional values
INHERITED(taOBase)
public:
  enum	RepType {
    GAUSSIAN,			// gaussian bump, with value = weighted average of tuned unit values
    LOCALIST			// each unit represents a distinct value; intermediate values represented by graded activity of neighbors; overall activity is weighted-average across all units
  };

  RepType	rep;		// #APPLY_IMMED type of representation of scalar value to use
  int		n_vals;		// number of values to represent in layer: layer geom.x must be >= 2 * n_vals because vals are represented in first row of layer!
  float		un_width;	// #CONDEDIT_ON_rep:GAUSSIAN sigma parameter of a gaussian specifying the tuning width of the coarse-coded units (in unit_range min-max units, unless norm_width is true, meaning use normalized 0-1 proportion of unit range)
  bool		norm_width;	// un_width is specified in normalized 0-1 proportion of unit range
  bool		clamp_pat;	// #DEF_false if true, environment provides full set of values to clamp over entire layer (instead of providing single scalar value to clamp on 1st unit, which then generates a corresponding distributed pattern)
  float		min_sum_act;	// #DEF_0.2 minimum total activity of all the units representing a value: when computing weighted average value, this is used as a minimum for the sum that you divide by
  float		mn_dst;		// #DEF_0.5 minimum distance factor for reading out multiple bumps: must be at least this times un_width far away from other bumps
  bool		clip_val;	// #DEF_true ensure that value remains within specified range

  float		x_min;		// #READ_ONLY #NO_SAVE #NO_INHERIT minimum unit value
  float		x_range;	// #READ_ONLY #NO_SAVE #NO_INHERIT range of unit values
  float		y_min;		// #READ_ONLY #NO_SAVE #NO_INHERIT minimum unit value
  float		y_range;	// #READ_ONLY #NO_SAVE #NO_INHERIT range of unit values
  float		x_val;		// #READ_ONLY #NO_SAVE #NO_INHERIT current val being represented (implementational, computed in InitVal())
  float		y_val;		// #READ_ONLY #NO_SAVE #NO_INHERIT current val being represented (implementational, computed in InitVal())
  float		x_incr;		// #READ_ONLY #NO_SAVE #NO_INHERIT increment per unit (implementational, computed in InitVal())
  float		y_incr;		// #READ_ONLY #NO_SAVE #NO_INHERIT increment per unit (implementational, computed in InitVal())
  int		x_size;		// #READ_ONLY #NO_SAVE #NO_INHERIT size of axis
  int		y_size;		// #READ_ONLY #NO_SAVE #NO_INHERIT size of axis
  float		un_width_x;	// #READ_ONLY #NO_SAVE #NO_INHERIT unit width, x axis (use for all computations -- can be normalized)
  float		un_width_y;	// #READ_ONLY #NO_SAVE #NO_INHERIT unit width, y axis (use for all computations -- can be normalized)

  virtual void	InitRange(float xmin, float xrng, float ymin, float yrng);
  // initialize range values, including normalized unit width values per axis
  virtual void	InitVal(float xval, float yval, int xsize, int ysize, float xmin, float xrng, float ymin, float yrng);
  // initialize implementational values for subsequently computing GetUnitAct to represent scalar val sval over unit group of ugp_size
  virtual float	GetUnitAct(int unit_idx);
  // get activation under current representation for unit at given index: MUST CALL InitVal first!
  virtual void	GetUnitVal(int unit_idx, float& x_cur, float& y_cur);
  // get target values associated with unit at given index: MUST CALL InitVal first!

  SIMPLE_COPY(TwoDValSpec);
  TA_BASEFUNS(TwoDValSpec);
private:
  void	Initialize();
  void 	Destroy()	{ };
};

class LEABRA_API TwoDValBias : public taOBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra initial bias for given activation value for scalar value units
INHERITED(taOBase)
public:
  enum UnitBias {		// bias on individual units
    NO_UN,			// no unit bias
    GC,				// bias value enters as a conductance in gc.h or gc.a
    BWT				// bias value enters as a bias.wt
  };

  enum WeightBias {		// bias on weights into units
    NO_WT,			// no weight bias
    WT				// input weights
  };

  UnitBias	un;		// #APPLY_IMMED bias on individual units
  float		un_gain;	// #CONDEDIT_OFF_un:NO_UN #DEF_1 gain multiplier (strength) of bias to apply for units.  WT = .03 as basic weight multiplier
  WeightBias	wt;		// #APPLY_IMMED bias on weights: always uses a val-shaped bias
  float		wt_gain;	// #CONDEDIT_OFF_wt:NO_WT #DEF_1 gain multiplier (strength) of bias to apply for weights (gain 1 = .03 wt value)
  float		x_val;		// X axis value location (center of gaussian bump)
  float		y_val;		// Y axis value location (center of gaussian bump)

  SIMPLE_COPY(TwoDValBias);
  TA_BASEFUNS(TwoDValBias);
private:
  void	Initialize();
  void 	Destroy()	{ };
};

class LEABRA_API TwoDValLayerSpec : public LeabraLayerSpec {
  // represents one or more two-d value(s) using a coarse-coded distributed code over units.  first row represents scalar value(s).  one val readout is weighted-average; multiple vals = max bumps over 3x3 local grid
INHERITED(LeabraLayerSpec)
public:
  TwoDValSpec	 twod;		// specifies how values are represented in terms of distributed patterns of activation across the layer
  MinMaxRange	 x_range;	// range of values represented across the X (horizontal) axis; for GAUSSIAN, add extra values above and below true useful range to prevent edge effects.
  MinMaxRange	 y_range;	// range of values represented across the Y (vertical) axis; for GAUSSIAN, add extra values above and below true useful range to prevent edge effects.
  TwoDValBias	 bias_val;	// specifies bias values
  MinMaxRange	 x_val_range;	// #READ_ONLY #NO_INHERIT actual range of values (scalar.min/max taking into account un_range)
  MinMaxRange	 y_val_range;	// #READ_ONLY #NO_INHERIT actual range of values (scalar.min/max taking into account un_range)

  virtual void	Settle_Init_Unit0(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_ScalarVal call Settle_Init_Unit on first units in each group (the value units) -- this is necessary b/c they are excluded from units_flat list and thus Compute_NetinScale, which is used for the global netin scale for the entire projection in Send_NetinDelta, and Init_TargFlags

  virtual void	ClampValue_ugp(Unit_Group* ugp, LeabraNetwork* net, float rescale=1.0f);
  // #CAT_TwoDVal clamp value in the first unit's ext field to the units in the group
  virtual void	ReadValue(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_TwoDVal read out current value represented by activations in layer
    virtual void ReadValue_ugp(LeabraLayer* lay, Unit_Group* ugp, LeabraNetwork* net);
    // #CAT_TwoDVal unit group version: read out current value represented by activations in layer
  virtual void	HardClampExt(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_TwoDVal hard clamp current ext values (on all units, after ClampValue called) to all the units (calls ResetAfterClamp)
    virtual void ResetAfterClamp(LeabraLayer* lay, LeabraNetwork* net);
    // #CAT_TwoDVal reset activation of first unit(s) after hard clamping

  virtual void	LabelUnits(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_TwoDVal label units in given layer with their underlying values
    virtual void LabelUnits_ugp(Unit_Group* ugp);
    // #CAT_TwoDVal label units with their underlying values
  virtual void	LabelUnitsNet(LeabraNetwork* net);
  // #BUTTON #CAT_TwoDVal label all layers in given network using this spec

  virtual void	Compute_BiasVal(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_TwoDVal initialize the bias value 
    virtual void Compute_WtBias_Val(Unit_Group* ugp, float x_val, float y_val);
    // #IGNORE
    virtual void Compute_UnBias_Val(Unit_Group* ugp, float x_val, float y_val);
    // #IGNORE

  override void BuildUnits_Threads(LeabraLayer* lay, LeabraNetwork* net);
    void BuildUnits_Threads_ugp(LeabraLayer* lay, Unit_Group* ug, LeabraNetwork* net);
  override void Init_Weights(LeabraLayer* lay, LeabraNetwork* net);
  override void	Settle_Init_Layer(LeabraLayer* lay, LeabraNetwork* net);
  override void	Compute_HardClamp(LeabraLayer* lay, LeabraNetwork* net);
  override void	Compute_CycleStats(LeabraLayer* lay, LeabraNetwork* net);

  // don't include first unit in any of these computations!
  override void Compute_AvgMaxVals_ugp(LeabraLayer* lay, Unit_Group* ug,
				       AvgMaxVals& vals, ta_memb_ptr mb_off);

  override float Compute_SSE(LeabraLayer* lay, LeabraNetwork* net, int& n_vals,
			     bool unit_avg = false, bool sqrt = false);
    virtual float Compute_SSE_ugp(Unit_Group* ugp, LeabraLayer* lay, int& n_vals);
    // #IGNORE
  override float Compute_NormErr(LeabraLayer* lay, LeabraNetwork* net);
    virtual float Compute_NormErr_ugp(LeabraLayer* lay, Unit_Group* ug, LeabraInhib* thr,
				       LeabraNetwork* net);
    // #IGNORE

  virtual void	ReConfig(Network* net, int n_units = -1);
  // #BUTTON reconfigure layer and associated specs for current scalar.rep type; if n_units > 0, changes number of units in layer to specified value

  void	HelpConfig();	// #BUTTON get help message for configuring this spec
  bool  CheckConfig_Layer(LeabraLayer* lay, bool quiet=false);

  TA_SIMPLE_BASEFUNS(TwoDValLayerSpec);
protected:
  void	UpdateAfterEdit_impl();
private:
  void 	Initialize();
  void	Destroy()		{ };
};

class LEABRA_API DecodeTwoDValLayerSpec : public TwoDValLayerSpec {
  // a two-d-value layer spec that copies its activations from one-to-one input prjns, to act as a decoder of another layer
INHERITED(TwoDValLayerSpec)
public:
  override void ReadValue_ugp(LeabraLayer* lay, Unit_Group* ugp, LeabraNetwork* net);
  override void	Compute_Inhib(LeabraLayer* lay, LeabraNetwork* net);

  // don't do any learning:
  override bool	Compute_SRAvg_Test(LeabraLayer* lay, LeabraNetwork* net)  { return false; }
  override bool	Compute_dWt_FirstPlus_Test(LeabraLayer* lay, LeabraNetwork* net) { return false; }
  override bool	Compute_dWt_SecondPlus_Test(LeabraLayer* lay, LeabraNetwork* net) { return false; }
  override bool	Compute_dWt_Nothing_Test(LeabraLayer* lay, LeabraNetwork* net) { return false; }

  TA_BASEFUNS_NOCOPY(DecodeTwoDValLayerSpec);
private:
  void 	Initialize();
  void	Destroy()		{ CutLinks(); }
};


//////////////////////////////////////
// 	V1RF Prjn Spec
//////////////////////////////////////

class LEABRA_API V1RFPrjnSpec : public ProjectionSpec {
  // V1 receptive field projection spec: does overlapping tiled receptive fields with Gabor and Blob filter weights - rf_spec.rf_width specifies the width of the receptive field, and rf_move specifies how much to move in input coordinates per each recv group
INHERITED(ProjectionSpec)
public:
  GaborV1SpecBase rf_spec;	// #SHOW_TREE receptive field specs
  FloatTwoDCoord rf_move;	// how much to move in input coordinates per each receiving layer group
  bool		wrap;		// if true, then connectivity has a wrap-around structure so it starts at -rf_move (wrapped to right/top) and goes +rf_move past the right/top edge (wrapped to left/bottom)
  float		dog_surr_mult;	// multiplier on surround weight values for DoG -- can be used to turn off surround entirely or to amplify it
  
  void 		Connect_impl(Projection* prjn);
  void		C_Init_Weights(Projection* prjn, RecvCons* cg, Unit* ru);

  virtual void	GraphFilter(DataTable* disp_data, int recv_unit_no);
  // #BUTTON #NULL_OK plot the filter gaussian into data table and generate a graph of a given unit number's gabor / blob filter
  virtual void	GridFilter(DataTable* disp_data);
  // #BUTTON #NULL_OK plot the filter gaussian into data table and generate a grid view of all the gabor or blob filters

  TA_SIMPLE_BASEFUNS(V1RFPrjnSpec);
protected:
  void	UpdateAfterEdit_impl();
private:
  void	Initialize();
  void 	Destroy()		{ };
};

class LEABRA_API SaliencyPrjnSpec : public ProjectionSpec {
  // Saliency projection spec from V1 layer: receiving layer must have a unit group for each feature, with each unit group having the V1 unit group geometry -- gets excitatory connection from feature corresponding to group index, and from all-but that feature in surrounding sending areas, producing a contrast enhancement effect.  Competition within group and across whole layer produces pop-out dynamics
INHERITED(ProjectionSpec)
public:
  int		convergence;	// how many sending groups to count as a single entity from the perspective of the saliency computation -- values larger than 1 make the saliency layer smaller than the sending layer
  bool		reciprocal;	// opposite direction connection: feedback to the features 
  bool		feat_only;	// only get excitatory input from feature only -- no DoG or anything else -- just pure feature aggregation (for use with kwta-based popout dynamics)
  int		feat_gps;	// number of feature groups contained within V1 unit group -- surround connections are only for within-group connections
  DoGFilterSpec	dog_wts;	// #CONDEDIT_OFF_feat_only Difference of Gaussians filter for setting the weights -- determines the width of the projection, etc -- where the net filter values are positive, receives excitation from the target feature, else from all-but the target feature
  float		wt_mult;	// #CONDEDIT_OFF_feat_only multiplier on weight values coming from the dog_wts 
  float		surr_mult;	// #CONDEDIT_OFF_feat_only multiplier on surround weight values -- DoG has very weak surround in general so it can be useful to increase that

  int		units_per_feat_gp; // #READ_ONLY #NO_SAVE #SHOW number of units per feature group (computed from sending layer)
  
  void 		Connect_impl(Projection* prjn);
  void		C_Init_Weights(Projection* prjn, RecvCons* cg, Unit* ru);

  virtual void 	Connect_feat_only(Projection* prjn);
  virtual void 	Connect_full_dog(Projection* prjn);

  virtual void	GraphFilter(DataTable* disp_data);
  // #BUTTON #NULL_OK plot the DoG center-surround weights into data table and generate a graph
  virtual void	GridFilter(DataTable* disp_data);
  // #BUTTON #NULL_OK plot the DoG center-surround weights into data table and generate a grid view

  TA_SIMPLE_BASEFUNS(SaliencyPrjnSpec);
private:
  void	Initialize();
  void 	Destroy()		{ };
};

class LEABRA_API GpAggregatePrjnSpec : public ProjectionSpec {
  // aggregates across a layer with unit groups into a receiving layer that has the same geometry as each of the unit groups -- each unit receives from the corresponding unit in each of the sending unit groups
INHERITED(ProjectionSpec)
public:
  
  void 		Connect_impl(Projection* prjn);

  TA_SIMPLE_BASEFUNS(GpAggregatePrjnSpec);
private:
  void	Initialize();
  void 	Destroy()		{ };
};

class LEABRA_API LeabraV1Layer : public LeabraLayer {
  // Specialized layer that implements competition both within unit groups and among features across the entire layer, where a feature is defined as a specific unit position within the unit groups (layer must have unit groups)
INHERITED(LeabraLayer)
public:
  LeabraUnit_Group		feat_gps;    // #LINK_GROUP #NO_SAVE #READ_ONLY #HIDDEN feature-level unit groups: the .gp subgroups are populated with sub unit groups, one per feature group, each containing links to the units
  LeabraInhib			feat_lay_thr; // #NO_SAVE #READ_ONLY #HIDDEN feature layer-level inhbition threshold stuff (use instead of layer's own thr values)

  override void	BuildUnits();
  override void	ResetSortBuf();

  TA_SIMPLE_BASEFUNS(LeabraV1Layer);
private:
  void	Initialize();
  void 	Destroy()		{ };
};

class LEABRA_API V1FeatInhibSpec : public taOBase {
  // ##INLINE ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra specifies inhibition parameters for V1 layer units based on feature-level inhibition
INHERITED(taOBase)
public:
  float		feat_gain;	// multiplier for feature-level inhibition
  float		dist_sigma;	// Gaussian std deviation (sigma) for weighting competitors based on distance, in normalized units relative to max x,y size dimension of the layer group geometry
  float		i_rat_thr;	// optimization threshold of unit i_thr relative to group's inhib threshold -- if less than this threshold, no distance costs are assessed

  void 	Defaults()	{ Initialize(); }
  TA_SIMPLE_BASEFUNS(V1FeatInhibSpec);
private:
  void	Initialize();
  void 	Destroy()	{ };
};

class LEABRA_API LeabraV1LayerSpec : public LeabraLayerSpec {
  // LayerSpec that implements competition both within unit groups and among features across the entire layer, where a feature is defined as a specific unit position within the unit groups (layer must have unit groups, and must be of LeabraV1Layer type!) -- feature inhibition is based on distance -- adds to layer-level gp_g based inhib for each unit in proportion to distance from active units
INHERITED(LeabraLayerSpec)
public:
  V1FeatInhibSpec	feat_inhib; // feature-level inhibition parameters

  virtual void	Compute_FeatGpActive(LeabraLayer* lay, LeabraUnit_Group* fug, LeabraNetwork* net);
  // compute active units in active_buf for given feature unit group

  override void	Compute_ApplyInhib(LeabraLayer* lay, LeabraNetwork* net);

  override bool CheckConfig_Layer(LeabraLayer* lay, bool quiet=false);
  
  TA_SIMPLE_BASEFUNS(LeabraV1LayerSpec);
private:
  void	Initialize();
  void 	Destroy()		{ };
};


////////////////////////////////////////////////
//	Special VE stuff for robotic arm sims

class LEABRA_API VELambdaMuscle : public taNBase {
  // a Lambda-model (Gribble et al, 1998) muscle, used in context of a VE arm joint -- as a fairly accurate simplification, we assume a linear relationship between joint angle and muscle length, and a constant moment arm (accurate for extensors)
INHERITED(taNBase)
public:
  enum MuscleType {
    FLEXOR,			// pulls a joint closed -- toward hi stop -- pectoralis for shoulder, biceps long head for elbow
    EXTENSOR,			// pulls a joint open -- toward lo stop -- deltoid for shoulder, triceps lateral head
  };

  //////////////////////////////
  //	Control signals

  float		lambda_norm;	// normalized (0-1) desired length of the muscle: this is the only control signal
  float		lambda;		// #READ_ONLY #SHOW desired length of the muscle in muscle-length units
  float		co_contract_pct; // normalized (0-1) percent of co-contraction to apply -- shortens lambda by a fixed proportion of the co_contract_len value (below)
  float		extra_force;	// a constant additional force value to apply to the muscle -- can be used for co-contraction or additional force commands beyond the equilibrium point specification

  //////////////////////////////
  //	Parameters

  // statics
  MuscleType	muscle_type;		// what type of muscle is it -- controls relationship between angle and muscle length
  float		moment_arm;		// (m, .02 for elbow, .04 for shoulder) moment arm length for applying force (assumed fixed) -- positive for flexors and negative for extensors
  MinMaxRange	len_range;		// (m) effective length range of the muscle over which it can contract and expand -- this corresponds to the lo-hi stop range of angles of the joint -- for flexors, min = hi stop, max = lo stop, for extensors, min = lo stop, max = hi stop.  for elbow bicep/tricep, min=0.28 max=0.37, for shoulder pectoralis/deltoid min=0.05 max=0.15 (est)
  float		co_contract_len;	// maximum length available for co-contraction -- must be < .95 * len_range.min (i.e., if muscle is at its shortest length for the joint stop, this is how much extra shorter it can possibly command to be from there, and still be a positive number)
  float		rest_len;		// #READ_ONLY #SHOW resting length, computed during init from resting angle

  // dynamics
  float		step_size;		// #READ_ONLY (s) set in init -- world step size in seconds
  float		vel_damp;		// #DEF_0.06 (mu, s) velocity damping
  float		reflex_delay;		// #DEF_0.025 (d, s) reflex delay -- how slowly reflex control reacts to changes in muscle length and velocity
  int		reflex_delay_idx;	// #READ_ONLY (d, steps) reflex delay, computed in units of step size
  float		m_rec_grad;		// #DEF_11.2 (c, mm^-1) muscle MN recruitment gradient
  float		m_mag;			// (rho, m^2) muscle force-generating magnitude (multiplier), related to cross-section of muscle size: biceps short head 2.1; biceps long head 11; deltoid 14.9; pectoralis 14.9; triceps lateral head 12.1; triceps long head 6.7;
  float		ca_dt;			// #DEF_0.015 (tau, s) calcium kinetics time constant -- note only using a first-order exponential time decay filter
  float		ca_dt_cmp;		// #READ_ONLY (tau, 1/steps) calcium kinetics time constant actually used in cmputations -- note only using a first-order exponential time decay filter -- this value is in time steps, not time per se
  float		fv1;			// #DEF_0.82 (f1, s/m) muscle force velocity dependence factor: constant offset
  float		fv2;			// #DEF_0.5 (f2, s/m) muscle force velocity dependence factor: atan multiplier
  float		fv3;			// #DEF_0.43 (f3, s/m) muscle force velocity dependence factor: atan constant offset
  float		fv4;			// #DEF_0.58 (f4, s/m) muscle force velocity dependence factor: velocity multiplier
  float		passive_k;		// (k, N/m) passive stiffness: biceps short head 36.5; biceps long head 190.9; deltoid 258.5; pectoralis 258.5; triceps lateral head 209.9; triceps long head 116.3;

  //////////////////////////////
  //	State values
  float		len;		// #READ_ONLY #SHOW (l, cm) current muscle length
  float		dlen;		// #READ_ONLY #SHOW (l-dot, cm/s) current muscle length velocity: rate of change of length
  float		act;		// #READ_ONLY #SHOW (A, N?) current muscle activation value
  float		m_act_force;	// #READ_ONLY #SHOW (~M, N) current muscle force from activation
  float		m_force;	// #READ_ONLY #SHOW (M, N) current muscle force after low-pass filtering by ca_dt
  float		force;		// #READ_ONLY #SHOW (N) final force value 
  float		torque;		// #READ_ONLY #SHOW (N) final torque (force * moment_arm)

  float_CircBuffer len_buf;	// #READ_ONLY #NO_SAVE current muscle length buffer (for delays)
  float_CircBuffer dlen_buf;	// #READ_ONLY #NO_SAVE current muscle length velocity buffer (for delays)

  VEBodyRef	muscle_obj;	// #SCOPE_VEObject if non-null, update this object with the new length information as the muscle changes (must be cylinder or capsule obj shape)

  virtual float	LenFmAngle(float norm_angle);
  // #CAT_Muscle compute muscle length from *normalized* joint angle (0 = lo stop, 1 = hi stop) -- uses a simple linear projection onto len_range which is fairly accurate

  virtual void	Init(float step_sz, float rest_norm_angle, float init_norm_angle,
		     float co_contract);
  // #CAT_Muscle initialize all parameters back to initial values, compute params, and set arm at initial angle (clear buffers, etc)

  virtual void	Compute_Force(float cur_norm_angle);
  // #CAT_Muscle compute force based on current parameters with given normalized angle (0 = lo stop, 1 = hi stop) (given by ODE presumably)

  virtual void	Compute_Lambda();
  // #CAT_Muscle compute lambda value from lambda_norm and co_contract_pct

  virtual void	SetTargAngle(float targ_norm_angle, float co_contract_pct);
  // #BUTTON #CAT_Muscle set target *normalized* (0 = lo stop, 1 = hi stop) angle for the joint, which computes the lambdas (target lengths) for the individual muscles -- the co_contract_pct determines what percentage of co-contraction (stiffnes) to apply, where the lambdas are shorter than they should otherwise be by the given amount, such that both will pull from opposite directions to cause the muscle to stay put

  TA_SIMPLE_BASEFUNS(VELambdaMuscle);
protected:
  void 	UpdateAfterEdit_impl();

private:
  void	Initialize();
  void 	Destroy()	{ };
};

// note on lengths and masses for typical arm: upper = .34m, 2.1kg; lower = .46m, 1.65kg

class LEABRA_API VELambdaArmJoint : public VEJoint {
  // a Lambda-model (Gribble et al, 1998) arm joint -- updates forces at every time step, in GetVAlsFromODE, applies them using SetForce
INHERITED(VEJoint)
public:
  VELambdaMuscle	flexor; // #SHOW_TREE flexor muscle
  VELambdaMuscle	extensor; // #SHOW_TREE extensor muscle
  float			targ_norm_angle; // #READ_ONLY #SHOW current target normalized angle
  float			targ_angle; // #READ_ONLY #SHOW current target raw angle
  float			cur_norm_angle; // #READ_ONLY #SHOW current actual normalized angle (cur raw angle is pos)
  float			co_contract_pct; // #READ_ONLY #SHOW current co-contraction pct

  virtual void	SetTargAngle(float trg_angle, float co_contract);
  // #BUTTON #CAT_Force set target angle for the joint, which computes the lambdas (target lengths) for the individual muscles -- the co_contract_pct determines what percentage of co-contraction (stiffnes) to apply, where the lambdas are shorter than they should otherwise be by the given amount, such that both will pull from opposite directions to cause the muscle to stay put (at least around .2 is needed, with .5 being better, to prevent big oscillations)

  virtual void	SetTargNormAngle(float trg_norm_angle, float co_contract);
  // #BUTTON #CAT_Force set normalized target angle (0 = lo stop, 1 = hi stop) for the joint, which computes the lambdas (target lengths) for the individual muscles -- the co_contract_pct determines what percentage of co-contraction (stiffnes) to apply, where the lambdas are shorter than they should otherwise be by the given amount, such that both will pull from opposite directions to cause the muscle to stay put (at least around .2 is needed, with .5 being better, to prevent big oscillations)

  override void	SetValsToODE();
  override void	GetValsFmODE(bool updt_disp = false);

  TA_SIMPLE_BASEFUNS(VELambdaArmJoint);
private:
  void	Initialize();
  void 	Destroy()	{ };
};


#endif // leabra_extra_h

