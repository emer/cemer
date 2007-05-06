// Copyright, 1995-2005, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of TA/PDP++
//
//   TA/PDP++ is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   TA/PDP++ is distributed in the hope that it will be useful,
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
  inline float 	Compute_Netin(RecvCons*, Unit*) { return 0.0f; }
  inline void 	Send_Netin(RecvCons*, Unit*) { };
  inline void 	Send_NetinDelta(LeabraRecvCons*, LeabraUnit*) { };
  inline void 	Compute_dWt(RecvCons*, Unit*) { };
  inline void	Compute_Weights(RecvCons*, Unit*) { };

  bool	 DMem_AlwaysLocal() { return true; }
  // these connections always need to be there on all nodes..

  void 	Initialize();
  void	Destroy()		{ };
  TA_BASEFUNS(MarkerConSpec);
};


//////////////////////////////////////////
// 	Context Layer for Sequential	//
//////////////////////////////////////////

class LEABRA_API CtxtUpdateSpec : public taBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra context updating specifications
  INHERITED(taBase)
public:
  float		fm_hid;		// from hidden (inputs to context layer)
  float		fm_prv;		// from previous context layer values (maintenance)
  float		to_out;		// outputs from context layer

  void	Initialize();
  void 	Destroy()	{ };
  SIMPLE_COPY(CtxtUpdateSpec);
  TA_BASEFUNS(CtxtUpdateSpec);
};

class LEABRA_API LeabraContextLayerSpec : public LeabraLayerSpec {
  // context layer that copies from its recv projection (like an input layer)
  INHERITED(LeabraLayerSpec)
public:
  CtxtUpdateSpec updt;		// ctxt updating constants: from hidden, from previous values (hysteresis), outputs from context (n/a on simple gate layer)

  void	Compute_HardClamp(LeabraLayer* lay, LeabraNetwork* net);
  // clamp from act_p values of sending layer

  virtual void Compute_Context(LeabraLayer* lay, LeabraUnit* u, LeabraNetwork* net);
  // get context source value for given context unit

  override bool  CheckConfig_Layer(LeabraLayer* lay, bool quiet=false);

  void	Defaults();

  void 	Initialize();
  void	Destroy()		{ };
  void	InitLinks();
  SIMPLE_COPY(LeabraContextLayerSpec);
  TA_BASEFUNS(LeabraContextLayerSpec);
};

//////////////////////////////////////////
// 	Misc Special Objects		//
//////////////////////////////////////////

class LEABRA_API LeabraLinUnitSpec : public LeabraUnitSpec {
  // a pure linear unit (suitable for an AC unit spec unit)
  INHERITED(LeabraUnitSpec)
public:
  void 	Compute_ActFmVm(LeabraUnit* u, LeabraLayer* lay, LeabraInhib* thr, LeabraNetwork* net);
  
  void	Defaults();

  void 	Initialize();
  void	Destroy()		{ };
  TA_BASEFUNS(LeabraLinUnitSpec);
};

class LEABRA_API LeabraNegBiasSpec : public LeabraBiasSpec {
  // only learns negative bias changes, not positive ones (decay restores back to zero)
  INHERITED(LeabraBiasSpec)
public:
  float		decay;		// rate of weight decay towards zero 
  bool		updt_immed;	// update weights immediately when weights are changed

  inline void	B_Compute_Weights(LeabraCon* cn, LeabraUnit* ru, LeabraUnitSpec* rus) {
    if(rus->act_reg.on) {		// do this in update so inactive units can be reached (no opt_thresh.updt)
      if(ru->act_avg < rus->act_reg.min)
	cn->dwt += cur_lrate * rus->act_reg.wt_dt;
      else if(ru->act_avg > rus->act_reg.max)
	cn->dwt -= cur_lrate * rus->act_reg.wt_dt;
    }
    if(cn->dwt > 0.0f)		// positive only
      cn->dwt = 0.0f;
    cn->dwt -= decay * cn->wt;
    cn->pdw = cn->dwt;
    cn->wt += cn->dwt;
    cn->dwt = 0.0f;
    C_ApplyLimits(cn, ru, NULL);
  }

  inline void	B_Compute_dWt(LeabraCon* cn, LeabraUnit* ru) {
    LeabraBiasSpec::B_Compute_dWt(cn, ru);
    if(updt_immed) B_Compute_Weights(cn, ru, (LeabraUnitSpec*)ru->GetUnitSpec());
  }

