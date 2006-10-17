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

#ifndef leabra_h
#define leabra_h

#include "pdpbase.h"
#include "netstru.h"
#include "pdpshell.h"

#include "fun_lookup.h"

#include "leabra_def.h"
#include "leabra_TA_type.h"

// pre-declare

class LeabraCon;
class LeabraConSpec;
class LeabraBiasSpec;
class LeabraCon_Group;

class LeabraUnitSpec;
class LeabraUnit;
class LeabraUnit_Group;

class LeabraInhib;
class LeabraLayerSpec;
class LeabraLayer;

class LeabraNetwork;
class LeabraProject;

// _

// The Leabra algorithm:

// Local, Error-driven and Associative, Biologically Realistic Algorithm
// Implements a balance between Hebbian and error-driven learning.

// Hebbian learning is performed using conditional principal
// components analysis (CPCA) algorithm with correction factor for
// sparse expected activity levels.

// Error driven learning is performed using GeneRec, which is a
// generalization of the Recirculation algorithm, and approximates
// Almeida-Pineda recurrent backprop.  The symmetric, midpoint version
// of GeneRec is used, which is equivalent to the contrastive Hebbian
// learning algorithm (CHL).  See O'Reilly (1996; Neural Computation)
// for more details.

// The activation function is a point-neuron approximation with both
// discrete spiking and continuous rate-code output.

// Layer or unit-group level inhibition can be computed directly using
// a k-winners-take-all (KWTA) function, producing sparse distributed
// representations.

// The net input is computed as an average, not a sum, over
// connections, based on normalized, sigmoidaly transformed weight
// values, which are subject to scaling on a connection-group level to
// alter relative contributions.  Automatic scaling is performed to
// compensate for differences in expected activity level in the
// different projections.

// The underlying internal linear weight value upon which learning occurs
// is computed from the nonlinear (sigmoidal) weight value prior to making
// weight changes, and is then converted back in UpdateWeights.  The linear
// weight is always stored as a negative value, so that shared weights or multiple
// weight updates do not try to linearize the already-linear value.  The learning
// rules have been updated to assume that wt is negative (and linear).

// use this macro for iterating over either unit groups one-by-one, or the one
// unit group that is the layer->units, and applying 'code' to either
#define UNIT_GP_ITR(lay, code) \
  int gi = 0; \
  if(lay->units.gp.size > 0) { \
    for(gi=0; gi<lay->units.gp.size; gi++) { \
      Unit_Group* ugp = (Unit_Group*)lay->units.gp[gi]; \
      code \
    } \
  } \
  else { \
    Unit_Group* ugp = (Unit_Group*)&(lay->units); \
    code \
  } 
  
class LeabraCon : public Connection {
  // Leabra connection
public:
  float		dwt;		// #NO_VIEW #NO_SAVE resulting net weight change
  float		pdw;		// #NO_SAVE previous delta-weight change

  void 	Initialize()		{ dwt = pdw = 0.0f; }
  void	Destroy()		{ };
  void	Copy_(const LeabraCon& cp);
  COPY_FUNS(LeabraCon, Connection);
  TA_BASEFUNS(LeabraCon);
};

class LEABRA_API WtScaleSpec : public taBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER weight scaling specification
public:
  float		abs;		// #DEF_1 absolute scaling (not subject to normalization: directly multiplies weight values)
  float		rel;		// [Default: 1] relative scaling (subject to normalization across all other projections into unit)

  inline float	NetScale() 	{ return abs * rel; }

  void	Initialize();
  void	Destroy()	{ };
  SIMPLE_COPY(WtScaleSpec);
  COPY_FUNS(WtScaleSpec, taBase);
  TA_BASEFUNS(WtScaleSpec);
};

class LEABRA_API WtSigSpec : public taBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER sigmoidal weight function specification
public:
  float		gain;		// #DEF_6 gain (contrast, sharpness) of the weight contrast function (1 = linear)
  float		off;		// #DEF_1.25 offset of the function (1=centered at .5, >1=higher, <1=lower)

  // function for implementing weight sigmodid
  static inline float	SigFun(float w, float gain, float off) {
    if(w <= 0.0f) return 0.0f;
    if(w >= 1.0f) return 1.0f;
    return 1.0f / (1.0f + powf(off * (1.0f - w) / w, gain));
  }

  // function for implementing inverse of weight sigmoid
  static inline float	SigFunInv(float w, float gain, float off) {
    if(w <= 0.0f) return 0.0f;
    if(w >= 1.0f) return 1.0f;
    return 1.0f / (1.0f + powf((1.0f - w) / w, 1.0f / gain) / off);
  }

  void	Initialize();
  void	Destroy()	{ };
  SIMPLE_COPY(WtSigSpec);
  COPY_FUNS(WtSigSpec, taBase);
  TA_BASEFUNS(WtSigSpec);
};

class LEABRA_API LearnMixSpec : public taBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER mixture of learning factors (hebbian vs. error-driven) specification
public:
  float		hebb;		// [Default: .01] amount of hebbian learning (should be relatively small, can be effective at .0001)
  float		err;		// #READ_ONLY #SHOW [Default: .99] amount of error driven learning, automatically computed to be 1-hebb
  bool		err_sb;		// #DEF_true apply exponential soft-bounding to the error learning component

  void	UpdateAfterEdit();
  void	Initialize();
  void	Destroy()	{ };
  SIMPLE_COPY(LearnMixSpec);
  COPY_FUNS(LearnMixSpec, taBase);
  TA_BASEFUNS(LearnMixSpec);
};

class LEABRA_API SAvgCorSpec : public taBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER sending average activation correction specifications: affects hebbian learning and netinput computation
public:
  float		cor;		// #DEF_0.4 proportion of correction to apply (0=none, 1=all, .5=half, etc)
  float		thresh;		// #DEF_0.001 threshold of sending average activation below which learning does not occur (prevents learning when there is no input)
  bool		norm_con_n;	// #DEF_false #AKA_div_gp_n in normalizing netinput, divide by the actual number of connections (recv group n), not the overall number of units in the sending layer; this is good when units only receive from a small fraction of sending layer units

  void	Initialize();
  void	Destroy()	{ };
  SIMPLE_COPY(SAvgCorSpec);
  COPY_FUNS(SAvgCorSpec, taBase);
  TA_BASEFUNS(SAvgCorSpec);
};

class LEABRA_API LeabraConSpec : public ConSpec {
  // Leabra connection specs
public:
  enum	LRSValue {		// what value to drive the learning rate schedule with
    NO_LRS,			// don't use a learning rate schedule
    EPOCH,			// current epoch counter
    EXT_REW_STAT,		// avg_ext_rew value on network (computed over an "epoch" of training): value is * 100 (0..100) 
    EXT_REW_AVG			// uses average reward computed by ExtRew layer (if present): value is units[0].act_avg (avg_rew) * 100 (0..100) 
  };

  bool		inhib;		// #DEF_false makes the connection inhibitory (to g_i instead of net)
  WtScaleSpec	wt_scale;	// scale weight values, both relative and absolute factors
  WtSigSpec	wt_sig;		// sigmoidal weight function for contrast enhancement: high gain makes weights more binary & discriminative
  float		lrate;		// #DEF_0.01 learning rate -- how fast do the weights change per experience
  float		cur_lrate;	// #READ_ONLY #NO_INHERIT #SHOW current actual learning rate = lrate * lrate_sched current value (* 1 if no lrate_sched)
  LRSValue	lrs_value;	// what value to drive the learning rate schedule with (Important: affects values entered in start_ctr fields of schedule!)
  Schedule	lrate_sched;	// schedule of learning rate over training epochs (multiplies lrate!)
  LearnMixSpec	lmix;		// mixture of hebbian & err-driven learning
  SAvgCorSpec	savg_cor;	// #AKA_fix_savg for Hebbian and netinput computation: correction for sending average act levels (i.e., renormalization); also norm_con_n for normalizing netinput computation
  
  FunLookup	wt_sig_fun;	// #HIDDEN #NO_SAVE #NO_INHERIT computes wt sigmoidal fun
  FunLookup	wt_sig_fun_inv;	// #HIDDEN #NO_SAVE #NO_INHERIT computes inverse of wt sigmoidal fun
  WtSigSpec	wt_sig_fun_lst;	// #HIDDEN #NO_SAVE #NO_INHERIT last values of wt sig parameters for which the wt_sig_fun's were computed; prevents excessive updating
  float		wt_sig_fun_res;	// #HIDDEN #NO_SAVE #NO_INHERIT last values of resolution parameters for which the wt_sig_fun's were computed

  void		C_Compute_WtFmLin(LeabraCon_Group*, LeabraCon* cn)
  { if(cn->wt < 0.0f) cn->wt = wt_sig_fun.Eval(-cn->wt);  }
  // weight is negative if it is in its linear form, only perform if negative
  inline virtual void	Compute_WtFmLin(LeabraCon_Group* gp);
  // compute actual weight value from linear weight value

  void		C_Compute_LinFmWt(LeabraCon_Group*, LeabraCon* cn)
  { if(cn->wt >= 0.0f) cn->wt = - wt_sig_fun_inv.Eval(cn->wt); }
  // weight is negative if it is in its linear form, only perform if positive
  inline virtual void	Compute_LinFmWt(LeabraCon_Group* gp);
  // compute linear weight value from actual weight value

  virtual void	C_InitWtState_Post(Con_Group*, Connection*, Unit*, Unit*) { };
  // hook for setting other weight-like values after initializing the weight value

  void 		C_InitWtState(Con_Group* cg, Connection* cn, Unit* ru, Unit* su) {
    ConSpec::C_InitWtState(cg, cn, ru, su); LeabraCon* lcn = (LeabraCon*)cn;
    lcn->pdw = 0.0f; C_InitWtState_Post(cg, cn, ru, su); }

  void 		C_InitWtDelta(Con_Group* cg, Connection* cn, Unit* ru, Unit* su)
  { ConSpec::C_InitWtDelta(cg, cn, ru, su); ((LeabraCon*)cn)->dwt=0.0f; }

  inline float 	C_Compute_Net(LeabraCon* cn, Unit*, Unit* su);
  inline float 	Compute_Net(Con_Group* cg, Unit* ru);
  // receiver-based net input 

  inline void 	C_Send_Net(LeabraCon_Group* cg, LeabraCon* cn, Unit* ru, float su_act_eff);
  inline void 	Send_Net(Con_Group* cg, Unit* su);
  // sender-based net input computation

  inline void 	C_Send_Inhib(LeabraCon_Group* cg, LeabraCon* cn, LeabraUnit* ru, float su_act_eff);
  inline virtual void Send_Inhib(LeabraCon_Group* cg, LeabraUnit* su);
  // sender-based inhibitiory net input computation

  inline void 	C_Send_NetDelta(LeabraCon_Group* cg, LeabraCon* cn, LeabraUnit* ru, float su_act_delta_eff);
  inline virtual void Send_NetDelta(LeabraCon_Group* cg, LeabraUnit* su);
  // sender-based delta net input computation (send_delta mode only)

  inline void 	C_Send_InhibDelta(LeabraCon_Group* cg, LeabraCon* cn, LeabraUnit* ru, float su_act_delta_eff);
  inline virtual void Send_InhibDelta(LeabraCon_Group* cg, LeabraUnit* su);
  // sender-based delta inhibitiory net input computation (send_delta mode only)

  inline void 	C_Send_ClampNet(LeabraCon_Group* cg, LeabraCon* cn, LeabraUnit* ru, float su_act_eff);
  inline virtual void Send_ClampNet(LeabraCon_Group* cg, LeabraUnit* su);
  // sender-based net input computation for clamp net

  inline virtual void Compute_SAvgCor(LeabraCon_Group* cg, LeabraUnit* ru);
  // compute hebb correction scaling term for sending average act (cg->savg_cor) based on layer target activity percent

  inline float	C_Compute_Hebb(LeabraCon* cn, LeabraCon_Group* cg,
			       float ru_act, float su_act);
  // compute Hebbian associative learning

  inline float 	C_Compute_Err(LeabraCon*, float ru_act_p, float ru_act_m,
				  float su_act_p, float su_act_m);
  // compute generec error term, sigmoid case

  inline void 	C_Compute_dWt(LeabraCon* cn, LeabraUnit* ru, float heb, float err);
  // combine associative and error-driven weight change, actually update dwt

  inline void 	Compute_dWt(Con_Group* cg, Unit* ru);
  // compute weight change: make new one of these for any C_ change above: hebb, err, dwt

  inline virtual void	B_Compute_dWt(LeabraCon* cn, LeabraUnit* ru);
  // compute bias weight change for netin model of bias weight

