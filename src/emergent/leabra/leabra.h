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

#include "netstru_extra.h"
#include "emergent_project.h"

#include "fun_lookup.h"

#include "leabra_def.h"
#include "leabra_TA_type.h"

// these are needed for FLT_MAX
#include <math.h>
#include <limits.h>
#include <float.h>

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
//

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
  
// NOTE: following is actually not that useful in practice -- better to just add
// equivalent logic and _time or _rate values directly into spec of interest
// definitely good to show both reps!

class LEABRA_API DtSpec : public taBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Math time constant specification -- shows both multiplier and time constant (inverse) value 
INHERITED(taBase)
public:
  bool		set_time;	// if true, time constant is entered in terms of time, otherwise, in terms of rate
  float		rate;		// #CONDEDIT_OFF_set_time rate factor = 1/time -- used for multiplicative update equations
  float		time;		// #CONDEDIT_ON_set_time temporal duration for the time constant -- how many msec or sec long (typically to reach a 1/e level with exponential dynamics) = 1/rate

  TA_SIMPLE_BASEFUNS(DtSpec);
protected:
  void	UpdateAfterEdit_impl() { if(set_time) rate = 1.0f / time; else time = 1.0f / rate; }
private:
  void	Initialize()    { set_time = false; rate = 1.0f; time = 1.0f; }
  void	Destroy()	{ };
};

// NOTE: XCAL currently uses SR as its default lrule, which requires synapse-level sravg 
// variables in the connection..  The product of pre/post guys also works but not as well

class LeabraCon : public Connection {
  // #STEM_BASE ##CAT_Leabra Leabra connection
public:
  float		pdw;		// #VIEW_HOT #NO_SAVE previous delta-weight change -- useful for viewing because current weight change (dwt) is typically reset to 0 when views are updated
  float		sravg_s;	// #NO_SAVE short time-scale, most recent (plus phase) average of sender and receiver activation product over time
  float		sravg_m;	// #NO_SAVE medium time-scale, trial-level average of sender and receiver activation product over time
  
  LeabraCon() { pdw = 0.0f; sravg_s = sravg_m = 0.0f; }
};

class LEABRA_API WtScaleSpec : public taBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra weight scaling specification
INHERITED(taBase)
public:
  float		abs;		// #DEF_1 #MIN_0 absolute scaling (not subject to normalization: directly multiplies weight values)
  float		rel;		// [Default: 1] #MIN_0 relative scaling that shifts balance between different projections (subject to normalization across all other projections into unit)

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
  bool		init;		// use these scaling values to initialize the wt_scale parameters during InitWeights (if false, these values have no effect at all)
  float		abs;		// #CONDEDIT_ON_init #DEF_1 #MIN_0 absolute scaling (not subject to normalization: directly multiplies weight values)
  float		rel;		// #CONDEDIT_ON_init [Default: 1] #MIN_0 relative scaling that shifts balance between different projections (subject to normalization across all other projections into unit)

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
  float		gain;		// #DEF_1;6 #MIN_0 gain (contrast, sharpness) of the weight contrast function (1 = linear)
  float		off;		// #DEF_1;1.25 #MIN_0 offset of the function (1=centered at .5, >1=higher, <1=lower)

  static inline float	SigFun(float w, float gn, float of) {
    if(w <= 0.0f) return 0.0f;
    if(w >= 1.0f) return 1.0f;
    return (1.0f / (1.0f + powf((of * (1.0f - w)) / w, gn)));
  }
  // static version of function for implementing weight sigmodid

  static inline float	InvFun(float w, float gn, float of) {
    if(w <= 0.0f) return 0.0f;
    if(w >= 1.0f) return 1.0f;
    return 1.0f / (1.0f + powf((1.0f - w) / w, 1.0f / gn) / of);
  }
  // static version of function for implementing inverse of weight sigmoid

  inline float	SigFmLinWt(float lw) {
    return SigFun(lw, gain, off);
  }
  // get sigmoidal contrast-enhanced weight from linear weight
  
  inline float	LinFmSigWt(float sw) {
    return InvFun(sw, gain, off);
  }
  // get linear weight from sigmoidal contrast-enhanced weight

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
  float		hebb;		// [Default: .01] #MIN_0 amount of hebbian learning (should be relatively small, can be effective at .0001)
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
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS ##CAT_Leabra CtLeabra temporally eXtended Contrastive Attractor Learning (XCAL) specs
INHERITED(taOBase)
public:
  enum LearnVar {
    XCAL_SR,			// XCAL with synapse-level send-recv avg (sravg) for short and medium time scales (as in CAL), relative to medium and long term averages, passed through the XCAL dwt function (see conspec for option to plot this)
    XCAL_SRMAX,			// XCAL_SR with learn threshold as MAX of medium (error-driven) and long-term (BCM) averages -- eliminates mvl_mix parameter, and is more plausible overall
    XCAL_SEP,			// XCAL with product of *separate* sending and receiving unit averages (from bias weight) for short and medium time scale, relative to medium and long term averages, passed through the XCAL dwt function (see conspec for option to plot this) -- this typically works almost as well as _SR, and is faster
    CAL,			// XCAL with synapse-level sravg for short and medium time scales (as in CAL) -- svm term (err-driven) does NOT go through XCAL function (main diff from _SR version)
    CHL,			// XCAL with CHL as the svm (err-driven) term (not going through XCAL function) -- basically CHL with XCAL homeostasis mechanism (BCM-like rule)
  };

  LearnVar	lrn_var;	// #DEF_XCAL_SR learning rule variant -- non-XCAL options are primarily for testing and specialized applications -- bias weights always use CAL or CHL if CHL is selected
  float		mvl_mix;	// #CONDEDIT_OFF_lrn_var:XCAL_SRMAX #DEF_0.001:1.0 [0.002 std] #MIN_0 amount that medium (trial) versus long (epoch) time scale learning contributes -- this is the self-organizing BCM-like homeostatic component of learning -- remainder is svm: short (plus phase) versus medium (trial) time scale which reflects pure error-driven learning
  float		svm_mix;	// #READ_ONLY 1-mvl_mix -- how much the short (plus phase) versus medium (trial) time-scale factor contributes to learning -- this the pure error-driven learning component -- the rest (mvl_mix = 1-svm_mix) is medium (trial) versus long (epoch) time-scale
  float		s_mix;		// #DEF_0.7:0.95 [0.90 std] #MIN_0 how much the short (plus phase) versus medium (trial) time-scale factor contributes to the synaptic activation term for learning -- s_mix just makes sure that plus-phase states are sufficiently long/important (e.g., dopamine) to drive strong positive learning to these states -- if 0 then svm term is also negated -- but vals < 1 are needed to ensure that when unit is off in plus phase (short time scale) that enough medium-phase trace remains to drive appropriate learning
  float		m_mix;		// #READ_ONLY 1-s_mix -- amount that medium time scale value contributes to synaptic activation level: see s_mix for details
  float		l_dt;		// #DEF_0.0001:0.01 [0.005 std for TRIAL, .0002 for CONT] #MIN_0 time constant (rate) for updating the long time-scale ravg_l value -- note this is ONLY applicable on the unit bias con spec, where it updates the unit-level ravg_l variable!!
  float		l_gain;		// #DEF_1.5 #MIN_0 gain for long time-scale ravg term -- needed to put into same terms as the s*r avg values used in the s and m components of learning -- note this is ONLY applicable on the unit bias con spec, where it updates the unit-level l_thr variable!!
  float		ml_dt;		// #DEF_0.4 #MIN_0 time constant (rate) for updating the medium-to-long time-scale ravg_ml value, which integrates over recent history of medium (trial level) averages, used for rapid adaptation of LTP vs LTD learning threshold in XCAL, and for learning based on receiver average activations with a trace of prior activations -- note this is computed on the unit bias con spec, where it updates the unit-level ravg_ml variable
  float		d_rev;		// #DEF_0.1 #MIN_0 proportional point within LTD range where magnitude reverses to go back down to zero at zero sravg -- err-driven svm component does better with smaller values, and BCM-like mvl component does better with larger values -- 0.10 is a compromise
  float		d_gain;		// #DEF_1 #MIN_0 multiplier on LTD values relative to LTP values -- generally do not change from 1.0 default unless using only BCM-style learning

  float		d_rev_ratio;	// #HIDDEN #READ_ONLY (1-d_rev)/d_rev -- multiplication factor in learning rule

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

  inline float  SymSbFun(float wt_val) {
    return 2.0f * wt_val * (1.0f - wt_val);
  }
  // symmetric soft bounding function -- factor of 2 to equate with asymmetric sb for overall lrate at a weight value of .5 (= .5)

  SIMPLE_COPY(XCalLearnSpec);
  TA_BASEFUNS(XCalLearnSpec);
protected:
  void	UpdateAfterEdit_impl();
private:
  void	Initialize();
  void 	Destroy()	{ };
};

class LEABRA_API XCalContSpec : public taOBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS ##CAT_Leabra CtLeabra eXtended Contrastive Attractor Learning (XCAL) fully continuous time specs (CTLEABRA_XCAL_C)
INHERITED(taOBase)
public:
  float		s_dt;		// #DEF_0.2 #MIN_0 (only for XCAL_C) time constant (rate) for continuously updating the short time-scale sravg_s value
  float		s_time;		// #READ_ONLY #SHOW (only for XCAL_C) time constant (in cycles, 1/s_dt) for continuously updating the short time-scale sravg_s value
  float		m_dt;		// #DEF_0.1 #MIN_0 (only for XCAL_C) time constant (rate) for continuous updating the medium time-scale sravg_m value
  float		m_time;		// #READ_ONLY #SHOW (only for XCAL_C) time constant (in cycles, 1/m_dt) for continuously updating the medium time-scale sravg_m value
  // todo: need some params like this for continuous mode -- currently still use trial-wise hooks
//   float		lrn_thr;	// threshold on sravg_m value to initiate learning, in continous mode
//   int		lrn_delay;	// delay after lrn_thr threshold has been crossed after which learning occurs

  SIMPLE_COPY(XCalContSpec);
  TA_BASEFUNS(XCalContSpec);
protected:
  void	UpdateAfterEdit_impl();
private:
  void	Initialize();
  void 	Destroy()	{ };
};

