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

#ifndef LeabraConSpec_h
#define LeabraConSpec_h 1

// parent includes:
#include "network_def.h"
#include <ConSpec>
#include <SpecMemberBase>

// member includes:
#include <Schedule>
#include <FunLookup>
#include <LeabraSRAvgCon>

// declare all other types mentioned but not required to include:
class LeabraUnit; // 
class LeabraRecvCons; // 
class LeabraLayer; // 
class LeabraSendCons; // 
class LeabraNetwork; // 
class DataTable; // 


eTypeDef_Of(WtScaleSpec);

class E_API WtScaleSpec : public SpecMemberBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra weight scaling specification
INHERITED(SpecMemberBase)
public:
  float		abs;		// #DEF_1 #MIN_0 absolute scaling (not subject to normalization: directly multiplies weight values)
  float		rel;		// [Default: 1] #MIN_0 relative scaling that shifts balance between different projections (subject to normalization across all other projections into unit)
  int		sem_extra;	// #CONDSHOW_OFF_old #DEF_2 #MIN_0 standard-error-of-the-mean (SEM) extra value to add to the average expected number of active connections to receive, for purposes of computing scaling factors with partial connectivity -- for 25% layer activity, binomial SEM = sqrt(p(1-p)) = .43, so 3x = 1.3 so 2 is a reasonable default, but can use different value to make scaling work better

  inline float	NetScale() 	{ return abs * rel; }

  float	SLayActScale(float savg, float lay_sz, float n_cons);
  // compute scaling factor (new version) based on sending layer activity level (savg) and number of connections and overall layer size

  inline float	FullScale(float savg, float lay_sz, float n_cons)
  { return NetScale() * SLayActScale(savg, lay_sz, n_cons); }
  // full scaling factor -- product of above two sub-factors

  override String       GetTypeDecoKey() const { return "ConSpec"; }

  TA_SIMPLE_BASEFUNS(WtScaleSpec);
protected:
  SPEC_DEFAULTS;
private:
  void	Initialize();
  void	Destroy()	{ };
  void	Defaults_init();
};

eTypeDef_Of(WtScaleSpecInit);

class E_API WtScaleSpecInit : public SpecMemberBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra initial weight scaling values -- applied to active WtScaleSpec values during InitWeights -- useful for adapting scale values
INHERITED(SpecMemberBase)
public:
  bool		init;		// use these scaling values to initialize the wt_scale parameters during InitWeights (if false, these values have no effect at all)
  float		abs;		// #CONDSHOW_ON_init #DEF_1 #MIN_0 absolute scaling (not subject to normalization: directly multiplies weight values)
  float		rel;		// #CONDSHOW_ON_init [Default: 1] #MIN_0 relative scaling that shifts balance between different projections (subject to normalization across all other projections into unit)

  override String       GetTypeDecoKey() const { return "ConSpec"; }

  TA_SIMPLE_BASEFUNS(WtScaleSpecInit);
protected:
  SPEC_DEFAULTS;
private:
  void	Initialize();
  void	Destroy()	{ };
  void	Defaults_init() { };	// note: does NOT do any init -- these vals are not really subject to defaults in the usual way, so don't mess with them
};

eTypeDef_Of(WtSigSpec);

class E_API WtSigSpec : public SpecMemberBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra sigmoidal weight function specification
INHERITED(SpecMemberBase)
public:
  float		gain;		// #DEF_1;6 #MIN_0 gain (contrast, sharpness) of the weight contrast function (1 = linear)
  float		off;		// #DEF_1:1.25 #MIN_0 offset of the function (1=centered at .5, >1=higher, <1=lower) -- 1.25 is standard for Leabra CHL, 1.0 for XCAL
  bool		dwt_norm;	// #DEF_true normalize weight changes -- this adds a small amount of computational cost but generally makes learning more robust -- dwt -= (act_p / sum act_p) (sum dwt) over projection

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

  override String       GetTypeDecoKey() const { return "ConSpec"; }

  TA_SIMPLE_BASEFUNS(WtSigSpec);
protected:
  SPEC_DEFAULTS;
  void	UpdateAfterEdit_impl();