  inline void		C_UpdateWeights(LeabraCon* cn, LeabraCon_Group* cg, 
					LeabraUnit* ru, LeabraUnit* su, LeabraUnitSpec* rus);
  // update weights, if activation regulation is NOT in effect
  inline void		C_Compute_ActReg(LeabraCon* cn, LeabraCon_Group* cg, 
					 LeabraUnit* ru, LeabraUnit* su, LeabraUnitSpec* rus);
  // compute dwt for activation regulation
  inline void		C_UpdateWeightsActReg(LeabraCon* cn, LeabraCon_Group* cg, 
					      LeabraUnit* ru, LeabraUnit* su, LeabraUnitSpec* rus);
  // update weights, if activation regulation is in effect
  inline void		UpdateWeights(Con_Group* cg, Unit* ru);
  inline virtual void	B_UpdateWeights(LeabraCon* cn, LeabraUnit* ru, LeabraUnitSpec* rus);

  virtual void	SetCurLrate(int epoch, LeabraNetwork* net);
  // set current learning rate based on schedule given epoch (or error value)

  virtual void	CreateWtSigFun(); // create the wt_sig_fun and wt_sig_fun_inv

  virtual void	Defaults();	// #BUTTON #CONFIRM restores default parameter settings: warning -- you will lose any unique parameters you might have set!

//TEMP  virtual void	GraphWtSigFun(GraphLog* graph_log);
  // #BUTTON #NULL_OK graph the sigmoidal weight contrast enhancement function (NULL = new graph log)

  void	UpdateAfterEdit();
  void 	Initialize();
  void	Destroy()		{ };
  void	InitLinks();
  SIMPLE_COPY(LeabraConSpec);
  COPY_FUNS(LeabraConSpec, ConSpec);
  TA_BASEFUNS(LeabraConSpec);
};

class LEABRA_API LeabraBiasSpec : public LeabraConSpec {
  // Leabra bias-weight connection specs (bias wts are a little bit special)
public:
  float		dwt_thresh;  // #DEF_0.1 don't change if dwt < thresh, prevents buildup of small changes

  inline void	B_Compute_dWt(LeabraCon* cn, LeabraUnit* ru);

  bool	CheckObjectType_impl(TAPtr obj);
  // make sure this can only go with a unitspec, not a project or a congroup

  void	Defaults();

  void 	Initialize();
  void	Destroy()		{ };
  SIMPLE_COPY(LeabraBiasSpec);
  COPY_FUNS(LeabraBiasSpec, LeabraConSpec);
  TA_BASEFUNS(LeabraBiasSpec);
};

class LEABRA_API LeabraCon_Group : public Con_Group {
  // Leabra connection group
public:
  float		scale_eff;	// #NO_SAVE effective scale parameter for netin
  float		savg_cor;	// #NO_SAVE savg correction factor for hebbian learning
  float		net;		// #NO_SAVE netinput to this con_group: not normally computed!

  void	Compute_LinFmWt()  { ((LeabraConSpec*)spec.spec)->Compute_LinFmWt(this); }

  void	Compute_WtFmLin()  { ((LeabraConSpec*)spec.spec)->Compute_WtFmLin(this); }

  void	C_InitWtState_Post(Connection* cn, Unit* ru, Unit* su) { 
    ((LeabraConSpec*)spec.spec)->C_InitWtState_Post(this, cn, ru, su);
  }
  // hook for setting other weight-like values after initializing the weight value

  void	SetCurLrate(int epoch, LeabraNetwork* net) { ((LeabraConSpec*)spec.spec)->SetCurLrate(epoch, net); }

  inline void 	Send_ClampNet(LeabraUnit* su)
  { ((LeabraConSpec*)spec.spec)->Send_ClampNet(this, su); }

  inline void 	Send_NetDelta(LeabraUnit* su)
  { ((LeabraConSpec*)spec.spec)->Send_NetDelta(this, su); }

  void 	Initialize();
  void	Destroy()		{ };
  void	Copy_(const LeabraCon_Group& cp);
  COPY_FUNS(LeabraCon_Group, Con_Group);
  TA_BASEFUNS(LeabraCon_Group);
};

class LEABRA_API ActFunSpec : public taBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER activation function specifications
public:
  enum IThrFun {	       
    STD,			// include all currents (except bias weights) in inhibitory threshold computation
    NO_A,			// exclude gc.a current: allows accommodation to knock out units from kwta active list, without other units coming in to take their place
    NO_H,			// exclude gc.h current: prevent hysteresis current from affecting inhibitory threshold computation
    NO_AH			// exclude gc.a and gc.h currents: prevent both accommodation and hysteresis currents from affecting inhibitory threshold computation
  };

  float		thr;		// #DEF_0.25 threshold value Theta (Q) for firing output activation 
  float		gain;		// #DEF_600 gain (gamma) of the sigmoidal rate-coded activation function 
  float		nvar;		// #DEF_0.005 variance of the Gaussian noise kernel for convolving with XX1 in NOISY_XX1
  float		avg_dt;		// #DEF_0.005 time constant for integrating activation average (computed across trials)
  bool		send_delta;	// #DEF_false #READ_ONLY send only changes in activation when it changes beyond opt_thresh.delta: COPIED FROM LeabraSettle!
  IThrFun	i_thr;		// #DEF_STD how to compute the inhibitory threshold for kWTA functions (what currents to include or exclude in determining what amount of inhibition would keep the unit just at threshold firing)

  void	Initialize();
  void	Destroy()	{ };
  SIMPLE_COPY(ActFunSpec);
  COPY_FUNS(ActFunSpec, taBase);
  TA_BASEFUNS(ActFunSpec);
};

class LEABRA_API SpikeFunSpec : public taBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER spiking activation function specs
public:
  float		decay;		// #DEF_0.05 exponential decay of activation produced by a spike (act(t+1) = act(t) * (1-decay))
  float		v_m_r;		// #DEF_0 post-spiking membrane potential to reset to, produces refractory effect
  float		eq_gain;	// #DEF_10 gain for computing act_eq relative to actual average: act_eq = eq_gain * (spikes/cycles)
  float		eq_dt;		// #DEF_0.02 if non-zero, eq is computed as a running average with this time constant
  float		hard_gain;	// #DEF_0.4 gain for hard-clamped external inputs, mutliplies ext.  constant external inputs otherwise have too much influence compared to spiking ones: Note: soft clamping is strongly recommended

  void	Initialize();
  void	Destroy()	{ };
  SIMPLE_COPY(SpikeFunSpec);
  COPY_FUNS(SpikeFunSpec, taBase);
  TA_BASEFUNS(SpikeFunSpec);
};

class LEABRA_API DepressSpec : public taBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER depressing synapses activation function specs
public:
  enum PSpike {
    P_NXX1,			// probability of spiking is based on NOISY_XX1 f(Vm - Q)
    P_LINEAR			// probability of spiking is based on LINEAR f(Vm - Q)
  };

  PSpike	p_spike;	// how to compute the probability of spiking, which is then mult by amp of spiking
  float		rec;		// #DEF_0.2 rate of recovery of spike amplitude (determines overall time constant of depression function)
  float		asymp_act;	// #DEF_0.5 asymptotic activation value (as proportion of 1) for a fully active unit (determines depl value)
  float		depl;		// #READ_ONLY #SHOW rate of depletion of spike amplitude as a function of activation output (computed from rec, asymp_act)
  float		max_amp;	// #READ_ONLY #SHOW maximum amplitude required to maintain asymptotic firing at normal clamp levels (copied to act_range.max) 

  void	UpdateAfterEdit();
  void	Initialize();
  void	Destroy()	{ };
  SIMPLE_COPY(DepressSpec);
  COPY_FUNS(DepressSpec, taBase);
  TA_BASEFUNS(DepressSpec);
};

class LEABRA_API OptThreshSpec : public taBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER optimization thresholds for faster processing
public:
  float		send;		// #DEF_0.1 don't send activation when act <= send -- greatly speeds processing
  float		delta;		// #DEF_0.005 don't send activation changes until they exceed this threshold: only for when LeabraSettle::send_delta is on!
  float		learn;		// #DEF_0.01 don't learn on recv unit weights when both phase acts <= learn
  bool		updt_wts;	// #DEF_true whether to apply learn threshold to updating weights (otherwise always update)
  float		phase_dif;	// #DEF_0 don't learn when +/- phase difference ratio (- / +) < phase_dif (.8 when used, but off by default)

  void	Initialize();
  void	Destroy()	{ };
  SIMPLE_COPY(OptThreshSpec);
  COPY_FUNS(OptThreshSpec, taBase);
  TA_BASEFUNS(OptThreshSpec);
};

class LEABRA_API DtSpec : public taBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER time constants
public:
  float		vm;		// #DEF_0.3 membrane potential time constant -- if units oscillate too much, then this is too high (but see d_vm_max for another solution)
  float		net;		// #DEF_0.7 net input time constant -- how fast to update net input (damps oscillations)
  float		d_vm_max;	// #DEF_0.025 maximum change in vm at any timestep (limits blowup)
  int		vm_eq_cyc;	// #AKA_cyc0_vm_eq #DEF_0 number of cycles to compute the vm as equilibirium potential given current inputs: set to 1 to quickly activate input layers; set to 100 to always use this computation
  float		vm_eq_dt;	// #DEF_1 time constant for integrating the vm_eq values: how quickly to move toward the current eq value from previous vm value

  void	Initialize();
  void	Destroy()	{ };
  SIMPLE_COPY(DtSpec);
  COPY_FUNS(DtSpec, taBase);
  TA_BASEFUNS(DtSpec);
};

class LEABRA_API LeabraChannels : public taBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER channels used in Leabra
public:
  float		e;		// Excitatory (glutamatergic synaptic sodium (Na) channel)
  float		l;		// Constant leak (potassium, K+) channel 
  float		i;		// inhibitory
  float		h;		// hysteresis (Ca)
  float		a;		// accomodation (k)

  void	Initialize();
  void	Destroy()	{ };
  void 	Copy_(const LeabraChannels& cp);
  COPY_FUNS(LeabraChannels, taBase);
  TA_BASEFUNS(LeabraChannels);
};

class LEABRA_API VChanSpec : public taBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER voltage gated channel specs
public:
  bool		on;		// #DEF_false true if channel is on
  float		b_dt;		// #CONDEDIT_ON_on:true time constant for integrating basis variable (basis ~ intracellular calcium which builds up slowly as function of activation)
  float		a_thr;		// #CONDEDIT_ON_on:true activation threshold of the channel: when basis > a_thr, conductance starts to build up (channels open)
  float		d_thr;		// #CONDEDIT_ON_on:true deactivation threshold of the channel: when basis < d_thr, conductance diminshes (channels close)
  float		g_dt;		// #CONDEDIT_ON_on:true time constant for changing conductance (activating or deactivating)
  bool		init;		// #CONDEDIT_ON_on:true initialize variables when state is intialized between trials (else with weights)

  void	UpdateBasis(float& basis, bool& on_off, float& gc, float act) {
    if(on) {
      basis += b_dt * (act - basis);
      if(basis > a_thr)
	on_off = true;
      if(on_off && (basis < d_thr))
	on_off = false;
      gc += g_dt * ((float)on_off - gc);
    }
  }

  void	Initialize();
  void	Destroy()	{ };
  SIMPLE_COPY(VChanSpec);
  COPY_FUNS(VChanSpec, taBase);
  TA_BASEFUNS(VChanSpec);
};

class LEABRA_API ActRegSpec : public taBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER activity regulation via weight adjustment
public:
  bool		on;		// whether to activity regulation is on (active) or not
  float		min;		// #CONDEDIT_ON_on:true #DEF_0 increase weights for units below this level of average activation
  float		max;		// #CONDEDIT_ON_on:true #DEF_0.35 decrease weights for units above this level of average activation 
  float		wt_dt;		// #CONDEDIT_ON_on:true #DEF_0.2 pre-lrate rate constant for making weight changes to rectify over-activation (dwt = cur_lrate * wt_dt * wt)

  void	Initialize();
  void	Destroy()	{ };
  SIMPLE_COPY(ActRegSpec);
  COPY_FUNS(ActRegSpec, taBase);
  TA_BASEFUNS(ActRegSpec);
};