class LEABRA_API XCalMiscSpec : public taOBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS ##CAT_Leabra CtLeabra eXtended Contrastive Attractor Learning (XCAL) extra misc specs (lower frequency usage)
INHERITED(taOBase)
public:
  float		ml_mix;		// #DEF_0 #MIN_0 how much the medium-to-long time scale average activations contribute to synaptic activation -- useful for capturing sequential dependencies between events, when these are present in the simulation, but not appropriate for random event sequences
  float		sm_mix;		// #READ_ONLY #DEF_1 #MIN_0 complement of ml_mix = 1-ml_mix -- how much the short & medium time scale average activations contribute to synaptic activation

  bool		use_sb;		// #DEF_true use soft weight bounding -- this should almost always be used except in special experimental circumstances
  bool		use_nd;		// #DEF_false use the act_nd variables (non-depressed) for computing sravg/ravg terms (else use raw act, which is raw spikes in spiking mode, and subject to depression if in place)

  float		avg_init;	// #DEF_0.15 #MIN_0 initial value for averages

  bool		do_init_sravg;	// #HIDDEN #READ_ONLY #NO_SAVE initialize sravg values at connection level -- this is a complicated test so it is precompiled.
  bool		do_comp_sravg;	// #HIDDEN #READ_ONLY #NO_SAVE compute sravg values at connection level -- this is a complicated test so it is precompiled.

  SIMPLE_COPY(XCalMiscSpec);
  TA_BASEFUNS(XCalMiscSpec);
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
  float		cor;		// #DEF_0.4:0.8 #MIN_0 #MAX_1 proportion of correction to apply (0=none, 1=all, .5=half, etc)
  float		thresh;		// #DEF_0.001 #MIN_0 threshold of sending average activation below which learning does not occur (prevents learning when there is no input)
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
  bool		on;		// whether to adapt relative netinput values for this connection (only applied if AdaptAbsNetin is called, after AbsRelNetin and AvgAbsRelNetin)
  float		trg_fm_input;	// #CONDEDIT_ON_on:true (typically 0.85) #MIN_0 #MAX_1 target relative netinput for fm_input projections (set by Compute_TrgRelNetin fun): all such projections should sum to this amount (divide equally among them) -- this plus fm_output and lateral should sum to 1. if other types are missing, this is increased in proportion
  float		trg_fm_output;	// #CONDEDIT_ON_on:true (typically 0.10) #MIN_0 #MAX_1 target relative netwinput for fm_output projections (set by Compute_TrgRelNetin fun): all such projections should sum to this amount (divide equally among them) -- this plus fm_input and lateral should sum to 1. if other types are missing, this is increased in proportion
  float		trg_lateral;	// #CONDEDIT_ON_on:true (typically 0.05) #MIN_0 #MAX_1 target relative netinput for lateral projections (set by Compute_TrgRelNetin fun): all such projections should sum to this amount (divide equally among them)  -- this plus fm_input and lateral should sum to 1.  if other types are missing, this is increased in proportion
  float		trg_sum;	// #READ_ONLY #SHOW sum of trg values -- should be 1!

  float		tol_lg;		// #CONDEDIT_ON_on:true #DEF_0.05 #MIN_0 #MAX_1 tolerance from target value (as a proportion of target value) on large numbers (>.25), within which parameters are not adapted
  float		tol_sm;		// #CONDEDIT_ON_on:true #DEF_0.2 #MIN_0 #MAX_1 tolerance from target value (as a proportion of target value) on small numbers (<.25), within which parameters are not adapted
  float		rel_lrate;	// #CONDEDIT_ON_on:true #DEF_0.2 #MIN_0 #MAX_1 adpatation 'learning' rate on wt_scale.rel parameter

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
    LEABRA_CHL,			// standard Leabra Contrastive Hebbian Learning rule with hebbian self-organizing factor: (s+r+) - (s-r-) + r+(s+ - w) -- s=sender,r=recv +=plus phase, -=minus phase, w= weight
    CTLEABRA_CAL,		// Continuous-Time Leabra Contrastive Attractor Learning rule: <sr>_s - <sr>_m -- s=sender, r=recv, <> = avg over short (plus phase) and medium (trial) time scales -- purely error-driven but inhibitory oscillations can drive self-organizing component
    CTLEABRA_XCAL,		// Continuous-Time Leabra temporally eXtended Contrastive Attractor Learning rule, trial-based version, which has two time scales of contrasts: short-vs-medium (svm) and medium-vs-long (mvl): (<sr>_s - <sr>_m) + (<sr>_m - <r>_l) -- s=sender, r=recv, <> = avg over short (plus phase), medium (trial), long (epoch) time scales.  svm is basically error-driven learning, and mvl is BCM-style self-organizing learning.
    CTLEABRA_XCAL_C,		// Continuous-Time Leabra temporally eXtended Contrastive Attractor Learning rule, fully continuous version, which has two time scales of contrasts: short-vs-medium (svm) and medium-vs-long (mvl): (<sr>_s - <sr>_m) + (<sr>_m - <r>_l) -- s=sender, r=recv, <> = avg over short (plus phase), medium (trial), long (epoch) time scales.  svm is basically error-driven learning, and mvl is BCM-style self-organizing learning.
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

  float		lrate;		// #CAT_Learning #DEF_0.01;0.02 #MIN_0 [0.01 for std Leabra, .02 for CtLeabra] learning rate -- how fast do the weights change per experience
  float		cur_lrate;	// #READ_ONLY #NO_INHERIT #SHOW #CAT_Learning current actual learning rate = lrate * lrate_sched current value (* 1 if no lrate_sched)
  LRSValue	lrs_value;	// #CAT_Learning what value to drive the learning rate schedule with (Important: affects values entered in start_ctr fields of schedule!)
  Schedule	lrate_sched;	// #CAT_Learning schedule of learning rate over training epochs or as a function of performance, as determined by lrs_value (NOTE: these factors multiply lrate to give the cur_lrate value)

  WtSigSpec	wt_sig;		// #CAT_Learning sigmoidal weight function for contrast enhancement: high gain makes weights more binary & discriminative
  LearnMixSpec	lmix;		// #CAT_Learning #CONDSHOW_ON_learn_rule:LEABRA_CHL mixture of hebbian & err-driven learning (note: no hebbian for CTLEABRA_XCAL)
  XCalLearnSpec	xcal;		// #CAT_Learning #CONDSHOW_ON_learn_rule:CTLEABRA_XCAL,CTLEABRA_XCAL_C XCAL (eXtended Contrastive Attractor Learning) learning parameters
  XCalContSpec	xcal_c;		// #CAT_Learning #CONDSHOW_ON_learn_rule:CTLEABRA_XCAL_C XCAL (eXtended Contrastive Attractor Learning) fully continuous-time learning parameters
  XCalMiscSpec	xcalm;		// #CAT_Learning #CONDSHOW_ON_learn_rule:CTLEABRA_XCAL,CTLEABRA_XCAL_C XCAL (eXtended Contrastive Attractor Learning) misc extra parameters
  SAvgCorSpec	savg_cor;	// #CAT_Learning for Hebbian and netinput computation: correction for sending average act levels (i.e., renormalization); also norm_con_n for normalizing netinput computation

  AdaptRelNetinSpec rel_net_adapt; // #CAT_Learning adapt relative netinput values based on targets for fm_input, fm_output, and lateral projections -- not used by default (call Compute_RelNetinAdapt to activate; requires Compute_RelNetin and Compute_AvgRelNetin for underlying data)
  
  FunLookup	wt_sig_fun;	// #HIDDEN #NO_SAVE #NO_INHERIT #CAT_Learning computes wt sigmoidal fun
  FunLookup	wt_sig_fun_inv;	// #HIDDEN #NO_SAVE #NO_INHERIT #CAT_Learning computes inverse of wt sigmoidal fun
  WtSigSpec	wt_sig_fun_lst;	// #HIDDEN #NO_SAVE #NO_INHERIT #CAT_Learning last values of wt sig parameters for which the wt_sig_fun's were computed; prevents excessive updating
  float		wt_sig_fun_res;	// #HIDDEN #NO_SAVE #NO_INHERIT #CAT_Learning last values of resolution parameters for which the wt_sig_fun's were computed

  float		SigFmLinWt(float lin_wt) { return wt_sig_fun.Eval(lin_wt);  }
  // #CAT_Learning get contrast-enhanced weight from linear weight value
  float		LinFmSigWt(float sig_wt) { return wt_sig_fun_inv.Eval(sig_wt); }
  // #CAT_Learning get linear weight value from contrast-enhanced sigmoidal weight value

  inline void 	C_Init_Weights(RecvCons* cg, Connection* cn, Unit* ru, Unit* su) {
    ConSpec::C_Init_Weights(cg, cn, ru, su); LeabraCon* lcn = (LeabraCon*)cn;
    lcn->pdw = 0.0f; lcn->sravg_s = lcn->sravg_m = xcalm.avg_init; }
  inline override void Init_Weights(RecvCons* cg, Unit* ru) {
    ConSpec::Init_Weights(cg, ru);
    if(wt_scale_init.init) { wt_scale.abs = wt_scale_init.abs;
      wt_scale.rel = wt_scale_init.rel; } }

  ///////////////////////////////////////////////////////////////
  //	Activation: Netinput -- only NetinDelta is supported

  inline void 	C_Send_NetinDelta_Thrd(Connection* cn, float* send_netin_vec,
				      LeabraUnit* ru, float su_act_delta_eff);
  inline void 	C_Send_NetinDelta_NoThrd(Connection* cn, LeabraUnit* ru,
					float su_act_delta_eff);
  inline void 	C_Send_InhibDelta_Thrd(Connection* cn, float* send_inhib_vec,
				      LeabraUnit* ru, float su_act_delta_eff);
  inline void 	C_Send_InhibDelta_NoThrd(Connection* cn, LeabraUnit* ru,
					float su_act_delta_eff);
  virtual void 	Send_NetinDelta(LeabraSendCons* cg, LeabraNetwork* net, int thread_no,
				float su_act_delta_eff);
  // #CAT_Activation sender-based delta-activation net input for con group (send net input to receivers) -- always goes into tmp matrix (thread_no >= 0!) and is then integrated into net through Compute_NetinInteg function on units

  // recv-based also needed for some statistics, but is NOT used for main compute code -- uses act_eq for sender act as well
  inline float 	C_Compute_Netin(LeabraCon* cn, LeabraUnit*, LeabraUnit* su);
  inline float 	Compute_Netin(RecvCons* cg, Unit* ru);

  ///////////////////////////////////////////////////////////////
  //	Learning

  /////////////////////////////////////
  // LeabraCHL code

  inline void 	Compute_SAvgCor(LeabraSendCons* cg, LeabraUnit* su);
  // #CAT_Learning compute hebb correction scaling term for sending average act (cg->savg_cor) based on layer target activity percent

  inline float	C_Compute_Hebb(LeabraCon* cn, LeabraSendCons* cg, float lin_wt,
			       float ru_act, float su_act);
  // #CAT_Learning compute Hebbian associative learning

  inline float 	C_Compute_Err_LeabraCHL(LeabraCon* cn, float lin_wt,
					float ru_act_p, float ru_act_m,
					float su_act_p, float su_act_m);
  // #CAT_Learning compute generec error term, sigmoid case

  inline void 	C_Compute_dWt(LeabraCon* cn, LeabraUnit* su, float heb, float err);
  // #CAT_Learning combine associative and error-driven weight change, actually update dwt
  inline void 	C_Compute_dWt_NoHebb(LeabraCon* cn, LeabraUnit* su, float err);
  // #CAT_Learning just error-driven weight change, actually update dwt
  virtual void 	Compute_dWt_LeabraCHL(LeabraSendCons* cg, LeabraUnit* su);
  // #CAT_Learning Leabra/CHL weight changes

  inline void	C_Compute_Weights_LeabraCHL(LeabraCon* cn);
  // #CAT_Learning compute weights for LeabraCHL learning rule
  virtual void	Compute_Weights_LeabraCHL(LeabraSendCons* cg, LeabraUnit* su);
  // #CAT_Learning overall compute weights for LeabraCHL learning rule

  /////////////////////////////////////
  // CtLeabraXCAL code

  inline void C_Compute_SRAvg_cont(LeabraCon* cn, float ru_act, float su_act);
  // accumulate sender-receiver activation product average -- continuous (cascaded) version
  inline void C_Compute_SRAvg_m(LeabraCon* cn, float ru_act, float su_act);
  // accumulate sender-receiver activation product average -- medium (trial-level) time scale
  inline void C_Compute_SRAvg_ms(LeabraCon* cn, float ru_act, float su_act);
  // accumulate sender-receiver activation product average -- medium (trial-level) and short (plus phase) time scales
  inline virtual void Compute_SRAvg(LeabraSendCons* cg, LeabraUnit* su, bool do_s);
  // accumulate sender-receiver activation product average -- only for CtLeabraCAL

  inline void C_Trial_Init_SRAvg(LeabraCon* cn);
  // initialize sender-receiver activation product averages for trial and below 
  inline void Trial_Init_SRAvg(LeabraSendCons* cg, LeabraUnit* su);
  // initialize sender-receiver activation product average (only for trial-wise mode, else just in init weights) -- called at start of trial

  inline void 	C_Compute_dWt_CtLeabraXCAL_SR_trial(LeabraCon* cn,
						    LeabraUnit* ru, LeabraUnit* su,
						    float sravg_s_nrm, float sravg_m_nrm);
  // #CAT_Learning compute temporally eXtended Contrastive Attractor Learning (XCAL) -- SR trial-wise version (requires normalization factors) -- uses synapse sravg terms
  inline void 	C_Compute_dWt_CtLeabraXCAL_SRMAX_trial(LeabraCon* cn,
						    LeabraUnit* ru, LeabraUnit* su,
						    float sravg_s_nrm, float sravg_m_nrm);
  // #CAT_Learning compute temporally eXtended Contrastive Attractor Learning (XCAL) -- SRMAX trial-wise version (requires normalization factors) -- uses synapse sravg terms
  inline void 	C_Compute_dWt_CtLeabraXCAL_SEP_trial(LeabraCon* cn,
						     LeabraUnit* ru, LeabraUnit* su,
						     float sravg_s_nrm, float sravg_m_nrm);
  // #CAT_Learning compute temporally eXtended Contrastive Attractor Learning (XCAL) -- SEP trial-wise version (requires normalization factors)
  inline void 	C_Compute_dWt_CtLeabraXCAL_SEP_trial_ml(LeabraCon* cn,
							LeabraUnit* ru, LeabraUnit* su,
							float sravg_s_nrm, float sravg_m_nrm);
  // #CAT_Learning SEP with medium-to-long factor > 0
  inline void 	C_Compute_dWt_CtLeabraXCAL_CAL_trial(LeabraCon* cn,
						     LeabraUnit* ru, LeabraUnit* su,
						     float sravg_s_nrm, float sravg_m_nrm);
  // #CAT_Learning compute temporally eXtended Contrastive Attractor Learning (XCAL) -- CAL trial-wise version (requires normalization factors) -- uses synapse sravg terms
  inline void 	C_Compute_dWt_CtLeabraXCAL_CHL_trial(LeabraCon* cn,
						     LeabraUnit* ru, LeabraUnit* su,
						     float sravg_s_nrm, float sravg_m_nrm);
  // #CAT_Learning compute temporally eXtended Contrastive Attractor Learning (XCAL) -- CHL trial-wise version (requires normalization factors) -- uses synapse sravg terms

  inline void 	C_Compute_dWt_CtLeabraXCAL_C_sep(LeabraCon* cn,
						 LeabraUnit* ru, LeabraUnit* su);
  // #CAT_Learning compute eXtended Contrastive Attractor Learning (XCAL) -- continuous version with separate post and pre factors (SR synapse one requires full learning algo practically)

  virtual void 	Compute_dWt_CtLeabraXCAL(LeabraSendCons* cg, LeabraUnit* su);
  // #CAT_Learning CtLeabraXCAL weight changes
  virtual void 	Compute_dWt_CtLeabraXCAL_C(LeabraSendCons* cg, LeabraUnit* su);
  // #CAT_Learning CtLeabraXCAL_C weight changes

  inline void	C_Compute_Weights_CtLeabraXCAL(LeabraCon* cn);
  // #CAT_Learning overall compute weights for CtLeabraXCAL learning rule
  virtual void	Compute_Weights_CtLeabraXCAL(LeabraSendCons* cg, LeabraUnit* su);
  // #CAT_Learning overall compute weights for CtLeabraXCAL learning rule


  /////////////////////////////////////
  // CtLeabraCAL code

  inline void 	C_Compute_dWt_CtLeabraCAL(LeabraCon* cn,
					  float sravg_s_nrm, float sravg_m_nrm);
  // #CAT_Learning compute Contrastive Attractor Learning (CAL)
  virtual void 	Compute_dWt_CtLeabraCAL(LeabraSendCons* cg, LeabraUnit* su);
  // #CAT_Learning CtLeabraCAL weight changes

  inline void	C_Compute_Weights_CtLeabraCAL(LeabraCon* cn);
  // #CAT_Learning overall compute weights for CtLeabraCAL learning rule
  virtual void	Compute_Weights_CtLeabraCAL(LeabraSendCons* cg, LeabraUnit* su);
  // #CAT_Learning overall compute weights for CtLeabraCAL learning rule

  /////////////////////////////////////
  // Master dWt, Weights functions

  inline void	Compute_Leabra_dWt(LeabraSendCons* cg, LeabraUnit* su);
  // #CAT_Learning overall compute delta-weights for Leabra -- just a switch on learn rule to select above algorithm-specific variant

  inline void	Compute_Leabra_Weights(LeabraSendCons* cg, LeabraUnit* su);
  // #CAT_Learning overall compute weights for Leabra -- just a switch on learn rule to select above algorithm-specific variant


  /////////////////////////////////////
  // 	Bias Weights

  virtual void	B_Compute_dWt_LeabraCHL(LeabraCon* cn, LeabraUnit* ru);
  // #CAT_Learning compute bias weight change for netin model of bias weight

  virtual void 	B_Compute_SRAvg(LeabraCon* cn, LeabraUnit* ru, bool do_s);
  // #CAT_Learning compute bias weight sender-receiver average (actually just receiver)

  virtual void 	B_Trial_Init_SRAvg(LeabraCon* cn, LeabraUnit* ru, LeabraLayer* rlay);
  // #CAT_Learning initialize bias weight sender-receiver average (actually just receiver)

  virtual void 	B_Init_RAvg_l(LeabraCon* cn, LeabraUnit* ru);
  // #CAT_Learning initialize long-time-scale sravg value, stored on the unit (called during init weights)

  virtual void	B_Compute_dWt_CtLeabraXCAL(LeabraCon* cn, LeabraUnit* ru,
						 LeabraLayer* rlay);
  // #CAT_Learning compute bias weight change for XCAL rule
  virtual void	B_Compute_dWt_CtLeabraXCAL_C(LeabraCon* cn, LeabraUnit* ru,
					     LeabraLayer* rlay);
  // #CAT_Learning compute bias weight change for XCAL_C rule
  virtual void	B_Compute_dWt_CtLeabraCAL(LeabraCon* cn, LeabraUnit* ru,
					  LeabraLayer* rlay);
  // #CAT_Learning compute bias weight change for CAL rule

  inline void	B_Compute_Leabra_dWt(LeabraCon* cn, LeabraUnit* ru, LeabraLayer* rlay);
  // #CAT_Learning overall compute bias delta-weights for Leabra -- just a switch on learn rule to select above algorithm-specific variant

  virtual void	B_Compute_Weights(LeabraCon* cn, LeabraUnit* ru);
  // #CAT_Learning update weights for bias connection (same for all algos)

  /////////////////////////////////////
  // General 

  virtual void Compute_CycSynDep(LeabraSendCons* cg, LeabraUnit* su) { };
  // #CAT_Activation compute cycle-level synaptic depression (must be defined by appropriate subclass) -- called at end of each cycle of computation if net_misc.cyc_syn_dep is on.

  virtual void	SetCurLrate(LeabraNetwork* net, int epoch);
  // #CAT_Learning set current learning rate based on schedule given epoch (or error value)
  virtual void	SetLearnRule(LeabraNetwork* net);
  // #CAT_Learning set current learning rule from the network

  virtual void	CreateWtSigFun(); // #CAT_Learning create the wt_sig_fun and wt_sig_fun_inv

  virtual void	Defaults();
  // #BUTTON #CONFIRM #CAT_ObjectMgmt restores default parameter settings: warning -- you will lose any unique parameters you might have set!

  virtual void	LogLrateSched(int epcs_per_step = 50, float n_steps=7);
  // #BUTTON #CAT_Learning establish a logarithmic learning rate schedule with given total number of steps (including first step at lrate) and epochs per step: numbers go down in sequence: 1, .5, .2, .1, .05, .02, .01, etc.. this is a particularly good lrate schedule for large nets on hard tasks

  virtual void	GraphWtSigFun(DataTable* graph_data = NULL);
  // #BUTTON #NULL_OK #NULL_TEXT_NewGraphData graph the sigmoidal weight contrast enhancement function (NULL = new data table)
  virtual void	GraphXCaldWtFun(DataTable* graph_data = NULL, float thr_p = 0.25);
  // #BUTTON #NULL_OK #NULL_TEXT_NewGraphData graph the xcal dWt function for given threshold value (NULL = new data table)
  virtual void	GraphXCalSoftBoundFun(DataTable* graph_data = NULL);
  // #BUTTON #NULL_OK #NULL_TEXT_NewGraphData graph the xcal soft weight bounding function (NULL = new data table)

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
  float		dwt_thresh;  // #DEF_0.1 #MIN_0 #CAT_Learning don't change if dwt < thresh, prevents buildup of small changes

  inline override void	B_Compute_dWt_LeabraCHL(LeabraCon* cn, LeabraUnit* ru);
  inline override void	B_Compute_dWt_CtLeabraXCAL(LeabraCon* cn, LeabraUnit* ru,
						   LeabraLayer* rlay);
  inline override void	B_Compute_dWt_CtLeabraXCAL_C(LeabraCon* cn, LeabraUnit* ru,
						     LeabraLayer* rlay);
  inline override void	B_Compute_dWt_CtLeabraCAL(LeabraCon* cn, LeabraUnit* ru,
						   LeabraLayer* rlay);

  bool	CheckObjectType_impl(taBase* obj);

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
  float		scale_eff;	// #NO_SAVE #CAT_Activation effective scale parameter for netin -- copied to send cons group where it is actually used, but it is computed here
  float		net;		// #NO_SAVE #CAT_Activation netinput to this con_group: only computed for special statistics such as RelNetin

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
  float		scale_eff;	// #NO_SAVE #CAT_Activation effective scale parameter for netin -- copied from recv cons group where it is computed
  float		savg_cor;	// #NO_SAVE #CAT_Learning savg correction factor for hebbian learning

  void 	Send_NetinDelta(LeabraNetwork* net, int thread_no, float su_act_delta)
  { ((LeabraConSpec*)GetConSpec())->Send_NetinDelta(this, net, thread_no, su_act_delta); }
  // #CAT_Activation send delta-netin

  void	Compute_dWt_LeabraCHL(LeabraUnit* su)
  { ((LeabraConSpec*)GetConSpec())->Compute_dWt_LeabraCHL(this, su); }
  // #CAT_Learning compute weight changes: Leabra CHL version
  void	Compute_Weights_LeabraCHL(LeabraUnit* su)
  { ((LeabraConSpec*)GetConSpec())->Compute_Weights_LeabraCHL(this, su); }
  // #CAT_Learning compute weights: Leabra CHL version

  void	Compute_SRAvg(LeabraUnit* su, bool do_s)
  { ((LeabraConSpec*)GetConSpec())->Compute_SRAvg(this, su, do_s); }
  // #CAT_Learning compute sending-receiving activation product averages
  void	Trial_Init_SRAvg(LeabraUnit* su)
  { ((LeabraConSpec*)GetConSpec())->Trial_Init_SRAvg(this, su); }
  // #CAT_Learning initialize sending-receiving activation product averages

  void	Compute_dWt_CtLeabraXCAL(LeabraUnit* su)
  { ((LeabraConSpec*)GetConSpec())->Compute_dWt_CtLeabraXCAL(this, su); }
  // #CAT_Learning compute weight changes: CtLeabra XCAL version
  void	Compute_Weights_CtLeabraXCAL(LeabraUnit* su)
  { ((LeabraConSpec*)GetConSpec())->Compute_Weights_CtLeabraXCAL(this, su); }
  // #CAT_Learning compute weights: CtLeabra XCAL version

  void	Compute_dWt_CtLeabraXCAL_C(LeabraUnit* su)
  { ((LeabraConSpec*)GetConSpec())->Compute_dWt_CtLeabraXCAL_C(this, su); }
  // #CAT_Learning compute weight changes: CtLeabra XCAL_C version

  void	Compute_dWt_CtLeabraCAL(LeabraUnit* su)
  { ((LeabraConSpec*)GetConSpec())->Compute_dWt_CtLeabraCAL(this, su); }
  // #CAT_Learning compute weight changes: CtLeabra CAL version
  void	Compute_Weights_CtLeabraCAL(LeabraUnit* su)
  { ((LeabraConSpec*)GetConSpec())->Compute_Weights_CtLeabraCAL(this, su); }
  // #CAT_Learning compute weights: CtLeabra CAL version

  void	Compute_Leabra_dWt(LeabraUnit* su)
  { ((LeabraConSpec*)GetConSpec())->Compute_Leabra_dWt(this, su); }
  // #CAT_Learning overall compute delta-weights for Leabra -- just a switch on learn rule to select above algorithm-specific variant
  void	Compute_Leabra_Weights(LeabraUnit* su)
  { ((LeabraConSpec*)GetConSpec())->Compute_Leabra_Weights(this, su); }
  // #CAT_Learning overall compute weights for Leabra -- just a switch on learn rule to select above algorithm-specific variant

  void	Compute_CycSynDep(LeabraUnit* su)
  { ((LeabraConSpec*)GetConSpec())->Compute_CycSynDep(this, su); }
  // #CAT_Activation compute cycle-level synaptic depression (must be defined by appropriate subclass) -- called at end of each cycle of computation if net_misc.cyc_syn_dep is on.

  void	Copy_(const LeabraSendCons& cp);
  TA_BASEFUNS(LeabraSendCons);
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
  float		gain;		// #DEF_600 #MIN_0 gain (gamma) of the sigmoidal rate-coded activation function 
  float		nvar;		// #DEF_0.005 #MIN_0 variance of the Gaussian noise kernel for convolving with XX1 in NOISY_XX1
  float		avg_dt;		// #DEF_0.005 #MIN_0 time constant for integrating activation average (computed across trials)
  float		avg_init;	// #DEF_0.15 #MIN_0 initial activation average value
  IThrFun	i_thr;		// [STD or NO_AH for da mod units) how to compute the inhibitory threshold for kWTA functions (what currents to include or exclude in determining what amount of inhibition would keep the unit just at threshold firing) -- for units with dopamine-like modulation using the a and h currents, NO_AH makes learning much more reliable because otherwise kwta partially compensates for the da modulation

  void 	Defaults()	{ Initialize(); }
  TA_SIMPLE_BASEFUNS(ActFunSpec);
protected:
  void	UpdateAfterEdit_impl();
private:
  void	Initialize();
  void	Destroy()	{ };
};

class LEABRA_API SpikeFunSpec : public taOBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra spiking activation function specs -- conductance is computed postsynaptically using an alpha function based on spike pulses sent presynaptically -- for clamped layers, spiking probability is proportional to external input controlled by the clamp_type and clamp_max_p values -- soft clamping may still be a better option though
INHERITED(taOBase)
public:
  float		rise;		// #DEF_0 #MIN_0 exponential rise time (in cycles) of the synaptic conductance according to the alpha function 1/(decay - rise) [e^(-t/decay) - e^(-t/rise)] -- set to 0 to only include decay time (1/decay e^(-t/decay)), which is highly optimized (doesn't use window -- just uses recursive exp decay) and thus the default!
  float		decay;		// #DEF_5 #MIN_0 exponential decay time (in cycles) of the synaptic conductance according to the alpha function 1/(decay - rise) [e^(-t/decay) - e^(-t/rise)] -- set to 0 to implement a delta function (not very useful)
  float		g_gain;		// #DEF_5 #MIN_0 multiplier for the spike-generated conductances when using alpha function which is normalized by area under the curve -- needed to recalibrate the alpha-function currents relative to rate code net input which is overall larger -- in general making this the same as the decay constant works well, effectively neutralizing the area normalization (results in consistent peak current, but differential integrated current over time as a function of rise and decay)
  int		window;		// #DEF_3 #MIN_0 spike integration window -- when rise==0, this window is used to smooth out the spike impulses similar to a rise time -- each net contributes over the window in proportion to 1/window -- for rise > 0, this is used for computing the alpha function -- should be long enough to incorporate the bulk of the alpha function, but the longer the window, the greater the computational cost
  float		eq_gain;	// #DEF_10 #MIN_0 gain for computing act_eq relative to actual average: act_eq = eq_gain * (spikes/cycles)
  float		eq_dt;		// #DEF_0.02 #MIN_0 #MAX_1 if non-zero, eq is computed as a running average with this time constant

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

class LEABRA_API SpikeMiscSpec : public taOBase {
  // ##INLINE ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra extra misc spiking parameters 
INHERITED(taOBase)
public:
  enum ClampType {		// how to generate spikes during hard clamp conditions
    POISSON,			// generate spikes according to Poisson distribution with probability = clamp_max_p * u->ext
    UNIFORM,			// generate spikes according to Uniform distribution with probability = clamp_max_p * u->ext
    REGULAR,			// generate spikes every 1 / (clamp_max_p * u->ext) cycles -- this works the best, at least in smaller networks, due to the lack of additional noise, and the synchrony of the inputs for driving synchrony elsewhere
  };

  float		clamp_max_p;	// #DEF_0.11 #MIN_0 #MAX_1 maximum probability of spike rate firing for hard-clamped external inputs -- multiply ext value times this to get overall probability of firing a spike -- distribution is determined by clamp_type
  ClampType	clamp_type;	// how to generate spikes when layer is hard clamped -- in many cases soft clamping may work better
  float		vm_r;		// #DEF_0;0.15 #AKA_v_m_r post-spiking membrane potential to reset to, produces refractory effect if lower than vm_init
  float		vm_dend;	// #DEF_0.3 how much to add to vm_dend value after every spike
  float		vm_dend_dt;	// #DEF_0.16 rate constant for updating the vm_dend value (used for spike-based learning)
  float		vm_dend_time;	// #READ_ONLY #SHOW time constant (in cycles, 1/vm_dend_dt) for updating the vm_dend value (used for spike-based learning)

  void 	Defaults()	{ Initialize(); }
  TA_SIMPLE_BASEFUNS(SpikeMiscSpec);
protected:
  void	UpdateAfterEdit_impl();
private:
  void	Initialize();
  void	Destroy()	{ };
};

class LEABRA_API ActAdaptSpec : public taOBase {
  // ##INLINE ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra activation-driven adaptation dynamics -- negative feedback on v_m based on sub- and super-threshold activation -- relatively rapid time-scale and especially relevant for spike-based models -- drives the adapt variable on the unit
INHERITED(taOBase)
public:
  bool		on;		// apply adaptation?
  float		dt_rate;	// #CONDSHOW_ON_on rate constant of the adaptation dynamics -- if the vm time step is considered to be 1 msec, then rate values of .02 to .1 (10 to 50 msec) are typical, with .02 being a regular spiking neuron, and .1 is fast spiking
  float		dt_time;	// #CONDSHOW_ON_on #READ_ONLY #SHOW time constant (in cycles = 1/dt_rate) of the adaptation dynamics
  float		vm_gain;	// #CONDSHOW_ON_on #MIN_0 #MAX_1 gain on the membrane potential v_m driving the adapt adaptation variable -- values around .1 are typical (resting v_m is subtracted from v_m for this, so that 0 adaptation occurs at rest)
  float		spike_gain;	// #CONDSHOW_ON_on value to add to the adapt adaptation variable after spiking -- range is around .01 for regular spiking neurons (strong adaptation) and lower -- for rate code activations, uses act value weighting and only computes every interval
  int		interval;	// #CONDSHOW_ON_on how many time steps between applying spike_gain for rate-coded activation function -- simulates the intrinsic delay obtained with spiking dynamics

  float	Compute_dAdapt(float vm, float adapt) {
    return dt_rate * (vm_gain * vm - adapt);
  }
  // compute the change in adapt given vm (with rest value already subtracted off) and adapt inputs

  void 	Defaults()	{ Initialize(); }
  TA_SIMPLE_BASEFUNS(ActAdaptSpec);
protected:
  void	UpdateAfterEdit_impl();
private:
  void	Initialize();
  void	Destroy()	{ };
};


class LEABRA_API DepressSpec : public taOBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra activation/spiking conveyed to other units is subject to synaptic depression: depletes a given amount per spike, and recovers with exponential recovery rate (also subject to trial/phase decay = recovery proportion)
INHERITED(taOBase)
public:
  bool		on;		// synaptic depression is in effect: multiplies normal activation computed by current activation function in effect
  float		rec;		// #CONDEDIT_ON_on #DEF_0.2 #MIN_0 rate of recovery of spike amplitude (determines overall time constant of depression function)
  float		asymp_act;	// #CONDEDIT_ON_on #DEF_0.5 #MIN_0 asymptotic activation value (as proportion of 1) for a fully active unit (determines depl value)
  float		depl;		// #CONDEDIT_ON_on #READ_ONLY #SHOW rate of depletion of spike amplitude as a function of activation output (computed from rec, asymp_act)
  int		interval;	// #CONDEDIT_ON_on #MIN_1 only update synaptic depression at given interval (in terms of cycles, using ct_cycle) -- this can be beneficial in producing a more delayed overall effect, as is observed with discrete spiking
  float		max_amp;	// #CONDEDIT_ON_on #MIN_0 maximum spike amplitude -- this is the multiplier factor for activation values -- set to clamp_norm_max_amp to maintain asymptotic values at normal hard clamp levels, or set to 1 to retain usual normalized activation values (val is copied to act_range.max)
  float		clamp_norm_max_amp;	// #CONDEDIT_ON_on #READ_ONLY #SHOW maximum spike amplitude required to maintain asymptotic firing at normal clamp levels -- set max_amp to this value for delta-based learning rules to normalize against large diffs across phases

  void 	Defaults()	{ Initialize(); }
  TA_SIMPLE_BASEFUNS(DepressSpec);