private:
  void	Initialize();
  void	Destroy()	{ };
  void	Defaults_init() { Initialize(); } // note: ConSpec defaults should modalize on learn_rule
};

eTypeDef_Of(LearnMixSpec);

class E_API LearnMixSpec : public SpecMemberBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra Leabra CHL mixture of learning factors (hebbian vs. error-driven) specification
INHERITED(SpecMemberBase)
public:
  float		hebb;		// [Default: .001] #MIN_0 amount of hebbian learning (should be relatively small, can be effective at .0001)
  float		err;		// #READ_ONLY #SHOW [Default: .999] amount of error driven learning, automatically computed to be 1-hebb
  bool		err_sb;		// #DEF_true apply exponential soft-bounding to the error learning component (applied in dWt)

  override String       GetTypeDecoKey() const { return "ConSpec"; }

  TA_SIMPLE_BASEFUNS(LearnMixSpec);
protected:
  SPEC_DEFAULTS;
  void	UpdateAfterEdit_impl();
private:
  void	Initialize();
  void	Destroy()	{ };
  void	Defaults_init() { };	// note: does NOT do any init -- these vals are not really subject to defaults in the usual way, so don't mess with them
};

eTypeDef_Of(XCalLearnSpec);

class E_API XCalLearnSpec : public SpecMemberBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS ##CAT_Leabra CtLeabra temporally eXtended Contrastive Attractor Learning (XCAL) specs
INHERITED(SpecMemberBase)
public:

  float		thr_l_mix;	// #DEF_0.001:1.0 [0.01 std] #MIN_0 #MAX_1 amount that long time-scale average contributes to the adaptive learning threshold -- this is the self-organizing BCM-like homeostatic component of learning -- remainder is thr_m_mix -- medium (trial-wise) time scale contribution, which reflects pure error-driven learning
  float		thr_m_mix;	// #READ_ONLY = 1 - thr_l_mix -- contribution of error-driven learning
  float		s_mix;		// #DEF_0.9 #MIN_0 #MAX_1 how much the short (plus phase) versus medium (trial) time-scale factor contributes to the synaptic activation term for learning -- s_mix just makes sure that plus-phase states are sufficiently long/important (e.g., dopamine) to drive strong positive learning to these states -- if 0 then svm term is also negated -- but vals < 1 are needed to ensure that when unit is off in plus phase (short time scale) that enough medium-phase trace remains to drive appropriate learning
  float		m_mix;		// #READ_ONLY 1-s_mix -- amount that medium time scale value contributes to synaptic activation level: see s_mix for details
  float		d_rev;		// #DEF_0.1 #MIN_0 proportional point within LTD range where magnitude reverses to go back down to zero at zero -- err-driven svm component does better with smaller values, and BCM-like mvl component does better with larger values -- 0.1 is a compromise
  float		d_thr;		// #DEF_0.0001;0.01 #MIN_0 minimum LTD threshold value below which no weight change occurs -- small default value is mainly to optimize computation for the many values close to zero associated with inactive synapses
  float		d_rev_ratio;	// #HIDDEN #READ_ONLY -(1-d_rev)/d_rev -- multiplication factor in learning rule -- builds in the minus sign!

  inline float  dWtFun(float srval, float thr_p) {
    float rval;
    if(srval < d_thr)
      rval = 0.0f;
    else if(srval > thr_p * d_rev)
      rval = (srval - thr_p);
    else
      rval = srval * d_rev_ratio;
    return rval;
  }

  inline float  dWtFun_dgain(float srval, float thr_p, float d_gain) {
    float rval;
    if(srval < d_thr)
      rval = 0.0f;
    else if(srval >= thr_p)
      rval = srval - thr_p;
    else if(srval > thr_p * d_rev)
      rval = d_gain * (srval - thr_p);
    else
      rval = d_gain * srval * d_rev_ratio;
    return rval;
  }
  // version that supports a differential d_gain term -- no longer included in base case

  inline float  SymSbFun(float wt_val) {
    return 2.0f * wt_val * (1.0f - wt_val);
  }
  // symmetric soft bounding function -- factor of 2 to equate with asymmetric sb for overall lrate at a weight value of .5 (= .5)

  override String       GetTypeDecoKey() const { return "ConSpec"; }

  SIMPLE_COPY(XCalLearnSpec);
  TA_BASEFUNS(XCalLearnSpec);
