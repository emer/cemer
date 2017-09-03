// Copyright 2017, Regents of the University of Colorado,
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
#include <ConGroup_inlines>
#include <LeabraConGroup>
#include <SpecMemberBase>

// member includes:
#include <Schedule>
#include <FunLookup>
#include <LeabraCon>
#include <LeabraUnitVars>
#include "ta_vector_ops.h"

// declare all other types mentioned but not required to include:
class LeabraUnit; // 
class LeabraLayer; // 
class LeabraConGroup; // 
class LeabraNetwork; // 
class DataTable; // 


eTypeDef_Of(WtScaleSpec);

class E_API WtScaleSpec : public SpecMemberBase {
  // ##INLINE ##NO_TOKENS ##CAT_Leabra weight scaling specification
INHERITED(SpecMemberBase)
public:
  float		abs;		// #DEF_1 #MIN_0 absolute scaling (not subject to normalization: directly multiplies weight values)
  float		rel;		// [Default: 1] #MIN_0 relative scaling that shifts balance between different projections (subject to normalization across all other projections into unit)
  bool          no_plus_net;    // #DEF_false do not send net input in the plus phase over this connection -- prevents this connection from driving error-driven learning in areas that it projects to -- typically used for projections to decoder output layers or in other specialized testing situations

  inline float	NetScale() 	{ return abs * rel; }

  float	SLayActScale(const float savg, const float lay_sz, const float n_cons);
  // compute scaling factor (new version) based on sending layer activity level (savg) and number of connections and overall layer size -- uses a fixed sem_extra standard-error-of-the-mean (SEM) extra value of 2 to add to the average expected number of active connections to receive, for purposes of computing scaling factors with partial connectivity -- for 25% layer activity, binomial SEM = sqrt(p(1-p)) = .43, so 3x = 1.3 so 2 is a reasonable default

  inline float	FullScale(const float savg, const float lay_sz, const float n_cons)
  { return NetScale() * SLayActScale(savg, lay_sz, n_cons); }
  // full scaling factor -- product of above two sub-factors

  String       GetTypeDecoKey() const override { return "ConSpec"; }

  TA_SIMPLE_BASEFUNS(WtScaleSpec);
protected:
  SPEC_DEFAULTS;
private:
  void	Initialize();
  void	Destroy()	{ };
  void	Defaults_init();
};

eTypeDef_Of(XCalLearnSpec);

class E_API XCalLearnSpec : public SpecMemberBase {
  // ##INLINE ##NO_TOKENS ##CAT_Leabra CtLeabra temporally eXtended Contrastive Attractor Learning (XCAL) specs
INHERITED(SpecMemberBase)
public:
  float         m_lrn;          // #DEF_1 #MIN_0 multiplier on learning based on the medium-term floating average threshold which produces error-driven learning -- this is typically 1 when error-driven learning is being used, and 0 when pure hebbian learning is used -- note that the long-term floating average threshold is provided by the receiving unit
  bool          set_l_lrn;      // #DEF_false if true, set a fixed l_lrn weighting factor that determines how much of the long-term floating average threshold (i.e., BCM, Hebbian) component of learning is used -- this is useful for setting a fully Hebbian learning connection, e.g., by setting m_lrn = 0 and l_lrn = 1. If false, then the receiving unit's avg_l_lrn factor is used, which dynamically modulates the amount of the long-term component as a function of how active overall it is
  float         l_lrn;          // #CONDSHOW_ON_set_l_lrn fixed l_lrn weighting factor that determines how much of the long-term floating average threshold (i.e., BCM, Hebbian) component of learning is used -- this is useful for setting a fully Hebbian learning connection, e.g., by setting m_lrn = 0 and l_lrn = 1. 
  float		d_rev;		// #DEF_0.1 #MIN_0 proportional point within LTD range where magnitude reverses to go back down to zero at zero -- err-driven svm component does better with smaller values, and BCM-like mvl component does better with larger values -- 0.1 is a compromise
  float		d_thr;		// #DEF_0.0001;0.01 #MIN_0 minimum LTD threshold value below which no weight change occurs -- small default value is mainly to optimize computation for the many values close to zero associated with inactive synapses
  float		d_rev_ratio;	// #HIDDEN #READ_ONLY -(1-d_rev)/d_rev -- multiplication factor in learning rule -- builds in the minus sign!

  inline float  dWtFun(const float srval, const float thr_p) {
    float rval;
    if(srval < d_thr)
      rval = 0.0f;
    else if(srval > thr_p * d_rev)
      rval = (srval - thr_p);
    else
      rval = srval * d_rev_ratio;
    return rval;
  }
  // XCAL function for weight change -- the "check mark" function 

  inline float  dWtFun_dgain(const float srval, const float thr_p, const float d_gain) {
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

  inline float  SymSbFun(const float wt_val) {
    return 2.0f * wt_val * (1.0f - wt_val);
  }
  // symmetric soft bounding function -- factor of 2 to equate with asymmetric sb for overall lrate at a weight value of .5 (= .5)

  inline float  LongLrate(const float ru_avg_l_lrn) {
    if(set_l_lrn) return l_lrn;
    return ru_avg_l_lrn;
  }
  // get the learning rate for long-term floating average component (BCM)

  String       GetTypeDecoKey() const override { return "ConSpec"; }

  SIMPLE_COPY(XCalLearnSpec);
  TA_BASEFUNS(XCalLearnSpec);
protected:
  SPEC_DEFAULTS;
  void	UpdateAfterEdit_impl() override;
private:
  void	Initialize();
  void 	Destroy()	{ };
  void	Defaults_init();
};

eTypeDef_Of(WtSigSpec);

class E_API WtSigSpec : public SpecMemberBase {
  // ##INLINE ##NO_TOKENS ##CAT_Leabra sigmoidal weight function specification
INHERITED(SpecMemberBase)
public:
  float		gain;		// #DEF_1;6 #MIN_0 gain (contrast, sharpness) of the weight contrast function (1 = linear)
  float		off;		// #DEF_1 #MIN_0 offset of the function (1=centered at .5, >1=higher, <1=lower) -- 1 is standard for XCAL
  bool          soft_bound;     // #DEF_true apply exponential soft bounding to the weight changes 