protected:
  void	UpdateAfterEdit_impl();
private:
  void	Initialize();
  void	Destroy()	{ };
};

class LEABRA_API SynDelaySpec : public taOBase {
  // ##INLINE ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra synaptic delay -- activation sent to other units is delayed by a given number of cycles
INHERITED(taOBase)
public:
  bool		on;		// is synaptic delay active?
  int		delay;		// #CONDEDIT_ON_on #MIN_0 number of cycles to delay for

  void 	Defaults()	{ Initialize(); }
  TA_SIMPLE_BASEFUNS(SynDelaySpec);
private:
  void	Initialize();
  void	Destroy()	{ };
};

class LEABRA_API OptThreshSpec : public taOBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra optimization thresholds for faster processing
INHERITED(taOBase)
public:
  float		send;		// #DEF_0.1 don't send activation when act <= send -- greatly speeds processing
  float		delta;		// #DEF_0.005 don't send activation changes until they exceed this threshold: only for when LeabraNetwork::send_delta is on!
  float		phase_dif;	// #DEF_0 don't learn when +/- phase difference ratio (- / +) < phase_dif (.8 when used, but off by default)

  void 	Defaults()	{ Initialize(); }
  TA_SIMPLE_BASEFUNS(OptThreshSpec);
private:
  void	Initialize();
  void	Destroy()	{ };
};

class LEABRA_API LeabraDtSpec : public taOBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra rate constants for temporal derivatives in Leabra (Vm, net input)
INHERITED(taOBase)
public:
  float		vm;		// #DEF_0.1:0.3 #MIN_0 membrane potential rate constant -- if units oscillate too much, then this is too high (but see d_vm_max for another solution)
  float		vm_time;	// #READ_ONLY #SHOW 1/vm rate constant = time in cycles for vm to reach 1/e of asymptotic value
  float		net;		// #DEF_0.7 #MIN_0 net input time constant -- how fast to update net input (damps oscillations)
  float		net_time;	// #READ_ONLY #SHOW 1/net rate constant = time in cycles for net to reach 1/e of asymptotic value
  bool		midpoint;	// #DEF_false use the midpoint method in computing the vm value -- better avoids oscillations and allows a larger dt.vm parameter to be used
  float		d_vm_max;	// #DEF_0.02:0.025 #MIN_0 maximum change in vm at any timestep (limits blowup)
  int		vm_eq_cyc;	// #AKA_cyc0_vm_eq #DEF_0 number of cycles to compute the vm as equilibirium potential given current inputs: set to 1 to quickly activate input layers; set to 100 to always use this computation
  float		vm_eq_dt;	// #DEF_1 #MIN_0 time constant for integrating the vm_eq values: how quickly to move toward the current eq value from previous vm value

  void 	Defaults()	{ Initialize(); }
  TA_SIMPLE_BASEFUNS(LeabraDtSpec);
protected:
  void	UpdateAfterEdit_impl();
private:
  void	Initialize();
  void	Destroy()	{ };
};

class LEABRA_API LeabraChannels : public taOBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra channels used in Leabra
INHERITED(taOBase)
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

class LEABRA_API VChanSpec : public taOBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra voltage gated channel specs
INHERITED(taOBase)
public:
  bool		on;		// #DEF_false true if channel is on
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

class LEABRA_API MaxDaSpec : public taOBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra how to compute the maxda value, which serves as a stopping criterion for settling
INHERITED(taOBase)
public:
  enum dAValue {
    DA_ONLY,			// just use da
    INET_ONLY,			// just use inet
    INET_DA			// use inet if no activity, then use da
  };

  dAValue	val;		// #DEF_INET_DA value to use for computing delta-activation (change in activation over cycles of settling).
  float		inet_scale;	// #DEF_1 #MIN_0 how to scale the inet measure to be like da
  float		lay_avg_thr;	// #DEF_0.01 #MIN_0 threshold for layer average activation to switch to da fm Inet

  void 	Defaults()	{ Initialize(); }
  TA_SIMPLE_BASEFUNS(MaxDaSpec);
private:
  void	Initialize();
  void	Destroy()	{ };
};

class LEABRA_API DaModSpec : public taOBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra specs for effects of da-based modulation: plus-phase = learning effects
INHERITED(taOBase)
public:
  enum ModType {
    PLUS_CONT,			// da modulates plus-phase activations (only) in a continuous manner
    PLUS_POST,			// da modulates plus-phase activations (only), at the end of the plus phase
  };

  bool		on;		// whether to actually modulate activations by da values
  ModType	mod;		// #CONDEDIT_ON_on:true how to apply DA modulation
  float		gain;		// #CONDEDIT_ON_on:true gain multiplier of da values

  void 	Defaults()	{ Initialize(); }
  TA_SIMPLE_BASEFUNS(DaModSpec);
private:
  void	Initialize();
  void 	Destroy()	{ };
};

class LEABRA_API NoiseAdaptSpec : public taOBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra specs for adapting the noise variance over time as a function of different variables
INHERITED(taOBase)
public:
  enum AdaptMode {
    FIXED_NOISE,		// no adaptation of noise: remains fixed at the noise.var value in the unit spec
    SCHED_CYCLES,		// use noise_sched over cycles of settling
    SCHED_EPOCHS,		// use noise_sched over epochs of learning
    PVLV_PVI,			// use PVLV PVi reward prediction values -- predicts primary value (PV) and inhibits it (i) -- only if a PVLV config is in place -- appropriate for output-oriented layers that are important for generating responses associated with rewards
    PVLV_LVE,			// use PVLV LVe reward prediction values -- learned value excitatory reward associations -- this can be active outside the time when primary rewards are expected, and is appropriate for working memory or other internal or sensory-oriented processing
    PVLV_MIN,			// use the minimum of PVi and LVe values -- an overall worst case appraisal of the state of the system
  };

  bool		trial_fixed;	// keep the same noise value over the entire trial -- prevents noise from being washed out and produces a stable effect that can be better used for learning -- this is strongly recommended for most learning situations
  AdaptMode 	mode;		// how to adapt noise variance over time
  float		min_pct;	// #CONDEDIT_OFF_mode:FIXED_NOISE,SCHED_CYCLES,SCHED_EPOCHS #DEF_0.5 minimum noise as a percentage (proportion) of overall maximum noise value (which is noise.var in unit spec)
  float		min_pct_c;	// #READ_ONLY 1-min_pct

  // this gives the short-long integration function proposed by Aston-Jones & Cohen
  // which turns out to not work very well at all: what matters is the specific 
  // reward prediction on the current trial, given by the PVLV option currently
//   inline float	SigFun(float val, float gn) {
//     if(val <= 0.0f) return 0.0f;
//     if(val >= 1.0f) return 1.0f;
//     return (1.0f / (1.0f + powf((1.0f - val) / val, gn)));
//   }

//   inline float	SLNoiseFun(float norm_st_val, float norm_lt_val) {
//     return 1.0f - ((1.0f - min_pct) * SigFun(norm_lt_val, lt_sig_gain) 
// 		   * (1.0f - st_mult * SigFun(norm_st_val, st_sig_gain)));
//   }
//   // compute short-long term noise function based on short and long term values normalized 0-1 where 1 is *best* performance and 0 is *worst* performance

  void 	Defaults()	{ Initialize(); }
  TA_SIMPLE_BASEFUNS(NoiseAdaptSpec);
protected:
  void	UpdateAfterEdit_impl();

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
    ACT_NOISE,			// noise in the activations
  };

  ActFun	act_fun;	// #CAT_Activation activation function to use
  ActFunSpec	act;		// #CAT_Activation activation function specs
  SpikeFunSpec	spike;		// #CONDSHOW_ON_act_fun:SPIKE #CAT_Activation spiking function specs (only for act_fun = SPIKE)
  SpikeMiscSpec	spike_misc;	// #CONDSHOW_ON_act_fun:SPIKE #CAT_Activation misc extra spiking function specs (only for act_fun = SPIKE)
  ActAdaptSpec 	adapt;		// #CAT_Activation activation-driven adaptation factor that drives spike rate adaptation dynamics based on both sub- and supra-threshold membrane potentials
  DepressSpec	depress;	// #CAT_Activation depressing synapses specs -- multiplies activation value by a spike amplitude/probability value that depresses with use and recovers exponentially
  SynDelaySpec	syn_delay;	// #CAT_Activation synaptic delay -- if active, activation sent to other units is delayed by a given amount
  OptThreshSpec	opt_thresh;	// #CAT_Learning optimization thresholds for speeding up processing when units are basically inactive
  MinMaxRange	clamp_range;	// #CAT_Activation range of clamped activation values (min, max, 0, .95 std), don't clamp to 1 because acts can't reach, so .95 instead
  MinMaxRange	vm_range;	// #CAT_Activation membrane potential range (min, max, 0-1 for normalized, -90-50 for bio-based)
  RandomSpec	v_m_init;	// #CAT_Activation what to initialize the membrane potential to (mean = .15, var = 0 std)
  LeabraDtSpec	dt;		// #CAT_Activation time constants (rate of updating): membrane potential (vm) and net input (net)
  LeabraChannels g_bar;		// #CAT_Activation [Defaults: 1, .1, 1, .1, .5] maximal conductances for channels
  LeabraChannels e_rev;		// #CAT_Activation [Defaults: 1, .15, .15, 1, 0] reversal potentials for each channel
  LeabraChannels e_rev_sub_thr;	// #CAT_Activation #READ_ONLY #NO_SAVE #HIDDEN e_rev - act.thr for each item -- used for compute_ithresh
  VChanSpec	hyst;		// #CAT_Activation [Defaults: .05, .8, .7, .1] hysteresis (excitatory) v-gated chan (Ca2+, NMDA)
  VChanSpec	acc;		// #CAT_Activation [Defaults: .01, .5, .1, .1] accomodation (inhibitory) v-gated chan (K+)
  DaModSpec	da_mod;		// #CAT_Learning da modulation of activations (for da-based learning, and other effects)
  MaxDaSpec	maxda;		// #CAT_Activation maximum change in activation (da) computation -- regulates settling
  NoiseType	noise_type;	// #CAT_Activation where to add random noise in the processing (if at all)
  RandomSpec	noise;		// #CONDEDIT_OFF_noise_type:NO_NOISE #CAT_Activation distribution parameters for random added noise
  NoiseAdaptSpec noise_adapt;	// #CONDEDIT_OFF_noise_type:NO_NOISE #CAT_Activation how to adapt the noise variance (var) value
  Schedule	noise_sched;	// #CONDEDIT_OFF_noise_type:NO_NOISE #CAT_Activation schedule of noise variance -- time scale depends on noise_adapt parameter (cycles, epochs, etc)

  FunLookup	nxx1_fun;	// #HIDDEN #NO_SAVE #NO_INHERIT #CAT_Activation convolved gaussian and x/x+1 function as lookup table
  FunLookup	noise_conv;	// #HIDDEN #NO_SAVE #NO_INHERIT #CAT_Activation gaussian for convolution

  ///////////////////////////////////////////////////////////////////////
  //	General Init functions

  virtual void	SetLearnRule(LeabraNetwork* net);
  // #CAT_Learning set current learning rule from the network

  override void Init_Weights(Unit* u, Network* net);
  override void	Init_Acts(Unit* u, Network* net);
  virtual void 	Init_ActAvg(LeabraUnit* u, LeabraNetwork* net);
  // #CAT_Activation initialize average activation values, used to control learning

  virtual void	DecayState(LeabraUnit* u, LeabraNetwork* net, float decay);
  // #CAT_Activation decay activation states towards initial values by given amount (0 = no decay, 1 = full decay)

  ///////////////////////////////////////////////////////////////////////
  //	TrialInit -- at start of trial

  virtual void	SetCurLrate(LeabraNetwork* net, int epoch);
  // #CAT_Learning set current learning rate based on epoch

  virtual void	Trial_Init_Unit(LeabraUnit* u, LeabraNetwork* net, int thread_no=-1);
  // #CAT_Activation trial unit-level initialization functions: DecayState, NoiseInit, Trial_Init_SRAvg

    virtual void Trial_DecayState(LeabraUnit* u, LeabraNetwork* net);
    // #CAT_Activation decay activation states towards initial values: at trial-level boundary
    virtual void Trial_NoiseInit(LeabraUnit* u, LeabraNetwork* net);
    // #CAT_Activation init trial-level noise -- ONLY called if noise_adapt.trial_fixed is set
    virtual void Trial_Init_SRAvg(LeabraUnit* u, LeabraNetwork* net);
    // #CAT_Learning reset the sender-receiver coproduct average -- call at start of trial

  ///////////////////////////////////////////////////////////////////////
  //	SettleInit -- at start of settling

  virtual void Settle_Init_Unit(LeabraUnit* u, LeabraNetwork* net, int thread_no=-1);
  // #CAT_Activation settle unit-level initialization functions: Init_TargFlags, DecayState, NetinScale

    virtual void Settle_Init_TargFlags(LeabraUnit* u, LeabraNetwork* net);
    // #CAT_Activation initialize external input flags based on phase
    virtual void Settle_DecayState(LeabraUnit* u, LeabraNetwork* net);
    // #CAT_Activation decay activation states towards initial values: at phase-level boundary -- start of settling
    virtual void Compute_NetinScale(LeabraUnit* u, LeabraNetwork* net);
    // #CAT_Activation compute net input scaling values -- call at start of settle just to be sure

  virtual void Compute_NetinScale_Senders(LeabraUnit* u, LeabraNetwork* net);
  // #CAT_Activation compute net input scaling values for sending cons -- copies from values computed in the recv guys -- has to be done as a second phase of the Settle_Init_Unit stage after all the recv ones are computed

  virtual void	Compute_HardClamp(LeabraUnit* u, LeabraNetwork* net);
  // #CAT_Activation force units to external values provided by environment
  virtual void	Compute_HardClampNoClip(LeabraUnit* u, LeabraNetwork* net);
  // #CAT_Activation hard-clamp units without clipping values to clamp_range (use for freezing activation states for example, e.g., in second plus phase)

  virtual void	ExtToComp(LeabraUnit* u, LeabraNetwork* net);
  // #CAT_Activation change external inputs to comparisons (remove input)
  virtual void	TargExtToComp(LeabraUnit* u, LeabraNetwork* net);
  // #CAT_Activation change target & external inputs to comparisons (remove targ & input)


  ///////////////////////////////////////////////////////////////////////
  //	Cycle Step 1: Netinput 

  virtual void 	Send_NetinDelta(LeabraUnit* u, LeabraNetwork* net, int thread_no=-1);
  // #CAT_Activation send netinput; sender based and only when act changes above a threshold -- only this delta form is supported
  virtual void	Compute_NetinInteg(LeabraUnit* u, LeabraNetwork* net, int thread_no=-1);
  // #CAT_Activation integrate newly-computed netinput delta values into a resulting complete netinput value for the network (does both excitatory and inhibitory)
    virtual void Compute_NetinInteg_Spike(LeabraUnit* u, LeabraNetwork* net);
    // #IGNORE called by Compute_NetinInteg for spiking units: compute actual netin conductance value for spiking units by integrating over spike

    virtual float Compute_IThresh(LeabraUnit* u, LeabraNetwork* net);
    // #CAT_Activation called by Compute_NetinInteg: compute inhibitory value that would place unit directly at threshold
      inline float Compute_IThreshStd(LeabraUnit* u, LeabraNetwork* net);
      // #IGNORE called by Compute_IThresh: compute inhibitory value that would place unit directly at threshold, using all currents EXCEPT bias.wt
      inline float Compute_IThreshNoA(LeabraUnit* u, LeabraNetwork* net);
      // #IGNORE called by Compute_IThresh: compute inhibitory value that would place unit directly at threshold, excluding gc.a current
      inline float Compute_IThreshNoH(LeabraUnit* u, LeabraNetwork* net);
      // #IGNORE called by Compute_IThresh: compute inhibitory value that would place unit directly at threshold, excluding gc.a current
      inline float Compute_IThreshNoAH(LeabraUnit* u, LeabraNetwork* net);
      // #IGNORE called by Compute_IThresh: compute inhibitory value that would place unit directly at threshold, excluding any gc.a, gc.h currents
      inline float Compute_IThreshAll(LeabraUnit* u, LeabraNetwork* net);
      // #IGNORE called by Compute_IThresh: compute inhibitory value that would place unit directly at threshold, using all currents INCLUDING bias.wt

  ///////////////////////////////////////////////////////////////////////
  //	Cycle Step 2: Inhibition

  inline void	Compute_ApplyInhib(LeabraUnit* u, LeabraNetwork* net, float inhib_val);
  // #CAT_Activation apply computed (kwta) inhibition value to unit inhibitory conductance


  ///////////////////////////////////////////////////////////////////////
  //	Cycle Step 3: Activation

  // main function is basic Compute_Act which calls all the various sub-functions
  override void	Compute_Act(Unit* u, Network* net, int thread_no=-1);

    virtual void Compute_Conduct(LeabraUnit* u, LeabraNetwork* net);
    // #CAT_Activation Act Step 1: compute input conductance values in the gc variables
      virtual void Compute_DaMod_PlusCont(LeabraUnit* u, LeabraNetwork* net);
      // #IGNORE called by Compute_Conduct compute da modulation as plus-phase continuous gc.h/.a

    virtual void Compute_Vm(LeabraUnit* u, LeabraNetwork* net);
    // #CAT_Activation Act Step 2: compute the membrane potential from input conductances
      inline float Compute_EqVm(LeabraUnit* u);
      // #CAT_Activation compute the equilibrium (asymptotic) membrante potential from input conductances (assuming they remain fixed as they are)

    virtual void Compute_ActFmVm(LeabraUnit* u, LeabraNetwork* net);
    // #CAT_Activation Act Step 2: compute the activation from membrane potential
      virtual void Compute_ActFmVm_rate(LeabraUnit* u, LeabraNetwork* net);
      // #CAT_Activation compute the activation from membrane potential -- rate code functions
        virtual float Compute_ActValFmVmVal_rate(float vm_val);
        // #CAT_Activation raw activation function: computes an activation value given membrane potential value based on current activation function -- does not update state variables or anything
      virtual void Compute_ActAdapt_rate(LeabraUnit* u, LeabraNetwork* net);
      // #CAT_Activation compute the activation-based adaptation value based on activation (spiking rate) and membrane potential -- rate code functions

      virtual void Compute_ActFmVm_spike(LeabraUnit* u, LeabraNetwork* net);
      // #CAT_Activation compute the activation from membrane potential -- discrete spiking
      virtual void Compute_ActAdapt_spike(LeabraUnit* u, LeabraNetwork* net);
      // #CAT_Activation compute the activation-based adaptation value based on spiking and membrane potential -- spike functions
      virtual void Compute_ClampSpike(LeabraUnit* u, LeabraNetwork* net, float spike_p);
      // #CAT_Activation compute spiking activation according to spike.clamp_type with given probability (typically spike.clamp_max_p * u->ext) -- includes depression and other active factors as done in Compute_ActFmVm_spike -- used for hard clamped inputs in spiking nets
    virtual void Compute_SelfReg_Cycle(LeabraUnit* u, LeabraNetwork* net);
    // #CAT_Activation Act Step 3: compute self-regulatory currents (hysteresis, accommodation) -- at the cycle time scale
    virtual void Compute_SelfReg_Trial(LeabraUnit* u, LeabraNetwork* net);
    // #CAT_Activation compute self-regulatory currents (hysteresis, accommodation) -- at the trial time scale

    virtual float Compute_Noise(LeabraUnit* u, LeabraNetwork* net);
    // #CAT_Activation utility fun to generate and return the noise value based on current settings -- will set unit->noise value as appropriate (generally excludes effect of noise_sched schedule)

  ///////////////////////////////////////////////////////////////////////
  //	Cycle Stats

  virtual float Compute_MaxDa(LeabraUnit* u, LeabraNetwork* net);
  // #CAT_Activation statistic function to compute the maximum delta-activation (change in activation); used to control settling -- not threadable as it is used to update net, layer, and Inhib values

  ///////////////////////////////////////////////////////////////////////
  //	Cycle Optional Misc

  virtual void 	Compute_CycSynDep(LeabraUnit* u, LeabraNetwork* net, int thread_no=-1);
  // #CAT_Activation compute cycle-level synaptic depression (must be defined by appropriate conspec subclass) -- called at end of each cycle of computation if net_misc.cyc_syn_dep is on -- threaded direct to units

  ///////////////////////////////////////////////////////////////////////
  //	Settle Final

  virtual void	PostSettle(LeabraUnit* u, LeabraNetwork* net);
  // #CAT_Activation set stuff after settling is over (act_m, act_p, etc), ActTimeAvg, DaMod_PlusPost
    virtual void Compute_ActTimeAvg(LeabraUnit* u, LeabraNetwork* net);
    // #CAT_Activation compute time-averaged activation of unit (using act.avg_dt time constant), typically done at end of settling in PostSettle function
    virtual void Compute_DaMod_PlusPost(LeabraUnit* u, LeabraNetwork* net);
    // #CAT_Activation post-plus dav modulation

  ///////////////////////////////////////////////////////////////////////
  //	Trial Update and Final

  virtual void	EncodeState(LeabraUnit*, LeabraNetwork*) { };
  // #CAT_Learning encode current state information after end of current trial (hook for time-based learning)

  ///////////////////////////////////////////////////////////////////////
  //	Learning

  virtual void 	Compute_SRAvg(LeabraUnit* u, LeabraNetwork* net, int thread_no=-1);
  // #CAT_Learning compute sending-receiving activation product averages (CtLeabraX/CAL)

  virtual void 	Compute_dWt_FirstPlus(LeabraUnit* u, LeabraNetwork* net, int thread_no=-1);
  // #CAT_Learning compute weight change after first plus phase has been encountered: standard layers do a weight change here, except under CtLeabra_X/CAL
  virtual void	Compute_dWt_SecondPlus(LeabraUnit* u, LeabraNetwork* net, int thread_no=-1);
  // #CAT_TrialFinal compute weight change after second plus phase has been encountered: standard layers do NOT do a weight change here -- only selected special ones
  virtual void	Compute_dWt_Nothing(LeabraUnit* u, LeabraNetwork* net, int thread_no=-1);
  // #CAT_TrialFinal compute weight change after final nothing phase: standard layers do a weight change here under both learning rules

  override void	Compute_Weights(Unit* u, Network* net, int thread_no=-1);

  ///////////////////////////////////////////////////////////////////////
  //	Stats

  override float Compute_SSE(Unit* u, Network* net, bool& has_targ);
  virtual float  Compute_NormErr(LeabraUnit* u, LeabraNetwork* net);
  // #CAT_Statistic compute normalized binary error (0-1 as function of bits off from target) according to settings on the network (returns a 1 or 0)

  ///////////////////////////////////////////////////////////////////////
  //	Misc Housekeeping, non Compute functions

  virtual void	CreateNXX1Fun();  // #CAT_Activation create convolved gaussian and x/x+1 

  virtual void	Defaults();	// #BUTTON #CONFIRM #CAT_ObjectMgmt restores default parameter settings: warning -- you will lose any unique parameters you might have set!

  virtual void	GraphVmFun(DataTable* graph_data, float g_i = .5, float min = 0.0, float max = 1.0, float incr = .01);
  // #MENU_BUTTON #MENU_ON_Graph #NULL_OK #NULL_TEXT_NewGraphData graph membrane potential (v_m) as a function of excitatory net input (net) for given inhib conductance (g_i) (NULL = new graph data)
  virtual void	GraphActFmVmFun(DataTable* graph_data, float min = .15, float max = .50, float incr = .001);
  // #MENU_BUTTON #MENU_ON_Graph #NULL_OK #NULL_TEXT_NewGraphData graph the activation function as a function of membrane potential (v_m) (NULL = new graph data)
  virtual void	GraphActFmNetFun(DataTable* graph_data, float g_i = .5, float min = 0.0, float max = 1.0, float incr = .001);
  // #MENU_BUTTON #MENU_ON_Graph #NULL_OK #NULL_TEXT_NewGraphData graph the activation function as a function of net input (projected through membrane potential) (NULL = new graph data)
  virtual void	GraphSpikeAlphaFun(DataTable* graph_data, bool force_alpha=false);
  // #MENU_BUTTON #MENU_ON_Graph #NULL_OK #NULL_TEXT_NewGraphData graph the spike alpha function for conductance integration over time window given in spike parameters -- last data point is the sum over the whole window (total conductance of a single spike) -- force_alpha means use explicit alpha function even when rise=0 (otherewise it simulates actual recursive exp decay used in optimized code)