class LEABRA_API MaxDaSpec : public taBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER how to compute the maxda value, which serves as a stopping criterion for settling
public:
  enum dAValue {
    DA_ONLY,			// just use da
    INET_ONLY,			// just use inet
    INET_DA			// use inet if no activity, then use da
  };

  dAValue	val;		// #DEF_INET_DA value to use for computing delta-activation (change in activation over cycles of settling).
  float		inet_scale;	// #DEF_1 how to scale the inet measure to be like da
  float		lay_avg_thr;	// #DEF_0.01 threshold for layer average activation to switch to da fm Inet

  void	Initialize();
  void	Destroy()	{ };
  SIMPLE_COPY(MaxDaSpec);
  COPY_FUNS(MaxDaSpec, taBase);
  TA_BASEFUNS(MaxDaSpec);
};

class LEABRA_API LeabraUnitSpec : public UnitSpec {
  // Leabra unit specifications, point-neuron approximation
public:
  enum ActFun {
    NOISY_XX1,			// x over x plus 1 convolved with Gaussian noise (noise is nvar)
    XX1,			// x over x plus 1, hard threshold, no noise convolution
    NOISY_LINEAR,		// simple linear output function (still thesholded) convolved with Gaussian noise (noise is nvar)
    LINEAR,			// simple linear output function (still thesholded)
    DEPRESS,			// depressing synapses activation function (rate coded)
    SPIKE			// discrete spiking activations (spike when > thr)
  };

  enum NoiseType {
    NO_NOISE,			// no noise added to processing
    VM_NOISE,			// noise in the value of v_m (membrane potential)
    NETIN_NOISE,		// noise in the net input (g_e)
    ACT_NOISE			// noise in the activations
  };

  ActFun	act_fun;	// activation function to use
  ActFunSpec	act;		// activation function specs
  SpikeFunSpec	spike;		// #CONDEDIT_ON_act_fun:SPIKE spiking function specs (only for act_fun = SPIKE)
  DepressSpec	depress;	// #CONDEDIT_ON_act_fun:DEPRESS depressing synapses activation function specs, note that act_range deterimines range of spk_amp spiking amplitude, max should be > 1
  OptThreshSpec	opt_thresh;	// optimization thresholds for speeding up processing when units are basically inactive
  MinMaxRange	clamp_range;	// range of clamped activation values (min, max, 0, .95 std), don't clamp to 1 because acts can't reach, so .95 instead
  MinMaxRange	vm_range;	// membrane potential range (min, max, 0-1 for normalized, -90-50 for bio-based)
  Random	v_m_init;	// what to initialize the membrane potential to (mean = .15, var = 0 std)
  DtSpec	dt;		// time constants (rate of updating): membrane potential (vm) and net input (net)
  LeabraChannels g_bar;		// [Defaults: 1, .1, 1, .1, .5] maximal conductances for channels
  LeabraChannels e_rev;		// [Defaults: 1, .15, .15, 1, 0] reversal potentials for each channel
  VChanSpec	hyst;		// [Defaults: .05, .8, .7, .1] hysteresis (excitatory) v-gated chan (Ca2+, NMDA)
  VChanSpec	acc;		// [Defaults: .01, .5, .1, .1] accomodation (inhibitory) v-gated chan (K+)
  ActRegSpec	act_reg;	// activity regulation via global scaling of weight values
  MaxDaSpec	maxda;		// maximum change in activation (da) computation -- regulates settling
  NoiseType	noise_type;	// where to add random noise in the processing (if at all)
  Random	noise;		// #CONDEDIT_OFF_noise_type:NO_NOISE distribution parameters for random added noise
  Schedule	noise_sched;	// #CONDEDIT_OFF_noise_type:NO_NOISE schedule of noise variance over settling cycles

  FunLookup	nxx1_fun;	// #HIDDEN #NO_SAVE #NO_INHERIT convolved gaussian and x/x+1 function as lookup table
  FunLookup	noise_conv;	// #HIDDEN #NO_SAVE #NO_INHERIT gaussian for convolution

  void 		InitWtState(Unit* u);
  virtual void 	InitActAvg(LeabraUnit* u);

  virtual void	SetCurLrate(LeabraUnit* u, LeabraNetwork* net, int epoch);
  // set current learning rate based on epoch

  //////////////////////////////////////////
  //	Stage 0: at start of settling	  // 
  //////////////////////////////////////////

  virtual void	InitDelta(LeabraUnit* u);
  virtual void	InitState(LeabraUnit* u, LeabraLayer* lay);
  void		InitState(Unit* u)	{ UnitSpec::InitState(u); }

  virtual void 	Compute_NetScale(LeabraUnit* u, LeabraLayer* lay, LeabraNetwork* net);
  // compute net input scaling values and input from hard-clamped inputs
  virtual void 	Compute_NetRescale(LeabraUnit* u, LeabraLayer* lay, LeabraNetwork* net, float new_scale);
  // rescale netinput scales by given amount
  virtual void 	Init_ClampNet(LeabraUnit* u, LeabraLayer* lay, LeabraNetwork* net);
  // init clam net value prior to sending
  virtual void 	Send_ClampNet(LeabraUnit* u, LeabraLayer* lay, LeabraNetwork* net);
  // compute net input from hard-clamped inputs (sender based)

  //////////////////////////////////
  //	Stage 1: netinput 	  //
  //////////////////////////////////

  void 		Send_Net(Unit* u) { UnitSpec::Send_Net(u); }
  void 		Send_Net(LeabraUnit* u, LeabraLayer* lay);  // add ext input, sender-based

  virtual void 	Send_NetDelta(LeabraUnit* u, LeabraLayer* lay);

  ////////////////////////////////////////////////////////////////
  //	Stage 2: netinput averages and clamping (if necc)	//
  ////////////////////////////////////////////////////////////////

  inline virtual void	Compute_NetAvg(LeabraUnit* u, LeabraLayer* lay, LeabraInhib* thr, LeabraNetwork* net);
  // compute netin average
  inline virtual void	Compute_InhibAvg(LeabraUnit* u, LeabraLayer* lay, LeabraInhib* thr, LeabraNetwork* net);
  // compute inhib netin average
  virtual void	Compute_HardClamp(LeabraUnit* u, LeabraLayer* lay, LeabraNetwork* net);
  // force units to external values provided by environment: also optimizes settling by only computing netinput once
  virtual void	Compute_HardClampNoClip(LeabraUnit* u, LeabraLayer* lay, LeabraNetwork* net);
  // hard-clamp units without clipping values to clamp_range (use for second plus phase clamping to settled values)
  virtual bool	Compute_SoftClamp(LeabraUnit* u, LeabraLayer* lay, LeabraNetwork* net);
  // soft-clamps unit, returns true if unit is not above .5

  ////////////////////////////////////////
  //	Stage 3: inhibition		//
  ////////////////////////////////////////

  virtual float	Compute_IThresh(LeabraUnit* u, LeabraLayer* lay, LeabraNetwork* net);
  // compute inhibitory value that would place unit directly at threshold
  virtual float	Compute_IThreshAll(LeabraUnit* u, LeabraLayer* lay, LeabraNetwork* net);
  // compute inhibitory value that would place unit directly at threshold, using all currents
  virtual float	Compute_IThreshNoA(LeabraUnit* u, LeabraLayer* lay, LeabraNetwork* net);
  // compute inhibitory value that would place unit directly at threshold, excluding gc.a current
  virtual float	Compute_IThreshNoH(LeabraUnit* u, LeabraLayer* lay, LeabraNetwork* net);
  // compute inhibitory value that would place unit directly at threshold, excluding gc.a current
  virtual float	Compute_IThreshNoAH(LeabraUnit* u, LeabraLayer* lay, LeabraNetwork* net);
  // compute inhibitory value that would place unit directly at threshold, excluding any gc.a, gc.h currents

  ////////////////////////////////////////
  //	Stage 4: the final activation 	//
  ////////////////////////////////////////

  override void	Compute_Act(Unit* u)	{ UnitSpec::Compute_Act(u); }
  virtual void 	Compute_Act(LeabraUnit* u, LeabraLayer* lay, LeabraInhib* thr, LeabraNetwork* net);
  // compute the final activation: calls following function steps

  virtual void	Compute_MaxDa(LeabraUnit* u, LeabraLayer* lay, LeabraInhib* thr, LeabraNetwork* net);

  virtual void Compute_Conduct(LeabraUnit* u, LeabraLayer* lay, LeabraInhib* thr, LeabraNetwork* net);
  // compute input conductance values in the gc variables
  virtual void Compute_Vm(LeabraUnit* u, LeabraLayer* lay, LeabraInhib* thr, LeabraNetwork* net);
  // compute the membrante potential from input conductances
  virtual void Compute_ActFmVm(LeabraUnit* u, LeabraLayer* lay, LeabraInhib* thr, LeabraNetwork* net);
  // compute the activation from membrane potential
  virtual void Compute_SelfReg(LeabraUnit* u, LeabraLayer* lay, LeabraInhib* thr, LeabraNetwork* net);
  // compute self-regulatory currents (hysteresis, accommodation)

  ////////////////////////////////////////
  //	Stage 5: Between Events 	//
  ////////////////////////////////////////

  virtual void	PhaseInit(LeabraUnit* u, LeabraLayer* lay, LeabraNetwork* net);
  // initialize external input flags based on phase
  virtual void	DecayPhase(LeabraUnit* u, LeabraLayer* lay, LeabraNetwork* net, float decay);
  // decay activation states towards initial values: at phase-level boundary
  virtual void	DecayEvent(LeabraUnit* u, LeabraLayer* lay, LeabraNetwork* net, float decay);
  // decay activation states towards initial values: at event-level boundary
  virtual void	ExtToComp(LeabraUnit* u, LeabraLayer* lay, LeabraNetwork* net);
  // change external inputs to comparisons (remove input)
  virtual void	TargExtToComp(LeabraUnit* u, LeabraLayer* lay, LeabraNetwork* net);
  // change target & external inputs to comparisons (remove targ & input)
  virtual void	PostSettle(LeabraUnit* u, LeabraLayer* lay, LeabraInhib* thr,
			   LeabraNetwork* net, bool set_both=false);
  // set stuff after settling is over (set_both = both _m and _p for current)

  ////////////////////////////////////////
  //	Stage 6: Learning 		//
  ////////////////////////////////////////

  override void	Compute_dWt(Unit*)	{ };
  virtual void	Compute_dWt(LeabraUnit* u, LeabraLayer* lay, LeabraNetwork* net);
  virtual void	Compute_dWt_impl(LeabraUnit* u, LeabraLayer* lay, LeabraNetwork* net);
  // actually do wt change

  virtual void	Compute_WtFmLin(LeabraUnit* u, LeabraLayer* lay, LeabraNetwork* net);
  // if weights need to be updated from linear values without doing updatewts

  override void	UpdateWeights(Unit* u);

  virtual void	EncodeState(LeabraUnit*, LeabraLayer*, LeabraNetwork*)	{ };
  // encode current state information (hook for time-based learning)

  override float Compute_SSE(Unit* u);	// compute sum squared error for this unit

  virtual void	CreateNXX1Fun();  // create convolved gaussian and x/x+1 

  virtual void	Defaults();	// #BUTTON #CONFIRM restores default parameter settings: warning -- you will lose any unique parameters you might have set!

//TEMP  virtual void	GraphVmFun(GraphLog* graph_log, float g_i = .5, float min = 0.0, float max = 1.0, float incr = .01);
  // #BUTTON #NULL_OK graph membrane potential (v_m) as a function of excitatory net input (net) for given inhib conductance (g_i) (NULL = new graph log)
//TEMP  virtual void	GraphActFmVmFun(GraphLog* graph_log, float min = .15, float max = .50, float incr = .001);
  // #BUTTON #NULL_OK graph the activation function as a function of membrane potential (v_m) (NULL = new graph log)
//TEMP  virtual void	GraphActFmNetFun(GraphLog* graph_log, float g_i = .5, float min = 0.0, float max = 1.0, float incr = .001);
  // #BUTTON #NULL_OK graph the activation function as a function of net input (projected through membrane potential) (NULL = new graph log)

  bool  CheckConfig(Unit* un, bool quiet=false);

  void	UpdateAfterEdit();	// to set _impl sig
  void 	Initialize();
  void	Destroy()		{ };
  void	InitLinks();
  SIMPLE_COPY(LeabraUnitSpec);
  COPY_FUNS(LeabraUnitSpec, UnitSpec);
  TA_BASEFUNS(LeabraUnitSpec);
};