  static inline float	SigFun(const float w, const float gn, const float of) {
    if(w <= 0.0f) return 0.0f;
    if(w >= 1.0f) return 1.0f;
    return (1.0f / (1.0f + powf((of * (1.0f - w)) / w, gn)));
  }
  // static version of function for implementing weight sigmodid

  static inline float	SigFun61(const float w) {
    if(w <= 0.0f) return 0.0f;
    if(w >= 1.0f) return 1.0f;
    const float pwguy = (1.0f - w) / w;
    return (1.0f / (1.0f + pwguy*pwguy*pwguy*pwguy*pwguy*pwguy));
  }
  // static version of function for implementing weight sigmodid -- for default gain = 6, offset = 1 params

  static inline float	InvFun(const float w, const float gn, const float of) {
    if(w <= 0.0f) return 0.0f;
    if(w >= 1.0f) return 1.0f;
    return 1.0f / (1.0f + powf((1.0f - w) / w, 1.0f / gn) / of);
  }
  // static version of function for implementing inverse of weight sigmoid

  static inline float	InvFun61(const float w) {
    if(w <= 0.0f) return 0.0f;
    if(w >= 1.0f) return 1.0f;
    return 1.0f / (1.0f + powf((1.0f - w) / w, 1.0f / 6.0f));
  }
  // static version of function for implementing inverse of weight sigmoid -- for default gain = 6, offset = 1 params

  inline float	SigFmLinWt(float lw)
  { if(gain == 1.0f && off == 1.0f) return lw; return SigFun(lw, gain, off); }
  // get sigmoidal contrast-enhanced weight from linear weight
  
  inline float	LinFmSigWt(const float sw) { return InvFun(sw, gain, off); }
  // get linear weight from sigmoidal contrast-enhanced weight

  String       GetTypeDecoKey() const override { return "ConSpec"; }

  TA_SIMPLE_BASEFUNS(WtSigSpec);
protected:
  SPEC_DEFAULTS;
  void	UpdateAfterEdit_impl() override;
private:
  void	Initialize();
  void	Destroy()	{ };
  void	Defaults_init();
};

eTypeDef_Of(LeabraMomentum);

class E_API LeabraMomentum : public SpecMemberBase {
  // ##INLINE ##NO_TOKENS ##CAT_Leabra implements standard simple momentum and normalization by the overall running-average magnitude of weight changes (which serves as an estimate of the variance in the weight changes, assuming zero net mean overall) -- accentuates consistent directions of weight change and cancels out dithering -- biologically captures slower timecourse of longer-term plasticity mechanisms
INHERITED(SpecMemberBase)
public:
  bool          on;             // #DEF_true whether to use standard simple momentum and normalization as function of the running-average magnitude (abs value) of weight changes, which serves as an estimate of the variance in the weight changes, assuming zero net mean overall -- implements MAX_ABS version of normalization -- divide by MAX of abs dwt and slow decay of current aggregated value -- used in AdaMax algorithm of Kingman & Ba (2014)
  float         dwavg_tau;      // #CONDSHOW_ON_on #MIN_1 #DEF_1000;10000 time constant for integration of dwavg average of delta weights used in normalization factor -- generally should be long-ish, between 1000-10000 -- integration rate factor is 1/tau
  float         norm_min;       // #CONDSHOW_ON_on #MIN_0 #DEF_0.001 minimum effective value of the normalization factor -- provides a lower bound to how much normalization can be applied -- in backprop this is typically 1e-8 but larger values work better in Leabra
  float         m_tau;          // #CONDSHOW_ON_on #MIN_1 #DEF_10;20 time constant factor for integration of momentum -- 1/tau is dt (e.g., .1), and 1-1/tau (e.g., .95 or .9) is traditional momentum time-integration factor
  float         lrate_comp;     // #MIN_0 #CONDSHOW_ON_on #DEF_0.01 overall learning rate multiplier to compensate for changes due to use of normalization and momentum  -- allows for a common master learning rate to be used between different conditions: normalization contributes = .1 and momentum = .1, so combined = .01 -- should generally use that value 

  float         dwavg_dt;      // #READ_ONLY #EXPERT rate constant of delta-weight average integration = 1 / dwavg_tau
  float         dwavg_dt_c;    // #READ_ONLY #EXPERT complement rate constant of delta-weight average integration = 1 - (1 / dwavg_tau)
  float         m_dt;          // #READ_ONLY #EXPERT rate constant of momentum integration = 1 / m_tau
  float         m_dt_c;        // #READ_ONLY #EXPERT complement rate constant of momentum integration = 1 - (1 / m_tau)

  inline float ComputeMoment(float& moment, float& dwavg, const float new_dwt) {
    dwavg = fmaxf(dwavg_dt_c * dwavg, fabsf(new_dwt));
    moment = m_dt_c * moment + new_dwt;
    if(dwavg != 0.0f) {
      return moment / fmaxf(dwavg, norm_min);
    }
    return moment;
  }
  // compute momentum version of new dwt change -- return normalized momentum value
  
  String       GetTypeDecoKey() const override { return "ConSpec"; }