  void 	Initialize();
  void	Destroy()		{ };
  SIMPLE_COPY(LeabraNegBiasSpec);
  TA_BASEFUNS(LeabraNegBiasSpec);
};

class LEABRA_API TrialSynDepCon : public LeabraCon {
  // synaptic depression connection at the trial level (as opposed to cycle level)
  INHERITED(LeabraCon)
public:
  float		effwt;		// #NO_SAVE effective weight value (can be depressed) -- used for sending ativation

  TrialSynDepCon() { effwt = 0.0f; }
};

class LEABRA_API SynDepSpec : public taBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra specs for synaptic depression
  INHERITED(taBase)
public:
  float		rec;		// #DEF_1 rate of recovery from depression
  float		depl;		// #DEF_1.1 rate of depletion of synaptic efficacy as a function of sender-receiver activations

  void	Initialize();
  void 	Destroy()	{ };
  SIMPLE_COPY(SynDepSpec);
  TA_BASEFUNS(SynDepSpec);
};

class LEABRA_API TrialSynDepConSpec : public LeabraConSpec {
  // synaptic depression connection at the trial level (as opposed to cycle level)
  INHERITED(LeabraConSpec)
public:
  SynDepSpec	syn_dep;	// synaptic depression specifications

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

  void Compute_dWt(RecvCons* cg, Unit* ru) {
    LeabraUnit* lru = (LeabraUnit*)ru;
    LeabraRecvCons* lcg = (LeabraRecvCons*) cg;
    Compute_SAvgCor(lcg, lru);
    if(((LeabraLayer*)cg->prjn->from)->acts_p.avg >= savg_cor.thresh) {
      for(int i=0; i<cg->cons.size; i++) {
	LeabraUnit* su = (LeabraUnit*)cg->Un(i);
	LeabraCon* cn = (LeabraCon*)cg->Cn(i);
	if(!(su->in_subgp &&
	     (((LeabraUnit_Group*)su->owner)->acts_p.avg < savg_cor.thresh))) {
	  float orig_wt = cn->wt;
	  C_Compute_LinFmWt(lcg, cn); // get weight into linear form
	  C_Compute_dWt(cn, lru, 
			C_Compute_Hebb(cn, lcg, lru->act_p, su->act_p),
			C_Compute_Err(cn, lru->act_p, lru->act_m, su->act_p, su->act_m));  
	  cn->wt = orig_wt; // restore original value; note: no need to convert there-and-back for dwt, saves numerical lossage!
	  // depression operates on nonlinear weight!
	  C_Depress_Wt((TrialSynDepCon*)cn, lru, su);
	}
      }
    }
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

  void 	C_Init_Weights_Post(RecvCons*, Connection* cn, Unit*, Unit*) {
    TrialSynDepCon* lcn = (TrialSynDepCon*)cn; lcn->effwt = lcn->wt;
  }

  float C_Compute_Netin(TrialSynDepCon* cn, Unit*, Unit* su) {
    return cn->effwt * su->act;
  }
  float Compute_Netin(RecvCons* cg, Unit* ru) {
    float rval=0.0f;
    CON_GROUP_LOOP(cg, rval += C_Compute_Netin((TrialSynDepCon*)cg->Cn(i), ru, cg->Un(i)));
    return ((LeabraRecvCons*)cg)->scale_eff * rval;
  }
  void C_Send_Inhib(LeabraSendCons* cg, TrialSynDepCon* cn, LeabraUnit* ru, LeabraUnit* su) {
    ru->gc.i += ((LeabraRecvCons*)ru->recv.FastEl(cg->recv_idx))->scale_eff * su->act * cn->effwt;
  }
  void Send_Inhib(LeabraSendCons* cg, LeabraUnit* su) {
    CON_GROUP_LOOP(cg, C_Send_Inhib(cg, (TrialSynDepCon*)cg->Cn(i), (LeabraUnit*)cg->Un(i), su));
  }
  void C_Send_Netin(LeabraSendCons* cg, TrialSynDepCon* cn, Unit* ru, Unit* su) {
    ru->net += ((LeabraRecvCons*)ru->recv.FastEl(cg->recv_idx))->scale_eff * su->act * cn->effwt;
  }
  void Send_Netin(SendCons* cg, Unit* su) {
    if(inhib)
      Send_Inhib((LeabraSendCons*)cg, (LeabraUnit*)su);
    else {
      CON_GROUP_LOOP(cg, C_Send_Netin((LeabraSendCons*)cg, (TrialSynDepCon*)cg->Cn(i), cg->Un(i), su));
    }
  }
  void C_Send_InhibDelta(LeabraSendCons* cg, TrialSynDepCon* cn, LeabraUnit* ru, LeabraUnit* su) {
    ru->g_i_delta += ((LeabraRecvCons*)ru->recv.FastEl(cg->recv_idx))->scale_eff * su->act_delta * cn->effwt;
  }
  void Send_InhibDelta(LeabraSendCons* cg, LeabraUnit* su) {
    CON_GROUP_LOOP(cg, C_Send_InhibDelta(cg, (TrialSynDepCon*)cg->Cn(i), (LeabraUnit*)cg->Un(i), su));
  }
  void C_Send_NetinDelta(LeabraSendCons* cg, TrialSynDepCon* cn, LeabraUnit* ru, LeabraUnit* su) {
    ru->net_delta += ((LeabraRecvCons*)ru->recv.FastEl(cg->recv_idx))->scale_eff * su->act_delta * cn->effwt;
  }
  void Send_NetinDelta(LeabraSendCons* cg, LeabraUnit* su) {
    if(inhib)
      Send_InhibDelta(cg, su);
    else {
      CON_GROUP_LOOP(cg, C_Send_NetinDelta(cg, (TrialSynDepCon*)cg->Cn(i), (LeabraUnit*)cg->Un(i), su));
    }
  }
  void C_Send_ClampNet(LeabraSendCons* cg, TrialSynDepCon* cn, LeabraUnit* ru, LeabraUnit* su) {
    ru->clmp_net += ((LeabraRecvCons*)ru->recv.FastEl(cg->recv_idx))->scale_eff * su->act * cn->effwt;
  }
  void Send_ClampNet(LeabraSendCons* cg, LeabraUnit* su) {
    CON_GROUP_LOOP(cg, C_Send_ClampNet(cg, (TrialSynDepCon*)cg->Cn(i), (LeabraUnit*)cg->Un(i), su));
  }

  void 	Initialize();
  void	Destroy()		{ };
  void  InitLinks();
  SIMPLE_COPY(TrialSynDepConSpec);
  TA_BASEFUNS(TrialSynDepConSpec);
protected:
  void 	UpdateAfterEdit_impl();

};

class LEABRA_API FastWtCon : public LeabraCon {
  // fast weight connection: standard wt learns fast, but decays toward slow weight value
public:
  float		swt;		// slow weight value
  float		sdwt;		// #NO_SAVE slow weight delta-weight change

