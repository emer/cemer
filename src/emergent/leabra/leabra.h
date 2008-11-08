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

#ifndef leabra_h
#define leabra_h

#include "emergent_base.h"
#include "netstru_extra.h"
#include "emergent_project.h"

#include "fun_lookup.h"

#include "leabra_def.h"
#include "leabra_TA_type.h"

// pre-declare

class LeabraCon;
class LeabraConSpec;
class LeabraBiasSpec;
class LeabraRecvCons;
class LeabraSendCons;

class LeabraUnitSpec;
class LeabraUnit;
class LeabraUnit_Group;

class LeabraPrjn;

class LeabraInhib;
class LeabraLayerSpec;
class LeabraLayer;

class LeabraNetwork;
class LeabraProject; 

class LeabraEngine;
class LeabraTask;
TA_SMART_PTRS(LeabraTask);//

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

// NEW as of 1/29/08: the weight value in the connection object 
// is ALWAYS the contrast-enhanced sigmoidal weight value
// where the underlying internal linear weight value is needed
// (i.e., during learning), it is computed and passed around
// as needed. This is much more efficient and less confusing
// than the prior business of converting the actual value
// back and forth and using + and - values to indicate this

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
  

// todo: if at any point CAL is dropped in favor of XCAL, then sravg_s and sravg_m can be 
// eliminated from the connection (at considerable performance savings) and ravg_s, ravg_m 
// added to the unit

class LeabraCon : public Connection {
  // #STEM_BASE ##CAT_Leabra Leabra connection
public:
  float		pdw;		// #NO_SAVE previous delta-weight change -- useful for viewing because current weight change (dwt) is typically reset to 0 when views are updated
  float		sravg_s;	// #NO_SAVE short time-scale, most recent (plus phase) average of sender and receiver activation product over time
  float		sravg_m;	// #NO_SAVE medium time-scale, trial-level average of sender and receiver activation product over time
  
  LeabraCon() { pdw = 0.0f; sravg_s = sravg_m = 0.0f; }
};

class LEABRA_API WtScaleSpec : public taBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra weight scaling specification
INHERITED(taBase)
public:
  float		abs;		// #DEF_1 absolute scaling (not subject to normalization: directly multiplies weight values)
  float		rel;		// [Default: 1] relative scaling that shifts balance between different projections (subject to normalization across all other projections into unit)

  inline float	NetScale() 	{ return abs * rel; }

  void 	Defaults()	{ Initialize(); }
  TA_SIMPLE_BASEFUNS(WtScaleSpec);
private:
  void	Initialize();
  void	Destroy()	{ };
};

class LEABRA_API WtScaleSpecInit : public taBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra initial weight scaling values -- applied to active WtScaleSpec values during InitWeights -- useful for adapting scale values
INHERITED(taBase)
public:
  bool		init;		// #APPLY_IMMED use these scaling values to initialize the wt_scale parameters during InitWeights (if false, these values have no effect at all)
  float		abs;		// #CONDEDIT_ON_init #DEF_1 absolute scaling (not subject to normalization: directly multiplies weight values)
  float		rel;		// #CONDEDIT_ON_init [Default: 1] relative scaling that shifts balance between different projections (subject to normalization across all other projections into unit)

  void 	Defaults()	{ Initialize(); }
  TA_SIMPLE_BASEFUNS(WtScaleSpecInit);
private:
  void	Initialize();
  void	Destroy()	{ };
};

class LEABRA_API WtSigSpec : public taOBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra sigmoidal weight function specification
INHERITED(taOBase)
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

  void 	Defaults()	{ Initialize(); }
  TA_SIMPLE_BASEFUNS(WtSigSpec);
protected:
  void	UpdateAfterEdit_impl();
private:
  void	Initialize();
  void	Destroy()	{ };
};

class LEABRA_API LearnMixSpec : public taBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra Leabra CHL mixture of learning factors (hebbian vs. error-driven) specification
INHERITED(taBase)
public:
  float		hebb;		// [Default: .01] amount of hebbian learning (should be relatively small, can be effective at .0001)
  float		err;		// #READ_ONLY #SHOW [Default: .99] amount of error driven learning, automatically computed to be 1-hebb
  bool		err_sb;		// #DEF_true apply exponential soft-bounding to the error learning component (applied in dWt)

  void 	Defaults()	{ Initialize(); }
  TA_SIMPLE_BASEFUNS(LearnMixSpec);
protected:
  void	UpdateAfterEdit_impl();
private:
  void	Initialize();
  void	Destroy()	{ };
};

class LEABRA_API XCalLearnSpec : public taOBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra CtLeabra eXtended Contrastive Attractor Learning (XCAL) specs
INHERITED(taOBase)
public:
  enum AvgUpdt {
    TRIAL,		// lock the updating of averages to the trial
    CONT,		// just continuously update averages according to different time constants
    CONT_CASC,		// just continuously update averages according to different time constants -- cascading the time constant values into each other (s updates m, m updates l)
  };

  enum ActNorm {
    KWTA_PCT,			// use slay.kwta_pct to normalize ravg_l value for learning
    RAVG_L_AVG,			// use slay.ravg_l_avg to normalize ravg_l value for learning
    NO_NORM,			// don't normalize ravg_l value for learning
  };

  enum LearnVar {
    AVG_PROD,			// XCAL default learning rule: use product of individual unit averages (from bias weight) for short and medium time scale
    CAL,			// use standard contrastive attractor learning rule (CAL) -- good for output layers because the homeostasis factor in XCAL which may not be consistent with the statistics of the output units
  };

  LearnVar	lrn_var;	// learning rule -- for xcal how to compute the short and medium timescale variables (AVG_PROD), or CAL for output layers or comparison purposes (can also be used for bias weights)
  float		lrn_s_mix;	// #DEF_0.8 #CONDEDIT_OFF_lrn_var:CAL how much the short time-scale (plus phase) sravg contributes to sravg term that drives learning -- the rest (1-s_mix) is medium time-scale (trial) sravg -- in addition to general recency effects, s_pct can also reflect dopamine and other neuromodulatory factors
  float		lrn_m_mix;	// #READ_ONLY #CONDEDIT_OFF_lrn_var:CAL 1-lrn_s_mix -- amount that sravg_m contributes to learning
  float		l_dt;		// #DEF_0.03 #CONDEDIT_OFF_lrn_var:CAL time constant for updating the long time-scale ravg_l value -- note this is ONLY applicable on the unit bias con spec, where it updates the unit-level ravg_l variable!!
  float		l_gain;		// #DEF_4 #CONDEDIT_OFF_lrn_var:CAL gain for long time-scale ravg term -- needed to put into same terms as the sravg values used in the s and m components of learning (note ravg_l is already multiplied by sending kwta_pct or ravg_l_avg to compensate for overall layer activation differences) -- use 5 for KWTA_PCT, 6-7 for RAVG_L_AVG and around 1-2 for NO_NORM, generally
  ActNorm	l_norm;		// #DEF_KWTA_PCT #CONDEDIT_OFF_lrn_var:CAL how to normalize the ravg_l values for learning 

  AvgUpdt	avg_updt;	// #DEF_TRIAL #CONDEDIT_OFF_lrn_var:CAL how to update the relevant sr average variables -- CONT versions are still untested, and still use trial level timing, just cont updating
  float		m_dt;		// #CONDSHOW_ON_avg_updt:CONT time constant for updating the medium time-scale sravg_m value
  float		s_dt;		// #CONDSHOW_ON_avg_updt:CONT time constant for updating the short time-scale sravg_s value
  // todo: need some params like this for continuous mode -- currently still use trial-wise hooks
//   float		lrn_thr;	// #CONDSHOW_ON_avg_updt:CONT threshold on sravg_m value to initiate learning, in continous mode
//   int		lrn_delay;	// #CONDSHOW_ON_avg_updt:CONT delay after lrn_thr threshold has been crossed after which learning occurs

  float		d_gain;		// #DEF_1.5 #CONDEDIT_OFF_lrn_var:CAL multiplier on LTD values relative to LTP values
  float		d_rev;		// #DEF_0.15 #CONDEDIT_OFF_lrn_var:CAL proportional point within LTD range where magnitude reverses to go back down to zero at zero sravg

  bool		use_nd;		// #DEF_false use the act_nd variables (non-depressed) for computing sravg/ravg terms (else use raw act, which is raw spikes in spiking mode, and subject to depression if in place)

  float		avg_init;	// #DEF_0.15 initial value for averages
  float		rnd_min_avg;	// #DEF_-1 [use .01] minimum ravg_l value, below which random values are added to weights to drive exploration (-1 = off)
  float		rnd_var;	// #DEF_0.1 #CONDEDIT_OFF_rnd_min_avg:-1 variance (range) for uniform random noise added to weights when avg_trl_avg < rnd_min_avg (noise is then multiplied by lrate)

  float		d_rev_ratio;	// #HIDDEN #READ_ONLY (1-d_rev)/d_rev -- muliplication factor in learning rule

  inline float  dWtFun(float srval, float thr_p) {
    float rval;
    if(srval >= thr_p)
      rval = srval - thr_p;
    else if(srval > thr_p * d_rev)
      rval = d_gain * (srval - thr_p);
    else
      rval = -d_gain * srval * d_rev_ratio;
    return rval;
  }

  SIMPLE_COPY(XCalLearnSpec);
  TA_BASEFUNS(XCalLearnSpec);
protected:
  void	UpdateAfterEdit_impl();
private:
  void	Initialize();
  void 	Destroy()	{ };
};

class LEABRA_API SAvgCorSpec : public taBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra sending average activation correction specifications: affects hebbian learning and netinput computation
INHERITED(taBase)
public:
  float		cor;		// #DEF_0.4 proportion of correction to apply (0=none, 1=all, .5=half, etc)
  float		thresh;		// #DEF_0.001 threshold of sending average activation below which learning does not occur (prevents learning when there is no input)
  bool		norm_con_n;	// #DEF_false #AKA_div_gp_n in normalizing netinput, divide by the actual number of connections (recv group n), not the overall number of units in the sending layer; this is good when units only receive from a small fraction of sending layer units

  void 	Defaults()	{ Initialize(); }
  TA_SIMPLE_BASEFUNS(SAvgCorSpec);
private:
  void	Initialize();
  void	Destroy()	{ };
};

class LEABRA_API AdaptRelNetinSpec : public taBase {
  // ##INLINE ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra parameters to adapt the relative netinput strength of different projections (to be used at epoch-level in AdaptRelNetin call, after AvgAbsRelNetin vals on projection have been computed)
INHERITED(taBase)
public:
  bool		on;		// #APPLY_IMMED whether to adapt relative netinput values for this connection (only applied if AdaptAbsNetin is called, after AbsRelNetin and AvgAbsRelNetin)
  float		trg_fm_input;	// #CONDEDIT_ON_on:true (typically 0.85) target relative netinput for fm_input projections (set by Compute_TrgRelNetin fun): all such projections should sum to this amount (divide equally among them) -- this plus fm_output and lateral should sum to 1. if other types are missing, this is increased in proportion
  float		trg_fm_output;	// #CONDEDIT_ON_on:true (typically 0.10) target relative netwinput for fm_output projections (set by Compute_TrgRelNetin fun): all such projections should sum to this amount (divide equally among them) -- this plus fm_input and lateral should sum to 1. if other types are missing, this is increased in proportion
  float		trg_lateral;	// #CONDEDIT_ON_on:true (typically 0.05) target relative netinput for lateral projections (set by Compute_TrgRelNetin fun): all such projections should sum to this amount (divide equally among them)  -- this plus fm_input and lateral should sum to 1.  if other types are missing, this is increased in proportion
  float		trg_sum;	// #READ_ONLY #SHOW sum of trg values -- should be 1!

  float		tol_lg;		// #CONDEDIT_ON_on:true #DEF_0.05 tolerance from target value (as a proportion of target value) on large numbers (>.25), within which parameters are not adapted
  float		tol_sm;		// #CONDEDIT_ON_on:true #DEF_0.2 tolerance from target value (as a proportion of target value) on small numbers (<.25), within which parameters are not adapted
  float		rel_lrate;	// #CONDEDIT_ON_on:true #DEF_0.2 adpatation 'learning' rate on wt_scale.rel parameter

  virtual bool	CheckInTolerance(float trg, float val);
  // check if value is inside the tolerance from trg

  void 	Defaults()	{ Initialize(); }
  TA_SIMPLE_BASEFUNS(AdaptRelNetinSpec);
protected:
  void	UpdateAfterEdit_impl();
private:
  void	Initialize();
  void	Destroy()	{ };
};

class LEABRA_API LeabraConSpec : public ConSpec {
  // #STEM_BASE ##CAT_Leabra Leabra connection specs
INHERITED(ConSpec)
public:
  enum LearnRule {
    LEABRA_CHL,			// standard Leabra Contrastive Hebbian Learning rule: (s+r+) - (s-r-) (s=sender,r=recv +=plus phase, -=minus phase)
    CTLEABRA_CAL,		// continuous-time Leabra Contrastive Attractor Learning rule: <sr>_s - <sr>_m -- s=sender, r=recv, <> = avg over short (plus phase) and medium (trial) time scales
    CTLEABRA_XCAL,		// continuous-time Leabra temporally eXtended Contrastive Attractor Learning rule: <sr>_s - <sr>_m - <r>_l -- s=sender, r=recv, <> = avg over short (plus phase), medium (trial), long (epoch) time scales
  };

  enum	LRSValue {		// what value to drive the learning rate schedule with
    NO_LRS,			// don't use a learning rate schedule
    EPOCH,			// current epoch counter
    EXT_REW_STAT,		// avg_ext_rew value on network (computed over an "epoch" of training): value is * 100 (0..100) 
    EXT_REW_AVG	= 0x0F,		// uses average reward computed by ExtRew layer (if present): value is units[0].act_avg (avg_rew) * 100 (0..100) 
  };

  LearnRule	learn_rule;	// #READ_ONLY #SHOW the learning rule, set by the overall network parameter and copied here -- determines what type of learning to perform
  bool		inhib;		// #DEF_false #CAT_Activation makes the connection inhibitory (to g_i instead of net)
  WtScaleSpec	wt_scale;	// #CAT_Activation scale effective weight values to control the overall strength of a projection -- relative shifts balance among different projections, while absolute is a direct multipler
  WtScaleSpecInit wt_scale_init;// #CAT_Activation initial values of wt_scale parameters, set during InitWeights -- useful for rel_net_adapt and abs_net_adapt (on LayerSpec)

  float		lrate;		// #CAT_Learning [0.01 for std leabra, .04-.08 for CtLeabra] learning rate -- how fast do the weights change per experience
  float		cur_lrate;	// #READ_ONLY #NO_INHERIT #SHOW #CAT_Learning current actual learning rate = lrate * lrate_sched current value (* 1 if no lrate_sched)
  LRSValue	lrs_value;	// #CAT_Learning what value to drive the learning rate schedule with (Important: affects values entered in start_ctr fields of schedule!)
  Schedule	lrate_sched;	// #CAT_Learning schedule of learning rate over training epochs or as a function of performance, as determined by lrs_value (NOTE: these factors multiply lrate to give the cur_lrate value)

  WtSigSpec	wt_sig;		// #CAT_Learning #CONDSHOW_ON_learn_rule:LEABRA_CHL sigmoidal weight function for contrast enhancement: high gain makes weights more binary & discriminative
  LearnMixSpec	lmix;		// #CAT_Learning #CONDSHOW_ON_learn_rule:LEABRA_CHL mixture of hebbian & err-driven learning (note: no hebbian for CTLEABRA_XCAL)
  XCalLearnSpec	xcal;		// #CAT_Learning #CONDSHOW_ON_learn_rule:CTLEABRA_XCAL XCAL (eXtended Contrastive Attractor Learning) learning parameters
  SAvgCorSpec	savg_cor;	// #CAT_Learning for Hebbian and netinput computation: correction for sending average act levels (i.e., renormalization); also norm_con_n for normalizing netinput computation

  AdaptRelNetinSpec rel_net_adapt; // #CAT_Learning adapt relative netinput values based on targets for fm_input, fm_output, and lateral projections -- not used by default (call Compute_RelNetinAdapt to activate; requires Compute_RelNetin and Compute_AvgRelNetin for underlying data)
  
  FunLookup	wt_sig_fun;	// #HIDDEN #NO_SAVE #NO_INHERIT #CAT_Learning computes wt sigmoidal fun
  FunLookup	wt_sig_fun_inv;	// #HIDDEN #NO_SAVE #NO_INHERIT #CAT_Learning computes inverse of wt sigmoidal fun
  WtSigSpec	wt_sig_fun_lst;	// #HIDDEN #NO_SAVE #NO_INHERIT #CAT_Learning last values of wt sig parameters for which the wt_sig_fun's were computed; prevents excessive updating
  float		wt_sig_fun_res;	// #HIDDEN #NO_SAVE #NO_INHERIT #CAT_Learning last values of resolution parameters for which the wt_sig_fun's were computed

  float		GetWtFmLin(float lin_wt) { return wt_sig_fun.Eval(lin_wt);  }
  // #CAT_Learning get contrast-enhanced weight from linear weight value
  float		GetLinFmWt(float sig_wt)     { return wt_sig_fun_inv.Eval(sig_wt); }
  // #CAT_Learning get linear weight value from contrast-enhanced sigmoidal weight value

  virtual void	C_Init_Weights_Post(RecvCons*, Connection*, Unit*, Unit*) { };
  // #CAT_Learning hook for setting other weight-like values after initializing the weight value

  inline void 	C_Init_Weights(RecvCons* cg, Connection* cn, Unit* ru, Unit* su) {
    ConSpec::C_Init_Weights(cg, cn, ru, su); LeabraCon* lcn = (LeabraCon*)cn;
    lcn->pdw = 0.0f; lcn->sravg_s = lcn->sravg_m = xcal.avg_init;
    C_Init_Weights_Post(cg, cn, ru, su); }
  inline override void Init_Weights(RecvCons* cg, Unit* ru) {
    ConSpec::Init_Weights(cg, ru);
    if(wt_scale_init.init) { wt_scale.abs = wt_scale_init.abs;
      wt_scale.rel = wt_scale_init.rel; } }

  ///////////////////////////////////////////////////////////////
  //		Activation

  inline float 	C_Compute_Netin(LeabraCon* cn, Unit*, Unit* su);
  inline override float Compute_Netin(RecvCons* cg, Unit* ru);
  // #CAT_Activation receiver-based net input 

  inline void 	C_Send_Netin(LeabraSendCons* cg, LeabraCon* cn, Unit* ru, float su_act_eff);
  inline override void 	Send_Netin(SendCons* cg, float su_act);
  // #CAT_Activation sender-based net input computation

  inline void 	C_Send_Inhib(LeabraSendCons* cg, LeabraCon* cn, LeabraUnit* ru, float su_act_eff);
  // #CAT_Activation sender-based inhibitiory net input computation

  inline void 	C_Send_NetinDelta(LeabraSendCons* cg, LeabraCon* cn, LeabraUnit* ru, float su_act_delta_eff);
  // #CAT_Activation sender-based delta net input computation (send_delta mode only)
  inline virtual void Send_NetinDelta(LeabraSendCons* cg, float su_act_delta);
  // #CAT_Activation sender-based delta net input computation (send_delta mode only)

  inline void 	C_Send_InhibDelta(LeabraSendCons* cg, LeabraCon* cn, LeabraUnit* ru, float su_act_delta_eff);
  // #CAT_Activation sender-based delta inhibitiory net input computation (send_delta mode only)

  inline void 	C_Send_ClampNet(LeabraSendCons* cg, LeabraCon* cn, LeabraUnit* ru, float su_act_eff);
  // #CAT_Activation sender-based net input computation for clamp net
  inline virtual void Send_ClampNet(LeabraSendCons* cg, float su_act);
  // #CAT_Activation sender-based net input computation for clamp net


  ///////////////////////////////////////////////////////////////
  //		Learning

  /////////////////////////////////////
  // LeabraCHL code

  inline virtual void Compute_SAvgCor(LeabraRecvCons* cg, LeabraUnit* ru);
  // #CAT_Learning compute hebb correction scaling term for sending average act (cg->savg_cor) based on layer target activity percent

  inline float	C_Compute_Hebb(LeabraCon* cn, LeabraRecvCons* cg, float lin_wt,
			       float ru_act, float su_act);
  // #CAT_Learning compute Hebbian associative learning

  inline float 	C_Compute_Err_LeabraCHL(LeabraCon* cn, float lin_wt,
					float ru_act_p, float ru_act_m,
					float su_act_p, float su_act_m);
  // #CAT_Learning compute generec error term, sigmoid case

  inline void 	C_Compute_dWt(LeabraCon* cn, LeabraUnit* ru, float heb, float err);
  // #CAT_Learning combine associative and error-driven weight change, actually update dwt
  inline void 	C_Compute_dWt_NoHebb(LeabraCon* cn, LeabraUnit* ru, float err);
  // #CAT_Learning just error-driven weight change, actually update dwt
  inline virtual void 	Compute_dWt_LeabraCHL(LeabraRecvCons* cg, LeabraUnit* ru);
  // #CAT_Learning Leabra/CHL weight changes

  inline void	C_Compute_Weights_LeabraCHL(LeabraCon* cn);
  // #CAT_Learning compute weights for LeabraCHL learning rule
  inline virtual void	Compute_Weights_LeabraCHL(LeabraRecvCons* cg, LeabraUnit* ru);
  // #CAT_Learning overall compute weights for LeabraCHL learning rule

  /////////////////////////////////////
  // CtLeabraXCAL code

  inline void C_Compute_SRAvg_m(LeabraCon* cn, float ru_act, float su_act);
  // accumulate sender-receiver activation product average -- medium (trial-level) time scale
  inline void C_Compute_SRAvg_ms(LeabraCon* cn, float ru_act, float su_act);
  // accumulate sender-receiver activation product average -- medium (trial-level) and short (plus phase) time scales
  inline void Compute_SRAvg(LeabraRecvCons* cg, LeabraUnit* ru, bool do_s);
  // accumulate sender-receiver activation product average -- only for CtLeabraCAL

  inline void C_Init_SRAvg(LeabraCon* cn);
  // initialize sender-receiver activation product averages for trial and below 
  inline void Init_SRAvg(LeabraRecvCons* cg, LeabraUnit* ru);
  // initialize sender-receiver activation product average (only for trial-wise mode, else just in init weights) -- called at start of trial

  inline void 	C_Compute_dWt_CtLeabraXCAL_avgprod(LeabraCon* cn, LeabraCon* rbias, LeabraCon* sbias,
				 float ru_ravg_l, float sravg_s_nrm, float sravg_m_nrm);
  // #CAT_Learning compute eXtended Contrastive Attractor Learning (XCAL) -- trial-wise version (requires normalization factors)
  inline void 	C_Compute_dWt_CtLeabraXCAL_cont(LeabraCon* cn, float ru_ravg_l);
  // #CAT_Learning compute eXtended Contrastive Attractor Learning (XCAL) -- continuous version (requires normalization factors) -- todo: needs timing things..

  inline virtual void 	Compute_dWt_CtLeabraXCAL(LeabraRecvCons* cg, LeabraUnit* ru);
  // #CAT_Learning CtLeabraXCAL weight changes

  inline void	C_Compute_dWt_Rnd_XCAL(LeabraCon* cn, float rnd_var);
  // #CAT_Learning random weight changes: connection level
  inline void	Compute_dWt_Rnd_XCAL(LeabraRecvCons* cg, LeabraUnit* ru, float rnd_var);
  // #CAT_Learning random weight changes

  inline void	C_Compute_Weights_CtLeabraXCAL(LeabraCon* cn);
  // #CAT_Learning overall compute weights for CtLeabraXCAL learning rule
  inline virtual void	Compute_Weights_CtLeabraXCAL(LeabraRecvCons* cg, LeabraUnit* ru);
  // #CAT_Learning overall compute weights for CtLeabraXCAL learning rule


  /////////////////////////////////////
  // CtLeabraCAL code