  TA_SIMPLE_BASEFUNS(LeabraMomentum);
protected:
  SPEC_DEFAULTS;
  void	UpdateAfterEdit_impl() override;
private:
  void	Initialize();
  void	Destroy()	{ };
  void	Defaults_init();
};

eTypeDef_Of(WtBalanceSpec);

class E_API WtBalanceSpec : public SpecMemberBase {
  // ##INLINE ##NO_TOKENS ##CAT_Leabra weight balance soft renormalization spec: maintains overall weight balance by progressively penalizing weight increases as a function of extent to which weight average exceeds high threshold value (and vice-versa for low threshold) -- plugs into soft bounding function -- see network times.bal_int for interval in trials of updating
INHERITED(SpecMemberBase)
public:
  bool          on;             // perform weight balance soft normalization?  if so, maintains overall weight balance across units by progressively penalizing weight increases as a function of extent to which sum of weights exceed high threshold value -- this is generally very beneficial for larger models where hog units are a problem, but not as much for smaller models where the additional cosntraints are not beneficial -- use renorm option to deal with overall weight decreases
  float         hi_thr;         // #CONDSHOW_ON_on #DEF_0.4 high threshold -- when average recv weights are above this threshold, weight increases are penalized in proportion to sigmoidal 1/(1+hi_gain*(avg-hi_thr)) function that saturates at maximum of 1 which means that there are no weight increases and all weight decreases -- weight decreases increase proportionally
  float         hi_gain;        // #CONDSHOW_ON_on #DEF_4 gain multiplier applied to above-threshold weight averages -- higher values turn weight increases down more rapidly as the weights become more imbalanced -- see hi_thr for equation
  float         lo_thr;         // #CONDSHOW_ON_on #DEF_0.2 low threshold -- when average recv weights are below this threshold, weight decreases are penalized in proportion to sigmoidal 1/(1+lo_gain*(lo-thr-avg)) function that saturates at maximum of 1 which means that there are no weight decreases and all weight increases -- weight increases increase proportionally
  float         lo_gain;        // #CONDSHOW_ON_on #DEF_4 gain multiplier applied to below-threshold weight averages -- higher values turn weight decreases down more rapidly as the weights become more imbalanced -- see hi_thr for equation
  
  inline void   WtBal(const float wt_avg, float& wb_inc, float& wb_dec) {
    if(wt_avg > hi_thr) {
      float wbi = hi_gain * (wt_avg - hi_thr);
      wb_inc = 1.0f / (1.0f + wbi); // gets sigmoidally small toward 0 as wbi gets smaller -- is quick acting but saturates -- apply pressure earlier..
      wb_dec = 2.0f - wb_inc; // as wb_inc goes down, wb_dec goes up..  sum to 2
    }
    else if(wt_avg < lo_thr) {
      float wbd = lo_gain * (lo_thr - wt_avg);
      wb_dec = 1.0f / (1.0f + wbd);
      wb_inc = 2.0f - wb_dec;
    }
    else {
      wb_inc = 1.0f;
      wb_dec = 1.0f;
    }
  }
  // compute weight balance factors for increase and decrease based on extent to which weights exceed thresholds
  
  String       GetTypeDecoKey() const override { return "ConSpec"; }

  TA_SIMPLE_BASEFUNS(WtBalanceSpec);
protected:
  SPEC_DEFAULTS;
  // void        UpdateAfterEdit_impl() override;
private:
  void        Initialize();
  void        Destroy()        { };
  void        Defaults_init();
};


eTypeDef_Of(AdaptWtScaleSpec);

class E_API AdaptWtScaleSpec : public SpecMemberBase {
  // ##INLINE ##NO_TOKENS ##CAT_Leabra parameters to adapt the scale multiplier on weights, as a function of weight value
INHERITED(SpecMemberBase)
public:
  bool          on;             // turn on weight scale adaptation as function of weight values
  float         tau;            // #CONDSHOW_ON_on #DEF_5000 time constant as a function of weight updates (trials) that weight scale adapts on -- should be fairly slow in general
  float         lo_thr;         // #CONDSHOW_ON_on #DEF_0.25 low threshold:  normalized contrast-enhanced effective weights (wt/scale, 0-1 range) below this value cause scale to move downward toward lo_scale value
  float         hi_thr;         // #CONDSHOW_ON_on #DEF_0.75 high threshold: normalized contrast-enhanced effective weights (wt/scale, 0-1 range) above this value cause scale to move upward toward hi_scale value
  float         lo_scale;       // #CONDSHOW_ON_on #MIN_0.01 #DEF_0.01 lowest value of scale
  float         hi_scale;       // #CONDSHOW_ON_on #DEF_2 highest value of scale

  float         dt;             // #READ_ONLY #EXPERT rate = 1 / tau
  
  inline void	AdaptWtScale(float& scale, const float wt) {
    const float nrm_wt = wt / scale;
    if(nrm_wt < lo_thr) {
      scale += dt * (lo_scale - scale);
    }
    else if(nrm_wt > hi_thr) {
      scale += dt * (hi_scale - scale);
    }
  }
  // adapt weight scale

  String       GetTypeDecoKey() const override { return "ConSpec"; }

  TA_SIMPLE_BASEFUNS(AdaptWtScaleSpec);
protected:
  SPEC_DEFAULTS;
  void	UpdateAfterEdit_impl() override;
private:
  void	Initialize();
  void	Destroy()	{ };
  void	Defaults_init();
};

eTypeDef_Of(SlowWtsSpec);

class E_API SlowWtsSpec : public SpecMemberBase {
  // ##INLINE ##NO_TOKENS ##CAT_Leabra slow weight specifications -- adds a more slowly-adapting weight factor on top of the standard more rapidly adapting weights
INHERITED(SpecMemberBase)
public:
  bool          on;             // are slow weights enabled?  if not, then there is just one effective weight value at each synapse, updated using the current cur_lrate factor (based on lrate plus any learning rate schedule)  if so, then a slowly adapting swt factor is updated to track the faster weight changes
  float         swt_pct;        // #CONDSHOW_ON_on #MIN_0 #MAX_1 what proportion the slowly adapting, more stable weight value contributes to the net effective weight value used in computing activations -- biologically this reflects the proportion of AMPA receptors that are stabilized vs. those that are subject to rapid insertion / removal
  int           slow_tau;      // #CONDSHOW_ON_on #DEF_100:3000 time #MIN_1 constant for adaptation of the slow weight value to track the fast weight value, in trials (computed at Compute_Weights) -- biologically the corresponding window of tag capture, and late-LTP transition times, are roughly 90 minutes or more, so for 100 msec trials = 10 trials per second, this is 54,000 trials -- but more typical values of around the size of an epoch are more appropriate for smaller more artificial networks
  float         wt_tau;        // #CONDSHOW_ON_on #DEF_1;4:60 #MIN_1 time constant for how quickly the effective weight (wt) adapts to changes in the fast weight values -- the biological rise time to maximum fast weight change is about 20 seconds, so for 100 msec trials = 10 trials per second, that is ~200 trials -- a value of 44 produces this result -- for more coarse-grained time scales, e.g., 1 trial / sec, scale proportionally, e.g., a value of 5 for 20 trials -- computationally this value is optional and 1 will negate its effects