  FastWtCon() { swt = sdwt = 0.0f; }
};

class LEABRA_API FastWtSpec : public taBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra specificiations for fast weights
  INHERITED(taBase)
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

  void	Initialize();
  void	Destroy()	{ };
  SIMPLE_COPY(FastWtSpec);
  TA_BASEFUNS(FastWtSpec);
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

  void 		C_Init_Weights_Post(RecvCons*, Connection* cn, Unit*, Unit*) {
    FastWtCon* lcn = (FastWtCon*)cn; lcn->swt = lcn->wt;
  }

  void 		C_Init_dWt(RecvCons* cg, Connection* cn, Unit* ru, Unit* su)
  { ConSpec::C_Init_dWt(cg, cn, ru, su); ((FastWtCon*)cn)->sdwt=0.0f; }

  void SetCurLrate(int epoch, LeabraNetwork* net);

  inline float C_Compute_SlowHebb(FastWtCon* cn, LeabraRecvCons* cg,
				  float ru_act, float su_act)
  {
    // swt is always stored positive, so signs are different.
    float swt_eff = (fast_wt.slw_sat) ? -cn->wt : cn->swt;
    return ru_act * (su_act * (cg->savg_cor - swt_eff) - (1.0f - su_act) * swt_eff);
  }

  // todo: somewhat inefficient to do this computation twice..