  inline void 	C_Compute_dWt_CtLeabraCAL(LeabraCon* cn,
					  float sravg_s_nrm, float sravg_m_nrm);
  // #CAT_Learning compute Contrastive Attractor Learning (CAL)
  inline virtual void 	Compute_dWt_CtLeabraCAL(LeabraRecvCons* cg, LeabraUnit* ru);
  // #CAT_Learning CtLeabraCAL weight changes

  inline void	C_Compute_Weights_CtLeabraCAL(LeabraCon* cn);
  // #CAT_Learning overall compute weights for CtLeabraCAL learning rule
  inline virtual void	Compute_Weights_CtLeabraCAL(LeabraRecvCons* cg, LeabraUnit* ru);
  // #CAT_Learning overall compute weights for CtLeabraCAL learning rule

  /////////////////////////////////////
  // 	Bias Weights

  inline virtual void	B_Compute_dWt_LeabraCHL(LeabraCon* cn, LeabraUnit* ru);
  // #CAT_Learning compute bias weight change for netin model of bias weight

  inline virtual void 	B_Compute_SRAvg(LeabraCon* cn, LeabraUnit* ru, bool do_s);
  // #CAT_Learning compute bias weight sender-receiver average (actually just receiver)

  inline virtual void 	B_Init_SRAvg(LeabraCon* cn, LeabraUnit* ru, LeabraLayer* rlay);
  // #CAT_Learning initialize bias weight sender-receiver average (actually just receiver)

  inline virtual void 	B_Init_RAvg_l(LeabraCon* cn, LeabraUnit* ru);
  // #CAT_Learning initialize long-time-scale sravg value, stored on the unit (called during init weights)

  inline virtual void	B_Compute_dWt_CtLeabraXCAL(LeabraCon* cn, LeabraUnit* ru,
							 LeabraLayer* rlay);
  // #CAT_Learning compute bias weight change for XCAL rule
  inline virtual void	B_Compute_dWt_CtLeabraCAL(LeabraCon* cn, LeabraUnit* ru,
						  LeabraLayer* rlay);
  // #CAT_Learning compute bias weight change for CAL rule

  inline virtual void	B_Compute_Weights(LeabraCon* cn, LeabraUnit* ru);
  // #CAT_Learning update weights for bias connection (same for all algos)

  /////////////////////////////////////
  // General 

  virtual void Compute_CycSynDep(LeabraRecvCons* cg, LeabraUnit* ru) { };
  // #CAT_Activation compute cycle-level synaptic depression (must be defined by appropriate subclass) -- called at end of each cycle of computation if net_misc.cyc_syn_dep is on.

  inline override void 	Compute_dWt(RecvCons* cg, Unit* ru) {
    if(learn_rule == LEABRA_CHL)
      Compute_dWt_LeabraCHL((LeabraRecvCons*)cg, (LeabraUnit*)ru);
    else if(learn_rule == CTLEABRA_CAL)
      Compute_dWt_CtLeabraCAL((LeabraRecvCons*)cg, (LeabraUnit*)ru);
    else // if(learn_rule == CTLEABRA_XCAL)
      Compute_dWt_CtLeabraXCAL((LeabraRecvCons*)cg, (LeabraUnit*)ru);
  }
  // #CAT_Learning do not redefine this function -- just splits out which code is relevant -- not actually called by the unitspec Compute_dWt function

  inline override void	Compute_Weights(RecvCons* cg, Unit* ru) {
    if(learn_rule == LEABRA_CHL)
      Compute_Weights_LeabraCHL((LeabraRecvCons*)cg, (LeabraUnit*)ru);
    else if(learn_rule == CTLEABRA_CAL)
      Compute_Weights_CtLeabraCAL((LeabraRecvCons*)cg, (LeabraUnit*)ru);
    else // if(learn_rule == CTLEABRA_XCAL)
      Compute_Weights_CtLeabraXCAL((LeabraRecvCons*)cg, (LeabraUnit*)ru);
  }
  // #CAT_Learning do not redefine this function -- just splits out which code is relevant -- not actually called by the unitspec Compute_Weights function

  virtual void	SetCurLrate(LeabraNetwork* net, int epoch);
  // #CAT_Learning set current learning rate based on schedule given epoch (or error value)
  virtual void	SetLearnRule(LeabraNetwork* net);
  // #CAT_Learning set current learning rule from the network

  virtual void	CreateWtSigFun(); // #CAT_Learning create the wt_sig_fun and wt_sig_fun_inv

  virtual void	Defaults();	// #BUTTON #CONFIRM #CAT_ObjectMgmt restores default parameter settings: warning -- you will lose any unique parameters you might have set!

  virtual void	GraphWtSigFun(DataTable* graph_data = NULL);
  // #BUTTON #NULL_OK graph the sigmoidal weight contrast enhancement function (NULL = new data table)
  virtual void	GraphXCaldWtFun(DataTable* graph_data = NULL, float thr_p = 0.25);
  // #BUTTON #NULL_OK graph the xcal dWt function for given threshold value (NULL = new data table)

  void	InitLinks();
  SIMPLE_COPY(LeabraConSpec);
  TA_BASEFUNS(LeabraConSpec);
protected:
  void	UpdateAfterEdit_impl();
private:
  void 	Initialize();
  void	Destroy()		{ };
};
TA_SMART_PTRS(LeabraConSpec)

class LEABRA_API LeabraBiasSpec : public LeabraConSpec {
  // Leabra bias-weight connection specs (bias wts are a little bit special)
INHERITED(LeabraConSpec)
public:
  float		dwt_thresh;  // #DEF_0.1 #CAT_Learning don't change if dwt < thresh, prevents buildup of small changes

  inline override void	B_Compute_dWt_LeabraCHL(LeabraCon* cn, LeabraUnit* ru);
  inline override void	B_Compute_dWt_CtLeabraXCAL(LeabraCon* cn, LeabraUnit* ru,
						   LeabraLayer* rlay);
  inline override void	B_Compute_dWt_CtLeabraCAL(LeabraCon* cn, LeabraUnit* ru,
						   LeabraLayer* rlay);

  bool	CheckObjectType_impl(TAPtr obj);

  void	Defaults();
  TA_SIMPLE_BASEFUNS(LeabraBiasSpec);
private:
  void 	Initialize();
  void	Destroy()		{ };
};
TA_SMART_PTRS(LeabraBiasSpec)

class LEABRA_API LeabraRecvCons : public RecvCons {
  // #STEM_BASE ##CAT_Leabra Leabra receiving connection group
INHERITED(RecvCons)
public:
  float		scale_eff;	// #NO_SAVE #CAT_Activation effective scale parameter for netin
  float		savg_cor;	// #NO_SAVE #CAT_Learning savg correction factor for hebbian learning
  float		net;		// #NO_SAVE #CAT_Activation netinput to this con_group: only computed for special statistics such as RelNetin

  void	C_Init_Weights_Post(Connection* cn, Unit* ru, Unit* su) { 
    ((LeabraConSpec*)GetConSpec())->C_Init_Weights_Post(this, cn, ru, su);
  }
  // #CAT_Learning hook for setting other weight-like values after initializing the weight value

  void	Compute_dWt_LeabraCHL(LeabraUnit* ru)
  { ((LeabraConSpec*)GetConSpec())->Compute_dWt_LeabraCHL(this, ru); }
  // #CAT_Learning compute weight changes: Leabra CHL version
  void	Compute_Weights_LeabraCHL(LeabraUnit* ru)
  { ((LeabraConSpec*)GetConSpec())->Compute_Weights_LeabraCHL(this, ru); }
  // #CAT_Learning compute weights: Leabra CHL version

  void	Compute_SRAvg(LeabraUnit* ru, bool do_s)
  { ((LeabraConSpec*)GetConSpec())->Compute_SRAvg(this, ru, do_s); }
  // #CAT_Learning compute sending-receiving activation product averages
  void	Init_SRAvg(LeabraUnit* ru)
  { ((LeabraConSpec*)GetConSpec())->Init_SRAvg(this, ru); }
  // #CAT_Learning initialize sending-receiving activation product averages

  void	Compute_dWt_CtLeabraXCAL(LeabraUnit* ru)
  { ((LeabraConSpec*)GetConSpec())->Compute_dWt_CtLeabraXCAL(this, ru); }
  // #CAT_Learning compute weight changes: CtLeabra XCAL version
  void	Compute_Weights_CtLeabraXCAL(LeabraUnit* ru)
  { ((LeabraConSpec*)GetConSpec())->Compute_Weights_CtLeabraXCAL(this, ru); }
  // #CAT_Learning compute weights: CtLeabra XCAL version

  void	Compute_dWt_CtLeabraCAL(LeabraUnit* ru)
  { ((LeabraConSpec*)GetConSpec())->Compute_dWt_CtLeabraCAL(this, ru); }
  // #CAT_Learning compute weight changes: CtLeabra CAL version
  void	Compute_Weights_CtLeabraCAL(LeabraUnit* ru)
  { ((LeabraConSpec*)GetConSpec())->Compute_Weights_CtLeabraCAL(this, ru); }
  // #CAT_Learning compute weights: CtLeabra CAL version

  void	Compute_CycSynDep(LeabraUnit* ru)
  { ((LeabraConSpec*)GetConSpec())->Compute_CycSynDep(this, ru); }
  // #CAT_Activation compute cycle-level synaptic depression (must be defined by appropriate subclass) -- called at end of each cycle of computation if net_misc.cyc_syn_dep is on.

  void	Copy_(const LeabraRecvCons& cp);
  TA_BASEFUNS(LeabraRecvCons);
private:
  void 	Initialize();
  void	Destroy()		{ };
};

class LEABRA_API LeabraSendCons : public SendCons {
  // #STEM_BASE ##CAT_Leabra Leabra sending connection group
INHERITED(SendCons)
public:
  inline void 	Send_ClampNet(float su_act)
  { ((LeabraConSpec*)GetConSpec())->Send_ClampNet(this, su_act); }
  // #CAT_Activation send input from clamped layers

  inline void 	Send_Netin(float su_act)
  { ((LeabraConSpec*)GetConSpec())->Send_Netin(this, su_act); }
  // #CAT_Activation send delta-netin

  inline void 	Send_NetinDelta(float su_act_delta)
  { ((LeabraConSpec*)GetConSpec())->Send_NetinDelta(this, su_act_delta); }
  // #CAT_Activation send delta-netin

  TA_BASEFUNS_NOCOPY(LeabraSendCons);
private:
  void 	Initialize();
  void	Destroy()		{ };
};


//////////////////////////////////////////////////////////////////////////
//			Unit Level Code

class LEABRA_API ActFunSpec : public taOBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra activation function specifications
INHERITED(taOBase)
public:
  enum IThrFun {	       
    STD,			// include all currents (except bias weights) in inhibitory threshold computation
    NO_A,			// exclude gc.a current: allows accommodation to knock out units from kwta active list, without other units coming in to take their place
    NO_H,			// exclude gc.h current: prevent hysteresis current from affecting inhibitory threshold computation
    NO_AH,			// exclude gc.a and gc.h currents: prevent both accommodation and hysteresis currents from affecting inhibitory threshold computation
    ALL,			// include all currents INCLUDING bias weights
  };

  float		thr;		// #DEF_0.25 threshold value Theta (Q) for firing output activation 
  float		gain;		// #DEF_600 gain (gamma) of the sigmoidal rate-coded activation function 
  float		nvar;		// #DEF_0.005 variance of the Gaussian noise kernel for convolving with XX1 in NOISY_XX1
  float		avg_dt;		// #DEF_0.005 time constant for integrating activation average (computed across trials)
  float		avg_init;	// #DEF_0.15 initial activation average value
  IThrFun	i_thr;		// [STD or NO_AH for da mod units) how to compute the inhibitory threshold for kWTA functions (what currents to include or exclude in determining what amount of inhibition would keep the unit just at threshold firing) -- for units with dopamine-like modulation using the a and h currents, NO_AH makes learning much more reliable because otherwise kwta partially compensates for the da modulation

  void 	Defaults()	{ Initialize(); }
  TA_SIMPLE_BASEFUNS(ActFunSpec);
protected:
  void	UpdateAfterEdit_impl();
private:
  void	Initialize();
  void	Destroy()	{ };
};

class LEABRA_API SpikeFunSpec : public taBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra spiking activation function specs -- conductance is computed postsynaptically using an alpha function based on spike pulses sent presynaptically
INHERITED(taBase)
public:
  float		rise;		// #DEF_0 exponential rise time (in cycles) of the synaptic conductance according to the alpha function 1/(decay - rise) [e^(-t/decay) - e^(-t/rise)] -- set to 0 to only include decay time (1/decay e^(-t/decay)), which is highly optimized (doesn't use window -- just uses recursive exp decay) and thus the default!
  float		decay;		// #DEF_5 exponential decay time (in cycles) of the synaptic conductance according to the alpha function 1/(decay - rise) [e^(-t/decay) - e^(-t/rise)] -- set to 0 to implement a delta function (not very useful)
  float		g_gain;		// #DEF_5 multiplier for the spike-generated conductances when using alpha function which is normalized by area under the curve -- needed to recalibrate the alpha-function currents relative to rate code net input which is overall larger -- in general making this the same as the decay constant works well, effectively neutralizing the area normalization (results in consistent peak current, but differential integrated current over time as a function of rise and decay)
  int		window;		// #DEF_3 spike integration window -- when rise==0, this window is used to smooth out the spike impulses similar to a rise time -- each net contributes over the window in proportion to 1/window -- for rise > 0, this is used for computing the alpha function -- should be long enough to incorporate the bulk of the alpha function, but the longer the window, the greater the computational cost
  float		v_m_r;		// #DEF_0 post-spiking membrane potential to reset to, produces refractory effect 
  float		eq_gain;	// #DEF_10 gain for computing act_eq relative to actual average: act_eq = eq_gain * (spikes/cycles)
  float		eq_dt;		// #DEF_0.02 if non-zero, eq is computed as a running average with this time constant
  float		hard_gain;	// #DEF_0.2 gain for hard-clamped external inputs, mutliplies ext.  constant external inputs otherwise have too much influence compared to spiking ones: Note: soft clamping is strongly recommended

  float		gg_decay;	// #READ_ONLY #NO_SAVE g_gain/decay
  float		gg_decay_sq;	// #READ_ONLY #NO_SAVE g_gain/decay^2
  float		gg_decay_rise; // #READ_ONLY #NO_SAVE g_gain/(decay-rise)
  float		oneo_decay;	// #READ_ONLY #NO_SAVE 1.0/decay
  float		oneo_rise;	// #READ_ONLY #NO_SAVE 1.0/rise

  float	ComputeAlpha(float t) {
    if(decay == 0.0f) return (t == 0.0f) ? g_gain : 0.0f; // delta function
    if(rise == 0.0f) return gg_decay * taMath_float::exp_fast(-t * oneo_decay);	 // exponential
    if(rise == decay) return t * gg_decay_sq * taMath_float::exp_fast(-t * oneo_decay); // symmetric alpha
    return gg_decay_rise * (taMath_float::exp_fast(-t * oneo_decay) - taMath_float::exp_fast(-t * oneo_rise)); // full alpha
  }

  void 	Defaults()	{ Initialize(); }
  TA_SIMPLE_BASEFUNS(SpikeFunSpec);
protected:
  void	UpdateAfterEdit_impl();
private:
  void	Initialize();
  void	Destroy()	{ };
};

class LEABRA_API DepressSpec : public taBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra activation/spiking conveyed to other units is subject to synaptic depression: depletes a given amount per spike, and recovers with exponential recovery rate (also subject to trial/phase decay = recovery proportion)
INHERITED(taBase)
public:
  bool		on;		// synaptic depression is in effect: multiplies normal activation computed by current activation function in effect
  float		rec;		// #CONDEDIT_ON_on #DEF_0.2 rate of recovery of spike amplitude (determines overall time constant of depression function)
  float		asymp_act;	// #CONDEDIT_ON_on #DEF_0.5 asymptotic activation value (as proportion of 1) for a fully active unit (determines depl value)
  float		depl;		// #CONDEDIT_ON_on #READ_ONLY #SHOW rate of depletion of spike amplitude as a function of activation output (computed from rec, asymp_act)
  float		max_amp;	// #CONDEDIT_ON_on maximum spike amplitude -- this is the multiplier factor for activation values -- set to clamp_norm_max_amp to maintain asymptotic values at normal hard clamp levels, or set to 1 to retain usual normalized activation values (val is copied to act_range.max)
  float		clamp_norm_max_amp;	// #CONDEDIT_ON_on #READ_ONLY #SHOW maximum spike amplitude required to maintain asymptotic firing at normal clamp levels -- set max_amp to this value for delta-based learning rules to normalize against large diffs across phases

  void 	Defaults()	{ Initialize(); }
  TA_SIMPLE_BASEFUNS(DepressSpec);
protected:
  void	UpdateAfterEdit_impl();
private:
  void	Initialize();
  void	Destroy()	{ };
};

class LEABRA_API SynDelaySpec : public taBase {
  // ##INLINE ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra synaptic delay -- activation sent to other units is delayed by a given number of cycles
INHERITED(taBase)
public:
  bool		on;		// is synaptic delay active?
  int		delay;		// #CONDEDIT_ON_on number of cycles to delay for

  void 	Defaults()	{ Initialize(); }
  TA_SIMPLE_BASEFUNS(SynDelaySpec);
private:
  void	Initialize();
  void	Destroy()	{ };
};

class LEABRA_API OptThreshSpec : public taBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra optimization thresholds for faster processing
INHERITED(taBase)
public:
  float		send;		// #DEF_0.1 don't send activation when act <= send -- greatly speeds processing
  float		delta;		// #DEF_0.005 don't send activation changes until they exceed this threshold: only for when LeabraNetwork::send_delta is on!
  float		learn;		// #DEF_0.01 don't learn on recv unit weights when both phase acts (or bias.sravg_m for Ct) <= learn
  float		phase_dif;	// #DEF_0 don't learn when +/- phase difference ratio (- / +) < phase_dif (.8 when used, but off by default)

  void 	Defaults()	{ Initialize(); }
  TA_SIMPLE_BASEFUNS(OptThreshSpec);
private:
  void	Initialize();
  void	Destroy()	{ };
};

class LEABRA_API DtSpec : public taBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra time constants
INHERITED(taBase)
public:
  float		vm;		// #DEF_0.3 membrane potential time constant -- if units oscillate too much, then this is too high (but see d_vm_max for another solution)
  float		net;		// #DEF_0.7 net input time constant -- how fast to update net input (damps oscillations)
  bool		midpoint;	// use the midpoint method in computing the vm value -- better avoids oscillations and allows a larger dt.vm parameter to be used
  float		d_vm_max;	// #DEF_0.025 maximum change in vm at any timestep (limits blowup)
  int		vm_eq_cyc;	// #AKA_cyc0_vm_eq #DEF_0 number of cycles to compute the vm as equilibirium potential given current inputs: set to 1 to quickly activate input layers; set to 100 to always use this computation
  float		vm_eq_dt;	// #DEF_1 time constant for integrating the vm_eq values: how quickly to move toward the current eq value from previous vm value

  void 	Defaults()	{ Initialize(); }
  TA_SIMPLE_BASEFUNS(DtSpec);
private:
  void	Initialize();
  void	Destroy()	{ };
};

class LEABRA_API LeabraChannels : public taBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra channels used in Leabra
INHERITED(taBase)
public:
  float		e;		// Excitatory (glutamatergic synaptic sodium (Na) channel)
  float		l;		// Constant leak (potassium, K+) channel 
  float		i;		// inhibitory
  float		h;		// hysteresis (Ca)
  float		a;		// accomodation (k)

  void 	Copy_(const LeabraChannels& cp); // used in units, so optimized copy needed
  TA_BASEFUNS(LeabraChannels);
private:
  void	Initialize();
  void	Destroy()	{ };
};

class LEABRA_API VChanSpec : public taBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra voltage gated channel specs
INHERITED(taBase)
public:
  bool		on;		// #APPLY_IMMED #DEF_false true if channel is on
  float		b_inc_dt;	// #CONDEDIT_ON_on:true #AKA_b_dt time constant for increases in basis variable (basis ~ intracellular calcium which builds up slowly as function of activation)
  float		b_dec_dt;	// #CONDEDIT_ON_on:true time constant for decreases in basis variable (basis ~ intracellular calcium which builds up slowly as function of activation)
  float		a_thr;		// #CONDEDIT_ON_on:true activation threshold of the channel: when basis > a_thr, conductance starts to build up (channels open)
  float		d_thr;		// #CONDEDIT_ON_on:true deactivation threshold of the channel: when basis < d_thr, conductance diminshes (channels close)
  float		g_dt;		// #CONDEDIT_ON_on:true time constant for changing conductance (activating or deactivating) -- if = 1, then gc is equal to the basis if channel is on
  bool		init;		// #CONDEDIT_ON_on:true initialize variables when state is intialized between trials (else with weights)
  bool		trl;		// #CONDEDIT_ON_on:true update after every trial instead of every cycle -- time constants need to be much larger in general

  void	UpdateBasis(float& basis, bool& on_off, float& gc, float act) {
    float del = act - basis;
    if(del > 0.0f)
      basis += b_inc_dt * del;
    else
      basis += b_dec_dt * del;
    if(basis > a_thr)
      on_off = true;
    if(on_off && (basis < d_thr))
      on_off = false;
    if(g_dt == 1.0f)
      gc = (on_off) ? basis : 0.0f;
    else
      gc += g_dt * ((float)on_off - gc);
  }

  void 	Defaults()	{ Initialize(); }
  TA_SIMPLE_BASEFUNS(VChanSpec);
private:
  void	Initialize();
  void	Destroy()	{ };
};

class LEABRA_API MaxDaSpec : public taBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra how to compute the maxda value, which serves as a stopping criterion for settling
INHERITED(taBase)
public:
  enum dAValue {
    DA_ONLY,			// just use da
    INET_ONLY,			// just use inet
    INET_DA			// use inet if no activity, then use da
  };

  dAValue	val;		// #DEF_INET_DA value to use for computing delta-activation (change in activation over cycles of settling).
  float		inet_scale;	// #DEF_1 how to scale the inet measure to be like da
  float		lay_avg_thr;	// #DEF_0.01 threshold for layer average activation to switch to da fm Inet

  void 	Defaults()	{ Initialize(); }
  TA_SIMPLE_BASEFUNS(MaxDaSpec);
private:
  void	Initialize();
  void	Destroy()	{ };
};

class LEABRA_API DaModSpec : public taBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra specs for effects of da-based modulation: plus-phase = learning effects
INHERITED(taBase)
public:
  enum ModType {
    PLUS_CONT,			// da modulates plus-phase activations (only) in a continuous manner
    PLUS_POST,			// da modulates plus-phase activations (only), at the end of the plus phase
  };

  bool		on;		// #APPLY_IMMED whether to actually modulate activations by da values
  ModType	mod;		// #CONDEDIT_ON_on:true how to apply DA modulation
  float		gain;		// #CONDEDIT_ON_on:true gain multiplier of da values

  void 	Defaults()	{ Initialize(); }
  TA_SIMPLE_BASEFUNS(DaModSpec);
private:
  void	Initialize();
  void 	Destroy()	{ };
};

class LEABRA_API LeabraUnitSpec : public UnitSpec {
  // #STEM_BASE ##CAT_Leabra Leabra unit specifications, point-neuron approximation
INHERITED(UnitSpec)
public:
  enum ActFun {
    NOISY_XX1,			// x over x plus 1 convolved with Gaussian noise (noise is nvar)
    XX1,			// x over x plus 1, hard threshold, no noise convolution
    NOISY_LINEAR,		// simple linear output function (still thesholded) convolved with Gaussian noise (noise is nvar)
    LINEAR,			// simple linear output function (still thesholded)
    SPIKE			// discrete spiking activations (spike when > thr)
  };