  float         fwt_pct;       // #CONDSHOW_ON_on #READ_ONLY #EXPERT percent of fast contribution to effective weight = 1 - swt_pct
  float		slow_dt;	// #CONDSHOW_ON_on #READ_ONLY #EXPERT rate constant of slow updating = 1 / slow_tau
  float         wt_dt;          // #CONDSHOW_ON_on #READ_ONLY #EXPERT rate constant of wt adaptation = 1 / wt_tau

  String       GetTypeDecoKey() const override { return "ConSpec"; }

  TA_SIMPLE_BASEFUNS(SlowWtsSpec);
protected:
  SPEC_DEFAULTS;
  void	UpdateAfterEdit_impl() override;
private:
  void	Initialize();
  void	Destroy()	{ };
  void	Defaults_init();
};

eTypeDef_Of(DeepLrateSpec);

class E_API DeepLrateSpec : public SpecMemberBase {
  // ##INLINE ##NO_TOKENS ##CAT_Leabra learning rate specs for DeepLeabra -- effective learning rate can be enhanced for units receiving deep attentional modulation vs. those without
INHERITED(SpecMemberBase)
public:
  bool          on;             // enable the deep attentional differential learning rates based on deep_lrn modulation signal
  float         bg_lrate;       // #CONDSHOW_ON_on #MIN_0 learning rate multiplier for background cortico-cortical activations -- this is the baseline learning rate prior to adding in contribution of deep_lrn term: lrate_eff = lrate * (bg_lrate + fg_lrate * deep_lrn)
  float         fg_lrate;       // #CONDSHOW_ON_on #MIN_0 learning rate multiplier for foreground activations, as a function of deep_lrn activation level: lrate_eff = lrate * (bg_lrate + fg_lrate * deep_lrn)

  String       GetTypeDecoKey() const override { return "ConSpec"; }

  TA_SIMPLE_BASEFUNS(DeepLrateSpec);
protected:
  SPEC_DEFAULTS;
private:
  void	Initialize();
  void 	Destroy()	{ };
  void	Defaults_init();
};


eTypeDef_Of(MarginLearnSpec);

class E_API MarginLearnSpec : public SpecMemberBase {
  // ##INLINE ##NO_TOKENS ##CAT_Leabra learning specs as function of whether units are on the margin
INHERITED(SpecMemberBase)
public:
  bool          lrate_mod;      // enable the margin-based modulation of the learning rate -- marginal units have a learning rate of 1, while others are reduced
  float         stable_lrate;   // #CONDSHOW_ON_lrate_mod #MIN_0 #DEF_0.5 learning rate multiplier for units that are outside of the margin (either solidly active or inactive) -- reduced relative to those on the margin (which have the default learning rate)
  bool          sign_dwt;       // use the sign of the margin state (low vs. high marginal status) to drive an additional dwt learning factor
  float         sign_lrn;       // #CONDSHOW_ON_sign_dwt #MIN_0 #DEF_0.5 amount of learning for sign-based learning factor

  inline float  MarginLrate(const float marg) {
    if(marg == -2.0f || marg == 2.0f) return stable_lrate;
    return 1.0f;
  }
  // the margin-based learning rate multiplier

  inline float  SignDwt(const float marg) {
    if(marg == -2.0f || marg == 2.0f) return 0.0f;
    return marg;
  }
  // get the margin-based sign dwt factor -- filters out on and off values
  
  String       GetTypeDecoKey() const override { return "ConSpec"; }

  TA_SIMPLE_BASEFUNS(MarginLearnSpec);
protected:
  SPEC_DEFAULTS;
private:
  void	Initialize();
  void 	Destroy()	{ };
  void	Defaults_init();
};


eTypeDef_Of(LeabraConSpec);

class E_API LeabraConSpec : public ConSpec {
  // #STEM_BASE ##CAT_Leabra Leabra connection specs
INHERITED(ConSpec)
public:
  enum LeabraConVars {
    SCALE = N_CON_VARS,        // scaling paramter -- effective weight value is scaled by this factor -- useful for topographic connectivity patterns e.g., to enforce more distant connections to always be lower in magnitude than closer connections -- set by custom weight init code for certain projection specs
    DWAVG,                     // average of absolute value of computed dwt values over time -- serves as an estimate of variance in weight changes over time
    MOMENT,                    // momentum -- time-integrated dwt changes, to accumulate a consistent direction of weight change and cancel out dithering contradictory changes
    FWT,                       // fast learning linear (underlying) weight value -- learns according to the lrate specified in the connection spec -- this is converted into the effective weight value, "wt", via sigmoidal contrast enhancement (wt_sig)
    SWT,                       // slow learning linear (underlying) weight value -- learns more slowly from weight changes than fast weights, and fwt decays down to swt over time
    N_LEABRA_CON_VARS,         // #IGNORE number of leabra con vars
  };

  enum Quarters {              // #BITS specifies gamma frequency quarters within an alpha-frequency trial on which to do things
    QNULL = 0x00,              // #NO_BIT no quarter (yeah..)
    Q1 = 0x01,                 // first quarter
    Q2 = 0x02,                 // second quarter
    Q3 = 0x04,                 // third quarter -- posterior cortical minus phase
    Q4 = 0x08,                 // fourth quarter -- posterior cortical plus phase
    Q2_Q4 = Q2 | Q4,           // #NO_BIT standard beta frequency option, for bg, pfc
  };