  // generec error term with sigmoid activation function, and soft bounding
  inline float C_Compute_SlowErr(FastWtCon* cn, float ru_act_p, float ru_act_m, float su_act_p, float su_act_m) {
    float err = (ru_act_p * su_act_p) - (ru_act_m * su_act_m);
    float swt_eff = (fast_wt.slw_sat) ? -cn->wt : cn->swt;
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

  inline void Compute_dWt(RecvCons* cg, Unit* ru) {
    LeabraUnit* lru = (LeabraUnit*)ru;
    LeabraRecvCons* lcg = (LeabraRecvCons*) cg;
    Compute_SAvgCor(lcg, lru);
    if(((LeabraLayer*)cg->prjn->from)->acts_p.avg >= savg_cor.thresh) {
      for(int i=0; i<cg->cons.size; i++) {
	LeabraUnit* su = (LeabraUnit*)cg->Un(i);
	FastWtCon* cn = (FastWtCon*)cg->Cn(i);
	if(!(su->in_subgp &&
	     (((LeabraUnit_Group*)su->owner)->acts_p.avg < savg_cor.thresh))) {
	  C_Compute_FastDecay(cn, lru, su);
	  float orig_wt = cn->wt;
	  C_Compute_LinFmWt(lcg, cn); // get weight into linear form
	  C_Compute_dWt(cn, lru, 
			C_Compute_Hebb(cn, lcg, lru->act_p, su->act_p),
			C_Compute_Err(cn, lru->act_p, lru->act_m, su->act_p, su->act_m));  
	  C_Compute_SlowdWt(cn, lru, 
			C_Compute_SlowHebb(cn, lcg, lru->act_p, su->act_p),
			C_Compute_SlowErr(cn, lru->act_p, lru->act_m, su->act_p, su->act_m));  
	  cn->wt = orig_wt; // restore original value; note: no need to convert there-and-back for dwt, saves numerical lossage!
	}
      }
    }
  }

  inline void C_Compute_Weights(FastWtCon* cn, LeabraRecvCons* cg,
			      LeabraUnit*, LeabraUnit*, LeabraUnitSpec*)
  {
    if(cn->sdwt != 0.0f) {
      cn->swt += cn->sdwt; // wt is not negative!
      if(cn->swt < wt_limits.min) cn->swt = wt_limits.min;
      if(cn->swt > wt_limits.max) cn->swt = wt_limits.max;
    }
    if(cn->dwt != 0.0f) {
      // always do this because of the decay term..
      C_Compute_LinFmWt(cg, cn);	// go to linear weights
      cn->wt -= cn->dwt; // wt is now negative in linear form -- signs are reversed!
      cn->wt = MAX(cn->wt, -1.0f);	// limit 0-1
      cn->wt = MIN(cn->wt, 0.0f);
      C_Compute_WtFmLin(cg, cn);	// go back to nonlinear weights
      // then put in real limits!!
      if(cn->wt < wt_limits.min) cn->wt = wt_limits.min;
      if(cn->wt > wt_limits.max) cn->wt = wt_limits.max;
    }
    cn->pdw = cn->dwt;
    cn->dwt = 0.0f;
    cn->sdwt = 0.0f;
  }

  inline void C_Compute_WeightsActReg(FastWtCon* cn, LeabraRecvCons* cg,
				    LeabraUnit* ru, LeabraUnit* su, LeabraUnitSpec* rus)
  {
    C_Compute_ActReg(cn, cg, ru, su, rus);
    C_Compute_Weights(cn, cg, ru, su, rus);
  }

  inline void Compute_Weights(RecvCons* cg, Unit* ru) {
    LeabraUnitSpec* rus = (LeabraUnitSpec*)ru->GetUnitSpec();
    LeabraRecvCons* lcg = (LeabraRecvCons*)cg;
    if(rus->act_reg.on) {		// do this in update so inactive units can be reached (no opt_thresh.updt)
      CON_GROUP_LOOP(cg, C_Compute_WeightsActReg((FastWtCon*)cg->Cn(i), lcg,
					       (LeabraUnit*)ru, (LeabraUnit*)cg->Un(i), rus));
    }
    else {
      CON_GROUP_LOOP(cg, C_Compute_Weights((FastWtCon*)cg->Cn(i), lcg,
					 (LeabraUnit*)ru, (LeabraUnit*)cg->Un(i), rus));
    }
    //  ApplyLimits(cg, ru); limits are automatically enforced anyway
  }

  void 	Initialize();
  void	Destroy()		{ };
  void  InitLinks();
  SIMPLE_COPY(FastWtConSpec);
  TA_BASEFUNS(FastWtConSpec);
};

class LEABRA_API ActAvgHebbMixSpec : public taBase {
  // ##INLINE ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra mixture of average activation hebbian learning and regular hebbian learning (on current act value)
  INHERITED(taBase)
public:
  float		act_avg;	// what proportion of average activation to include in hebbian receiving unit activation learning term
  float		cur_act;	// #READ_ONLY #SHOW 1.0 - act_avg -- proportion of current activation for hebbian learning