//   virtual void	GraphSLNoiseAdaptFun(DataTable* graph_data, float incr = 0.05f);
//   // #MENU_BUTTON #MENU_ON_Graph #NULL_OK #NULL_TEXT_NewGraphData graph the short and long-term noise adaptation function, which integrates both short-term and long-term performance values
  virtual void TimeExp(int mode, int nreps=100000000);
  // #MENU_BUTTON #MENU_ON_Graph ime how long it takes to compute various forms of exp() function: mode=0 = double sum ctrl (baseline), mode=1 = std double exp(), mode=2 = taMath_double::exp_fast, mode=3 = float sum ctrl (float baseline), mode=4 = expf, mode=5 = taMath_float::exp_fast -- this is the dominant cost in spike alpha function computation, so we're interested in optimizing it..

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
  float		act_eq;		// #VIEW_HOT #CAT_Activation rate-code equivalent activity value (time-averaged spikes or just act)
  float		act_nd;		// #CAT_Activation non-depressed rate-code equivalent activity value (time-averaged spikes or just act) -- used for final phase-based variables used in learning and stats
  float		act_avg;	// #CAT_Activation average activation (of final plus phase activation state) over long time intervals (dt = act.avg_dt)
  float		ravg_ml;	// #CAT_Activation medium-to-long time-scale average activation (as computed in the bias connection and spec) which integrates over recent history of medium (trial level) averages, used for rapid adaptation of l_thr = LTP vs LTD learning threshold in XCAL, and for learning based on receiver average activations with a trace of prior activations
  float		ravg_l;		// #CAT_Activation long time-scale average of medium-time scale (trial level) activation (as computed in the bias connection and spec), used for l_thr = the LTP vs LTD learning threshold in XCAL
  float		l_thr;		// #CAT_Activation long time-scale LTP vs LTD learning threshold in XCAL BCM-style learning -- l_gain * MAX(ravg_l, ravg_ml) (as computed in the bias connection and spec)
  float		act_m;		// #VIEW_HOT #CAT_Activation minus_phase activation (act_nd), set after settling, used for learning and performance stats 
  float		act_p;		// #VIEW_HOT #CAT_Activation plus_phase activation (act_nd), set after settling, used for learning and performance stats
  float		act_dif;	// #VIEW_HOT #CAT_Activation difference between plus and minus phase acts, gives unit err contribution
  float		act_m2;		// #VIEW_HOT #CAT_Activation second minus_phase (e.g., nothing phase) activation (act_nd), set after settling, used for learning and performance stats
  float		act_p2;		// #CAT_Activation second plus_phase activation (act_nd), set after settling, used for learning and performance stats
  float		act_dif2;	// #CAT_Activation difference between second set of phases, where relevant (e.g., act_p - act_m2 for MINUS_PLUS_NOTHING, or act_p2 - act_p for MINUS_PLUS_PLUS)
  float		da;		// #NO_SAVE #CAT_Activation delta activation: change in act from one cycle to next, used to stop settling
  VChanBasis	vcb;		// #CAT_Activation voltage-gated channel basis variables
  LeabraUnitChans gc;		// #DMEM_SHARE_SET_1 #NO_SAVE #CAT_Activation current unit channel conductances
  float		I_net;		// #NO_SAVE #CAT_Activation net current produced by all channels
  float		v_m;		// #NO_SAVE #CAT_Activation membrane potential
  float		vm_dend;	// #NO_SAVE #CAT_Activation dendritic membrane potential -- reflects back-propagated spike values in spiking mode -- these are not subject to immediate AHP and thus decay exponentially, and are used for learning
  float		adapt;		// #NO_SAVE #CAT_Activation adaptation factor -- driven by both sub-threshold membrane potential and spiking activity -- subtracts directly from the membrane potential on every time step
  float		noise;		// #NO_SAVE #CAT_Activation noise value added to unit (noise_type on unit spec determines where it is added) -- this can be used in learning in some cases
  float 	dav;		// #VIEW_HOT #CAT_Activation dopamine value (da is delta activation) which modulates activations (e.g., via accom and hyst currents) to then drive learning
  float 	maint_h;	// #CAT_Activation maintained hysteresis current value (e.g., for PFC units)

  bool		in_subgp;	// #READ_ONLY #NO_SAVE #CAT_Structure determine if unit is in a subgroup
  float		net_scale;	// #NO_VIEW #NO_SAVE #EXPERT #CAT_Activation total netinput scaling basis
  float		bias_scale;	// #NO_VIEW #NO_SAVE #EXPERT #CAT_Activation bias weight scaling factor
  float		prv_net;	// #NO_VIEW #NO_SAVE #EXPERT #CAT_Activation previous net input (for time averaging)
  float		prv_g_i;	// #NO_VIEW #NO_SAVE #EXPERT #CAT_Activation previous inhibitory conductance value (for time averaging)

  float		act_sent;	// #NO_VIEW #NO_SAVE #EXPERT #CAT_Activation last activation value sent (only send when diff is over threshold)
  float		net_raw;	// #NO_VIEW #NO_SAVE #EXPERT #CAT_Activation raw net input received from sending units (send delta delta's are added to this value)
  float		net_delta;	// #NO_VIEW #NO_SAVE #EXPERT #CAT_Activation delta net input received from sending units -- only used for non-threaded case
  float		g_i_raw;	// #NO_VIEW #NO_SAVE #EXPERT #CAT_Activation raw inhib net input received from sending units (increments the deltas in send_delta)
  float		g_i_delta;	// #NO_VIEW #NO_SAVE #EXPERT #CAT_Activation delta inhibitory net input received from sending units -- only used for non-threaded case

  float		i_thr;		// #NO_SAVE #CAT_Activation inhibitory threshold value for computing kWTA
  float		spk_amp;	// #CAT_Activation amplitude/probability of spiking output (for synaptic depression function if unit spec depress.on is on)
  float		misc_1;		// #NO_SAVE #CAT_Activation miscellaneous variable for other algorithms that need it
  float		misc_2;		// #NO_SAVE #CAT_Activation miscellaneous variable for other algorithms that need it
  float_CircBuffer act_buf;	// #NO_VIEW #NO_SAVE #CAT_Activation buffer of activation states for synaptic delay computation
  float_CircBuffer spike_buf;	// #NO_VIEW #NO_SAVE #CAT_Activation buffer of net input from spikes for synaptic integration over discrete spikes

  inline void	AddToActBuf(SynDelaySpec& sds) {
    if(sds.on) act_buf.CircAddLimit(act, sds.delay);
  }
  // add current activation to act buf if synaptic delay is on

  inline LeabraLayer*	own_lay() const {return (LeabraLayer*)inherited::own_lay();}
  inline LeabraUnit_Group* own_ugp() const {return (LeabraUnit_Group*)owner;}
  LeabraInhib*		own_thr() const;

  ///////////////////////////////////////////////////////////////////////
  //	General Init functions

  void	Init_ActAvg(LeabraNetwork* net)
  { ((LeabraUnitSpec*)GetUnitSpec())->Init_ActAvg(this, net); }
  // #CAT_Activation initialize average activation

  void DecayState(LeabraNetwork* net, float decay)
  { ((LeabraUnitSpec*)GetUnitSpec())->DecayState(this, net, decay); }
  // #CAT_Activation decay activation states towards initial values by given amount (0 = no decay, 1 = full decay)

  ///////////////////////////////////////////////////////////////////////
  //	TrialInit -- at start of trial

  void	Trial_Init_Unit(LeabraNetwork* net, int thread_no=-1)
  { ((LeabraUnitSpec*)GetUnitSpec())->Trial_Init_Unit(this, net, thread_no); }
  // #CAT_Activation trial unit-level initialization functions: DecayState, NoiseInit, Trial_Init_SRAvg

    void Trial_DecayState(LeabraNetwork* net)
    { ((LeabraUnitSpec*)GetUnitSpec())->Trial_DecayState(this, net); }
    // #CAT_Activation decay activation states towards initial values: at trial-level boundary
    void Trial_NoiseInit(LeabraNetwork* net)
    { ((LeabraUnitSpec*)GetUnitSpec())->Trial_NoiseInit(this, net); }
    // #CAT_Activation initialize noise at trial level
    void Trial_Init_SRAvg(LeabraNetwork* net)
    { ((LeabraUnitSpec*)GetUnitSpec())->Trial_Init_SRAvg(this, net); }	  
    // #CAT_Learning reset the sender-receiver coproduct average -- do at start of trial

  ///////////////////////////////////////////////////////////////////////
  //	SettleInit -- at start of settling

  void	Settle_Init_Unit(LeabraNetwork* net, int thread_no=-1)
  { ((LeabraUnitSpec*)GetUnitSpec())->Settle_Init_Unit(this, net, thread_no); }
  // #CAT_Activation settle unit-level initialization functions: Init_TargFlags, DecayState, NetinScale

    void Settle_Init_TargFlags(LeabraNetwork* net)
    { ((LeabraUnitSpec*)GetUnitSpec())->Settle_Init_TargFlags(this, net); }
    // #CAT_Activation initialize external input flags based on phase
    void Settle_DecayState(LeabraNetwork* net, int)
    { ((LeabraUnitSpec*)GetUnitSpec())->Settle_DecayState(this, net); }
    // #CAT_Activation decay activation states towards initial values: at phase-level boundary at start of settling
    void Compute_NetinScale(LeabraNetwork* net, int)
    { ((LeabraUnitSpec*)GetUnitSpec())->Compute_NetinScale(this, net); }
    // #CAT_Activation compute net input scaling values

  void Compute_NetinScale_Senders(LeabraNetwork* net, int)
  { ((LeabraUnitSpec*)GetUnitSpec())->Compute_NetinScale_Senders(this, net); }
  // #CAT_Activation compute net input scaling values for sending cons -- copies from values computed in the recv guys -- has to be done as a second phase of the Settle_Init_Unit stage after all the recv ones are computed

  void Compute_HardClamp(LeabraNetwork* net) 
  { ((LeabraUnitSpec*)GetUnitSpec())->Compute_HardClamp(this, net); }
    // #CAT_Activation force units to external values provided by environment: also optimizes settling by only computing netinput once
  void	Compute_HardClampNoClip(LeabraNetwork* net) 
  { ((LeabraUnitSpec*)GetUnitSpec())->Compute_HardClampNoClip(this, net); }
  // #CAT_Activation hard-clamp units without clipping values to clamp_range (use for second plus phase clamping to settled values)

  void	ExtToComp(LeabraNetwork* net)
  { ((LeabraUnitSpec*)GetUnitSpec())->ExtToComp(this, net); }
  // #CAT_Activation change external inputs to comparisons (remove input)
  void	TargExtToComp(LeabraNetwork* net)
  { ((LeabraUnitSpec*)GetUnitSpec())->TargExtToComp(this, net); }
  // #CAT_Activation change target & external inputs to comparisons (remove targ & input)

  ///////////////////////////////////////////////////////////////////////
  //	Cycle Step 1: Netinput 

  void	Send_NetinDelta(LeabraNetwork* net, int thread_no=-1)
  { ((LeabraUnitSpec*)GetUnitSpec())->Send_NetinDelta(this, net, thread_no); }
  // #CAT_Activation send netinput; sender based and only when act changes above a threshold -- only this delta form is supported
  void	Compute_NetinInteg(LeabraNetwork* net, int thread_no=-1)
  { ((LeabraUnitSpec*)GetUnitSpec())->Compute_NetinInteg(this, net, thread_no); }
  // #CAT_Activation integrate newly-computed netinput delta values into a resulting complete netinput value for the network (does both excitatory and inhibitory)
  float Compute_IThresh(LeabraNetwork* net)
  { return ((LeabraUnitSpec*)GetUnitSpec())->Compute_IThresh(this, net); }
  // #CAT_Activation called by Compute_NetinInteg: compute inhibitory value that would place unit directly at threshold

  ///////////////////////////////////////////////////////////////////////
  //	Cycle Step 2: Inhibition

  void	Compute_ApplyInhib(LeabraNetwork* net, float inhib_val)
  { ((LeabraUnitSpec*)GetUnitSpec())->Compute_ApplyInhib(this, net, inhib_val); }
  // #CAT_Activation apply computed inhibitory value (kwta) to unit inhibitory conductance

  ///////////////////////////////////////////////////////////////////////
  //	Cycle Step 3: Activation

  // main function is basic Compute_Act which calls a bunch of sub-functions on the unitspec

  float Compute_ActValFmVmVal_rate(float vm_val)
  { return ((LeabraUnitSpec*)GetUnitSpec())->Compute_ActValFmVmVal_rate(vm_val); }
  // #CAT_Activation raw activation function: computes rate-coded activation value given membrane potential value based on current activation function -- does not update state variables or anything

  ///////////////////////////////////////////////////////////////////////
  //	Cycle Stats

  float	Compute_MaxDa(LeabraNetwork* net) 
  { return ((LeabraUnitSpec*)GetUnitSpec())->Compute_MaxDa(this, net); }
  // #CAT_Activation compute the maximum delta-activation (change in activation); used to control settling

  ///////////////////////////////////////////////////////////////////////
  //	Cycle Optional Misc

  void	Compute_CycSynDep(LeabraNetwork* net, int thread_no=-1)
  { ((LeabraUnitSpec*)GetUnitSpec())->Compute_CycSynDep(this, net, thread_no); }
  // #CAT_Activation compute cycle-level synaptic depression (must be defined by appropriate subclass) -- called at end of each cycle of computation if net_misc.cyc_syn_dep is on.

  ///////////////////////////////////////////////////////////////////////
  //	Settle Final

  void	PostSettle(LeabraNetwork* net)
  { ((LeabraUnitSpec*)GetUnitSpec())->PostSettle(this, net); }
  // #CAT_Activation set stuff after settling is over (act_m, act_p etc)

  ///////////////////////////////////////////////////////////////////////
  //	Trial Final

  void 	EncodeState(LeabraNetwork* net)
  { ((LeabraUnitSpec*)GetUnitSpec())->EncodeState(this, net); }
  // #CAT_Learning encode current state information at end of trial (hook for time-based learning)
  void 	Compute_SelfReg_Trial(LeabraNetwork* net)
  { ((LeabraUnitSpec*)GetUnitSpec())->Compute_SelfReg_Trial(this, net); }
  // #CAT_Activation compute self-regulation (accommodation, hysteresis) at end of trial


  ///////////////////////////////////////////////////////////////////////
  //	Learning

  void	Compute_SRAvg(LeabraNetwork* net, int thread_no=-1)
  { ((LeabraUnitSpec*)GetUnitSpec())->Compute_SRAvg(this, net, thread_no); }
  // #CAT_Learning compute sending-receiving activation product averages (CtLeabra_X/CAL)

  void 	Compute_dWt_FirstPlus(LeabraNetwork* net, int thread_no=-1)
  { ((LeabraUnitSpec*)GetUnitSpec())->Compute_dWt_FirstPlus(this, net, thread_no); }
  // #CAT_Learning compute weight change after first plus phase has been encountered: standard layers do a weight change here, except under CtLeabra_X/CAL
  void 	Compute_dWt_SecondPlus(LeabraNetwork* net, int thread_no=-1)
  { ((LeabraUnitSpec*)GetUnitSpec())->Compute_dWt_SecondPlus(this, net, thread_no); }
  // #CAT_Learning compute weight change after second plus phase has been encountered: standard layers do NOT do a weight change here -- only selected special ones
  void 	Compute_dWt_Nothing(LeabraNetwork* net, int thread_no=-1)
  { ((LeabraUnitSpec*)GetUnitSpec())->Compute_dWt_Nothing(this, net, thread_no); }
  // #CAT_Learning compute weight change after final nothing phase: standard layers do a weight change here under both learning rules


  ///////////////////////////////////////////////////////////////////////
  //	Stats

  float	Compute_NormErr(LeabraNetwork* net)
  { return ((LeabraUnitSpec*)GetUnitSpec())->Compute_NormErr(this, net); }
  // #CAT_Learning compute normalized binary trial-wise error

  ///////////////////////////////////////////////////////////////////////
  //	Misc Housekeeping, non Compute functions

  void	GetInSubGp();

  void	InitLinks();
  void	Copy_(const LeabraUnit& cp);
  TA_BASEFUNS(LeabraUnit);
private:
  void 	Initialize();
  void	Destroy()		{ };
};

#ifndef __MAKETA__
typedef void (LeabraUnit::*LeabraUnitMethod)(LeabraNetwork*, int);
// this is required to disambiguate unit thread method guys -- double casting
#endif 

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
  virtual void	CheckInhibCons(LeabraNetwork* net);
  // #CAT_Structure check for inhibitory connections -- sets flag on network

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

class LEABRA_API AvgMaxVals : public taOBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra holds average and max statistics
INHERITED(taOBase)
public:
  bool		cmpt;		// whether to compute these values or not -- only for optional values
  float		avg;		// #DMEM_AGG_SUM average value
  float		max;		// #DMEM_AGG_SUM maximum value
  int 		max_i;		// index of unit with maximum value

  inline void	InitVals()	{ avg = 0.0f; max = -FLT_MAX; max_i = -1; }
  inline void	UpdtVals(float val, int idx)
  { avg += val; if(val > max) { max = val; max_i = idx; } }
  inline void	CalcAvg(int n) { if(n > 0) avg /= (float)n; }

  inline void	UpdtFmAvgMax(const AvgMaxVals& oth, int gpn, int idx)
  { avg += oth.avg * (float)gpn; if(oth.max > max) { max = oth.max; max_i = idx; } }
  
  void	Copy_(const AvgMaxVals& cp);
  TA_BASEFUNS(AvgMaxVals);
private:
  void	Initialize();
  void 	Destroy()	{ };
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

  InhibType	type;		// how to compute inhibition (g_i)
  float		kwta_pt;	// #DEF_0.25;0.6;0.2 [Default: .25 for KWTA_INHIB and KWTA_KV2K, .6 for KWTA_AVG, .2 for AVG_MAX_PT_INHIB] point to place inhibition between k and k+1 (or avg and max for AVG_MAX_PT_INHIB)
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

  K_From	k_from;		// how is the active_k determined: directly by k, by pct, or by no. of units where ext > pat_q
  int		k;		// #CONDEDIT_ON_k_from:USE_K desired number of active units in the layer
  float		pct;		// #CONDEDIT_ON_k_from:USE_PCT desired proportion of activity (used to compute a k value based on layer size, .25 std)
  float		pat_q;		// #HIDDEN #DEF_0.5 threshold for pat_k based activity level: add to k if ext > pat_q
  bool		diff_act_pct;	// #DEF_false if true, use different actual percent activity for overall layer activation
  float		act_pct;	// #CONDEDIT_ON_diff_act_pct:true actual percent activity to put in kwta.pct field of layer
  bool		gp_i;		// compute inhibition including all of the layers in the same group, or unit groups within the layer: each items computed inhib vals are multipled by gp_g scaling, then MAX'd, and each item's inhib is the MAX of this pooled MAX value and its original own value
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
  bool		on;		// whether to perform the tie breaking function at all
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

  AdaptType	type;		// what to adapt, or none for nothing
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
  bool		hard;		// #DEF_true whether to hard clamp inputs where activation is directly set to external input value (act = ext, computed once at start of settle) or do soft clamping where ext is added into net input (net += gain * ext)
  float		gain;		// #CONDEDIT_OFF_hard:true #DEF_0.2;0.5 soft clamp gain factor (net += gain * ext)

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