  enum NoiseType {
    NO_NOISE,			// no noise added to processing
    VM_NOISE,			// noise in the value of v_m (membrane potential)
    NETIN_NOISE,		// noise in the net input (g_e)
    ACT_NOISE			// noise in the activations
  };

  ActFun	act_fun;	// #APPLY_IMMED #CAT_Activation activation function to use
  ActFunSpec	act;		// #CAT_Activation activation function specs
  SpikeFunSpec	spike;		// #CONDEDIT_ON_act_fun:SPIKE #CAT_Activation spiking function specs (only for act_fun = SPIKE)
  DepressSpec	depress;	// #CAT_Activation depressing synapses specs -- multiplies activation value by a spike amplitude/probability value that depresses with use and recovers exponentially
  SynDelaySpec	syn_delay;	// #CAT_Activation synaptic delay -- if active, activation sent to other units is delayed by a given amount
  OptThreshSpec	opt_thresh;	// #CAT_Learning optimization thresholds for speeding up processing when units are basically inactive
  MinMaxRange	clamp_range;	// #CAT_Activation range of clamped activation values (min, max, 0, .95 std), don't clamp to 1 because acts can't reach, so .95 instead
  MinMaxRange	vm_range;	// #CAT_Activation membrane potential range (min, max, 0-1 for normalized, -90-50 for bio-based)
  Random	v_m_init;	// #CAT_Activation what to initialize the membrane potential to (mean = .15, var = 0 std)
  DtSpec	dt;		// #CAT_Activation time constants (rate of updating): membrane potential (vm) and net input (net)
  LeabraChannels g_bar;		// #CAT_Activation [Defaults: 1, .1, 1, .1, .5] maximal conductances for channels
  LeabraChannels e_rev;		// #CAT_Activation [Defaults: 1, .15, .15, 1, 0] reversal potentials for each channel
  LeabraChannels e_rev_sub_thr;	// #CAT_Activation #READ_ONLY #NO_SAVE #HIDDEN e_rev - act.thr for each item -- used for compute_ithresh
  VChanSpec	hyst;		// #CAT_Activation [Defaults: .05, .8, .7, .1] hysteresis (excitatory) v-gated chan (Ca2+, NMDA)
  VChanSpec	acc;		// #CAT_Activation [Defaults: .01, .5, .1, .1] accomodation (inhibitory) v-gated chan (K+)
  DaModSpec	da_mod;		// #CAT_Learning da modulation of activations (for da-based learning, and other effects)
  MaxDaSpec	maxda;		// #CAT_Activation maximum change in activation (da) computation -- regulates settling
  NoiseType	noise_type;	// #APPLY_IMMED #CAT_Activation where to add random noise in the processing (if at all)
  Random	noise;		// #CONDEDIT_OFF_noise_type:NO_NOISE #CAT_Activation distribution parameters for random added noise
  Schedule	noise_sched;	// #CONDEDIT_OFF_noise_type:NO_NOISE #CAT_Activation schedule of noise variance over settling cycles

  FunLookup	nxx1_fun;	// #HIDDEN #NO_SAVE #NO_INHERIT #CAT_Activation convolved gaussian and x/x+1 function as lookup table
  FunLookup	noise_conv;	// #HIDDEN #NO_SAVE #NO_INHERIT #CAT_Activation gaussian for convolution

  override void Init_Weights(Unit* u);
  virtual void 	Init_ActAvg(LeabraUnit* u);
  // #CAT_Activation initialize average activation values, used to control learning

  virtual void	SetCurLrate(LeabraNetwork* net, int epoch);
  // #CAT_Learning set current learning rate based on epoch
  virtual void	SetLearnRule(LeabraNetwork* net);
  // #CAT_Learning set current learning rule from the network

  //////////////////////////////////////////
  //	Stage 0: at start of settling	  // 
  //////////////////////////////////////////

//  virtual void	Init_Netin(LeabraUnit* u);
  virtual void	Init_Acts(LeabraUnit* u, LeabraLayer* lay);
  void		Init_Acts(Unit* u)	{ UnitSpec::Init_Acts(u); }