  bool		inhib;		// #DEF_false #CAT_Activation makes the connection inhibitory (to g_i instead of net)
  WtScaleSpec	wt_scale;	// #CAT_Activation scale effective weight values to control the overall strength of a projection -- relative shifts balance among different projections, while absolute is a direct multipler

  bool		learn;		// #CAT_Learning #DEF_true individual control over whether learning takes place in this connection spec -- if false, no learning will take place regardless of any other settings -- if true, learning will take place if it is enabled at the network and other relevant levels
  Quarters      learn_qtr;      // #CAT_Learning #CONDSHOW_ON_learn quarters after which learning (Compute_dWt) should take place
  float		lrate;		// #CAT_Learning #CONDSHOW_ON_learn #DEF_0.04 #MIN_0  learning rate -- how fast the weights change per experience -- since version 7.8.5, a hidden factor of 2 has been removed, so this should be 2x what you used to use previously (e.g., default used to be .02, now is .04) -- this governs the rate of change for the fastest adapting weights -- see slow_wts for a more slowly adapting factor
  bool          use_lrate_sched; // #CAT_Learning #CONDSHOW_ON_learn use the lrate_sched learning rate schedule if one exists -- allows learning rate to change over time as a function of epoch count
  float		cur_lrate;	// #READ_ONLY #NO_INHERIT #SHOW #CONDSHOW_ON_learn #CAT_Learning current actual learning rate = lrate * lrate_sched current value (* 1 if no lrate_sched)
  float		lrs_mult;	// #READ_ONLY #NO_INHERIT #CAT_Learning learning rate multiplier obtained from the learning rate schedule
  Schedule	lrate_sched;	// #CAT_Learning schedule of learning rate over training epochs (NOTE: these factors (lrs_mult) multiply lrate to give the cur_lrate value)
  bool          use_unlearnable; // #CAT_Learning #CONDSHOW_ON_learn #AKA_ignore_unlearnable ignore unlearnable trials

  XCalLearnSpec	xcal;		// #CAT_Learning #CONDSHOW_ON_learn XCAL (eXtended Contrastive Attractor Learning) learning parameters
  WtSigSpec	wt_sig;		// #CAT_Learning #CONDSHOW_ON_learn sigmoidal weight function for contrast enhancement: high gain makes weights more binary & discriminative
  LeabraMomentum momentum;      // #CAT_Learning #CONDSHOW_ON_learn implements standard, simple momentum and normalization by the overall running-average magnitude of weight changes (which serves as an estimate of the variance in the weight changes, assuming zero net mean overall) -- accentuates consistent directions of weight change and cancels out dithering
  WtBalanceSpec wt_bal;         // #CAT_Learning #CONDSHOW_ON_learn weight balance maintenance spec: a soft form of normalization that maintains overall weight balance across units by progressively penalizing weight increases as a function of extent to which average weights exceed target value, and vice-versa when weight average is less than target -- alters rate of weight increases vs. decreases in soft bounding function
  AdaptWtScaleSpec adapt_scale;	// #CAT_Learning #CONDSHOW_ON_learn parameters to adapt the scale multiplier on weights, as a function of weight value
  SlowWtsSpec   slow_wts;       // #CAT_Learning #CONDSHOW_ON_learn slow weight specifications -- adds a more slowly-adapting weight factor on top of the standard more rapidly adapting weights
  DeepLrateSpec deep;		// #CAT_Learning #CONDSHOW_ON_learn learning rate specs for DeepLeabra learning rate modulation -- effective learning rate can be enhanced for units receiving thalamic modulation vs. those without
  MarginLearnSpec margin;	// #CAT_Learning #CONDSHOW_ON_learn learning specs for modulation as a function of marginal activation status -- emphasize learning for units on the margin
  FunLookup	wt_sig_fun;	// #HIDDEN #NO_SAVE #NO_INHERIT #CAT_Learning computes wt sigmoidal fun 
  FunLookup	wt_sig_fun_inv;	// #HIDDEN #NO_SAVE #NO_INHERIT #CAT_Learning computes inverse of wt sigmoidal fun
  WtSigSpec	wt_sig_fun_lst;	// #HIDDEN #NO_SAVE #NO_INHERIT #CAT_Learning last values of wt sig parameters for which the wt_sig_fun's were computed; prevents excessive updating
  float		wt_sig_fun_res;	// #HIDDEN #NO_SAVE #NO_INHERIT #CAT_Learning last values of resolution parameters for which the wt_sig_fun's were computed

  inline float	SigFmLinWt(float lw)
  { if(wt_sig.gain == 1.0f && wt_sig.off == 1.0f) return lw; return wt_sig_fun.Eval(lw); }
  // #CAT_Learning get contrast-enhanced weight from linear weight value
  inline float	LinFmSigWt(float sig_wt) { return wt_sig_fun_inv.Eval(sig_wt); }
  // #CAT_Learning get linear weight value from contrast-enhanced sigmoidal weight value

  inline  bool Quarter_LearnNow(int qtr)
  { return learn_qtr & (1 << qtr); }
  // #CAT_Learning test whether to learn at given quarter (pass net->quarter as arg)

  inline void Init_Weights(ConGroup* cg, Network* net, int thr_no) override {
    Init_Weights_symflag(net, thr_no);

    float* wts = cg->OwnCnVar(WT);
    float* dwts = cg->OwnCnVar(DWT);
    float* scales = cg->OwnCnVar(SCALE);
    // NOTE: it is ESSENTIAL that Init_Weights ONLY does wt, dwt, and scale -- all other vars
    // MUST be initialized in post -- projections with topo weights ONLY do these specific
    // variables but no others..

    int eff_thr_no = net->HasNetFlag(Network::INIT_WTS_1_THREAD) ? 0 : thr_no;
    
    const int sz = cg->size;
    for(int i=0; i<sz; i++) {
      scales[i] = 1.0f;         // default -- must be set in prjn spec if different
    }
    
    for(int i=0; i<sz; i++) {
      if(rnd.type != Random::NONE) {
        C_Init_Weight_Rnd(wts[i], eff_thr_no);
      }
      C_Init_dWt(dwts[i]);
    }
  }