class LEABRA_API VChanBasis : public taBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER basis variables for vchannels
public:
  float		hyst;		// hysteresis
  float		acc;		// fast accomodation
  bool		hyst_on;	// #NO_VIEW binary thresholded mode state variable, hyst
  bool		acc_on;		// #NO_VIEW binary thresholded mode state variable, acc
  float		g_h;		// #NO_VIEW hysteresis conductance
  float		g_a;		// #NO_VIEW accomodation conductance

  void	Initialize();
  void	Destroy()	{ };
  void 	Copy_(const VChanBasis& cp);
  COPY_FUNS(VChanBasis, taBase);
  TA_BASEFUNS(VChanBasis);
};

class LEABRA_API LeabraUnitChans : public taBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER channels used in Leabra units
public:
  float		l;		// leak
  float		i;		// #DMEM_SHARE_SET_1 inhibitory
  float		h;		// hysteresis (Ca)
  float		a;		// accomodation (K)

  void	Initialize();
  void	Destroy()	{ };
  void 	Copy_(const LeabraUnitChans& cp);
  COPY_FUNS(LeabraUnitChans, taBase);
  TA_BASEFUNS(LeabraUnitChans);
};

class LEABRA_API LeabraUnit : public Unit {
  // ##DMEM_SHARE_SETS_5 Leabra unit, point-neuron approximation
public:
  float		act_eq;		// #NO_SAVE rate-code equivalent activity value (time-averaged spikes or just act)
  float		act_avg;	// average activation over long time intervals (dt = act.avg_dt)
  float		act_m;		// minus_phase activation, set after settling, used for learning
  float		act_p;		// plus_phase activation, set after settling, used for learning
  float		act_dif;	// difference between plus and minus phase acts, gives unit err contribution
  float		da;		// #NO_SAVE delta activation: change in act from one cycle to next, used to stop settling
  VChanBasis	vcb;		// voltage-gated channel basis variables
  LeabraUnitChans gc;		// #DMEM_SHARE_SET_1 #NO_SAVE current unit channel conductances
  float		I_net;		// #NO_SAVE net current produced by all channels
  float		v_m;		// #NO_SAVE membrane potential

  bool		in_subgp;	// #READ_ONLY #NO_SAVE determine if unit is in a subgroup
  float		clmp_net;	// #NO_VIEW #NO_SAVE #DETAIL #DMEM_SHARE_SET_4 hard-clamp net input (no need to recompute)
  float		net_scale;	// #NO_VIEW #NO_SAVE #DETAIL total netinput scaling basis
  float		bias_scale;	// #NO_VIEW #NO_SAVE #DETAIL bias weight scaling factor
  float		prv_net;	// #NO_VIEW #NO_SAVE #DETAIL previous net input (for time averaging)
  float		prv_g_i;	// #NO_VIEW #NO_SAVE #DETAIL previous inhibitory conductance value (for time averaging)

  float		act_sent;	// #NO_VIEW #NO_SAVE #DETAIL last activation value sent (only send when diff is over threshold)
  float		act_delta;	// #NO_VIEW #NO_SAVE #DETAIL change in activation to send to other units
  float		net_raw;	// #NO_VIEW #NO_SAVE #DETAIL raw net input received from sending units (increments the deltas in send_delta)
  float		net_delta;	// #NO_VIEW #NO_SAVE #DETAIL #DMEM_SHARE_SET_3 change in netinput received from other units  (send_delta mode only)
  float		g_i_raw;	// #NO_VIEW #NO_SAVE #DETAIL raw inhib net input received from sending units (increments the deltas in send_delta)
  float		g_i_delta;	// #NO_VIEW #NO_SAVE #DETAIL #DMEM_SHARE_SET_3 change in inhibitory netinput received from other units (send_delta mode only)

  float		i_thr;		// #NO_SAVE inhibitory threshold value for computing kWTA
  float		spk_amp;	// amplitude of spiking output (for depressing synapse activation function)
  float		misc_1;		// #NO_VIEW miscellaneous variable for other algorithms that need it (e.g., TdLayerSpec)

  void		InitActAvg()
  { ((LeabraUnitSpec*)spec.spec)->InitActAvg(this); }

  void		InitDelta()
  { ((LeabraUnitSpec*)spec.spec)->InitDelta(this); }
  void		InitState(LeabraLayer* lay)
  { ((LeabraUnitSpec*)spec.spec)->InitState(this, lay); }

  void		SetCurLrate(LeabraNetwork* net, int epoch)
  { ((LeabraUnitSpec*)spec.spec)->SetCurLrate(this, net, epoch); }

  void		Compute_NetScale(LeabraLayer* lay, LeabraNetwork* net)
  { ((LeabraUnitSpec*)spec.spec)->Compute_NetScale(this, lay, net); }
  void		Compute_NetRescale(LeabraLayer* lay, LeabraNetwork* net, float new_scale)
  { ((LeabraUnitSpec*)spec.spec)->Compute_NetRescale(this, lay, net, new_scale); }
  void		Init_ClampNet(LeabraLayer* lay, LeabraNetwork* net)
  { ((LeabraUnitSpec*)spec.spec)->Init_ClampNet(this, lay, net); }
  void		Send_ClampNet(LeabraLayer* lay, LeabraNetwork* net)
  { ((LeabraUnitSpec*)spec.spec)->Send_ClampNet(this, lay, net); }

  void		Send_Net(LeabraLayer* lay)
  { ((LeabraUnitSpec*)spec.spec)->Send_Net(this, lay); }
  void		Send_NetDelta(LeabraLayer* lay)
  { ((LeabraUnitSpec*)spec.spec)->Send_NetDelta(this, lay); }

  void		Compute_NetAvg(LeabraLayer* lay, LeabraInhib* athr, LeabraNetwork* net)
  { ((LeabraUnitSpec*)spec.spec)->Compute_NetAvg(this, lay, athr, net); }
  void		Compute_InhibAvg(LeabraLayer* lay, LeabraInhib* athr, LeabraNetwork* net)
  { ((LeabraUnitSpec*)spec.spec)->Compute_InhibAvg(this, lay, athr, net); }
  void		Compute_HardClamp(LeabraLayer* lay, LeabraNetwork* net) 
  { ((LeabraUnitSpec*)spec.spec)->Compute_HardClamp(this, lay, net); }
  void		Compute_HardClampNoClip(LeabraLayer* lay, LeabraNetwork* net) 
  { ((LeabraUnitSpec*)spec.spec)->Compute_HardClampNoClip(this, lay, net); }
  bool		Compute_SoftClamp(LeabraLayer* lay, LeabraNetwork* net) 
  { return ((LeabraUnitSpec*)spec.spec)->Compute_SoftClamp(this, lay, net); }

  float		Compute_IThresh(LeabraLayer* lay, LeabraNetwork* net)
  { return ((LeabraUnitSpec*)spec.spec)->Compute_IThresh(this, lay, net); }
  float		Compute_IThreshNoAH(LeabraLayer* lay, LeabraNetwork* net)
  { return ((LeabraUnitSpec*)spec.spec)->Compute_IThreshNoAH(this, lay, net); }

  void		Compute_Act()	{ Unit::Compute_Act(); }
  void 		Compute_Act(LeabraLayer* lay, LeabraInhib* athr, LeabraNetwork* net) 
  { ((LeabraUnitSpec*)spec.spec)->Compute_Act(this, lay, athr, net); }

  void 		Compute_MaxDa(LeabraLayer* lay, LeabraInhib* athr, LeabraNetwork* net) 
  { ((LeabraUnitSpec*)spec.spec)->Compute_MaxDa(this, lay, athr, net); }

  void		PhaseInit(LeabraLayer* lay, LeabraNetwork* net)
  { ((LeabraUnitSpec*)spec.spec)->PhaseInit(this, lay, net); }
  void		DecayEvent(LeabraLayer* lay, LeabraNetwork* net, float decay)
  { ((LeabraUnitSpec*)spec.spec)->DecayEvent(this, lay, net, decay); }
  void		DecayPhase(LeabraLayer* lay, LeabraNetwork* net, float decay)
  { ((LeabraUnitSpec*)spec.spec)->DecayPhase(this, lay, net, decay); }
  void		ExtToComp(LeabraLayer* lay, LeabraNetwork* net)
  { ((LeabraUnitSpec*)spec.spec)->ExtToComp(this, lay, net); }
  void		TargExtToComp(LeabraLayer* lay, LeabraNetwork* net)
  { ((LeabraUnitSpec*)spec.spec)->TargExtToComp(this, lay, net); }
  void		PostSettle(LeabraLayer* lay, LeabraInhib* athr, LeabraNetwork* net, bool set_both=false)
  { ((LeabraUnitSpec*)spec.spec)->PostSettle(this, lay, athr, net, set_both); }

  void 		Compute_dWt(LeabraLayer* lay, LeabraNetwork* net)
  { ((LeabraUnitSpec*)spec.spec)->Compute_dWt(this, lay, net); }	  

  void 		Compute_WtFmLin(LeabraLayer* lay, LeabraNetwork* net) 
  { ((LeabraUnitSpec*)spec.spec)->Compute_WtFmLin(this, lay, net); }

  void 		EncodeState(LeabraLayer* lay, LeabraNetwork* net)
  { ((LeabraUnitSpec*)spec.spec)->EncodeState(this, lay, net); }

  void		GetInSubGp();

  void 	Initialize();
  void	Destroy()		{ };
  void	InitLinks();
  SIMPLE_COPY(LeabraUnit);
  COPY_FUNS(LeabraUnit, Unit);
  TA_BASEFUNS(LeabraUnit);
};

class LEABRA_API LeabraSort : public taPtrList<LeabraUnit> {
  // ##NO_TOKENS ##NO_UPDATE_AFTER used for sorting units in kwta computation
protected:
  int		El_Compare_(void* a, void* b) const
  { int rval=-1; if(((LeabraUnit*)a)->net < ((LeabraUnit*)b)->net) rval=1;
    else if(((LeabraUnit*)a)->net == ((LeabraUnit*)b)->net) rval=0; return rval; }
  // compare two items for purposes of sorting: descending order by net
public:
  int	FindNewNetPos(float nw_net);	  // find position in list for a new net value
  void	FastInsertLink(void* it, int where); // faster version of insert link fun
};

// misc data-holding structures

class LEABRA_API KWTASpec : public taBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER specifies k-winner-take-all parameters
public:
  enum K_From {
    USE_K,			// use the k specified directly
    USE_PCT,			// use the percentage pct to compute the k as a function of layer size
    USE_PAT_K			// use the activity level of the current event pattern (k = # of units > pat_q)
  };

  K_From	k_from;		// how is the active_k determined: directly by k, by pct, or by no. of units where ext > pat_q
  int		k;		// #CONDEDIT_ON_k_from:USE_K desired number of active units in the layer
  float		pct;		// #CONDEDIT_ON_k_from:USE_PCT desired proportion of activity (used to compute a k value based on layer size, .25 std)
  float		pat_q;		// #HIDDEN #DEF_0.5 threshold for pat_k based activity level: add to k if ext > pat_q
  bool		diff_act_pct;	// #DEF_false if true, use different actual percent activity for overall layer activation
  float		act_pct;	// #CONDEDIT_ON_diff_act_pct:true actual percent activity to put in kwta.pct field of layer
  bool		gp_i;		// compute inhibition including all of the layers in the same group, or unit groups within the layer: each items computed inhib vals are multipled by gp_g scaling, then MAX'd, and each item's inhib is the MAX of this pooled MAX value and its original own value
  float		gp_g;		// #CONDEDIT_ON_gp_i:true how much this item (layer or unit group) contributes to the pooled layer group values

  void	Initialize();
  void 	Destroy()	{ };
  SIMPLE_COPY(KWTASpec);
  COPY_FUNS(KWTASpec, taBase);
  TA_BASEFUNS(KWTASpec);
};

class LEABRA_API KwtaTieBreak : public taBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER break ties where all the units have similar netinputs and thus none get activated.  this lowers the inhibition so that all get active to some extent
public:
  bool		on;		// whether to perform the tie breaking function at all
  float		k_thr; 		// #CONDEDIT_ON_on:true #DEF_1 threshold on inhibitory threshold (i_thr) for top kwta units before tie break is engaged: don't break ties for weakly activated layers
  float		diff_thr;	// #CONDEDIT_ON_on:true #DEF_0.2 threshold on difference ratio between top k and rest (k_ithr - k1_ithr) / k_ithr for a tie to be indicated.  This is also how much k1_ithr is reduced relative to k_ithr to fix the tie: sets a lower limit on this value.  larger values mean higher overall activations during ties, but you dont' want to activate the tie mechanism unnecessarily either.

  void	Initialize();
  void 	Destroy()	{ };
  SIMPLE_COPY(KwtaTieBreak);
  COPY_FUNS(KwtaTieBreak, taBase);
  TA_BASEFUNS(KwtaTieBreak);
};

