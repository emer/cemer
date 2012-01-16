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

// extra specialized classes for variations on the Leabra algorithm

class LEABRA_API MarkerConSpec : public LeabraConSpec {
  // connection spec that marks special projections: doesn't send netin or adapt weights
INHERITED(LeabraConSpec)
public:
  // don't send regular net inputs or learn!
  override void Send_NetinDelta(LeabraSendCons*, LeabraNetwork* net, int thread_no, 
				float su_act_delta_eff) { };
  override void Compute_dWt(RecvCons*, Unit*) { };
  override void Compute_dWt_LeabraCHL(LeabraSendCons*, LeabraUnit*) { };
  override void Compute_dWt_CtLeabraCAL(LeabraSendCons*, LeabraUnit*) { };
  override void Compute_dWt_CtLeabraXCAL(LeabraSendCons*, LeabraUnit*) { };
  override void Compute_SRAvg(LeabraSendCons*, LeabraUnit*, bool do_s) { };
  override void Trial_Init_SRAvg(LeabraSendCons*, LeabraUnit*) { };
  override void	Compute_Weights(RecvCons*, Unit*) { };
  override void	Compute_Weights_LeabraCHL(LeabraSendCons*, LeabraUnit*) { };
  override void	Compute_Weights_CtLeabraCAL(LeabraSendCons*, LeabraUnit*) { };
  override void	Compute_Weights_CtLeabraXCAL(LeabraSendCons*, LeabraUnit*) { };

  override bool	DMem_AlwaysLocal() { return true; }
  // these connections always need to be there on all nodes..

  TA_BASEFUNS_NOCOPY(MarkerConSpec);
protected:
  SPEC_DEFAULTS;
private:
  void 	Initialize();
  void	Destroy()		{ };
  void	Defaults_init();
};


//////////////////////////////////////////
// 	Context Layer for Sequential	//
//////////////////////////////////////////

class LEABRA_API CtxtUpdateSpec : public SpecMemberBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra context updating specifications
INHERITED(SpecMemberBase)
public:
  float		fm_hid;		// from hidden (inputs to context layer)
  float		fm_prv;		// from previous context layer values (maintenance)
  float		to_out;		// outputs from context layer

  SIMPLE_COPY(CtxtUpdateSpec);
  TA_BASEFUNS(CtxtUpdateSpec);
protected:
  SPEC_DEFAULTS;
private:
  void	Initialize();
  void 	Destroy()	{ };
  void	Defaults_init() { }; 	// note: does NOT do any init -- these vals are not really subject to defaults in the usual way, so don't mess with them
};

class LEABRA_API CtxtNSpec : public SpecMemberBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra context counting specifications
INHERITED(SpecMemberBase)
public:
  int		n_trials;	// #MIN_1 update every n trials
  int		n_offs;		// #MIN_0 offset of n during count, ex using 2 lays with 0 and N/2 gives half alternating offset

  TA_SIMPLE_BASEFUNS(CtxtNSpec);
protected:
  SPEC_DEFAULTS;
private:
  void	Initialize() 	{ n_trials = 2; n_offs = 0; }
  void 	Destroy()	{ };
  void	Defaults_init() { }; 	// note: does NOT do any init -- these vals are not really subject to defaults in the usual way, so don't mess with them
};

class LEABRA_API LeabraContextLayerSpec : public LeabraLayerSpec {
  // context layer that copies from its recv projection (like an input layer)
INHERITED(LeabraLayerSpec)
public:
  enum UpdateCriteria {
    UC_TRIAL = 0, // updates every trial (traditional "ContextLayer" behavior)
    UC_MANUAL, // manually updated via TriggerUpdate
    UC_N_TRIAL, // updates every n trials
  };
  
  UpdateCriteria update_criteria; // #DEF_UC_TRIAL #NO_SAVE_EMPTY how to determine when to copy the sending layer
  CtxtUpdateSpec updt;		// ctxt updating constants: from hidden, from previous values (hysteresis), outputs from context (n/a on simple gate layer)
  CtxtNSpec	n_spec; // #CONDSHOW_ON_update_criteria:UC_N_TRIAL trials per update and optional offset for multi
  
  override void	Compute_HardClamp(LeabraLayer* lay, LeabraNetwork* net);
  // clamp from act_p values of sending layer
  override bool  CheckConfig_Layer(Layer* lay, bool quiet=false);

  // don't do any learning:
  override bool	Compute_SRAvg_Test(LeabraLayer* lay, LeabraNetwork* net)  { return false; }
  override bool	Compute_dWt_FirstPlus_Test(LeabraLayer* lay, LeabraNetwork* net) { return false; }
  override bool	Compute_dWt_SecondPlus_Test(LeabraLayer* lay, LeabraNetwork* net) { return false; }
  override bool	Compute_dWt_Nothing_Test(LeabraLayer* lay, LeabraNetwork* net) { return false; }

  void TriggerUpdate(LeabraLayer* lay); // manually trigger an update of the context layer -- generally called at end of a Trial -- can always be called even if not on MANUAL
  
  DumpQueryResult Dump_QuerySaveMember(MemberDef* md);
  TA_SIMPLE_BASEFUNS(LeabraContextLayerSpec);
  
protected:
  SPEC_DEFAULTS;
  STATIC_CONST String do_update_key;
  virtual void Compute_Context(LeabraLayer* lay, LeabraUnit* u, LeabraNetwork* net);
  // get context source value for given context unit

private:
  void 	Initialize();
  void	Destroy()		{ };
  void	Defaults_init();
};


class LEABRA_API LeabraMultCopyLayerSpec : public LeabraLayerSpec {
  // layer that copies activations from one layer and multiplies them by values from another -- i.e., multiplicative gating -- must recv from 2 prjns (any more ignored) -- first is copy activation, second is multiplication activation
INHERITED(LeabraLayerSpec)
public:
  bool		one_minus;	// if true, use 1-mult activation as the multiplier -- this is useful with mutually exclusive options in the multipliers, where you hook each up to the *other* alternative, such that this other alternative inhibits this option
  float		mult_gain;	// multiplier gain -- multiply the mult act value by this gain, with a max overall resulting net mult value of 1

  virtual void	Compute_MultCopyAct(LeabraLayer* lay, LeabraNetwork* net);
  // compute mult copy activations -- replaces std act fun -- called in Compute_CycleStats -- just overwrites whatever the regular funs compute

  override void	Compute_Inhib(LeabraLayer* lay, LeabraNetwork* net) { };
  override void Compute_CycleStats(LeabraLayer* lay, LeabraNetwork* net);

  // don't do any learning:
  override bool	Compute_SRAvg_Test(LeabraLayer* lay, LeabraNetwork* net)  { return false; }
  override bool	Compute_dWt_FirstPlus_Test(LeabraLayer* lay, LeabraNetwork* net) { return false; }
  override bool	Compute_dWt_SecondPlus_Test(LeabraLayer* lay, LeabraNetwork* net) { return false; }
  override bool	Compute_dWt_Nothing_Test(LeabraLayer* lay, LeabraNetwork* net) { return false; }

  bool  CheckConfig_Layer(Layer* lay, bool quiet=false);

  TA_SIMPLE_BASEFUNS(LeabraMultCopyLayerSpec);
protected:
  SPEC_DEFAULTS;
private:
  void 	Initialize();
  void	Destroy()		{ };
  void	Defaults_init()		{ };
};

//////////////////////////////////////////
// 	Misc Special Objects		//
//////////////////////////////////////////

class LEABRA_API LeabraLinUnitSpec : public LeabraUnitSpec {
  // a pure linear unit (suitable for an AC unit spec unit)
INHERITED(LeabraUnitSpec)
public:
  void 	Compute_ActFmVm(LeabraUnit* u, LeabraNetwork* net);
  
  TA_BASEFUNS(LeabraLinUnitSpec);
protected:
  SPEC_DEFAULTS;
private:
  void 	Initialize();
  void	Destroy()		{ };
  void	Defaults_init() 	{ }
};

// note: the following is not compatible with the leabra unit equations

// class LEABRA_API IzhikevichSpec : public taOBase {
//   // ##INLINE ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra Izhikevich simplified Hodgkin-Huxley type spiking dynamics -- uses SPIKE mode and adds special v_m dynamics -- default parameters are for normalized V_m range (0-1) which corresponds to -90..50 in neural mV terms
// INHERITED(taOBase)
// public:
//   bool		on;		// Use this activation function, in conjunction with SPIKE mode
//   float		rec_dt;		// #CONDSHOW_ON_on #MIN_0 #MAX_1 time constant of the recovery variable (a in original paper) -- if the vm time step is considered to be 1 msec, then values of .02 to .1 are typical, with .02 being a regular spiking neuron, and .1 is fast spiking
//   float		rec_v_gain;	// #CONDSHOW_ON_on #MIN_0 #MAX_1 gain on the membrane potential driving the recovery variable (b in original paper) -- values between .2 and .25 are typical, with .2 being a regular spiking neuron
//   float		rec_spike;	// #CONDSHOW_ON_on value to add to the recovery variable after spiking (d in original paper) -- to convert from mV units to normalized, divide by 140 -- range is .057 (8 in mV) for regular spiking to .0004 (.05 in mV) for thalamocortical -- fast spiking is 0.0143 (2 in mV) -- note that v_m_r in spike field is the reset var for vm (set to .15 for std regular spiking)
//   float		v_m_thr;	// #CONDSHOW_ON_on #DEF_0.86 threshold membrane potential for initiating a spike -- note that this is much higher than act.thr, which is not used under this model
//   float		rec_c;		// #READ_ONLY #NO_SAVE constant term in the recovery equation = -.642857 * rec_v_gain

//   float	Compute_dVm(float vm, float rec, float I_net) {
//     // use two .5 steps to achieve better numerical stability
//     float dvm = .5f * (5.6f * vm * vm - 2.2f * vm + .1f - rec + I_net);
//     vm += dvm;
//     dvm += .5f * (5.6f * vm * vm - 2.2f * vm + .1f - rec + I_net);
//     return dvm;
//   }
//   // compute the change in Vm given vm, rec and I_net inputs -- constants are for the normalized form of the equation
//   float	Compute_dRec(float vm, float rec) {
//     return rec_dt * (rec_v_gain * vm + rec_c - rec);
//   }
//   // compute the change in rec given vm and rec inputs -- constants are for the normalized form of the equation
//   float	Init_Rec(float vm) {
//     return rec_v_gain * vm + rec_c;
//   }
//   // initial rec value based on initial vm value (equilibrium point)

//   TA_SIMPLE_BASEFUNS(IzhikevichSpec);
// protected:
//   void	UpdateAfterEdit_impl();
// private:
//   void	Initialize();
//   void	Destroy()	{ };
// };


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
protected:
  SPEC_DEFAULTS;
private:
  void 	Initialize();
  void	Destroy()		{ };
  void	Defaults_init() { };	
};


//////////////////////////////////////////////////////////////////
// 	Variations of XCAL

class LEABRA_API XCalSRAvgConSpec : public LeabraConSpec {
  // send-recv average at the connection level learning in XCal
INHERITED(LeabraConSpec)
public:

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

  TA_SIMPLE_BASEFUNS(XCalSRAvgConSpec);
protected:
  SPEC_DEFAULTS;
//   void 	UpdateAfterEdit_impl();

private:
  void 	Initialize();
  void	Destroy()		{ };
  void	Defaults_init() 	{ };
};


class LEABRA_API XCalHebbConSpec : public LeabraConSpec {
  // xcal version of hebbian learning
INHERITED(LeabraConSpec)
public:
  float		hebb_mix;	// #DEF_0 #MIN_0 #MAX_1 amount of "pure" hebbian learning operating within the thr_l_mix BCM-like component -- actually the amount of fixed sending layer expected activity level to mulitply the recv long-term average activation by (remainder is multiplied by the current sending medium time scale activation, as is done in BCM) -- this is not useful for most cases, but is included for generality
  float		hebb_mix_c;	// #READ_ONLY 1 - hebb_mix -- amount of sending medium time scale activation to mulitply ru avg_l by
  float		su_act_min;	// #DEF_0 #MIN_0 #MAX_1 NOTE: this is only useful for hebb_mixminimum effective activation for sending units as entering into learning rule -- because the xcal curve returns to 0 when the S*R coproduct is 0, an inactive sending unit will never experience any weight change -- this is counter to the hebbian form of learning, where an active recv unit will decrease weights from inactive senders -- ensuring a minimal amount of activation avoids this issue, and reflects the low background rate of neural and synaptic activity that actually exists in the brain

  inline void Compute_dWt_CtLeabraXCAL(LeabraSendCons* cg, LeabraUnit* su) {
    LeabraLayer* slay = (LeabraLayer*)cg->prjn->from.ptr();
    float su_avg_s = MAX(su->avg_s, su_act_min);
    float su_avg_m = MAX(su->avg_m, su_act_min);
    float su_act_mult = xcal.thr_l_mix * (hebb_mix * slay->kwta.pct + hebb_mix_c * su_avg_m);

    for(int i=0; i<cg->size; i++) {
      LeabraUnit* ru = (LeabraUnit*)cg->Un(i);
      C_Compute_dWt_CtLeabraXCAL_trial((LeabraCon*)cg->OwnCn(i), ru, su_avg_s, su_avg_m,
				       su_act_mult);
    }
  }

  TA_SIMPLE_BASEFUNS(XCalHebbConSpec);
protected:
  SPEC_DEFAULTS;
  void	UpdateAfterEdit_impl();
private:
  void 	Initialize();
  void	Destroy()		{ };
  void	Defaults_init() 	{ };
};

// todo: this should be supported with a special unit and unitspec where needed -- completely 
// untested at this point..
//
// class LEABRA_API XCalMlTraceConSpec : public LeabraConSpec {
//   // xcal with ml_mix > 0 -- provides a trace of recv activation in learning
// INHERITED(LeabraConSpec)
// public:
//   float		ml_mix;		// #DEF_0 #MIN_0 how much the medium-to-long time scale average activations contribute to synaptic activation -- useful for capturing sequential dependencies between events, when these are present in the simulation, but not appropriate for random event sequences
//   float		sm_mix;		// #READ_ONLY #DEF_1 #MIN_0 complement of ml_mix = 1-ml_mix -- how much the short & medium time scale average activations contribute to synaptic activation

//   inline void C_Compute_dWt_CtLeabraXCAL_trial(LeabraCon* cn, LeabraUnit* ru, LeabraUnit* su,
// 			       float su_avg_s, float su_avg_m, float su_act_mult) {
//     float srs = ru->avg_s * su_avg_s;
//     float srm = ru->avg_m * su_avg_m;
//     float srml = sm_mix * srm + ml_mix * (ru->avg_ml * su->avg_ml);
//     float sm_mix = xcal.s_mix * srs + xcal.m_mix * srml;
//     float effthr = xcal.thr_m_mix * srm + su_act_mult * ru->avg_l;
//     cn->dwt += cur_lrate * xcal.dWtFun(sm_mix, effthr);
//   }