  inline void Init_Weights_scale(ConGroup* rcg, Network* net, int thr_no, float init_wt_val)
    override {
    Init_Weights_symflag(net, thr_no);

    // this is called *receiver based*!!!

    int eff_thr_no = net->HasNetFlag(Network::INIT_WTS_1_THREAD) ? 0 : thr_no;
    
    const int sz = rcg->size;
    for(int i=0; i<sz; i++) {
      if(rnd.type != Random::NONE) {
        C_Init_Weight_Rnd(rcg->PtrCn(i, SCALE, net), eff_thr_no);
      }
      rcg->PtrCn(i, WT, net) = init_wt_val;
      C_Init_dWt(rcg->PtrCn(i, DWT, net));
    }
  }
  
  void  ApplySymmetry_s(ConGroup* cg, Network* net, int thr_no) override;

  inline void Init_Weights_rcgp(LeabraConGroup* cg, LeabraNetwork* net, int thr_no);
  // #IGNORE recv con group init weights -- for weight balance params
  
  inline void Init_Weights_post(ConGroup* cg, Network* net, int thr_no) override;

  void  LoadWeightVal(float wtval, ConGroup* cg, int cidx, Network* net) override {
    cg->Cn(cidx, WT, net) = wtval;
    float linwt = LinFmSigWt(wtval / cg->Cn(cidx, SCALE, net));
    cg->Cn(cidx, SWT, net) = linwt;
    cg->Cn(cidx, FWT, net) = linwt;
  }

  void  SetConScale(float scale, ConGroup* cg, int cidx, Network* net, int thr_no) override {
    cg->Cn(cidx, SCALE, net) = scale;
  }

  void  RenormScales(ConGroup* cg, Network* net, int thr_no, bool mult_norm,
                     float avg_wt) override;
  
  ///////////////////////////////////////////////////////////////
  //	Activation: Netinput -- only NetinDelta is supported

  virtual void	Compute_NetinScale(LeabraConGroup* recv_gp, LeabraLayer* from,
                                   bool plus_phase = false);
  // #IGNORE compute recv_gp->scale_eff based on params in from layer
  inline virtual bool  DoesStdNetin() { return true; }
  // #IGNORE does this connection send standard netinput? if so, it will be included in the CUDA send netin computation -- otherwise a separate function is required
  inline virtual bool  DoesStdDwt() { return true; }
  // #IGNORE does this connection compute a standard XCAL dWt function? if so, it will be included in the CUDA Compute_dWt computation -- otherwise a separate function is required
  inline virtual bool  IsMarkerCon() { return false; }
  // #IGNORE is this a marker con (MarkerConSpec) -- optimized check for higher speed
  inline virtual bool  IsDeepCtxtCon() { return false; }
  // #IGNORE is this a deep context connection (DeepCtxtConSpec) -- optimized check for higher speed
  inline virtual bool  IsDeepRawCon() { return false; }
  // #IGNORE is this a send deep_raw connection (SendDeepRawConSpec) -- optimized check for higher speed
  inline virtual bool  IsDeepModCon() { return false; }
  // #IGNORE is this a send deep_mod connection (SendDeepModConSpec) -- optimized check for higher speed

  inline void 	C_Send_NetinDelta(const float wt, float* send_netin_vec,
                                  const int ru_idx, const float su_act_delta_eff)
  { send_netin_vec[ru_idx] += wt * su_act_delta_eff; }
  // #IGNORE
#ifdef TA_VEC_USE
  inline void 	Send_NetinDelta_vec(LeabraConGroup* cg, const float su_act_delta_eff,
                                    float* send_netin_vec, const float* wts);
  // #IGNORE vectorized version
#endif
  inline void 	Send_NetinDelta_impl(LeabraConGroup* cg, LeabraNetwork* net,
                                     int thr_no, const float su_act_delta,
                                     const float* wts);
  // #IGNORE implementation that uses specified weights -- typically only diff in different subclasses is the weight variables used
  inline virtual void 	Send_NetinDelta(LeabraConGroup* cg, LeabraNetwork* net,
                                        int thr_no, const float su_act_delta);
  // #IGNORE #CAT_Activation sender-based delta-activation net input for con group (send net input to receivers) -- always goes into tmp matrix (thr_no >= 0!) and is then integrated into net through Compute_NetinInteg function on units

  // recv-based also needed for some statistics, but is NOT used for main compute code -- uses act_eq for sender act as well
  inline float 	C_Compute_Netin(const float wt, const float su_act)
  { return wt * su_act;	}
  // #IGNORE NOTE: doesn't work with spiking -- need a separate function to use act_eq for that case -- using act_eq does NOT work with scalarval etc
  inline float 	Compute_Netin(ConGroup* cg, Network* net, int thr_no) override;
  // #IGNORE

  ///////////////////////////////////////////////////////////////
  //	Learning

  /////////////////////////////////////
  // CtLeabraXCAL code

  inline void   GetLrates(LeabraConGroup* cg, float& clrate, bool& deep_on,
                          float& bg_lrate, float& fg_lrate);
  // #IGNORE get the current learning rates including layer-specific and potential deep modulations

  // todo: should go back and explore this at some point:
  // if(xcal.one_thr) {
  //   float eff_thr = ru_avg_l_lrn * ru_avg_l + (1.0f - ru_avg_l_lrn) * srm;
  //   eff_thr = fminf(eff_thr, 1.0f);
  //   dwt += clrate * xcal.dWtFun(srs, eff_thr);
  // }
  // also: fminf(ru_avg_l,1.0f) for threshold as an option..