  void	Initialize();
  void	Destroy()	{ };
  SIMPLE_COPY(ActAvgHebbMixSpec);
  TA_BASEFUNS(ActAvgHebbMixSpec);
protected:
  void	UpdateAfterEdit_impl();
};

class LEABRA_API ActAvgHebbConSpec : public LeabraConSpec {
  // hebbian learning that includes a proportion of average activation over time, in addition to standard current unit activation;  produces a trace-based learning effect for learning over trajectories
  INHERITED(LeabraConSpec)
public:
  ActAvgHebbMixSpec	act_avg_hebb; // mixture of current and average activations to use in hebbian learning

  inline float C_Compute_Hebb(LeabraCon* cn, LeabraRecvCons* cg,
			      float ru_act, float su_act, float ru_avg_act)
  {
    // wt is negative in linear form, so using opposite sign of usual here
    float eff_ru_act = act_avg_hebb.act_avg * ru_avg_act + act_avg_hebb.cur_act * ru_act;
    return eff_ru_act * (su_act * (cg->savg_cor + cn->wt) +
			 (1.0f - su_act) * cn->wt);
  }

  // this computes weight changes based on sender at time t-1
  inline void Compute_dWt(RecvCons* cg, Unit* ru) {
    LeabraUnit* lru = (LeabraUnit*)ru;
    LeabraRecvCons* lcg = (LeabraRecvCons*) cg;
    Compute_SAvgCor(lcg, lru);
    if(((LeabraLayer*)cg->prjn->from)->acts_p.avg >= savg_cor.thresh) {
      for(int i=0; i<cg->cons.size; i++) {
	LeabraUnit* su = (LeabraUnit*)cg->Un(i);
	LeabraCon* cn = (LeabraCon*)cg->Cn(i);
	if(!(su->in_subgp &&
	     (((LeabraUnit_Group*)su->owner)->acts_p.avg < savg_cor.thresh))) {
	  float orig_wt = cn->wt;
	  C_Compute_LinFmWt(lcg, cn); // get weight into linear form
	  C_Compute_dWt(cn, lru, 
			C_Compute_Hebb(cn, lcg, lru->act_p, su->act_p, lru->act_avg),
			C_Compute_Err(cn, lru->act_p, lru->act_m, su->act_p, su->act_m));  
	  cn->wt = orig_wt; // restore original value; note: no need to convert there-and-back for dwt, saves numerical lossage!
	}
      }
    }
  }

  void 	Initialize();
  void	Destroy()		{ };
  void	InitLinks();
  SIMPLE_COPY(ActAvgHebbConSpec);
  TA_BASEFUNS(ActAvgHebbConSpec);
protected:
  void	UpdateAfterEdit_impl();
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

class LEABRA_API ScalarValSpec : public taBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra specs for scalar values
  INHERITED(taBase)
public:
  enum	RepType {
    GAUSSIAN,			// gaussian bump, with value = weighted average of tuned unit values
    LOCALIST,			// each unit represents a distinct value; intermediate values represented by graded activity of neighbors; overall activity is weighted-average across all units
  };

  RepType	rep;		// #APPLY_IMMED type of representation of scalar value to use
  float		un_width;	// #CONDEDIT_ON_rep:GAUSSIAN sigma parameter of a gaussian specifying the tuning width of the coarse-coded units (in min-max units)
  bool		clamp_pat;	// #DEF_false if true, environment provides full set of values to clamp over entire layer (instead of providing single scalar value to clamp on 1st unit, which then generates a corresponding distributed pattern)
  float		min_sum_act;	// #DEF_0.2 minimum total activity of all the units representing a value: when computing weighted average value, this is used as a minimum for the sum that you divide by
  bool		val_mult_lrn;	// #DEF_false for learning, effectively multiply the learning rate by the minus-plus phase difference in overall represented value (i.e., if overall value is the same, no learning takes place)
  bool		clip_val;	// #DEF_true ensure that value remains within specified range
  bool		send_thr;	// #DEF_false use unitspec.opt_thresh.send threshold to cut off small activation contributions to overall average value (i.e., if unit's activation is below this threshold, it doesn't contribute to weighted average computation)