//   inline void Compute_dWt_CtLeabraXCAL(LeabraSendCons* cg, LeabraUnit* su) {
//     float su_avg_s = su->avg_s;
//     float su_avg_m = su->avg_m;
//     float su_act_mult = xcal.thr_l_mix * su_avg_m;

//     for(int i=0; i<cg->size; i++) {
//       LeabraUnit* ru = (LeabraUnit*)cg->Un(i);
//       C_Compute_dWt_CtLeabraXCAL_trial((LeabraCon*)cg->OwnCn(i), ru, su, su_avg_s, su_avg_m,
// 				       su_act_mult);
//     }
//   }

//   TA_SIMPLE_BASEFUNS(XCalMlTraceConSpec);
// protected:
//   SPEC_DEFAULTS;
//   void	UpdateAfterEdit_impl();
// private:
//   void 	Initialize();
//   void	Destroy()	{ };
//   void	Defaults_init() { };
// };


//////////////////////////////////////////
// 	Synaptic Depression: Trial Level

class LEABRA_API TrialSynDepCon : public LeabraCon {
  // synaptic depression connection at the trial level (as opposed to cycle level)
INHERITED(LeabraCon)
public:
  float		effwt;		// #NO_SAVE effective weight value (can be depressed) -- used for sending ativation

  TrialSynDepCon() { effwt = 0.0f; }
};

class LEABRA_API TrialSynDepSpec : public SpecMemberBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra specs for synaptic depression
INHERITED(SpecMemberBase)
public:
  float		rec;		// #DEF_1 rate of recovery from depression
  float		depl;		// #DEF_1.1 rate of depletion of synaptic efficacy as a function of sender-receiver activations

  SIMPLE_COPY(TrialSynDepSpec);
  TA_BASEFUNS(TrialSynDepSpec);
protected:
  SPEC_DEFAULTS;
