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

// this contains all full classes that appear as members of _core specs
// it must be included directly in LeabraConSpec.h, _cpp.h, _cuda.h
// the STATE_CLASS macro will define a _cpp _cuda or plain (no suffix) version

// this pragma ensures that maketa properly grabs this type information even though
// this file is included in the other files -- we get ta info for main and _cpp, not cuda
#pragma maketa_file_is_target LeabraConSpec


class STATE_CLASS(WtScaleSpec) : public STATE_CLASS(SpecMemberBase) {
  // ##INLINE ##NO_TOKENS ##CAT_Leabra weight scaling specification
INHERITED(SpecMemberBase)
public:
  float		abs;		// #DEF_1 #MIN_0 absolute scaling (not subject to normalization: directly multiplies weight values)
  float		rel;		// [Default: 1] #MIN_0 relative scaling that shifts balance between different projections (subject to normalization across all other projections into unit)
  bool          no_plus_net;    // #DEF_false do not send net input in the plus phase over this connection -- prevents this connection from driving error-driven learning in areas that it projects to -- typically used for projections to decoder output layers or in other specialized testing situations

  INLINE float	NetScale() 	{ return abs * rel; }

  INLINE float	SLayActScale(const float savg, const float lay_sz, const float n_cons) {
    const float sem_extra = 2.0f;    // nobody ever modifies this, so keep it const
    int slay_act_n = (int)(savg * lay_sz + .5f); // sending layer actual # active
    slay_act_n = MAX(slay_act_n, 1);
    float rval = 1.0f;
    if(n_cons == lay_sz) {
      rval = 1.0f / (float)slay_act_n;
    }
    else {
      int r_max_act_n = MIN((int)n_cons, slay_act_n); // max number we could get
      int r_avg_act_n = (int)(savg * n_cons + .5f);// recv average actual # active if uniform
      r_avg_act_n = MAX(r_avg_act_n, 1);
      int r_exp_act_n = r_avg_act_n + sem_extra;
      r_exp_act_n = MIN(r_exp_act_n, r_max_act_n);
      rval = 1.0f / (float)r_exp_act_n;
    }
    return rval;
  }
  // compute scaling factor (new version) based on sending layer activity level (savg) and number of connections and overall layer size -- uses a fixed sem_extra standard-error-of-the-mean (SEM) extra value of 2 to add to the average expected number of active connections to receive, for purposes of computing scaling factors with partial connectivity -- for 25% layer activity, binomial SEM = sqrt(p(1-p)) = .43, so 3x = 1.3 so 2 is a reasonable default

  INLINE float	FullScale(const float savg, const float lay_sz, const float n_cons)
  { return NetScale() * SLayActScale(savg, lay_sz, n_cons); }
  // full scaling factor -- product of above two sub-factors

  STATE_DECO_KEY("ConSpec");
  STATE_TA_STD_CODE_SPEC(WtScaleSpec);
private:
  void	Initialize() {
    rel = 1.0f;    abs = 1.0f;
    Defaults_init();
  }
  void	Defaults_init() {  no_plus_net = false; }
};