class LEABRA_API LayAbsNetAdaptSpec : public taOBase {
  // ##INLINE ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra adapt absolute netinput values by adjusting the wt_scale.abs parameters in the conspecs of projections into this layer, based on differences between time-averaged max netinput values and the target
INHERITED(taOBase)
public:
  bool		on;		// whether to apply layer netinput rescaling
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

  InhibGroup	inhib_group;	// #CAT_Activation what to consider the inhibitory group (layer or unit subgroups, or both)
  LeabraInhibSpec inhib;	// #CAT_Activation how to compute inhibition -- for kwta modes, a single global inhibition value is computed for the entire layer
  KWTASpec	kwta;		// #CONDEDIT_OFF_inhib_group:UNIT_GROUPS #CAT_Activation desired activity level over entire layer (NOTE: used to set target activity for UNIT_INHIB, AVG_MAX_PT_INHIB, but not used for actually computing inhib for these cases)
  KWTASpec	gp_kwta;	// #CONDEDIT_OFF_inhib_group:ENTIRE_LAYER #CAT_Activation desired activity level for units within unit groups (not for ENTIRE_LAYER) (NOTE: used to set target activity for UNIT_INHIB, AVG_MAX_PT_INHIB, but not used for actually computing inhib for these cases)
  KwtaTieBreak	tie_brk;	// #CAT_Activation break ties when all the units in the layer have similar netinputs, which puts the inhbition value too close to everyone's threshold and produces no activation at all.  this will lower the inhibition and allow all the units to have some activation
  AdaptISpec	adapt_i;	// #CAT_Activation adapt the inhibition: either i_kwta_pt point based on diffs between actual and target k level (for avg-based), or g_bar.i for unit-inhib
  ClampSpec	clamp;		// #CAT_Activation how to clamp external inputs to units (hard vs. soft)
  DecaySpec	decay;		// #CAT_Activation decay of activity state vars between events, -/+ phase, and 2nd set of phases (if appl)
  CtLayerInhibMod  ct_inhib_mod; // layer-level inhibitory modulation parameters, to be used instead of network-level values where needed
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

  ///////////////////////////////////////////////////////////////////////
  //	General Init functions

  virtual void BuildUnits_Threads(LeabraLayer* lay, LeabraNetwork* net);
  // #IGNORE build unit-level thread information: flat list of units, etc -- this is called by network BuildUnits_Threads so that layers (and layerspecs) can potentially modify which units get added to the compute lists, and thus which are subject to standard computations -- default is all units in the layer