class LEABRA_API AdaptISpec : public taBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER specifies adaptive kwta specs (esp for avg-based)
public:
  enum AdaptType {
    NONE,			// don't adapt anything
    KWTA_PT,			// adapt kwta point (i_kwta_pt) based on running-average layer activation as compared to target value
    G_BAR_I,			// adapt g_bar.i for unit inhibition values based on layer activation at any point in time
    G_BAR_IL			// adapt g_bar.i and g_bar.l for unit inhibition & leak values based on layer activation at any point in time
  };

  AdaptType	type;		// what to adapt, or none for nothing
  float		tol;		// #CONDEDIT_OFF_type:NONE #DEF_0.02 tolerance around target avg act before changing parameter
  float		p_dt;		// #CONDEDIT_OFF_type:NONE #DEF_0.1 #AKA_pt_dt time constant for changing the parameter (i_kwta_pt or g_bar.i)
  float		mx_d;		// #CONDEDIT_OFF_type:NONE #DEF_0.9 maximum deviation (proportion) from initial parameter setting allowed
  float		l;		// #CONDEDIT_ON_type:G_BAR_IL proportion of difference from target activation to allocate to the leak in G_BAR_IL mode
  float		a_dt;		// #CONDEDIT_ON_type:KWTA_PT #DEF_0.005 time constant for integrating average average activation, which is basis for adapting i_kwta_pt

  void	Initialize();
  void 	Destroy()	{ };
  SIMPLE_COPY(AdaptISpec);
  COPY_FUNS(AdaptISpec, taBase);
  TA_BASEFUNS(AdaptISpec);
};

class LEABRA_API ClampSpec : public taBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER specs for clamping 
public:
  bool		hard;		// #DEF_true whether to hard clamp inputs to this layer or not
  float		gain;		// #CONDEDIT_OFF_hard:true #DEF_0.5 starting soft clamp gain factor (net = gain * ext)
  float		d_gain;		// #CONDEDIT_OFF_hard:true [Default: 0] for soft clamp, delta to increase gain when target units not > .5 (0 = off, .1 std when used)

  void	Initialize();
  void 	Destroy()	{ };
  SIMPLE_COPY(ClampSpec);
  COPY_FUNS(ClampSpec, taBase);
  TA_BASEFUNS(ClampSpec);
};

class LEABRA_API DecaySpec : public taBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER holds decay values
public:
  float		event;		// #DEF_1 proportion decay of state vars between events
  float		phase;		// #DEF_1 proportion decay of state vars between minus and plus phases 
  float		phase2;		// #DEF_0 proportion decay of state vars between 2nd set of phases (if appl, 0 std)
  bool		clamp_phase2;	// #DEF_false if true, hard-clamp second plus phase activations to prev plus phase (only special layers will then update -- optimizes speed)

  void	Initialize();
  void 	Destroy()	{ };
  SIMPLE_COPY(DecaySpec);
  COPY_FUNS(DecaySpec, taBase);
  TA_BASEFUNS(DecaySpec);
};

class LEABRA_API LayNetRescaleSpec : public taBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER global rescale of layer netinputs to prevent blowup
public:
  bool		on;		// whether to apply layer netinput rescaling
  float		max_net; 	// #CONDEDIT_ON_on:true #DEF_0.6 target maximum netinput value
  float		net_extra;	// #CONDEDIT_ON_on:true #DEF_0.2 extra netin value to add to actual to anticipate further increases, preventing too many updates

  void	Initialize();
  void 	Destroy()	{ };
  SIMPLE_COPY(LayNetRescaleSpec);
  COPY_FUNS(LayNetRescaleSpec, taBase);
  TA_BASEFUNS(LayNetRescaleSpec);
};

class LEABRA_API LeabraLayerSpec : public LayerSpec {
  // Leabra layer specs, computes inhibitory input for all units in layer
public:
  enum Compute_I {		// how to compute the inhibition
    KWTA_INHIB,			// between thresholds of k and k+1th most activated units (sets precise k value, should use i_kwta_pt = .25 std)
    KWTA_AVG_INHIB,		// average of top k vs avg of rest (provides more flexibility in actual k value, should use i_kwta_pt = .6 std)
    AVG_MAX_PT_INHIB,		// put inhib value at i_kwta_pt between avg and max values for layer!
    UNIT_INHIB			// unit-based inhibition (g_i from netinput -- requires connections with inhib flag set to provide inhibition)
  };

  enum InhibGroup {
    ENTIRE_LAYER,		// treat entire layer as one inhibitory group (even if subgroups exist)
    UNIT_GROUPS,		// treat sub unit groups as separate inhibitory groups (but also uses gp_i and gp_g if set, to have some sharing of inhib across groups
    LAY_AND_GPS			// compute inhib over both groups and whole layer, inhibi is max of layer and group inhib
  };

  KWTASpec	kwta;		// #CONDEDIT_OFF_inhib_group:UNIT_GROUPS desired activity level over entire layer (NOTE: used to set target activity for UNIT_INHIB, AVG_MAX_PT_INHIB, but not used for actually computing inhib for these cases)
  KWTASpec	gp_kwta;	// #CONDEDIT_OFF_inhib_group:ENTIRE_LAYER desired activity level for units within unit groups (not for ENTIRE_LAYER) (NOTE: used to set target activity for UNIT_INHIB, AVG_MAX_PT_INHIB, but not used for actually computing inhib for these cases)
  InhibGroup	inhib_group;	// what to consider the inhibitory group (layer or unit subgroups, or both)
  Compute_I	compute_i;	// how to compute inhibition (g_i): two forms of kwta or unit-level inhibition
  float		i_kwta_pt;	// [Default: .25 for KWTA_INHIB, .6 for KWTA_AVG, .2 for AVG_MAX_PT_INHIB] point to place inhibition between k and k+1 (or avg and max for AVG_MAX_PT_INHIB)
  float		gp_i_pt;	// #CONDEDIT_ON_compute_i:AVG_MAX_PT_INHIB [Default: .2] for unit groups: point to place inhibition between avg and max for AVG_MAX_PT_INHIB
  KwtaTieBreak	tie_brk;	// break ties when all the units in the layer have similar netinputs, which puts the inhbition value too close to everyone's threshold and produces no activation at all.  this will lower the inhibition and allow all the units to have some activation
  AdaptISpec	adapt_i;	// #AKA_adapt_pt adapt the inhibition: either i_kwta_pt point based on diffs between actual and target k level (for avg-based), or g_bar.i for unit-inhib
  ClampSpec	clamp;		// how to clamp external inputs to units (hard vs. soft)
  DecaySpec	decay;		// decay of activity state vars between events, -/+ phase, and 2nd set of phases (if appl)
  LayNetRescaleSpec net_rescale; // rescale layer-wide netinputs to prevent blowup, when max net exceeds specified net value

  virtual void	InitWtState(LeabraLayer* lay);
  // initialize weight values and other permanent state
  virtual void	InitActAvg(LeabraLayer* lay);
  // initialize act_avg values

  virtual void	SetCurLrate(LeabraLayer* lay, LeabraNetwork* net, int epoch);
  // set current learning rate based on epoch

  //////////////////////////////////////////
  //	Stage 0: at start of settling	  // 
  //////////////////////////////////////////

  virtual void	Compute_Active_K(LeabraLayer* lay);
  // prior to settling: compute actual activity levels based on spec, inputs, etc
  virtual void	Compute_Active_K_impl(LeabraLayer* lay, Unit_Group* ug,
				      LeabraInhib* thr, KWTASpec& kwtspec);
  virtual int	Compute_Pat_K(LeabraLayer* lay, Unit_Group* ug, LeabraInhib* thr);
  // above are implementation helpers

  virtual void	InitState(LeabraLayer* lay);
  // prior to settling: initialize dynamic state variables
  virtual void	Compute_HardClampPhase2(LeabraLayer* lay, LeabraNetwork* net);
  // prior to settling: hard-clamp inputs (special code for hard clamping in phase 2 based on prior acts)
  virtual void	Compute_HardClamp(LeabraLayer* lay, LeabraNetwork* net);
  // prior to settling: hard-clamp inputs
  virtual void	Compute_NetScale(LeabraLayer* lay, LeabraNetwork* net);
  // prior to settling: compute netinput scaling values
  virtual void	Init_ClampNet(LeabraLayer* lay, LeabraNetwork* net);
  // prior to settling: init clamp net variable prior to sending
  virtual void	Send_ClampNet(LeabraLayer* lay, LeabraNetwork* net);
  // prior to settling: compute input from hard-clamped

  //////////////////////////////////
  //	Stage 1: netinput 	  //
  //////////////////////////////////

  virtual void 	Send_Net(LeabraLayer* lay);
  // compute net inputs
  virtual void 	Send_NetDelta(LeabraLayer* lay);
  // compute net inputs as changes in activation

  ////////////////////////////////////////////////////////////////
  //	Stage 2: netinput averages and clamping (if necc)	//
  ////////////////////////////////////////////////////////////////

  virtual void	Compute_Clamp_NetAvg(LeabraLayer* lay, LeabraNetwork* net);
  // clamp and compute averages of net inputs that were already computed
  virtual void	Compute_NetAvg(LeabraLayer* lay, Unit_Group* ug, LeabraInhib* thr, LeabraNetwork* net);
  virtual void	Compute_SoftClamp(LeabraLayer* lay, LeabraNetwork* net);
  // soft-clamp inputs by adding to net input

  ////////////////////////////////////////
  //	Stage 3: inhibition		//
  ////////////////////////////////////////

  virtual void	InitInhib(LeabraLayer* lay);
  // initialize the inhibitory state values
  virtual void	Compute_Inhib(LeabraLayer* lay, LeabraNetwork* net);
  // stage two: compute the inhibition for layer
  virtual void	Compute_Inhib_impl(LeabraLayer* lay, Unit_Group* ug, LeabraInhib* thr, LeabraNetwork* net);
  // implementation of inhibition computation for either layer or unit group
  virtual void	Compute_Inhib_kWTA(LeabraLayer* lay, Unit_Group* ug, LeabraInhib* thr, LeabraNetwork* net);
  // implementation of basic kwta inhibition computation
  virtual void	Compute_Inhib_kWTA_Avg(LeabraLayer* lay, Unit_Group* ug, LeabraInhib* thr, LeabraNetwork* net);
  // implementation of kwta avg-based inhibition computation
  virtual void	Compute_Inhib_AvgMaxPt(LeabraLayer* lay, Unit_Group* ug, LeabraInhib* thr, LeabraNetwork* net);
  // implementation of avg-max-pt inhibition computation
  virtual void	Compute_Inhib_kWTA_Gps(LeabraLayer* lay, LeabraNetwork* net);
  // implementation of GPS_THEN_UNITS kwta on groups
  virtual void 	Compute_Inhib_BreakTie(LeabraInhib* thr);
  // break any ties in the kwta function

  virtual void	Compute_LayInhibToGps(LeabraLayer* lay, LeabraNetwork* net);
  // Stage 3.25: for layer groups, need to propagate inhib out to unit groups

  ////// Stage 3.5: second pass of inhibition to do averaging

  virtual void 	Compute_InhibAvg(LeabraLayer* lay, LeabraNetwork* net);
  // stage three: compute final activation
  virtual void 	Compute_InhibAvg_impl(LeabraLayer* lay, Unit_Group* ug, LeabraInhib* thr, LeabraNetwork* net);

  ////////////////////////////////////////
  //	Stage 4: the final activation 	//
  ////////////////////////////////////////

  virtual void 	Compute_ActAvg_ugp(LeabraLayer*, Unit_Group* ug, LeabraInhib* thr, LeabraNetwork*);
  virtual void	Compute_ActAvg(LeabraLayer* lay, LeabraNetwork* net);
  // helper function to compute acts.avg from act_eq
  virtual void 	Compute_ActMAvg_ugp(LeabraLayer*, Unit_Group* ug, LeabraInhib* thr, LeabraNetwork*);
  virtual void	Compute_ActMAvg(LeabraLayer* lay, LeabraNetwork* net);
  // helper function to compute acts_m.avg from act_m
  virtual void 	Compute_ActPAvg_ugp(LeabraLayer*, Unit_Group* ug, LeabraInhib* thr, LeabraNetwork*);
  virtual void	Compute_ActPAvg(LeabraLayer* lay, LeabraNetwork* net);
  // helper function to compute acts_p.avg from act_p