protected:
  SPEC_DEFAULTS;
  void	UpdateAfterEdit_impl();
private:
  void	Initialize();
  void 	Destroy()	{ };
  void	Defaults_init() { Initialize(); }
};

eTypeDef_Of(SAvgCorSpec);

class E_API SAvgCorSpec : public SpecMemberBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra sending average activation correction specifications: affects hebbian learning and netinput computation
INHERITED(SpecMemberBase)
public:
  float		cor;		// #DEF_0.4:0.8 #MIN_0 #MAX_1 proportion of correction to apply (0=none, 1=all, .5=half, etc)
  float		thresh;		// #DEF_0.001 #MIN_0 threshold of sending average activation below which learning does not occur (prevents learning when there is no input)
  bool		norm_con_n;	// #DEF_true #AKA_div_gp_n #OBSOLETE WARNING: this is now obsolete and only used if wt_scale.old = true -- in normalizing netinput, divide by the actual number of connections (recv group n), not the overall number of units in the sending layer -- THIS SHOULD ALWAYS BE ON AND IS THE NEW DEFAULT

  override String       GetTypeDecoKey() const { return "ConSpec"; }

  TA_SIMPLE_BASEFUNS(SAvgCorSpec);
protected:
  SPEC_DEFAULTS;
private:
  void	Initialize();
  void	Destroy()	{ };
  void	Defaults_init() { Initialize(); }
};

eTypeDef_Of(AdaptRelNetinSpec);

class E_API AdaptRelNetinSpec : public SpecMemberBase {
  // ##INLINE ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra parameters to adapt the relative netinput strength of different projections (to be used at epoch-level in AdaptRelNetin call, after AvgAbsRelNetin vals on projection have been computed)
INHERITED(SpecMemberBase)
public:
  bool		on;		// whether to adapt relative netinput values for this connection (only applied if AdaptAbsNetin is called, after AbsRelNetin and AvgAbsRelNetin)
  float		trg_fm_input;	// #CONDSHOW_ON_on (typically 0.85) #MIN_0 #MAX_1 target relative netinput for fm_input projections (set by Compute_TrgRelNetin fun): all such projections should sum to this amount (divide equally among them) -- this plus fm_output and lateral should sum to 1. if other types are missing, this is increased in proportion
  float		trg_fm_output;	// #CONDSHOW_ON_on (typically 0.10) #MIN_0 #MAX_1 target relative netwinput for fm_output projections (set by Compute_TrgRelNetin fun): all such projections should sum to this amount (divide equally among them) -- this plus fm_input and lateral should sum to 1. if other types are missing, this is increased in proportion
  float		trg_lateral;	// #CONDSHOW_ON_on (typically 0.05) #MIN_0 #MAX_1 target relative netinput for lateral projections (set by Compute_TrgRelNetin fun): all such projections should sum to this amount (divide equally among them)  -- this plus fm_input and lateral should sum to 1.  if other types are missing, this is increased in proportion
  float		trg_sum;	// #CONDSHOW_ON_on #READ_ONLY #SHOW sum of trg values -- should be 1!

  float		tol_lg;		// #CONDSHOW_ON_on #DEF_0.05 #MIN_0 #MAX_1 tolerance from target value (as a proportion of target value) on large numbers (>.25), within which parameters are not adapted
  float		tol_sm;		// #CONDSHOW_ON_on #DEF_0.2 #MIN_0 #MAX_1 tolerance from target value (as a proportion of target value) on small numbers (<.25), within which parameters are not adapted
  float		rel_lrate;	// #CONDSHOW_ON_on #DEF_0.2 #MIN_0 #MAX_1 adpatation 'learning' rate on wt_scale.rel parameter

  virtual bool	CheckInTolerance(float trg, float val);
  // check if value is inside the tolerance from trg

  override String       GetTypeDecoKey() const { return "ConSpec"; }

  TA_SIMPLE_BASEFUNS(AdaptRelNetinSpec);
protected:
  SPEC_DEFAULTS;
  void	UpdateAfterEdit_impl();
private:
  void	Initialize();
  void	Destroy()	{ };
  void	Defaults_init();	// does not change the on flag
};