  inline float 	C_Compute_dWt_CtLeabraXCAL
    (const float ru_avg_s, const float ru_avg_m, const float su_avg_s, const float su_avg_m,
     const float ru_avg_l, const float ru_avg_l_lrn, const float ru_margin) 
  { float srs = ru_avg_s * su_avg_s;
    float srm = ru_avg_m * su_avg_m;
    float new_dwt = (ru_avg_l_lrn * xcal.dWtFun(srs, ru_avg_l) +
                    xcal.m_lrn * xcal.dWtFun(srs, srm));
    if(margin.sign_dwt) {
      float mdwt = ru_avg_l_lrn * margin.sign_lrn * margin.SignDwt(ru_margin) * su_avg_s;
      new_dwt += mdwt;
    }
    return new_dwt;
  }
  // #IGNORE compute temporally eXtended Contrastive Attractor Learning (XCAL), returning new dwt

// #ifdef TA_VEC_USE
//   inline void Compute_dWt_CtLeabraXCAL_vec
//     (LeabraConGroup* cg, float* dwts, float* ru_avg_s, float* ru_avg_m, float* ru_avg_l,
//      float* ru_avg_l_lrn, float* ru_thal,
//      const bool deep_on, const float clrate, const float bg_lrate, const float fg_lrate,
//      const float su_avg_s, const float su_avg_m);
//   // #IGNORE vectorized version
// #endif

  inline void	Compute_dWt(ConGroup* cg, Network* net, int thr_no) override;

  inline void	C_Compute_Weights_CtLeabraXCAL
    (float& wt, float& dwt, float& fwt, float& swt, float& scale,
     const float wb_inc, const float wb_dec)
  {
    if(dwt == 0.0f) return;
    if(wt_sig.soft_bound) {
      if(dwt > 0.0f)	dwt *= wb_inc * (1.0f - fwt);
      else		dwt *= wb_dec * fwt;
    }
    else {
      if(dwt > 0.0f)	dwt *= wb_inc;
      else		dwt *= wb_dec;
    }
    fwt += dwt;
    C_ApplyLimits(fwt);
    // swt = fwt;  // leave swt as pristine original weight value -- saves time
    // and is useful for visualization!
    wt = scale * SigFmLinWt(fwt);
    dwt = 0.0f;

    if(adapt_scale.on) {
      adapt_scale.AdaptWtScale(scale, wt);
    }
  }
  // #IGNORE overall compute weights for CtLeabraXCAL learning rule -- no slow wts

  inline void	C_Compute_Weights_CtLeabraXCAL_slow
    (float& wt, float& dwt, float& fwt, float& swt, float& scale,
     const float wb_inc, const float wb_dec)
  { 
    if(dwt > 0.0f)	dwt *= wb_inc * (1.0f - fwt);
    else		dwt *= wb_dec * fwt;
    fwt += dwt;
    float eff_wt = slow_wts.swt_pct * swt + slow_wts.fwt_pct * fwt;
    float nwt = scale * SigFmLinWt(eff_wt);
    wt += slow_wts.wt_dt * (nwt - wt);
    swt += slow_wts.slow_dt * (fwt - swt);
    dwt = 0.0f;
    
    if(adapt_scale.on) {
      adapt_scale.AdaptWtScale(scale, wt);
    }
  }
  // #IGNORE overall compute weights for CtLeabraXCAL learning rule -- slow wts

// #ifdef TA_VEC_USE
//   inline void	Compute_Weights_CtLeabraXCAL_vec
//     (LeabraConGroup* cg, float* wts, float* dwts, float* fwts, float* swts, float* scales);
//   // #IGNORE overall compute weights for CtLeabraXCAL learning rule -- no fast wts -- vectorized
//   inline void	Compute_Weights_CtLeabraXCAL_slow_vec
//     (LeabraConGroup* cg, float* wts, float* dwts, float* fwts, float* swts, float* scales);
//   // #IGNORE overall compute weights for CtLeabraXCAL learning rule -- fast wts -- vectorized
// #endif

  inline void	Compute_Weights(ConGroup* cg, Network* net, int thr_no) override;

  inline virtual void 	Compute_WtBal(LeabraConGroup* cg, LeabraNetwork* net,
                                      int thr_no);
  // #IGNORE compute weight balance factors

  inline virtual void Compute_EpochWeights(LeabraConGroup* cg, LeabraNetwork* net,
                                           int thr_no) { };
  // #IGNORE compute epoch-level weights

  /////////////////////////////////////
  // 	Bias Weights

  // same as original:
  inline void B_Init_dWt(UnitVars* uv, Network* net, int thr_no) override {
    C_Init_dWt(uv->bias_dwt);
  }

  inline void B_Init_Weights_post(UnitVars* u, Network* net, int thr_no) override {
    LeabraUnitVars* uv = (LeabraUnitVars*)u;
    float wt = uv->bias_wt; uv->bias_swt = wt; uv->bias_fwt = wt;
  }

  inline void B_Compute_dWt(UnitVars* u, Network* net, int thr_no) override {
    if(!learn) return;
    LeabraUnitVars* uv = (LeabraUnitVars*)u;
    // only err is useful contributor to this learning
    float dw = uv->avg_s - uv->avg_m;
    uv->bias_dwt += cur_lrate * dw;
  }

  inline void B_Compute_Weights(UnitVars* u, Network* net, int thr_no) override {
    if(!learn) return;
    LeabraUnitVars* uv = (LeabraUnitVars*)u;
    float& wt =  uv->bias_wt;
    float& dwt = uv->bias_dwt;
    float& fwt = uv->bias_fwt;
    float& swt = uv->bias_swt;
    fwt += dwt;
    if(slow_wts.on) {
      float eff_wt = slow_wts.swt_pct * swt + slow_wts.fwt_pct * fwt;
      wt += slow_wts.wt_dt * (eff_wt - wt);
      swt += slow_wts.slow_dt * (fwt - swt);
    }
    else {
      swt = fwt;
      wt = fwt;
    }
    dwt = 0.0f;
    C_ApplyLimits(wt);
  }