  virtual void 	Compute_Act(LeabraLayer* lay, LeabraNetwork* net);
  // stage three: compute final activation
  virtual void 	Compute_Act_impl(LeabraLayer* lay, Unit_Group* ug, LeabraInhib* thr, LeabraNetwork* net);
  virtual void 	Compute_NetRescale(LeabraLayer* lay, LeabraNetwork* net);
  // do net rescaling to prevent blowup based on netin.max

  ////////////////////////////////////////
  //	Stage 5: Between Events 	//
  ////////////////////////////////////////

  virtual void	PhaseInit(LeabraLayer* lay, LeabraNetwork* net);
  // initialize start of a setting phase, set input flags appropriately, etc

  virtual void	DecayEvent(LeabraLayer* lay, LeabraNetwork* net);
  virtual void	DecayPhase(LeabraLayer* lay, LeabraNetwork* net);
  virtual void	DecayPhase2(LeabraLayer* lay, LeabraNetwork* net);

  virtual void	ExtToComp(LeabraLayer* lay, LeabraNetwork* net);
  // change external inputs to comparisons (remove input)
  virtual void	TargExtToComp(LeabraLayer* lay, LeabraNetwork* net);
  // change target & external inputs to comparisons (remove targ & input)
  virtual void	PostSettle(LeabraLayer* lay, LeabraNetwork* net, bool set_both=false);
  // after settling, keep track of phase variables, etc.
//   virtual void	NormMPActs(LeabraLayer* lay, LeabraNetwork* net);
//   // normalize minus and plus phase activations to the same average level
  virtual void	AdaptGBarI(LeabraLayer* lay, LeabraNetwork* net);
  // adapt inhibitory conductances based on target activation values relative to current values

  ////////////////////////////////////////
  //	Stage 6: Learning 		//
  ////////////////////////////////////////

  virtual void	AdaptKWTAPt(LeabraLayer* lay, LeabraNetwork* net);
  // adapt the kwta point based on average activity
  virtual void	Compute_dWt(LeabraLayer* lay, LeabraNetwork* net);
  virtual void	Compute_WtFmLin(LeabraLayer* lay, LeabraNetwork* net);
  // use this if weights will be used again for activations prior to being updated

  virtual LeabraLayer* FindLayerFmSpec(LeabraLayer* lay, int& prjn_idx, TypeDef* layer_spec);
  // find a layer that given layer receives from based on the type of layer spec
  virtual LeabraLayer* FindLayerFmSpecExact(LeabraLayer* lay, int& prjn_idx, TypeDef* layer_spec);
  // find a layer that given layer receives from based on the type of layer spec: uses exact type match, not inherits!
  static  LeabraLayer* FindLayerFmSpecNet(Network* net, TypeDef* layer_spec);
  // find a layer in network based on the type of layer spec

  virtual void	HelpConfig();	// #BUTTON get help message for configuring this spec
  override bool CheckConfig(LeabraLayer* lay, bool quiet=false);
  // check for for misc configuration settings required by different algorithms, including settings on the processes

  virtual void	Defaults();	// #BUTTON #CONFIRM restores default parameter settings: warning -- you will lose any unique parameters you might have set!

  void	UpdateAfterEdit();
  void 	Initialize();
  void	Destroy()		{ CutLinks(); }
  void	InitLinks();
  void	CutLinks();
  SIMPLE_COPY(LeabraLayerSpec);
  COPY_FUNS(LeabraLayerSpec, LayerSpec);
  TA_BASEFUNS(LeabraLayerSpec);
};

SpecPtr_of(LeabraLayerSpec);

class LEABRA_API AvgMaxVals : public taBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER holds average and max statistics
public:
  float		avg;		// average value
  float		max;		// maximum value
  int 		max_i;		// index of unit with maximum value
  
  void	Initialize();
  void 	Destroy()	{ };
  void	Copy_(const AvgMaxVals& cp);
  COPY_FUNS(AvgMaxVals, taBase);
  TA_BASEFUNS(AvgMaxVals);
};

class LEABRA_API KWTAVals : public taBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER holds values for kwta stuff
public:
  int		k;       	// target number of active units for this collection
  float		pct;		// actual percent activity in group
  float		pct_c;		// #HIDDEN complement of (1.0 - ) actual percent activity in group
  int		adth_k;		// #HIDDEN adapting threshold k value -- how many units can adapt per time
  float		k_ithr;		// inhib threshold for k unit (top k for kwta_avg)
  float		k1_ithr;	// inhib threshold for k+1 unit (other units for kwta_avg)
  float		ithr_r;		// log of ratio of ithr values (indicates signal differentiation)
  float		ithr_diff;	// normalized difference ratio for k vs k+1 ithr values: (k_ithr - k1_ithr) / k_ithr
  int		tie_brk;	// was a tie break operation applied to this layer based on ithr_diff value?

  void		Compute_Pct(int n_units);
  void		Compute_IThrR(); // compute ithr_r ratio value

  void	Initialize();
  void 	Destroy()	{ };
  void	Copy_(const KWTAVals& cp);
  COPY_FUNS(KWTAVals, taBase);
  TA_BASEFUNS(KWTAVals);
};

class LEABRA_API AdaptIVals : public taBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER holds values for adapting kwta stuff
public:
  float		avg_avg;	// average of the average activation in a layer
  float		i_kwta_pt;	// adapting point to place inhibition between k and k+1 for kwta
  float		g_bar_i;	// adapting g_bar.i value 
  float		g_bar_l;	// adapting g_bar.l value 

  void	Initialize();
  void 	Destroy()	{ };
  void	Copy_(const AdaptIVals& cp);
  COPY_FUNS(AdaptIVals, taBase);
  TA_BASEFUNS(AdaptIVals);
};

class LEABRA_API InhibVals : public taBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER holds values for inhibition
public:
  float		kwta;		// inhibition due to kwta function
  float		g_i;		// overall value of the inhibition
  float		gp_g_i;		// g_i from the layer or unit group, if applicable
  float		g_i_orig; 	// original value of the inhibition (before any layer group effects set in)

  void	Initialize();
  void 	Destroy()	{ };
  void	Copy_(const InhibVals& cp);
  COPY_FUNS(InhibVals, taBase);
  TA_BASEFUNS(InhibVals);
};

class LEABRA_API LeabraInhib {
  // holds threshold-computation values, used as a parent class for layers, etc
public:
  LeabraSort 	active_buf;	// #HIDDEN #NO_SAVE list of active units
  LeabraSort 	inact_buf;	// #HIDDEN #NO_SAVE list of inactive units

  AvgMaxVals	netin;		// #READ_ONLY net input values for the layer
  AvgMaxVals	i_thrs;		// #READ_ONLY inhibitory threshold values for the layer
  AvgMaxVals	acts;		// #READ_ONLY activation values for the layer
  AvgMaxVals	acts_p;		// #READ_ONLY plus-phase activation stats for the layer
  AvgMaxVals	acts_m;		// #READ_ONLY minus-phase activation stats for the layer
  float		phase_dif_ratio; // #READ_ONLY #SHOW phase-difference ratio (acts_m.avg / acts_p.avg)
 
  KWTAVals	kwta;		// #READ_ONLY values for kwta -- activity levels, etc NOTE THIS IS A COMPUTED VALUE: k IS SET IN LayerSpec!
  InhibVals	i_val;		// #READ_ONLY #SHOW inhibitory values computed by kwta
  AvgMaxVals	un_g_i;		// #READ_ONLY average and stdev (not max) values for unit inhib-to-thresh
  AdaptIVals	adapt_i;	// #READ_ONLY #AKA_adapt_pt adapting inhibition values

  void	Inhib_SetVals(float val)	{ i_val.g_i = val; i_val.g_i_orig = val; }
  void	Inhib_ResetSortBuf() 		{ active_buf.size = 0; inact_buf.size = 0; }
  void	Inhib_InitState(LeabraLayerSpec* lay);
  void	Inhib_Initialize();
  void	Inhib_Copy_(const LeabraInhib& cp);
};

class LEABRA_API LeabraLayer : public Layer, public LeabraInhib {
  // Leabra Layer: implicit inhibition for soft kWTA behavior
public:
  LeabraLayerSpec_SPtr	spec;	// the spec for this layer: controls all functions of layer
  float		stm_gain;	// actual stim gain for soft clamping, can be incremented to ensure clamped units active
  bool		hard_clamped;	// this layer is actually hard clamped
  int		prv_phase;	// #READ_ONLY previous phase value (needed for 2nd plus phases and the like)
  float		dav;		// #READ_ONLY dopamine-like modulatory value (where applicable)
  float		net_rescale;	// #READ_ONLY computed netinput rescaling factor (updated by net_rescale)
  int		da_updt;	// #READ_ONLY true if da triggered an update (either + to store or - reset)
  int_Array	misc_iar;	// #HIDDEN misc int array of data

  void	Build();

  void	InitWtState() 	{ spec->InitWtState(this); }
  void	InitActAvg() 	{ spec->InitActAvg(this); }
  void	InitInhib() 	{ spec->InitInhib(this); } // initialize inhibitory state
  void	ModifyState()	{ spec->DecayEvent(this, NULL); } // this is what modify means..

  void	SetCurLrate(LeabraNetwork* net, int epoch) { spec->SetCurLrate(this, net, epoch); }
  
  void	Compute_Active_K()			{ spec->Compute_Active_K(this); }
  void	InitState() 				{ spec->InitState(this); }

  void	Compute_HardClamp(LeabraNetwork* net) 	{ spec->Compute_HardClamp(this, net); }
  void	Compute_NetScale(LeabraNetwork* net) 	{ spec->Compute_NetScale(this, net); }
  void	Init_ClampNet(LeabraNetwork* net) 	{ spec->Init_ClampNet(this, net); }
  void	Send_ClampNet(LeabraNetwork* net) 	{ spec->Send_ClampNet(this, net); }

  void	Send_Net()				{ spec->Send_Net(this); }
  void	Send_NetDelta()				{ spec->Send_NetDelta(this); }

  void	Compute_Clamp_NetAvg(LeabraNetwork* net)  { spec->Compute_Clamp_NetAvg(this, net); }

  void	Compute_Inhib(LeabraNetwork* net) 	{ spec->Compute_Inhib(this, net); }
  void	Compute_LayInhibToGps(LeabraNetwork* net) { spec->Compute_LayInhibToGps(this, net); }
  void	Compute_InhibAvg(LeabraNetwork* net)	{ spec->Compute_InhibAvg(this, net); }

  void	Compute_Act()				{ spec->Compute_Act(this, NULL); }
  void	Compute_Act(LeabraNetwork* net) 		{ spec->Compute_Act(this, net); }

  void	PhaseInit(LeabraNetwork* net)		{ spec->PhaseInit(this, net); }
  void	DecayEvent(LeabraNetwork* net)		{ spec->DecayEvent(this, net); } // decay between events
  void	DecayPhase(LeabraNetwork* net)    	{ spec->DecayPhase(this, net); } // decay between phases
  void	DecayPhase2(LeabraNetwork* net)  		{ spec->DecayPhase2(this, net); } // decay between 2nd set of phases

  void	ExtToComp(LeabraNetwork* net)		{ spec->ExtToComp(this, net); }
  void	TargExtToComp(LeabraNetwork* net)		{ spec->TargExtToComp(this, net); }
  void	PostSettle(LeabraNetwork* net, bool set_both=false) { spec->PostSettle(this, net, set_both); }

  void	Compute_dWt(LeabraNetwork* net) 		{ spec->Compute_dWt(this, net); }
  void	Compute_dWt() 				{ spec->Compute_dWt(this, NULL); }
  void	Compute_WtFmLin(LeabraNetwork* net) 	{ spec->Compute_WtFmLin(this, net); }
  void	UpdateWeights();

  virtual void	ResetSortBuf();

  bool		SetLayerSpec(LayerSpec* sp);
  LayerSpec*	GetLayerSpec()		{ return (LayerSpec*)spec.spec; }
  bool		CheckTypes();

  override bool  CheckConfig(bool quiet=false);

  void	UpdateAfterEdit();	// reset sort_buf after any edits..

  void	Initialize();
  void	Destroy()		{ CutLinks(); }
  void	InitLinks();
  void	CutLinks();
  void	Copy_(const LeabraLayer& cp);
  COPY_FUNS(LeabraLayer, Layer);
  TA_BASEFUNS(LeabraLayer);
};

class LEABRA_API LeabraUnit_Group : public Unit_Group, public LeabraInhib {
  // for independent subgroups of competing units within a single layer
public:
  int		misc_state;	// miscellaneous state variable
  int		misc_state1;	// second miscellaneous state variable 
  int		misc_state2;	// third miscellaneous state variable 