eTypeDef_Of(LeabraConSpec);

class E_API LeabraConSpec : public ConSpec {
  // #STEM_BASE ##CAT_Leabra Leabra connection specs
INHERITED(ConSpec)
public:
  // IMPORTANT programming note: this enum must be same as in LeabraNetwork

  enum LearnRule {
    CTLEABRA_XCAL,		// Continuous-Time Leabra temporally eXtended Contrastive Attractor Learning rule, trial-based version, which has two time scales of contrasts: short-vs-medium (svm) and medium-vs-long (mvl): (<sr>_s - <sr>_m) + (<sr>_m - <r>_l) -- s=sender, r=recv, <> = avg over short (plus phase), medium (trial), long (epoch) time scales.  svm is basically error-driven learning, and mvl is BCM-style self-organizing learning.
    LEABRA_CHL,			// standard Leabra Contrastive Hebbian Learning rule with hebbian self-organizing factor: (s+r+) - (s-r-) + r+(s+ - w) -- s=sender,r=recv +=plus phase, -=minus phase, w= weight
    CTLEABRA_XCAL_C,		// Continuous-Time Leabra temporally eXtended Contrastive Attractor Learning rule, fully continuous version, which has two time scales of contrasts: short-vs-medium (svm) and medium-vs-long (mvl): (<sr>_s - <sr>_m) + (<sr>_m - <r>_l) -- s=sender, r=recv, <> = avg over short (plus phase), medium (trial), long (epoch) time scales.  svm is basically error-driven learning, and mvl is BCM-style self-organizing learning.
    CTLEABRA_CAL,		// Continuous-Time Leabra Contrastive Attractor Learning rule: <sr>_s - <sr>_m -- s=sender, r=recv, <> = avg over short (plus phase) and medium (trial) time scales -- purely error-driven but inhibitory oscillations can drive self-organizing component -- requires LeabraSRAvgCon connections
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
  bool          diff_scale_p;   // #CAT_Activation use a different wt_scale setting for the plus phase compared to the std wt_scale which is used only for the minus phase if this is checked
  WtScaleSpec	wt_scale_p;	// #CAT_Activation #CONDSHOW_ON_diff_scale_p plus phase only: scale effective weight values to control the overall strength of a projection -- relative shifts balance among different projections, while absolute is a direct multipler
  WtScaleSpecInit wt_scale_init;// #CAT_Activation initial values of wt_scale parameters, set during InitWeights -- useful for rel_net_adapt and abs_net_adapt (on LayerSpec)

  bool		learn;		// #CAT_Learning #DEF_true individual control over whether learning takes place in this connection spec -- if false, no learning will take place regardless of any other settings -- if true, learning will take place if it is enabled at the network and other relevant levels
  float		lrate;		// #CAT_Learning #DEF_0.01;0.02 #MIN_0 [0.01 for std Leabra, .02 for CtLeabra] #CONDSHOW_ON_learn learning rate -- how fast do the weights change per experience
  float		cur_lrate;	// #READ_ONLY #NO_INHERIT #SHOW #CAT_Learning current actual learning rate = lrate * lrate_sched current value (* 1 if no lrate_sched)
  LRSValue	lrs_value;	// #CAT_Learning #CONDSHOW_ON_learn what value to drive the learning rate schedule with (Important: affects values entered in start_ctr fields of schedule!)
  Schedule	lrate_sched;	// #CAT_Learning schedule of learning rate over training epochs or as a function of performance, as determined by lrs_value (NOTE: these factors multiply lrate to give the cur_lrate value)

  WtSigSpec	wt_sig;		// #CAT_Learning #CONDSHOW_ON_learn sigmoidal weight function for contrast enhancement: high gain makes weights more binary & discriminative
  LearnMixSpec	lmix;		// #CAT_Learning #CONDSHOW_ON_learn_rule:LEABRA_CHL&&learn mixture of hebbian & err-driven learning (note: no hebbian for CTLEABRA_XCAL)
  XCalLearnSpec	xcal;		// #CAT_Learning #CONDSHOW_ON_learn_rule:CTLEABRA_XCAL,CTLEABRA_XCAL_C&&learn XCAL (eXtended Contrastive Attractor Learning) learning parameters
  SAvgCorSpec	savg_cor;	// #CAT_Learning for Hebbian and netinput computation: correction for sending average act levels (i.e., renormalization); also norm_con_n for normalizing netinput computation