  virtual void	SetLearnRule(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_Learning set current learning rule from the network

  virtual void	Init_Weights(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_Learning initialize weight values and other permanent state
    virtual void Init_Inhib(LeabraLayer* lay, LeabraNetwork* net);
    // #CAT_Activation called in Init_Weights initialize the inhibitory state values
    virtual void Init_Stats(LeabraLayer* lay, LeabraNetwork* net);
    // #CAT_Statistic called in Init_Weights intialize statistic variables

  virtual void	Init_Acts(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_Activation initialize unit-level dynamic state variables (activations, etc)

  virtual void	Init_ActAvg(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_Activation initialize act_avg values

  virtual void 	DecayState(LeabraLayer* lay, LeabraNetwork* net, float decay);
  // #CAT_Activation decay activation states towards initial values by given amount (0 = no decay, 1 = full decay)

  ///////////////////////////////////////////////////////////////////////
  //	TrialInit -- at start of trial

  virtual void	SetCurLrate(LeabraLayer* lay, LeabraNetwork* net, int epoch);
  // #CAT_Learning set current learning rate based on epoch -- goes through projections
  virtual void	Trial_Init_Layer(LeabraLayer* lay, LeabraNetwork* net) { };
  // #CAT_Learning layer level trial init -- overload where needed

    virtual void	Trial_DecayState(LeabraLayer* lay, LeabraNetwork* net);
    // #CAT_Activation NOT CALLED DURING STD PROCESSING decay activations and other state between events
    virtual void	Trial_NoiseInit(LeabraLayer* lay, LeabraNetwork* net);
    // #CAT_Activation NOT CALLED DURING STD PROCESSING initialize various noise factors at start of trial
    virtual void	Trial_Init_SRAvg(LeabraLayer* lay, LeabraNetwork* net);
    // #CAT_Learning NOT CALLED DURING STD PROCESSING reset the sender-receiver coproduct average (CtLeabra_X/CAL)

  ///////////////////////////////////////////////////////////////////////
  //	SettleInit -- at start of settling

  virtual void	Compute_Active_K(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_Activation prior to settling: compute actual activity levels based on spec, inputs, etc
    virtual void Compute_Active_K_ugp(LeabraLayer* lay, Unit_Group* ug,
				      LeabraInhib* thr, KWTASpec& kwtspec);
    // #IGNORE unit gp version
    virtual int	Compute_Pat_K(LeabraLayer* lay, Unit_Group* ug, LeabraInhib* thr);
    // #IGNORE PAT_K compute

  virtual void	Settle_Init_Layer(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_Activation initialize start of a setting phase: all layer-level misc init takes place here (calls TargFlags_Layer) -- other stuff all done directly in Settle_Init_Units call

  virtual void	Settle_Init_TargFlags(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_Activation initialize start of a setting phase, set input flags appropriately, etc
    virtual void Settle_Init_TargFlags_Layer(LeabraLayer* lay, LeabraNetwork* net);
    // #IGNORE layer-level initialize start of a setting phase, set input flags appropriately, etc
  virtual void	Compute_HardClamp(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_Activation prior to settling: hard-clamp inputs
  virtual void	Compute_HardClampPhase2(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_Activation prior to settling: hard-clamp inputs (special code for hard clamping in phase 2 based on prior acts)

  virtual void	ExtToComp(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_Activation change external inputs to comparisons (remove input)
  virtual void	TargExtToComp(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_Activation change target & external inputs to comparisons (remove targ & input)


  ///////////////////////////////////////////////////////////////////////
  //	Cycle Step 1: Netinput 

  // main computation is direct Send_NetinDelta call on units through threading mechanism
  // followed by Compute_NetinInteg on units

  virtual void	Compute_NetinStats(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_Activation compute AvgMax stats on netin and i_thr values computed during netin computation -- used for various regulatory and monitoring functions
    virtual void Compute_NetinStats_ugp(Unit_Group* ug, LeabraInhib* thr);
    // #IGNORE compute AvgMax stats on netin and i_thr values computed during netin computation -- per unit group

  ///////////////////////////////////////////////////////////////////////
  //	Cycle Step 2: Inhibition

  virtual void	Compute_Inhib(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_Activation compute the inhibition for layer -- this is the main call point into this stage of processing
    virtual void Compute_Inhib_impl(LeabraLayer* lay, Unit_Group* ug, LeabraInhib* thr,
				   LeabraNetwork* net, LeabraInhibSpec& ispec);
    // #IGNORE implementation of inhibition computation for either layer or unit group

    virtual void Compute_Inhib_kWTA_Sort(Unit_Group* ug, LeabraInhib* thr, LeabraSort& act_buf,
					LeabraSort& inact_buf, int k_eff, float& k_net, int& k_idx);
    // #CAT_Activation implementation of sort into active and inactive unit buffers -- basic to various kwta functions: eff_k = effective k to use, k_net = net of kth unit (lowest unit in act_buf), k_idx = index of kth unit
    virtual void Compute_Inhib_BreakTie(LeabraInhib* thr);
    // #IGNORE break any ties in the kwta function -- called by specific kwta functions, and depends on tie_brk.on

    virtual void Compute_Inhib_kWTA(LeabraLayer* lay, Unit_Group* ug, LeabraInhib* thr,
				   LeabraNetwork* net, LeabraInhibSpec& ispec);
    // #IGNORE implementation of basic kwta inhibition computation
    virtual void Compute_Inhib_kWTA_Avg(LeabraLayer* lay, Unit_Group* ug, LeabraInhib* thr,
				       LeabraNetwork* net, LeabraInhibSpec& ispec);
    // #IGNORE implementation of kwta avg-based inhibition computation
    virtual void Compute_Inhib_kWTA_kv2k(LeabraLayer* lay, Unit_Group* ug, LeabraInhib* thr,
						LeabraNetwork* net, LeabraInhibSpec& ispec);
    // #IGNORE implementation of k vs. 2k wta avg-based inhibition computation
    virtual void Compute_Inhib_kWTA_CompCost(LeabraLayer* lay, Unit_Group* ug, LeabraInhib* thr,
						    LeabraNetwork* net, LeabraInhibSpec& ispec);
    // #IGNORE implementation of kwta competitor cost inhibition computation
    virtual void Compute_Inhib_AvgMaxPt(LeabraLayer* lay, Unit_Group* ug, LeabraInhib* thr,
					       LeabraNetwork* net, LeabraInhibSpec& ispec);
    // #IGNORE implementation of avg-max-pt inhibition computation
    virtual void Compute_Inhib_Max(LeabraLayer* lay, Unit_Group* ug, LeabraInhib* thr,
					  LeabraNetwork* net, LeabraInhibSpec& ispec);
    // #IGNORE implementation of max inhibition computation
    virtual void Compute_Inhib_kWTA_Gps(LeabraLayer* lay, LeabraNetwork* net,
					       LeabraInhibSpec& ispec);
    // #IGNORE implementation of GPS_THEN_UNITS kwta on groups

    virtual void Compute_CtDynamicInhib(LeabraLayer* lay, LeabraNetwork* net);
    // #IGNORE compute extra dynamic inhibition for CtLeabra_X/CAL algorithm

  virtual void	Compute_LayInhibToGps(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_Activation Stage 2.2: for layer groups, need to propagate inhib out to unit groups

  virtual void	Compute_ApplyInhib(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_Activation Stage 2.3: apply computed inhib value to individual unit inhibitory conductances
    virtual void Compute_ApplyInhib_ugp(LeabraLayer* lay, Unit_Group* ug,
					       LeabraInhib* thr, LeabraNetwork* net);
    // #IGNORE unit-group apply inhibition computation

  ///////////////////////////////////////////////////////////////////////
  //	Cycle Step 3: Activation

  // main function is basic Compute_Act which calls a bunch of sub-functions on the unitspec
  // called directly on units through threading mechanism

  ///////////////////////////////////////////////////////////////////////
  //	Cycle Stats

  virtual void	Compute_CycleStats(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_Statistic compute cycle-level stats -- acts AvgMax, MaxDa, OutputName, etc
  // this does all the indented functions below

    virtual void Compute_AvgMaxVals_ugp(LeabraLayer* lay, Unit_Group* ug, AvgMaxVals& vals, ta_memb_ptr mb_off);
    // #IGNORE utility to compute avg max vals for units in group, with member offset mb_off from unit
    virtual void Compute_Acts_AvgMax(LeabraLayer* lay, LeabraNetwork* net);
    // #CAT_Statistic compute activation AvgMaxVals (acts)

    virtual void Compute_MaxDa(LeabraLayer* lay, LeabraNetwork* net);
    // #CAT_Activation compute maximum delta-activation in layer (used for stopping criterion)
      virtual void Compute_MaxDa_ugp(LeabraLayer* lay, Unit_Group* ug, LeabraInhib* thr,
				     LeabraNetwork* net);
      // #IGNORE unit group compute maximum delta-activation

    virtual void Compute_OutputName(LeabraLayer* lay, LeabraNetwork* net);
    // #CAT_Statistic compute the output_name field from the layer acts.max_i (only for OUTPUT or TARGET layers)
      virtual void Compute_OutputName_ugp(LeabraLayer* lay, Unit_Group* ug, LeabraInhib* thr, LeabraNetwork* net);
      // #IGNORE compute the output_name field from the layer acts.max_i (only for OUTPUT or TARGET layers)

    virtual void Compute_UnitInhib_AvgMax(LeabraLayer* lay, LeabraNetwork* net);
    // #CAT_Statistic compute unit inhibition AvgMaxVals (un_g_i)

  ///////////////////////////////////////////////////////////////////////
  //	Cycle Stats -- optional non-default guys

  virtual float	Compute_TopKAvgAct(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_Statistic compute the average activation of the top k most active units (useful as a measure of recognition) -- requires a kwta inhibition function to be in use, and operates on current act_eq values
    virtual float Compute_TopKAvgAct_ugp(LeabraLayer* lay, Unit_Group* ug, LeabraInhib* thr,
					 LeabraNetwork* net);
    // #IGNORE ugp version

  virtual float	Compute_TopKAvgNetin(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_Statistic compute the average net input of the top k most active units (useful as a measure of recognition) -- requires a kwta inhibition function to be in use, and operates on current net values
    virtual float Compute_TopKAvgNetin_ugp(LeabraLayer* lay, Unit_Group* ug, LeabraInhib* thr,
					   LeabraNetwork* net);
    // #IGNORE ugp version

  ///////////////////////////////////////////////////////////////////////
  //	Cycle Optional Misc

  virtual void	Compute_MidMinus(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_Activation do special processing midway through the minus phase, as determined by the mid_minus_cycle parameter, if > 0 -- currently used for the PBWM algorithm -- stores act_m2

  ///////////////////////////////////////////////////////////////////////
  //	SettleFinal

  virtual void	PostSettle(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_Activation after settling, keep track of phase variables, etc.
    virtual void PostSettle_GetMinus(LeabraLayer* lay, LeabraNetwork* net);
    // #CAT_Activation get minus phase act stats
    virtual void PostSettle_GetPlus(LeabraLayer* lay, LeabraNetwork* net);
    // #CAT_Activation get plus phase act stats
    virtual void PostSettle_GetMinus2(LeabraLayer* lay, LeabraNetwork* net);
    // #CAT_Activation get 2nd minus phase act stats
    virtual void PostSettle_GetPlus2(LeabraLayer* lay, LeabraNetwork* net);
    // #CAT_Activation get 2nd plus phase act stats
    virtual void PostSettle_GetPhaseDifRatio(LeabraLayer* lay, LeabraNetwork* net);
    // #CAT_Activation get phase dif ratio from minus to plus
    virtual void AdaptGBarI(LeabraLayer* lay, LeabraNetwork* net);
    // #CAT_Activation adapt inhibitory conductances based on target activation values relative to current values

  virtual void	Compute_ActM_AvgMax(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_Activation compute acts_m AvgMaxVals from act_m -- not currently used
  virtual void	Compute_ActP_AvgMax(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_Activation compute acts_p AvgMaxVals from act_p -- not currently used

  ///////////////////////////////////////////////////////////////////////
  //	TrialFinal

  virtual void	EncodeState(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_Learning encode final state information at end of trial for time-based learning across trials
  virtual void	Compute_SelfReg_Trial(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_Activation update self-regulation (accommodation, hysteresis) at end of trial

  ///////////////////////////////////////////////////////////////////////
  //	Learning

  virtual void	AdaptKWTAPt(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_Activation adapt the kwta point based on average activity

  virtual void	Compute_dWt_Layer_pre(LeabraLayer* lay, LeabraNetwork* net) { };
  // #CAT_Learning do special computations at layer level prior to standard unit-level thread dwt computation -- not used in base class but is in various derived classes

  virtual bool	Compute_SRAvg_Test(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_Learning test whether to compute sravg values -- default is true, but some layers might opt out for various reasons
  virtual bool	Compute_dWt_FirstPlus_Test(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_Learning test whether to compute weight change after first plus phase has been encountered: standard layers do a weight change here, except under CtLeabra_X/CAL
  virtual bool	Compute_dWt_SecondPlus_Test(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_Learning test whether to compute weight change after second plus phase has been encountered: standard layers do NOT do a weight change here -- only selected special ones
  virtual bool	Compute_dWt_Nothing_Test(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_Learning compute weight change after final nothing phase: standard layers do a weight change here under both learning rules

  ///////////////////////////////////////////////////////////////////////
  //	Trial-level Stats

  virtual float	Compute_SSE(LeabraLayer* lay, LeabraNetwork* net,
			    int& n_vals, bool unit_avg = false, bool sqrt = false);
  // #CAT_Statistic compute sum squared error of activation vs target over the entire layer -- always returns the actual sse, but unit_avg and sqrt flags determine averaging and sqrt of layer's own sse value

  virtual float	Compute_NormErr_ugp(LeabraLayer* lay, Unit_Group* ug, LeabraInhib* thr,
				    LeabraNetwork* net);
  // #CAT_Statistic compute normalized binary error for given unit group -- just gets the raw sum over unit group
  virtual float	Compute_NormErr(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_Statistic compute normalized binary error -- layer-level value is already normalized, and network just averages across the layers (each layer contributes equally to overal normalized value, instead of contributing in proportion to number of units) -- returns -1 if not an err target defined in same way as sse

  ////////////////////////////////////////////////////////////////////////////////
  //	Parameter Adaptation over longer timesales

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
  override bool CheckConfig_Layer(Layer* lay, bool quiet=false);
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
  AvgMaxVals	un_g_i;		// #READ_ONLY #EXPERT #CAT_Activation unit inhib values (optionally computed)
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
  bool		hard_clamped;	// #READ_ONLY #SHOW #CAT_Activation this layer is actually hard clamped
  float		ravg_l_avg;	// #READ_ONLY #EXPERT #CAT_Activation layer-wise average of ravg_l values in the layers
  float		dav;		// #READ_ONLY #EXPERT #CAT_Learning dopamine-like modulatory value (where applicable)
  AvgMaxVals	avg_netin;	// #READ_ONLY #EXPERT #CAT_Activation net input values for the layer, averaged over an epoch-level timescale
  AvgMaxVals	avg_netin_sum;	// #READ_ONLY #EXPERT #CAT_Activation #DMEM_AGG_SUM sum of net input values for the layer, for computing average over an epoch-level timescale
  int		avg_netin_n;	// #READ_ONLY #EXPERT #CAT_Activation #DMEM_AGG_SUM number of times sum is updated for computing average
  float		norm_err;	// #GUI_READ_ONLY #SHOW #CAT_Statistic normalized binary error value for this layer, computed subject to the parameters on the network
  int		da_updt;	// #READ_ONLY #EXPERT #CAT_Learning true if da triggered an update (either + to store or - reset)

#ifdef DMEM_COMPILE
  DMemAggVars	dmem_agg_sum;		// #IGNORE aggregation of network variables using SUM op (currently only OP in use -- add others as needed)
  virtual void 	DMem_InitAggs();
  // #IGNORE initialize aggregation stuff
  virtual void	DMem_ComputeAggs(MPI_Comm comm);
  // #IGNORE aggregate network variables across procs for trial-level dmem 
#endif

  override void CheckSpecs();
  override void	BuildUnits();
  override void BuildUnits_Threads(Network* net)
  { if(spec) spec->BuildUnits_Threads(this, (LeabraNetwork*)net); }

  ///////////////////////////////////////////////////////////////////////
  //	General Init functions

  void	SetLearnRule(LeabraNetwork* net) 	{ if(spec) spec->SetLearnRule(this, net); }
  // #CAT_Learning set current learning rule from the network

  override void	Init_Weights(Network* net)
  { if(spec) spec->Init_Weights(this, (LeabraNetwork*)net); }
  // #CAT_Learning initialize weight values and other permanent state

  void	Init_ActAvg(LeabraNetwork* net) 	{ spec->Init_ActAvg(this, net); }
  // #CAT_Activation initialize act_avg values

  override void	Init_Acts(Network* net)
  { if(spec) spec->Init_Acts(this, (LeabraNetwork*)net); }
  // #CAT_Activation initialize unit-level dynamic state variables (activations, etc)

  void	DecayState(LeabraNetwork* net, float decay) { spec->DecayState(this, net, decay); }
  // #CAT_Activation decay activation states towards initial values by given amount (0 = no decay, 1 = full decay)

  void	CheckInhibCons(LeabraNetwork* net);
  // #CAT_Structure check for inhibitory connections -- sets flag on network
  
  ///////////////////////////////////////////////////////////////////////
  //	TrialInit -- at start of trial

  void	SetCurLrate(LeabraNetwork* net, int epoch) { spec->SetCurLrate(this, net, epoch); }
  // #CAT_Learning set current learning rate based on epoch
  void	Trial_Init_Layer(LeabraNetwork* net) { spec->Trial_Init_Layer(this, net); }
  // #CAT_Learning layer-level trial init

    void	Trial_DecayState(LeabraNetwork* net)	{ spec->Trial_DecayState(this, net); }
    // #CAT_Activation NOT CALLED DURING STD PROCESSING decay activations and other state between events
    void	Trial_NoiseInit(LeabraNetwork* net)	{ spec->Trial_NoiseInit(this, net); }
    // #CAT_Activation NOT CALLED DURING STD PROCESSING initialize various noise factors at start of trial
    void 	Trial_Init_SRAvg(LeabraNetwork* net)   	{ spec->Trial_Init_SRAvg(this, net); }
    // #CAT_Learning NOT CALLED DURING STD PROCESSING initialize sending-receiving activation product averages (CtLeabra_X/CAL)

  ///////////////////////////////////////////////////////////////////////
  //	SettleInit -- at start of settling

  void	Compute_Active_K(LeabraNetwork* net)	{ spec->Compute_Active_K(this, net); }
  // #CAT_Activation prior to settling: compute actual activity levels based on spec, inputs, etc

  void	Settle_Init_Layer(LeabraNetwork* net)	{ spec->Settle_Init_Layer(this, net); }
  // #CAT_Activation initialize start of a setting phase: all layer-level misc init takes place here (calls TargFlags_Layer) -- other stuff all done directly in Settle_Init_Units call

  void	Settle_Init_TargFlags(LeabraNetwork* net) { spec->Settle_Init_TargFlags(this, net); }
  // #CAT_Activation initialize start of a setting phase, set input flags appropriately, etc
    void	Settle_Init_TargFlags_Layer(LeabraNetwork* net)
    { spec->Settle_Init_TargFlags_Layer(this, net); }
    // #IGNORE layer-level initialize start of a setting phase, set input flags appropriately, etc
  void	Compute_HardClamp(LeabraNetwork* net) 	{ spec->Compute_HardClamp(this, net); }
  // #CAT_Activation prior to settling: hard-clamp inputs

  void	ExtToComp(LeabraNetwork* net)		{ spec->ExtToComp(this, net); }
  // #CAT_Activation change external inputs to comparisons (remove input)
  void	TargExtToComp(LeabraNetwork* net)	{ spec->TargExtToComp(this, net); }
  // #CAT_Activation change target & external inputs to comparisons (remove targ & input)

  ///////////////////////////////////////////////////////////////////////
  //	Cycle Step 1: Netinput 

  // main computation is direct Send_NetinDelta call on units through threading mechanism

  void	Compute_NetinStats(LeabraNetwork* net)  { spec->Compute_NetinStats(this, net); }
  // #CAT_Activation compute AvgMax stats on netin and i_thr values computed during netin computation -- used for various regulatory and monitoring functions

  ///////////////////////////////////////////////////////////////////////
  //	Cycle Step 2: Inhibition

  void	Compute_Inhib(LeabraNetwork* net) 	{ spec->Compute_Inhib(this, net); }
  // #CAT_Activation compute the inhibition for layer
  void	Compute_LayInhibToGps(LeabraNetwork* net) { spec->Compute_LayInhibToGps(this, net); }
  // #CAT_Activation Stage 2.2: for layer groups, need to propagate inhib out to unit groups
  void	Compute_ApplyInhib(LeabraNetwork* net)	{ spec->Compute_ApplyInhib(this, net); }
  // #CAT_Activation Stage 2.3: apply computed inhib value to individual unit inhibitory conductances

  ///////////////////////////////////////////////////////////////////////
  //	Cycle Step 3: Activation

  // main function is basic Compute_Act which calls a bunch of sub-functions on the unitspec
  // called directly on units through threading mechanism

  ///////////////////////////////////////////////////////////////////////
  //	Cycle Stats

  void	Compute_CycleStats(LeabraNetwork* net)
  { return spec->Compute_CycleStats(this, net); }
  // #CAT_Statistic compute cycle-level stats -- acts AvgMax, MaxDa, OutputName, etc

  ///////////////////////////////////////////////////////////////////////
  //	Cycle Stats -- optional non-default guys

  float	Compute_TopKAvgAct(LeabraNetwork* net)  { return spec->Compute_TopKAvgAct(this, net); }
  // #CAT_Statistic compute the average activation of the top k most active units (useful as a measure of recognition) -- requires a kwta inhibition function to be in use, and operates on current act_eq values
  float	Compute_TopKAvgNetin(LeabraNetwork* net)  { return spec->Compute_TopKAvgNetin(this, net); }
  // #CAT_Statistic compute the average netinput of the top k most active units (useful as a measure of recognition) -- requires a kwta inhibition function to be in use, and operates on current act_eq values

  ///////////////////////////////////////////////////////////////////////
  //	Cycle Optional Misc

  void	Compute_MidMinus(LeabraNetwork* net)	{ spec->Compute_MidMinus(this, net); }
  // #CAT_Activation do special processing midway through the minus phase, as determined by the mid_minus_cycle parameter, if > 0 -- currently used for the PBWM algorithm

  ///////////////////////////////////////////////////////////////////////
  //	SettleFinal

  void	PostSettle(LeabraNetwork* net)	{ spec->PostSettle(this, net); }
  // #CAT_Activation after settling, keep track of phase variables, etc.

  void	Compute_ActM_AvgMax(LeabraNetwork* net) { spec->Compute_ActM_AvgMax(this, net); }
  // #CAT_Activation compute acts_m.avg from act_m
  void	Compute_ActP_AvgMax(LeabraNetwork* net) { spec->Compute_ActP_AvgMax(this, net); }
  // #CAT_Activation compute acts_p.avg from act_p

  ///////////////////////////////////////////////////////////////////////
  //	TrialFinal

  void	EncodeState(LeabraNetwork* net)		{ spec->EncodeState(this, net); }
  // #CAT_Learning encode final state information at end of trial for time-based learning across trials
  void	Compute_SelfReg_Trial(LeabraNetwork* net) { spec->Compute_SelfReg_Trial(this, net); }
  // #CAT_Activation update self-regulation (accommodation, hysteresis) at end of trial


  ///////////////////////////////////////////////////////////////////////
  //	Learning

  bool	Compute_SRAvg_Test(LeabraNetwork* net)
  { return spec->Compute_SRAvg_Test(this, net); }
  // #CAT_Learning test whether to compute sravg values -- default is true, but some layers might opt out for various reasons

  void	Compute_dWt_Layer_pre(LeabraNetwork* net)  { spec->Compute_dWt_Layer_pre(this, net); }
  // #CAT_Learning do special computations at layer level prior to standard unit-level thread dwt computation -- not used in base class but is in various derived classes

  bool	Compute_dWt_FirstPlus_Test(LeabraNetwork* net)
  { return spec->Compute_dWt_FirstPlus_Test(this, net); }
  // #CAT_Learning test whether to compute weight change after first plus phase has been encountered: standard layers do a weight change here, except under CtLeabra_X/CAL
  bool	Compute_dWt_SecondPlus_Test(LeabraNetwork* net)
  { return spec->Compute_dWt_SecondPlus_Test(this, net); }
  // #CAT_Learning test whether to compute weight change after second plus phase has been encountered: standard layers do NOT do a weight change here -- only selected special ones
  bool	Compute_dWt_Nothing_Test(LeabraNetwork* net)
  { return spec->Compute_dWt_Nothing_Test(this, net); }
  // #CAT_Learning test whether to compute weight change after final nothing phase: standard layers do a weight change here under both learning rules

  ///////////////////////////////////////////////////////////////////////
  //	Trial-level Stats

  override float Compute_SSE(Network* net, int& n_vals,
			     bool unit_avg = false, bool sqrt = false)
  { return spec->Compute_SSE(this, (LeabraNetwork*)net, n_vals, unit_avg, sqrt); }

  float Compute_NormErr(LeabraNetwork* net)
  { return spec->Compute_NormErr(this, net); }
  // #CAT_Statistic compute normalized binary error across layer (returns normalized value or -1 for not applicable, averaged at network level -- see layerspec for more info)

  ////////////////////////////////////////////////////////////////////////////////
  //	Parameter Adaptation over longer timesales

  void	AdaptKWTAPt(LeabraNetwork* net) { spec->AdaptKWTAPt(this, net); }
  // #CAT_Activation adapt the kwta point based on average activity

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

  ////////////////////////////////////////////
  //	Misc structural routines

  virtual void	ResetSortBuf();
  override void	TriggerContextUpdate();

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
  float		misc_float;	// #CAT_Activation miscellaneous floating point variable 
  float		misc_float1;	// #CAT_Activation second miscellaneous floating point variable 
  float		misc_float2;	// #CAT_Activation third miscellaneous floating point variable 

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
  int		minus;		// #DEF_50:200 number of cycles to run in the minus phase with only inputs and no targets (used by CtLeabraSettle program), sets cycle_max -- can be 0
  int		plus;		// #DEF_20:200 number of cycles to run in the plus phase with input and target activations (used by CtLeabraSettle program), sets cycle_max -- must be > 0
  int		inhib;		// #DEF_0;1 number of cycles to run in the final inhibitory phase -- network can do MINUS_PLUS_PLUS, MINUS_PLUS_MINUS, or MINUS_PLUS_NOTHING for inputs on this phase
  int		n_avg_only_epcs; // #DEF_1 number of epochs during which time only sravg values are accumulated, and no learning occurs

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
  int		start;		// #DEF_30:60 number of cycles from the start of a new pattern to start computing sravg value -- avoid transitional states that are too far away from attractor state
  int		end;		// #DEF_0;1 number of cycles from the start of the final inhibitory phase to continue recording sravg
  int		interval;	// #DEF_5 how frequently to compute sravg -- more infrequent updating saves computational costs as sravg is expensive
  int		plus_s_st;	// [10 for spiking, else plus-1, typically 19] how many cycles into the plus phase should the short time scale sravg computation start (only for TRIAL sravg computation)

  SIMPLE_COPY(CtSRAvgSpec);
  TA_BASEFUNS(CtSRAvgSpec);
  //protected:
  //  void UpdateAfterEdit_impl();

private:
  void	Initialize();
  void 	Destroy()	{ };
};

class LEABRA_API CtSRAvgVals : public taOBase {
  // ##INLINE ##NO_TOKENS ##CAT_Leabra sender-receiver average values accumulated during Ct processing -- e.g., overall vals used for normalization
INHERITED(taOBase)
public:
  float		s_sum;	// #READ_ONLY #EXPERT sum of sravg_s (short time-scale, plus phase) weightings (count of number of times sravg has been computed) -- used for normalizing the overall average
  float		s_nrm;	// #READ_ONLY #EXPERT normalization term computed from sravg_s_sum -- multiply connection-level sravg_s by this value
  float		m_sum;	// #READ_ONLY #EXPERT sum of sravg_m (medium time-scale, trial) weightings (count of number of times sravg has been computed) -- used for normalizing the overall average
  float		m_nrm;	// #READ_ONLY #EXPERT normalization term computed from sravg_m_sum -- multiply connection-level sravg_m by this value
  bool		do_s;	// #READ_ONLY #EXPERT flag set during Compute_SRAvg for whether to compute short-time scale (plus phase) sravg values now or not

  void		InitVals() { s_sum = s_nrm = m_sum = m_nrm = 0.0f; do_s = false; }

  SIMPLE_COPY(CtSRAvgVals);
  TA_BASEFUNS(CtSRAvgVals);
// protected:
//   void UpdateAfterEdit_impl();

private:
  void	Initialize();
  void 	Destroy()	{ };
};

class LEABRA_API CtSineInhibMod : public taOBase {
  // ##INLINE ##NO_TOKENS ##CAT_Leabra sinusoidal inhibitory modulation parameters simulating initial burst of activation and subsequent oscillatory ringing
INHERITED(taOBase)
public:
  int		start;		// #DEF_30:60 number of cycles from onset of new input to start applying sinusoidal inhibitory modulation
  int		duration;	// #DEF_20 number of cycles from start to apply modulation
  float		n_pi;		// #DEF_2 number of multiples of PI to produce within duration of modulation (1.0 = positive only wave, 2.0 = full pos/neg wave, 4.0 = two waves, etc)
  float		burst_i;	// #DEF_0.02;0 maximum reduction in inhibition as a proportion of computed kwta value to subtract for positive activation (burst) phase of wave -- value should be a positive number
  float		trough_i;	// #DEF_0.02;0 maximum extra inhibition as proportion of computed kwta value to add for negative activation (trough) phase of wave -- value shoudl be a positive number

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

class LEABRA_API CtFinalInhibMod : public taOBase {
  // ##INLINE ##NO_TOKENS ##CAT_Leabra extra inhibition to apply at end of stimulus processing during inhib phase, to clear out existing pattern
INHERITED(taOBase)
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
    LEABRA_CHL,			// standard Leabra Contrastive Hebbian Learning rule with hebbian self-organizing factor: (s+r+) - (s-r-) + r+(s+ - w) -- s=sender,r=recv +=plus phase, -=minus phase, w= weight
    CTLEABRA_CAL,		// Continuous-Time Leabra Contrastive Attractor Learning rule: <sr>_s - <sr>_m -- s=sender, r=recv, <> = avg over short (plus phase) and medium (trial) time scales -- purely error-driven but inhibitory oscillations can drive self-organizing component
    CTLEABRA_XCAL,		// Continuous-Time Leabra temporally eXtended Contrastive Attractor Learning rule, trial-based version, which has two time scales of contrasts: short-vs-medium (svm) and medium-vs-long (mvl): (<sr>_s - <sr>_m) + (<sr>_m - <r>_l) -- s=sender, r=recv, <> = avg over short (plus phase), medium (trial), long (epoch) time scales.  svm is basically error-driven learning, and mvl is BCM-style self-organizing learning.
    CTLEABRA_XCAL_C,		// Continuous-Time Leabra temporally eXtended Contrastive Attractor Learning rule, fully continuous version, which has two time scales of contrasts: short-vs-medium (svm) and medium-vs-long (mvl): (<sr>_s - <sr>_m) + (<sr>_m - <r>_l) -- s=sender, r=recv, <> = avg over short (plus phase), medium (trial), long (epoch) time scales.  svm is basically error-driven learning, and mvl is BCM-style self-organizing learning.
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
    PLUS_NOTHING,		// just an auto-encoder (no initial minus phase)
    MINUS_PLUS_NOTHING,		// standard for CtLeabra_X/CAL and auto-encoder version with final 'nothing' minus phase
    MINUS_PLUS_MINUS,		// alternative version for CtLeabra_X/CAL with input still in final phase -- this 2nd minus is also marked as a nothing_phase 
    MINUS_PLUS_PLUS,		// two plus phases for gated context layer updating in second plus phase, for the PBWM model
    MINUS_PLUS_PLUS_NOTHING,	// PBWM in CtLeabra_X/CAL mode
    MINUS_PLUS_PLUS_MINUS,	// PBWM in CtLeabra_X/CAL mode, alternative final inhib stage
  };

  enum ThreadFlags { // #BITS flags for controlling the parallel threading process (which functions are threaded)
    TF_NONE	= 0x00,	// #NO_BIT no thread flags set
    NETIN 	= 0x01,	// ~20% of compute time, norm comp val = 1.0, the net input computation (sender-based), computed per cycle
    NETIN_INTEG	= 0x02,	// ~20% of compute time, norm comp val = 1.0, the net input computation (sender-based), computed per cycle
    SRAVG 	= 0x04,	// ~12% of compute time, norm comp val = 0.9, the sender-receiver average activation (xcal only), computed per ct_sravg.interval (typically every 5 cycles)
    ACT		= 0x08,	// ~7% of compute time, norm comp val = 0.4, activation, computed per cycle
    WEIGHTS	= 0x10,	// ~7% of compute time, norm comp val = 1.0, weight update from dwt changes, computed per trial (and still that expensive)
    DWT		= 0x20,	// ~3% of compute time, norm comp val = 0.6, delta-weight changes (learning), computed per trial
    TRIAL_INIT	= 0x40,	// ~2% of compute time, norm comp val = 0.2, trial-level initialization -- includes SRAvg init over connections if using xcal, which can be expensive
    SETTLE_INIT	= 0x80,	// ~.5% of compute time, norm comp val = 0.1, settle-level initialization -- only at unit level and the most lightweight function -- may not be worth it in general to parallelize
    TF_ALL	= 0xFF,	// #NO_BIT all thread flags set
  };

  LearnRule	learn_rule;	// The variant of Leabra learning rule to use 
  PhaseOrder	phase_order;	// [Default: MINUS_PLUS] #CAT_Counter number and order of phases to present
  bool		no_plus_test;	// #DEF_true #CAT_Counter don't run the plus phase when testing
  StateInit	sequence_init;	// #DEF_DO_NOTHING #CAT_Activation how to initialize network state at start of a sequence of trials

  Phase		phase;		// #GUI_READ_ONLY #SHOW #CAT_Counter #VIEW type of settling phase
  bool		nothing_phase;	// #GUI_READ_ONLY #SHOW #CAT_Counter the current phase is a NOTHING phase (phase will indicate MINUS for learning purposes)
  int		phase_no;	// #GUI_READ_ONLY #SHOW #CAT_Counter #VIEW phase as an ordinal number (regular phase is Phase enum)
  int		phase_max;	// #CAT_Counter maximum number of phases to run (note: this is set by Trial_Init depending on phase_order)

  int		ct_cycle;	// #GUI_READ_ONLY #SHOW #CAT_Counter #VIEW continuous time cycle counter: counts up from start of trial 
  float		time_inc;	// how much to increment the network time variable every cycle -- this goes monotonically up from the last weight init or manual reset

  int		cycle_max;	// #CAT_Counter #CONDEDIT_ON_learn_rule:LEABRA_CHL #DEF_60 maximum number of cycles to settle for: note for CtLeabra_X/CAL this is overridden by phase specific settings by the settle process
  int		mid_minus_cycle; // #CAT_Counter #DEF_-1:30 cycle number for computations that take place roughly mid-way through the minus phase -- used for PBWM algorithm -- effective min_cycles for minus phase will be this value + min_cycles -- set to -1 to disable
  int		min_cycles;	// #CAT_Counter #CONDEDIT_ON_learn_rule:LEABRA_CHL #DEF_15:35 minimum number of cycles to settle for
  int		min_cycles_phase2; // #CAT_Counter #CONDEDIT_ON_learn_rule:LEABRA_CHL #DEF_35 minimum number of cycles to settle for in second phase

  CtTrialTiming	 ct_time;	// #CAT_Learning #CONDEDIT_OFF_learn_rule:LEABRA_CHL timing parameters for ct leabra trial: Settle_Init sets the cycle_max based on these values
  CtSRAvgSpec	 ct_sravg;	// #CAT_Learning #CONDEDIT_OFF_learn_rule:LEABRA_CHL parameters controlling computation of sravg value as a function of cycles
  CtSineInhibMod ct_sin_i;	// #CAT_Learning #CONDEDIT_OFF_learn_rule:LEABRA_CHL sinusoidal inhibition parameters for inhibitory modulations during trial, simulating oscillations resulting from imperfect inhibtory set point behavior
  CtFinalInhibMod ct_fin_i;	// #CAT_Learning #CONDEDIT_OFF_learn_rule:LEABRA_CHL final inhibition parameters for extra inhibition to apply during final inhib phase, simulating slow-onset GABA currents
  CtSRAvgVals	sravg_vals;	// #CAT_Learning #READ_ONLY #EXPERT sender-receiver average computation values, e.g., for normalizing sravg values
  ThreadFlags	thread_flags;	// #CAT_Structure #EXPERT #NO_SAVE flags for controlling the parallel threading process (which functions are threaded) -- this is just for testing and debugging purposes, and not for general use -- they are not saved

  float		minus_cycles;	// #GUI_READ_ONLY #SHOW #CAT_Statistic #VIEW cycles to settle in the minus phase -- this is the typical settling time statistic to record
  float		avg_cycles;	// #GUI_READ_ONLY #SHOW #CAT_Statistic average settling cycles in the minus phase (computed over previous epoch)
  float		avg_cycles_sum; // #READ_ONLY #DMEM_AGG_SUM #CAT_Statistic sum for computing current average cycles in this epoch
  int		avg_cycles_n;	// #READ_ONLY #DMEM_AGG_SUM #CAT_Statistic N for average cycles computation for this epoch

  String	minus_output_name; // #GUI_READ_ONLY #SHOW #CAT_Statistic #VIEW output_name in the minus phase -- for recording in logs as network's response (output_name in plus phase is clamped target value)

  LeabraNetMisc	net_misc;	// misc network level parameters for leabra

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

  float		ext_rew;	// #GUI_READ_ONLY #SHOW #CAT_Statistic #VIEW external reward value (on this trial) -- only computed if ExtRewLayerSpec or similar exists in network -- equals PVe value in PVLV framework
  bool		ext_rew_avail; 	// #GUI_READ_ONLY #SHOW #CAT_Statistic actual external reward value is available (on this trial) -- only computed if ExtRewLayerSpec or similar exists in network -- if false then no feedback was provided on this trial
  float		norew_val; 	// #GUI_READ_ONLY #CAT_Statistic no-reward value (serves as a baseline against which ext_rew can be compared against -- if greater, then positive reward, if less, then negative reward -- typically 0.5 but can vary
  float		avg_ext_rew;	// #GUI_READ_ONLY #SHOW #CAT_Statistic average external reward value (computed over previous epoch)
  float		pvlv_pvi;	// #GUI_READ_ONLY #SHOW #CAT_Statistic PVLV primary reward prediction value PVi for the current trial -- updated on a cycle-by-cycle basis -- used for noise modulation among perhaps other things
  float		pvlv_pvr;	// #GUI_READ_ONLY #SHOW #CAT_Statistic PVLV primary reward availability prediction value PVr for the current trial -- updated on a cycle-by-cycle basis
  float		pvlv_lve;	// #GUI_READ_ONLY #SHOW #CAT_Statistic PVLV learned reward prediction value LVe (excitatory, rapidly adapting) for the current trial -- updated on a cycle-by-cycle basis -- used for noise modulation among perhaps other things
  float		pvlv_lvi;	// #GUI_READ_ONLY #SHOW #CAT_Statistic PVLV learned reward prediction value LVi (inhibitory, slowly adapting) for the current trial -- updated on a cycle-by-cycle basis -- used for noise modulation among perhaps other things
  bool		pv_detected;	// #GUI_READ_ONLY #SHOW #CAT_Statistic PVLV detected a situation where primary reward value is expected to be available, based on learned encoding of similar such situations in the past -- computed by the PVrLayerSpec continuously in the minus phase
  float		avg_ext_rew_sum; // #READ_ONLY #DMEM_AGG_SUM #CAT_Statistic sum for computing current average external reward value in this epoch
  int		avg_ext_rew_n;	// #READ_ONLY #DMEM_AGG_SUM #CAT_Statistic N for average external reward value computation for this epoch

  bool		off_errs;	// #DEF_true #CAT_Statistic include in norm_err computation units that were incorrectly off (should have been on but were actually off) -- either 1 or both of off_errs and on_errs must be set
  bool		on_errs;	// #DEF_true #CAT_Statistic include in norm_err computation units that were incorrectly on (should have been off but were actually on) -- either 1 or both of off_errs and on_errs must be set
  float		norm_err;	// #GUI_READ_ONLY #SHOW #CAT_Statistic #VIEW normalized binary (Hamming) error on this trial: number of units that were incorrectly activated or incorrectly inactivated (see off_errs to exclude latter)
  float		avg_norm_err;	// #GUI_READ_ONLY #SHOW #CAT_Statistic average normalized binary error value (computed over previous epoch)
  float		avg_norm_err_sum; // #READ_ONLY #DMEM_AGG_SUM #CAT_Statistic sum for computing current average norm err in this epoch
  int		avg_norm_err_n;	// #READ_ONLY #DMEM_AGG_SUM #CAT_Statistic N for average norm err value computation for this epoch

  bool		inhib_cons_used; // #READ_ONLY #NO_SAVE #CAT_Threads inhibitory connections are being used in this network -- detected during buildunits_threads to determine if space should be allocated, etc for send_inhib vals
  float_Matrix	send_inhib_tmp; // #READ_ONLY #NO_SAVE #CAT_Threads temporary storage for threaded sender-based inhib netinput computation -- dimensions are [un_idx][task] (inner = units, outer = task, such that units per task is contiguous in memory)

  ///////////////////////////////////////////////////////////////////////
  //	Thread Flags

  inline void	SetThreadFlag(ThreadFlags flg)
  { thread_flags = (ThreadFlags)(thread_flags | flg); }
  // set flag state on
  inline void	ClearThreadFlag(ThreadFlags flg)
  { thread_flags = (ThreadFlags)(thread_flags & ~flg); }
  // clear flag state (set off)
  inline bool	HasThreadFlag(ThreadFlags flg) const { return (thread_flags & flg); }
  // check if flag is set
  inline void	SetThreadFlagState(ThreadFlags flg, bool on)
  { if(on) SetThreadFlag(flg); else ClearThreadFlag(flg); }
  // set flag state according to on bool (if true, set flag, if false, clear it)

  ///////////////////////////////////////////////////////////////////////
  //	General Init functions

  override void	Init_Acts();
  override void	Init_Counters();
  override void	Init_Stats();
  override void	Init_Sequence();
  override void Init_Weights();

  virtual void	DecayState(float decay);
  // #CAT_Activation decay activation states towards initial values by given amount (0 = no decay, 1 = full decay)

  virtual void	SetLearnRule_ConSpecs(BaseSpec_Group* spgp);
  // #IGNORE set the current learning rule into all conspecs in given spec group (recursive)
  virtual void	SetLearnRule();
  // #CAT_ObjectMgmt set the current learning rule into the conspecs on this network (done by network UAE only when rule changed)

  virtual void	CheckInhibCons();
  override void	BuildUnits_Threads();
  override bool RecvOwnsCons() { return false; }

  ///////////////////////////////////////////////////////////////////////
  //	TrialInit -- at start of trial

  virtual void 	Trial_Init();
  // #CAT_TrialInit initialize at start of trial (SetCurLrate, set phase_max, Decay state)
    virtual void Trial_Init_Phases();
    // #CAT_TrialInit init phase_max and current phase based on phase_order -- network-only
    virtual void SetCurLrate();
    // #CAT_TrialInit set the current learning rate according to the LeabraConSpec parameters -- goes through projections

    virtual void Trial_Init_Unit();
    // #CAT_TrialInit trial unit-level initialization functions: DecayState, NoiseInit, Trial_Init_SRAvg -- replaces those functions
      virtual void Trial_DecayState();
      // #CAT_TrialInit decay the state in between trials (params in LayerSpec) -- goes to units via layers -- not typically used, Trial_Init_Unit instead
      virtual void Trial_NoiseInit();
      // #CAT_TrialInit initialize various noise factors at start of trial -- goes to units via layers
      virtual void Trial_Init_SRAvg();
      // #CAT_Learning initialize sending-receiving activation coproduct averages (CtLeabra_X/CAL) -- goes to connections via units, layers
    virtual void Trial_Init_Layer();
    // #CAT_TrialInit layer-level trial init (not used in base code, can be overloaded)

  ///////////////////////////////////////////////////////////////////////
  //	SettleInit -- at start of settling

  virtual void  Settle_Init();
  // #CAT_SettleInit initialize network for settle-level processing (decay, active k, hard clamp, netscale)
    virtual void Settle_Init_CtTimes();
    // #CAT_SettleInit initialize cycles based on network phases for CtLeabra_X/CAL
    virtual void Compute_Active_K();
    // #CAT_SettleInit determine the active k values for each layer based on pcts, etc (called by Settle_Init) -- must have hard clamp called first 
    virtual void Settle_Init_Unit();
    // #CAT_TrialInit settle unit-level initialization functions: Init_TargFlags, DecayState, NetinScale
    virtual void Settle_Init_Layer();
    // #CAT_TrialInit settle layer-level initialization hook -- default calls TargFlags_Layer, and can be used for hook for other guys
      virtual void Settle_Init_TargFlags();
      // #CAT_SettleInit initialize at start of settling phase -- sets target external input flags based on phase -- not called by default -- direct to unit level function
      virtual void Settle_DecayState();
      // #CAT_SettleInit logic for performing decay and updating external input settings as a function of phase
      virtual void Compute_NetinScale();
      // #CAT_SettleInit compute netinput scaling values by projection

    virtual void Compute_NetinScale_Senders();
    // #CAT_SettleInit compute net input scaling values for sending cons -- copies from values computed in the recv guys -- has to be done as a second phase of the Settle_Init_Unit stage after all the recv ones are computed

    virtual void Compute_HardClamp();
    // #CAT_SettleInit compute hard clamping from external inputs

    virtual void ExtToComp();
    // #CAT_SettleInit move external input values to comparison values (not currently used)
    virtual void TargExtToComp();
    // #CAT_SettleInit move target and external input values to comparison (for PLUS_NOTHING, called by Settle_Init)

  virtual void  NewInputData_Init();
  // #CAT_SettleInit perform initialization stuff needed to update external input data signals so they actually show up as activations in the network: Settle_Init_Layer, Settle_Init_TrgFlags, Compute_HardClamp

  ////////////////////////////////////////////////////////////////
  //	Cycle_Run

  virtual void	Cycle_Run();
  // #CAT_Cycle compute one cycle of updating: netinput, inhibition, activations

  ///////////////////////////////////////////////////////
  //	Cycle Stage 1: netinput

  override void	Send_Netin();
  // #CAT_Cycle compute netinputs (sender-delta based -- only send when sender activations change) -- new values go in net_delta or g_i_delta (summed up from tmp array for threaded case)
  virtual void Compute_NetinInteg();
  // #CAT_Cycle Stage 1.2 integrate newly-computed netinput delta values into a resulting complete netinput value for the network (does both excitatory and inhibitory)

  virtual void Compute_NetinStats();
  // #CAT_Cycle Stage 1.3 compute AvgMax stats on netin and i_thr values computed during netin computation -- used for various regulatory and monitoring functions -- not threadable


  ///////////////////////////////////////////////////////////////////////
  //	Cycle Step 2: Inhibition

  virtual void	Compute_Inhib();
  // #CAT_Cycle compute inhibitory conductances (kwta) -- also calls LayInhibToGps to coordinate group-level inhibition sharing
  virtual void	Compute_ApplyInhib();
  // #CAT_Cycle Stage 2.3 apply inhibitory conductances from kwta to individual units -- separate step after all inhib is integrated and computed

  ///////////////////////////////////////////////////////////////////////
  //	Cycle Step 3: Activation

  override void	Compute_Act();
  // #CAT_Cycle compute activations

  ///////////////////////////////////////////////////////////////////////
  //	Cycle Stats

  virtual void	Compute_CycleStats();
  // #CAT_Cycle compute cycle-level stats -- acts AvgMax, MaxDa, OutputName, etc

  ///////////////////////////////////////////////////////////////////////
  //	Cycle Optional Misc

  virtual void 	Compute_CycSynDep();
  // #CAT_Activation compute cycle-level synaptic depression (must be defined by appropriate subclass) -- called at end of each cycle of computation if net_misc.cyc_syn_dep is on -- threaded direct to units
  virtual void	Compute_MidMinus();
  // #CAT_Activation do special processing midway through the minus phase, as determined by the mid_minus_cycles parameter, if > 0 -- currently used for the PBWM algorithm

  ///////////////////////////////////////////////////////////////////////
  //	Settle Final

  virtual void	Settle_Final();
  // #CAT_SettleFinal do final processing after settling (postsettle, Compute_dWt if needed)
    virtual void PostSettle();
    // #CAT_SettleFinal perform computations in layers at end of settling  (called by Settle_Final)
    virtual void Settle_Compute_dWt();
    // #CAT_SettleFinal compute weight changes at end of settling as needed depending on phase order -- all weight changes are computed here for consistency
    virtual void AdaptKWTAPt();
    // #CAT_SettleFinal adapt the kwta point based on average activation values

  ///////////////////////////////////////////////////////////////////////
  //	Trial Update and Final

  virtual void Trial_UpdatePhase();
  // #CAT_TrialInit update phase based on phase_no -- typically called by program in Trial loop over settling

  virtual void	Trial_Final();
  // #CAT_TrialFinal do final processing after trial (Compute_dWt, EncodeState)
    virtual void EncodeState();
    // #CAT_TrialFinal encode final state information at end of trial for time-based learning across trials
    virtual void Compute_SelfReg_Trial();
    // #CAT_TrialFinal update self-regulation (accommodation, hysteresis) at end of trial

  ///////////////////////////////////////////////////////////////////////
  //	Learning

  virtual void 	Compute_SRAvg();
  // #CAT_Learning compute sending-receiving activation coproduct averages (CtLeabra_X/CAL) -- called at the Cycle_Run level, and threaded down to unit level

  virtual void 	Compute_dWt_SRAvg();
  // #CAT_Learning compute sravg vals at start of dwt computation (nrm terms)

  virtual void	Compute_dWt_Layer_pre();
  // #CAT_Learning do special computations at layer level prior to standard unit-level thread dwt computation -- not used in base class but is in various derived classes

  virtual void	Compute_dWt_FirstPlus();
  // #CAT_Learning compute weight change after first plus phase has been encountered: standard layers do a weight change here, except under CtLeabra_X/CAL
  virtual void	Compute_dWt_SecondPlus();
  // #CAT_Learning compute weight change after second plus phase has been encountered: standard layers do NOT do a weight change here -- only selected special ones
  virtual void	Compute_dWt_Nothing();
  // #CAT_Learning compute weight change after final nothing phase: standard layers do a weight change here under both learning rules

  override void Compute_Weights_impl();

  ///////////////////////////////////////////////////////////////////////
  //	Stats

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
  // #MENU #MENU_SEP_BEFORE #NULL_OK #NULL_TEXT_NewGraphData graph the overall inhibitory modulation curve, including sinusoidal and final -- if flip_sign is true, then sign is reversed so that graph looks like the activation profile instead of the inhibition profile

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


inline void LeabraConSpec::C_Send_NetinDelta_Thrd(Connection* cn, float* send_netin_vec,
					     LeabraUnit* ru, float su_act_delta_eff) {
  send_netin_vec[ru->flat_idx] += cn->wt * su_act_delta_eff;
}

inline void LeabraConSpec::C_Send_NetinDelta_NoThrd(Connection* cn, LeabraUnit* ru,
						   float su_act_delta_eff) {
  ru->net_delta += cn->wt * su_act_delta_eff;
}

inline void LeabraConSpec::C_Send_InhibDelta_Thrd(Connection* cn, float* send_inhib_vec,
						 LeabraUnit* ru, float su_act_delta_eff) {
  send_inhib_vec[ru->flat_idx] += cn->wt * su_act_delta_eff;
}

inline void LeabraConSpec::C_Send_InhibDelta_NoThrd(Connection* cn, LeabraUnit* ru,
						   float su_act_delta_eff) {
  ru->g_i_delta += cn->wt * su_act_delta_eff;
}

inline void LeabraConSpec::Send_NetinDelta(LeabraSendCons* cg, LeabraNetwork* net,
					   int thread_no, float su_act_delta) {
  float su_act_delta_eff = cg->scale_eff * su_act_delta;
  if(inhib && net->inhib_cons_used) { // both must agree that inhib is ok
    if(thread_no < 0) {
      CON_GROUP_LOOP(cg, C_Send_InhibDelta_NoThrd(cg->OwnCn(i), (LeabraUnit*)cg->Un(i),
						 su_act_delta_eff));
    }
    else {
      float* send_inhib_vec = net->send_inhib_tmp.el
	+ net->send_inhib_tmp.FastElIndex(0, thread_no);
      CON_GROUP_LOOP(cg, C_Send_InhibDelta_Thrd(cg->OwnCn(i), send_inhib_vec,
					(LeabraUnit*)cg->Un(i), su_act_delta_eff));
    }
  }
  else {
    if(thread_no < 0) {
      CON_GROUP_LOOP(cg, C_Send_NetinDelta_NoThrd(cg->OwnCn(i), (LeabraUnit*)cg->Un(i),
						 su_act_delta_eff));
    }
    else {
      float* send_netin_vec = net->send_netin_tmp.el
	+ net->send_netin_tmp.FastElIndex(0, thread_no);
      CON_GROUP_LOOP(cg, C_Send_NetinDelta_Thrd(cg->OwnCn(i), send_netin_vec,
					       (LeabraUnit*)cg->Un(i), su_act_delta_eff));
    }
  }
}

inline float LeabraConSpec::C_Compute_Netin(LeabraCon* cn, LeabraUnit*, LeabraUnit* su) {
  return cn->wt * su->act_eq;
}

inline float LeabraConSpec::Compute_Netin(RecvCons* cg, Unit* ru) {
  float rval=0.0f;
  CON_GROUP_LOOP(cg, rval += C_Compute_Netin((LeabraCon*)cg->PtrCn(i), (LeabraUnit*)ru,
					     (LeabraUnit*)cg->Un(i)));
  return ((LeabraRecvCons*)cg)->scale_eff * rval;
}


////////////////////////////////////////////////////
//     Computing dWt: LeabraCHL

inline void LeabraConSpec::Compute_SAvgCor(LeabraSendCons* cg, LeabraUnit*) {
  LeabraLayer* fm = (LeabraLayer*)cg->prjn->from.ptr();
  float savg = .5f + savg_cor.cor * (fm->kwta.pct - .5f);
  savg = MAX(savg_cor.thresh, savg); // keep this computed value within bounds
  cg->savg_cor = .5f / savg;
}

inline float LeabraConSpec::C_Compute_Hebb(LeabraCon* cn, LeabraSendCons* cg,
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

inline void LeabraConSpec::Compute_dWt_LeabraCHL(LeabraSendCons* cg, LeabraUnit* su) {
  Compute_SAvgCor(cg, su);
  if(((LeabraLayer*)cg->prjn->from.ptr())->acts_p.avg < savg_cor.thresh) return;

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

/////////////////////////////////////
//	Compute_Weights_LeabraCHL

inline void LeabraConSpec::C_Compute_Weights_LeabraCHL(LeabraCon* cn)
{
  if(cn->dwt != 0.0f) {
    cn->wt = SigFmLinWt(LinFmSigWt(cn->wt) + cn->dwt);
  }
  cn->pdw = cn->dwt;
  cn->dwt = 0.0f;
}

inline void LeabraConSpec::Compute_Weights_LeabraCHL(LeabraSendCons* cg, LeabraUnit* su) {
  CON_GROUP_LOOP(cg, C_Compute_Weights_LeabraCHL((LeabraCon*)cg->OwnCn(i)));
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

inline void LeabraConSpec::C_Compute_SRAvg_cont(LeabraCon* cn, float ru_act, float su_act) {
  float sr = ru_act * su_act;
  cn->sravg_s += xcal_c.s_dt * (sr - cn->sravg_s);
  cn->sravg_m += xcal_c.m_dt * (cn->sravg_s - cn->sravg_m);
}

inline void LeabraConSpec::Compute_SRAvg(LeabraSendCons* cg, LeabraUnit* su, bool do_s) {
  if(!xcalm.do_comp_sravg) return;

  if(learn_rule == CTLEABRA_XCAL_C) {
    CON_GROUP_LOOP(cg, C_Compute_SRAvg_cont((LeabraCon*)cg->OwnCn(i), 
					    ((LeabraUnit*)cg->Un(i))->act_nd, su->act_nd));
  }
  else {
    if(do_s) {
      CON_GROUP_LOOP(cg, C_Compute_SRAvg_ms((LeabraCon*)cg->OwnCn(i),
					    ((LeabraUnit*)cg->Un(i))->act_nd, su->act_nd));
    }
    else {
      CON_GROUP_LOOP(cg, C_Compute_SRAvg_m((LeabraCon*)cg->OwnCn(i), 
					   ((LeabraUnit*)cg->Un(i))->act_nd, su->act_nd));
    }
  }
}

inline void LeabraConSpec::C_Trial_Init_SRAvg(LeabraCon* cn) {
  cn->sravg_s = 0.0f;
  cn->sravg_m = 0.0f;
}

inline void LeabraConSpec::Trial_Init_SRAvg(LeabraSendCons* cg, LeabraUnit* su) {
  if(!xcalm.do_init_sravg) return;

  CON_GROUP_LOOP(cg, C_Trial_Init_SRAvg((LeabraCon*)cg->OwnCn(i)));
}

//////////////////////////////////////////////////////////////////////////////////
//     Computing dWt: CtLeabra_XCAL

inline void LeabraConSpec::
C_Compute_dWt_CtLeabraXCAL_SR_trial(LeabraCon* cn, LeabraUnit* ru, LeabraUnit* su,
				    float sravg_s_nrm, float sravg_m_nrm) {
  float srs = (sravg_s_nrm * cn->sravg_s);
  float srm = (sravg_m_nrm * cn->sravg_m);
  float sm_mix = xcal.s_mix * srs + xcal.m_mix * srm;
  cn->dwt += cur_lrate * (xcal.svm_mix * xcal.dWtFun(sm_mix, srm) + 
 			  xcal.mvl_mix * xcal.dWtFun(sm_mix, ru->l_thr));
}

inline void LeabraConSpec::
C_Compute_dWt_CtLeabraXCAL_SRMAX_trial(LeabraCon* cn, LeabraUnit* ru, LeabraUnit* su,
				    float sravg_s_nrm, float sravg_m_nrm) {
  float srs = (sravg_s_nrm * cn->sravg_s);
  float srm = (sravg_m_nrm * cn->sravg_m);
  float sm_mix = xcal.s_mix * srs + xcal.m_mix * srm;
  float effthr = MAX(srm, ru->l_thr);
  cn->dwt += cur_lrate * xcal.dWtFun(sm_mix, effthr);
}

inline void LeabraConSpec::
C_Compute_dWt_CtLeabraXCAL_SEP_trial(LeabraCon* cn, LeabraUnit* ru, LeabraUnit* su,
				     float sravg_s_nrm, float sravg_m_nrm) {
  LeabraCon* rbias = (LeabraCon*)ru->bias.OwnCn(0);
  LeabraCon* sbias = (LeabraCon*)su->bias.OwnCn(0);
  float srs = (sravg_s_nrm * rbias->sravg_s) * (sravg_s_nrm * sbias->sravg_s);
  float srm = (sravg_m_nrm * rbias->sravg_m) * (sravg_m_nrm * sbias->sravg_m);
  float sm_mix = xcal.s_mix * srs + xcal.m_mix * srm;
  cn->dwt += cur_lrate * (xcal.svm_mix * xcal.dWtFun(sm_mix, srm) + 
			  xcal.mvl_mix * xcal.dWtFun(sm_mix, ru->l_thr));
}

inline void LeabraConSpec::
C_Compute_dWt_CtLeabraXCAL_SEP_trial_ml(LeabraCon* cn, LeabraUnit* ru, LeabraUnit* su,
					float sravg_s_nrm, float sravg_m_nrm) {
  LeabraCon* rbias = (LeabraCon*)ru->bias.OwnCn(0);
  LeabraCon* sbias = (LeabraCon*)su->bias.OwnCn(0);
  float srs = (sravg_s_nrm * rbias->sravg_s) * (sravg_s_nrm * sbias->sravg_s);
  float srm = (sravg_m_nrm * rbias->sravg_m) * (sravg_m_nrm * sbias->sravg_m);
  float srml = xcalm.sm_mix * srm + xcalm.ml_mix * (ru->ravg_ml * su->ravg_ml);
  float sm_mix = xcal.s_mix * srs + xcal.m_mix * srml;
  cn->dwt += cur_lrate * (xcal.svm_mix * xcal.dWtFun(sm_mix, srm) + 
			  xcal.mvl_mix * xcal.dWtFun(sm_mix, ru->l_thr));
}

inline void LeabraConSpec::
C_Compute_dWt_CtLeabraXCAL_CAL_trial(LeabraCon* cn, LeabraUnit* ru, LeabraUnit* su, 
				     float sravg_s_nrm, float sravg_m_nrm) {
  float srs = (sravg_s_nrm * cn->sravg_s);
  float srm = (sravg_m_nrm * cn->sravg_m);
  float sm_mix = xcal.s_mix * srs + xcal.m_mix * srm;
  cn->dwt += cur_lrate * (xcal.svm_mix * (srs - srm) + 
			  xcal.mvl_mix * xcal.dWtFun(sm_mix, ru->l_thr));
}

inline void LeabraConSpec::
C_Compute_dWt_CtLeabraXCAL_CHL_trial(LeabraCon* cn, LeabraUnit* ru, LeabraUnit* su,
				     float sravg_s_nrm, float sravg_m_nrm)
{
  float srs = (sravg_s_nrm * cn->sravg_s);
  float srm = (sravg_m_nrm * cn->sravg_m);
  float sm_mix = xcal.s_mix * srs + xcal.m_mix * srm;
  cn->dwt += cur_lrate * (xcal.svm_mix * ((ru->act_p * su->act_p) - (ru->act_m * su->act_m)) +
			  xcal.mvl_mix * xcal.dWtFun(sm_mix, ru->l_thr));
}

inline void LeabraConSpec::Compute_dWt_CtLeabraXCAL(LeabraSendCons* cg, LeabraUnit* su) {
  LeabraLayer* rlay = (LeabraLayer*)cg->prjn->layer;
  LeabraNetwork* net = (LeabraNetwork*)rlay->own_net;
//   // threshold is mix of l and ml terms:
//   float ru_thr = MAX(ru->ravg_l, ru->ravg_ml);
//   ru_thr *= xcal.l_gain;
//   LeabraCon* rbias = (LeabraCon*)ru->bias.OwnCn(0);

  float sravg_s_nrm = net->sravg_vals.s_nrm;
  float sravg_m_nrm = net->sravg_vals.m_nrm;
  if(xcal.lrn_var == XCalLearnSpec::XCAL_SR) {
    for(int i=0; i<cg->size; i++) {
      LeabraUnit* ru = (LeabraUnit*)cg->Un(i);
      C_Compute_dWt_CtLeabraXCAL_SR_trial((LeabraCon*)cg->OwnCn(i), ru, su,
					  sravg_s_nrm, sravg_m_nrm);
    }
  }
  else if(xcal.lrn_var == XCalLearnSpec::XCAL_SRMAX) {
    for(int i=0; i<cg->size; i++) {
      LeabraUnit* ru = (LeabraUnit*)cg->Un(i);
      C_Compute_dWt_CtLeabraXCAL_SRMAX_trial((LeabraCon*)cg->OwnCn(i), ru, su,
					     sravg_s_nrm, sravg_m_nrm);
    }
  }
  else if(xcal.lrn_var == XCalLearnSpec::XCAL_SEP) {
    if(xcalm.ml_mix > 0.0f) {
      for(int i=0; i<cg->size; i++) {
	LeabraUnit* ru = (LeabraUnit*)cg->Un(i);
	C_Compute_dWt_CtLeabraXCAL_SEP_trial_ml((LeabraCon*)cg->OwnCn(i), ru, su,
						sravg_s_nrm, sravg_m_nrm);
      }
    }
    else {
      for(int i=0; i<cg->size; i++) {
	LeabraUnit* ru = (LeabraUnit*)cg->Un(i);
	C_Compute_dWt_CtLeabraXCAL_SEP_trial((LeabraCon*)cg->OwnCn(i), ru, su,
					     sravg_s_nrm, sravg_m_nrm);
      }
    }
  }
  else if(xcal.lrn_var == XCalLearnSpec::CAL) {
    for(int i=0; i<cg->size; i++) {
      LeabraUnit* ru = (LeabraUnit*)cg->Un(i);
      C_Compute_dWt_CtLeabraXCAL_CAL_trial((LeabraCon*)cg->OwnCn(i), ru, su,
					   sravg_s_nrm, sravg_m_nrm);
    }
  }
  else if(xcal.lrn_var == XCalLearnSpec::CHL) {
    for(int i=0; i<cg->size; i++) {
      LeabraUnit* ru = (LeabraUnit*)cg->Un(i);
      C_Compute_dWt_CtLeabraXCAL_CHL_trial((LeabraCon*)cg->OwnCn(i), ru, su,
					   sravg_s_nrm, sravg_m_nrm);
    }
  }
}

/////////////	XCAL_C

inline void LeabraConSpec::C_Compute_dWt_CtLeabraXCAL_C_sep(LeabraCon* cn,
							    LeabraUnit* ru, LeabraUnit* su) {
  LeabraCon* rbias = (LeabraCon*)ru->bias.OwnCn(0);
  LeabraCon* sbias = (LeabraCon*)su->bias.OwnCn(0);
  float srs = rbias->sravg_s * sbias->sravg_s;
  float srm = rbias->sravg_m * sbias->sravg_m;
  float sm_mix = xcal.s_mix * srs + xcal.m_mix * srm;
  float effthr = MAX(srm, ru->l_thr);
  cn->dwt += cur_lrate * xcal.dWtFun(sm_mix, effthr);
  // using SR_MAX by default
//   cn->dwt += cur_lrate * (xcal.svm_mix * xcal.dWtFun(sm_mix, srm) + 
// 			  xcal.mvl_mix * xcal.dWtFun(sm_mix, ru->l_thr));
}

inline void LeabraConSpec::Compute_dWt_CtLeabraXCAL_C(LeabraSendCons* cg, LeabraUnit* su) {
//  LeabraLayer* rlay = (LeabraLayer*)cg->prjn->layer;
//  LeabraNetwork* net = (LeabraNetwork*)rlay->own_net;

  for(int i=0; i<cg->size; i++) {
    LeabraUnit* ru = (LeabraUnit*)cg->Un(i);
    C_Compute_dWt_CtLeabraXCAL_C_sep((LeabraCon*)cg->OwnCn(i), ru, su);
  }
}


/////////////////////////////////////
//	Compute_Weights_CtLeabraXCAL

inline void LeabraConSpec::C_Compute_Weights_CtLeabraXCAL(LeabraCon* cn) {
  // always do soft bounding, at this point (post agg across processors, etc)
  float lin_wt = LinFmSigWt(cn->wt);
  if(xcalm.use_sb) {
    if(cn->dwt > 0.0f)	cn->dwt *= (1.0f - lin_wt);
    else		cn->dwt *= lin_wt;
  }

  if(cn->dwt != 0.0f) {
    cn->wt = SigFmLinWt(lin_wt + cn->dwt);
  }
  cn->pdw = cn->dwt;
  cn->dwt = 0.0f;
}

inline void LeabraConSpec::Compute_Weights_CtLeabraXCAL(LeabraSendCons* cg, LeabraUnit* su) {
  CON_GROUP_LOOP(cg, C_Compute_Weights_CtLeabraXCAL((LeabraCon*)cg->OwnCn(i)));
  //  ApplyLimits(cg, ru); limits are automatically enforced anyway
}


//////////////////////////////////////////////////////////////////////////////////
//     Computing dWt: CtLeabra_CAL

inline void LeabraConSpec::C_Compute_dWt_CtLeabraCAL(LeabraCon* cn,
						     float sravg_s_nrm, float sravg_m_nrm) {
  cn->dwt += cur_lrate * (sravg_s_nrm * cn->sravg_s - sravg_m_nrm * cn->sravg_m);
}

inline void LeabraConSpec::Compute_dWt_CtLeabraCAL(LeabraSendCons* cg, LeabraUnit* su) {
  // note: not doing all the checks for layers/groups inactive in plus phase: not needed since no hebb stuff
  LeabraLayer* rlay = (LeabraLayer*)cg->prjn->layer;
  LeabraNetwork* net = (LeabraNetwork*)rlay->own_net;
  CON_GROUP_LOOP(cg,
		 C_Compute_dWt_CtLeabraCAL((LeabraCon*)cg->OwnCn(i),
			   net->sravg_vals.s_nrm, net->sravg_vals.m_nrm));
}

/////////////////////////////////////
//	Compute_Weights_CtLeabraCAL

inline void LeabraConSpec::C_Compute_Weights_CtLeabraCAL(LeabraCon* cn)
{
  // always do soft bounding, at this point (post agg across processors, etc)
  float lin_wt = LinFmSigWt(cn->wt);
  if(cn->dwt > 0.0f)	cn->dwt *= (1.0f - lin_wt);
  else			cn->dwt *= lin_wt;

  if(cn->dwt != 0.0f) {
    cn->wt = SigFmLinWt(lin_wt + cn->dwt);
  }
  cn->pdw = cn->dwt;
  cn->dwt = 0.0f;
}

inline void LeabraConSpec::Compute_Weights_CtLeabraCAL(LeabraSendCons* cg, LeabraUnit* su) {
  CON_GROUP_LOOP(cg, C_Compute_Weights_CtLeabraCAL((LeabraCon*)cg->OwnCn(i)));
  //  ApplyLimits(cg, ru); limits are automatically enforced anyway
}


/////////////////////////////////////
// Master dWt function

inline void LeabraConSpec::Compute_Leabra_dWt(LeabraSendCons* cg, LeabraUnit* su) {
  switch(learn_rule) {
  case LEABRA_CHL:
    Compute_dWt_LeabraCHL(cg, su);
    break;
  case CTLEABRA_CAL:
    Compute_dWt_CtLeabraCAL(cg, su);
    break;
  case CTLEABRA_XCAL:
    Compute_dWt_CtLeabraXCAL(cg, su);
    break;
  case CTLEABRA_XCAL_C:
    Compute_dWt_CtLeabraXCAL_C(cg, su);
    break;
  }
}

inline void LeabraConSpec::Compute_Leabra_Weights(LeabraSendCons* cg, LeabraUnit* su) {
  switch(learn_rule) {
  case LEABRA_CHL:
    Compute_Weights_LeabraCHL(cg, su);
    break;
  case CTLEABRA_CAL:
    Compute_Weights_CtLeabraCAL(cg, su);
    break;
  case CTLEABRA_XCAL:
    Compute_Weights_CtLeabraXCAL(cg, su);
    break;
  case CTLEABRA_XCAL_C:
    Compute_Weights_CtLeabraXCAL(cg, su);
    break;
  }
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
  float ru_act;
  if(learn_rule <= CTLEABRA_CAL || xcalm.use_nd) {
    ru_act = ru->act_nd;
  }
  else {
    ru_act = ru->act;
    LeabraUnitSpec* rus = (LeabraUnitSpec*)ru->GetUnitSpec();
    if(rus->act_fun == LeabraUnitSpec::SPIKE)
      ru_act *= rus->spike.eq_gain;
  }
  if(learn_rule == CTLEABRA_XCAL_C) {
    // note: this is cascade version -- each builds upon the other
    cn->sravg_s += xcal_c.s_dt * (ru_act - cn->sravg_s);
    cn->sravg_m += xcal_c.m_dt * (cn->sravg_s - cn->sravg_m);
    ru->ravg_ml += xcal.ml_dt * (cn->sravg_m - ru->ravg_ml);
    ru->ravg_l += xcal.l_dt * (cn->sravg_m - ru->ravg_l);
    ru->l_thr = xcal.l_gain * MAX(ru->ravg_l, ru->ravg_ml);
    // note: updating unit-level ravg_l, ravg_ml, l_thr variables here..
  }
  else {
    cn->sravg_m += ru_act;
    if(do_s)
      cn->sravg_s += ru_act;
  }
}

inline void LeabraConSpec::B_Trial_Init_SRAvg(LeabraCon* cn, LeabraUnit* ru,
					      LeabraLayer* rlay) {
  if(learn_rule != CTLEABRA_XCAL_C) {
    LeabraNetwork* net = (LeabraNetwork*)rlay->own_net;
    float rm = net->sravg_vals.m_nrm * cn->sravg_m;
    ru->ravg_l += xcal.l_dt * (rm - ru->ravg_l);
    ru->ravg_ml += xcal.ml_dt * (rm - ru->ravg_ml);
    ru->l_thr = xcal.l_gain * MAX(ru->ravg_l, ru->ravg_ml);
    cn->sravg_s = 0.0f;
    cn->sravg_m = 0.0f;
  }
}

inline void LeabraConSpec::B_Init_RAvg_l(LeabraCon* cn, LeabraUnit* ru) {
  ru->ravg_l = xcalm.avg_init;
  ru->ravg_ml = xcalm.avg_init;
  ru->l_thr = xcal.l_gain * MAX(ru->ravg_l, ru->ravg_ml);
}

inline void LeabraConSpec::B_Compute_dWt_CtLeabraXCAL(LeabraCon* cn, LeabraUnit* ru,
						      LeabraLayer* rlay) {
  float dw;
  if(xcal.lrn_var == XCalLearnSpec::CHL) {
    dw = ru->act_p - ru->act_m;
  }
  else {
    LeabraNetwork* net = (LeabraNetwork*)rlay->own_net;
    // cal only for bias weights: only err is useful contributor to this learning
    dw = (net->sravg_vals.s_nrm * cn->sravg_s - net->sravg_vals.m_nrm * cn->sravg_m);
  }
  cn->dwt += cur_lrate * dw;
}

inline void LeabraConSpec::B_Compute_dWt_CtLeabraXCAL_C(LeabraCon* cn, LeabraUnit* ru,
							LeabraLayer* rlay) {
//  LeabraNetwork* net = (LeabraNetwork*)rlay->own_net;
  // cal only for bias weights: only err is useful contributor to this learning
  float dw = (cn->sravg_s - cn->sravg_m);
  cn->dwt += cur_lrate * dw;
}

inline void LeabraConSpec::B_Compute_dWt_CtLeabraCAL(LeabraCon* cn, LeabraUnit* ru,
						     LeabraLayer* rlay) {
  LeabraNetwork* net = (LeabraNetwork*)rlay->own_net;
  float dw = (net->sravg_vals.s_nrm * cn->sravg_s - net->sravg_vals.m_nrm * cn->sravg_m);
  cn->dwt += cur_lrate * dw;
}

/////////////////////////////////////
// Master Bias dWt function

inline void LeabraConSpec::B_Compute_Leabra_dWt(LeabraCon* cn, LeabraUnit* ru, LeabraLayer* rlay) {
  switch(learn_rule) {
  case LEABRA_CHL:
    B_Compute_dWt_LeabraCHL(cn, ru);
    break;
  case CTLEABRA_CAL:
    B_Compute_dWt_CtLeabraCAL(cn, ru, rlay);
    break;
  case CTLEABRA_XCAL:
    B_Compute_dWt_CtLeabraXCAL(cn, ru, rlay);
    break;
  case CTLEABRA_XCAL_C:
    B_Compute_dWt_CtLeabraXCAL_C(cn, ru, rlay);
    break;
  }
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
  if(xcal.lrn_var == XCalLearnSpec::CHL) {
    dw = ru->act_p - ru->act_m;
  }
  else {
    // cal only for bias weights: only err is useful contributor to this learning
    LeabraNetwork* net = (LeabraNetwork*)rlay->own_net;
    dw = (net->sravg_vals.s_nrm * cn->sravg_s - net->sravg_vals.m_nrm * cn->sravg_m);
  }
  if(fabsf(dw) >= dwt_thresh)
    cn->dwt += cur_lrate * dw;
}

inline void LeabraBiasSpec::B_Compute_dWt_CtLeabraXCAL_C(LeabraCon* cn, LeabraUnit* ru,
							 LeabraLayer* rlay) {
//  LeabraNetwork* net = (LeabraNetwork*)rlay->own_net;
  // cal only for bias weights: only err is useful contributor to this learning
  float dw = (cn->sravg_s - cn->sravg_m);
  if(fabsf(dw) >= dwt_thresh)
    cn->dwt += cur_lrate * dw;
}

inline void LeabraBiasSpec::B_Compute_dWt_CtLeabraCAL(LeabraCon* cn, LeabraUnit* ru,
						     LeabraLayer* rlay) {
  LeabraNetwork* net = (LeabraNetwork*)rlay->own_net;
  float dw = (net->sravg_vals.s_nrm * cn->sravg_s - net->sravg_vals.m_nrm * cn->sravg_m);
  if(fabsf(dw) >= dwt_thresh)
    cn->dwt += cur_lrate * dw;
}


//////////////////////////
//	Unit NetAvg   	//
//////////////////////////

inline void LeabraUnitSpec::Compute_ApplyInhib(LeabraUnit* u, LeabraNetwork*, float inhib_val) {
  // if you have a computed inhibition value, apply it full force, overwriting anything else
  u->g_i_raw = inhib_val;
  u->gc.i = inhib_val;
  u->prv_g_i = inhib_val;
}

inline float LeabraUnitSpec::Compute_IThreshStd(LeabraUnit* u, LeabraNetwork* net) {
  float non_bias_net = u->net;
  if(u->bias.size)		// subtract out bias weights so they can change k
    non_bias_net -= u->bias_scale * u->bias.OwnCn(0)->wt;
  // including the ga and gh terms
  return ((non_bias_net * e_rev_sub_thr.e + u->gc.l * e_rev_sub_thr.l
	   + u->gc.a * e_rev_sub_thr.a + u->gc.h * e_rev_sub_thr.h) /
	  (act.thr - e_rev.i));
} 

inline float LeabraUnitSpec::Compute_IThreshNoA(LeabraUnit* u, LeabraNetwork* net) {
  float non_bias_net = u->net;
  if(u->bias.size)		// subtract out bias weights so they can change k
    non_bias_net -= u->bias_scale * u->bias.OwnCn(0)->wt;
  // NOT including the ga term
  return ((non_bias_net * e_rev_sub_thr.e + u->gc.l * e_rev_sub_thr.l
	   + u->gc.h * e_rev_sub_thr.h) /
	  (act.thr - e_rev.i));
} 

inline float LeabraUnitSpec::Compute_IThreshNoH(LeabraUnit* u, LeabraNetwork* net) {
  float non_bias_net = u->net;
  if(u->bias.size)		// subtract out bias weights so they can change k
    non_bias_net -= u->bias_scale * u->bias.OwnCn(0)->wt;
  // NOT including the gh terms
  return ((non_bias_net * e_rev_sub_thr.e + u->gc.l * e_rev_sub_thr.l
	   + u->gc.a * e_rev_sub_thr.a) /
	  (act.thr - e_rev.i));
} 

inline float LeabraUnitSpec::Compute_IThreshNoAH(LeabraUnit* u, LeabraNetwork* net) {
  float non_bias_net = u->net;
  if(u->bias.size)		// subtract out bias weights so they can change k
    non_bias_net -= u->bias_scale * u->bias.OwnCn(0)->wt;
  // NOT including the ga and gh terms
  return ((non_bias_net * e_rev_sub_thr.e + u->gc.l * e_rev_sub_thr.l) /
	  (act.thr - e_rev.i));
} 

inline float LeabraUnitSpec::Compute_IThreshAll(LeabraUnit* u, LeabraNetwork* net) {
  // including the ga and gh terms and bias weights
  return ((u->net * e_rev_sub_thr.e + u->gc.l * e_rev_sub_thr.l
	   + u->gc.a * e_rev_sub_thr.a + u->gc.h * e_rev_sub_thr.h) /
	  (act.thr - e_rev.i));
} 

inline float LeabraUnitSpec::Compute_EqVm(LeabraUnit* u) {
  float new_v_m = (((u->net * e_rev.e) + (u->gc.l * e_rev.l) + (u->gc.i * e_rev.i) +
		   (u->gc.h * e_rev.h) + (u->gc.a * e_rev.a)) / 
		  (u->net + u->gc.l + u->gc.i + u->gc.h + u->gc.a));
  return new_v_m;
}


//////////////////////////////////
//	Leabra Wizard		//
//////////////////////////////////

class LEABRA_API LeabraWizard : public Wizard {
  // #STEM_BASE ##CAT_Leabra Leabra-specific wizard for automating construction of simulation objects
INHERITED(Wizard)
public:
  enum PBWMMode {
    GATE_BIAS,			// uses the gate biases and out_go_clear to provide a scaffolding of generally good gating dynamics, which learning can then shape as appropriate -- this is the current default behavior -- somewhat slower learning but more sensible overall behavior
    PROMISCUOUS,		// promiscuous Go firing with no gate_bias and no out_go_clear -- basically in perpetual updating mode -- learns very fast and reliably, but the resulting gating dynamic is often fairly random and hard to interpret -- this was the way it worked prior to 10/2009 and version 5.0.1 of the software
  };

  override bool StdNetwork(TypeDef* net_type = &TA_LeabraNetwork, Network* net = NULL);
  override bool	UpdateInputDataFmNet(Network* net, DataTable* data_table);

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
  virtual bool PVLV_OutToPVe(LeabraNetwork* net, LeabraLayer* output_layer,
				 bool disconnect = false);
  // #MENU_BUTTON #NO_SCOPE_1 make (or break if disconnect = true) connection between given output_layer in given network and the PVe layer, which uses this output layer together with the RewTarg layer input to automatically compute reward value based on performance
  virtual bool PVLV_ToLayerGroup(LeabraNetwork* net);
  // #MENU_BUTTON move all the PVLV layers to a PVLV layer group, which is the new default way of organizing these layers

  virtual bool 	PBWM(LeabraNetwork* net, bool da_mod_all = false,
		     int n_stripes=4, bool out_gate=true,
		     bool no_lrn_pfc=false);
  // #MENU_BUTTON #MENU_SEP_BEFORE configure all the layers and specs for the prefrontal-cortex basal ganglia working memory system (PBWM) -- does a PVLV configuration first (see PVLV for details) and then adds a basal ganglia gating system that is trained by PVLV dopamine signals.  The gating system determines when the PFC working memory representations are updated;  da_mod_all = have da value modulate all the regular units in the network; out_gate = each PFC layer has separate output gated layer and corresponding matrix output gates; nolrn_pfc = pfc does not learn -- just copies input acts directly (useful for demonstration but not as realistic or powerful)
  virtual bool 	PBWM_Mode(LeabraNetwork* net, PBWMMode mode);
  // #MENU_BUTTON change the general behavior of the model by adjusting a set of parameters in concert to achieve different overall dynamics -- network must already be configured as a PBWM model *WITH* output gating (difference only really applies in this case)

  virtual bool 	PBWM_CvtV1_impl(String& proj_str);
  // #IGNORE do the actual gsub conversions to V1
  virtual bool 	PBWM_CvtV1File(const String& proj_file_nm, bool load_after = true);
  // #MENU_BUTTON #CAT_File #EXT_proj #FILE_DIALOG_LOAD convert an older "version 1" (anything prior to version 5.0.1 of emergent) PBWM project file to use the V1 specs, which replicates the exact prior funcionality -- these projects can typically also be run using the new basic specs, using the PARSIMONIUS PBWM_Mode -- once converted, if load_after, then THIS FILE IS THEN LOADED OVER EXISTING PROJECT (and can then be saved to a new file name -- must be reloaded to get views to update) -- otherwise THE ORIGINAL PROJECT IS OVERWRITTEN

  virtual bool PBWM_SetNStripes(LeabraNetwork* net, int n_stripes, int n_units=-1);
  // #MENU_BUTTON #MENU_SEP_BEFORE set number of "stripes" (unit groups) throughout the entire set of pfc/bg layers (n_units = -1 = use current # of units)
  virtual bool PBWM_ToLayerGroups(LeabraNetwork* net);
  // #MENU_BUTTON move all the PBWM layers to PBWM_BG and PBWM_PFC layer groups, which is the new default way of organizing these layers
  virtual bool PBWM_Remove(LeabraNetwork* net);
  // #MENU_BUTTON remove all the PBWM (and PVLV) specific items from the network (specs and layers) -- can be useful for converting between PBWM versions -- ONLY works when layers are organized into groups

  override bool	StdProgs();
  override bool	TestProgs(Program* call_test_from, bool call_in_loop=true, int call_modulus=1);

  TA_BASEFUNS_NOCOPY(LeabraWizard);
private:
  void 	Initialize();
  void 	Destroy()	{ };
};

#endif // leabra_h