  void	Initialize();
  void	Destroy()		{ };
  void	InitLinks();
  void	Copy_(const LeabraUnit_Group& cp);
  COPY_FUNS(LeabraUnit_Group, Unit_Group);
  TA_BASEFUNS(LeabraUnit_Group);
};

//////////////////////////
//	Inlines		// 
//////////////////////////


//////////////////////////
//     WtSigFun 	//
//////////////////////////

void LeabraConSpec::Compute_LinFmWt(LeabraCon_Group* cg) {
  CON_GROUP_LOOP(cg, C_Compute_LinFmWt(cg, (LeabraCon*)cg->Cn(i)));
}

void LeabraConSpec::Compute_WtFmLin(LeabraCon_Group* cg) {
  CON_GROUP_LOOP(cg, C_Compute_WtFmLin(cg, (LeabraCon*)cg->Cn(i)));
}

//////////////////////////
//      Netin   	//
//////////////////////////

float LeabraConSpec::C_Compute_Net(LeabraCon* cn, Unit*, Unit* su) {
  return cn->wt * su->act;
}
float LeabraConSpec::Compute_Net(Con_Group* cg, Unit* ru) {
  float rval=0.0f;
  CON_GROUP_LOOP(cg, rval += C_Compute_Net((LeabraCon*)cg->Cn(i), ru, cg->Un(i)));
  return ((LeabraCon_Group*)cg)->scale_eff * rval;
}

void LeabraConSpec::C_Send_Inhib(LeabraCon_Group*, LeabraCon* cn, LeabraUnit* ru, float su_act_eff) {
  ru->gc.i += su_act_eff * cn->wt;
}
void LeabraConSpec::Send_Inhib(LeabraCon_Group* cg, LeabraUnit* su) {
  // apply scale based only on first unit in con group: saves lots of redundant mulitplies!
  // LeabraUnitSpec::CheckConfig checks that this is ok.
  Unit* ru = cg->Un(0);
  float su_act_eff = ((LeabraCon_Group*)ru->recv.FastGp(cg->other_idx))->scale_eff * su->act;
  CON_GROUP_LOOP(cg, C_Send_Inhib(cg, (LeabraCon*)cg->Cn(i), (LeabraUnit*)cg->Un(i), su_act_eff));
}

void LeabraConSpec::C_Send_Net(LeabraCon_Group*, LeabraCon* cn, Unit* ru, float su_act_eff) {
  ru->net += su_act_eff * cn->wt;
}
void LeabraConSpec::Send_Net(Con_Group* cg, Unit* su) {
  // apply scale based only on first unit in con group: saves lots of redundant mulitplies!
  // LeabraUnitSpec::CheckConfig checks that this is ok.
  Unit* ru = cg->Un(0);
  float su_act_eff = ((LeabraCon_Group*)ru->recv.FastGp(cg->other_idx))->scale_eff * su->act;
  if(inhib)
    CON_GROUP_LOOP(cg, C_Send_Inhib((LeabraCon_Group*)cg, (LeabraCon*)cg->Cn(i), (LeabraUnit*)cg->Un(i), su_act_eff));
  else {
    CON_GROUP_LOOP(cg, C_Send_Net((LeabraCon_Group*)cg, (LeabraCon*)cg->Cn(i), cg->Un(i), su_act_eff));
  }
}

///////////////////

void LeabraConSpec::C_Send_InhibDelta(LeabraCon_Group*, LeabraCon* cn, LeabraUnit* ru, float su_act_delta_eff) {
  ru->g_i_delta += su_act_delta_eff * cn->wt;
}
void LeabraConSpec::Send_InhibDelta(LeabraCon_Group* cg, LeabraUnit* su) {
  Unit* ru = cg->Un(0);
  float su_act_delta_eff = ((LeabraCon_Group*)ru->recv.FastGp(cg->other_idx))->scale_eff * su->act_delta;
  CON_GROUP_LOOP(cg, C_Send_InhibDelta(cg, (LeabraCon*)cg->Cn(i), (LeabraUnit*)cg->Un(i), su_act_delta_eff));
}

void LeabraConSpec::C_Send_NetDelta(LeabraCon_Group*, LeabraCon* cn, LeabraUnit* ru, float su_act_delta_eff) {
  ru->net_delta += su_act_delta_eff * cn->wt;
}

void LeabraConSpec::Send_NetDelta(LeabraCon_Group* cg, LeabraUnit* su) {
  Unit* ru = cg->Un(0);
  float su_act_delta_eff = ((LeabraCon_Group*)ru->recv.FastGp(cg->other_idx))->scale_eff * su->act_delta;
  if(inhib)
    CON_GROUP_LOOP(cg, C_Send_InhibDelta(cg, (LeabraCon*)cg->Cn(i), (LeabraUnit*)cg->Un(i), su_act_delta_eff));
  else {
    CON_GROUP_LOOP(cg, C_Send_NetDelta(cg, (LeabraCon*)cg->Cn(i), (LeabraUnit*)cg->Un(i), su_act_delta_eff));
  }
}

///////////////////

void LeabraConSpec::C_Send_ClampNet(LeabraCon_Group*, LeabraCon* cn, LeabraUnit* ru, float su_act_eff) {
  ru->clmp_net += su_act_eff * cn->wt;
}
void LeabraConSpec::Send_ClampNet(LeabraCon_Group* cg, LeabraUnit* su) {
  Unit* ru = cg->Un(0);
  float su_act_eff = ((LeabraCon_Group*)ru->recv.FastGp(cg->other_idx))->scale_eff * su->act;
  CON_GROUP_LOOP(cg, C_Send_ClampNet(cg, (LeabraCon*)cg->Cn(i), (LeabraUnit*)cg->Un(i), su_act_eff));
}

//////////////////////////
//     Computing dWt 	//
//////////////////////////

inline void LeabraConSpec::Compute_SAvgCor(LeabraCon_Group* cg, LeabraUnit*) {
  LeabraLayer* fm = (LeabraLayer*)cg->prjn->from;
  float savg = .5f + savg_cor.cor * (fm->kwta.pct - .5f);
  savg = MAX(savg_cor.thresh, savg); // keep this computed value within bounds
  cg->savg_cor = .5f / savg;
}

inline float LeabraConSpec::C_Compute_Hebb(LeabraCon* cn, LeabraCon_Group* cg,
					   float ru_act, float su_act)
{
  // wt is negative in linear form, so using opposite sign of usual here
  return ru_act * (su_act * (cg->savg_cor + cn->wt) +
		   (1.0f - su_act) * cn->wt);
}

// generec error term with sigmoid activation function, and soft bounding
inline float LeabraConSpec::C_Compute_Err
(LeabraCon* cn, float ru_act_p, float ru_act_m, float su_act_p, float su_act_m) {
  float err = (ru_act_p * su_act_p) - (ru_act_m * su_act_m);
  // wt is negative in linear form, so using opposite sign of usual here
  if(lmix.err_sb) {
    if(err > 0.0f)	err *= (1.0f + cn->wt);
    else		err *= -cn->wt;	
  }
  return err;
}

// combine hebbian and error-driven
inline void LeabraConSpec::C_Compute_dWt(LeabraCon* cn, LeabraUnit*, float heb, float err) {
  float dwt = lmix.err * err + lmix.hebb * heb;
  cn->dwt += cur_lrate * dwt;
}

inline void LeabraConSpec::Compute_dWt(Con_Group* cg, Unit* ru) {
  LeabraUnit* lru = (LeabraUnit*)ru;
  LeabraCon_Group* lcg = (LeabraCon_Group*) cg;
  Compute_SAvgCor(lcg, lru);
  if(((LeabraLayer*)cg->prjn->from)->acts_p.avg >= savg_cor.thresh) {
    for(int i=0; i<cg->size; i++) {
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
      }
    }
  }
}

inline void LeabraConSpec::C_UpdateWeights(LeabraCon* cn, LeabraCon_Group* cg,
					   LeabraUnit*, LeabraUnit*, LeabraUnitSpec*)
{
  // no act reg!
  if(cn->dwt != 0.0f) {
    C_Compute_LinFmWt(cg, cn);	// go to linear weights
    cn->wt -= cn->dwt; // wt is now negative in linear form -- signs are reversed!
    // note: this fun sets 0-1 limits automatically!
    C_Compute_WtFmLin(cg, cn);	// go back to nonlinear weights
    // then put in real limits!!
    if(cn->wt < wt_limits.min) cn->wt = wt_limits.min;
    if(cn->wt > wt_limits.max) cn->wt = wt_limits.max;
  }
  cn->pdw = cn->dwt;
  cn->dwt = 0.0f;
}

inline void LeabraConSpec::C_Compute_ActReg(LeabraCon* cn, LeabraCon_Group*,
					    LeabraUnit* ru, LeabraUnit*, LeabraUnitSpec* rus)
{
  // do this in update so inactive units can be reached (no opt_thresh.updt)
  // act_reg.on:
  float dwinc = 0.0f;
  if(ru->act_avg < rus->act_reg.min)
    dwinc = rus->act_reg.wt_dt;
  else if(ru->act_avg > rus->act_reg.max)
    dwinc = -rus->act_reg.wt_dt;
  if(dwinc != 0.0f) {
    float wtval = wt_sig_fun_inv.Eval(cn->wt);
    cn->dwt += cur_lrate * dwinc * wtval; // weight is + !
  }
}

inline void LeabraConSpec::C_UpdateWeightsActReg(LeabraCon* cn, LeabraCon_Group* cg,
						 LeabraUnit* ru, LeabraUnit* su, LeabraUnitSpec* rus)
{
  C_Compute_ActReg(cn, cg, ru, su, rus);
  C_UpdateWeights(cn, cg, ru, su, rus);
}

inline void LeabraConSpec::UpdateWeights(Con_Group* cg, Unit* ru) {
  LeabraUnitSpec* rus = (LeabraUnitSpec*)ru->spec.spec;
  LeabraCon_Group* lcg = (LeabraCon_Group*)cg;
  if(rus->act_reg.on) {		// do this in update so inactive units can be reached (no opt_thresh.updt)
    CON_GROUP_LOOP(cg, C_UpdateWeightsActReg((LeabraCon*)cg->Cn(i), lcg,
					     (LeabraUnit*)ru, (LeabraUnit*)cg->Un(i), rus));
  }
  else {
    CON_GROUP_LOOP(cg, C_UpdateWeights((LeabraCon*)cg->Cn(i), lcg,
				       (LeabraUnit*)ru, (LeabraUnit*)cg->Un(i), rus));
  }
//  ApplyLimits(cg, ru); limits are automatically enforced anyway
}

inline void LeabraConSpec::B_Compute_dWt(LeabraCon* cn, LeabraUnit* ru) {
  float err = ru->act_p - ru->act_m;
  cn->dwt += cur_lrate * err;
}
  
// default is not to do anything tricky with the bias weights
inline void LeabraConSpec::B_UpdateWeights(LeabraCon* cn, LeabraUnit* ru, LeabraUnitSpec* rus) {
  if(rus->act_reg.on) {		// do this in update so inactive units can be reached (no opt_thresh.updt)
    if(ru->act_avg < rus->act_reg.min)
      cn->dwt += cur_lrate * rus->act_reg.wt_dt;
    else if(ru->act_avg > rus->act_reg.max)
      cn->dwt -= cur_lrate * rus->act_reg.wt_dt;
  }
  cn->pdw = cn->dwt;
  cn->wt += cn->dwt;
  cn->dwt = 0.0f;
  C_ApplyLimits(cn, ru, NULL);
}

inline void LeabraBiasSpec::B_Compute_dWt(LeabraCon* cn, LeabraUnit* ru) {
  float err = ru->act_p - ru->act_m;
  if(fabsf(err) >= dwt_thresh)
    cn->dwt += cur_lrate * err;
}

//////////////////////////
// 	Network		//
//////////////////////////

class LEABRA_API LeabraNetwork : public Network {
  // Leabra network: has specific functions to operate on Leabra Networks
INHERITED(Network)
public:
  enum StateInit {		// ways of initializing the state of the network
    DO_NOTHING,			// do nothing
    INIT_STATE,			// initialize state
    DECAY_STATE			// decay the state
  };
    
  enum Phase {
    MINUS_PHASE = 0,		// minus phase
    PLUS_PHASE = 1,		// plus phase
    MINUS_2 = 2,		// second minus phase
    PLUS_2 = 3			// second plus phase
  };