  AdaptRelNetinSpec rel_net_adapt; // #CAT_Learning #CONDSHOW_ON_learn adapt relative netinput values based on targets for fm_input, fm_output, and lateral projections -- not used by default (call Compute_RelNetinAdapt to activate; requires Compute_RelNetin and Compute_AvgRelNetin for underlying data)
  
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
    lcn->pdw = 0.0f; }
  inline override void Init_Weights(RecvCons* cg, Unit* ru) {
    ConSpec::Init_Weights(cg, ru);
    if(wt_scale_init.init) { wt_scale.abs = wt_scale_init.abs;
      wt_scale.rel = wt_scale_init.rel; } }

  ///////////////////////////////////////////////////////////////
  //	Activation: Netinput -- only NetinDelta is supported

  virtual void	Compute_NetinScale(LeabraRecvCons* recv_gp, LeabraLayer* from,
                                   bool plus_phase = false);
  // compute recv_gp->scale_eff based on params in from layer
  virtual bool  IsTICtxtCon() { return false; }
  // is this a TI context connection (LeabraTICtctConSpec) -- optimized check for higher speed

  inline void 	C_Send_NetinDelta_Thread(Connection* cn, float* send_netin_vec,
				      LeabraUnit* ru, float su_act_delta_eff);
  // #IGNORE
  inline void 	C_Send_NetinDelta_NoThread(Connection* cn, LeabraUnit* ru,
					float su_act_delta_eff);
  // #IGNORE
  virtual void 	Send_NetinDelta(LeabraSendCons* cg, LeabraNetwork* net, int thread_no,
				float su_act_delta);
  // #CAT_Activation sender-based delta-activation net input for con group (send net input to receivers) -- always goes into tmp matrix (thread_no >= 0!) and is then integrated into net through Compute_NetinInteg function on units

  // recv-based also needed for some statistics, but is NOT used for main compute code -- uses act_eq for sender act as well
  inline float 	C_Compute_Netin(LeabraCon* cn, LeabraUnit*, LeabraUnit* su);
  // #IGNORE
  inline float 	Compute_Netin(RecvCons* cg, Unit* ru);
  // #IGNORE

  ///////////////////////////////////////////////////////////////
  //	Learning

  /////////////////////////////////////
  // LeabraCHL code

  inline void 	Compute_SAvgCor(LeabraSendCons* cg, LeabraUnit* su);
  // #CAT_Learning #IGNORE compute hebb correction scaling term for sending average act (cg->savg_cor) based on layer target activity percent

  inline float	C_Compute_Hebb(LeabraCon* cn, LeabraSendCons* cg, float lin_wt,
			       float ru_act, float su_act);
  // #CAT_Learning #IGNORE compute Hebbian associative learning

  inline float 	C_Compute_Err_LeabraCHL(LeabraCon* cn, float lin_wt,
					float ru_act_p, float ru_act_m,
					float su_act_p, float su_act_m);
  // #CAT_Learning #IGNORE compute generec error term, sigmoid case

  inline void 	C_Compute_dWt(LeabraCon* cn, LeabraUnit* su, float heb, float err);
  // #CAT_Learning #IGNORE combine associative and error-driven weight change, actually update dwt
  inline void 	C_Compute_dWt_NoHebb(LeabraCon* cn, LeabraUnit* su, float err);
  // #CAT_Learning #IGNORE just error-driven weight change, actually update dwt
  virtual void 	Compute_dWt_LeabraCHL(LeabraSendCons* cg, LeabraUnit* su);
  // #CAT_Learning Leabra/CHL weight changes

  inline void	C_Compute_Weights_LeabraCHL(LeabraCon* cn);
  // #CAT_Learning #IGNORE compute weights for LeabraCHL learning rule
  virtual void	Compute_Weights_LeabraCHL(LeabraSendCons* cg, LeabraUnit* su);
  // #CAT_Learning overall compute weights for LeabraCHL learning rule