  float		min;		// #READ_ONLY #NO_SAVE #NO_INHERIT minimum unit value
  float		range;		// #READ_ONLY #NO_SAVE #NO_INHERIT range of unit values
  float		val;		// #READ_ONLY #NO_SAVE #NO_INHERIT current val being represented (implementational, computed in InitVal())
  float		incr;		// #READ_ONLY #NO_SAVE #NO_INHERIT increment per unit (implementational, computed in InitVal())

  virtual void	InitVal(float sval, int ugp_size, float umin, float urng);
  // initialize implementational values for subsequently computing GetUnitAct to represent scalar val sval over unit group of ugp_size
  virtual float	GetUnitAct(int unit_idx);
  // get activation under current representation for unit at given index: MUST CALL InitVal first!
  virtual float	GetUnitVal(int unit_idx);
  // get target value associated with unit at given index: MUST CALL InitVal first!

  void	Initialize();
  void 	Destroy()	{ };
  SIMPLE_COPY(ScalarValSpec);
  TA_BASEFUNS(ScalarValSpec);
};

class LEABRA_API ScalarValBias : public taBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra initial bias for given activation value for scalar value units
  INHERITED(taBase)
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

  void	Initialize();
  void 	Destroy()	{ };
  SIMPLE_COPY(ScalarValBias);
  TA_BASEFUNS(ScalarValBias);
};

class LEABRA_API ScalarValLayerSpec : public LeabraLayerSpec {
  // represents a scalar value using a coarse-coded distributed code over units.  first unit represents scalar value.
  INHERITED(LeabraLayerSpec)
public:
  ScalarValSpec	 scalar;	// specifies how values are represented in terms of distributed patterns of activation across the layer
  MinMaxRange	 unit_range;	// range of values represented across the units; for GAUSSIAN, add extra values above and below true useful range to prevent edge effects.
  ScalarValBias	 bias_val;	// specifies bias for given value (as gaussian bump) 
  MinMaxRange	 val_range;	// #READ_ONLY #NO_INHERIT actual range of values (scalar.min/max taking into account un_range)

  virtual void	ClampValue(Unit_Group* ugp, LeabraNetwork* net, float rescale=1.0f);
  // clamp value in the first unit's ext field to the units in the group
  virtual float	ClampAvgAct(int ugp_size);
  // computes the average activation for a clamped unit pattern (for computing rescaling)
  virtual float	ReadValue(Unit_Group* ugp, LeabraNetwork* net);
  // read out current value represented by activations in layer
  virtual void	ResetAfterClamp(LeabraLayer* lay, LeabraNetwork* net);
  // reset activation of first unit(s) after hard clamping
  virtual void	HardClampExt(LeabraLayer* lay, LeabraNetwork* net);
  // hard clamp current ext values (on all units, after ClampValue called) to all the units

  virtual void	LabelUnits_impl(Unit_Group* ugp);
  // label units with their underlying values
  virtual void	LabelUnits(LeabraLayer* lay);
  // #BUTTON label units in given layer with their underlying values
  virtual void	LabelUnitsNet(Network* net);
  // #BUTTON label all layers in given network using this spec

  virtual void	Compute_WtBias_Val(Unit_Group* ugp, float val);
  virtual void	Compute_UnBias_Val(Unit_Group* ugp, float val);
  virtual void	Compute_UnBias_NegSlp(Unit_Group* ugp);
  virtual void	Compute_UnBias_PosSlp(Unit_Group* ugp);
  virtual void	Compute_BiasVal(LeabraLayer* lay);
  // initialize the bias value 

  void 	Init_Weights(LeabraLayer* lay);
  void	Compute_NetinScale(LeabraLayer* lay, LeabraNetwork* net);
  void	Compute_HardClamp(LeabraLayer* lay, LeabraNetwork* net);
  void 	Compute_Act_impl(LeabraLayer* lay, Unit_Group* ug, LeabraInhib* thr, LeabraNetwork* net);