  enum PhaseOrder {
    MINUS_PLUS,			// standard minus-plus (err and assoc)
    PLUS_ONLY,			// only present the plus phase (hebbian-only)
    MINUS_PLUS_NOTHING,		// auto-encoder version with final 'nothing' minus phase
    PLUS_NOTHING,		// just the auto-encoder (no initial minus phase)
    MINUS_PLUS_PLUS,		// two plus phases for gated context layer updating
    MINUS_PLUS_2		// two minus-plus phases (for pfc/bg system)
  };

  enum FirstPlusdWt {
    NO_FIRST_DWT,		// for three phase cases: don't change weights after first plus
    ONLY_FIRST_DWT,		// for three phase cases: only change weights after first plus
    ALL_DWT			// for three phase cases: change weights after *both* post-minus phases
  };

  PhaseOrder	phase_order;	// [Default: MINUS_PLUS] #CAT_Counter number and order of phases to present
  bool		no_plus_test;	// #DEF_true #CAT_Counter don't run the plus phase when testing
  StateInit	trial_init;	// #DEF_DECAY_STATE #CAT_Activation how to initialize network state at start of trial
  StateInit	sequence_init;	// #DEF_DO_NOTHING #CAT_Activation how to initialize network state at start of a sequence of trials
  FirstPlusdWt	first_plus_dwt;	// #CONDEDIT_ON_phase_order:MINUS_PLUS_PLUS #CAT_Learning how to change weights on first plus phase if 2 plus phases (applies only to standard leabralayer specs -- others must decide on their own!)

  Phase		phase;		// #GUI_READ_ONLY #SHOW #CAT_Counter type of settling phase
  int		phase_no;	// #GUI_READ_ONLY #SHOW #CAT_Counter phase as an ordinal number (regular phase is Phase enum)
  int		phase_max;	// #CAT_Counter maximum number of phases to run (note: this is set by Trial_Init depending on phase_order)

  int		cycle_max;	// #DEF_60 #CAT_Counter maximum number of cycles to settle for
  int		min_cycles;	// #DEF_15 #CAT_Counter minimum number of cycles to settle for
  int		min_cycles_phase2; // #DEF_15 #CAT_Counter minimum number of cycles to settle for in second phase
  int		netin_mod;	// #DEF_1 net #CAT_Optimization input computation modulus: how often to compute netinput vs. activation update (2 = faster)
  bool		send_delta;	// #DEF_false #CAT_Optimization send netin deltas instead of raw netin: more efficient (automatically sets corresponding unitspec flag)

  float		maxda_stopcrit;	// #DEF_0.005 #CAT_Statistic stopping criterion for max da
  float		maxda;		// #GUI_READ_ONLY #SHOW maximum #CAT_Statistic change in activation (delta-activation) over network; used in stopping settling

  float		trg_max_act_stopcrit;	// #CAT_Statistic stopping criterion for target-layer maximum activation (can be used for stopping settling)
  float		trg_max_act;	// #GUI_READ_ONLY #SHOW #CAT_Statistic target-layer maximum activation (can be used for stopping settling)

  float		ext_rew;	// #GUI_READ_ONLY #SHOW #CAT_Statistic external reward value (on this trial)
  float		avg_ext_rew;	// #GUI_READ_ONLY #SHOW #CAT_Statistic average external reward value (computed over previous epoch)
  float		avg_ext_rew_sum; // #READ_ONLY #CAT_Statistic sum for computing current average external reward value in this epoch
  int		avg_ext_rew_n;	// #READ_ONLY #CAT_Statistic N for average external reward value computation for this epoch

  override void	InitCounters();
  override void	InitStats();

  // single cycle-level functions
  virtual void	Compute_Net();	// #CAT_Cycle compute netinputs (sender based, if send_delta, then only when sender activations change)
  virtual void	Compute_Clamp_NetAvg();	// #CAT_Cycle add in clamped netinput values (computed once at start of settle) and average netinput values
  virtual void	Compute_Inhib(); // #CAT_Cycle compute inhibitory conductances (kwta)
  virtual void	Compute_InhibAvg(); // #CAT_Cycle compute average inhibitory conductances
  virtual void	Compute_Act();	// #CAT_Cycle compute activations, and max delta activation

  virtual void	Cycle_Run();	// #CAT_Cycle compute one cycle of updating: netinput, inhibition, activations

  // settling-phase level functions
  virtual void	Compute_Active_K(); // #CAT_SettleInit determine the active k values for each layer based on pcts, etc (called by Settle_Init)
  virtual void	DecayPhase();	// #CAT_SettleInit decay activations and other state between minus-plus phases (called by Settle_Init)
  virtual void	DecayPhase2();	// #CAT_SettleInit decay activations and other state between second and third phase (if applicable) (called by Settle_Init)
  virtual void	PhaseInit();	// #CAT_SettleInit initialize at start of settling phase (called by Settle_Init)
  virtual void	ExtToComp();	// #CAT_SettleInit move external input values to comparison values (not currently used)
  virtual void	TargExtToComp(); // #CAT_SettleInit move target and external input values to comparison (for PLUS_NOTHING, called by Settle_Init)
  virtual void	Compute_HardClamp(); // #CAT_SettleInit compute hard clamping from external inputs (called by Settle_Init)
  virtual void	Compute_NetScale(); // #CAT_SettleInit compute netinput scaling values by projection (called by Settle_Init)
  virtual void	Send_ClampNet(); // #CAT_SettleInit send clamped activation netinputs to other layers -- only needs to be computed once (called by Settle_Init)

  virtual void  Settle_Init();	  // #CAT_SettleInit initialize network for settle-level processing (decay, active k, hard clamp, netscale, clampnet)

  virtual void	PostSettle();	// #CAT_SettleFinal perform computations in layers at end of settling  (called by Settle_Final)
  virtual void	PostSettle_NStdLay(); // #CAT_SettleFinal perform post-settle computations in layers for non-standard layers (called by Settle_Init)

  virtual void	Settle_Final();	  // #CAT_SettleFinal do final processing after settling (postsettle, Compute_dWt if needed

  // trial-level functions
  virtual void	SetCurLrate();	// #CAT_TrialInit set the current learning rate according to the LeabraConSpec parameters
  virtual void	DecayEvent();	// #CAT_TrialInit decay activations and other state between events (trial-level)
  virtual void	DecayState();	// #CAT_TrialInit decay the state in between trials (params in LayerSpec)

  virtual void 	Trial_Init();	// #CAT_TrialInit initialize at start of trial (SetCurLrate, set phase_max, Decay state)

  virtual void	Trial_UpdatePhase(); // update phase based on phase_no -- return false if no more phases need to be run

  virtual void	EncodeState();	// #CAT_TrialFinal encode final state information for subsequent use
  virtual void	Compute_dWt_NStdLay(); // #CAT_TrialFinal compute weight change on non-nstandard layers (depends on which phase is being run)
  virtual void	Compute_dWt();	// #CAT_TrialFinal compute weight change on all layers
  virtual void	Compute_ExtRew(); // #CAT_Statistic compute external reward information
  override void	Compute_SSE();	// #CAT_Statistic compute sum squared error AND also call Compute_ExtRew
  virtual void	Trial_Final();	// #CAT_TrialFinal do final processing after trial (Compute_dWt, EncodeState)

  virtual void	Compute_AvgExtRew(); // #CAT_Statistic compute average external reward information (at an epoch-level timescale)
  override void	Compute_EpochSSE(); // #CAT_Statistic compute epoch-level sum squared error and related statistics, INCLUDING AvgExtRew

  override void	SetProjectionDefaultTypes(Projection* prjn);

  void	Initialize();
  void 	Destroy()		{}
  SIMPLE_COPY(LeabraNetwork);
  COPY_FUNS(LeabraNetwork, Network);
  TA_BASEFUNS(LeabraNetwork);
};

class LEABRA_API LeabraProject : public ProjectBase {
  // project for Leabra models
INHERITED(ProjectBase)
public:

  void	Initialize();
  void 	Destroy()		{}
  TA_BASEFUNS(LeabraProject);
};

//////////////////////////
//	Unit NetAvg   	//
//////////////////////////

void LeabraUnitSpec::Compute_NetAvg(LeabraUnit* u, LeabraLayer* lay, LeabraInhib*, LeabraNetwork* net) {
  if(act.send_delta) {
    u->net_raw += u->net_delta;
    u->net += u->clmp_net + u->net_raw;
  }
  u->net = u->prv_net + dt.net * (u->net - u->prv_net);
  u->prv_net = u->net;
  if((noise_type == NETIN_NOISE) && (noise.type != Random::NONE) && (net->cycle >= 0)) {
    u->net += noise_sched.GetVal(net->cycle) * noise.Gen();
  }
  u->i_thr = Compute_IThresh(u, lay, net);
}

void LeabraUnitSpec::Compute_InhibAvg(LeabraUnit* u, LeabraLayer*, LeabraInhib* thr, LeabraNetwork*) {
  if(act.send_delta) {
    u->g_i_raw += u->g_i_delta;
    u->gc.i = u->g_i_raw;
  }
  if(thr->i_val.g_i > 0.0f)
    u->gc.i = thr->i_val.g_i; // add in inhibition from global inhib fctn
  else {
    u->gc.i = u->prv_g_i + dt.net * (u->gc.i - u->prv_g_i);
  }
  u->prv_g_i = u->gc.i;
  // don't add -- either or!
  //  u->gc.i += g_bar.i * thr->i_val.g_i; // add in inhibition from global inhib fctn
}

//////////////////////////////////
//	Leabra Wizard		//
//////////////////////////////////

class LEABRA_API LeabraWizard : public Wizard {
  // Leabra-specific wizard for automating construction of simulation objects
public:
  override void StdNetwork(Network* net = NULL);
  // #MENU_BUTTON #MENU_ON_Network #NULL_OK make a standard network according to the current settings (if net == NULL, new network is created)

  virtual void	StdLayerSpecs(LeabraNetwork* net);
  // #MENU_BUTTON #MENU_ON_Network #MENU_SEP_BEFORE make standard layer specs for a basic Leabra network (KWTA_AVG 25% for hiddens, KWTA PAT_K for input/output)

  virtual void	SRNContext(LeabraNetwork* net);
  // #MENU_BUTTON configure a simple-recurrent-network context layer in the network

  virtual void	UnitInhib(LeabraNetwork* net, int n_inhib_units=10);
  // #MENU_BUTTON configures unit-based inhibition for all layers in the network

  virtual void 	TD(LeabraNetwork* net, bool bio_labels = false, bool td_mod_all = false);
  // #MENU_BUTTON #MENU_SEP_BEFORE configure standard TD reinforcement learning layers; bio_labels = use biologically-based labels for layers, else functional; td_mod_all = have td value modulate all the regular units in the network

  virtual void 	PVLV(LeabraNetwork* net, bool bio_labels = false, bool localist_val = true, bool fm_hid_cons=true, bool fm_out_cons=false, bool da_mod_all = false);
  // #MENU_BUTTON configure PVLV (pavlovian perceived value and local value) learning layers in a network; bio_labels = use biologically-based labels for layers, else functional; localist_val = use localist value representations for lvpv layers; da_mod_all = have da value modulate all the regular units in the network

  virtual void 	BgPFC(LeabraNetwork* net, bool bio_labels = false, bool localist_val = true, bool fm_hid_cons=true, bool fm_out_cons=false, bool da_mod_all = false, int n_stripes=4, bool mat_fm_pfc_full = false, bool out_gate=false, bool nolrn_pfc=false, bool lr_sched = true);
  // #MENU_BUTTON #MENU_SEP_BEFORE configure all the layers and specs for doing basal-ganglia based gating of the pfc layer; bio_labels = label layers with biological, else functional, names; localist_val = use localist value representations for lvpv layers; fm_hid_cons = make cons to pfc/bg fm hidden layers; fm_out_cons = from output layers; mat_fm_pfc_full = make pfc -> matrix prjn full (else stripe-specific); out_gate = each PFC layer has separate output gated layer and corresponding matrix output gates; patch per-stripe learning system optional; nolrn_pfc = pfc does not learn -- just copies input acts directly; da_mod_all = have da value modulate all the regular units in the network; lr_sched = make a learning rate schedule on BG learn cons

  virtual void SetPFCStripes(LeabraNetwork* net, int n_stripes, int n_units=-1);
  // #MENU_BUTTON set number of "stripes" (unit groups) throughout the entire set of pfc/bg layers (n_units = -1 = use current # of units)

  override void	StdProgs();

  void 	Initialize();
  void 	Destroy()	{ };
//   SIMPLE_COPY(LeabraWizard);
//   COPY_FUNS(LeabraWizard, Wizard);
  TA_BASEFUNS(LeabraWizard);
};

#endif // leabra_h