  /////////////////////////////////////
  // CtLeabraXCAL code

  inline void 	C_Compute_dWt_CtLeabraXCAL_trial(LeabraCon* cn, LeabraUnit* ru,
				 float su_avg_s, float su_avg_m, float su_act_mult);
  // #CAT_Learning #IGNORE compute temporally eXtended Contrastive Attractor Learning (XCAL) -- separate computation of sr averages -- trial-wise version 
  virtual void 	Compute_dWt_CtLeabraXCAL(LeabraSendCons* cg, LeabraUnit* su);
  // #CAT_Learning CtLeabraXCAL weight changes

  inline void	C_Compute_Weights_CtLeabraXCAL(LeabraCon* cn);
  // #CAT_Learning #IGNORE overall compute weights for CtLeabraXCAL learning rule
  virtual void	Compute_Weights_CtLeabraXCAL(LeabraSendCons* cg, LeabraUnit* su);
  // #CAT_Learning overall compute weights for CtLeabraXCAL learning rule


  /////////////////////////////////////
  // CtLeabraXCalC code -- note that this is RECEIVER BASED due to triggered nature of learning

  inline void 	C_Compute_dWt_CtLeabraXCalC(LeabraCon* cn, LeabraUnit* ru, LeabraUnit* su);
  // #CAT_Learning #IGNORE compute temporally eXtended Contrastive Attractor Learning -- fully continuous version (XCAL_C)
  virtual void 	Compute_dWt_CtLeabraXCalC(LeabraRecvCons* cg, LeabraUnit* ru);
  // #CAT_Learning compute temporally eXtended Contrastive Attractor Learning -- fully continuous version (XCAL_C)

  /////////////////////////////////////
  // CtLeabraCAL code

  inline void C_Compute_SRAvg_m(LeabraSRAvgCon* cn, float ru_act, float su_act);
  // #IGNORE accumulate sender-receiver activation product average -- medium (trial-level) time scale
  inline void C_Compute_SRAvg_ms(LeabraSRAvgCon* cn, float ru_act, float su_act);
  // #IGNORE accumulate sender-receiver activation product average -- medium (trial-level) and short (plus phase) time scales
  virtual void Compute_SRAvg(LeabraSendCons* cg, LeabraUnit* su, bool do_s);
  // accumulate sender-receiver activation product average -- only for CtLeabraCAL

  inline void C_Trial_Init_SRAvg(LeabraSRAvgCon* cn);
  // #IGNORE initialize sender-receiver activation product averages for trial and below 
  virtual void Trial_Init_SRAvg(LeabraSendCons* cg, LeabraUnit* su);
  // initialize sender-receiver activation product average (only for trial-wise mode, else just in init weights) -- called at start of trial

  inline void 	C_Compute_dWt_CtLeabraCAL(LeabraSRAvgCon* cn,
					  float sravg_s_nrm, float sravg_m_nrm);
  // #CAT_Learning #IGNORE compute Contrastive Attractor Learning (CAL)
  virtual void 	Compute_dWt_CtLeabraCAL(LeabraSendCons* cg, LeabraUnit* su);
  // #CAT_Learning CtLeabraCAL weight changes

  inline void	C_Compute_Weights_CtLeabraCAL(LeabraSRAvgCon* cn);
  // #CAT_Learning #IGNORE overall compute weights for CtLeabraCAL learning rule
  virtual void	Compute_Weights_CtLeabraCAL(LeabraSendCons* cg, LeabraUnit* su);
  // #CAT_Learning overall compute weights for CtLeabraCAL learning rule

  /////////////////////////////////////
  // Master dWt, Weights functions

  inline void	Compute_Leabra_dWt(LeabraSendCons* cg, LeabraUnit* su);
  // #CAT_Learning #IGNORE overall compute delta-weights for Leabra -- just a switch on learn rule to select above algorithm-specific variant

  inline void	Compute_Leabra_Weights(LeabraSendCons* cg, LeabraUnit* su);
  // #CAT_Learning #IGNORE overall compute weights for Leabra -- just a switch on learn rule to select above algorithm-specific variant