class STATE_CLASS(XCalLearnSpec) : public STATE_CLASS(SpecMemberBase) {
  // ##INLINE ##NO_TOKENS ##CAT_Leabra CtLeabra temporally eXtended Contrastive Attractor Learning (XCAL) specs
INHERITED(SpecMemberBase)
public:
  float         m_lrn;          // #DEF_1 #MIN_0 multiplier on learning based on the medium-term floating average threshold which produces error-driven learning -- this is typically 1 when error-driven learning is being used, and 0 when pure hebbian learning is used -- note that the long-term floating average threshold is provided by the receiving unit
  bool          set_l_lrn;      // #DEF_false if true, set a fixed l_lrn weighting factor that determines how much of the long-term floating average threshold (i.e., BCM, Hebbian) component of learning is used -- this is useful for setting a fully Hebbian learning connection, e.g., by setting m_lrn = 0 and l_lrn = 1. If false, then the receiving unit's avg_l_lrn factor is used, which dynamically modulates the amount of the long-term component as a function of how active overall it is
  float         l_lrn;          // #CONDSHOW_ON_set_l_lrn fixed l_lrn weighting factor that determines how much of the long-term floating average threshold (i.e., BCM, Hebbian) component of learning is used -- this is useful for setting a fully Hebbian learning connection, e.g., by setting m_lrn = 0 and l_lrn = 1. 
  float		d_rev;		// #DEF_0.1 #MIN_0 proportional point within LTD range where magnitude reverses to go back down to zero at zero -- err-driven svm component does better with smaller values, and BCM-like mvl component does better with larger values -- 0.1 is a compromise
  float		d_thr;		// #DEF_0.0001;0.01 #MIN_0 minimum LTD threshold value below which no weight change occurs -- small default value is mainly to optimize computation for the many values close to zero associated with inactive synapses
  float         lrn_thr;        // #DEF_0.01 xcal learning threshold -- don't learn when sending unit activation is below this value in both phases -- due to the nature of the learning function being 0 when the sr coproduct is 0, it should not affect learning in any substantial way -- nonstandard learning algorithms that have different properties should ignore it
  float         s_mult;        // multiplicative factor on short-time-scale average activations -- to compensate for overall average differences in activation level at end of settling vs. earlier -- needed especially for adaptation mechanism

  float		d_rev_ratio;	// #HIDDEN #READ_ONLY -(1-d_rev)/d_rev -- multiplication factor in learning rule -- builds in the minus sign!