  /////////////////////////////////////
  // General 

  inline void Compute_CopyWeights(ConGroup* cg, ConGroup* src_cg,
                                  LeabraNetwork* net);
  // #IGNORE copy weights from src_cg to cg -- typically used to compute synchronization of weights thought to take place during sleep -- typically in TI mode, where the Thal pathway synchronizes with the Super weights -- can be useful for any plus phase conveying weights to avoid positive feedback loop dynamics

  virtual void	Trial_Init_Specs(LeabraNetwork* net);
  // #CAT_Learning initialize specs and specs update network flags -- e.g., set current learning rate based on schedule given epoch (or error value)

  virtual void	CreateWtSigFun();
  // #CAT_Learning create the wt_sig_fun and wt_sig_fun_inv

  virtual void	LogLrateSched(int epcs_per_step = 50, int n_steps=5, int bump_step=-1);
  // #MENU_BUTTON #MENU_ON_LrateSched #CAT_Learning establish a logarithmic learning rate schedule with given total number of steps (including first step at lrate) and epochs per step: numbers go down in sequence: 1, .5, .2, .1, .05, .02, .01, etc.. this is a particularly good lrate schedule for large nets on hard tasks -- if bump_step > 0 (3 is a good default), the lrate bumps back up to 1 and back down to that step level, and then proceeds from there -- this can pop a model out of a local minimum and result in better final performance
  virtual void	ExpLrateSched(int epcs_per_step = 50, int n_steps=7, float pct_per_step = 0.5);
  // #MENU_BUTTON #MENU_ON_LrateSched #CAT_Learning establish an exponentially-decreasing learning rate schedule with given total number of steps (including first step at lrate) and epochs per step -- learning rate changes by pct_per_step at each step in epoch increment
  virtual void	LinearLrateSched(int epcs_per_step = 50, int n_steps=7, float final_lrate_factor = 0.01, bool interpolate = false);
  // #MENU_BUTTON #MENU_ON_LrateSched #CAT_Learning establish a linearly-decreasing learning rate schedule with given total number of steps (including first step at lrate) and epochs per step -- learning rate changes linearly to achieve the final lrate factor at the last step -- if interpolate is set then lrate changes every epoch according to linear slope, otherwise it just moves in discrete steps
  virtual void	TriangleLrateSched(int epcs_per_step = 50, int n_cycles=8, float low_factor = 0.1, int log_drop_cycles = 2, bool interpolate = true);
  // #MENU_BUTTON #MENU_ON_LrateSched #CAT_Learning establish a triangle "bump" lrate schedule that oscillates between 1 and low_factor lrate multipliers every 2 * epcs_per_step cycles, for given number of cycles, with a log-lrate-sched drop after given number of cycles -- based on Leslie Smith paper on triangle schedule, and expanding on our prior bump steps  idea -- interpolate allows lrate to change linearly every epoch to produce triangle effect -- otherwise it is just discrete oscillations -- first step is flat initial high lrate before cycling starts

  virtual void	GraphWtSigFun(DataTable* graph_data = NULL);
  // #MENU_BUTTON #MENU_ON_Graph #NULL_OK #NULL_TEXT_NewGraphData graph the sigmoidal weight contrast enhancement function (NULL = new data table)
  virtual void	GraphXCaldWtFun(DataTable* graph_data = NULL, float thr_p = 0.25);
  // #MENU_BUTTON #MENU_ON_Graph #NULL_OK #NULL_TEXT_NewGraphData graph the xcal dWt function for given threshold value (NULL = new data table)
  virtual void	GraphXCalSoftBoundFun(DataTable* graph_data = NULL);
  // #MENU_BUTTON #MENU_ON_Graph #NULL_OK #NULL_TEXT_NewGraphData graph the xcal soft weight bounding function (NULL = new data table)
  virtual void	GraphSlowWtsFun(int trials = 6000, DataTable* graph_data = NULL);
  // #MENU_BUTTON #MENU_ON_Graph #NULL_OK #NULL_TEXT_NewGraphData graph the trajectory of fast and slow weight change dynamics over trials, in response to a single .1 dwt change (NULL = new data table)
  virtual void	GraphLrateSched(DataTable* graph_data = NULL);
  // #MENU_BUTTON #MENU_ON_Graph #NULL_OK #NULL_TEXT_NewGraphData graph the current learning rate schedule

  virtual void 	WtScaleCvt(float slay_kwta_pct=.25, int slay_n_units=100,
                           int n_recv_cons=5, bool norm_con_n=true);
  // #MENU_BUTTON #MENU_ON_Graph helper for converting from old wt_scale computation to new one -- enter parameters for the sending layer kwta pct (overall layer activit), number of receiving connections, and whether the norm_con_n flag was on or off (effectively always on in new version) -- it reports what the effective weight scaling was in the old version, what it is in the new version, and what you should set the wt_scale.abs to to get equivalent performance, assuming wt_scale.abs reflects what was set previously

  bool          SaveConVarToWeights(ConGroup* cg, MemberDef* md) override;
  
  bool          CheckConfig_RecvCons(Projection* prjn, bool quiet=false) override;
  // check for for misc configuration settings required by different algorithms

  void          GetPrjnName(Projection& prjn, String& nm) override;
  String        GetToolbarName() const override { return "connect spec"; }

  void	InitLinks() override;
  SIMPLE_COPY(LeabraConSpec);
  TA_BASEFUNS(LeabraConSpec);
protected:
  SPEC_DEFAULTS;
  void	UpdateAfterEdit_impl() override;
private:
  void 	Initialize();
  void	Destroy()		{ };
  void	Defaults_init();
};

TA_SMART_PTRS(E_API, LeabraConSpec);

#endif // LeabraConSpec_h