  // don't include first unit in any of these computations!
  void 	Compute_ActAvg_ugp(LeabraLayer*, Unit_Group* ug, LeabraInhib* thr, LeabraNetwork*);
  void 	Compute_ActMAvg_ugp(LeabraLayer*, Unit_Group* ug, LeabraInhib* thr, LeabraNetwork*);
  void 	Compute_ActPAvg_ugp(LeabraLayer*, Unit_Group* ug, LeabraInhib* thr, LeabraNetwork*);

  virtual void 	Compute_dWt_Ugp(Unit_Group* ugp, LeabraLayer* lay, LeabraNetwork* net);
  // compute weight changes just for one unit group
  void	Compute_dWt(LeabraLayer* lay, LeabraNetwork* net);

  virtual float Compute_SSE_Ugp(Unit_Group* ugp, LeabraLayer* lay, int& n_vals);
  override float Compute_SSE(LeabraLayer* lay, int& n_vals,
			     bool unit_avg = false, bool sqrt = false);

  virtual void	ReConfig(Network* net, int n_units = -1);
  // #BUTTON reconfigure layer and associated specs for current scalar.rep type; if n_units > 0, changes number of units in layer to specified value

  void	HelpConfig();	// #BUTTON get help message for configuring this spec
  bool  CheckConfig_Layer(LeabraLayer* lay, bool quiet=false);

  void 	Initialize();
  void	Destroy()		{ };
  void  InitLinks();
  SIMPLE_COPY(ScalarValLayerSpec);
  TA_BASEFUNS(ScalarValLayerSpec);
protected:
  void	UpdateAfterEdit_impl();
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

  void	Initialize();
  void 	Destroy()		{ };
  SIMPLE_COPY(ScalarValSelfPrjnSpec);
  TA_BASEFUNS(ScalarValSelfPrjnSpec);
};


//////////////////////////////////
// 	TwoD Value Layer	//
//////////////////////////////////

class LEABRA_API TwoDValSpec : public taBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra specs for two-dimensional values
  INHERITED(taBase)
public:
  enum	RepType {
    GAUSSIAN,			// gaussian bump, with value = weighted average of tuned unit values
    LOCALIST			// each unit represents a distinct value; intermediate values represented by graded activity of neighbors; overall activity is weighted-average across all units
  };

  RepType	rep;		// #APPLY_IMMED type of representation of scalar value to use
  int		n_vals;		// number of values to represent in layer: layer geom.x must be >= 2 * n_vals because vals are represented in first row of layer!
  float		un_width;	// #CONDEDIT_ON_rep:GAUSSIAN sigma parameter of a gaussian specifying the tuning width of the coarse-coded units (in min-max units)
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

  virtual void	InitVal(float xval, float yval, int xsize, int ysize, float xmin, float xrng, float ymin, float yrng);
  // initialize implementational values for subsequently computing GetUnitAct to represent scalar val sval over unit group of ugp_size
  virtual float	GetUnitAct(int unit_idx);
  // get activation under current representation for unit at given index: MUST CALL InitVal first!
  virtual void	GetUnitVal(int unit_idx, float& x_cur, float& y_cur);
  // get target values associated with unit at given index: MUST CALL InitVal first!

  void	Initialize();
  void 	Destroy()	{ };
  SIMPLE_COPY(TwoDValSpec);
  TA_BASEFUNS(TwoDValSpec);
};