private:
  void	Initialize();
  void 	Destroy()	{ };
  void	Defaults_init() { };
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
  virtual void Depress_Wt(LeabraSendCons* cg, LeabraUnit* su) {
    CON_GROUP_LOOP(cg, C_Depress_Wt((TrialSynDepCon*)cg->OwnCn(i), (LeabraUnit*)cg->Un(i), su));
  }

  override void Compute_dWt_LeabraCHL(LeabraSendCons* cg, LeabraUnit* su) {
    inherited::Compute_dWt_LeabraCHL(cg, su);
    Depress_Wt(cg, su);
  }
  override void Compute_dWt_CtLeabraXCAL(LeabraSendCons* cg, LeabraUnit* su) {
    inherited::Compute_dWt_CtLeabraXCAL(cg, su);
    Depress_Wt(cg, su);
  }
  override void Compute_dWt_CtLeabraCAL(LeabraSendCons* cg, LeabraUnit* su) {
    inherited::Compute_dWt_CtLeabraCAL(cg, su);
    Depress_Wt(cg, su);
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

  override void Send_NetinDelta(LeabraSendCons* cg, LeabraNetwork* net,
			      int thread_no, float su_act_delta) {
    float su_act_delta_eff = cg->scale_eff * su_act_delta;
    if(inhib && net->inhib_cons_used) { // both must agree that inhib is ok
      if(thread_no < 0) {
	CON_GROUP_LOOP(cg, C_Send_InhibDelta_NoThrd((TrialSynDepCon*)cg->OwnCn(i),
						   (LeabraUnit*)cg->Un(i), su_act_delta_eff));
      }
      else {
	float* send_inhib_vec = net->send_inhib_tmp.el
	  + net->send_inhib_tmp.FastElIndex(0, thread_no);
	CON_GROUP_LOOP(cg, C_Send_InhibDelta_Thrd((TrialSynDepCon*)cg->OwnCn(i),
						 send_inhib_vec, (LeabraUnit*)cg->Un(i),
						 su_act_delta_eff));
      }
    }
    else {
      if(thread_no < 0) {
	CON_GROUP_LOOP(cg, C_Send_NetinDelta_NoThrd((TrialSynDepCon*)cg->OwnCn(i),
						   (LeabraUnit*)cg->Un(i), su_act_delta_eff));
      }
      else {
	float* send_netin_vec = net->send_netin_tmp.el
	  + net->send_netin_tmp.FastElIndex(0, thread_no);
	CON_GROUP_LOOP(cg, C_Send_NetinDelta_Thrd((TrialSynDepCon*)cg->OwnCn(i), send_netin_vec,
						 (LeabraUnit*)cg->Un(i), su_act_delta_eff));
      }
    }
  }

  float C_Compute_Netin(TrialSynDepCon* cn, LeabraUnit*, LeabraUnit* su) {
    return cn->effwt * su->act_eq;
  }
  float Compute_Netin(RecvCons* cg, Unit* ru) {
    float rval=0.0f;
    CON_GROUP_LOOP(cg, rval += C_Compute_Netin((TrialSynDepCon*)cg->PtrCn(i), (LeabraUnit*)ru,
					       (LeabraUnit*)cg->Un(i)));
    return ((LeabraRecvCons*)cg)->scale_eff * rval;
  }


  TA_SIMPLE_BASEFUNS(TrialSynDepConSpec);
protected:
  SPEC_DEFAULTS;
  void 	UpdateAfterEdit_impl();

private:
  void 	Initialize();
  void	Destroy()		{ };
  void	Defaults_init() 	{ };
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

class LEABRA_API CycleSynDepSpec : public SpecMemberBase {
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

class LEABRA_API CycleSynDepConSpec : public LeabraConSpec {
  // synaptic depression connection at the cycle level (as opposed to the trial level) -- this is the simpler version -- Ca_i based version below
INHERITED(LeabraConSpec)
public:
  CycleSynDepSpec	syn_dep;	// synaptic depression specifications

  inline void C_Compute_CycSynDep(CycleSynDepCon* cn, LeabraUnit* ru, LeabraUnit* su) {
    syn_dep.Depress(cn->effwt, cn->wt, ru->act_eq, su->act_eq);
  }
  inline override void Compute_CycSynDep(LeabraSendCons* cg, LeabraUnit* su) {
    CON_GROUP_LOOP(cg, C_Compute_CycSynDep((CycleSynDepCon*)cg->OwnCn(i),
					   (LeabraUnit*)cg->Un(i), su));
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

  override void Send_NetinDelta(LeabraSendCons* cg, LeabraNetwork* net,
			      int thread_no, float su_act_delta) {
    float su_act_delta_eff = cg->scale_eff * su_act_delta;
    if(inhib && net->inhib_cons_used) { // both must agree that inhib is ok
      if(thread_no < 0) {
	CON_GROUP_LOOP(cg, C_Send_InhibDelta_NoThrd((CycleSynDepCon*)cg->OwnCn(i),
						   (LeabraUnit*)cg->Un(i), su_act_delta_eff));
      }
      else {
	float* send_inhib_vec = net->send_inhib_tmp.el
	  + net->send_inhib_tmp.FastElIndex(0, thread_no);
	CON_GROUP_LOOP(cg, C_Send_InhibDelta_Thrd((CycleSynDepCon*)cg->OwnCn(i),
						 send_inhib_vec, (LeabraUnit*)cg->Un(i),
						 su_act_delta_eff));
      }
    }
    else {
      if(thread_no < 0) {
	CON_GROUP_LOOP(cg, C_Send_NetinDelta_NoThrd((CycleSynDepCon*)cg->OwnCn(i),
						   (LeabraUnit*)cg->Un(i), su_act_delta_eff));
      }
      else {
	float* send_netin_vec = net->send_netin_tmp.el
	  + net->send_netin_tmp.FastElIndex(0, thread_no);
	CON_GROUP_LOOP(cg, C_Send_NetinDelta_Thrd((CycleSynDepCon*)cg->OwnCn(i), send_netin_vec,
						 (LeabraUnit*)cg->Un(i), su_act_delta_eff));
      }
    }
  }

  float C_Compute_Netin(CycleSynDepCon* cn, LeabraUnit*, LeabraUnit* su) {
    return cn->effwt * su->act_eq;
  }
  float Compute_Netin(RecvCons* cg, Unit* ru) {
    float rval=0.0f;
    CON_GROUP_LOOP(cg, rval += C_Compute_Netin((CycleSynDepCon*)cg->PtrCn(i), (LeabraUnit*)ru,
					       (LeabraUnit*)cg->Un(i)));
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

class LEABRA_API CaiSynDepSpec : public SpecMemberBase {
  // ##INLINE ##NO_TOKENS ##CAT_Leabra specs for synaptic depression based in synaptic integration of calcium
INHERITED(SpecMemberBase)
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
  SPEC_DEFAULTS;
  void UpdateAfterEdit_impl();

private:
  void	Initialize();
  void 	Destroy()	{ };
  void	Defaults_init() { Initialize(); }
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
  inline void Compute_Cai(LeabraSendCons* cg, LeabraUnit* su) {
    CON_GROUP_LOOP(cg, C_Compute_Cai((CaiSynDepCon*)cg->OwnCn(i), (LeabraUnit*)cg->Un(i), su));
  }

  // connection-level synaptic depression: syn dep direct
  inline void C_Compute_CycSynDep(CaiSynDepCon* cn, LeabraUnit* ru, LeabraUnit* su) {
    cn->effwt = cn->wt * ca_dep.SynDep(cn->cai);
  }
  // connection-level synaptic depression: ca mediated
  inline override void Compute_CycSynDep(LeabraSendCons* cg, LeabraUnit* su) {
    Compute_Cai(cg, su);
    CON_GROUP_LOOP(cg, C_Compute_CycSynDep((CaiSynDepCon*)cg->OwnCn(i),
					   (LeabraUnit*)cg->Un(i), su));
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

  override void Send_NetinDelta(LeabraSendCons* cg, LeabraNetwork* net,
			      int thread_no, float su_act_delta) {
    float su_act_delta_eff = cg->scale_eff * su_act_delta;
    if(inhib && net->inhib_cons_used) { // both must agree that inhib is ok
      if(thread_no < 0) {
	CON_GROUP_LOOP(cg, C_Send_InhibDelta_NoThrd((CaiSynDepCon*)cg->OwnCn(i),
						   (LeabraUnit*)cg->Un(i), su_act_delta_eff));
      }
      else {
	float* send_inhib_vec = net->send_inhib_tmp.el
	  + net->send_inhib_tmp.FastElIndex(0, thread_no);
	CON_GROUP_LOOP(cg, C_Send_InhibDelta_Thrd((CaiSynDepCon*)cg->OwnCn(i),
						 send_inhib_vec, (LeabraUnit*)cg->Un(i),
						 su_act_delta_eff));
      }
    }
    else {
      if(thread_no < 0) {
	CON_GROUP_LOOP(cg, C_Send_NetinDelta_NoThrd((CaiSynDepCon*)cg->OwnCn(i),
						   (LeabraUnit*)cg->Un(i), su_act_delta_eff));
      }
      else {
	float* send_netin_vec = net->send_netin_tmp.el
	  + net->send_netin_tmp.FastElIndex(0, thread_no);
	CON_GROUP_LOOP(cg, C_Send_NetinDelta_Thrd((CaiSynDepCon*)cg->OwnCn(i), send_netin_vec,
						 (LeabraUnit*)cg->Un(i), su_act_delta_eff));
      }
    }
  }

  float C_Compute_Netin(CaiSynDepCon* cn, LeabraUnit*, LeabraUnit* su) {
    return cn->effwt * su->act_eq;
  }
  float Compute_Netin(RecvCons* cg, Unit* ru) {
    float rval=0.0f;
    CON_GROUP_LOOP(cg, rval += C_Compute_Netin((CaiSynDepCon*)cg->PtrCn(i), 
					       (LeabraUnit*)ru, (LeabraUnit*)cg->Un(i)));
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


////////////////////////////////////////////////////////////////
//      Synaptic Depression: Cycle Level, Ca-Based, with SRAvg

// todo: could inherit from CaiSynDepCon/Spec and probably save a lot of code, but sravg guys might be more difficult -- try that later

class LEABRA_API SRAvgCaiSynDepCon : public LeabraSRAvgCon {
  // send-recv average at the connection level learning in XCal, combined with synaptic depression connection at the cycle level, based on synaptic integration of calcium
INHERITED(LeabraCon)
public:
  float		effwt;		// #NO_SAVE effective weight value (subject to synaptic depression) -- used for sending activation
  float		cai;		// #NO_SAVE intracellular postsynaptic calcium current integrated over cycles, used for synaptic depression

  SRAvgCaiSynDepCon() { effwt = 0.0f; cai = 0.0f; }
};

class LEABRA_API SRAvgCaiSynDepConSpec : public LeabraConSpec {
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

  inline void C_Send_InhibDelta_Thrd(SRAvgCaiSynDepCon* cn, float* send_inhib_vec,
				    LeabraUnit* ru, float su_act_delta_eff) {
    send_inhib_vec[ru->flat_idx] += cn->effwt * su_act_delta_eff;
  }

  inline void C_Send_InhibDelta_NoThrd(SRAvgCaiSynDepCon* cn, LeabraUnit* ru, float su_act_delta_eff) {
    ru->g_i_delta += cn->effwt * su_act_delta_eff;
  }

  override void Send_NetinDelta(LeabraSendCons* cg, LeabraNetwork* net,
			      int thread_no, float su_act_delta) {
    float su_act_delta_eff = cg->scale_eff * su_act_delta;
    if(inhib && net->inhib_cons_used) { // both must agree that inhib is ok
      if(thread_no < 0) {
	CON_GROUP_LOOP(cg, C_Send_InhibDelta_NoThrd((SRAvgCaiSynDepCon*)cg->OwnCn(i),
						   (LeabraUnit*)cg->Un(i), su_act_delta_eff));
      }
      else {
	float* send_inhib_vec = net->send_inhib_tmp.el
	  + net->send_inhib_tmp.FastElIndex(0, thread_no);
	CON_GROUP_LOOP(cg, C_Send_InhibDelta_Thrd((SRAvgCaiSynDepCon*)cg->OwnCn(i),
						 send_inhib_vec, (LeabraUnit*)cg->Un(i),
						 su_act_delta_eff));
      }
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

class LEABRA_API FastWtSpec : public SpecMemberBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra specificiations for fast weights
INHERITED(SpecMemberBase)
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
protected:
  SPEC_DEFAULTS;
private:
  void	Initialize();
  void	Destroy()	{ };
  void	Defaults_init() { Initialize(); }
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

  void 		C_Init_Weights_post(BaseCons* cg, Connection* cn, Unit* ru, Unit* su) {
    inherited::C_Init_Weights_post(cg, cn, ru, su);
    FastWtCon* lcn = (FastWtCon*)cn; lcn->swt = lcn->wt;
  }

  void 		C_Init_dWt(RecvCons* cg, Connection* cn, Unit* ru, Unit* su)
  { ConSpec::C_Init_dWt(cg, cn, ru, su); ((FastWtCon*)cn)->sdwt=0.0f; }

  override void SetCurLrate(LeabraNetwork* net, int epoch);

  inline float C_Compute_SlowHebb(FastWtCon* cn, LeabraSendCons* cg,
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

  override void Compute_dWt_LeabraCHL(LeabraSendCons* cg, LeabraUnit* su) {
    Compute_SAvgCor(cg, su);
    if(((LeabraLayer*)cg->prjn->from.ptr())->acts_p.avg < savg_cor.thresh) return;

    for(int i=0; i<cg->size; i++) {
      LeabraUnit* ru = (LeabraUnit*)cg->Un(i);
      FastWtCon* cn = (FastWtCon*)cg->OwnCn(i);
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

  inline void Compute_Weights_LeabraCHL(LeabraSendCons* cg, LeabraUnit* su) {
    CON_GROUP_LOOP(cg, C_Compute_Weights_LeabraCHL((FastWtCon*)cg->OwnCn(i)));
    //  ApplyLimits(cg, ru); limits are automatically enforced anyway
  }

  TA_SIMPLE_BASEFUNS(FastWtConSpec);
protected:
  SPEC_DEFAULTS;
private:
  void 	Initialize();
  void	Destroy()		{ };
  void	Defaults_init() 	{ };
};


//////////////////////////////////////////////////////////////////////////
//	Activation Trace Hebbian learning (Foldiak, Rolls etc)

class LEABRA_API ActAvgHebbMixSpec : public SpecMemberBase {
  // ##INLINE ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra mixture of average activation hebbian learning and regular hebbian learning (on current act value)
INHERITED(SpecMemberBase)
public:
  float		act_avg;	// what proportion of average activation to include in hebbian receiving unit activation learning term
  float		cur_act;	// #READ_ONLY #SHOW 1.0 - act_avg -- proportion of current activation for hebbian learning

  SIMPLE_COPY(ActAvgHebbMixSpec);
  TA_BASEFUNS(ActAvgHebbMixSpec);
protected:
  SPEC_DEFAULTS;
  void	UpdateAfterEdit_impl();
private:
  void	Initialize();
  void	Destroy()	{ };
  void	Defaults_init() { Initialize(); }
};

class LEABRA_API ActAvgHebbConSpec : public LeabraConSpec {
  // hebbian learning that includes a proportion of average activation over time, in addition to standard current unit activation;  produces a trace-based learning effect for learning over trajectories -- only for Leabra_CHL
INHERITED(LeabraConSpec)
public:
  ActAvgHebbMixSpec	act_avg_hebb; // mixture of current and average activations to use in hebbian learning

 inline float C_Compute_Hebb(LeabraCon* cn, LeabraSendCons* cg, float lin_wt,
			     float ru_act, float su_act, float ru_avg_act)
  {
    // wt is negative in linear form, so using opposite sign of usual here
    float eff_ru_act = act_avg_hebb.act_avg * ru_avg_act + act_avg_hebb.cur_act * ru_act;
    return eff_ru_act * (su_act * (cg->savg_cor - lin_wt) - (1.0f - su_act) * lin_wt);
  }

  // this computes weight changes based on sender at time t-1
  override void Compute_dWt_LeabraCHL(LeabraSendCons* cg, LeabraUnit* su) {
    Compute_SAvgCor(cg, su);
    if(((LeabraLayer*)cg->prjn->from.ptr())->acts_p.avg < savg_cor.thresh) return;

    for(int i=0; i<cg->size; i++) {
      LeabraUnit* ru = (LeabraUnit*)cg->Un(i);
      LeabraCon* cn = (LeabraCon*)cg->OwnCn(i);
      float lin_wt = LinFmSigWt(cn->wt);
      C_Compute_dWt(cn, ru, 
		    C_Compute_Hebb(cn, cg, lin_wt, ru->act_p, su->act_p, ru->act_avg),
		    C_Compute_Err_LeabraCHL(cn, lin_wt, ru->act_p, ru->act_m, su->act_p, su->act_m));  
    }
  }

  TA_SIMPLE_BASEFUNS(ActAvgHebbConSpec);
protected:
  SPEC_DEFAULTS;
  void	UpdateAfterEdit_impl();
private:
  void 	Initialize();
  void	Destroy()		{ };
  void	Defaults_init() 	{ };
};

class LEABRA_API LeabraDeltaConSpec : public LeabraConSpec {
  // basic delta-rule learning (plus - minus) * sender, with sender in the minus phase -- soft bounding as specified in spec -- no hebbian or anything else
INHERITED(LeabraConSpec)
public:
  inline void C_Compute_dWt_Delta(LeabraCon* cn, LeabraUnit* ru, LeabraUnit* su) {
    float lin_wt = LinFmSigWt(cn->wt);
    float dwt = (ru->act_p - ru->act_m) * su->act_m; // basic delta rule, sender in minus
    if(lmix.err_sb) {
      if(dwt > 0.0f)	dwt *= (1.0f - lin_wt);
      else		dwt *= lin_wt;
    }
    cn->dwt += cur_lrate * dwt;
  }

  inline void C_Compute_dWt_Delta_CAL(LeabraCon* cn, LeabraUnit* ru, LeabraUnit* su) {
    float dwt = (ru->act_p - ru->act_m) * su->act_m; // basic delta rule, sender in minus
    cn->dwt += cur_lrate * dwt;
    // soft bounding is managed in the weight update phase, not in dwt
  }

  override void Compute_dWt_LeabraCHL(LeabraSendCons* cg, LeabraUnit* su) {
    for(int i=0; i<cg->size; i++) {
      LeabraUnit* ru = (LeabraUnit*)cg->Un(i);
      LeabraCon* cn = (LeabraCon*)cg->OwnCn(i);
      C_Compute_dWt_Delta(cn, ru, su);  
    }
  }

  override void Compute_dWt_CtLeabraXCAL(LeabraSendCons* cg, LeabraUnit* su) {
    for(int i=0; i<cg->size; i++) {
      LeabraUnit* ru = (LeabraUnit*)cg->Un(i);
      LeabraCon* cn = (LeabraCon*)cg->OwnCn(i);
      C_Compute_dWt_Delta_CAL(cn, ru, su);  
    }
  }

  override void Compute_dWt_CtLeabraCAL(LeabraSendCons* cg, LeabraUnit* su) {
    for(int i=0; i<cg->size; i++) {
      LeabraUnit* ru = (LeabraUnit*)cg->Un(i);
      LeabraCon* cn = (LeabraCon*)cg->OwnCn(i);
      C_Compute_dWt_Delta_CAL(cn, ru, su);  
    }
  }

  TA_SIMPLE_BASEFUNS(LeabraDeltaConSpec);
protected:
  SPEC_DEFAULTS;
  void	UpdateAfterEdit_impl();
private:
  void 	Initialize();
  void	Destroy()		{ };
  void	Defaults_init() 	{ Initialize(); }
};

//////////////////////////////////////////////////
//		XCAL Spike-Based Learning

// turn this on to enable various debugging things..
// #define XCAL_DEBUG 1

class LeabraSpikeCon : public LeabraSRAvgCon {
  // #STEM_BASE ##CAT_Leabra Leabra connection for spike-based learning
public:
  float		sravg_ss;	// #NO_SAVE super-short time-scale average of sender and receiver activation product over time (just for smoothing over transients) -- cascaded into sravg_s
  float		nmda;		// #NO_SAVE proportion of open NMDA receptor channels
  float		ca;		// #NO_SAVE postsynaptic Ca value, drives learning

#ifdef XCAL_DEBUG
  float		srprod_s;	// #NO_SAVE TODO: temp!!! s-r product comparison short term value (from sep bias guys)
  float		srprod_m;	// #NO_SAVE TODO: temp!!! s-r product comparison medium term value (from sep bias guys)
#endif
  
  LeabraSpikeCon() { sravg_ss = nmda = ca = 0.0f;
#ifdef XCAL_DEBUG
    srprod_s = srprod_m = 0.0f;
#endif
  }
};

class LEABRA_API XCALSpikeSpec : public SpecMemberBase {
  // ##INLINE ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra XCAL purely spiking learning rule based on Urakubo et al 2008
INHERITED(SpecMemberBase)
public:
  bool		ss_sr;	   // #DEF_false do super-short sender-recv multiplication instead of full Urakubo thing -- just for testing/debugging

  float		ca_norm;   // #DEF_5 normalization factor for ca -- divide all ca constants by this amount
  float		k_ca;	   // #READ_ONLY #SHOW (.3 in original units) effective Ca that gives 50% inhibition of maximal NMDA receptor activity
  float		ca_vgcc;   // #READ_ONLY #SHOW (1.3 in original units) Ca influx resulting from receiver spiking (due to voltage gated calcium channels)
  float		ca_v_nmda; // #READ_ONLY #SHOW (0.0223 in original units) Ca influx due to membrane-potential (voltage) driven NMDA receptor activation
  float		ca_nmda;   // #READ_ONLY #SHOW (0.5 in original units) Ca influx from NMDA that is NOT driven by membrane potential 
  float		ca_dt;     // #DEF_20 time constant (in msec) for decay of Ca 
  float		ca_rate;   // #READ_ONLY #NO_SAVE rate constant (1/dt) for decay of Ca 
  float		ca_off;	   // #DEF_0.55 offset for ca -- subtract this amount from ca (clipped to zero) for learning computations
  float		nmda_dt;   // #DEF_40 time constant (in msec) for decay of NMDA receptor conductance
  float		nmda_rate; // #READ_ONLY #NO_SAVE rate constant (1/dt) for decay of NMDA receptor conductance
  
  TA_SIMPLE_BASEFUNS(XCALSpikeSpec);
protected:
  SPEC_DEFAULTS;
  void	UpdateAfterEdit_impl();
private:
  void	Initialize();
  void	Destroy()	{ };
  void	Defaults_init() { Initialize(); }
};

class LEABRA_API LeabraXCALSpikeConSpec : public LeabraConSpec {
  // XCAL purely spiking learning rule based on Urakubo et al 2008 -- computes a postsynaptic calcium value that drives learning using the XCAL_C fully continous-time learning parameters
INHERITED(LeabraConSpec)
public:
  XCALSpikeSpec	xcal_spike;	// #CAT_Learning #CONDSHOW_ON_learn_rule:CTLEABRA_XCAL_C XCAL (eXtended Contrastive Attractor Learning) spike-based fully continuous-time learning parameters

  inline void 	C_Init_Weights(RecvCons* cg, Connection* cn, Unit* ru, Unit* su) {
    LeabraConSpec::C_Init_Weights(cg, cn, ru, su); LeabraSpikeCon* lcn = (LeabraSpikeCon*)cn;
    lcn->sravg_ss = 0.15f; lcn->sravg_s = 0.15f; lcn->sravg_m = 0.15f;
    lcn->nmda = 0.0f; lcn->ca = 0.0f; 
#ifdef XCAL_DEBUG
    lcn->srprod_s = lcn->srprod_m = xcal.avg_init;
#endif
  }

  inline void Trial_Init_SRAvg(LeabraSendCons* cg, LeabraUnit* su) { };
  // never init..

  inline void C_Compute_SRAvg_spike(LeabraSpikeCon* cn, LeabraUnit* ru, LeabraUnit* su,
				    LeabraUnitSpec* us) {
    // this happens every cycle, and is the place to compute nmda and ca -- expensive!! :(
    float dnmda = -cn->nmda * xcal_spike.nmda_rate;
    float dca = (cn->nmda * (xcal_spike.ca_v_nmda * ru->vm_dend + xcal_spike.ca_nmda))
      - (cn->ca * xcal_spike.ca_rate);
    if(su->act > 0.5f) { dnmda += xcal_spike.k_ca / (xcal_spike.k_ca + cn->ca); }
    if(ru->act > 0.5f) { dca += xcal_spike.ca_vgcc; }
    cn->nmda += dnmda;
    cn->ca += dca;
    float sr = (cn->ca - xcal_spike.ca_off);
    if(sr < 0.0f) sr = 0.0f;
    cn->sravg_ss += us->act_avg.ss_dt * (sr - cn->sravg_ss);
    cn->sravg_s += us->act_avg.s_dt * (cn->sravg_ss - cn->sravg_s);
    cn->sravg_m += us->act_avg.m_dt * (cn->sravg_s - cn->sravg_m);

#ifdef XCAL_DEBUG
    cn->srprod_s = ru->avg_s * su->avg_s;
    cn->srprod_m = ru->avg_m * su->avg_m;
#endif
  }

  inline void C_Compute_SRAvg_sssr(LeabraSpikeCon* cn, LeabraUnit* ru, LeabraUnit* su,
				   LeabraUnitSpec* us) {
    cn->sravg_ss = ru->avg_ss * su->avg_ss; // use ss to capture local time window
    cn->sravg_s += us->act_avg.s_dt * (cn->sravg_ss - cn->sravg_s);
    cn->sravg_m += us->act_avg.m_dt * (cn->sravg_s - cn->sravg_m);
  }

  inline void Compute_SRAvg(LeabraSendCons* cg, LeabraUnit* su, bool do_s) {
    LeabraUnitSpec* us = (LeabraUnitSpec*)su->GetUnitSpec();
    if(learn_rule == CTLEABRA_XCAL_C) {
      if(xcal_spike.ss_sr) {
	CON_GROUP_LOOP(cg, C_Compute_SRAvg_sssr((LeabraSpikeCon*)cg->OwnCn(i), 
						(LeabraUnit*)cg->Un(i), su, us));
      }
      else {
	CON_GROUP_LOOP(cg, C_Compute_SRAvg_spike((LeabraSpikeCon*)cg->OwnCn(i), 
						 (LeabraUnit*)cg->Un(i), su, us));
      }
    }
    else {
      inherited::Compute_SRAvg(cg, su, do_s);
    }
  }

  inline void C_Compute_dWt_CtLeabraXCAL_spike(LeabraSpikeCon* cn, LeabraUnit* ru,
					       LeabraUnit* su, float su_act_mult) {
    float srs = cn->sravg_s;
    float srm = cn->sravg_m;
    float sm_mix = xcal.s_mix * srs + xcal.m_mix * srm;
    float effthr = xcal.thr_m_mix * srm + su_act_mult * ru->avg_l;
    cn->dwt += cur_lrate * xcal.dWtFun(sm_mix, effthr);
  }

  inline void Compute_dWt_CtLeabraXCAL(LeabraSendCons* cg, LeabraUnit* su) {
    float su_avg_m = su->avg_m;
    float su_act_mult = xcal.thr_l_mix * su_avg_m;

    if(learn_rule == CTLEABRA_XCAL_C) {
      for(int i=0; i<cg->size; i++) {
	LeabraUnit* ru = (LeabraUnit*)cg->Un(i);
	C_Compute_dWt_CtLeabraXCAL_spike((LeabraSpikeCon*)cg->OwnCn(i), ru, su, su_act_mult);
      }
    }
    else {
      inherited::Compute_dWt_CtLeabraXCAL(cg, su);
    }
  }

  virtual void	GraphXCALSpikeSim(DataTable* graph_data = NULL,
		  LeabraUnitSpec* unit_spec = NULL,
		  float rate_min=0.0f, float rate_max=150.0f, float rate_inc=5.0f,
		  float max_time=250.0f, int reps_per_point=5,
				  float lin_norm=0.01f);
  // #BUTTON #NULL_OK #NULL_TEXT_NewGraphData graph a simulation of the XCAL spike function by running a simulated synapse with poisson firing rates sampled over given range, with given samples per point, and other parameters as given

  TA_SIMPLE_BASEFUNS(LeabraXCALSpikeConSpec);
protected:
  SPEC_DEFAULTS;
  void	UpdateAfterEdit_impl();
private:
  void 	Initialize();
  void	Destroy()		{ };
  void	Defaults_init() 	{ };
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
  inline void Compute_LimPrecWts(LeabraSendCons* cg, LeabraUnit* su) {
    CON_GROUP_LOOP(cg, C_Compute_LimPrecWts((LeabraCon*)cg->OwnCn(i)));
  }

  inline override void	Compute_Weights_LeabraCHL(LeabraSendCons* cg, LeabraUnit* su) {
    inherited::Compute_Weights_LeabraCHL(cg, su);
    Compute_LimPrecWts(cg, su);
  }

  inline override void	Compute_Weights_CtLeabraXCAL(LeabraSendCons* cg, LeabraUnit* su) {
    inherited::Compute_Weights_CtLeabraXCAL(cg, su);
    Compute_LimPrecWts(cg, su);
  }

  inline override void	Compute_Weights_CtLeabraCAL(LeabraSendCons* cg, LeabraUnit* su) {
    inherited::Compute_Weights_CtLeabraCAL(cg, su);
    Compute_LimPrecWts(cg, su);
  }

  // NOTE: bias weights typically not subject to limited precision!

  SIMPLE_COPY(LeabraLimPrecConSpec);
  TA_BASEFUNS(LeabraLimPrecConSpec);
protected:
  SPEC_DEFAULTS;
//   void	UpdateAfterEdit_impl();
private:
  void 	Initialize();
  void	Destroy()		{ };
  void	Defaults_init()		{ };
};

////////////////////////////////////////////////////////////////////////
//	da-noise modulated learning as in MazzoniAndersenJordan91

class LEABRA_API LeabraDaNoise : public SpecMemberBase {
  // ##INLINE ##NO_TOKENS ##CAT_Leabra da-noise modulated learning as in MazzoniAndersenJordan91
INHERITED(SpecMemberBase)
public:
  float		da_noise;	// #DEF_0:1 amount to add of additional reinforcement-learning term based on unit dopamine value (dav) and TRIAL_VM_NOISE noise value, as in MazzoniAndersenJordan91: dwt = dav * (ru_act_p+noise - ru_act_p-noise) * su_act -- activation with noise (std acts as computed) minus activation without noise (specially computed given noise value) times sending activation times dopamine value -- if it does better and noise made unit more active, then make it more active next time (and so on for all other permutations)
  float		std_leabra;	// #DEF_0:1 how much of standard leabra learning to include in addition to the da_noise term

  SIMPLE_COPY(LeabraDaNoise);
  TA_BASEFUNS(LeabraDaNoise);
protected:
  SPEC_DEFAULTS;
  //  void UpdateAfterEdit_impl();
private:
  void	Initialize();
  void 	Destroy()	{ };
  void	Defaults_init() { Initialize(); }
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

  inline void Compute_dWt_DaNoise(LeabraSendCons* cg, LeabraUnit* su) {
    // compute what activation value would be if we subtract out noise -- note that
    // we don't save v_m by phase so this is necessarily on the current v_m val, assumed
    // to be plus-phase value

    for(int i=0; i<cg->size; i++) {
      LeabraUnit* ru = (LeabraUnit*)cg->Un(i);
      LeabraUnitSpec* rus = (LeabraUnitSpec*)ru->GetUnitSpec();

      // note: with switch to sender-based, this is very expensive -- fortunately it doesn't
      // really work so we don't care.. :) -- also, not supporting act.gelin at all here
      float ru_act_nonoise = ru->Compute_ActValFmVmVal_rate((ru->v_m - ru->noise) - rus->act.thr);
      float dav = ru->dav * da_noise.da_noise;

      LeabraCon* cn = (LeabraCon*)cg->OwnCn(i);
      float lin_wt = LinFmSigWt(cn->wt);
      C_Compute_dWt_DaNoise(cn, lin_wt, dav, ru->act_p, ru_act_nonoise, su->act_p);
    }
  }

  inline void C_Compute_dWt(LeabraCon* cn, LeabraUnit*, float heb, float err) {
    float dwt = lmix.err * err + lmix.hebb * heb;
    cn->dwt += da_noise.std_leabra * cur_lrate * dwt;
  }

  override void Compute_dWt_LeabraCHL(LeabraSendCons* cg, LeabraUnit* su) {
    if(da_noise.std_leabra > 0.0f) {
      // this is a copy of the main fun, but uses above C_Compute_dWt which mults dwt
      Compute_SAvgCor(cg, su);
      if(((LeabraLayer*)cg->prjn->from.ptr())->acts_p.avg >= savg_cor.thresh) {
	for(int i=0; i<cg->size; i++) {
	  LeabraUnit* ru = (LeabraUnit*)cg->Un(i);
	  LeabraCon* cn = (LeabraCon*)cg->OwnCn(i);
	  float lin_wt = LinFmSigWt(cn->wt);
	  C_Compute_dWt(cn, ru, 
			C_Compute_Hebb(cn, cg, lin_wt, ru->act_p, su->act_p),
			C_Compute_Err_LeabraCHL(cn, lin_wt, ru->act_p, ru->act_m,
						su->act_p, su->act_m));  
	}
      }
    }
    if(da_noise.da_noise > 0.0f) {
      Compute_dWt_DaNoise(cg, su);
    }
  }

  // todo: add xcal version perhaps if promising..  not.. so maybe not..

  TA_SIMPLE_BASEFUNS(LeabraDaNoiseConSpec);
protected:
  SPEC_DEFAULTS;
//   void	UpdateAfterEdit_impl();
private:
  void 	Initialize();
  void	Destroy()		{ };
  void	Defaults_init() 	{ };
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

class LEABRA_API ScalarValSpec : public SpecMemberBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra specs for scalar values
INHERITED(SpecMemberBase)
public:
  enum	RepType {
    GAUSSIAN,			// gaussian bump, with value = weighted average of tuned unit values
    LOCALIST,			// each unit represents a distinct value; intermediate values represented by graded activity of neighbors; overall activity is weighted-average across all units
  };

  RepType	rep;		// type of representation of scalar value to use
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
protected:
  SPEC_DEFAULTS;
private:
  void	Initialize();
  void 	Destroy()	{ };
  void	Defaults_init();
};

class LEABRA_API ScalarValBias : public SpecMemberBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra initial bias for given activation value for scalar value units
INHERITED(SpecMemberBase)
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

  UnitBias	un;		// bias on individual units
  BiasShape	un_shp;		// shape of unit bias
  float		un_gain;	// #CONDEDIT_OFF_un:NO_UN #DEF_1 gain multiplier (strength) of bias to apply for units.  WT = .03 as basic weight multiplier
  WeightBias	wt;		// bias on weights: always uses a val-shaped bias
  float		wt_gain;	// #CONDEDIT_OFF_wt:NO_WT #DEF_1 gain multiplier (strength) of bias to apply for weights (gain 1 = .03 wt value)
  float		val;		// value location (center of gaussian bump)

  SIMPLE_COPY(ScalarValBias);
  TA_BASEFUNS(ScalarValBias);
protected:
  SPEC_DEFAULTS;
private:
  void	Initialize();
  void 	Destroy()	{ };
  void	Defaults_init() { }; 	// note: does NOT do any init -- these vals are not really subject to defaults in the usual way, so don't mess with them
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
    virtual void Settle_Init_Unit0_ugp(LeabraLayer* lay, Layer::AccessMode acc_md, int gpidx,
				       LeabraNetwork* net);
    // #IGNORE

  virtual void	ClampValue_ugp(LeabraLayer*, Layer::AccessMode acc_md, int gpidx,
			       LeabraNetwork* net, float rescale=1.0f);
  // #CAT_ScalarVal clamp value in the first unit's ext field to the units in the group
  virtual float	ClampAvgAct(int ugp_size);
  // #CAT_ScalarVal computes the average activation for a clamped unit pattern (for computing rescaling)
  virtual void	ReadValue(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_ScalarVal read out current value represented by activations in layer
    virtual float ReadValue_ugp(LeabraLayer* lay, Layer::AccessMode acc_md, int gpidx,
				LeabraNetwork* net);
    // #CAT_ScalarVal unit group version: read out current value represented by activations in layer
  virtual void 	Compute_ExtToPlus_ugp(LeabraLayer* lay, Layer::AccessMode acc_md, int gpidx,
				      LeabraNetwork* net);
  // #CAT_ScalarVal copy ext values to act_p -- used for internally-generated training signals for learning in several subclasses
  virtual void 	Compute_ExtToAct_ugp(LeabraLayer* lay, Layer::AccessMode acc_md, int gpidx,
				      LeabraNetwork* net);
  // #CAT_ScalarVal copy ext values to act -- used for dynamically computed clamped layers
  virtual void HardClampExt(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_ScalarVal hard clamp current ext values (on all units, after ClampValue called) to all the units (calls ResetAfterClamp)
    virtual void ResetAfterClamp(LeabraLayer* lay, LeabraNetwork* net);
    // #CAT_ScalarVal reset activation of first unit(s) after hard clamping
    virtual void ResetAfterClamp_ugp(LeabraLayer* lay, Layer::AccessMode acc_md, int gpidx,
				LeabraNetwork* net);
    // #IGNORE

  virtual void	LabelUnits(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_ScalarVal label units in given layer with their underlying values
    virtual void LabelUnits_ugp(LeabraLayer* lay, Layer::AccessMode acc_md, int gpidx,
				LeabraNetwork* net);
    // #CAT_ScalarVal label units with their underlying values
  virtual void	LabelUnitsNet(LeabraNetwork* net);
  // #BUTTON #CAT_ScalarVal label all layers in given network using this spec

  virtual void	Compute_BiasVal(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_ScalarVal initialize the bias value 
    virtual void Compute_WtBias_Val(LeabraLayer* lay, Layer::AccessMode acc_md, int gpidx,
				    float val);
    // #IGNORE
    virtual void Compute_UnBias_Val(LeabraLayer* lay, Layer::AccessMode acc_md, int gpidx,
				    float val);
    // #IGNORE
    virtual void Compute_UnBias_NegSlp(LeabraLayer* lay, Layer::AccessMode acc_md, int gpidx);
    // #IGNORE
    virtual void Compute_UnBias_PosSlp(LeabraLayer* lay, Layer::AccessMode acc_md, int gpidx);
    // #IGNORE

  override void BuildUnits_Threads(LeabraLayer* lay, LeabraNetwork* net);
    virtual void BuildUnits_Threads_ugp(LeabraLayer* lay, Layer::AccessMode acc_md, int gpidx,
					LeabraNetwork* net);
  override void Init_Weights(LeabraLayer* lay, LeabraNetwork* net);
  override void	Settle_Init_Layer(LeabraLayer* lay, LeabraNetwork* net);
  override void	Compute_HardClamp(LeabraLayer* lay, LeabraNetwork* net);
  override void	Compute_CycleStats(LeabraLayer* lay, LeabraNetwork* net);

  // don't include first unit in averages..
  override void Compute_AvgMaxVals_ugp(LeabraLayer* lay,
				       Layer::AccessMode acc_md, int gpidx,
				       AvgMaxVals& vals, ta_memb_ptr mb_off);

  override float Compute_SSE(LeabraLayer* lay, LeabraNetwork* net, int& n_vals,
			     bool unit_avg = false, bool sqrt = false);
    virtual float Compute_SSE_ugp(LeabraLayer* lay, Layer::AccessMode acc_md, int gpidx,
				  int& n_vals);
    // #IGNORE
  override float Compute_NormErr(LeabraLayer* lay, LeabraNetwork* net);
    override float Compute_NormErr_ugp(LeabraLayer* lay,
				       Layer::AccessMode acc_md, int gpidx,
				       LeabraInhib* thr, LeabraNetwork* net);
    // #IGNORE

  virtual void	ReConfig(Network* net, int n_units = -1);
  // #BUTTON #CAT_ScalarVal reconfigure layer and associated specs for current scalar.rep type; if n_units > 0, changes number of units in layer to specified value

  void	HelpConfig();	// #BUTTON get help message for configuring this spec
  bool  CheckConfig_Layer(Layer* lay, bool quiet=false);

  TA_SIMPLE_BASEFUNS(ScalarValLayerSpec);
protected:
  SPEC_DEFAULTS;
  void	UpdateAfterEdit_impl();
private:
  void 	Initialize();
  void	Destroy()		{ };
  void	Defaults_init();
};

class LEABRA_API ScalarValSelfPrjnSpec : public ProjectionSpec {
  // special projection for making self-connection that establishes neighbor similarity in scalar val
INHERITED(ProjectionSpec)
public:
  int	width;			// width of neighborhood, in units (i.e., connect width units to the left, and width units to the right)
  float	wt_width;		// #CONDEDIT_ON_init_wts width of the sigmoid for providing initial weight values
  float	wt_max;			// #CONDEDIT_ON_init_wts maximum weight value (of 1st neighbor -- not of self unit!)

  virtual void	Connect_UnitGroup(Layer* lay, Layer::AccessMode acc_md, int gpidx,
				  Projection* prjn);
  void		Connect_impl(Projection* prjn);
  void		C_Init_Weights(Projection* prjn, RecvCons* cg, Unit* ru);
  // uses weight values as specified in the tesselel's

  TA_SIMPLE_BASEFUNS(ScalarValSelfPrjnSpec);
protected:
  SPEC_DEFAULTS;
private:
  void	Initialize();
  void 	Destroy()		{ };
  void	Defaults_init() 	{ };
};

class LEABRA_API MotorForceSpec : public SpecMemberBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra specs for scalar values
INHERITED(SpecMemberBase)
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
protected:
  SPEC_DEFAULTS;
private:
  void	Initialize();
  void 	Destroy()	{ };
  void	Defaults_init() { };
};

class LEABRA_API MotorForceLayerSpec : public ScalarValLayerSpec {
  // represents motor force as a function of joint position and velocity using scalar val layer spec: layer uses unit groups -- each group represents a force (typically localist), and groups are organized in X axis by position, Y axis by velocity.  Overall value is weighted average from neighboring unit groups
INHERITED(ScalarValLayerSpec)
public:
  MotorForceSpec motor_force;   // misc specs for motor force representation
  MinMaxRange	 pos_range;	// range of position values encoded over the X axis of unit groups in the layer
  MinMaxRange	 vel_range;	// range of velocity values encoded over the Y axis of unit groups in the layer
  bool		 add_noise;	// add some noise after computing value from layer
  RandomSpec	 force_noise;	// #CONDEDIT_ON_add_noise parameters for random added noise to forces

  virtual float	ReadForce(LeabraLayer* lay, LeabraNetwork* net, float pos, float vel);
  // #CAT_MotorForce read the force value from the layer, as a gaussian weighted average over units near the current position and velocity values
  virtual void	ClampForce(LeabraLayer* lay, LeabraNetwork* net, float force, float pos, float vel);
  // #CAT_MotorForce clamp the force value to the layer, as a gaussian weighted average over units near the current position and velocity values

  override void	Compute_BiasVal(LeabraLayer* lay, LeabraNetwork* net);
  
  bool  CheckConfig_Layer(Layer* lay, bool quiet=false);

  TA_SIMPLE_BASEFUNS(MotorForceLayerSpec);
protected:
  SPEC_DEFAULTS;
  void	UpdateAfterEdit_impl();
private:
  void 	Initialize();
  void	Destroy()		{ };
  void	Defaults_init() 	{ };
};


//////////////////////////////////
// 	TwoD Value Layer	//
//////////////////////////////////

class LEABRA_API TwoDValLeabraLayer : public LeabraLayer {
  // represents one or more two-d value(s) using a coarse-coded distributed code over units.  one val readout is weighted-average; multiple vals = max bumps over 3x3 local grid -- uses separate matrix storage of x,y values (prev impl used first row of layer)
INHERITED(LeabraLayer)
public:
  enum TwoDXY {			// x-y two-d vals
    TWOD_X,			// the horizontal (X) value encoded in the layer
    TWOD_Y,			// the vertical (Y) value encoded in the layer
    TWOD_XY,			// number of xy vals (2)
  };

  enum TwoDValTypes {		// different values encoded in the twod_vals matrix
    TWOD_EXT,			// external inputs
    TWOD_TARG,			// target values
    TWOD_ACT,			// current activation
    TWOD_ACT_M,			// minus phase activations
    TWOD_ACT_P,			// plus phase activations
    TWOD_ACT_DIF,		// difference between plus and minus phase activations
    TWOD_ACT_M2,		// second minus phase activations
    TWOD_ACT_P2,		// second plus phase activations
    TWOD_ACT_DIF2,		// difference between second plus and minus phase activations
    TWOD_ERR,			// error from target: targ - act_m
    TWOD_SQERR,			// squared error from target: (targ - act_m)^2
    TWOD_N,			// number of val types to encode
  };
  
  float_Matrix		twod_vals; // #SHOW_TREE matrix of layer-encoded values, dimensions: [gp_y][gp_x][n_vals][TWOD_N][TWOD_XY] (outer to inner) -- gp_y and gp_x are group indices, size 1,1, for a layer with no unit groups

  inline float	GetTwoDVal(TwoDXY xy, TwoDValTypes val_typ, int val_no, int gp_x=0, int gp_y=0) {
    return twod_vals.SafeElAsFloat(xy, val_typ, val_no, gp_x, gp_y);
  }
  // #CAT_TwoD get a two-d value encoded in the twod_vals data 
  inline void	GetTwoDVals(float& x_val, float& y_val, TwoDValTypes val_typ, int val_no, int gp_x=0, int gp_y=0) {
    x_val = twod_vals.SafeElAsFloat(TWOD_X, val_typ, val_no, gp_x, gp_y);
    y_val = twod_vals.SafeElAsFloat(TWOD_Y, val_typ, val_no, gp_x, gp_y);
  }
  // #CAT_TwoD get a two-d value encoded in the twod_vals data 

  inline void	SetTwoDVal(Variant val, TwoDXY xy, TwoDValTypes val_typ, int val_no, int gp_x=0, int gp_y=0) {
    twod_vals.SetFmVar(val, xy, val_typ, val_no, gp_x, gp_y);
  }
  // #CAT_TwoD set a two-d value encoded in the twod_vals data 
  inline void	SetTwoDVals(Variant x_val, Variant y_val, TwoDValTypes val_typ, int val_no, int gp_x=0, int gp_y=0) {
    twod_vals.SetFmVar(x_val, TWOD_X, val_typ, val_no, gp_x, gp_y);
    twod_vals.SetFmVar(y_val, TWOD_Y, val_typ, val_no, gp_x, gp_y);
  }
  // #CAT_TwoD set both two-d values encoded in the twod_vals data 

  virtual void		UpdateTwoDValsGeom();
  // update the twod_vals geometry based on current layer and layer spec settings

  TA_SIMPLE_BASEFUNS(TwoDValLeabraLayer);
protected:
  override void	UpdateAfterEdit_impl();

  override void	ApplyInputData_2d(taMatrix* data, Unit::ExtType ext_flags,
				  Random* ran, const TwoDCoord& offs, bool na_by_range=false);
  override void	ApplyInputData_Flat4d(taMatrix* data, Unit::ExtType ext_flags,
				      Random* ran, const TwoDCoord& offs, bool na_by_range=false);
  override void	ApplyInputData_Gp4d(taMatrix* data, Unit::ExtType ext_flags,
				    Random* ran, bool na_by_range=false);

private:
  void	Initialize();
  void 	Destroy()		{ };
};

class LEABRA_API TwoDValSpec : public SpecMemberBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra specs for two-dimensional values
INHERITED(SpecMemberBase)
public:
  enum	RepType {
    GAUSSIAN,			// gaussian bump, with value = weighted average of tuned unit values
    LOCALIST			// each unit represents a distinct value; intermediate values represented by graded activity of neighbors; overall activity is weighted-average across all units
  };

  RepType	rep;		// type of representation of scalar value to use
  int		n_vals;		// number of distinct sets of X,Y values to represent in layer (i.e., if > 1, then multiple bumps are encoded -- uses peaks to locate values for multiple, and full weighted average for single value
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
protected:
  SPEC_DEFAULTS;
private:
  void	Initialize();
  void 	Destroy()	{ };
  void	Defaults_init() { };
};

class LEABRA_API TwoDValBias : public SpecMemberBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra initial bias for given activation value for scalar value units
INHERITED(SpecMemberBase)
public:
  enum UnitBias {		// bias on individual units
    NO_UN,			// no unit bias
    GC,				// bias value enters as a conductance in gc.h or gc.a
    BWT,			// bias value enters as a bias.wt
  };

  enum WeightBias {		// bias on weights into units
    NO_WT,			// no weight bias
    WT,				// input weights
  };

  UnitBias	un;		// bias on individual units
  float		un_gain;	// #CONDEDIT_OFF_un:NO_UN #DEF_1 gain multiplier (strength) of bias to apply for units.  WT = .03 as basic weight multiplier
  WeightBias	wt;		// bias on weights: always uses a val-shaped bias
  float		wt_gain;	// #CONDEDIT_OFF_wt:NO_WT #DEF_1 gain multiplier (strength) of bias to apply for weights (gain 1 = .03 wt value)
  float		x_val;		// X axis value location (center of gaussian bump)
  float		y_val;		// Y axis value location (center of gaussian bump)

  SIMPLE_COPY(TwoDValBias);
  TA_BASEFUNS(TwoDValBias);
protected:
  SPEC_DEFAULTS;
private:
  void	Initialize();
  void 	Destroy()	{ };
  void	Defaults_init() { };	// note: does NOT do any init -- these vals are not really subject to defaults in the usual way, so don't mess with them
};

class LEABRA_API TwoDValLayerSpec : public LeabraLayerSpec {
  // represents one or more two-d value(s) using a coarse-coded distributed code over units.  one val readout is weighted-average; multiple vals = max bumps over 3x3 local grid -- requires TwoDValLeabraLayer to encode values (no longer using first row of units)
INHERITED(LeabraLayerSpec)
public:
  TwoDValSpec	 twod;		// specifies how values are represented in terms of distributed patterns of activation across the layer
  MinMaxRange	 x_range;	// range of values represented across the X (horizontal) axis; for GAUSSIAN, add extra values above and below true useful range to prevent edge effects.
  MinMaxRange	 y_range;	// range of values represented across the Y (vertical) axis; for GAUSSIAN, add extra values above and below true useful range to prevent edge effects.
  TwoDValBias	 bias_val;	// specifies bias values
  MinMaxRange	 x_val_range;	// #READ_ONLY #NO_INHERIT actual range of values (scalar.min/max taking into account un_range)
  MinMaxRange	 y_val_range;	// #READ_ONLY #NO_INHERIT actual range of values (scalar.min/max taking into account un_range)

  virtual void	ClampValue_ugp(TwoDValLeabraLayer* lay, Layer::AccessMode acc_md, int gpidx,
			       LeabraNetwork* net, float rescale=1.0f);
  // #CAT_TwoDVal clamp value in the first unit's ext field to the units in the group
  virtual void	ReadValue(TwoDValLeabraLayer* lay, LeabraNetwork* net);
  // #CAT_TwoDVal read out current value represented by activations in layer
    virtual void ReadValue_ugp(TwoDValLeabraLayer* lay, Layer::AccessMode acc_md, int gpidx,
			       LeabraNetwork* net);
    // #CAT_TwoDVal unit group version: read out current value represented by activations in layer
  virtual void	HardClampExt(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_TwoDVal hard clamp current ext values (on all units, after ClampValue called) to all the units (calls ResetAfterClamp)

  virtual void	LabelUnits(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_TwoDVal label units in given layer with their underlying values
    virtual void LabelUnits_ugp(LeabraLayer* lay, Layer::AccessMode acc_md, int gpidx);
    // #CAT_TwoDVal label units with their underlying values
  virtual void	LabelUnitsNet(LeabraNetwork* net);
  // #BUTTON #CAT_TwoDVal label all layers in given network using this spec

  virtual void	Compute_BiasVal(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_TwoDVal initialize the bias value 
    virtual void Compute_WtBias_Val(LeabraLayer* lay, Layer::AccessMode acc_md, int gpidx,
				    float x_val, float y_val);
    // #IGNORE
    virtual void Compute_UnBias_Val(LeabraLayer* lay, Layer::AccessMode acc_md, int gpidx,
				    float x_val, float y_val);
    // #IGNORE

  override void Init_Weights(LeabraLayer* lay, LeabraNetwork* net);
  override void	Settle_Init_Layer(LeabraLayer* lay, LeabraNetwork* net);
    override void Settle_Init_TargFlags_Layer(LeabraLayer* lay, LeabraNetwork* net);
    virtual void Settle_Init_TargFlags_Layer_ugp(TwoDValLeabraLayer* lay,
						 Layer::AccessMode acc_md, int gpidx,
						 LeabraNetwork* net);
    // #IGNORE
  override void	Compute_HardClamp(LeabraLayer* lay, LeabraNetwork* net);
  override void	Compute_CycleStats(LeabraLayer* lay, LeabraNetwork* net);
  override void	PostSettle(LeabraLayer* lay, LeabraNetwork* net);
    virtual void PostSettle_ugp(TwoDValLeabraLayer* lay,
				Layer::AccessMode acc_md, int gpidx, LeabraNetwork* net);
    // #CAT_TwoDVal unit group version: update variables based on phase

  override float Compute_SSE(LeabraLayer* lay, LeabraNetwork* net, int& n_vals,
			     bool unit_avg = false, bool sqrt = false);
    virtual float Compute_SSE_ugp(LeabraLayer* lay, Layer::AccessMode acc_md, int gpidx,
				  int& n_vals);
    // #IGNORE
  override float Compute_NormErr(LeabraLayer* lay, LeabraNetwork* net);
    virtual float Compute_NormErr_ugp(LeabraLayer* lay,
				      Layer::AccessMode acc_md, int gpidx,
				      LeabraInhib* thr, LeabraNetwork* net);
    // #IGNORE

  virtual void	ReConfig(Network* net, int n_units = -1);
  // #BUTTON reconfigure layer and associated specs for current scalar.rep type; if n_units > 0, changes number of units in layer to specified value

  void	HelpConfig();	// #BUTTON get help message for configuring this spec
  bool  CheckConfig_Layer(Layer* lay, bool quiet=false);

  TA_SIMPLE_BASEFUNS(TwoDValLayerSpec);
protected:
  SPEC_DEFAULTS;
  void	UpdateAfterEdit_impl();
private:
  void 	Initialize();
  void	Destroy()		{ };
  void	Defaults_init() 	{ };
};

class LEABRA_API DecodeTwoDValLayerSpec : public TwoDValLayerSpec {
  // a two-d-value layer spec that copies its activations from one-to-one input prjns, to act as a decoder of another layer
INHERITED(TwoDValLayerSpec)
public:
  override void ReadValue_ugp(TwoDValLeabraLayer* lay, Layer::AccessMode acc_md, int gpidx,
			      LeabraNetwork* net);
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


/*

//////////////////////////////////
// 	FourD Value Layer	//
//////////////////////////////////

class LEABRA_API FourDValLeabraLayer : public LeabraLayer {
  // represents one or more four-d value(s) using a coarse-coded distributed code over units, with units within unit groups representing the Z,W coords and overall unit groups themselves representing X,Y coords.  one val readout is weighted-average; multiple vals = max bumps over 3x3 local grid -- uses separate matrix storage of values 
INHERITED(LeabraLayer)
public:
  enum FourDXYZW {			// x-y two-d vals
    FOURD_X,			// the horizontal (X) value encoded in the layer
    FOURD_Y,			// the vertical (Y) value encoded in the layer
    FOURD_Z,			// the unit group (Z) value encoded in the layer
    FOURD_W,			// the unit group (w) value encoded in the layer
    FOURD_XYZW,			// number of xyzw vals (4)
  };

  enum FourDValTypes {		// different values encoded in the twod_vals matrix
    FOURD_EXT,			// external inputs
    FOURD_TARG,			// target values
    FOURD_ACT,			// current activation
    FOURD_ACT_M,		// minus phase activations
    FOURD_ACT_P,		// plus phase activations
    FOURD_ACT_DIF,		// difference between plus and minus phase activations
    FOURD_ACT_M2,		// second minus phase activations
    FOURD_ACT_P2,		// second plus phase activations
    FOURD_ACT_DIF2,		// difference between second plus and minus phase activations
    FOURD_ERR,			// error from target: targ - act_m
    FOURD_SQERR,		// squared error from target: (targ - act_m)^2
    FOURD_N,			// number of val types to encode
  };
  
  float_Matrix		fourd_vals; // #SHOW_TREE matrix of layer-encoded values, dimensions: [gp_y][gp_x][n_vals][FOURD_N][FOURD_XYZW] (outer to inner) -- gp_y and gp_x are group indices -- 0,0 always used for global layer vals, groups encoded as 1+gp_idx

  inline float	GetFourDVal(FourDXYZW xyzw, FourDValTypes val_typ, int val_no, int gp_x=0, int gp_y=0) {
    return fourd_vals.SafeElAsFloat(xyzw, val_typ, val_no, gp_x, gp_y);
  }
  // #CAT_FourD get a two-d value encoded in the twod_vals data 
  inline void	GetFourDVals(float& x_val, float& y_val, float& z_val, float& w_val,
			     FourDValTypes val_typ, int val_no, int gp_x=0, int gp_y=0) {
    x_val = fourd_vals.SafeElAsFloat(FOURD_X, val_typ, val_no, gp_x, gp_y);
    y_val = fourd_vals.SafeElAsFloat(FOURD_Y, val_typ, val_no, gp_x, gp_y);
    z_val = fourd_vals.SafeElAsFloat(FOURD_Z, val_typ, val_no, gp_x, gp_y);
    w_val = fourd_vals.SafeElAsFloat(FOURD_W, val_typ, val_no, gp_x, gp_y);
  }

  inline void	SetFourDVal(Variant val, FourDXYZW xyzw, FourDValTypes val_typ, int val_no, int gp_x=0, int gp_y=0) {
    fourd_vals.SetFmVar(val, xyzw, val_typ, val_no, gp_x, gp_y);
  }
  // #CAT_FourD set a four-d value encoded in the fourd_vals data 
  inline void	SetFourDVals(Variant x_val, Variant y_val, Variant z_val, Variant w_val,
			     FourDValTypes val_typ, int val_no, int gp_x=0, int gp_y=0) {
    fourd_vals.SetFmVar(x_val, FOURD_X, val_typ, val_no, gp_x, gp_y);
    fourd_vals.SetFmVar(y_val, FOURD_X, val_typ, val_no, gp_x, gp_y);
    fourd_vals.SetFmVar(z_val, FOURD_Z, val_typ, val_no, gp_x, gp_y);
    fourd_vals.SetFmVar(w_val, FOURD_W, val_typ, val_no, gp_x, gp_y);
  }
  // #CAT_FourD set four-d values encoded in the fourd_vals data 

  virtual void		UpdateFourDValsGeom();
  // update the fourd_vals geometry based on current layer and layer spec settings

  TA_SIMPLE_BASEFUNS(FourDValLeabraLayer);
protected:
  override void	UpdateAfterEdit_impl();

  override void	ApplyInputData_2d(taMatrix* data, Unit::ExtType ext_flags,
				  Random* ran, const TwoDCoord& offs, bool na_by_range=false);
  override void	ApplyInputData_Flat4d(taMatrix* data, Unit::ExtType ext_flags,
				      Random* ran, const TwoDCoord& offs, bool na_by_range=false);
  override void	ApplyInputData_Gp4d(taMatrix* data, Unit::ExtType ext_flags,
				    Random* ran, bool na_by_range=false);

private:
  void	Initialize();
  void 	Destroy()		{ };
};

class LEABRA_API FourDValSpec : public SpecMemberBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra specs for four-dimensional values
INHERITED(SpecMemberBase)
public:
  enum	RepType {
    GAUSSIAN,			// gaussian bump, with value = weighted average of tuned unit values
    LOCALIST			// each unit represents a distinct value; intermediate values represented by graded activity of neighbors; overall activity is weighted-average across all units
  };

  RepType	rep;		// type of representation of scalar value to use
  int		n_vals;		// number of distinct sets of X,Y values to represent in layer (i.e., if > 1, then multiple bumps are encoded -- uses peaks to locate values for multiple, and full weighted average for single value
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

  SIMPLE_COPY(FourDValSpec);
  TA_BASEFUNS(FourDValSpec);
protected:
  SPEC_DEFAULTS;
private:
  void	Initialize();
  void 	Destroy()	{ };
  void	Defaults_init() { };
};

class LEABRA_API FourDValBias : public SpecMemberBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra initial bias for given activation value for four-d value units
INHERITED(SpecMemberBase)
public:
  enum UnitBias {		// bias on individual units
    NO_UN,			// no unit bias
    GC,				// bias value enters as a conductance in gc.h or gc.a
    BWT,			// bias value enters as a bias.wt
  };

  enum WeightBias {		// bias on weights into units
    NO_WT,			// no weight bias
    WT,				// input weights
  };

  UnitBias	un;		// bias on individual units
  float		un_gain;	// #CONDEDIT_OFF_un:NO_UN #DEF_1 gain multiplier (strength) of bias to apply for units.  WT = .03 as basic weight multiplier
  WeightBias	wt;		// bias on weights: always uses a val-shaped bias
  float		wt_gain;	// #CONDEDIT_OFF_wt:NO_WT #DEF_1 gain multiplier (strength) of bias to apply for weights (gain 1 = .03 wt value)
  float		x_val;		// X axis value location (center of gaussian bump)
  float		y_val;		// Y axis value location (center of gaussian bump)

  SIMPLE_COPY(FourDValBias);
  TA_BASEFUNS(FourDValBias);
protected:
  SPEC_DEFAULTS;
private:
  void	Initialize();
  void 	Destroy()	{ };
  void	Defaults_init() { };
};

class LEABRA_API FourDValLayerSpec : public LeabraLayerSpec {
  // represents one or more four-d value(s) using a coarse-coded distributed code over units, with units within unit groups representing the Z,W coords and overall unit groups themselves representing X,Y coords.  one val readout is weighted-average; multiple vals = max bumps over 3x3 local grid -- requires FourDValLeabraLayer to encode values (no longer using first row of units)
INHERITED(LeabraLayerSpec)
public:
  FourDValSpec	 fourd;		// specifies how values are represented in terms of distributed patterns of activation across the layer
  MinMaxRange	 x_range;	// range of values represented across the X (horizontal) axis; for GAUSSIAN, add extra values above and below true useful range to prevent edge effects.
  MinMaxRange	 y_range;	// range of values represented across the Y (vertical) axis; for GAUSSIAN, add extra values above and below true useful range to prevent edge effects.
  FourDValBias	 bias_val;	// specifies bias values
  MinMaxRange	 x_val_range;	// #READ_ONLY #NO_INHERIT actual range of values (scalar.min/max taking into account un_range)
  MinMaxRange	 y_val_range;	// #READ_ONLY #NO_INHERIT actual range of values (scalar.min/max taking into account un_range)

  virtual void	ClampValue_ugp(LeabraLayer* lay, Layer::AccessMode acc_md, int gpidx,
                               LeabraNetwork* net, float rescale=1.0f);
  // #CAT_FourDVal clamp value in the first unit's ext field to the units in the group
  virtual void	ReadValue(FourDValLeabraLayer* lay, LeabraNetwork* net);
  // #CAT_FourDVal read out current value represented by activations in layer
    virtual void ReadValue_ugp(FourDValLeabraLayer* lay, Layer::AccessMode acc_md, int gpidx,
                               LeabraNetwork* net);
    // #CAT_FourDVal unit group version: read out current value represented by activations in layer
  virtual void	HardClampExt(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_FourDVal hard clamp current ext values (on all units, after ClampValue called) to all the units (calls ResetAfterClamp)

  virtual void	LabelUnits(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_FourDVal label units in given layer with their underlying values
    virtual void LabelUnits_ugp(LeabraLayer* lay, Layer::AccessMode acc_md, int gpidx);
    // #CAT_FourDVal label units with their underlying values
  virtual void	LabelUnitsNet(LeabraNetwork* net);
  // #BUTTON #CAT_FourDVal label all layers in given network using this spec

  virtual void	Compute_BiasVal(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_FourDVal initialize the bias value 
    virtual void Compute_WtBias_Val(LeabraLayer* lay, Layer::AccessMode acc_md, int gpidx,
                                    float x_val, float y_val);
    // #IGNORE
    virtual void Compute_UnBias_Val(LeabraLayer* lay, Layer::AccessMode acc_md, int gpidx,
                                    float x_val, float y_val);
    // #IGNORE

  override void Init_Weights(LeabraLayer* lay, LeabraNetwork* net);
  override void	Settle_Init_Layer(LeabraLayer* lay, LeabraNetwork* net);
    override void Settle_Init_TargFlags_Layer(LeabraLayer* lay, LeabraNetwork* net);
  override void	Compute_HardClamp(LeabraLayer* lay, LeabraNetwork* net);
  override void	Compute_CycleStats(LeabraLayer* lay, LeabraNetwork* net);
  override void	PostSettle(LeabraLayer* lay, LeabraNetwork* net);
    virtual void PostSettle_ugp(FourDValLeabraLayer* lay, Layer::AccessMode acc_md, int gpidx,
                                LeabraNetwork* net);
    // #CAT_FourDVal unit group version: update variables based on phase

  override float Compute_SSE(LeabraLayer* lay, LeabraNetwork* net, int& n_vals,
			     bool unit_avg = false, bool sqrt = false);
    virtual float Compute_SSE_ugp(LeabraLayer* lay, Layer::AccessMode acc_md, int gpidx,
                                  int& n_vals);
    // #IGNORE
  override float Compute_NormErr(LeabraLayer* lay, LeabraNetwork* net);
    virtual float Compute_NormErr_ugp(LeabraLayer* lay, Layer::AccessMode acc_md, int gpidx,
                                      LeabraInhib* thr, LeabraNetwork* net);
    // #IGNORE

  virtual void	ReConfig(Network* net, int n_units = -1);
  // #BUTTON reconfigure layer and associated specs for current scalar.rep type; if n_units > 0, changes number of units in layer to specified value

  void	HelpConfig();	// #BUTTON get help message for configuring this spec
  bool  CheckConfig_Layer(Layer* lay, bool quiet=false);

  TA_SIMPLE_BASEFUNS(FourDValLayerSpec);
protected:
  void	UpdateAfterEdit_impl();
private:
  void 	Initialize();
  void	Destroy()		{ };
};

*/

//////////////////////////////////////
// 	V1RF Prjn Spec
//////////////////////////////////////

// this is invalidated by rewrite

// class LEABRA_API V1RFPrjnSpec : public ProjectionSpec {
//   // V1 receptive field projection spec: does overlapping tiled receptive fields with Gabor and Blob filter weights - rf_spec.rf_width specifies the width of the receptive field, and rf_move specifies how much to move in input coordinates per each recv group
// INHERITED(ProjectionSpec)
// public:
//   GaborV1SpecBase rf_spec;	// #SHOW_TREE receptive field specs
//   FloatTwoDCoord rf_move;	// how much to move in input coordinates per each receiving layer group
//   bool		wrap;		// if true, then connectivity has a wrap-around structure so it starts at -rf_move (wrapped to right/top) and goes +rf_move past the right/top edge (wrapped to left/bottom)
//   float		dog_surr_mult;	// multiplier on surround weight values for DoG -- can be used to turn off surround entirely or to amplify it

//   TwoDCoord 	 trg_recv_geom;	// #READ_ONLY #SHOW target receiving layer gp geometry -- computed from send and rf_width, move by TrgRecvFmSend button, or given by TrgSendFmRecv
//   TwoDCoord 	 trg_send_geom;	// #READ_ONLY #SHOW target sending layer geometry -- computed from recv and rf_width, move by TrgSendFmRecv button, or given by TrgRecvFmSend
 
//   void 		Connect_impl(Projection* prjn);
//   void		C_Init_Weights(Projection* prjn, RecvCons* cg, Unit* ru);

//   virtual bool	TrgRecvFmSend(int send_x, int send_y);
//   // #BUTTON compute target recv layer geometry based on given sending layer geometry -- updates trg_recv_geom and trg_send_geom members, including fixing send to be an appropriate even multiple of rf_move -- returns true if send values provided result are same "good" ones that come out the end
//   virtual bool	TrgSendFmRecv(int recv_x, int recv_y);
//   // #BUTTON compute target recv layer geometry based on given sending layer geometry -- updates trg_recv_geom and trg_send_geom members, including fixing recv to be an appropriate even multiple of rf_move --  -- returns true if send values provided result are same "good" ones that come out the end

//   virtual void	GraphFilter(DataTable* disp_data, int recv_unit_no);
//   // #BUTTON #NULL_OK plot the filter gaussian into data table and generate a graph of a given unit number's gabor / blob filter
//   virtual void	GridFilter(DataTable* disp_data);
//   // #BUTTON #NULL_OK plot the filter gaussian into data table and generate a grid view of all the gabor or blob filters

//   TA_SIMPLE_BASEFUNS(V1RFPrjnSpec);
// protected:
//   SPEC_DEFAULTS;
//   void	UpdateAfterEdit_impl();
// private:
//   void	Initialize();
//   void 	Destroy()		{ };
//   void	Defaults_init() 	{ };
// };

class LEABRA_API LeabraExtOnlyUnitSpec : public LeabraUnitSpec {
  // only units with an above-threshold (opt_thresh.send) of ext input are allowed to get active -- soft clamping inputs provide a multiplicative mask on the input dynamics -- kind of a special form of sigma-pi unit -- useful e.g., for visual inputs computed by V2 layer, where network dynamics then operate to disambiguate, but cannot hallucinate -- if no ext input, uses first prjn, first con sending act
INHERITED(LeabraUnitSpec)
public:
  
  override void  Compute_NetinInteg(LeabraUnit* u, LeabraNetwork* net, int thread_no);

  TA_BASEFUNS(LeabraExtOnlyUnitSpec);
protected:
  SPEC_DEFAULTS;
private:
  void 	Initialize();
  void	Destroy()		{ };
  void	Defaults_init() { Initialize(); }
};



class LEABRA_API SaliencyPrjnSpec : public ProjectionSpec {
  // Saliency projection spec from V1 layer: receiving layer must have a unit group for each feature, with each unit group having the V1 unit group geometry -- gets excitatory connection from feature corresponding to group index, and from all-but that feature in surrounding sending areas, producing a contrast enhancement effect.  Competition within group and across whole layer produces pop-out dynamics
INHERITED(ProjectionSpec)
public:
  int		convergence;	// how many sending groups to count as a single entity from the perspective of the saliency computation -- values larger than 1 make the saliency layer smaller than the sending layer
  bool		reciprocal;	// opposite direction connection: feedback to the features 
  bool		feat_only;	// only get excitatory input from feature only -- no DoG or anything else -- just pure feature aggregation (for use with kwta-based popout dynamics)
  int		feat_gps;	// number of feature groups contained within V1 unit group -- surround connections are only for within-group connections
  DoGFilter	dog_wts;	// #CONDEDIT_OFF_feat_only Difference of Gaussians filter for setting the weights -- determines the width of the projection, etc -- where the net filter values are positive, receives excitation from the target feature, else from all-but the target feature
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
protected:
  SPEC_DEFAULTS;
private:
  void	Initialize();
  void 	Destroy()		{ };
  void	Defaults_init() 	{ };
};

class LEABRA_API V1LateralContourPrjnSpec : public ProjectionSpec {
  // lateral projections within V1 layer to support continuation of contours -- helps make edges more robust to differences in bottom-up strength, based on principle of good continuation -- requires unit groups within layer where x dim represents angle
INHERITED(ProjectionSpec)
public:
  int		radius;		// #DEF_2:10 how far to connect in any one direction (in unit group units)
  bool		wrap;		// #DEF_true wrap around layer coordinates (else clip at ends)
  float		ang_pow;	// #DEF_4 wt = (angle - feature_angle)^ang_pow -- values > 1 result in a more focal distribution for close angles, and less weight at off-angles.
  float		dist_sigma;	// #DEF_1 sigma for gaussian function of distance -- how much the weight drops off as a function of distance (multiplies angle weights) -- in normalized units relative to the radius
  float		con_thr;	// #DEF_0.2 threshold for making a connection -- weight values below this are not even connected
  float		oth_feat_wt;	// #DEF_0.5 weight multiplier for units that have a different featural encoding (e.g., on-center vs. off-center or another color contrast) -- encoded by y axis of unit group

  override void	Connect_impl(Projection* prjn);
  override void	C_Init_Weights(Projection* prjn, RecvCons* cg, Unit* ru);

  TA_SIMPLE_BASEFUNS(V1LateralContourPrjnSpec);
protected:
  override void UpdateAfterEdit_impl();
private:
  void	Initialize();
  void	Destroy()	{ };
};

class LEABRA_API V2BoLateralPrjnSpec : public ProjectionSpec {
  // lateral projections within V2 layer to support border ownership computation
INHERITED(ProjectionSpec)
public:
  int		radius;		// #DEF_2:10 how far to connect in any one direction (in unit group units)
  bool		wrap;		// #DEF_true wrap around layer coordinates (else clip at ends)
  bool		t_on;		// #DEF_true turn on the special T junction detector weights -- only for a 90 degree angle perpendicular, behind the border edge
  bool		opp_on;		// #DEF_true make connections from opponent border unit (same orientation, opposite BO coding) -- can help to resolve long rectalinear elements
  float		ang_sig;	// #DEF_0.5 sigma for gaussian around target angle -- same for all
  float		dist_sig_line;	// #DEF_0.8 sigma for gaussian distance -- for linear continuation case (delta-angle = 0) -- should in general go longer than for the off-angle cases
  float		dist_sig_oth;	// #DEF_0.5 sigma for gaussian distance -- for other angles (delta-angle != 0) -- should in general go shorter than for the linear case
  float		line_mag;	// #DEF_0.8 magnitude for the linear continuation case -- can actually make this a bit weaker b/c it is non-descriminative
  float		weak_mag;	// #DEF_0.5 weaker magnitude -- applies to acute angle intersections
  float		con_thr;	// #DEF_0.2 threshold for making a connection -- weight values below this are not even connected
  int		test_ang;	// #DEF_-1 #MIN_-1 #MAX_7 set to 0..7, computes only that delta-angle's worth of connectivity (0 = same angle between send and recv, 1 = 45 delta, 2 = 90 delta, 3 = 135 delta) -- useful for visually tuning the receptive field parameters separately

  ///////// use stencils to speed processing
  float_Matrix	v2ffbo_weights;  // #READ_ONLY #NO_SAVE weights for V2 feedforward border ownership inputs from length sum

  virtual float	ConWt(TwoDCoord& suc, int rang_dx, int sang_dx, int rdir, int sdir);
  // connection weight in terms of send unit group coord (suc), recv angle index (0-3 in 45 deg incr), and send angle index (0-3), and r/s bo direction (0-1) -- used for creating stencil
  virtual void	CreateStencils();
  // create stencil -- always done as first step in connection function

  override void	Connect_impl(Projection* prjn);
  override void	C_Init_Weights(Projection* prjn, RecvCons* cg, Unit* ru);

  TA_SIMPLE_BASEFUNS(V2BoLateralPrjnSpec);
protected:
  override void UpdateAfterEdit_impl();
private:
  void	Initialize();
  void	Destroy()	{ };
};


class EMERGENT_API FgBoEllipseGpPrjnEl : public taNBase {
  // #AKA_FgBoGroupingPrjnEl ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Spec one element of a figure-ground border-ownership grouping projection spec -- contains parameters for a specific sized item
INHERITED(taNBase)
public:
  int		con_radius;	// maximum distance for how far to connect in any one direction (in unit group units) -- this then determines most other things
  float		wt_radius;	// distance at which the weight value is maximum -- the target weidth of the grouping detector -- specified as a normalized proportion of the con_radius
  float		dist_sig;	// #DEF_0.2:0.4 sigma for gaussian distance compared to wt_radius target -- in normalized units as a function of wt_radius
  float		ang_sig;	// #DEF_1 sigma for gaussian around target angle -- how widely to connect units around the target angle given by the perpendicular to the radius line
  float		ellipse_ratio;	// #MAX_1 #MIN_0.01 ratio of short over long side for ellpisoid shapes -- values less than 1 produce an ellipse -- long side is always con_radius 
  float		ellipse_angle;	// #CONDSHOW_OFF_ellipse_ratio:1 angle in degrees for the orientation of the long axis of the ellipse -- 0 = horizontal, 90 = vertical, etc.
  float		max_wt;		// magnitude multiplier for all weights -- determines the maximum weight value
  float		min_wt;		// #DEF_0.1 minimum weight value -- weights cannot go below this value -- this is applied after con_thr -- can be useful to retain some weight values to enable subsequent learning
  float		con_thr;	// #DEF_0.2 threshold for making a connection -- weight values below this are not connected -- set to a low value to allow learning

  ///////// use stencils to speed processing
  float_Matrix	fgbo_weights;  // #READ_ONLY #NO_SAVE weights for FgBo projection -- serves as a stencil for the connection

  virtual float	ConWt(TwoDCoord& suc, int sang_dx, int sdir);
  // connection weight in terms of send unit group coord (suc), sending angle index (0-3 in 45 deg incr), and bo direction (0-1) -- used for creating stencil

  virtual void	CreateStencil();
  // create stencil -- always done as first step in connection function

  TA_SIMPLE_BASEFUNS(FgBoEllipseGpPrjnEl);
protected:
  override void UpdateAfterEdit_impl();
private:
  void	Initialize();
  void	Destroy()	{ };
};

class EMERGENT_API FgBoEllipseGpPrjnEl_List : public taList<FgBoEllipseGpPrjnEl> {
  // #AKA_FgBoGroupingPrjnEl_List ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Spec list of FgBoEllipseGpPrjnEl objects
INHERITED(taList<FgBoEllipseGpPrjnEl>)
public:
  TA_BASEFUNS_LITE_NOCOPY(FgBoEllipseGpPrjnEl_List);
private:
  void	Initialize() 		{ };
  void 	Destroy()		{ };
};

class LEABRA_API FgBoEllipseGpPrjnSpec : public ProjectionSpec {
  // #AKA_FgBoGroupingPrjnSpec figure-ground border-ownership grouping projection spec -- 
INHERITED(ProjectionSpec)
public:
  bool		wrap;		// #DEF_true wrap around layer coordinates (else clip at ends)
  bool		reciprocal; 	// set this for connections going the opposite direction, from grouping back to V2 Bo units
  FgBoEllipseGpPrjnEl_List	group_specs; // specifications for each grouping size

  virtual void	CreateStencils();
  // create stencil -- always done as first step in connection function

  override void	Connect_impl(Projection* prjn);
  override void	C_Init_Weights(Projection* prjn, RecvCons* cg, Unit* ru);

  virtual FgBoEllipseGpPrjnEl* NewGroupSpec();
  // #BUTTON create a new group_specs item for specifying one grouping size

  TA_SIMPLE_BASEFUNS(FgBoEllipseGpPrjnSpec);
protected:
  override void UpdateAfterEdit_impl();
private:
  void	Initialize();
  void	Destroy()	{ };
};

class LEABRA_API FgBoWedgeGpPrjnSpec : public TiledGpRFPrjnSpec {
  // TiledGpRFPrjnSpec connectvity with initial weights (when init_wts is set) configured in pattern of 4 1/4 circle wedges, and 4 90 degree straight segments with different border prefs, onto V2 border ownership connections -- recv group size must be 8 x depth where depth is number of depths represented in V2 layer -- 4 units are each quadrant of the wedge
INHERITED(TiledGpRFPrjnSpec)
public:
  float		dist_sigma;	// #CONDEDIT_ON_init_wts #DEF_0.8 sigma for gaussian for distance from center of circle representing the wedge
  float		ang_sigma;	// #CONDEDIT_ON_init_wts #DEF_1 sigma for gaussian around target angle -- how widely to connect units around the target angle given by the perpendicular to the radius line -- controls how tightly tuned the angles are
  float		wt_base;	// #CONDEDIT_ON_init_wts #DEF_0.25 base weight value for all connections (except from other depth, which are not even connected)
  float		wt_range;	// #CONDEDIT_ON_init_wts #DEF_0.5 range of weight values assigned by the gaussian functions of distance and angle, on top of wt_base
  
  float_Matrix	fgbo_weights;  // #READ_ONLY #NO_SAVE weights for FgBo projection -- serves as a stencil for the connection

  virtual float	ConWt_Wedge(int wedge, TwoDCoord& suc, TwoDCoord& su_geo, int sang_dx, int sdir);
  // connection weight in terms of send unit group coord (suc), sending angle index (0-3 in 45 deg incr), and bo direction (0-1) -- used for creating stencil
  virtual float	ConWt_Line(int line, TwoDCoord& suc, TwoDCoord& su_geo, int sang_dx, int sdir);
  // connection weight in terms of send unit group coord (suc), sending angle index (0-3 in 45 deg incr), and bo direction (0-1) -- used for creating stencil

  virtual void	CreateStencil();
  // create stencil -- always done as first step in connection function

  override void Connect_impl(Projection* prjn);
  override void	Connect_UnitGroup(Projection* prjn, Layer* recv_lay, Layer* send_lay,
				  int rgpidx, int sgpidx, int alloc_loop);
  override void	C_Init_Weights(Projection* prjn, RecvCons* cg, Unit* ru);

  TA_SIMPLE_BASEFUNS(FgBoWedgeGpPrjnSpec);
protected:
  override void	UpdateAfterEdit_impl();
private:
  void	Initialize();
  void	Destroy()	{ };
};


class LEABRA_API V1EndStopPrjnSpec : public ProjectionSpec {
  // end-stop detectors within V1 layer -- connectivity and weights that enable units to detect when one orientation terminates into another -- recv layer must have unit groups with one row of n_angles units, while sender has multiple rows of n_angles units (recv integrates over rows)
INHERITED(ProjectionSpec)
public:
  enum LnOrtho {   // line, orthogonal to the line -- for v1s_ang_slopes
    LINE,	   // along the direction of the line
    ORTHO,	   // orthogonal to the line
  };
  enum XY {	   // x, y component of stencils etc -- for clarity in code
    X,
    Y,
  };

  int		n_angles;	// #DEF_4 number of angles in both the input and recv layer
  int		end_stop_dist;	// #DEF_2 end-stop distance factor -- how far away from the central point should we look for opposing orientations
  float		adjang_wt;	// #DEF_0.2 weight for adjacent angles in the end stop computation -- adjacent angles are often activated for edges that are not exactly aligned with the gabor angles, so they can result in false positives
  bool		wrap;		// #DEF_true wrap around layer coordinates (else clip at ends) -- only wrap is currently supported and will be enforced automatically

  float_Matrix	v1s_ang_slopes; // #READ_ONLY #NO_SAVE angle slopes [dx,dy][line,ortho][angles] -- dx, dy slopes for lines and orthogonal lines for each fo the angles
  int_Matrix	v1c_es_stencils;  // #READ_ONLY #NO_SAVE stencils for complex end stop cells [x,y][sum_line=2][max_line=2][angles]
  float_Matrix	v1c_es_angwts;  // #READ_ONLY #NO_SAVE weights for different angles relative to a given angle [n_angles][n_angles]

  override void	Connect_impl(Projection* prjn);
  override void	C_Init_Weights(Projection* prjn, RecvCons* cg, Unit* ru);

  virtual void	InitStencils(Projection* prjn);
  // initialize stencils -- does not depend on prjn, only params (spec can be reused for any prjn)

  TA_SIMPLE_BASEFUNS(V1EndStopPrjnSpec);
protected:
  override void UpdateAfterEdit_impl();
private:
  void	Initialize();
  void	Destroy()	{ };
};

class LEABRA_API VisDisparityPrjnSpec : public ProjectionSpec {
  // visual disparity projection spec: receiving layer units within groups encode different offset disparities (near..far) from two sending layers (first prjn MUST be right eye, second MUST be left eye -- right is just one-to-one dominant driver) -- should have same gp_geom as sending layer gp_geom -- features within sending gps are replicated for each disparity -- MUST only have one of these per configuration of sending / recv layers, as local data is stored and cached from connection for use in initweights
INHERITED(ProjectionSpec)
public:
  int		n_disps;	// #DEF_0:3 number of different disparities encoded in each direction away from the focal plane (e.g., 1 = -1 near, 0 = focal, +1 far) -- each disparity tuned cell responds to a range of actual disparities around a central value, defined by disp * disp_off
  float		disp_range_pct;  // #DEF_0.02:0.1 range (half width) of disparity tuning around central offset value for each disparity cell -- expressed as proportion of total input image width -- total disparity tuning width for each cell is 2*disp_range + 1, and activation is weighted by gaussian tuning over this range (see gauss_sig)
  float		gauss_sig; 	// #CONDEDIT_ON_init_wts #DEF_0.7:1.5 gaussian sigma for weighting the contribution of different disparities over the disp_range -- expressed as a proportion of disp_range -- last disparity on near/far ends does not come back down from peak gaussian value (ramp to plateau instead of gaussian)
  float		disp_spacing;	// #DEF_2:2.5 spacing between different disparity detector cells in terms of disparity offset tunings -- expressed as a multiplier on disp_range -- this should generally remain the default value of 2, so that the space is properly covered by the different disparity detectors, but 2.5 can also be useful to not have any overlap between disparities to prevent ambiguous activations (e.g., for figure-ground segregation)
  int		end_extra;	// #DEF_2 extra disparity detecting range on the ends of the disparity spectrum (nearest and farthest detector cells) -- adds beyond the disp_range -- to extend out and capture all reasonable disparities -- expressed as a multiplier on disp_range 
  bool		wrap;		// if true, then connectivity has a wrap-around structure so it starts at -rf_move (wrapped to right/top) and goes +rf_move past the right/top edge (wrapped to left/bottom)

  int		disp_range; 	// #READ_ONLY #SHOW range (half width) of disparity tuning around central offset value for each disparity cell -- integer value computed from disp_range_pct -- total disparity tuning width for each cell is 2*disp_range + 1, and activation is weighted by gaussian tuning over this range (see gauss_sig)
  int		disp_spc;	// #READ_ONLY #SHOW integer value of spacing between different disparity detector cells -- computed from disp_spacing and disp_range
  int		end_ext;	// #READ_ONLY #SHOW integer value of extra disparity detecting range on the ends of the disparity spectrum (nearest and farthest detector cells) -- adds beyond the disp_range -- to extend out and capture all reasonable disparities

  int		tot_disps;	// #READ_ONLY total number of disparities coded: 1 + 2 * n_disps
  int		max_width;	// #READ_ONLY maximum total width (1 + 2 * disp_range + end_ext)
  int		max_off;	// #READ_ONLY maximum possible offset -- furthest point out in any of the stencils
  int		tot_offs;	// #READ_ONLY 1 + 2 * max_off

  int_Matrix	v1b_widths; 	// #READ_ONLY #NO_SAVE width of stencils for binocularity detectors 1d: [tot_disps]
  float_Matrix	v1b_weights;	// #READ_ONLY #NO_SAVE v1 binocular gaussian weighting factors for integrating disparity values into v1b unit activations -- for each tuning disparity [max_width][tot_disps] -- only v1b_widths[disp] are used per disparity
  int_Matrix	v1b_stencils; 	// #READ_ONLY #NO_SAVE stencils for binocularity detectors, in terms of v1s location offsets per image: 2d: [XY][max_width][tot_disps]

  override void Connect_impl(Projection* prjn);
  override void	C_Init_Weights(Projection* prjn, RecvCons* cg, Unit* ru);

  virtual void InitStencils(Projection* prjn);
  // initialize the stencils for given geometry of layers for this projection -- called at Connect_LeftEye
  virtual void Connect_RightEye(Projection* prjn);
  // right eye is simple replicated one-to-one connectivity
  virtual void Connect_LeftEye(Projection* prjn);
  // left eye has all the disparity integration connections

  virtual void	UpdateFmV1sSize(int v1s_img_x) {
    disp_range = (int)((disp_range_pct * (float)v1s_img_x) + 0.5f);
    disp_spc = (int)(disp_spacing * (float)disp_range);
    end_ext = end_extra * disp_range;
    max_width = 1 + 2*disp_range + end_ext;
    max_off = n_disps * disp_spc + disp_range + end_ext;
    tot_offs = 1 + 2 * max_off;
  }

  TA_SIMPLE_BASEFUNS(VisDisparityPrjnSpec);
protected:
  override void UpdateAfterEdit_impl();
private:
  void	Initialize();
  void 	Destroy()		{ };
};


class LEABRA_API VisDispLaySpec : public taOBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra visual disparity layer specs
INHERITED(taOBase)
public:
  bool		max_l;		// #DEF_true compute left = max(weighted left inputs)
  bool		incl_other_res;	// #DEF_true include other resolution layers that are connected to this one -- looks for VisDisparityLayerSpec layers and multiplies by their misc_1 values
  int		updt_cycles;	// #DEF_-1:5 (-1=always) how many cycles to compute updates for -- computation is expensive so if inputs are static, don't keep computing beyond what is necessary

  // todo: add stuff about horiz thign

  TA_SIMPLE_BASEFUNS(VisDispLaySpec);
private:
  void	Initialize();
  void	Destroy()	{ };
};

class LEABRA_API VisDisparityLayerSpec : public LeabraLayerSpec {
  // visual disparity layer spec: receiving layer units within groups encode different offset disparities (near..far) from two sending layers (first prjn MUST be VisDisparityPrjnSpec from right eye, second MUST be same spec type from left eye -- right is just one-to-one dominant driver) -- this layerspec computes sqrt(left*right) activation into ext of units, which can be added into netinput if !clamp.hard, or activation forced to this value otherwise -- also manages horizontal apeture problem -- use MarkerConSpec for these prjs to prevent computation otherwise
INHERITED(LeabraLayerSpec)
public:
  VisDispLaySpec	disp;	// disparity computation specs

  virtual void	ComputeDispToExt(LeabraLayer* lay, LeabraNetwork* net);
  // main function: compute disparity values into ext in units

  // these are two entry points for applying ext inputs depending on clamp.hard status
  override void	Compute_ExtraNetin(LeabraLayer* lay, LeabraNetwork* net);
  override void Compute_CycleStats(LeabraLayer* lay, LeabraNetwork* net);

  override bool  CheckConfig_Layer(Layer* lay, bool quiet=false);

  TA_SIMPLE_BASEFUNS(VisDisparityLayerSpec);
// protected:
//   override void UpdateAfterEdit_impl();
private:
  void	Initialize();
  void 	Destroy()		{ };
};

class LEABRA_API TiledGpRFOneToOnePrjnSpec : public TiledGpRFPrjnSpec {
  // TiledGpRFPrjnSpec connectvity with one-to-one connections for units with the same index within a unit group -- useful for establishing connections among layers with the same unit-group structure (see also TiledGpRFOneToOneWtsPrjnSpec for a softer version where only weights are set)
INHERITED(TiledGpRFPrjnSpec)
public:
  float		gauss_sigma;		// #CONDEDIT_ON_init_wts gaussian width parameter for initial weight values (only with init_wts on) to give a tuning curve in terms of distance from center of overall rf (normalized units)
  int		su_idx_st;		// starting sending unit index within each unit group to start connecting from -- allows for layers to have diff unit group structure
  int		ru_idx_st;		// starting receiving unit index within each unit group to start connecting from -- allows for layers to have diff unit group structure
  int		gp_n_cons;		// [-1 = all] number of units to connect within each group -- allows for layers to have diff unit group structure

  override void	Connect_UnitGroup(Projection* prjn, Layer* recv_lay, Layer* send_lay,
				  int rgpidx, int sgpidx, int alloc_loop);
  override void	C_Init_Weights(Projection* prjn, RecvCons* cg, Unit* ru);

  TA_SIMPLE_BASEFUNS(TiledGpRFOneToOnePrjnSpec);
private:
  void	Initialize();
  void	Destroy()	{ };
};


class LEABRA_API TiledGpRFOneToOneWtsPrjnSpec : public TiledGpRFPrjnSpec {
  // TiledGpRFPrjnSpec connectvity with initial weights (when init_wts is set) that have differential weights for units with the same index within a unit group vs. differential weights -- useful for establishing connections among layers with the same unit-group structure (see also TiledGpRFOnetToOnePrjnSpec for harder version where connections are only made among units with same index within group)
INHERITED(TiledGpRFPrjnSpec)
public:
  float		one_to_one_wt;	// #CONDEDIT_ON_init_wts weight between units with the same index in the unit group
  float		other_wt;	// #CONDEDIT_ON_init_wts weight between other non-one-to-one units

  override void	C_Init_Weights(Projection* prjn, RecvCons* cg, Unit* ru);

  TA_SIMPLE_BASEFUNS(TiledGpRFOneToOneWtsPrjnSpec);
private:
  void	Initialize();
  void	Destroy()	{ };
};

class LEABRA_API V2toV4DepthPrjnSpec : public TiledGpRFPrjnSpec {
  // TiledGpRFPrjnSpec that only connects to a specific depth in V2 border ownership input layer
INHERITED(TiledGpRFPrjnSpec)
public:
  int		depth_idx;	// which depth index (0..) to connect to

  override void	Connect_UnitGroup(Projection* prjn, Layer* recv_lay, Layer* send_lay,
				  int rgpidx, int sgpidx, int alloc_loop);

  TA_SIMPLE_BASEFUNS(V2toV4DepthPrjnSpec);
private:
  void	Initialize();
  void	Destroy()	{ };
};

class LEABRA_API V1FeatInhibSpec : public SpecMemberBase {
  // ##INLINE ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra specifies inhibition parameters for feature inhibition
INHERITED(SpecMemberBase)
public:
  bool		on;		// #DEF_true whether to use neighborhood inhibition
  int		n_angles;	// #CONDSHOW_ON_on #DEF_4 number of different angles represented in the layer unit groups -- should be equivalent to x-axis dimension of number of units in the unit group
  int		inhib_d; 	// #CONDSHOW_ON_on #DEF_1 distance of neighborhood for inhibition to apply to same feature in neighboring locations spreading out on either side along the orthogonal direction relative to the orientation tuning
  float		inhib_g;	// #CONDSHOW_ON_on #DEF_0.8 gain factor for feature-specific inhibition from neighbors -- this proportion of the neighboring feature's threshold-inhibition value (used in computing kwta) is spread among neighbors according to inhib_d distance
  bool		wrap;		// #CONDSHOW_ON_on #DEF_true wrap around effective connectivity on sides and top/bottom 

  int		tot_ni_len;	// #READ_ONLY total length of neighborhood inhibition stencils = 2 * neigh_inhib_d + 1

  TA_SIMPLE_BASEFUNS(V1FeatInhibSpec);
protected:
  SPEC_DEFAULTS;
  void 	UpdateAfterEdit_impl();
private:
  void	Initialize();
  void 	Destroy()	{ };
  void	Defaults_init() { Initialize(); }
};

class LEABRA_API LeabraV1LayerSpec : public LeabraLayerSpec {
  // LayerSpec that implements competition both within unit groups and among features in nearby unit groups, where a feature is defined as a specific unit position within the unit groups (layer must have unit groups) -- inhibition is oriented according to x axis of feature position, assuming that this encodes orientation angle (0 = horiz), and acts just like the gp_i, gp_g form of inhibition
INHERITED(LeabraLayerSpec)
public:
  enum XY {	   // x, y component of stencils etc -- for clarity in code
    X,
    Y,
  };
  enum LnOrtho {   // line, orthogonal to the line -- for v1s_ang_slopes
    LINE,	   // along the direction of the line
    ORTHO,	   // orthogonal to the line
  };

  V1FeatInhibSpec	feat_inhib; // feature-level inhibition parameters
  float_Matrix		v1s_ang_slopes; // #READ_ONLY #NO_SAVE angle slopes [dx,dy][line,ortho][angles] -- dx, dy slopes for lines and orthogonal lines for each of the angles
  int_Matrix		v1s_ni_stencils; // #READ_ONLY #NO_SAVE stencils for neighborhood inhibition [x,y][tot_ni_len][angles]

  virtual void	UpdateStencils();
  // update the stencils that determine the topology of neighborhood connections based on orientation

  virtual void	Compute_FeatGpActive(LeabraLayer* lay, LeabraUnit_Group* fug, LeabraNetwork* net);
  // compute active units in active_buf for given feature unit group

  override void	Compute_ApplyInhib(LeabraLayer* lay, LeabraNetwork* net);

  override bool CheckConfig_Layer(Layer* lay, bool quiet=false);
  
  TA_SIMPLE_BASEFUNS(LeabraV1LayerSpec);
protected:
  SPEC_DEFAULTS;
  void 	UpdateAfterEdit_impl();
private:
  void	Initialize();
  void 	Destroy()		{ };
  void	Defaults_init() 	{ };
};


///////////////////////////////////////////////////////////////////
//	Cerebellum-related special guys

class LEABRA_API CerebConj2PrjnSpec : public ProjectionSpec {
  // cerebellar-inspired conjunctive projection spec, 2nd order conjunctions between two topographic input maps -- first one in layer prjn is outer group (across unit groups), 2nd one is inner group (within unit groups)
INHERITED(ProjectionSpec)
public:
  TwoDCoord 	 rf_width;	// size of the receptive field -- should be an even number
  FloatTwoDCoord rf_move;	// how much to move in input coordinates per each receiving increment (unit group or unit within group, depending on whether inner or outer) -- typically 1/2 rf_width
  float		gauss_sigma;	// #CONDEDIT_ON_init_wts gaussian width parameter for initial weight values to give a tuning curve
  bool		wrap;		// if true, then connectivity has a wrap-around structure so it starts at -rf_move (wrapped to right/top) and goes +rf_move past the right/top edge (wrapped to left/bottom)

  TwoDCoord 	 trg_recv_geom;	// #READ_ONLY #SHOW target receiving layer geometry (either gp or unit, depending on outer vs. inner) -- computed from send and rf_width, move by TrgRecvFmSend button, or given by TrgSendFmRecv
  TwoDCoord 	 trg_send_geom;	// #READ_ONLY #SHOW target sending layer geometry -- computed from recv and rf_width, move by TrgSendFmRecv button, or given by TrgRecvFmSend

  virtual  void Connect_Inner(Projection* prjn);
  // inner connect: unit position within the unit group determines sender location
  virtual  void Connect_Outer(Projection* prjn);
  // outer connect: unit_group position determines sender location

  override void Connect_impl(Projection* prjn);
  override void	C_Init_Weights(Projection* prjn, RecvCons* cg, Unit* ru);

  virtual bool	TrgRecvFmSend(int send_x, int send_y);
  // #BUTTON compute target recv layer geometry based on given sending layer geometry -- updates trg_recv_geom and trg_send_geom members, including fixing send to be an appropriate even multiple of rf_move -- returns true if send values provided result are same "good" ones that come out the end
  virtual bool	TrgSendFmRecv(int recv_x, int recv_y);
  // #BUTTON compute target recv layer geometry based on given sending layer geometry -- updates trg_recv_geom and trg_send_geom members, including fixing recv to be an appropriate even multiple of rf_move --  -- returns true if send values provided result are same "good" ones that come out the end

  TA_SIMPLE_BASEFUNS(CerebConj2PrjnSpec);
protected:
  SPEC_DEFAULTS;
private:
  void	Initialize();
  void 	Destroy()		{ };
  void	Defaults_init() 	{ };
};


#endif // leabra_extra_h