  virtual void 	Compute_dWt_Norm(LeabraRecvCons* cg, LeabraUnit* ru);
  // #CAT_Learning compute dwt normalization

  /////////////////////////////////////
  // 	Bias Weights

  virtual void	B_Compute_dWt_LeabraCHL(LeabraCon* cn, LeabraUnit* ru);
  // #CAT_Learning compute bias weight change for netin model of bias weight

  virtual void	B_Compute_dWt_CtLeabraXCAL(LeabraCon* cn, LeabraUnit* ru,
						 LeabraLayer* rlay);
  // #CAT_Learning compute bias weight change for XCAL rule
  virtual void	B_Compute_dWt_CtLeabraCAL(LeabraCon* cn, LeabraUnit* ru,
					  LeabraLayer* rlay);
  // #CAT_Learning compute bias weight change for CAL rule

  inline void	B_Compute_Leabra_dWt(LeabraCon* cn, LeabraUnit* ru, LeabraLayer* rlay);
  // #CAT_Learning #IGNORE overall compute bias delta-weights for Leabra -- just a switch on learn rule to select above algorithm-specific variant

  virtual void	B_Compute_Weights(LeabraCon* cn, LeabraUnit* ru);
  // #CAT_Learning #IGNORE update weights for bias connection (same for all algos)

  /////////////////////////////////////
  // General 

  virtual void Compute_CycSynDep(LeabraSendCons* cg, LeabraUnit* su) { };
  // #CAT_Activation compute cycle-level synaptic depression (must be defined by appropriate subclass) -- called at end of each cycle of computation if net_misc.cyc_syn_dep is on.

  virtual void	SetCurLrate(LeabraNetwork* net, int epoch);
  // #CAT_Learning set current learning rate based on schedule given epoch (or error value)
  virtual void	SetLearnRule(LeabraNetwork* net);
  // #CAT_Learning set current learning rule from the network

  virtual void	CreateWtSigFun();
  // #CAT_Learning create the wt_sig_fun and wt_sig_fun_inv

  virtual void	LogLrateSched(int epcs_per_step = 50, float n_steps=7);
  // #BUTTON #CAT_Learning establish a logarithmic learning rate schedule with given total number of steps (including first step at lrate) and epochs per step: numbers go down in sequence: 1, .5, .2, .1, .05, .02, .01, etc.. this is a particularly good lrate schedule for large nets on hard tasks

  virtual void	GraphWtSigFun(DataTable* graph_data = NULL);
  // #BUTTON #NULL_OK #NULL_TEXT_NewGraphData graph the sigmoidal weight contrast enhancement function (NULL = new data table)
  virtual void	GraphXCaldWtFun(DataTable* graph_data = NULL, float thr_p = 0.25);
  // #BUTTON #NULL_OK #NULL_TEXT_NewGraphData graph the xcal dWt function for given threshold value (NULL = new data table)
  virtual void	GraphXCalSoftBoundFun(DataTable* graph_data = NULL);
  // #BUTTON #NULL_OK #NULL_TEXT_NewGraphData graph the xcal soft weight bounding function (NULL = new data table)

  virtual void 	WtScaleCvt(float slay_kwta_pct=.25, int slay_n_units=100,
                           int n_recv_cons=5, bool norm_con_n=true);
  // #BUTTON helper for converting from old wt_scale computation to new one -- enter parameters for the sending layer kwta pct (overall layer activit), number of receiving connections, and whether the norm_con_n flag was on or off (effectively always on in new version) -- it reports what the effective weight scaling was in the old version, what it is in the new version, and what you should set the wt_scale.abs to to get equivalent performance, assuming wt_scale.abs reflects what was set previously

  override bool CheckConfig_RecvCons(RecvCons* cg, bool quiet=false);
  // check for for misc configuration settings required by different algorithms

  void	InitLinks();
  SIMPLE_COPY(LeabraConSpec);
  TA_BASEFUNS(LeabraConSpec);
protected:
  SPEC_DEFAULTS;
  void	UpdateAfterEdit_impl();
private:
  void 	Initialize();
  void	Destroy()		{ };
  void	Defaults_init();
};

TA_SMART_PTRS(LeabraConSpec);

#endif // LeabraConSpec_h