class LEABRA_API TwoDValBias : public taBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra initial bias for given activation value for scalar value units
  INHERITED(taBase)
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

  void	Initialize();
  void 	Destroy()	{ };
  SIMPLE_COPY(TwoDValBias);
  TA_BASEFUNS(TwoDValBias);
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

  virtual void	ClampValue(Unit_Group* ugp, LeabraNetwork* net, float rescale=1.0f);
  // clamp value in the first unit's ext field to the units in the group
  virtual void	ReadValue(Unit_Group* ugp, LeabraNetwork* net);
  // read out current value represented by activations in layer
  virtual void	ResetAfterClamp(LeabraLayer* lay, LeabraNetwork* net);
  // reset activation of first unit(s) after hard clamping
  virtual void	HardClampExt(LeabraLayer* lay, LeabraNetwork* net);
  // hard clamp current ext values (on all units, after ClampValue called) to all the units

  virtual void	LabelUnits_impl(Unit_Group* ugp);
  // label units with their underlying values
  virtual void	LabelUnits(LeabraLayer* lay);
  // #BUTTON label units in given layer with their underlying values
  virtual void	LabelUnitsNet(Network* net);
  // #BUTTON label all layers in given network using this spec

  virtual void	Compute_WtBias_Val(Unit_Group* ugp, float x_val, float y_val);
  virtual void	Compute_UnBias_Val(Unit_Group* ugp, float x_val, float y_val);
  virtual void	Compute_BiasVal(LeabraLayer* lay);
  // initialize the bias value 

  void 	Init_Weights(LeabraLayer* lay);
  void	Compute_NetinScale(LeabraLayer* lay, LeabraNetwork* net);
  void	Compute_HardClamp(LeabraLayer* lay, LeabraNetwork* net);
  void 	Compute_Act_impl(LeabraLayer* lay, Unit_Group* ug, LeabraInhib* thr, LeabraNetwork* net);

  // don't include first unit in any of these computations!
  void 	Compute_ActAvg_ugp(LeabraLayer*, Unit_Group* ug, LeabraInhib* thr, LeabraNetwork*);
  void 	Compute_ActMAvg_ugp(LeabraLayer*, Unit_Group* ug, LeabraInhib* thr, LeabraNetwork*);
  void 	Compute_ActPAvg_ugp(LeabraLayer*, Unit_Group* ug, LeabraInhib* thr, LeabraNetwork*);

  virtual void 	Compute_dWtUgp(Unit_Group* ugp, LeabraLayer* lay, LeabraNetwork* net);
  // compute weight changes just for one unit group
  void	Compute_dWt(LeabraLayer* lay, LeabraNetwork* net);

  virtual float Compute_SSE_Ugp(Unit_Group* ugp, LeabraLayer* lay, int& n_vals);
  override float Compute_SSE(LeabraLayer* lay, int& n_vals,
			     bool unit_avg = false, bool sqrt = false);

  virtual void	ReConfig(Network* net, int n_units = -1);
  // #BUTTON reconfigure layer and associated specs for current scalar.rep type; if n_units > 0, changes number of units in layer to specified value

  void	HelpConfig();	// #BUTTON get help message for configuring this spec
  bool  CheckConfig_Layer(LeabraLayer* lay, bool quiet=false);

  void 	Initialize();
  void	Destroy()		{ };
  void  InitLinks();
  SIMPLE_COPY(TwoDValLayerSpec);
  TA_BASEFUNS(TwoDValLayerSpec);
protected:
  void	UpdateAfterEdit_impl();
};

class LEABRA_API DecodeTwoDValLayerSpec : public TwoDValLayerSpec {
  // a two-d-value layer spec that copies its activations from one-to-one input prjns, to act as a decoder of another layer
  INHERITED(TwoDValLayerSpec)
public:
  void	Compute_Inhib(LeabraLayer* lay, LeabraNetwork* net);
  void 	Compute_InhibAvg(LeabraLayer* lay, LeabraNetwork* net);
  void 	Compute_Act_impl(LeabraLayer* lay, Unit_Group* ug, LeabraInhib* thr, LeabraNetwork* net);
  void	Compute_dWt(LeabraLayer* lay, LeabraNetwork* net);

  void 	Initialize();
  void	Destroy()		{ CutLinks(); }
  TA_BASEFUNS(DecodeTwoDValLayerSpec);
};


//////////////////////////////////////
// 	V1RF Prjn Spec
//////////////////////////////////////

class LEABRA_API V1RFPrjnSpec : public TiledRFPrjnSpec {
  // V1 receptive field projection spec: does a TiledRFPrjnSpec connectivity with Gabor and Blob filter weights
  INHERITED(TiledRFPrjnSpec)
public:
  GaborV1Spec	rf_spec;	// #SHOW_TREE receptive field specs
  
  void 		Connect_impl(Projection* prjn);

  void		C_Init_Weights(Projection* prjn, RecvCons* cg, Unit* ru);
  // set gabor weights

  virtual void	GraphFilter(DataTable* disp_data, int recv_unit_no);
  // #BUTTON #NULL_OK plot the filter gaussian into data table and generate a graph of a given unit number's gabor / blob filter
  virtual void	GridFilter(DataTable* disp_data);
  // #BUTTON #NULL_OK plot the filter gaussian into data table and generate a grid view of all the gabor or blob filters

  void	Initialize();
  void 	Destroy()		{ };
  TA_SIMPLE_BASEFUNS(V1RFPrjnSpec);
protected:
  void	UpdateAfterEdit_impl();
};

#endif // leabra_extra_h