  INLINE float  dWtFun(const float srval, const float thr_p) {
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

  INLINE float  dWtFun_dgain(const float srval, const float thr_p, const float d_gain) {
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

  INLINE float  SymSbFun(const float wt_val) {
    return 2.0f * wt_val * (1.0f - wt_val);
  }
  // symmetric soft bounding function -- factor of 2 to equate with asymmetric sb for overall lrate at a weight value of .5 (= .5)

  INLINE float  LongLrate(const float ru_avg_l_lrn) {
    if(set_l_lrn) return l_lrn;
    return ru_avg_l_lrn;
  }
  // get the learning rate for long-term floating average component (BCM)

  STATE_DECO_KEY("ConSpec");
  STATE_TA_STD_CODE_SPEC(XCalLearnSpec);
  STATE_UAE( d_rev_ratio = (1.0f - d_rev) / d_rev;
             if(d_rev > 0.0f)
               d_rev_ratio = -(1.0f - d_rev) / d_rev;
             else
               d_rev_ratio = -1.0f;
             );
  
private:
  void	Initialize() {   Defaults_init(); }
  void	Defaults_init() {
    m_lrn = 1.0f;  set_l_lrn = false;  l_lrn = 1.0f;  d_rev = 0.10f;  d_thr = 0.0001f;
    lrn_thr = 0.01f; s_mult = 1.0f; d_rev_ratio = -(1.0f - d_rev) / d_rev;
  }
};


class STATE_CLASS(WtSigSpec) : public STATE_CLASS(SpecMemberBase) {
  // ##INLINE ##NO_TOKENS ##CAT_Leabra sigmoidal weight function specification
INHERITED(SpecMemberBase)
public:
  float		gain;		// #DEF_1;6 #MIN_0 gain (contrast, sharpness) of the weight contrast function (1 = linear)
  float		off;		// #DEF_1 #MIN_0 offset of the function (1=centered at .5, >1=higher, <1=lower) -- 1 is standard for XCAL
  bool          soft_bound;     // #DEF_true apply exponential soft bounding to the weight changes 

  static INLINE float	SigFun(const float w, const float gn, const float of) {
    if(w <= 0.0f) return 0.0f;
    if(w >= 1.0f) return 1.0f;
    return (1.0f / (1.0f + powf((of * (1.0f - w)) / w, gn)));
  }
  // static version of function for implementing weight sigmodid

  static INLINE float	SigFun61(const float w) {
    if(w <= 0.0f) return 0.0f;
    if(w >= 1.0f) return 1.0f;
    const float pwguy = (1.0f - w) / w;
    return (1.0f / (1.0f + pwguy*pwguy*pwguy*pwguy*pwguy*pwguy));
  }
  // static version of function for implementing weight sigmodid -- for default gain = 6, offset = 1 params

  static INLINE float	InvFun(const float w, const float gn, const float of) {
    if(w <= 0.0f) return 0.0f;
    if(w >= 1.0f) return 1.0f;
    return 1.0f / (1.0f + powf((1.0f - w) / w, 1.0f / gn) / of);
  }
  // static version of function for implementing inverse of weight sigmoid

  static INLINE float	InvFun61(const float w) {
    if(w <= 0.0f) return 0.0f;
    if(w >= 1.0f) return 1.0f;
    return 1.0f / (1.0f + powf((1.0f - w) / w, 1.0f / 6.0f));
  }
  // static version of function for implementing inverse of weight sigmoid -- for default gain = 6, offset = 1 params

  INLINE float	SigFmLinWt(float lw) {
    if(gain == 1.0f && off == 1.0f) return lw;
    if(gain == 6.0f && off == 1.0f) return SigFun61(lw);
    return SigFun(lw, gain, off);
  }
  // get sigmoidal contrast-enhanced weight from linear weight
  
  INLINE float	LinFmSigWt(const float sw) {
    if(gain == 1.0f && off == 1.0f) return sw;
    if(gain == 6.0f && off == 1.0f) return InvFun61(sw);
    return InvFun(sw, gain, off);
  }
  // get linear weight from sigmoidal contrast-enhanced weight

  STATE_DECO_KEY("ConSpec");
  STATE_TA_STD_CODE_SPEC(WtSigSpec);
private:
  void	Initialize()    {  Defaults_init(); }
  void	Defaults_init() {  gain = 6.0f;  off = 1.0f;  soft_bound = true; }
};


class STATE_CLASS(LeabraMomentum) : public STATE_CLASS(SpecMemberBase) {
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

  INLINE float ComputeMoment(float& moment, float& dwavg, const float new_dwt) {
    dwavg = fmaxf(dwavg_dt_c * dwavg, fabsf(new_dwt));
    moment = m_dt_c * moment + new_dwt;
    if(dwavg != 0.0f) {
      return moment / fmaxf(dwavg, norm_min);
    }
    return moment;
  }
  // compute momentum version of new dwt change -- return normalized momentum value

  INLINE void   UpdtVals() {
    dwavg_dt = 1.0f / dwavg_tau;  dwavg_dt_c = 1.0f - dwavg_dt;  m_dt = 1.0f / m_tau;
    m_dt_c = 1.0f - m_dt;
  }
  // #IGNORE
  
  STATE_DECO_KEY("ConSpec");
  STATE_TA_STD_CODE_SPEC(LeabraMomentum);
  STATE_UAE( UpdtVals(); );
private:
  void	Initialize()    {   Defaults_init(); }
  void	Defaults_init() {
    on = true; dwavg_tau = 1000.0f;  norm_min = 0.001f;  m_tau = 20.0f;  lrate_comp = 0.01f;
    UpdtVals();
  }
};


class STATE_CLASS(WtBalanceSpec) : public STATE_CLASS(SpecMemberBase) {
  // ##INLINE ##NO_TOKENS ##CAT_Leabra weight balance soft renormalization spec: maintains overall weight balance by progressively penalizing weight increases as a function of how strong the weights are overall (subject to thresholding) and long time-averaged activation -- plugs into soft bounding function -- see network times.bal_int for interval in trials of updating
INHERITED(SpecMemberBase)
public:
  bool          on;             // perform weight balance soft normalization?  if so, maintains overall weight balance across units by progressively penalizing weight increases as a function of amount of averaged weight above a high threshold (hi_thr) and long time-average activation above an act_thr -- this is generally very beneficial for larger models where hog units are a problem, but not as much for smaller models where the additional constraints are not beneficial -- uses a sigmoidal function: wb_inc = 1 / (1 + hi_gain*(wb_avg - hi_thr) + act_gain * (act_avg - act_thr))) 
  float         avg_thr;         // #CONDSHOW_ON_on #DEF_0.5 threshold on weight value for inclusion into the weight average that is then subject to the further hi_thr threshold for then driving a change in weight balance -- this avg_thr allows only stronger weights to contribute so that weakening of lower weights does not dilute sensitivity to number and strength of strong weights
  bool          norm_all;       // #CONDSHOW_ON_on normalize average by the full number of connections -- which then measures both the number and strength of above avg_thr weights -- or, if false, normalize only by number of above-threshold weights, which then is not sensitive to the number of such weights but just to their overall strength
  bool          thr_max;        // #CONDSHOW_ON_on hi_thr is a multiplier on the wb_avg_max value across all units in this same projection (from the previous iteration of wt balance) -- automatically self-normalizing
  float         hi_thr;         // #CONDSHOW_ON_on high threshold on weight average (subject to avg_thr) before it drives changes in weight increase vs. decrease factors -- see thr_max for meaning of this value
  float         hi_gain;        // #CONDSHOW_ON_on #DEF_2 gain multiplier applied to above-hi_thr thresholded weight averages -- higher values turn weight increases down more rapidly as the weights become more imbalanced 
  float         act_thr;        // #CONDSHOW_ON_on #DEF_0.25 threshold for long time-average activation (act_avg) contribution to weight balance -- based on act_avg relative to act_thr -- same statistic that we use to measure hogging with default .3 threshold
  float         act_gain;       // #CONDSHOW_ON_on gain multiplier applied to above-threshold weight averages -- higher values turn weight increases down more rapidly as the weights become more imbalanced -- see act_thr for equation
  // float         lo_thr;         // #CONDSHOW_ON_on #DEF_0.2 low threshold -- when average recv weights are below this threshold, weight decreases are penalized in proportion to sigmoidal 1/(1+lo_gain*(lo-thr-avg)) function that saturates at maximum of 1 which means that there are no weight decreases and all weight increases -- weight increases increase proportionally
  // float         lo_gain;        // #CONDSHOW_ON_on #DEF_4 gain multiplier applied to below-threshold weight averages -- higher values turn weight decreases down more rapidly as the weights become more imbalanced -- see hi_thr for equation
  
  INLINE void   WtBal
    (const float wb_avg, const float act_avg, const float wb_avg_max,
     float& wb_fact, float& wb_inc, float& wb_dec) {
    wb_fact = 0.0f;
    if(thr_max) {
      if(wb_avg_max > 0.0f) {
        float eff_hi_thr = hi_thr * wb_avg_max;
        if(wb_avg > eff_hi_thr) wb_fact += hi_gain * (wb_avg - eff_hi_thr);
      }
    }
    else {
      if(wb_avg > hi_thr)       wb_fact += hi_gain * (wb_avg - hi_thr);
    }
    if(act_avg > act_thr)       wb_fact += act_gain * (act_avg - act_thr);
    wb_inc = 1.0f / (1.0f + wb_fact); // gets sigmoidally small toward 0 as wb_fact gets larger -- is quick acting but saturates -- apply pressure earlier..
    wb_dec = 2.0f - wb_inc; // as wb_inc goes down, wb_dec goes up..  sum to 2
  }
  // compute weight balance factors for increase and decrease based on extent to which weights and average act exceed thresholds
  
  
  STATE_DECO_KEY("ConSpec");
  STATE_TA_STD_CODE_SPEC(WtBalanceSpec);
private:
  void        Initialize()      {   Defaults_init(); }
  void        Defaults_init() {
    on = true; avg_thr = 0.5f; norm_all = true; thr_max = true; hi_thr = 0.8f; hi_gain = 4.0f;
    act_thr = 0.25f; act_gain = 0.0f; 
    // lo_thr = 0.2f; lo_gain = 4.0f;
  }
};



class STATE_CLASS(AdaptWtScaleSpec) : public STATE_CLASS(SpecMemberBase) {
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
  
  INLINE void	AdaptWtScale(float& scale, const float wt) {
    const float nrm_wt = wt / scale;
    if(nrm_wt < lo_thr) {
      scale += dt * (lo_scale - scale);
    }
    else if(nrm_wt > hi_thr) {
      scale += dt * (hi_scale - scale);
    }
  }
  // adapt weight scale

  STATE_DECO_KEY("ConSpec");
  STATE_TA_STD_CODE_SPEC(AdaptWtScaleSpec);
  STATE_UAE( dt = 1.0f / tau; );
private:
  void	Initialize()     {   on = false;  Defaults_init(); }
  void	Defaults_init() {
    tau = 5000.0f;  lo_thr = 0.25f;  hi_thr = 0.75f;  lo_scale = 0.01f;  hi_scale = 2.0f;
    dt = 1.0f / tau;
  }
};


class STATE_CLASS(SlowWtsSpec) : public STATE_CLASS(SpecMemberBase) {
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

  INLINE void   UpdtVals() {
    fwt_pct = 1.0f - swt_pct;  slow_dt = 1.0f /(float)slow_tau;  wt_dt = 1.0f / wt_tau;
  }
  // #IGNORE
  
  STATE_DECO_KEY("ConSpec");
  STATE_TA_STD_CODE_SPEC(SlowWtsSpec);
  STATE_UAE( UpdtVals(); );
private:
  void	Initialize()    {   on = false;  wt_tau = 1.0f;  Defaults_init(); }
  void	Defaults_init() {   swt_pct = 0.8f; slow_tau = 100; UpdtVals(); }

};


class STATE_CLASS(DeepLrateSpec) : public STATE_CLASS(SpecMemberBase) {
  // ##INLINE ##NO_TOKENS ##CAT_Leabra learning rate specs for DeepLeabra -- effective learning rate can be enhanced for units receiving deep attentional modulation vs. those without
INHERITED(SpecMemberBase)
public:
  bool          on;             // enable the deep attentional differential learning rates based on deep_lrn modulation signal
  float         bg_lrate;       // #CONDSHOW_ON_on #MIN_0 learning rate multiplier for background cortico-cortical activations -- this is the baseline learning rate prior to adding in contribution of deep_lrn term: lrate_eff = lrate * (bg_lrate + fg_lrate * deep_lrn)
  float         fg_lrate;       // #CONDSHOW_ON_on #MIN_0 learning rate multiplier for foreground activations, as a function of deep_lrn activation level: lrate_eff = lrate * (bg_lrate + fg_lrate * deep_lrn)

  STATE_DECO_KEY("ConSpec");
  STATE_TA_STD_CODE_SPEC(DeepLrateSpec);
private:
  void	Initialize() {  on = false;  bg_lrate = 1.0f;  fg_lrate = 0.0f;  Defaults_init(); }
  void	Defaults_init()  { };
};


class STATE_CLASS(MarginLearnSpec) : public STATE_CLASS(SpecMemberBase) {
  // ##INLINE ##NO_TOKENS ##CAT_Leabra learning specs as function of whether units are on the margin
INHERITED(SpecMemberBase)
public:
  bool          lrate_mod;      // enable the margin-based modulation of the learning rate -- marginal units have a learning rate of 1, while others are reduced
  float         stable_lrate;   // #CONDSHOW_ON_lrate_mod #MIN_0 #DEF_0.5 learning rate multiplier for units that are outside of the margin (either solidly active or inactive) -- reduced relative to those on the margin (which have the default learning rate)
  bool          sign_dwt;       // use the sign of the margin state (low vs. high marginal status) to drive an additional dwt learning factor
  float         sign_lrn;       // #CONDSHOW_ON_sign_dwt #MIN_0 #DEF_0.5 amount of learning for sign-based learning factor

  INLINE float  MarginLrate(const float marg) {
    if(marg == -2.0f || marg == 2.0f) return stable_lrate;
    return 1.0f;
  }
  // the margin-based learning rate multiplier

  INLINE float  SignDwt(const float marg) {
    if(marg == -2.0f || marg == 2.0f) return 0.0f;
    return marg;
  }
  // get the margin-based sign dwt factor -- filters out on and off values
  
  STATE_DECO_KEY("ConSpec");
  STATE_TA_STD_CODE_SPEC(MarginLearnSpec);
private:
  void	Initialize()    {  lrate_mod = false;  sign_dwt = false;  Defaults_init(); }
  void	Defaults_init() {  stable_lrate = 0.5f;  sign_lrn = 0.5f; }
};