  virtual void 	Compute_NetinScale(LeabraUnit* u, LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_Activation compute net input scaling values and input from hard-clamped inputs
  virtual void 	Compute_NetinRescale(LeabraUnit* u, LeabraLayer* lay, LeabraNetwork* net, float new_scale);
  // #CAT_Activation rescale netinput scales by given amount
  virtual void 	Init_ClampNet(LeabraUnit* u, LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_Activation init clam net value prior to sending
  virtual void 	Send_ClampNet(LeabraUnit* u, LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_Activation compute net input from hard-clamped inputs (sender based)

  //////////////////////////////////
  //	Stage 1: netinput 	  //
  //////////////////////////////////

  void 		Send_Netin(Unit* u) { UnitSpec::Send_Netin(u); }
  void 		Send_Netin(LeabraUnit* u, LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_Activation send netinput; add ext input, sender-based

  virtual void 	Send_NetinDelta(LeabraUnit* u, LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_Activation send netinput; sender based and only when act changes above a threshold

  ////////////////////////////////////////////////////////////////
  //	Stage 2: netinput averages and clamping (if necc)	//
  ////////////////////////////////////////////////////////////////

  virtual void Compute_Netin_Spike(LeabraUnit* u, LeabraLayer* lay, LeabraInhib* thr, LeabraNetwork* net);
  // #CAT_Activation compute actual netin conductance value for spiking units by integrating over spike
  inline virtual void	Compute_NetinAvg(LeabraUnit* u, LeabraLayer* lay, LeabraInhib* thr, LeabraNetwork* net);
  // #CAT_Activation compute netin average
  inline virtual void	Compute_ApplyInhib(LeabraUnit* u, LeabraLayer* lay, LeabraInhib* thr, LeabraNetwork* net, float inhib_val);
  // #CAT_Activation apply computed (kwta) inhibition value to unit inhibitory conductance
  inline virtual void	Compute_InhibAvg(LeabraUnit* u, LeabraLayer* lay, LeabraInhib* thr, LeabraNetwork* net);
  // #CAT_Activation compute inhib netin average
  virtual void	Compute_HardClamp(LeabraUnit* u, LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_Activation force units to external values provided by environment: also optimizes settling by only computing netinput once
  virtual void	Compute_HardClampNoClip(LeabraUnit* u, LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_Activation hard-clamp units without clipping values to clamp_range (use for second plus phase clamping to settled values)
  virtual bool	Compute_SoftClamp(LeabraUnit* u, LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_Activation soft-clamps unit, returns true if unit is not above .5

  ////////////////////////////////////////
  //	Stage 3: inhibition		//
  ////////////////////////////////////////

  virtual float	Compute_IThresh(LeabraUnit* u, LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_Activation compute inhibitory value that would place unit directly at threshold
  virtual float	Compute_IThreshStd(LeabraUnit* u, LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_Activation compute inhibitory value that would place unit directly at threshold, using all currents EXCEPT bias.wt
  virtual float	Compute_IThreshNoA(LeabraUnit* u, LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_Activation compute inhibitory value that would place unit directly at threshold, excluding gc.a current
  virtual float	Compute_IThreshNoH(LeabraUnit* u, LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_Activation compute inhibitory value that would place unit directly at threshold, excluding gc.a current
  virtual float	Compute_IThreshNoAH(LeabraUnit* u, LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_Activation compute inhibitory value that would place unit directly at threshold, excluding any gc.a, gc.h currents
  virtual float	Compute_IThreshAll(LeabraUnit* u, LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_Activation compute inhibitory value that would place unit directly at threshold, using all currents INCLUDING bias.wt

  ////////////////////////////////////////
  //	Stage 4: the final activation 	//
  ////////////////////////////////////////

  override void	Compute_Act(Unit* u)	{ UnitSpec::Compute_Act(u); }
  virtual void 	Compute_Act(LeabraUnit* u, LeabraLayer* lay, LeabraInhib* thr, LeabraNetwork* net);
  // #CAT_Activation compute the final activation: calls following function steps

  virtual void	Compute_MaxDa(LeabraUnit* u, LeabraLayer* lay, LeabraInhib* thr, LeabraNetwork* net);
  // #CAT_Activation compute the maximum delta-activation (change in activation); used to control settling

  virtual void Compute_DaMod_PlusCont(LeabraUnit* u, LeabraLayer* lay, LeabraInhib* thr, LeabraNetwork* net);
  // #CAT_Activation compute da modulation as plus-phase continuous gc.h/.a
  virtual void Compute_Conduct(LeabraUnit* u, LeabraLayer* lay, LeabraInhib* thr, LeabraNetwork* net);
  // #CAT_Activation compute input conductance values in the gc variables
  virtual void Compute_Vm(LeabraUnit* u, LeabraLayer* lay, LeabraInhib* thr, LeabraNetwork* net);
  // #CAT_Activation compute the membrante potential from input conductances
  virtual void Compute_ActFmVm(LeabraUnit* u, LeabraLayer* lay, LeabraInhib* thr, LeabraNetwork* net);
  // #CAT_Activation compute the activation from membrane potential
  virtual void Compute_ActFmVm_rate(LeabraUnit* u, LeabraLayer* lay, LeabraInhib* thr, LeabraNetwork* net);
  // #CAT_Activation compute the activation from membrane potential -- rate code functions
  virtual void Compute_ActFmVm_spike(LeabraUnit* u, LeabraLayer* lay, LeabraInhib* thr, LeabraNetwork* net);
  // #CAT_Activation compute the activation from membrane potential -- discrete spiking
  virtual void Compute_SelfReg_Cycle(LeabraUnit* u, LeabraLayer* lay, LeabraInhib* thr, LeabraNetwork* net);
  // #CAT_Activation compute self-regulatory currents (hysteresis, accommodation) -- at the cycle time scale
  virtual void Compute_SelfReg_Trial(LeabraUnit* u, LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_Activation compute self-regulatory currents (hysteresis, accommodation) -- at the trial time scale
  virtual void 	Compute_CycSynDep(LeabraUnit* u, LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_Activation compute cycle-level synaptic depression (must be defined by appropriate subclass) -- called at end of each cycle of computation if net_misc.cyc_syn_dep is on.


  ////////////////////////////////////////
  //	Stage 5: Between Events 	//
  ////////////////////////////////////////

  virtual void	PhaseInit(LeabraUnit* u, LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_Activation initialize external input flags based on phase
  virtual void	DecayPhase(LeabraUnit* u, LeabraLayer* lay, LeabraNetwork* net, float decay);
  // #CAT_Activation decay activation states towards initial values: at phase-level boundary
  virtual void	DecayEvent(LeabraUnit* u, LeabraLayer* lay, LeabraNetwork* net, float decay);
  // #CAT_Activation decay activation states towards initial values: at event-level boundary
  virtual void	ExtToComp(LeabraUnit* u, LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_Activation change external inputs to comparisons (remove input)
  virtual void	TargExtToComp(LeabraUnit* u, LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_Activation change target & external inputs to comparisons (remove targ & input)
  virtual void	Compute_ActTimeAvg(LeabraUnit* u, LeabraLayer* lay, LeabraInhib* thr,
				   LeabraNetwork* net);
  // #CAT_Activation compute time-averaged activation of unit (using act.avg_dt time constant), typically done at end of settling in PostSettle function
  virtual void	Compute_DaMod_PlusPost(LeabraUnit* u, LeabraLayer* lay, LeabraInhib* thr,
			   LeabraNetwork* net);
  // #CAT_Activation post-plus dav modulation
  virtual void	PostSettle(LeabraUnit* u, LeabraLayer* lay, LeabraInhib* thr,
			   LeabraNetwork* net);
  // #CAT_Activation set stuff after settling is over (act_m, act_p, etc)

  ////////////////////////////////////////
  //	Stage 6: Learning 		//
  ////////////////////////////////////////

  virtual void 	Compute_SRAvg(LeabraUnit* u, LeabraLayer* lay, LeabraNetwork* net, bool do_s);
  // #CAT_Learning compute sending-receiving activation product averages (CtLeabraX/CAL)
  virtual void	Init_SRAvg(LeabraUnit* u, LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_Learning reset the sender-receiver coproduct average -- call at start of trial

  override void	Compute_dWt(Unit*)	{ };
  virtual void	Compute_dWt(LeabraUnit* u, LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_Learning actually do wt change: learn!
  virtual void	Compute_dWt_impl(LeabraUnit* u, LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_Learning actually do wt change: learn!

  override void	Compute_Weights(Unit* u) { };
  virtual void	Compute_Weights(LeabraUnit* u, LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_Learning update the weights based on computed weight changes

  virtual void	EncodeState(LeabraUnit*, LeabraLayer*, LeabraNetwork*) { };
  // #CAT_Learning encode current state information after end of current trial (hook for time-based learning)

  override float Compute_SSE(bool& has_targ, Unit* u);
  virtual float  Compute_NormErr(LeabraUnit* u, LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_Statistic compute normalized binary error (0-1 as function of bits off from target) according to settings on the network (returns a 1 or 0)

  virtual void	CreateNXX1Fun();  // #CAT_Activation create convolved gaussian and x/x+1 

  virtual void	Defaults();	// #BUTTON #CONFIRM #CAT_ObjectMgmt restores default parameter settings: warning -- you will lose any unique parameters you might have set!

  virtual void	GraphVmFun(DataTable* graph_data, float g_i = .5, float min = 0.0, float max = 1.0, float incr = .01);
  // #BUTTON #NULL_OK graph membrane potential (v_m) as a function of excitatory net input (net) for given inhib conductance (g_i) (NULL = new graph data)
  virtual void	GraphActFmVmFun(DataTable* graph_data, float min = .15, float max = .50, float incr = .001);
  // #BUTTON #NULL_OK graph the activation function as a function of membrane potential (v_m) (NULL = new graph data)
  virtual void	GraphActFmNetFun(DataTable* graph_data, float g_i = .5, float min = 0.0, float max = 1.0, float incr = .001);
  // #BUTTON #NULL_OK graph the activation function as a function of net input (projected through membrane potential) (NULL = new graph data)
  virtual void	GraphSpikeAlphaFun(DataTable* graph_data, bool force_alpha=false);
  // #BUTTON #NULL_OK graph the spike alpha function for conductance integration over time window given in spike parameters -- last data point is the sum over the whole window (total conductance of a single spike) -- force_alpha means use explicit alpha function even when rise=0 (otherewise it simulates actual recursive exp decay used in optimized code)
  virtual void TimeExp(int mode, int nreps=100000000);
  // #BUTTON time how long it takes to compute various forms of exp() function: mode=0 = double sum ctrl (baseline), mode=1 = std double exp(), mode=2 = taMath_double::exp_fast, mode=3 = float sum ctrl (float baseline), mode=4 = expf, mode=5 = taMath_float::exp_fast -- this is the dominant cost in spike alpha function computation, so we're interested in optimizing it..

  override bool  CheckConfig_Unit(Unit* un, bool quiet=false);

  void	InitLinks();
  SIMPLE_COPY(LeabraUnitSpec);
  TA_BASEFUNS(LeabraUnitSpec);
protected:
  void	UpdateAfterEdit_impl();	// to set _impl sig
  void 	CheckThisConfig_impl(bool quiet, bool& rval);

private:
  void 	Initialize();
  void	Destroy()		{ };
};
TA_SMART_PTRS(LeabraUnitSpec)

class LEABRA_API VChanBasis : public taBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER basis variables for vchannels
INHERITED(taBase)
public:
  float		hyst;		// hysteresis
  float		acc;		// fast accomodation
  bool		hyst_on;	// #NO_VIEW binary thresholded mode state variable, hyst
  bool		acc_on;		// #NO_VIEW binary thresholded mode state variable, acc
  float		g_h;		// #NO_VIEW hysteresis conductance
  float		g_a;		// #NO_VIEW accomodation conductance

  void 	Copy_(const VChanBasis& cp);
  TA_BASEFUNS(VChanBasis);
private:
  void	Initialize();
  void	Destroy()	{ };
};

class LEABRA_API LeabraUnitChans : public taBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra channels used in Leabra units
INHERITED(taBase)
public:
  float		l;		// leak
  float		i;		// #DMEM_SHARE_SET_1 inhibitory
  float		h;		// hysteresis (Ca)
  float		a;		// accomodation (K)

  void 	Copy_(const LeabraUnitChans& cp);
  TA_BASEFUNS(LeabraUnitChans);
private:
  void	Initialize();
  void	Destroy()	{ };
};

class LEABRA_API LeabraUnit : public Unit {
  // #STEM_BASE ##DMEM_SHARE_SETS_5 ##CAT_Leabra Leabra unit, point-neuron approximation
INHERITED(Unit)
public:
  float		act_eq;		// #CAT_Activation rate-code equivalent activity value (time-averaged spikes or just act)
  float		act_nd;		// #CAT_Activation non-depressed rate-code equivalent activity value (time-averaged spikes or just act) -- used for final phase-based variables used in learning and stats
  float		act_avg;	// #CAT_Activation average activation (of final plus phase activation state) over long time intervals (dt = act.avg_dt)
  float		ravg_l;		// #CAT_Activation long time-scale average of medium-time scale (trial level) activation (as computed in the bias connection and spec) for the purposes of learning based on receiver average activations in XCAL algorithm
  float		act_m;		// #CAT_Activation minus_phase activation (act_nd), set after settling, used for learning and performance stats 
  float		act_p;		// #CAT_Activation plus_phase activation (act_nd), set after settling, used for learning and performance stats
  float		act_dif;	// #CAT_Activation difference between plus and minus phase acts, gives unit err contribution
  float		act_m2;		// #CAT_Activation second minus_phase (e.g., nothing phase) activation (act_nd), set after settling, used for learning and performance stats
  float		act_p2;		// #CAT_Activation second plus_phase activation (act_nd), set after settling, used for learning and performance stats
  float		act_dif2;	// #CAT_Activation difference between second set of phases, where relevant (e.g., act_p - act_m2 for MINUS_PLUS_NOTHING, or act_p2 - act_p for MINUS_PLUS_PLUS)
  float		da;		// #NO_SAVE #CAT_Activation delta activation: change in act from one cycle to next, used to stop settling
  VChanBasis	vcb;		// #CAT_Activation voltage-gated channel basis variables
  LeabraUnitChans gc;		// #DMEM_SHARE_SET_1 #NO_SAVE #CAT_Activation current unit channel conductances
  float		I_net;		// #NO_SAVE #CAT_Activation net current produced by all channels
  float		v_m;		// #NO_SAVE #CAT_Activation membrane potential
  float 	dav;		// #CAT_Activation dopamine value (da is delta activation) which modulates activations (e.g., via accom and hyst currents) to then drive learning
  float 	maint_h;	// #CAT_Activation maintained hysteresis current value (e.g., for PFC units)

  bool		in_subgp;	// #READ_ONLY #NO_SAVE #CAT_Structure determine if unit is in a subgroup
  float		clmp_net;	// #NO_VIEW #NO_SAVE #EXPERT #DMEM_SHARE_SET_4 #CAT_Activation hard-clamp net input (no need to recompute)
  float		net_scale;	// #NO_VIEW #NO_SAVE #EXPERT #CAT_Activation total netinput scaling basis
  float		bias_scale;	// #NO_VIEW #NO_SAVE #EXPERT #CAT_Activation bias weight scaling factor
  float		prv_net;	// #NO_VIEW #NO_SAVE #EXPERT #CAT_Activation previous net input (for time averaging)
  float		prv_g_i;	// #NO_VIEW #NO_SAVE #EXPERT #CAT_Activation previous inhibitory conductance value (for time averaging)

  float		act_sent;	// #NO_VIEW #NO_SAVE #EXPERT #CAT_Activation last activation value sent (only send when diff is over threshold)
  float		net_raw;	// #NO_VIEW #NO_SAVE #EXPERT #CAT_Activation raw net input received from sending units (increments the deltas in send_delta)
  float		net_delta;	// #NO_VIEW #NO_SAVE #EXPERT #DMEM_SHARE_SET_3 #CAT_Activation change in netinput received from other units  (send_delta mode only)
  float		g_i_raw;	// #NO_VIEW #NO_SAVE #EXPERT #CAT_Activation raw inhib net input received from sending units (increments the deltas in send_delta)
  float		g_i_delta;	// #NO_VIEW #NO_SAVE #EXPERT #DMEM_SHARE_SET_3 #CAT_Activation change in inhibitory netinput received from other units (send_delta mode only)

  float		i_thr;		// #NO_SAVE #CAT_Activation inhibitory threshold value for computing kWTA
  float		spk_amp;	// #CAT_Activation amplitude/probability of spiking output (for synaptic depression function if unit spec depress.on is on)
  float		misc_1;		// #NO_VIEW #NO_SAVE #CAT_Activation miscellaneous variable for other algorithms that need it
  float		misc_2;		// #NO_VIEW #NO_SAVE #CAT_Activation miscellaneous variable for other algorithms that need it
  float		misc_3;		// #NO_VIEW #NO_SAVE #CAT_Activation miscellaneous variable for other algorithms that need it
  float_CircBuffer act_buf;	// #NO_VIEW #NO_SAVE #CAT_Activation buffer of activation states for synaptic delay computation
  float_CircBuffer spike_buf;	// #NO_VIEW #NO_SAVE #CAT_Activation buffer of net input from spikes for synaptic integration over discrete spikes

  inline void	AddToActBuf(SynDelaySpec& sds) {
    if(sds.on) act_buf.CircAddLimit(act, sds.delay);
  }
  // add current activation to act buf if synaptic delay is on

  inline LeabraLayer*	own_lay() const {return (LeabraLayer*)own_lay_();}

  void		Init_ActAvg()
  { ((LeabraUnitSpec*)GetUnitSpec())->Init_ActAvg(this); }
  // #CAT_Activation initialize average activation

/*was  void		Init_Netin()
  { ((LeabraUnitSpec*)GetUnitSpec())->Init_Netin(this); }*/
  void		Init_Netin();
  void		Init_NetinDelta();
  void		Init_Acts(LeabraLayer* lay)
  { ((LeabraUnitSpec*)GetUnitSpec())->Init_Acts(this, lay); }

  void		Compute_NetinScale(LeabraLayer* lay, LeabraNetwork* net)
  { ((LeabraUnitSpec*)GetUnitSpec())->Compute_NetinScale(this, lay, net); }
  // #CAT_Activation compute net input scaling values and input from hard-clamped inputs
  void		Compute_NetinRescale(LeabraLayer* lay, LeabraNetwork* net, float new_scale)
  { ((LeabraUnitSpec*)GetUnitSpec())->Compute_NetinRescale(this, lay, net, new_scale); }
  // #CAT_Activation rescale netinput scales by given amount
  void		Init_ClampNet(LeabraLayer* lay, LeabraNetwork* net)
  { ((LeabraUnitSpec*)GetUnitSpec())->Init_ClampNet(this, lay, net); }
  // #CAT_Activation init clam net value prior to sending
  void		Send_ClampNet(LeabraLayer* lay, LeabraNetwork* net)
  { ((LeabraUnitSpec*)GetUnitSpec())->Send_ClampNet(this, lay, net); }
  // #CAT_Activation compute net input from hard-clamped inputs (sender based)

  void		Send_Netin(LeabraLayer* lay, LeabraNetwork* net)
  { ((LeabraUnitSpec*)GetUnitSpec())->Send_Netin(this, lay, net); }
  // #CAT_Activation send netinput; add ext input, sender-based
  void		Send_NetinDelta(LeabraLayer* lay, LeabraNetwork* net)
  { ((LeabraUnitSpec*)GetUnitSpec())->Send_NetinDelta(this, lay, net); }
  // #CAT_Activation send netinput; sender based and only when act changes above a threshold

  void		Compute_NetinAvg(LeabraLayer* lay, LeabraInhib* athr, LeabraNetwork* net)
  { ((LeabraUnitSpec*)GetUnitSpec())->Compute_NetinAvg(this, lay, athr, net); }
  // #CAT_Activation compute netin average
  void		Compute_ApplyInhib(LeabraLayer* lay, LeabraInhib* athr, LeabraNetwork* net, float inhib_val)
  { ((LeabraUnitSpec*)GetUnitSpec())->Compute_ApplyInhib(this, lay, athr, net, inhib_val); }
  // #CAT_Activation apply computed inhibitory value (kwta) to unit inhibitory conductance
  void		Compute_InhibAvg(LeabraLayer* lay, LeabraInhib* athr, LeabraNetwork* net)
  { ((LeabraUnitSpec*)GetUnitSpec())->Compute_InhibAvg(this, lay, athr, net); }
  // #CAT_Activation compute inhib netin average
  void		Compute_HardClamp(LeabraLayer* lay, LeabraNetwork* net) 
  { ((LeabraUnitSpec*)GetUnitSpec())->Compute_HardClamp(this, lay, net); }
  // #CAT_Activation force units to external values provided by environment: also optimizes settling by only computing netinput once
  void		Compute_HardClampNoClip(LeabraLayer* lay, LeabraNetwork* net) 
  { ((LeabraUnitSpec*)GetUnitSpec())->Compute_HardClampNoClip(this, lay, net); }
  // #CAT_Activation hard-clamp units without clipping values to clamp_range (use for second plus phase clamping to settled values)
  bool		Compute_SoftClamp(LeabraLayer* lay, LeabraNetwork* net) 
  { return ((LeabraUnitSpec*)GetUnitSpec())->Compute_SoftClamp(this, lay, net); }
  // #CAT_Activation soft-clamps unit, returns true if unit is not above .5

  float		Compute_IThresh(LeabraLayer* lay, LeabraNetwork* net)
  { return ((LeabraUnitSpec*)GetUnitSpec())->Compute_IThresh(this, lay, net); }
  // #CAT_Activation compute inhibitory value that would place unit directly at threshold
  float		Compute_IThreshNoAH(LeabraLayer* lay, LeabraNetwork* net)
  { return ((LeabraUnitSpec*)GetUnitSpec())->Compute_IThreshNoAH(this, lay, net); }
  // #CAT_Activation compute inhibitory value that would place unit directly at threshold, excluding any gc.a, gc.h currents

  void		Compute_Act()	{ Unit::Compute_Act(); }
  void 		Compute_Act(LeabraLayer* lay, LeabraInhib* athr, LeabraNetwork* net) 
  { ((LeabraUnitSpec*)GetUnitSpec())->Compute_Act(this, lay, athr, net); }
  // #CAT_Activation compute the final activation: calls following function steps

  void 		Compute_MaxDa(LeabraLayer* lay, LeabraInhib* athr, LeabraNetwork* net) 
  { ((LeabraUnitSpec*)GetUnitSpec())->Compute_MaxDa(this, lay, athr, net); }
  // #CAT_Activation compute the maximum delta-activation (change in activation); used to control settling

  void		Compute_CycSynDep(LeabraLayer* lay, LeabraNetwork* net)
  { ((LeabraUnitSpec*)GetUnitSpec())->Compute_CycSynDep(this, lay, net); }
  // #CAT_Activation compute cycle-level synaptic depression (must be defined by appropriate subclass) -- called at end of each cycle of computation if net_misc.cyc_syn_dep is on.

  void		PhaseInit(LeabraLayer* lay, LeabraNetwork* net)
  { ((LeabraUnitSpec*)GetUnitSpec())->PhaseInit(this, lay, net); }
  // #CAT_Activation initialize external input flags based on phase
  void		DecayPhase(LeabraLayer* lay, LeabraNetwork* net, float decay)
  { ((LeabraUnitSpec*)GetUnitSpec())->DecayPhase(this, lay, net, decay); }
  // #CAT_Activation decay activation states towards initial values: at phase-level boundary
  void		DecayEvent(LeabraLayer* lay, LeabraNetwork* net, float decay)
  { ((LeabraUnitSpec*)GetUnitSpec())->DecayEvent(this, lay, net, decay); }
  // #CAT_Activation decay activation states towards initial values: at event-level boundary
  void		ExtToComp(LeabraLayer* lay, LeabraNetwork* net)
  { ((LeabraUnitSpec*)GetUnitSpec())->ExtToComp(this, lay, net); }
  // #CAT_Activation change external inputs to comparisons (remove input)
  void		TargExtToComp(LeabraLayer* lay, LeabraNetwork* net)
  { ((LeabraUnitSpec*)GetUnitSpec())->TargExtToComp(this, lay, net); }
  // #CAT_Activation change target & external inputs to comparisons (remove targ & input)
  void		PostSettle(LeabraLayer* lay, LeabraInhib* athr, LeabraNetwork* net)
  { ((LeabraUnitSpec*)GetUnitSpec())->PostSettle(this, lay, athr, net); }
  // #CAT_Activation set stuff after settling is over (act_m, act_p etc)

  void		Compute_SRAvg(LeabraLayer* lay, LeabraNetwork* net, bool do_s)
  { ((LeabraUnitSpec*)GetUnitSpec())->Compute_SRAvg(this, lay, net, do_s); }
  // #CAT_Learning compute sending-receiving activation product averages (CtLeabra_X/CAL)
  void 		Init_SRAvg(LeabraLayer* lay, LeabraNetwork* net)
  { ((LeabraUnitSpec*)GetUnitSpec())->Init_SRAvg(this, lay, net); }	  
  // #CAT_Learning reset the sender-receiver coproduct average -- do at start of trial
  void 		Compute_dWt(LeabraLayer* lay, LeabraNetwork* net)
  { ((LeabraUnitSpec*)GetUnitSpec())->Compute_dWt(this, lay, net); }	  
  // #CAT_Learning actually do wt change: learn!
  void 		Compute_Weights(LeabraLayer* lay, LeabraNetwork* net)
  { ((LeabraUnitSpec*)GetUnitSpec())->Compute_Weights(this, lay, net); }	  
  // #CAT_Learning update weights based on computed weight changes (dwt's)

  void 		EncodeState(LeabraLayer* lay, LeabraNetwork* net)
  { ((LeabraUnitSpec*)GetUnitSpec())->EncodeState(this, lay, net); }
  // #CAT_Learning encode current state information at end of trial (hook for time-based learning)
  float		Compute_NormErr(LeabraLayer* lay, LeabraNetwork* net)
  { return ((LeabraUnitSpec*)GetUnitSpec())->Compute_NormErr(this, lay, net); }
  // #CAT_Learning compute normalized binary trial-wise error

  void 		Compute_SelfReg_Trial(LeabraLayer* lay, LeabraNetwork* net)
  { ((LeabraUnitSpec*)GetUnitSpec())->Compute_SelfReg_Trial(this, lay, net); }
  // #CAT_Activation compute self-regulation (accommodation, hysteresis) at end of trial

  void		GetInSubGp();

  void	InitLinks();
  void	Copy_(const LeabraUnit& cp);
  TA_BASEFUNS(LeabraUnit);
private:
  void 	Initialize();
  void	Destroy()		{ };
};


//////////////////////////////////////////////////////////////////////////
//			Projection Level Code

class LEABRA_API LeabraPrjn: public Projection {
  // #STEM_BASE ##CAT_Leabra leabra specific projection -- has special variables at the projection-level
INHERITED(Projection)
public:
  float		netin_avg;	// #READ_ONLY #EXPERT #CAT_Statistic average netinput values for the recv projections into this layer
  float		netin_rel;	// #READ_ONLY #EXPERT #CAT_Statistic relative netinput values for the recv projections into this layer

  float		avg_netin_avg;	// #READ_ONLY #EXPERT #CAT_Statistic average netinput values for the recv projections into this layer, averaged over an epoch
  float		avg_netin_avg_sum;// #READ_ONLY #HIDDEN #DMEM_AGG_SUM #CAT_Statistic average netinput values for the recv projections into this layer, sum over an epoch
  float		avg_netin_rel;	// #READ_ONLY #EXPERT #CAT_Statistic relative netinput values for the recv projections into this layer, averaged over an epoch
  float		avg_netin_rel_sum; // #READ_ONLY #HIDDEN #DMEM_AGG_SUM #CAT_Statistic relative netinput values for the recv projections into this layer, sum over an epoch (for computing average)
  int		avg_netin_n; // #READ_ONLY #HIDDEN #DMEM_AGG_SUM #CAT_Statistic count for computing epoch-level averages

  float		trg_netin_rel;	// #CAT_Learning target value for avg_netin_rel -- used for adapting scaling and actual layer activations to achieve desired relative netinput levels -- important for large multilayered networks, where bottom-up projections should be stronger than top-down ones.  this value can be set automatically based on the projection direction and other projections, as determined by the con spec

  virtual void	SetCurLrate(LeabraNetwork* net, int epoch);
  // #CAT_Learning set current learning rate based on epoch
  virtual void	SetLearnRule(LeabraNetwork* net);
  // #CAT_Learning set current learning rule from the network

  virtual void	Init_Stats();	// #CAT_Statistic intialize statistic counters

#ifdef DMEM_COMPILE
  DMemAggVars	dmem_agg_sum;		// #IGNORE aggregation of network variables using SUM op (currently only OP in use -- add others as needed)
  virtual void 	DMem_InitAggs();
  // #IGNORE initialize aggregation stuff
  virtual void	DMem_ComputeAggs(MPI_Comm comm);
  // #IGNORE aggregate network variables across procs for trial-level dmem 
#endif

  void	Copy_(const LeabraPrjn& cp);
  TA_BASEFUNS(LeabraPrjn);
private:
  void 	Initialize();
  void 	Destroy();
};

//////////////////////////////////////////////////////////////////////////
//			Layer Level Code

class LEABRA_API LeabraSort : public taPtrList<LeabraUnit> {
  // ##NO_TOKENS ##NO_UPDATE_AFTER ##CAT_Leabra used for sorting units in kwta computation
INHERITED(taPtrList<LeabraUnit>)
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

class LEABRA_API LeabraInhibSpec : public taOBase {
  // ##INLINE ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra specifies how inhibition is computed in Leabra system (kwta, unit inhib, etc)
INHERITED(taOBase)
public:
  enum InhibType {		// how to compute the inhibition
    KWTA_INHIB,			// between thresholds of k and k+1th most activated units (sets precise k value, should use i_kwta_pt = .25 std)
    KWTA_AVG_INHIB,		// average of top k vs avg of rest (provides more flexibility in actual k value, should use i_kwta_pt = .6 std)
    KWTA_KV2K,			// average of top k vs avg of next k (2k) -- avoids long "tail" of distribution of weakly active units, while providing similar flexibility as KWTA_AVG_INHIB, and also is equivalent to KWTA_INHIB for k=1 -- i_kwta_pt = .25 is std.  In general, this is now preferred to KWTA_AVG_INHIB
    KWTA_COMP_COST,		// competitor cost kwta function: inhibition is i_kwta_pt below the k'th unit's threshold inhibition value if there are no strong competitors (>comp_thr proportion of kth inhib val), and each competitor increases inhibition linearly (normalized by total possible = n-k) with gain comp_gain -- produces cleaner competitive dynamics and considerable kwta flexibility
    AVG_MAX_PT_INHIB,		// put inhib value at i_kwta_pt between avg and max values for layer
    MAX_INHIB,			// put inhib value at i_kwta_pt below max guy in layer
    UNIT_INHIB			// unit-based inhibition (g_i from netinput -- requires connections with inhib flag set to provide inhibition)
  };

  InhibType	type;		// #APPLY_IMMED how to compute inhibition (g_i)
  float		kwta_pt;	// [Default: .25 for KWTA_INHIB and KWTA_KV2K, .6 for KWTA_AVG, .2 for AVG_MAX_PT_INHIB] point to place inhibition between k and k+1 (or avg and max for AVG_MAX_PT_INHIB)
  float		min_i;		// minimum inhibition value -- set this higher than zero to prevent units from getting active even if there is not much overall excitation
  float		comp_thr;	// #CONDEDIT_ON_type:KWTA_COMP_COST [0-1] Threshold for competitors in KWTA_COMP_COST -- competitor threshold inhibition is normalized by k'th inhibition and those above this threshold are counted as competitors 
  float		comp_gain;	// #CONDEDIT_ON_type:KWTA_COMP_COST Gain for competitors in KWTA_COMP_COST -- how much to multiply contribution of competitors to increase inhibition level
  float		gp_pt;		// #CONDEDIT_ON_type:AVG_MAX_PT_INHIB [Default: .2] for unit groups: point to place inhibition between avg and max for AVG_MAX_PT_INHIB

  void 	Defaults()	{ Initialize(); }
  TA_SIMPLE_BASEFUNS(LeabraInhibSpec);
private:
  void	Initialize();
  void 	Destroy()	{ };
};

class LEABRA_API KWTASpec : public taOBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra specifies k-winner-take-all parameters
INHERITED(taOBase)
public:
  enum K_From {
    USE_K,			// use the k specified directly
    USE_PCT,			// use the percentage pct to compute the k as a function of layer size
    USE_PAT_K			// use the activity level of the current event pattern (k = # of units > pat_q)
  };

  K_From	k_from;		// #APPLY_IMMED how is the active_k determined: directly by k, by pct, or by no. of units where ext > pat_q
  int		k;		// #CONDEDIT_ON_k_from:USE_K desired number of active units in the layer
  float		pct;		// #CONDEDIT_ON_k_from:USE_PCT desired proportion of activity (used to compute a k value based on layer size, .25 std)
  float		pat_q;		// #HIDDEN #DEF_0.5 threshold for pat_k based activity level: add to k if ext > pat_q
  bool		diff_act_pct;	// #DEF_false #APPLY_IMMED if true, use different actual percent activity for overall layer activation
  float		act_pct;	// #CONDEDIT_ON_diff_act_pct:true actual percent activity to put in kwta.pct field of layer
  bool		gp_i;		// #APPLY_IMMED compute inhibition including all of the layers in the same group, or unit groups within the layer: each items computed inhib vals are multipled by gp_g scaling, then MAX'd, and each item's inhib is the MAX of this pooled MAX value and its original own value
  float		gp_g;		// #CONDEDIT_ON_gp_i:true how much this item (layer or unit group) contributes to the pooled layer group values

  void 	Defaults()	{ Initialize(); }
  TA_SIMPLE_BASEFUNS(KWTASpec);
private:
  void	Initialize();
  void 	Destroy()	{ };
};

class LEABRA_API KwtaTieBreak : public taOBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra break ties where all the units have similar netinputs and thus none get activated.  this lowers the inhibition so that all get active to some extent
INHERITED(taOBase)
public:
  bool		on;		// #APPLY_IMMED whether to perform the tie breaking function at all
  float		k_thr; 		// #CONDEDIT_ON_on:true #DEF_1 threshold on inhibitory threshold (i_thr) for top kwta units before tie break is engaged: don't break ties for weakly activated layers
  float		diff_thr;	// #CONDEDIT_ON_on:true #DEF_0.2 threshold on difference ratio between top k and rest (k_ithr - k1_ithr) / k_ithr for a tie to be indicated.  This is also how much k1_ithr is reduced relative to k_ithr to fix the tie: sets a lower limit on this value.  larger values mean higher overall activations during ties, but you dont' want to activate the tie mechanism unnecessarily either.

  void 	Defaults()	{ Initialize(); }
  TA_SIMPLE_BASEFUNS(KwtaTieBreak);
private:
  void	Initialize();
  void 	Destroy()	{ };
};

class LEABRA_API AdaptISpec : public taOBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra specifies adaptive kwta specs (esp for avg-based)
INHERITED(taOBase)
public:
  enum AdaptType {
    NONE,			// don't adapt anything
    KWTA_PT,			// adapt kwta point (i_kwta_pt) based on running-average layer activation as compared to target value
    G_BAR_I,			// adapt g_bar.i for unit inhibition values based on layer activation at any point in time
    G_BAR_IL			// adapt g_bar.i and g_bar.l for unit inhibition & leak values based on layer activation at any point in time
  };

  AdaptType	type;		// #APPLY_IMMED what to adapt, or none for nothing
  float		tol;		// #CONDEDIT_OFF_type:NONE #DEF_0.02 tolerance around target avg act before changing parameter
  float		p_dt;		// #CONDEDIT_OFF_type:NONE #DEF_0.1 #AKA_pt_dt time constant for changing the parameter (i_kwta_pt or g_bar.i)
  float		mx_d;		// #CONDEDIT_OFF_type:NONE #DEF_0.9 maximum deviation (proportion) from initial parameter setting allowed
  float		l;		// #CONDEDIT_ON_type:G_BAR_IL proportion of difference from target activation to allocate to the leak in G_BAR_IL mode
  float		a_dt;		// #CONDEDIT_ON_type:KWTA_PT #DEF_0.005 time constant for integrating average average activation, which is basis for adapting i_kwta_pt

  void 	Defaults()	{ Initialize(); }
  TA_SIMPLE_BASEFUNS(AdaptISpec);
private:
  void	Initialize();
  void 	Destroy()	{ };
};

class LEABRA_API ClampSpec : public taOBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra specs for clamping 
INHERITED(taOBase)
public:
  bool		hard;		// #APPLY_IMMED #DEF_true whether to hard clamp inputs to this layer or not
  float		gain;		// #CONDEDIT_OFF_hard:true #DEF_0.5 starting soft clamp gain factor (net = gain * ext)
  float		d_gain;		// #CONDEDIT_OFF_hard:true [Default: 0] for soft clamp, delta to increase gain when target units not > .5 (0 = off, .1 std when used)

  void 	Defaults()	{ Initialize(); }
  TA_SIMPLE_BASEFUNS(ClampSpec);
private:
  void	Initialize();
  void 	Destroy()	{ };
};

class LEABRA_API DecaySpec : public taOBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra holds decay values
INHERITED(taOBase)
public:
  float		event;		// #DEF_1 proportion decay of state vars between events
  float		phase;		// [1 for Leabra_CHL, 0 for CtLeabra_X/CAL] proportion decay of state vars between minus and plus phases 
  float		phase2;		// #DEF_0 proportion decay of state vars between 2nd set of phases (if appl, 0 std)
  bool		clamp_phase2;	// #DEF_false if true, hard-clamp second plus phase activations to prev plus phase (only special layers will then update -- optimizes speed)

  void 	Defaults()	{ Initialize(); }
  TA_SIMPLE_BASEFUNS(DecaySpec);
private:
  void	Initialize();
  void 	Destroy()	{ };
};

class LEABRA_API CtLayerInhibMod : public taOBase {
  // ##INLINE ##NO_TOKENS ##CAT_Leabra layer-level sinusoidal and final inhibitory modulation parameters simulating initial burst of activation and subsequent oscillatory ringing
INHERITED(taOBase)
public:
  bool		use_sin;	// if on, actually use layer-level sinusoidal values (burst_i, trough_i) -- else use network level
  float		burst_i;	// #CONDEDIT_ON_use_sin [.02] maximum reduction in inhibition as a proportion of computed kwta value to subtract for positive activation (burst) phase of wave -- value should be a positive number
  float		trough_i;	// #CONDEDIT_ON_use_sin [.02] maximum extra inhibition as proportion of computed kwta value to add for negative activation (trough) phase of wave -- value shoudl be a positive number
  bool		use_fin;	// if on, actually use layer-level final values (inhib_i) -- else use network level
  float		inhib_i;	// #CONDEDIT_ON_use_fin [.05 when in use] maximum extra inhibition as proportion of computed kwta value to add during final inhib phase

  SIMPLE_COPY(CtLayerInhibMod);
  TA_BASEFUNS(CtLayerInhibMod);
// protected:
//   void UpdateAfterEdit_impl();

private:
  void	Initialize();
  void 	Destroy()	{ };
};

class LEABRA_API LayNetRescaleSpec : public taOBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra global rescale of layer netinputs to prevent blowup
INHERITED(taOBase)
public:
  bool		on;		// #APPLY_IMMED whether to apply layer netinput rescaling
  float		max_net; 	// #CONDEDIT_ON_on:true #DEF_0.6 target maximum netinput value
  float		net_extra;	// #CONDEDIT_ON_on:true #DEF_0.2 extra netin value to add to actual to anticipate further increases, preventing too many updates

  void 	Defaults()	{ Initialize(); }
  TA_SIMPLE_BASEFUNS(LayNetRescaleSpec);
private:
  void	Initialize();
  void 	Destroy()	{ };
};

class LEABRA_API LayAbsNetAdaptSpec : public taOBase {
  // ##INLINE ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra adapt absolute netinput values by adjusting the wt_scale.abs parameters in the conspecs of projections into this layer, based on differences between time-averaged max netinput values and the target
INHERITED(taOBase)
public:
  bool		on;		// #APPLY_IMMED whether to apply layer netinput rescaling
  float		trg_net; 	// #CONDEDIT_ON_on:true #DEF_0.5 target maximum netinput value
  float		tol;		// #CONDEDIT_ON_on:true #DEF_0.1 tolerance around target value -- if actual value is within this tolerance from target, then do not adapt
  float		abs_lrate;	// #CONDEDIT_ON_on:true #DEF_0.2 learning rate for adapting the wt_scale.abs parameters for all projections into layer

  void 	Defaults()	{ Initialize(); }
  TA_SIMPLE_BASEFUNS(LayAbsNetAdaptSpec);
private:
  void	Initialize();
  void 	Destroy()	{ };
};

class LEABRA_API LeabraLayerSpec : public LayerSpec {
  // #STEM_BASE ##CAT_Leabra Leabra layer specs, computes inhibitory input for all units in layer
INHERITED(LayerSpec)
public:
  enum InhibGroup {
    ENTIRE_LAYER,		// treat entire layer as one inhibitory group (even if subgroups exist)
    UNIT_GROUPS,		// treat sub unit groups as separate inhibitory groups (but also uses gp_i and gp_g if set, to have some sharing of inhib across groups
    LAY_AND_GPS			// compute inhib over both groups and whole layer, inhibi is max of layer and group inhib
  };

  InhibGroup	inhib_group;	// #APPLY_IMMED #CAT_Activation what to consider the inhibitory group (layer or unit subgroups, or both)
  LeabraInhibSpec inhib;	// #CAT_Activation how to compute inhibition -- for kwta modes, a single global inhibition value is computed for the entire layer
  KWTASpec	kwta;		// #CONDEDIT_OFF_inhib_group:UNIT_GROUPS #CAT_Activation desired activity level over entire layer (NOTE: used to set target activity for UNIT_INHIB, AVG_MAX_PT_INHIB, but not used for actually computing inhib for these cases)
  KWTASpec	gp_kwta;	// #CONDEDIT_OFF_inhib_group:ENTIRE_LAYER #CAT_Activation desired activity level for units within unit groups (not for ENTIRE_LAYER) (NOTE: used to set target activity for UNIT_INHIB, AVG_MAX_PT_INHIB, but not used for actually computing inhib for these cases)
  KwtaTieBreak	tie_brk;	// #CAT_Activation break ties when all the units in the layer have similar netinputs, which puts the inhbition value too close to everyone's threshold and produces no activation at all.  this will lower the inhibition and allow all the units to have some activation
  AdaptISpec	adapt_i;	// #CAT_Activation adapt the inhibition: either i_kwta_pt point based on diffs between actual and target k level (for avg-based), or g_bar.i for unit-inhib
  ClampSpec	clamp;		// #CAT_Activation how to clamp external inputs to units (hard vs. soft)
  DecaySpec	decay;		// #CAT_Activation decay of activity state vars between events, -/+ phase, and 2nd set of phases (if appl)
  CtLayerInhibMod  ct_inhib_mod; // layer-level inhibitory modulation parameters, to be used instead of network-level values where needed
  LayNetRescaleSpec net_rescale; // #CAT_Activation rescale layer-wide netinputs to prevent blowup, when max net exceeds specified net value
  LayAbsNetAdaptSpec abs_net_adapt; // #CAT_Learning adapt absolute netinput values (must call AbsRelNetin functions, and AdaptAbsNetin)

  // old parameters that have been moved into LeabraInhibSpec: only for converting old projects!
  enum Compute_I {		// legacy conversion inhib compute enum -- keep sync'd with LeabraInhibSpec!!
    KWTA_INHIB,			// between thresholds of k and k+1th most activated units (sets precise k value, should use i_kwta_pt = .25 std)
    KWTA_AVG_INHIB,		// average of top k vs avg of rest (provides more flexibility in actual k value, should use i_kwta_pt = .6 std)
    KWTA_KV2K,			// average of top k vs avg of next k (2k) -- avoids long "tail" of distribution of weakly active units, while providing similar flexibility as KWTA_AVG_INHIB, and also is equivalent to KWTA_INHIB for k=1 -- i_kwta_pt = .25 is std.  In general, this is now preferred to KWTA_AVG_INHIB
    KWTA_COMP_COST,		// competitor cost kwta function: inhibition is i_kwta_pt below the k'th unit's threshold inhibition value if there are no strong competitors (>comp_thr proportion of kth inhib val), and each competitor increases inhibition linearly (normalized by total possible = n-k) with gain comp_gain -- produces cleaner competitive dynamics and considerable kwta flexibility
    AVG_MAX_PT_INHIB,		// put inhib value at i_kwta_pt between avg and max values for layer
    MAX_INHIB,			// put inhib value at i_kwta_pt below max guy in layer
    UNIT_INHIB			// unit-based inhibition (g_i from netinput -- requires connections with inhib flag set to provide inhibition)
  };
  Compute_I	old_compute_i;	// #READ_ONLY #NO_SAVE #HIDDEN #AKA_compute_i how to compute inhibition (g_i): two forms of kwta or unit-level inhibition
  float		old_i_kwta_pt;	// #READ_ONLY #NO_SAVE #HIDDEN #AKA_i_kwta_pt point to place inhibition between k and k+1 (or avg and max for AVG_MAX_PT_INHIB)
  float		old_gp_i_pt;	// #READ_ONLY #NO_SAVE #HIDDEN #AKA_gp_i_pt for unit groups: point to place inhibition between avg and max for AVG_MAX_PT_INHIB

  virtual void	Init_Weights(LeabraLayer* lay);
  // #CAT_Learning initialize weight values and other permanent state
  virtual void	Init_ActAvg(LeabraLayer* lay);
  // #CAT_Activation initialize act_avg values
  virtual void	Init_Stats(LeabraLayer* lay);
  // #CAT_Statistic intialize statistic variables

  virtual void	SetCurLrate(LeabraLayer* lay, LeabraNetwork* net, int epoch);
  // #CAT_Learning set current learning rate based on epoch
  virtual void	SetLearnRule(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_Learning set current learning rule from the network

  //////////////////////////////////////////
  //	Stage 0: at start of settling	  // 
  //////////////////////////////////////////

  virtual void	Compute_Active_K(LeabraLayer* lay);
  // #CAT_Activation prior to settling: compute actual activity levels based on spec, inputs, etc
  virtual void	Compute_Active_K_impl(LeabraLayer* lay, Unit_Group* ug,
				      LeabraInhib* thr, KWTASpec& kwtspec);
  virtual int	Compute_Pat_K(LeabraLayer* lay, Unit_Group* ug, LeabraInhib* thr);
  // #CAT_Activation above are implementation helpers

  virtual void	Init_Acts(LeabraLayer* lay);
  // #CAT_Activation prior to settling: initialize dynamic state variables
  virtual void	Compute_HardClampPhase2(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_Activation prior to settling: hard-clamp inputs (special code for hard clamping in phase 2 based on prior acts)
  virtual void	Compute_HardClamp(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_Activation prior to settling: hard-clamp inputs
  virtual void	Compute_NetinScale(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_Activation prior to settling: compute netinput scaling values
  virtual void	Init_ClampNet(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_Activation prior to settling: init clamp net variable prior to sending
  virtual void	Send_ClampNet(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_Activation prior to settling: compute input from hard-clamped

  //////////////////////////////////
  //	Stage 1: netinput 	  //
  //////////////////////////////////

  virtual void 	Send_Netin(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_Activation compute net inputs
  virtual void 	Send_NetinDelta(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_Activation compute net inputs as changes in activation

  ////////////////////////////////////////////////////////////////
  //	Stage 2: netinput averages and clamping (if necc)	//
  ////////////////////////////////////////////////////////////////

  virtual void	Compute_Clamp_NetAvg(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_Activation clamp and compute averages of net inputs that were already computed
  virtual void	Compute_NetinAvg(LeabraLayer* lay, Unit_Group* ug, LeabraInhib* thr, LeabraNetwork* net);
  virtual void	Compute_SoftClamp(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_Activation soft-clamp inputs by adding to net input

  ////////////////////////////////////////
  //	Stage 3: inhibition		//
  ////////////////////////////////////////

  virtual void	Init_Inhib(LeabraLayer* lay);
  // #CAT_Activation initialize the inhibitory state values
  virtual void	Compute_Inhib(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_Activation stage two: compute the inhibition for layer
  virtual void	Compute_Inhib_impl(LeabraLayer* lay, Unit_Group* ug, LeabraInhib* thr,
				   LeabraNetwork* net, LeabraInhibSpec& ispec);
  // #CAT_Activation implementation of inhibition computation for either layer or unit group

  virtual void	Compute_Inhib_kWTA_Sort(Unit_Group* ug, LeabraInhib* thr, LeabraSort& act_buf, LeabraSort& inact_buf, int k_eff, float& k_net, int& k_idx);
  // #CAT_Activation implementation of sort into active and inactive unit buffers -- basic to various kwta functions: eff_k = effective k to use, k_net = net of kth unit (lowest unit in act_buf), k_idx = index of kth unit
  virtual void 	Compute_Inhib_BreakTie(LeabraInhib* thr);
  // #CAT_Activation break any ties in the kwta function

  virtual void	Compute_Inhib_kWTA(LeabraLayer* lay, Unit_Group* ug, LeabraInhib* thr,
				   LeabraNetwork* net, LeabraInhibSpec& ispec);
  // #CAT_Activation implementation of basic kwta inhibition computation
  virtual void	Compute_Inhib_kWTA_Avg(LeabraLayer* lay, Unit_Group* ug, LeabraInhib* thr,
				       LeabraNetwork* net, LeabraInhibSpec& ispec);
  // #CAT_Activation implementation of kwta avg-based inhibition computation
  virtual void	Compute_Inhib_kWTA_kv2k(LeabraLayer* lay, Unit_Group* ug, LeabraInhib* thr,
					LeabraNetwork* net, LeabraInhibSpec& ispec);
  // #CAT_Activation implementation of k vs. 2k wta avg-based inhibition computation
  virtual void	Compute_Inhib_kWTA_CompCost(LeabraLayer* lay, Unit_Group* ug, LeabraInhib* thr,
					    LeabraNetwork* net, LeabraInhibSpec& ispec);
  // #CAT_Activation implementation of kwta competitor cost inhibition computation
  virtual void	Compute_Inhib_AvgMaxPt(LeabraLayer* lay, Unit_Group* ug, LeabraInhib* thr,
				       LeabraNetwork* net, LeabraInhibSpec& ispec);
  // #CAT_Activation implementation of avg-max-pt inhibition computation
  virtual void	Compute_Inhib_Max(LeabraLayer* lay, Unit_Group* ug, LeabraInhib* thr,
				  LeabraNetwork* net, LeabraInhibSpec& ispec);
  // #CAT_Activation implementation of max inhibition computation
  virtual void	Compute_Inhib_kWTA_Gps(LeabraLayer* lay, LeabraNetwork* net,
				       LeabraInhibSpec& ispec);
  // #CAT_Activation implementation of GPS_THEN_UNITS kwta on groups

  virtual void	Compute_LayInhibToGps(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_Activation Stage 3.25: for layer groups, need to propagate inhib out to unit groups

  virtual void 	Compute_CtDynamicInhib(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_Activation compute extra dynamic inhibition for CtLeabra_X/CAL algorithm

  ////// Stage 3.5: apply computed inhib value to individual unit inhibitory conductances
  virtual void	Compute_ApplyInhib(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_Activation apply computed inhib value to individual unit inhibitory conductances
  virtual void	Compute_ApplyInhib_impl(LeabraLayer* lay, Unit_Group* ug, LeabraInhib* thr, LeabraNetwork* net);
  // #CAT_Activation implementation of apply inhibition computation for either layer or unit group

  ////// Stage 3.75: second pass of inhibition to do averaging
  virtual void 	Compute_InhibAvg(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_Activation stage three: compute average inhibition value
  virtual void 	Compute_InhibAvg_impl(LeabraLayer* lay, Unit_Group* ug, LeabraInhib* thr, LeabraNetwork* net);
  // #CAT_Activation stage three: compute average inhibition value

  ////////////////////////////////////////
  //	Stage 4: the final activation 	//
  ////////////////////////////////////////

  virtual void 	Compute_ActAvg_ugp(LeabraLayer*, Unit_Group* ug, LeabraInhib* thr, LeabraNetwork*);
  // #CAT_Activation compute acts.avg from act_eq
  virtual void	Compute_ActAvg(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_Activation compute acts.avg from act_eq
  virtual void 	Compute_MaxDa_ugp(LeabraLayer*, Unit_Group* ug, LeabraInhib* thr, LeabraNetwork*);
  // #CAT_Activation compute maximum delta-activation in layer (used for stopping criterion)
  virtual void	Compute_MaxDa(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_Activation compute maximum delta-activation in layer (used for stopping criterion)

  virtual void 	Compute_ActMAvg_ugp(LeabraLayer*, Unit_Group* ug, LeabraInhib* thr, LeabraNetwork*);
  // #CAT_Activation compute acts_m.avg from act_m
  virtual void	Compute_ActMAvg(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_Activation compute acts_m.avg from act_m
  virtual void 	Compute_ActPAvg_ugp(LeabraLayer*, Unit_Group* ug, LeabraInhib* thr, LeabraNetwork*);
  // #CAT_Activation compute acts_p.avg from act_p
  virtual void	Compute_ActPAvg(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_Activation compute acts_p.avg from act_p

  virtual void 	Compute_Act(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_Activation stage three: compute final activation
  virtual void 	Compute_Act_impl(LeabraLayer* lay, Unit_Group* ug, LeabraInhib* thr, LeabraNetwork* net);
  // #CAT_Activation stage three: compute final activation
  virtual void 	Compute_NetinRescale(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_Activation do net rescaling to prevent blowup based on netin.max

  virtual void	Compute_OutputName_ugp(LeabraLayer* lay, Unit_Group* ug, LeabraInhib* thr, LeabraNetwork* net);
  // #CAT_Statistic compute the output_name field from the layer acts.max_i (only for OUTPUT or TARGET layers)
  virtual void	Compute_OutputName(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_Statistic compute the output_name field from the layer acts.max_i (only for OUTPUT or TARGET layers)

  virtual float	Compute_TopKAvgAct_ugp(LeabraLayer* lay, Unit_Group* ug, LeabraInhib* thr, LeabraNetwork* net);
  // #CAT_Statistic compute the average activation of the top k most active units (useful as a measure of recognition) -- requires a kwta inhibition function to be in use, and operates on current act_eq values
  virtual float	Compute_TopKAvgAct(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_Statistic compute the average activation of the top k most active units (useful as a measure of recognition) -- requires a kwta inhibition function to be in use, and operates on current act_eq values
  virtual float	Compute_TopKAvgNetin_ugp(LeabraLayer* lay, Unit_Group* ug, LeabraInhib* thr, LeabraNetwork* net);
  // #CAT_Statistic compute the average net input of the top k most active units (useful as a measure of recognition) -- requires a kwta inhibition function to be in use, and operates on current net values
  virtual float	Compute_TopKAvgNetin(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_Statistic compute the average net input of the top k most active units (useful as a measure of recognition) -- requires a kwta inhibition function to be in use, and operates on current net values

  virtual void 	Compute_CycSynDep(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_Activation compute cycle-level synaptic depression (must be defined by appropriate subclass) -- called at end of each cycle of computation if net_misc.cyc_syn_dep is on.


  ////////////////////////////////////////
  //	Stage 5: Between Events 	//
  ////////////////////////////////////////

  virtual void	PhaseInit(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_Activation initialize start of a setting phase, set input flags appropriately, etc

  virtual void	DecayEvent(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_Activation decay activations and other state between events
  virtual void	DecayPhase(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_Activation decay activations and other state between phases
  virtual void	DecayPhase2(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_Activation decay activations and other state between second phase

  virtual void	ExtToComp(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_Activation change external inputs to comparisons (remove input)
  virtual void	TargExtToComp(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_Activation change target & external inputs to comparisons (remove targ & input)
  virtual void	PostSettle_GetMinus(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_Activation get minus phase act stats
  virtual void	PostSettle_GetPlus(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_Activation get plus phase act stats
  virtual void	PostSettle_GetMinus2(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_Activation get 2nd minus phase act stats
  virtual void	PostSettle_GetPlus2(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_Activation get 2nd plus phase act stats
  virtual void	PostSettle_GetPhaseDifRatio(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_Activation get phase dif ratio from minus to plus
  virtual void	PostSettle(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_Activation after settling, keep track of phase variables, etc.
//   virtual void	NormMPActs(LeabraLayer* lay, LeabraNetwork* net);
//   // normalize minus and plus phase activations to the same average level
  virtual void	AdaptGBarI(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_Activation adapt inhibitory conductances based on target activation values relative to current values

  virtual void	EncodeState(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_Learning encode final state information at end of trial for time-based learning across trials
  virtual void	Compute_SelfReg_Trial(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_Activation update self-regulation (accommodation, hysteresis) at end of trial

  ////////////////////////////////////////
  //	Stage 6: Learning 		//
  ////////////////////////////////////////

  virtual void 	Compute_SRAvg(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_Learning compute sending-receiving activation product averages (CtLeabra_X/CAL)
  virtual void	Init_SRAvg(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_Learning reset the sender-receiver coproduct average -- needed when no learning happening (CtLeabra_X/CAL)
  virtual void	AdaptKWTAPt(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_Activation adapt the kwta point based on average activity

  virtual void	Compute_dWt_impl(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_Learning learn: compute the weight changes -- actually do it
  virtual void	Compute_dWt_FirstPlus(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_Learning compute weight change after first plus phase has been encountered: standard layers do a weight change here, except under CtLeabra_X/CAL
  virtual void	Compute_dWt_SecondPlus(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_Learning compute weight change after second plus phase has been encountered: standard layers do NOT do a weight change here -- only selected special ones
  virtual void	Compute_dWt_Nothing(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_Learning compute weight change after final nothing phase: standard layers do a weight change here under both learning rules

  virtual void	Compute_Weights(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_Learning learn: update the weights based on computed weight changes (dwt's)

  virtual float	Compute_SSE(LeabraLayer* lay, int& n_vals, bool unit_avg = false, bool sqrt = false);
  // #CAT_Statistic compute sum squared error of activation vs target over the entire layer -- always returns the actual sse, but unit_avg and sqrt flags determine averaging and sqrt of layer's own sse value
  virtual float	Compute_NormErr_ugp(LeabraLayer* lay, Unit_Group* ug, LeabraInhib* thr,
				    LeabraNetwork* net);
  // #CAT_Statistic compute normalized binary error for given unit group -- just gets the raw sum over unit group
  virtual float	Compute_NormErr(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_Statistic compute normalized binary error -- layer-level value is already normalized, and network just averages across the layers (each layer contributes equally to overal normalized value, instead of contributing in proportion to number of units) -- returns -1 if not an err target defined in same way as sse

  ////////////////////////////////////////////////////////////////////////////////
  //	Stage 7: Parameter Adaptation over longer timesales

  virtual void	Compute_AbsRelNetin(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_Statistic compute the absolute layer-level and relative netinput from different projections into this layer
  virtual void	Compute_AvgAbsRelNetin(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_Statistic compute time-average relative netinput from different projections into this layer (e.g., every epoch)

  virtual void	Compute_TrgRelNetin(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_Learning compute target rel netin based on projection direction information plus the adapt_rel_net values in the conspec
  virtual void	Compute_AdaptRelNetin(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_Learning adapt the relative input values by changing the conspec wt_scale.rel parameter; See Compute_AdaptAbsNetin for adaptation of wt_scale.abs parameters to achieve good netinput values overall
  virtual void	Compute_AdaptAbsNetin(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_Learning adapt the absolute net input values by changing the conspec wt_scale.abs parameter

  ////////////////////////////////////////////
  //	Misc structural routines

  virtual LeabraLayer* FindLayerFmSpec(LeabraLayer* lay, int& prjn_idx, TypeDef* layer_spec);
  // #CAT_Structure find a layer that given layer receives from based on the type of layer spec
  virtual LeabraLayer* FindLayerFmSpecExact(LeabraLayer* lay, int& prjn_idx, TypeDef* layer_spec);
  // #CAT_Structure find a layer that given layer receives from based on the type of layer spec: uses exact type match, not inherits!
  static  LeabraLayer* FindLayerFmSpecNet(Network* net, TypeDef* layer_spec);
  // #CAT_Structure find a layer in network based on the type of layer spec

  virtual void	HelpConfig();	// #BUTTON #CAT_Structure get help message for configuring this spec
  override bool CheckConfig_Layer(LeabraLayer* lay, bool quiet=false);
  // check for for misc configuration settings required by different algorithms, including settings on the processes NOTE: this routine augments the default layer checks, it doesn't replace them

  virtual void	Defaults();	// #BUTTON #CONFIRM #CAT_ObjectMgmt restores default parameter settings: warning -- you will lose any unique parameters you might have set!

  void	InitLinks();
  SIMPLE_COPY(LeabraLayerSpec);
  TA_BASEFUNS(LeabraLayerSpec);
protected:
  void	UpdateAfterEdit_impl();
private:
  void 	Initialize();
  void	Destroy()		{ CutLinks(); }
};

SpecPtr_of(LeabraLayerSpec);
TA_SMART_PTRS(LeabraLayerSpec)

class LEABRA_API AvgMaxVals : public taOBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra holds average and max statistics
INHERITED(taOBase)
public:
  float		avg;		// #DMEM_AGG_SUM average value
  float		max;		// #DMEM_AGG_SUM maximum value
  int 		max_i;		// index of unit with maximum value
  
  void	Copy_(const AvgMaxVals& cp);
  void 	Defaults()	{ Initialize(); }
  TA_BASEFUNS(AvgMaxVals);
private:
  void	Initialize();
  void 	Destroy()	{ };
};

class LEABRA_API KWTAVals : public taOBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra holds values for kwta stuff
INHERITED(taOBase)
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

  void	Copy_(const KWTAVals& cp);
  TA_BASEFUNS(KWTAVals);
private:
  void	Initialize();
  void 	Destroy()	{ };
};

class LEABRA_API AdaptIVals : public taOBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra holds values for adapting kwta stuff
INHERITED(taOBase)
public:
  float		avg_avg;	// average of the average activation in a layer
  float		i_kwta_pt;	// adapting point to place inhibition between k and k+1 for kwta
  float		g_bar_i;	// adapting g_bar.i value 
  float		g_bar_l;	// adapting g_bar.l value 

  void	Copy_(const AdaptIVals& cp);
  TA_BASEFUNS(AdaptIVals);
private:
  void	Initialize();
  void 	Destroy()	{ };
};

class LEABRA_API InhibVals : public taOBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra holds values for inhibition
INHERITED(taOBase)
public:
  float		kwta;		// inhibition due to kwta function
  float		g_i;		// overall value of the inhibition
  float		gp_g_i;		// g_i from the layer or unit group, if applicable
  float		g_i_orig; 	// original value of the inhibition (before any layer group effects set in)

  void	Copy_(const InhibVals& cp);
  void 	Defaults()	{ Initialize(); }
  TA_BASEFUNS(InhibVals);
private:
  void	Initialize();
  void 	Destroy()	{ };
};

class LEABRA_API LeabraInhib {
  // ##CAT_Leabra holds threshold-computation values, used as a parent class for layers, etc
public:
  LeabraSort 	active_buf;	// #HIDDEN #NO_SAVE #CAT_Activation list of active units
  LeabraSort 	inact_buf;	// #HIDDEN #NO_SAVE #CAT_Activation list of inactive units
  LeabraSort 	active_2k_buf;	// #HIDDEN #NO_SAVE #CAT_Activation list of 2k active units
  LeabraSort 	inact_2k_buf;	// #HIDDEN #NO_SAVE #CAT_Activation list of 2k inactive units

  AvgMaxVals	netin;		// #READ_ONLY #EXPERT #CAT_Activation net input values for the layer
  AvgMaxVals	i_thrs;		// #READ_ONLY #EXPERT #CAT_Activation inhibitory threshold values for the layer
  AvgMaxVals	acts;		// #READ_ONLY #EXPERT #CAT_Activation activation values for the layer
  AvgMaxVals	acts_p;		// #READ_ONLY #EXPERT #CAT_Activation plus-phase activation stats for the layer
  AvgMaxVals	acts_m;		// #READ_ONLY #EXPERT #CAT_Activation minus-phase activation stats for the layer
  float		phase_dif_ratio; // #READ_ONLY #SHOW #CAT_Activation phase-difference ratio (acts_m.avg / acts_p.avg)
  AvgMaxVals	acts_p2;	// #READ_ONLY #EXPERT #CAT_Activation second plus-phase activation stats for the layer
  AvgMaxVals	acts_m2;	// #READ_ONLY #EXPERT #CAT_Activation second minus-phase activation stats for the layer
 
  KWTAVals	kwta;		// #READ_ONLY #EXPERT #CAT_Activation values for kwta -- activity levels, etc NOTE THIS IS A COMPUTED VALUE: k IS SET IN LayerSpec!
  InhibVals	i_val;		// #READ_ONLY #SHOW #CAT_Activation inhibitory values computed by kwta
  AvgMaxVals	un_g_i;		// #READ_ONLY #EXPERT #CAT_Activation average and stdev (not max) values for unit inhib-to-thresh
  AdaptIVals	adapt_i;	// #READ_ONLY #AKA_adapt_pt #EXPERT #CAT_Activation adapting inhibition values
  float		maxda;		// #GUI_READ_ONLY #SHOW #CAT_Statistic maximum change in activation (delta-activation) over network; used in stopping settling

  void	Inhib_SetVals(float val)	{ i_val.g_i = val; i_val.g_i_orig = val; }
  void	Inhib_ResetSortBuf() 		{ active_buf.size = 0; inact_buf.size = 0; }
  void	Inhib_Init_Acts(LeabraLayerSpec* lay);
  void	Inhib_Initialize();
  void	Inhib_Copy_(const LeabraInhib& cp);
};

class LEABRA_API LeabraLayer : public Layer, public LeabraInhib {
  // #STEM_BASE ##CAT_Leabra layer that implements the Leabra algorithms
INHERITED(Layer)
public:
  LeabraLayerSpec_SPtr	spec;	// #CAT_Structure the spec for this layer: controls all functions of layer
  float		stm_gain;	// #READ_ONLY #EXPERT #CAT_Activation actual stim gain for soft clamping, can be incremented to ensure clamped units active
  bool		hard_clamped;	// #READ_ONLY #SHOW #CAT_Activation this layer is actually hard clamped
  float		sravg_s_sum;	// #READ_ONLY #EXPERT #CAT_Activation sum of sravg_s (short time-scale, plus phase) weightings (count of number of times sravg has been computed) -- used for normalizing the overall average
  float		sravg_s_nrm;	// #READ_ONLY #EXPERT #CAT_Activation normalization term computed from sravg_s_sum -- multiply connection-level sravg_s by this value
  float		sravg_m_sum;	// #READ_ONLY #EXPERT #CAT_Activation sum of sravg_m (medium time-scale, trial) weightings (count of number of times sravg has been computed) -- used for normalizing the overall average
  float		sravg_m_nrm;	// #READ_ONLY #EXPERT #CAT_Activation normalization term computed from sravg_m_sum -- multiply connection-level sravg_m by this value
  float		ravg_l_avg;	// #READ_ONLY #EXPERT #CAT_Activation layer-wise average of ravg_l values in the layers
  float		dav;		// #READ_ONLY #EXPERT #CAT_Learning dopamine-like modulatory value (where applicable)
  float		net_rescale;	// #READ_ONLY #EXPERT #CAT_Activation computed netinput rescaling factor (updated by net_rescale)
  AvgMaxVals	avg_netin;	// #READ_ONLY #EXPERT #CAT_Activation net input values for the layer, averaged over an epoch-level timescale
  AvgMaxVals	avg_netin_sum;	// #READ_ONLY #EXPERT #CAT_Activation #DMEM_AGG_SUM sum of net input values for the layer, for computing average over an epoch-level timescale
  int		avg_netin_n;	// #READ_ONLY #EXPERT #CAT_Activation #DMEM_AGG_SUM number of times sum is updated for computing average
  float		norm_err;	// #GUI_READ_ONLY #SHOW #CAT_Statistic normalized binary error value for this layer, computed subject to the parameters on the network
  int		da_updt;	// #READ_ONLY #EXPERT #CAT_Learning true if da triggered an update (either + to store or - reset)
  int_Array	misc_iar;	// #HIDDEN #CAT_Activation misc int array of data

#ifdef DMEM_COMPILE
  DMemAggVars	dmem_agg_sum;		// #IGNORE aggregation of network variables using SUM op (currently only OP in use -- add others as needed)
  virtual void 	DMem_InitAggs();
  // #IGNORE initialize aggregation stuff
  virtual void	DMem_ComputeAggs(MPI_Comm comm);
  // #IGNORE aggregate network variables across procs for trial-level dmem 
#endif

  override void	BuildUnits();

  void	Init_Weights() 	{ if(spec) spec->Init_Weights(this); }
  // #CAT_Learning initialize weight values and other permanent state
  void	Init_ActAvg() 	{ spec->Init_ActAvg(this); }
  // #CAT_Activation initialize act_avg values
  void	Init_Inhib() 	{ spec->Init_Inhib(this); } // initialize inhibitory state
  // #CAT_Activation initialize the inhibitory state values

  void	SetCurLrate(LeabraNetwork* net, int epoch) { spec->SetCurLrate(this, net, epoch); }
  // #CAT_Learning set current learning rate based on epoch
  void	SetLearnRule(LeabraNetwork* net) 	   { spec->SetLearnRule(this, net); }
  // #CAT_Learning set current learning rule from the network
  
  void	Compute_Active_K()			{ spec->Compute_Active_K(this); }
  // #CAT_Activation prior to settling: compute actual activity levels based on spec, inputs, etc
  void	Init_Acts() 				{ if(spec) spec->Init_Acts(this); }
  // #CAT_Activation prior to settling: initialize dynamic state variables

  void	Compute_HardClamp(LeabraNetwork* net) 	{ spec->Compute_HardClamp(this, net); }
  // #CAT_Activation prior to settling: hard-clamp inputs
  void	Compute_NetinScale(LeabraNetwork* net) 	{ spec->Compute_NetinScale(this, net); }
  // #CAT_Activation prior to settling: compute netinput scaling values
  void	Init_ClampNet(LeabraNetwork* net) 	{ spec->Init_ClampNet(this, net); }
  // #CAT_Activation prior to settling: init clamp net variable prior to sending
  void	Send_ClampNet(LeabraNetwork* net) 	{ spec->Send_ClampNet(this, net); }
  // #CAT_Activation prior to settling: compute input from hard-clamped

  void	Send_Netin(LeabraNetwork* net)		{ spec->Send_Netin(this, net); }
  // #CAT_Activation compute net inputs
  void	Send_Netin()				{ spec->Send_Netin(this, NULL); }
  void	Send_NetinDelta(LeabraNetwork* net)	{ spec->Send_NetinDelta(this, net); }
  // #CAT_Activation compute net inputs as changes in activation
  void	Send_NetinDelta()			{ spec->Send_NetinDelta(this, NULL); }

  void	Compute_Clamp_NetAvg(LeabraNetwork* net)  { spec->Compute_Clamp_NetAvg(this, net); }
  // #CAT_Activation clamp and compute averages of net inputs that were already computed

  void	Compute_Inhib(LeabraNetwork* net) 	{ spec->Compute_Inhib(this, net); }
  // #CAT_Activation compute the inhibition for layer
  void	Compute_LayInhibToGps(LeabraNetwork* net) { spec->Compute_LayInhibToGps(this, net); }
  // #CAT_Activation for layer groups, need to propagate inhib out to unit groups
  void	Compute_ApplyInhib(LeabraNetwork* net)	{ spec->Compute_ApplyInhib(this, net); }
  // #CAT_Activation apply inhibition value to unit inhibitory conductances
  void	Compute_InhibAvg(LeabraNetwork* net)	{ spec->Compute_InhibAvg(this, net); }
  // #CAT_Activation compute average inhibition value (integrating unit inhib etc)

  override void	Compute_Act()	{ spec->Compute_Act(this, (LeabraNetwork*)own_net); }

  float	Compute_TopKAvgAct(LeabraNetwork* net)  { return spec->Compute_TopKAvgAct(this, net); }
  // #CAT_Statistic compute the average activation of the top k most active units (useful as a measure of recognition) -- requires a kwta inhibition function to be in use, and operates on current act_eq values
  float	Compute_TopKAvgNetin(LeabraNetwork* net)  { return spec->Compute_TopKAvgNetin(this, net); }
  // #CAT_Statistic compute the average netinput of the top k most active units (useful as a measure of recognition) -- requires a kwta inhibition function to be in use, and operates on current act_eq values

  void 	Compute_CycSynDep(LeabraNetwork* net) 
  { ((LeabraLayerSpec*)spec.SPtr())->Compute_CycSynDep(this, net); }
  // #CAT_Activation compute cycle-level synaptic depression (must be defined by appropriate subclass) -- called at end of each cycle of computation if net_misc.cyc_syn_dep is on.

  void	Compute_ActMAvg(LeabraNetwork* net) { spec->Compute_ActMAvg(this, net); }
  // #CAT_Activation compute acts_m.avg from act_m
  void	Compute_ActPAvg(LeabraNetwork* net) { spec->Compute_ActPAvg(this, net); }
  // #CAT_Activation compute acts_p.avg from act_p

  void	PhaseInit(LeabraNetwork* net)		{ spec->PhaseInit(this, net); }
  // #CAT_Activation initialize start of a setting phase, set input flags appropriately, etc
  void	DecayEvent(LeabraNetwork* net)		{ spec->DecayEvent(this, net); }
  // #CAT_Activation decay activations and other state between events
  void	DecayPhase(LeabraNetwork* net)    	{ spec->DecayPhase(this, net); }
  // #CAT_Activation decay activations and other state between phases
  void	DecayPhase2(LeabraNetwork* net)  	{ spec->DecayPhase2(this, net); }
  // #CAT_Activation decay activations and other state between second phase

  void	ExtToComp(LeabraNetwork* net)		{ spec->ExtToComp(this, net); }
  // #CAT_Activation change external inputs to comparisons (remove input)
  void	TargExtToComp(LeabraNetwork* net)	{ spec->TargExtToComp(this, net); }
  // #CAT_Activation change target & external inputs to comparisons (remove targ & input)
  void	PostSettle(LeabraNetwork* net)		{ spec->PostSettle(this, net); }
  // #CAT_Activation after settling, keep track of phase variables, etc.

  void	EncodeState(LeabraNetwork* net)		{ spec->EncodeState(this, net); }
  // #CAT_Learning encode final state information at end of trial for time-based learning across trials
  void	Compute_SelfReg_Trial(LeabraNetwork* net) { spec->Compute_SelfReg_Trial(this, net); }
  // #CAT_Activation update self-regulation (accommodation, hysteresis) at end of trial

  void 	Compute_SRAvg(LeabraNetwork* net) 
  { ((LeabraLayerSpec*)spec.SPtr())->Compute_SRAvg(this, net); }
  // #CAT_Learning compute sending-receiving activation product averages (CtLeabra_X/CAL)
  void 	Init_SRAvg(LeabraNetwork* net) 
  { ((LeabraLayerSpec*)spec.SPtr())->Init_SRAvg(this, net); }
  // #CAT_Learning initialize sending-receiving activation product averages (CtLeabra_X/CAL)

  void	Compute_dWt_FirstPlus(LeabraNetwork* net)
  { ((LeabraLayerSpec*)spec.SPtr())->Compute_dWt_FirstPlus(this, net); }
  // #CAT_Learning compute weight change after first plus phase has been encountered: standard layers do a weight change here, except under CtLeabra_X/CAL
  void	Compute_dWt_SecondPlus(LeabraNetwork* net)
  { ((LeabraLayerSpec*)spec.SPtr())->Compute_dWt_SecondPlus(this, net); }
  // #CAT_Learning compute weight change after second plus phase has been encountered: standard layers do NOT do a weight change here -- only selected special ones
  void	Compute_dWt_Nothing(LeabraNetwork* net)
  { ((LeabraLayerSpec*)spec.SPtr())->Compute_dWt_Nothing(this, net); }
  // #CAT_Learning compute weight change after final nothing phase: standard layers do a weight change here under both learning rules

  override void	Compute_dWt() 	{ spec->Compute_dWt_impl(this, (LeabraNetwork*)own_net); }
  // note: do not call this function directly -- just does weight updates directly
  override void	Compute_Weights() { spec->Compute_Weights(this, (LeabraNetwork*)own_net); }

  override float Compute_SSE(int& n_vals, bool unit_avg = false, bool sqrt = false)
  { return spec->Compute_SSE(this, n_vals, unit_avg, sqrt); }

  virtual float Compute_NormErr(LeabraNetwork* net)
  { return spec->Compute_NormErr(this, net); }
  // #CAT_Statistic compute normalized binary error across layer (returns normalized value or -1 for not applicable, averaged at network level -- see layerspec for more info)

  void	Compute_AbsRelNetin(LeabraNetwork* net)	{ spec->Compute_AbsRelNetin(this, net); }
  // #CAT_Statistic compute the absolute layer-level and relative netinput from different projections into this layer
  void	Compute_AvgAbsRelNetin(LeabraNetwork* net) { spec->Compute_AvgAbsRelNetin(this, net); }
  // #CAT_Statistic compute the average absolute layer-level and relative netinput from different projections into this layer (over an epoch-level timescale)

  void	Compute_TrgRelNetin(LeabraNetwork* net) { spec->Compute_TrgRelNetin(this, net); }
  // #CAT_Statistic compute target rel netin based on projection direction information plus the adapt_rel_net values in the conspec
  void	Compute_AdaptRelNetin(LeabraNetwork* net) { spec->Compute_AdaptRelNetin(this, net); }
  // #CAT_Statistic adapt the relative input values by changing the conspec wt_scale.rel parameter; See Compute_AdaptAbsNetin for adaptation of wt_scale.abs parameters to achieve good netinput values overall
  void	Compute_AdaptAbsNetin(LeabraNetwork* net) { spec->Compute_AdaptAbsNetin(this, net); }
  // #CAT_Statistic adapt the absolute net input values by changing the conspec wt_scale.abs parameter

  virtual void	ResetSortBuf();

  bool		SetLayerSpec(LayerSpec* sp);
  LayerSpec*	GetLayerSpec()		{ return (LayerSpec*)spec.SPtr(); }

  void	InitLinks();
  void	CutLinks();
  void	Copy_(const LeabraLayer& cp);
  TA_BASEFUNS(LeabraLayer);
protected:
  void	UpdateAfterEdit_impl();
  override void  CheckThisConfig_impl(bool quiet, bool& rval);
private:
  void	Initialize();
  void	Destroy()		{ CutLinks(); }
};
TA_SMART_PTRS(LeabraLayer)

class LEABRA_API LeabraUnit_Group : public Unit_Group, public LeabraInhib {
  // #STEM_BASE ##CAT_Leabra for independent subgroups of competing units within a single layer
INHERITED(Unit_Group)
public:
  int		misc_state;	// #CAT_Activation miscellaneous state variable
  int		misc_state1;	// #CAT_Activation second miscellaneous state variable 
  int		misc_state2;	// #CAT_Activation third miscellaneous state variable 

  void	InitLinks();
  void	Copy_(const LeabraUnit_Group& cp);
  TA_BASEFUNS(LeabraUnit_Group);
private:
  void	Initialize();
  void	Destroy()		{ };
};


//////////////////////////
// 	Network		//
//////////////////////////

class LEABRA_API LeabraNetMisc : public taOBase {
  // ##INLINE ##NO_TOKENS ##CAT_Leabra misc network-level parameters for Leabra
INHERITED(taOBase)
public:
  bool		cyc_syn_dep;	// if true, enable synaptic depression calculations at the synapse level (also need conspecs to implement this -- this just enables computation)
  int		syn_dep_int;	// [20] #CONDEDIT_ON_cyc_syn_dep synaptic depression interval -- how frequently to actually perform synaptic depression within a trial (uses ct_cycle variable which counts up continously through trial)

  SIMPLE_COPY(LeabraNetMisc);
  TA_BASEFUNS(LeabraNetMisc);
// protected:
//   void UpdateAfterEdit_impl();

private:
  void	Initialize();
  void 	Destroy()	{ };
};

class LEABRA_API CtTrialTiming : public taOBase {
  // ##INLINE ##NO_TOKENS ##CAT_Leabra timing parameters for a single stimulus input trial of ct learning algorithm
INHERITED(taOBase)
public:
  int		minus;		// [50] number of cycles to run in the minus phase with only inputs and no targets (used by CtLeabraSettle program), sets cycle_max -- can be 0
  int		plus;		// [20] number of cycles to run in the plus phase with input and target activations (used by CtLeabraSettle program), sets cycle_max -- must be > 0
  int		inhib;		// [1] number of cycles to run in the final inhibitory phase -- network can do MINUS_PLUS_PLUS, MINUS_PLUS_MINUS, or MINUS_PLUS_NOTHING for inputs on this phase
  int		n_avg_only_epcs; // #DEF_2 number of epochs during which time only sravg values are accumulated, and no learning occurs

  int		total_cycles;	// #READ_ONLY computed total number of cycles per trial
  int		inhib_start;	// #READ_ONLY computed start of inhib phase (=minus + plus)

  SIMPLE_COPY(CtTrialTiming);
  TA_BASEFUNS(CtTrialTiming);
protected:
  void UpdateAfterEdit_impl();

private:
  void	Initialize();
  void 	Destroy()	{ };
};

class LEABRA_API CtSRAvgSpec : public taOBase {
  // ##INLINE ##NO_TOKENS ##CAT_Leabra how to compute the sravg value as a function of cycles 
INHERITED(taOBase)
public:
  int		start;		// [30] number of cycles from the start of a new pattern to start computing sravg value -- avoid transitional states that are too far away from attractor state
  int		end;		// [1] number of cycles from the start of the final inhibitory phase to continue recording sravg
  int		interval;	// [5] how frequently to compute sravg -- more infrequent updating saves computational costs as sravg is expensive
  int		plus_s_st;	// [10] how many cycles into the plus phase should the short time scale sravg computation start (only for TRIAL sravg computation)

  SIMPLE_COPY(CtSRAvgSpec);
  TA_BASEFUNS(CtSRAvgSpec);
  //protected:
  //  void UpdateAfterEdit_impl();

private:
  void	Initialize();
  void 	Destroy()	{ };
};

class LEABRA_API CtSineInhibMod : public taBase {
  // ##INLINE ##NO_TOKENS ##CAT_Leabra sinusoidal inhibitory modulation parameters simulating initial burst of activation and subsequent oscillatory ringing
INHERITED(taBase)
public:
  int		start;		// [30] number of cycles from onset of new input to start applying sinusoidal inhibitory modulation
  int		duration;	// [20] number of cycles from start to apply modulation
  float		n_pi;		// number of multiples of PI to produce within duration of modulation (1.0 = positive only wave, 2.0 = full pos/neg wave, 4.0 = two waves, etc)
  float		burst_i;	// [.02] maximum reduction in inhibition as a proportion of computed kwta value to subtract for positive activation (burst) phase of wave -- value should be a positive number
  float		trough_i;	// [.02] maximum extra inhibition as proportion of computed kwta value to add for negative activation (trough) phase of wave -- value shoudl be a positive number

  float		GetInhibMod(int ct_cycle, float bi, float ti) {
    if((ct_cycle < start) || (ct_cycle >= (start + duration))) return 0.0f;
    float rads = (((float)(ct_cycle - start) / (float)duration) * taMath_float::pi * n_pi);
    float sinval = -taMath_float::sin(rads);
    if(sinval < 0.0f) 	sinval *= bi; // signs are reversed for inhib vs activation
    else		sinval *= ti;
    return sinval;
  }
  // returns inhibitory modulation to apply as a fraction of computed kwta value

  SIMPLE_COPY(CtSineInhibMod);
  TA_BASEFUNS(CtSineInhibMod);
// protected:
//   void UpdateAfterEdit_impl();

private:
  void	Initialize();
  void 	Destroy()	{ };
};

class LEABRA_API CtFinalInhibMod : public taBase {
  // ##INLINE ##NO_TOKENS ##CAT_Leabra extra inhibition to apply at end of stimulus processing during inhib phase, to clear out existing pattern
INHERITED(taBase)
public:
  int		start;		// number of cycles into inhib phase for inhibition ramp to start
  int		end;		// number of cycles into inhib phase for inhibition ramp to end -- remains at full inhibition level from end to end of inhib phase
  float		inhib_i;	// [.05 when in use] maximum extra inhibition as proportion of computed kwta value to add during final inhib phase

  float		GetInhibMod(int inh_cyc, float ii) {
    if(inh_cyc < start) return 0.0f;
    if(inh_cyc >= end) return ii;
    float slp = (float)(inh_cyc - start) / (float)(end - start);
    return slp * ii;
  }
  // returns inhibitory modulation to apply as a fraction of computed kwta value

  SIMPLE_COPY(CtFinalInhibMod);
  TA_BASEFUNS(CtFinalInhibMod);
// protected:
//   void UpdateAfterEdit_impl();

private:
  void	Initialize();
  void 	Destroy()	{ };
};

class LEABRA_API LeabraNetwork : public Network {
  // #STEM_BASE ##CAT_Leabra network that uses the Leabra algorithms and objects
INHERITED(Network)
public:
  // IMPORTANT programming note: this enum must be same as in LeabraConSpec, and all Ct-like 
  // algos that compute SRAvg etc must be *after* CTLEABRA_CAL, and vice-versa
  // (i.e., logic uses >= and <= on CTLEABRA_CAL for some things)

  enum LearnRule {
    LEABRA_CHL,			// use the standard Leabra Contrastive Hebbian Learning rule: (s+r+) - (s-r-) (s=sender,r=recv +=plus phase, -=minus phase)
    CTLEABRA_CAL,		// continuous-time Leabra Contrastive Attractor Learning rule: <sr>_s - <sr>_m -- s=sender, r=recv, <> = avg over short (plus phase) and medium (trial) time scales
    CTLEABRA_XCAL,		// continuous-time Leabra temporally eXtended Contrastive Attractor Learning rule: <sr>_s - <sr>_m - <r>_l -- s=sender, r=recv, <> = avg over short (plus phase), medium (trial), long (epoch) time scales
  };

  enum StateInit {		// ways of initializing the state of the network
    DO_NOTHING,			// do nothing
    INIT_STATE,			// initialize state
    DECAY_STATE,		// decay the state
  };
    
  enum Phase {
    MINUS_PHASE = 0,		// minus phase
    PLUS_PHASE = 1,		// plus phase
  };

  enum PhaseOrder {
    MINUS_PLUS,			// standard minus-plus (err and assoc)
    PLUS_MINUS,			// reverse order: plus phase first
    PLUS_ONLY,			// only present the plus phase (hebbian-only)
    MINUS_PLUS_NOTHING,		// standard for CtLeabra_X/CAL and auto-encoder version with final 'nothing' minus phase
    MINUS_PLUS_MINUS,		// alternative version for CtLeabra_X/CAL with input still in final phase -- this 2nd minus is also marked as a nothing_phase 
    PLUS_NOTHING,		// just an auto-encoder (no initial minus phase)
    MINUS_PLUS_PLUS,		// two plus phases for gated context layer updating in second plus phase, for the PBWM model
    MINUS_PLUS_PLUS_NOTHING,	// PBWM in CtLeabra_X/CAL mode
    MINUS_PLUS_PLUS_MINUS,	// PBWM in CtLeabra_X/CAL mode, alternative final inhib stage
  };

  LearnRule	learn_rule;	// The variant of Leabra learning rule to use 
  PhaseOrder	phase_order;	// #APPLY_IMMED [Default: MINUS_PLUS] #CAT_Counter number and order of phases to present
  bool		no_plus_test;	// #DEF_true #CAT_Counter don't run the plus phase when testing
  StateInit	trial_init;	// #DEF_DECAY_STATE #CAT_Activation how to initialize network state at start of trial
  StateInit	sequence_init;	// #DEF_DO_NOTHING #CAT_Activation how to initialize network state at start of a sequence of trials

  Phase		phase;		// #GUI_READ_ONLY #SHOW #CAT_Counter #VIEW type of settling phase
  bool		nothing_phase;	// #GUI_READ_ONLY #SHOW #CAT_Counter the current phase is a NOTHING phase (phase will indicate MINUS for learning purposes)
  int		phase_no;	// #GUI_READ_ONLY #SHOW #CAT_Counter #VIEW phase as an ordinal number (regular phase is Phase enum)
  int		phase_max;	// #CAT_Counter maximum number of phases to run (note: this is set by Trial_Init depending on phase_order)

  int		ct_cycle;	// #GUI_READ_ONLY #SHOW #CAT_Counter #VIEW continuous time cycle counter: counts up from start of trial 

  int		cycle_max;	// #DEF_60 #CAT_Counter #CONDEDIT_ON_learn_rule:LEABRA_CHL maximum number of cycles to settle for: note for CtLeabra_X/CAL this is overridden by phase specific settings by the settle process
  int		min_cycles;	// #DEF_15 #CAT_Counter #CONDEDIT_ON_learn_rule:LEABRA_CHL minimum number of cycles to settle for
  int		min_cycles_phase2; // #DEF_35 #CAT_Counter #CONDEDIT_ON_learn_rule:LEABRA_CHL minimum number of cycles to settle for in second phase

  CtTrialTiming	 ct_time;	// #CAT_Learning #CONDEDIT_OFF_learn_rule:LEABRA_CHL timing parameters for ct leabra trial: Settle_Init sets the cycle_max based on these values
  CtSRAvgSpec	 ct_sravg;	// #CAT_Learning #CONDEDIT_OFF_learn_rule:LEABRA_CHL parameters controlling computation of sravg value as a function of cycles
  CtSineInhibMod ct_sin_i;	// #CAT_Learning #CONDEDIT_OFF_learn_rule:LEABRA_CHL sinusoidal inhibition parameters for inhibitory modulations during trial, simulating oscillations resulting from imperfect inhibtory set point behavior
  CtFinalInhibMod ct_fin_i;	// #CAT_Learning #CONDEDIT_OFF_learn_rule:LEABRA_CHL final inhibition parameters for extra inhibition to apply during final inhib phase, simulating slow-onset GABA currents

  float		minus_cycles;	// #GUI_READ_ONLY #SHOW #CAT_Statistic #VIEW cycles to settle in the minus phase -- this is the typical settling time statistic to record
  float		avg_cycles;	// #GUI_READ_ONLY #SHOW #CAT_Statistic average settling cycles in the minus phase (computed over previous epoch)
  float		avg_cycles_sum; // #READ_ONLY #DMEM_AGG_SUM #CAT_Statistic sum for computing current average cycles in this epoch
  int		avg_cycles_n;	// #READ_ONLY #DMEM_AGG_SUM #CAT_Statistic N for average cycles computation for this epoch

  String	minus_output_name; // #GUI_READ_ONLY #SHOW #CAT_Statistic #VIEW output_name in the minus phase -- for recording in logs as network's response (output_name in plus phase is clamped target value)

  LeabraNetMisc	net_misc;	// misc network level parameters for leabra
  int		netin_mod;	// #DEF_1 net #CAT_Optimization input computation modulus: how often to compute netinput vs. activation update (2 = faster)
  bool		send_delta;	// #DEF_true #CAT_Optimization send netin deltas instead of raw netin: more efficient (automatically sets corresponding unitspec flag)

  float		send_pct;	// #GUI_READ_ONLY #SHOW #CAT_Statistic proportion of sending units that actually sent activations on this cycle
  int		send_pct_n;	// #READ_ONLY #CAT_Statistic number of units sending activation this cycle
  int		send_pct_tot;	// #READ_ONLY #CAT_Statistic total number of units that could send activation this cycle
  float		avg_send_pct;	// #GUI_READ_ONLY #SHOW #CAT_Statistic average proportion of units sending activation over an epoch
  float		avg_send_pct_sum; // #READ_ONLY #DMEM_AGG_SUM #CAT_Statistic sum for computing current average send_pct per epoch (integrates over cycles and trials etc)
  int		avg_send_pct_n; // #READ_ONLY #DMEM_AGG_SUM #CAT_Statistic sum for computing current average send_pct per epoch (integrates over cycles and trials etc)

  float		maxda_stopcrit;	// #DEF_0.005 #CAT_Statistic stopping criterion for max da
  float		maxda;		// #GUI_READ_ONLY #SHOW #CAT_Statistic #VIEW maximum change in activation (delta-activation) over network; used in stopping settling

  float		trg_max_act_stopcrit;	// #CAT_Statistic stopping criterion for target-layer maximum activation (can be used for stopping settling)
  float		trg_max_act;	// #GUI_READ_ONLY #SHOW #CAT_Statistic target-layer maximum activation (can be used for stopping settling)

  float		ext_rew;	// #GUI_READ_ONLY #SHOW #CAT_Statistic #VIEW external reward value (on this trial)
  float		avg_ext_rew;	// #GUI_READ_ONLY #SHOW #CAT_Statistic average external reward value (computed over previous epoch)
  float		avg_ext_rew_sum; // #READ_ONLY #DMEM_AGG_SUM #CAT_Statistic sum for computing current average external reward value in this epoch
  int		avg_ext_rew_n;	// #READ_ONLY #DMEM_AGG_SUM #CAT_Statistic N for average external reward value computation for this epoch

  bool		off_errs;	// #DEF_true #CAT_Statistic include in norm_err computation units that were incorrectly off (should have been on but were actually off) -- either 1 or both of off_errs and on_errs must be set
  bool		on_errs;	// #DEF_true #CAT_Statistic include in norm_err computation units that were incorrectly on (should have been off but were actually on) -- either 1 or both of off_errs and on_errs must be set
  float		norm_err;	// #GUI_READ_ONLY #SHOW #CAT_Statistic #VIEW normalized binary (Hamming) error on this trial: number of units that were incorrectly activated or incorrectly inactivated (see off_errs to exclude latter)
  float		avg_norm_err;	// #GUI_READ_ONLY #SHOW #CAT_Statistic average normalized binary error value (computed over previous epoch)
  float		avg_norm_err_sum; // #READ_ONLY #DMEM_AGG_SUM #CAT_Statistic sum for computing current average norm err in this epoch
  int		avg_norm_err_n;	// #READ_ONLY #DMEM_AGG_SUM #CAT_Statistic N for average norm err value computation for this epoch

  override void	Init_Counters();
  override void	Init_Stats();
  override void	Init_Sequence();

  virtual void	SetLearnRule_ConSpecs(BaseSpec_Group* spgp);
  // #IGMORE set the current learning rule into all conspecs in given spec group (recursive)
  virtual void	SetLearnRule();
  // #CAT_ObjectMgmt set the current learning rule into the conspecs on this network (done by network UAE only when rule changed)

  // single cycle-level functions
  virtual void	Compute_Netin();	// #CAT_Cycle compute netinputs (sender based, if send_delta, then only when sender activations change)
  virtual void	Compute_Clamp_NetAvg();	// #CAT_Cycle add in clamped netinput values (computed once at start of settle) and average netinput values
  virtual void	Compute_Inhib(); // #CAT_Cycle compute inhibitory conductances (kwta)
  virtual void	Compute_ApplyInhib(); // #CAT_Cycle apply inhibitory conductances from kwta to individual units
  virtual void	Compute_InhibAvg(); // #CAT_Cycle compute average inhibitory conductances (unit-level inhib)
  virtual void	Compute_Act();	// #CAT_Cycle compute activations, and max delta activation

  virtual void 	Compute_CycSynDep();
  // #CAT_Activation compute cycle-level synaptic depression (must be defined by appropriate subclass) -- called at end of each cycle of computation if net_misc.cyc_syn_dep is on.

  virtual void 	Compute_SRAvg();
  // #CAT_Learning compute sending-receiving activation coproduct averages (CtLeabra_X/CAL)
  virtual void 	Init_SRAvg();
  // #CAT_Learning initialize sending-receiving activation coproduct averages (CtLeabra_X/CAL)

  virtual void	Cycle_Run();	// #CAT_Cycle compute one cycle of updating: netinput, inhibition, activations

  // settling-phase level functions
  virtual void	Compute_Active_K(); // #CAT_SettleInit determine the active k values for each layer based on pcts, etc (called by Settle_Init)
  virtual void	DecayPhase();	// #CAT_SettleInit decay activations and other state between minus-plus phases (called by Settle_Init)
  virtual void	DecayPhase2();	// #CAT_SettleInit decay activations and other state between second and third phase (if applicable) (called by Settle_Init)
  virtual void	PhaseInit();	// #CAT_SettleInit initialize at start of settling phase -- sets external input flags based on phase (called by Settle_Init)
  virtual void	ExtToComp();	// #CAT_SettleInit move external input values to comparison values (not currently used)
  virtual void	TargExtToComp(); // #CAT_SettleInit move target and external input values to comparison (for PLUS_NOTHING, called by Settle_Init)
  virtual void	Compute_HardClamp(); // #CAT_SettleInit compute hard clamping from external inputs (called by Settle_Init)
  virtual void	Compute_NetinScale(); // #CAT_SettleInit compute netinput scaling values by projection (called by Settle_Init)
  virtual void	Send_ClampNet(); // #CAT_SettleInit send clamped activation netinputs to other layers -- only needs to be computed once (called by Settle_Init)
  virtual void  Settle_Init_Decay(); // #CAT_SettleInit logic for performing decay and updating external input settings as a function of phase
  virtual void  Settle_Init_CtTimes(); // #CAT_SettleInit initialize cycles based on network phases for CtLeabra_X/CAL

  virtual void  Settle_Init();	  // #CAT_SettleInit initialize network for settle-level processing (decay, active k, hard clamp, netscale, clampnet)

  virtual void	PostSettle();	// #CAT_SettleFinal perform computations in layers at end of settling  (called by Settle_Final)

  virtual void	Settle_Final();	  // #CAT_SettleFinal do final processing after settling (postsettle, Compute_dWt if needed

  // trial-level functions
  virtual void	SetCurLrate();	// #CAT_TrialInit set the current learning rate according to the LeabraConSpec parameters
  virtual void	DecayEvent();	// #CAT_TrialInit decay activations and other state between events (trial-level)
  virtual void	DecayState();	// #CAT_TrialInit decay the state in between trials (params in LayerSpec)

  virtual void 	Trial_Init();	// #CAT_TrialInit initialize at start of trial (SetCurLrate, set phase_max, Decay state)

  virtual void	Trial_UpdatePhase(); // #CAT_TrialInit update phase based on phase_no -- return false if no more phases need to be run

  virtual void	EncodeState();
  // #CAT_TrialFinal encode final state information at end of trial for time-based learning across trials
  virtual void	Compute_SelfReg_Trial();
  // #CAT_TrialFinal update self-regulation (accommodation, hysteresis) at end of trial

  virtual void	Compute_dWt_FirstPlus();
  // #CAT_TrialFinal compute weight change after first plus phase has been encountered: standard layers do a weight change here, except under CtLeabra_X/CAL
  virtual void	Compute_dWt_SecondPlus();
  // #CAT_TrialFinal compute weight change after second plus phase has been encountered: standard layers do NOT do a weight change here -- only selected special ones
  virtual void	Compute_dWt_Nothing();
  // #CAT_TrialFinal compute weight change after final nothing phase: standard layers do a weight change here under both learning rules

  virtual void	Compute_ExtRew();
  // #CAT_Statistic compute external reward information: Must be called in plus phase (phase_no == 1)
  virtual void	Compute_NormErr();
  // #CAT_Statistic compute normalized binary error: called in TrialStats
  virtual void	Compute_MinusCycles();
  // #CAT_Statistic compute minus-phase cycles (and increment epoch sums) -- at the end of the minus phase (of course)
  override void	Compute_TrialStats();
  // #CAT_Statistic compute trial-level statistics, including SSE and minus cycles -- to be called at end of minus phase
  virtual bool	Compute_TrialStats_Test();
  // #CAT_Statistic determine whether it is time to run trial stats -- typically the minus phase but it depends on network phase_order settings etc
  virtual void	Trial_Final();
  // #CAT_TrialFinal do final processing after trial (Compute_dWt, EncodeState)

  virtual void	Compute_AbsRelNetin();
  // #CAT_Statistic compute the absolute layer-level and relative netinput from different projections into layers in network
  virtual void	Compute_AvgAbsRelNetin();
  // #CAT_Statistic compute time-average absolute layer-level and relative netinput from different projections into layers in network (e.g. over epoch timescale)
  virtual void	Compute_TrgRelNetin();
  // #MENU #MENU_SEP_BEFORE #CONFIRM #CAT_Learning compute target rel netin based on projection direction information plus the adapt_rel_net values in the conspec
  virtual void	Compute_AdaptRelNetin();
  // #CAT_Learning adapt the relative input values by changing the conspec wt_scale.rel parameter; See Compute_AdaptAbsNetin for adaptation of wt_scale.abs parameters to achieve good netinput values overall
  virtual void	Compute_AdaptAbsNetin();
  // #CAT_Learning adapt the absolute net input values by changing the conspec wt_scale.abs parameter

  virtual void	Compute_AvgCycles();
  // #CAT_Statistic compute average cycles (at an epoch-level timescale)
  virtual void	Compute_AvgExtRew();
  // #CAT_Statistic compute average external reward information (at an epoch-level timescale)
  virtual void	Compute_AvgNormErr();
  // #CAT_Statistic compute average norm_err (at an epoch-level timescale)
  virtual void	Compute_AvgSendPct();
  // #CAT_Statistic compute average sending pct (at an epoch-level timescale)
  override void	Compute_EpochStats();
  // #CAT_Statistic compute epoch-level statistics, including SSE, AvgExtRew and AvgCycles
  override void	SetProjectionDefaultTypes(Projection* prjn);

  virtual void	GraphInhibMod(bool flip_sign = true, DataTable* graph_data = NULL);
  // #BUTTON #NULL_OK graph the overall inhibitory modulation curve, including sinusoidal and final -- if flip_sign is true, then sign is reversed so that graph looks like the activation profile instead of the inhibition profile

  TA_SIMPLE_BASEFUNS(LeabraNetwork);
protected:
  int	prv_learn_rule;		// previous learning rule for triggering updates
  void 	UpdateAfterEdit_impl();
private:
  void	Initialize();
  void 	Destroy()		{}
};

class LEABRA_API LeabraProject : public ProjectBase {
  // #STEM_BASE ##CAT_Leabra project for Leabra models
INHERITED(ProjectBase)
public:

  TA_BASEFUNS_NOCOPY(LeabraProject);
private:
  void	Initialize();
  void 	Destroy()		{}
};

///////////////////////////////////////////////////////////////////////////////
//	Inlines	

//////////////////////////
//      Netin

float LeabraConSpec::C_Compute_Netin(LeabraCon* cn, Unit*, Unit* su) {
  return cn->wt * su->act;
}
float LeabraConSpec::Compute_Netin(RecvCons* cg, Unit* ru) {
  float rval=0.0f;
  CON_GROUP_LOOP(cg, rval += C_Compute_Netin((LeabraCon*)cg->Cn(i), ru, cg->Un(i)));
  return ((LeabraRecvCons*)cg)->scale_eff * rval;
}

void LeabraConSpec::C_Send_Inhib(LeabraSendCons*, LeabraCon* cn, LeabraUnit* ru, float su_act_eff) {
  ru->gc.i += su_act_eff * cn->wt;
}

void LeabraConSpec::C_Send_Netin(LeabraSendCons*, LeabraCon* cn, Unit* ru, float su_act_eff) {
  ru->net += su_act_eff * cn->wt;
}
void LeabraConSpec::Send_Netin(SendCons* cg, float su_act) {
  // apply scale based only on first unit in con group: saves lots of redundant mulitplies!
  // LeabraUnitSpec::CheckConfig checks that this is ok.
  Unit* ru = cg->Un(0);
  float su_act_eff = ((LeabraRecvCons*)ru->recv.FastEl(cg->recv_idx))->scale_eff * su_act;
  if(inhib) {
    CON_GROUP_LOOP(cg, C_Send_Inhib((LeabraSendCons*)cg, (LeabraCon*)cg->Cn(i), (LeabraUnit*)cg->Un(i), su_act_eff));
  }
  else {
    CON_GROUP_LOOP(cg, C_Send_Netin((LeabraSendCons*)cg, (LeabraCon*)cg->Cn(i), cg->Un(i), su_act_eff));
  }
}

///////////////////

void LeabraConSpec::C_Send_InhibDelta(LeabraSendCons*, LeabraCon* cn, LeabraUnit* ru, float su_act_delta_eff) {
  ru->g_i_delta += su_act_delta_eff * cn->wt;
}

void LeabraConSpec::C_Send_NetinDelta(LeabraSendCons*, LeabraCon* cn, LeabraUnit* ru, float su_act_delta_eff) {
  ru->net_delta += su_act_delta_eff * cn->wt;
}

void LeabraConSpec::Send_NetinDelta(LeabraSendCons* cg, float su_act_delta) {
  Unit* ru = cg->Un(0);
  float su_act_delta_eff = ((LeabraRecvCons*)ru->recv.FastEl(cg->recv_idx))->scale_eff * su_act_delta;
  if(inhib) {
    CON_GROUP_LOOP(cg, C_Send_InhibDelta(cg, (LeabraCon*)cg->Cn(i), (LeabraUnit*)cg->Un(i), su_act_delta_eff));
  }
  else {
    CON_GROUP_LOOP(cg, C_Send_NetinDelta(cg, (LeabraCon*)cg->Cn(i), (LeabraUnit*)cg->Un(i), su_act_delta_eff));
  }
}

///////////////////

void LeabraConSpec::C_Send_ClampNet(LeabraSendCons*, LeabraCon* cn, LeabraUnit* ru, float su_act_eff) {
  ru->clmp_net += su_act_eff * cn->wt;
}
void LeabraConSpec::Send_ClampNet(LeabraSendCons* cg, float su_act) {
  Unit* ru = cg->Un(0);
  float su_act_eff = ((LeabraRecvCons*)ru->recv.FastEl(cg->recv_idx))->scale_eff * su_act;
  CON_GROUP_LOOP(cg, C_Send_ClampNet(cg, (LeabraCon*)cg->Cn(i), (LeabraUnit*)cg->Un(i), su_act_eff));
}

////////////////////////////////////////////////////
//     Computing dWt: LeabraCHL

inline void LeabraConSpec::Compute_SAvgCor(LeabraRecvCons* cg, LeabraUnit*) {
  LeabraLayer* fm = (LeabraLayer*)cg->prjn->from.ptr();
  float savg = .5f + savg_cor.cor * (fm->kwta.pct - .5f);
  savg = MAX(savg_cor.thresh, savg); // keep this computed value within bounds
  cg->savg_cor = .5f / savg;
}

inline float LeabraConSpec::C_Compute_Hebb(LeabraCon* cn, LeabraRecvCons* cg,
					   float lin_wt, float ru_act, float su_act)
{
  return ru_act * (su_act * (cg->savg_cor - lin_wt) - (1.0f - su_act) * lin_wt);
}

// generec error term with sigmoid activation function, and soft bounding
inline float LeabraConSpec::C_Compute_Err_LeabraCHL(LeabraCon* cn, float lin_wt,
						    float ru_act_p, float ru_act_m,
						    float su_act_p, float su_act_m) {
  float err = (ru_act_p * su_act_p) - (ru_act_m * su_act_m);
  if(lmix.err_sb) {
    if(err > 0.0f)	err *= (1.0f - lin_wt);
    else		err *= lin_wt;
  }
  return err;
}

// combine hebbian and error-driven
inline void LeabraConSpec::C_Compute_dWt(LeabraCon* cn, LeabraUnit*,
					 float heb, float err) {
  float dwt = lmix.err * err + lmix.hebb * heb;
  cn->dwt += cur_lrate * dwt;
}

// combine hebbian and error-driven
inline void LeabraConSpec::C_Compute_dWt_NoHebb(LeabraCon* cn, LeabraUnit*,
						float err) {
  cn->dwt += cur_lrate * err;
}

inline void LeabraConSpec::Compute_dWt_LeabraCHL(LeabraRecvCons* cg, LeabraUnit* ru) {
  Compute_SAvgCor(cg, ru);
  if(((LeabraLayer*)cg->prjn->from.ptr())->acts_p.avg >= savg_cor.thresh) {
    for(int i=0; i<cg->cons.size; i++) {
      LeabraUnit* su = (LeabraUnit*)cg->Un(i);
      LeabraCon* cn = (LeabraCon*)cg->Cn(i);
      if(!(su->in_subgp &&
	   (((LeabraUnit_Group*)su->owner)->acts_p.avg < savg_cor.thresh))) {
	float lin_wt = GetLinFmWt(cn->wt);
	C_Compute_dWt(cn, ru, 
		      C_Compute_Hebb(cn, cg, lin_wt, ru->act_p, su->act_p),
		      C_Compute_Err_LeabraCHL(cn, lin_wt, ru->act_p, ru->act_m,
					      su->act_p, su->act_m));  
      }
    }
  }
}

/////////////////////////////////////
//	Compute_Weights_LeabraCHL

inline void LeabraConSpec::C_Compute_Weights_LeabraCHL(LeabraCon* cn)
{
  if(cn->dwt != 0.0f) {
    cn->wt = GetWtFmLin(GetLinFmWt(cn->wt) + cn->dwt);
  }
  cn->pdw = cn->dwt;
  cn->dwt = 0.0f;
}

inline void LeabraConSpec::Compute_Weights_LeabraCHL(LeabraRecvCons* cg, LeabraUnit* ru) {
  CON_GROUP_LOOP(cg, C_Compute_Weights_LeabraCHL((LeabraCon*)cg->Cn(i)));
  //  ApplyLimits(cg, ru); limits are automatically enforced anyway
}

//////////////////////////////////////////////////////////////////////////////////
//     CtLeabra XCAL SRAvg stuff

inline void LeabraConSpec::C_Compute_SRAvg_m(LeabraCon* cn, float ru_act, float su_act) {
  cn->sravg_m += ru_act * su_act;
}

inline void LeabraConSpec::C_Compute_SRAvg_ms(LeabraCon* cn, float ru_act, float su_act) {
  float sr = ru_act * su_act;
  cn->sravg_m += sr;
  cn->sravg_s += sr;
}

inline void LeabraConSpec::Compute_SRAvg(LeabraRecvCons* cg, LeabraUnit* ru, bool do_s) {
  if(do_s) {
    CON_GROUP_LOOP(cg, C_Compute_SRAvg_ms((LeabraCon*)cg->Cn(i), ru->act_nd,
					  ((LeabraUnit*)cg->Un(i))->act_nd));
  }
  else {
    CON_GROUP_LOOP(cg, C_Compute_SRAvg_m((LeabraCon*)cg->Cn(i), ru->act_nd, 
					 ((LeabraUnit*)cg->Un(i))->act_nd));
  }
}

inline void LeabraConSpec::C_Init_SRAvg(LeabraCon* cn) {
  cn->sravg_s = 0.0f;
  cn->sravg_m = 0.0f;
}

inline void LeabraConSpec::Init_SRAvg(LeabraRecvCons* cg, LeabraUnit* ru) {
  if(learn_rule == CTLEABRA_CAL || xcal.avg_updt == XCalLearnSpec::TRIAL) {
    CON_GROUP_LOOP(cg, C_Init_SRAvg((LeabraCon*)cg->Cn(i)));
  }
}

//////////////////////////////////////////////////////////////////////////////////
//     Computing dWt: CtLeabra_XCAL

inline void LeabraConSpec::
C_Compute_dWt_CtLeabraXCAL_avgprod(LeabraCon* cn, LeabraCon* rbias, LeabraCon* sbias,
				   float ru_ravg_l, float sravg_s_nrm, float sravg_m_nrm) {
  float srm = (sravg_m_nrm * rbias->sravg_m) * (sravg_m_nrm * sbias->sravg_m);
  float lrn = xcal.lrn_s_mix * (sravg_s_nrm * rbias->sravg_s) * (sravg_s_nrm * sbias->sravg_s)
    + xcal.lrn_m_mix * srm;
  float thr_p = MAX(srm, ru_ravg_l);
  cn->dwt += cur_lrate * xcal.dWtFun(lrn, thr_p);
}

inline void LeabraConSpec::C_Compute_dWt_CtLeabraXCAL_cont(LeabraCon* cn, float ru_ravg_l) {
  // appropriate scaling factors are already applied to ru_avg_l
  float lrn = xcal.lrn_s_mix * cn->sravg_s + xcal.lrn_m_mix * cn->sravg_m;
  float thr_p = MAX(cn->sravg_m, ru_ravg_l);
  cn->dwt += cur_lrate * xcal.dWtFun(lrn, thr_p);
}

inline void LeabraConSpec::Compute_dWt_CtLeabraXCAL(LeabraRecvCons* cg, LeabraUnit* ru) {
  // note: not doing all the checks for layers/groups inactive in plus phase: not needed since no hebb stuff
  LeabraLayer* slay = (LeabraLayer*)cg->prjn->from.ptr();
  float slay_avg_act;
  if(xcal.l_norm == XCalLearnSpec::KWTA_PCT)
    slay_avg_act = slay->kwta.pct;
  else if(xcal.l_norm == XCalLearnSpec::RAVG_L_AVG)
    slay_avg_act = slay->ravg_l_avg;
  else // NO_NORM
    slay_avg_act = 1.0f;
  float ru_avg_l = xcal.l_gain * slay_avg_act * ru->ravg_l;

  if(xcal.avg_updt == XCalLearnSpec::TRIAL) {
    LeabraLayer* rlay = (LeabraLayer*)cg->prjn->layer;
    LeabraCon* rbias = (LeabraCon*)ru->bias.Cn(0);

    float sravg_s_nrm = rlay->sravg_s_nrm;
    float sravg_m_nrm = rlay->sravg_m_nrm;
    if(xcal.lrn_var == XCalLearnSpec::AVG_PROD) {
      CON_GROUP_LOOP(cg,
	C_Compute_dWt_CtLeabraXCAL_avgprod((LeabraCon*)cg->Cn(i), rbias,
				   (LeabraCon*)((LeabraUnit*)cg->Un(i))->bias.Cn(0),
				   ru_avg_l, sravg_s_nrm, sravg_m_nrm));
    }
    else if(xcal.lrn_var == XCalLearnSpec::CAL) {
      CON_GROUP_LOOP(cg,
		     C_Compute_dWt_CtLeabraCAL((LeabraCon*)cg->Cn(i),
					       rlay->sravg_s_nrm, rlay->sravg_m_nrm));
    }
  }
  else {
    // todo: this actually requires all the timing stuff too..
    CON_GROUP_LOOP(cg,
		   C_Compute_dWt_CtLeabraXCAL_cont((LeabraCon*)cg->Cn(i), ru_avg_l));
  }
}

/////////////////////////////////////
//	Compute_Weights_CtLeabraXCAL

inline void LeabraConSpec::C_Compute_dWt_Rnd_XCAL(LeabraCon* cn, float rnd_var) {
  cn->dwt += cur_lrate * rnd_var * Random::ZeroOne();
}

inline void LeabraConSpec::Compute_dWt_Rnd_XCAL(LeabraRecvCons* cg, LeabraUnit* ru,
						float rnd_var) {
  CON_GROUP_LOOP(cg, C_Compute_dWt_Rnd_XCAL((LeabraCon*)cg->Cn(i), rnd_var));
}

inline void LeabraConSpec::C_Compute_Weights_CtLeabraXCAL(LeabraCon* cn)
{
  // always do soft bounding, at this point (post agg across processors, etc)
  if(cn->dwt > 0.0f)	cn->dwt *= (1.0f - cn->wt);
  else			cn->dwt *= cn->wt;

  cn->wt += cn->dwt;		// weights always linear
  // optimize: don't bother with this if always doing soft bounding above
  // wt_limits.ApplyMinLimit(cn->wt); wt_limits.ApplyMaxLimit(cn->wt);
  cn->pdw = cn->dwt;
  cn->dwt = 0.0f;
}

inline void LeabraConSpec::Compute_Weights_CtLeabraXCAL(LeabraRecvCons* cg, LeabraUnit* ru) {
  if(ru->ravg_l < xcal.rnd_min_avg) {
    Compute_dWt_Rnd_XCAL(cg, ru, xcal.rnd_var);
  }
  CON_GROUP_LOOP(cg, C_Compute_Weights_CtLeabraXCAL((LeabraCon*)cg->Cn(i)));
  //  ApplyLimits(cg, ru); limits are automatically enforced anyway
}


//////////////////////////////////////////////////////////////////////////////////
//     Computing dWt: CtLeabra_CAL

inline void LeabraConSpec::C_Compute_dWt_CtLeabraCAL(LeabraCon* cn,
						     float sravg_s_nrm, float sravg_m_nrm) {
  cn->dwt += cur_lrate * (sravg_s_nrm * cn->sravg_s - sravg_m_nrm * cn->sravg_m);
}

inline void LeabraConSpec::Compute_dWt_CtLeabraCAL(LeabraRecvCons* cg, LeabraUnit* ru) {
  // note: not doing all the checks for layers/groups inactive in plus phase: not needed since no hebb stuff
  LeabraLayer* rlay = (LeabraLayer*)cg->prjn->layer;
  CON_GROUP_LOOP(cg,
		 C_Compute_dWt_CtLeabraCAL((LeabraCon*)cg->Cn(i),
					   rlay->sravg_s_nrm, rlay->sravg_m_nrm));
}

/////////////////////////////////////
//	Compute_Weights_CtLeabraCAL

inline void LeabraConSpec::C_Compute_Weights_CtLeabraCAL(LeabraCon* cn)
{
  // always do soft bounding, at this point (post agg across processors, etc)
  if(cn->dwt > 0.0f)	cn->dwt *= (1.0f - cn->wt);
  else			cn->dwt *= cn->wt;

  cn->wt += cn->dwt;		// weights always linear
  // optimize: don't bother with this if always doing soft bounding above
  // wt_limits.ApplyMinLimit(cn->wt); wt_limits.ApplyMaxLimit(cn->wt);
  cn->pdw = cn->dwt;
  cn->dwt = 0.0f;
}

inline void LeabraConSpec::Compute_Weights_CtLeabraCAL(LeabraRecvCons* cg, LeabraUnit* ru) {
  CON_GROUP_LOOP(cg, C_Compute_Weights_CtLeabraCAL((LeabraCon*)cg->Cn(i)));
  //  ApplyLimits(cg, ru); limits are automatically enforced anyway
}

//////////////////////////////////////////////////////////////////////
//	Bias Weights: threshold dwt versions in LeabraBiasSpec

inline void LeabraConSpec::B_Compute_dWt_LeabraCHL(LeabraCon* cn, LeabraUnit* ru) {
  float err = ru->act_p - ru->act_m;
  cn->dwt += cur_lrate * err;
}
  
// default is not to do anything tricky with the bias weights
inline void LeabraConSpec::B_Compute_Weights(LeabraCon* cn, LeabraUnit* ru) {
  cn->pdw = cn->dwt;
  cn->wt += cn->dwt;
  cn->dwt = 0.0f;
  C_ApplyLimits(cn, ru, NULL);
}

inline void LeabraConSpec::B_Compute_SRAvg(LeabraCon* cn, LeabraUnit* ru, bool do_s) {
  if(learn_rule == CTLEABRA_CAL || xcal.use_nd) {
    if(learn_rule == CTLEABRA_CAL || xcal.avg_updt == XCalLearnSpec::TRIAL) {
      cn->sravg_m += ru->act_nd;
      if(do_s)
	cn->sravg_s += ru->act_nd;
    }
    else if(xcal.avg_updt == XCalLearnSpec::CONT) {
      cn->sravg_s += xcal.s_dt * (ru->act_nd - cn->sravg_s);
      cn->sravg_m += xcal.m_dt * (ru->act_nd - cn->sravg_m);
      ru->ravg_l += xcal.l_dt * (ru->act_nd - ru->ravg_l);
      // note: updating unit-level ravg_l variable here..
    }
    else if(xcal.avg_updt == XCalLearnSpec::CONT_CASC) {
      cn->sravg_s += xcal.s_dt * (ru->act_nd - cn->sravg_s);
      cn->sravg_m += xcal.m_dt * (cn->sravg_s - cn->sravg_m);
      ru->ravg_l += xcal.l_dt * (cn->sravg_m - ru->ravg_l);
      // note: updating unit-level ravg_l variable here..
    }
  }
  else {
    float ru_act = ru->act;
    LeabraUnitSpec* rus = (LeabraUnitSpec*)ru->GetUnitSpec();
    if(rus->act_fun == LeabraUnitSpec::SPIKE)
      ru_act *= rus->spike.eq_gain;
    if(xcal.avg_updt == XCalLearnSpec::TRIAL) {
      cn->sravg_m += ru_act;
      if(do_s)
	cn->sravg_s += ru_act;
    }
    else if(xcal.avg_updt == XCalLearnSpec::CONT) {
      cn->sravg_s += xcal.s_dt * (ru_act - cn->sravg_s);
      cn->sravg_m += xcal.m_dt * (ru_act - cn->sravg_m);
      ru->ravg_l += xcal.l_dt * (ru_act - ru->ravg_l);
      // note: updating unit-level ravg_l variable here..
    }
    else if(xcal.avg_updt == XCalLearnSpec::CONT_CASC) {
      cn->sravg_s += xcal.s_dt * (ru_act - cn->sravg_s);
      cn->sravg_m += xcal.m_dt * (cn->sravg_s - cn->sravg_m);
      ru->ravg_l += xcal.l_dt * (cn->sravg_m - ru->ravg_l);
      // note: updating unit-level ravg_l variable here..
    }
  }
}

inline void LeabraConSpec::B_Init_SRAvg(LeabraCon* cn, LeabraUnit* ru,
						      LeabraLayer* rlay) {
  if(learn_rule == CTLEABRA_CAL || xcal.avg_updt == XCalLearnSpec::TRIAL) {
    // note: updating unit-level ravg_l variable here..
    ru->ravg_l += xcal.l_dt * (rlay->sravg_m_nrm * cn->sravg_m - ru->ravg_l);
    cn->sravg_s = 0.0f;
    cn->sravg_m = 0.0f;
  }
}

inline void LeabraConSpec::B_Init_RAvg_l(LeabraCon* cn, LeabraUnit* ru) {
  ru->ravg_l = xcal.avg_init;
}

inline void LeabraConSpec::B_Compute_dWt_CtLeabraXCAL(LeabraCon* cn, LeabraUnit* ru,
						      LeabraLayer* rlay) {
  float dw;
  if(xcal.avg_updt == XCalLearnSpec::TRIAL) {
    if(xcal.lrn_var == XCalLearnSpec::CAL) {
      dw = (rlay->sravg_s_nrm * cn->sravg_s - rlay->sravg_m_nrm * cn->sravg_m);
    }
    else {
      float srm = rlay->sravg_m_nrm * cn->sravg_m;
      float lrn = xcal.lrn_s_mix * rlay->sravg_s_nrm * cn->sravg_s + xcal.lrn_m_mix * srm;
      float thr_p = MAX(srm, ru->ravg_l);
      // note: not using l_gain here -- defaults to 1
      dw = xcal.dWtFun(lrn, thr_p);
    }
  }
  else {
    float lrn = xcal.lrn_s_mix * cn->sravg_s + xcal.lrn_m_mix * cn->sravg_m;
    float thr_p = MAX(cn->sravg_m, ru->ravg_l);
    dw = xcal.dWtFun(lrn, thr_p);
  }
  cn->dwt += cur_lrate * dw;
}

inline void LeabraConSpec::B_Compute_dWt_CtLeabraCAL(LeabraCon* cn, LeabraUnit* ru,
						     LeabraLayer* rlay) {
  float dw = (rlay->sravg_s_nrm * cn->sravg_s - rlay->sravg_m_nrm * cn->sravg_m);
  cn->dwt += cur_lrate * dw;
}


/////////////////////////////////////////
//	LeabraBiasSpec real ones

// todo: note that this is very wasteful replication of code for just one last line..

inline void LeabraBiasSpec::B_Compute_dWt_LeabraCHL(LeabraCon* cn, LeabraUnit* ru) {
  float err = ru->act_p - ru->act_m;
  if(fabsf(err) >= dwt_thresh)
    cn->dwt += cur_lrate * err;
}

inline void LeabraBiasSpec::B_Compute_dWt_CtLeabraXCAL(LeabraCon* cn, LeabraUnit* ru,
						      LeabraLayer* rlay) {
  float dw;
  if(xcal.avg_updt == XCalLearnSpec::TRIAL) {
    if(xcal.lrn_var == XCalLearnSpec::CAL) {
      dw = (rlay->sravg_s_nrm * cn->sravg_s - rlay->sravg_m_nrm * cn->sravg_m);
    }
    else {
      float srm = rlay->sravg_m_nrm * cn->sravg_m;
      float lrn = xcal.lrn_s_mix * rlay->sravg_s_nrm * cn->sravg_s + xcal.lrn_m_mix * srm;
      float thr_p = MAX(srm, ru->ravg_l);
      // note: not using l_gain here -- defaults to 1
      dw = xcal.dWtFun(lrn, thr_p);
    }
  }
  else {
    float lrn = xcal.lrn_s_mix * cn->sravg_s + xcal.lrn_m_mix * cn->sravg_m;
    float thr_p = MAX(cn->sravg_m, ru->ravg_l);
    dw = xcal.dWtFun(lrn, thr_p);
  }
  if(fabsf(dw) >= dwt_thresh)
    cn->dwt += cur_lrate * dw;
}

inline void LeabraBiasSpec::B_Compute_dWt_CtLeabraCAL(LeabraCon* cn, LeabraUnit* ru,
						     LeabraLayer* rlay) {
  float dw = (rlay->sravg_s_nrm * cn->sravg_s - rlay->sravg_m_nrm * cn->sravg_m);
  if(fabsf(dw) >= dwt_thresh)
    cn->dwt += cur_lrate * dw;
}


//////////////////////////
//	Unit NetAvg   	//
//////////////////////////

void LeabraUnitSpec::Compute_NetinAvg(LeabraUnit* u, LeabraLayer* lay, LeabraInhib* thr, LeabraNetwork* net) {
  if(net->send_delta) {
    u->net_raw += u->net_delta;
    u->net += u->clmp_net + u->net_raw;
  }
  if(act_fun == SPIKE) {
    Compute_Netin_Spike(u,lay,thr,net);
    return;			// does everything
  }
  u->net = u->prv_net + dt.net * (u->net - u->prv_net);
  u->prv_net = u->net;
  if((noise_type == NETIN_NOISE) && (noise.type != Random::NONE) && (net->cycle >= 0)) {
    u->net += noise_sched.GetVal(net->cycle) * noise.Gen();
  }
  u->i_thr = Compute_IThresh(u, lay, net);
}

void LeabraUnitSpec::Compute_ApplyInhib(LeabraUnit* u, LeabraLayer*, LeabraInhib*, LeabraNetwork*, float inhib_val) {
  if(inhib_val <= 0.0f) return;	// nothing to apply
  // if you have a computed inhibition value, apply it full force, overwriting anything else
  u->g_i_raw = inhib_val;
  u->gc.i = inhib_val;
  u->prv_g_i = inhib_val;
  u->g_i_delta = 0.0f;
}

// todo: need a mech for inhib spiking
void LeabraUnitSpec::Compute_InhibAvg(LeabraUnit* u, LeabraLayer*, LeabraInhib* thr, LeabraNetwork* net) {
  if(net->send_delta) {
    u->g_i_raw += u->g_i_delta;
    u->gc.i = u->g_i_raw;
  }
  u->gc.i = u->prv_g_i + dt.net * (u->gc.i - u->prv_g_i);
  u->prv_g_i = u->gc.i;
}

//////////////////////////////////
//	Leabra Wizard		//
//////////////////////////////////

class LEABRA_API LeabraWizard : public Wizard {
  // #STEM_BASE ##CAT_Leabra Leabra-specific wizard for automating construction of simulation objects
INHERITED(Wizard)
public:
  override bool StdNetwork(TypeDef* net_type = &TA_LeabraNetwork, Network* net = NULL);

  virtual bool	StdLayerSpecs(LeabraNetwork* net);
  // #MENU_BUTTON #MENU_ON_Network #MENU_SEP_BEFORE make standard layer specs for a basic Leabra network (KWTA_AVG 25% for hiddens, KWTA PAT_K for input/output)

  virtual bool	SRNContext(LeabraNetwork* net);
  // #MENU_BUTTON configure a simple-recurrent-network context layer in the network

  virtual bool	UnitInhib(LeabraNetwork* net, int n_inhib_units=10);
  // #MENU_BUTTON configures unit-based inhibition for all layers in the network

  virtual bool 	TD(LeabraNetwork* net, bool bio_labels = false, bool td_mod_all = false);
  // #MENU_BUTTON #MENU_SEP_BEFORE configure standard TD reinforcement learning layers; bio_labels = use biologically-based labels for layers, else functional; td_mod_all = have td value modulate all the regular units in the network

  virtual bool 	PVLV(LeabraNetwork* net, bool da_mod_all = false);
  // #MENU_BUTTON #MENU_SEP_BEFORE configure PVLV (pavlovian primary value and learned value) learning layers in a network -- provides a simulated dopamine signal that reflects unexpected primary rewards (PV = primary value system) and unexpected learned reward assocations (conditioned stimuli; LV = learned value = system); da_mod_all = have da value modulate all the regular units in the network

  virtual bool PVLV_ConnectLayer(LeabraNetwork* net, LeabraLayer* sending_layer,
				 bool disconnect = false);
  // #MENU_BUTTON #NO_SCOPE_1 make (or break if disconnect = true) connections between given sending_layer in given network and the learning PVLV layers (PVr, PVi, LVe, LVi, NV), each of which should typically receive from the same sending layers

  virtual bool 	PBWM(LeabraNetwork* net, bool da_mod_all = false,
		     int n_stripes=4, bool out_gate=false,
		     bool no_lrn_pfc=false);
  // #MENU_BUTTON #MENU_SEP_BEFORE configure all the layers and specs for the prefrontal-cortex basal ganglia working memory system (PBWM) -- does a PVLV configuration first (see PVLV for details) and then adds a basal ganglia gating system that is trained by PVLV dopamine signals.  The gating system determines when the PFC working memory representations are updated;  da_mod_all = have da value modulate all the regular units in the network; out_gate = each PFC layer has separate output gated layer and corresponding matrix output gates; nolrn_pfc = pfc does not learn -- just copies input acts directly (useful for demonstration but not as realistic or powerful)

  virtual bool SetPFCStripes(LeabraNetwork* net, int n_stripes, int n_units=-1);
  // #MENU_BUTTON set number of "stripes" (unit groups) throughout the entire set of pfc/bg layers (n_units = -1 = use current # of units)

  override bool	StdProgs();
  override bool	TestProgs(Program* call_test_from, bool call_in_loop=true, int call_modulus=1);

  TA_BASEFUNS_NOCOPY(LeabraWizard);
private:
  void 	Initialize();
  void 	Destroy()	{ };
};


class LEABRA_API LeabraEngineInst: public NetEngineInst {
INHERITED(NetEngineInst)
public:
  UnitPtrList		send_units; // list of only the sending units for a cycle
  float_Matrix		excit; // [task][un_idx] -- scratchpad excit (net)
  float_Matrix		inhib; // [task][un_idx] -- scratchpad inhit (g.i)
  
  inline LeabraUnit*	unit(int i) const {return (LeabraUnit*)units[i];}
  inline LeabraLayer*	layer(int i) const {return (LeabraLayer*)layers.FastEl(i);}
  inline LeabraNetwork* net() const {return (LeabraNetwork*)owner;} 
  
  virtual bool		OnSend_Netin() {return false;}
  virtual bool		OnSend_NetinDelta() {return false;}
  
  SIMPLE_LINKS(LeabraEngineInst);
  TA_BASEFUNS_NOCOPY(LeabraEngineInst);
protected:
  int			n_tasks; // how many tasks actually being used this DoProc
  void			AssertScratchDims(int tasks, int units); // asserts mod4
  void			RollupWritebackScratch_Netin(); 
    // rolls up, so sum is in [0][], and writes back, for Netin algo
  void			RollupWritebackScratch_NetinDelta(); 
    // rolls up, so sum is in [0][], and writes back, for NetinDelta algo
private:
  void	Initialize();
  void	Destroy();
};

class LEABRA_API LeabraEngine: public NetEngine {
// abstract definition of LeabraEngine
INHERITED(NetEngine)
public:
  
  TA_BASEFUNS_NOCOPY(LeabraEngine);
protected:
  taEngineInst*		NewEngineInst_impl() const;
private:
  void	Initialize();
  void	Destroy();
};


class LEABRA_API LeabraTask: public taTask {
// #VIRT_BASE #NO_INSTANCE one instance of a task for doing Leabra algorithm calculations -- concretely defines the default impl in absence of any other engine
INHERITED(taTask)
public:
  enum Proc { // the proc to execute
    P_Compute_Act,
    P_Compute_dWt,
    P_Compute_Weights,
    P_Send_Netin,
    P_Send_NetinDelta_flat,
    P_Send_NetinDelta_list,
  };
  
  inline LeabraEngineInst* inst() const {return (LeabraEngineInst*)m_inst;} 
  
  override void		run(); 
  
  TA_BASEFUNS_NOCOPY(LeabraTask);
protected:
  virtual void		DoCompute_Act() {}
  virtual void		DoCompute_dWt() {}
  virtual void		DoCompute_Weights() {}
  virtual void		DoSend_Netin() {}
  virtual void		DoSend_NetinDelta_flat() {}
  virtual void		DoSend_NetinDelta_list() {}
  
private:
  void	Initialize();
  void	Destroy();
};

#ifdef TA_USE_THREADS

class LeabraThreadEngineTask;

class LEABRA_API LeabraThreadEngine: public LeabraEngine {
// threaded LeabraEngine
INHERITED(LeabraEngine)
public:
  int			n_threads; // #MIN_1 #MAX_32 number of threads to use -- typically==number of cores
  bool			nibble; // #DEF_true set false to disable nibbling (for debugging threads)
  
  TA_BASEFUNS_NOCOPY(LeabraThreadEngine);
protected:
  taEngineInst* 	NewEngineInst_impl() const;
private:
  void	Initialize();
  void	Destroy();
};


class LEABRA_API LeabraThreadEngineInst: public LeabraEngineInst {
INHERITED(LeabraEngineInst)
friend class LeabraThreadEngineTask;
public:
  static const int	UNIT_CHUNK_SIZE = 64 / TA_POINTER_SIZE;
    // number of units to process by a thread in one chunk; MUST BE power of 2
  static const int	MAX_THREADS = 32; // arbitrary number -- bump when 64 core chips arrive ;)
#ifndef __MAKETA__
  taTaskThread*		threads[MAX_THREADS]; // one per task, except [0]
#endif  
// timers, for convenience
  TimeUsedHR		tm_tot; // total
  TimeUsedHR		tm_send_units;
  TimeUsedHR		tm_make_threads;
  TimeUsedHR		tm_run0; // only for t0, threads have their own
  TimeUsedHR		tm_nibble;
  TimeUsedHR		tm_sync;
  TimeUsedHR		tm_roll_write;
  
  bool			nibble; // #DEF_true set false to disable nibbling (for debugging threads)

  inline LeabraThreadEngineTask* task(int i) const {return (LeabraThreadEngineTask*)tasks[i];}
  inline LeabraThreadEngine* engine() const {return (LeabraThreadEngine*)m_engine.ptr();}
  override void		setTaskCount(int val);
  
  void 			DoUnitProc(int proc_id);
  void 			DoLayerProc(int proc_id);

  override bool		OnCompute_Act(); // Layer 
  override bool		OnCompute_dWt(); // Layer 
  override bool		OnCompute_Weights(); // Layer 
  override bool		OnSend_Netin();
  override bool		OnSend_NetinDelta(); //
  
  SIMPLE_LINKS(LeabraThreadEngineInst);
  TA_BASEFUNS_NOCOPY(LeabraThreadEngineInst); //
  
#ifndef __MAKETA__ 

//  data cols, set OnBuild if logging true
  DataCol*		col_n_units;
  DataCol*		col_n_tasks;
  DataCol*		col_tm_tot;
  DataCol*		col_tm_send_units;
  DataCol*		col_tm_make_threads;
  DataCol*		col_tm_release;
  DataCol*		col_tm_run;
  DataCol*		col_tm_nibble;
  DataCol*		col_tm_sync;
  DataCol*		col_tm_roll_write;
#endif 
protected:
  int			n_items; // this is the number of items that will be proc'ed in the algo
#ifdef DEBUG
  int			n_items_done; // sanity check to insure threading working right
#endif
  override void		OnBuild_impl(); // main build
  bool			OnSend_NetinDelta_flat(); // using all units
  bool			OnSend_NetinDelta_list(); // using a prebuilt list
  override void		WriteLogRecord_impl(); // only called if use_log
private:
  void	Initialize();
  void	Destroy();
};


class LEABRA_API LeabraThreadEngineTask: public LeabraTask {
// task
INHERITED(LeabraTask)
public:
  static int		g_l; // layers are done globally
#ifndef __MAKETA__ // stupid maketa chokes on 'inline static'
  inline static void 	StatC_Send_Inhib(LeabraCon* cn, float* ru_i, float su_act_eff) 
    {*ru_i += su_act_eff * cn->wt;} // #IGNORE
  inline static void 	StatC_Send_Excit(LeabraCon* cn, float* ru_e, float su_act_eff) 
    {*ru_e += su_act_eff * cn->wt;} // #IGNORE
#endif

// variables shared by any kind of task:
  bool			init_done; // lets us safely nibble from main task
  int			g_i; // index of unit, layer, etc. on which to start
  
// Unit routine variables:
  int			scr_units; // number of slots in scratch -- is mod4 value
  float*		excit; // our subportion of excit scratch
  float*		inhib; // our subportion of excit scratch
  
// Layer routine variables:

  inline LeabraThreadEngineInst* inst() const {return (LeabraThreadEngineInst*)m_inst;} 
  
  TA_BASEFUNS_NOCOPY(LeabraThreadEngineTask); //
protected:
// Unit routines:  
  override void		DoSend_Netin();
  void			InitScratch_Send_Netin();
  void 			DoSend_Netin_Gp(bool is_excit, SendCons* cg, float su_act);
  
  override void		DoSend_NetinDelta_flat();
  override void		DoSend_NetinDelta_list();
  void			InitScratch_Send_NetinDelta();
  void 			DoSend_NetinDelta_Gp(bool is_excit, SendCons* cg, float act_delta); //
  
// Layer routines:
  override void		DoCompute_Act(); //
  override void		DoCompute_dWt(); //
  override void		DoCompute_Weights(); //
private:
  void	Initialize();
  void	Destroy() {}
};


#endif // TA_USE_THREADS


#endif // leabra_h
