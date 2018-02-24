// Copyright 2017-2018, Regents of the University of Colorado,
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

#ifdef __MAKETA__
#pragma maketa_file_is_target LeabraConSpec
#endif


class STATE_CLASS(WtScaleSpec) : public STATE_CLASS(SpecMemberBase) {
  // ##INLINE ##NO_TOKENS ##CAT_Leabra weight scaling specification
INHERITED(SpecMemberBase)
public:
  float         abs;            // #DEF_1 #MIN_0 absolute scaling (not subject to normalization: directly multiplies weight values)
  float         rel;            // [Default: 1] #MIN_0 relative scaling that shifts balance between different projections (subject to normalization across all other projections into unit)
  bool          no_plus_net;    // #DEF_false do not send net input in the plus phase over this connection -- prevents this connection from driving error-driven learning in areas that it projects to -- typically used for projections to decoder output layers or in other specialized testing situations

  INLINE float  NetScale()      { return abs * rel; }

  INLINE float  SLayActScale(const float savg, const float lay_sz, const float n_cons) {
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

  INLINE float  FullScale(const float savg, const float lay_sz, const float n_cons)
  { return NetScale() * SLayActScale(savg, lay_sz, n_cons); }
  // full scaling factor -- product of above two sub-factors

  STATE_DECO_KEY("ConSpec");
  STATE_TA_STD_CODE_SPEC(WtScaleSpec);
private:
  void  Initialize() {
    rel = 1.0f;    abs = 1.0f;
    Defaults_init();
  }
  void  Defaults_init() {  no_plus_net = false; }
};


class STATE_CLASS(LeabraLearnSpec) : public STATE_CLASS(SpecMemberBase) {
  // ##INLINE ##NO_TOKENS ##CAT_Leabra parameters affecting overall learning rule, including experimental learning rule options 
INHERITED(SpecMemberBase)
public:
  enum LearnRule {             // overall form of learning rule to use
    DELTA_FF_FB,               // delta-rule in the feedforward projections: su.su_avg_s_lrn * (ru.ru_avg_s_lrn - ru.avg_m), and switched (sender <-> receiver) delta-rule in the feedback projections -- maintains symmetry while following the  correct backprop gradient -- also uses a compatible form of BCM Hebbian learning: xcal(su.su_avg_s_lrn * ru.avg_s, su.su_avg_s_lrn * ru.avg_l) -- learns significantly better than CHL in most cases -- requires accurately setting the fb flag for feedback projections so they switch
    XCAL_CHL,                  // original contrastive-hebbian-learning form of error-driven learning (within the XCal function: xcal(su.su_avg_s_lrn * ru.ru_avg_s_lrn, su.avg_m * ru.avg_m)), which achieves symmetry with underlying delta-rule formulation by having both sides of the connection learn with the average of each-other's gradient, while also using the average of short and medium (plus and minus) phase variables for sending activation.  BCM Hebbian is xcal(su.su_avg_s_lrn * ru.ru_avg_s_lrn, ru.avg_l) -- recommend updating to DELTA_FF_FB, which requires accurately identifying the FF vs. FB projections
    EXPT,                      // use an experimental form of learning defined by further selections of errule and bcmrule
  };

  enum  BcmLearnRule {         // what form of BCM Hebbian learning to use, in terms of the factor being compared against ru_avg_l in the XCAL function
    SRS,                       // bcm = xcal(su.su_avg_s_lrn * ru.ru_avg_s_lrn, ru_avg_l)
    RS,                        // bcm = su.su_avg_s_lrn * xcal(ru.ru_avg_s_lrn, ru_avg_l) 
    RS_SIN,                    // bcm = xcal(su.su_avg_s_lrn * ru.ru_avg_s_lrn, su.su_avg_s_lrn * ru_avg_l) -- now the preferred default, DELTA_FF_FB
    CPCA,                      // cpca = ru.ru_avg_s_lrn * (su.su_avg_s_lrn, wt_gain * wt_lin)
  };

  enum  ErrLearnRule {          // what form of error-driven learning to use?
    ERR_DELTA_FF_FB,            // current default version: uses feedback flag to select between normal and reversed / switched delta
    XCAL,                       // standard XCAL-CHL function of short vs. medium term average coproduct
    DELTA,                      // delta rule: su.su_avg_s_lrn * (ru.ru_avg_s_lrn - ru.avg_m)
    XCAL_DELTA,                 // xcal version of delta rule: su.su_avg_s_lrn * xcal(ru.ru_avg_s_lrn, ru.avg_m)
    XCAL_DELTA_SIN,             // xcal version of delta rule with sending term included inside xcal function -- now the preferred default in DELTA_FF_FB rule: xcal(su.su_avg_s_lrn * ru.ru_avg_s_lrn, su.su_avg_s_lrn * ru.avg_m)
    REV_DELTA,                  // reversed delta rule (sender delta instead of recv) -- see DELTA -- for top-down projections
    REV_XCAL_DELTA,             // reversed xcal delta rule (sender delta instead of recv) -- see XCAL_DELTA -- for top-down projections
    REV_XCAL_DELTA_SIN,         // reversed xcal delta rule (sender delta instead of recv) -- see XCAL_DELTA_SIN -- for top-down projections
    CHL,                        // pure contrastive hebbian learning: su.su_avg_s_lrn * ru.ru_avg_s_lrn - su.avg_m * ru.avg_m
  };

  LearnRule     rule;           // #DEF_DELTA_FF_FB overall form of learning rule to use
  ErrLearnRule  errule;         // #CONDSHOW_ON_rule:EXPT error-driven learning rule to use -- for exploration purposes..
  BcmLearnRule  bcmrule;        // #CONDSHOW_ON_rule:EXPT BCM Hebbian learning rule to use -- for exploration purposes..
  float         cp_gain;        // #CONDSHOW_ON_rule:EXPT gain on sending activation factor in CP and CPCA learning rules

  STATE_DECO_KEY("ConSpec");
  STATE_TA_STD_CODE_SPEC(LeabraLearnSpec);
  //  STATE_UAE(  );
  
private:
  void  Initialize() {   Defaults_init(); }
  void  Defaults_init() {
    rule = XCAL_CHL; errule = XCAL;  bcmrule = SRS; cp_gain = 0.8f;
  }
};


class STATE_CLASS(XCalLearnSpec) : public STATE_CLASS(SpecMemberBase) {
  // ##INLINE ##NO_TOKENS ##CAT_Leabra CtLeabra temporally eXtended Contrastive Attractor Learning function (XCAL) specs
INHERITED(SpecMemberBase)
public:
  float         m_lrn;          // #DEF_1 #MIN_0 multiplier on learning based on the medium-term floating average threshold which produces error-driven learning -- this is typically 1 when error-driven learning is being used, and 0 when pure hebbian learning is used -- note that the long-term floating average threshold is provided by the receiving unit
  bool          set_l_lrn;      // #DEF_false if true, set a fixed l_lrn weighting factor that determines how much of the long-term floating average threshold (i.e., BCM, Hebbian) component of learning is used -- this is useful for setting a fully Hebbian learning connection, e.g., by setting m_lrn = 0 and l_lrn = 1. If false, then the receiving unit's avg_l_lrn factor is used, which dynamically modulates the amount of the long-term component as a function of how active overall it is
  float         l_lrn;          // #CONDSHOW_ON_set_l_lrn fixed l_lrn weighting factor that determines how much of the long-term floating average threshold (i.e., BCM, Hebbian) component of learning is used -- this is useful for setting a fully Hebbian learning connection, e.g., by setting m_lrn = 0 and l_lrn = 1. 
  float         d_rev;          // #DEF_0.1 #MIN_0 #MAX_0.99 proportional point within LTD range where magnitude reverses to go back down to zero at zero -- err-driven svm component does better with smaller values, and BCM-like mvl component does better with larger values -- 0.1 is a compromise
  // float         d_gain;         // #DEF_1 #MIN_0 extra gain factor on LTD -- to compensate for reduction in LTD from higher levels of d_rev -- e.g., 2.0 for d_rev = .5 balances that out
  float         d_thr;          // #DEF_0.0001;0.01 #MIN_0 minimum LTD threshold value below which no weight change occurs -- this is now *relative* to the threshold
  float         thr_p_min;      // minimum thr_p potentiation threshold in absolute terms
  float         lrn_thr;        // #DEF_0.01 xcal learning threshold -- don't learn when sending unit activation is below this value in both phases -- due to the nature of the learning function being 0 when the sr coproduct is 0, it should not affect learning in any substantial way -- nonstandard learning algorithms that have different properties should ignore it

  float         d_rev_ratio;    // #HIDDEN #READ_ONLY -(1-d_rev)/d_rev -- multiplication factor in learning rule -- builds in the minus sign!

  INLINE float  dWtFun(float srval, float thr_p) {
    if(thr_p < thr_p_min) thr_p = thr_p_min;
    float d_thr_eff = d_thr * thr_p;
    if(srval < d_thr_eff)
      return 0.0f;
    srval -= d_thr_eff;
    thr_p -= d_thr;
    float rval;
    if(srval > thr_p * d_rev)
      rval = (srval - thr_p);
    else
      rval = srval * d_rev_ratio;
    return rval;
  }
  // XCAL function for weight change -- the "check mark" function -- no d_gain

  INLINE float  dWtFun_dgain(float srval, float thr_p, const float d_gain = 1.0f) {
    if(thr_p < thr_p_min) thr_p = thr_p_min;
    float d_thr_eff = d_thr * thr_p;
    if(srval < d_thr_eff)
      return 0.0f;
    srval -= d_thr_eff;
    thr_p -= d_thr;
    float rval;
    if(srval >= thr_p)
      rval = srval - thr_p;
    else if(srval > thr_p * d_rev)
      rval = d_gain * (srval - thr_p);
    else
      rval = d_gain * srval * d_rev_ratio;
    return rval;
  }
  // XCAL function for weight change -- the "check mark" function -- with d_gain

  INLINE float  SymSbFun(const float wt_val) {
    return 2.0f * wt_val * (1.0f - wt_val);
  }
  // symmetric soft bounding function -- factor of 2 to equate with asymmetric sb for overall lrate at a weight value of .5 (= .5)

  INLINE float  LongLrate(const float ru_avg_l_lrn) {
    if(set_l_lrn) return l_lrn;
    return ru_avg_l_lrn;
  }
  // get the learning rate for long-term floating average component (BCM)

  INLINE void   UpdtVals() {
    d_rev_ratio = (1.0f - d_rev) / d_rev;
    if(d_rev > 0.0f)
      d_rev_ratio = -(1.0f - d_rev) / d_rev;
    else
      d_rev_ratio = -1.0f;
  }

  
  STATE_DECO_KEY("ConSpec");
  STATE_TA_STD_CODE_SPEC(XCalLearnSpec);
  STATE_UAE( UpdtVals(); );
  
private:
  void  Initialize() {   Defaults_init(); }
  void  Defaults_init() {
    m_lrn = 1.0f;  set_l_lrn = false;  l_lrn = 1.0f;  d_rev = 0.10f;  // d_gain = 1.0f;
    thr_p_min = 0.001f;
    d_thr = 0.0001f;  lrn_thr = 0.01f; UpdtVals();
  }
};


class STATE_CLASS(WtSigSpec) : public STATE_CLASS(SpecMemberBase) {
  // ##INLINE ##NO_TOKENS ##CAT_Leabra sigmoidal weight function specification
INHERITED(SpecMemberBase)
public:
  float         gain;           // #DEF_1;6 #MIN_0 gain (contrast, sharpness) of the weight contrast function (1 = linear)
  float         off;            // #DEF_1 #MIN_0 offset of the function (1=centered at .5, >1=higher, <1=lower) -- 1 is standard for XCAL
  bool          soft_bound;     // #DEF_true apply exponential soft bounding to the weight changes 

  static INLINE float   SigFun(const float w, const float gn, const float of) {
    if(w <= 0.0f) return 0.0f;
    if(w >= 1.0f) return 1.0f;
    return (1.0f / (1.0f + powf((of * (1.0f - w)) / w, gn)));
  }
  // static version of function for implementing weight sigmodid

  static INLINE float   SigFun61(const float w) {
    if(w <= 0.0f) return 0.0f;
    if(w >= 1.0f) return 1.0f;
    const float pwguy = (1.0f - w) / w;
    return (1.0f / (1.0f + pwguy*pwguy*pwguy*pwguy*pwguy*pwguy));
  }
  // static version of function for implementing weight sigmodid -- for default gain = 6, offset = 1 params

  static INLINE float   InvFun(const float w, const float gn, const float of) {
    if(w <= 0.0f) return 0.0f;
    if(w >= 1.0f) return 1.0f;
    return 1.0f / (1.0f + powf((1.0f - w) / w, 1.0f / gn) / of);
  }
  // static version of function for implementing inverse of weight sigmoid

  static INLINE float   InvFun61(const float w) {
    if(w <= 0.0f) return 0.0f;
    if(w >= 1.0f) return 1.0f;
    return 1.0f / (1.0f + powf((1.0f - w) / w, 1.0f / 6.0f));
  }
  // static version of function for implementing inverse of weight sigmoid -- for default gain = 6, offset = 1 params

  INLINE float  SigFmLinWt(float lw) {
    if(gain == 1.0f && off == 1.0f) return lw;
    if(gain == 6.0f && off == 1.0f) return SigFun61(lw);
    return SigFun(lw, gain, off);
  }
  // get sigmoidal contrast-enhanced weight from linear weight
  
  INLINE float  LinFmSigWt(const float sw) {
    if(gain == 1.0f && off == 1.0f) return sw;
    if(gain == 6.0f && off == 1.0f) return InvFun61(sw);
    return InvFun(sw, gain, off);
  }
  // get linear weight from sigmoidal contrast-enhanced weight

  STATE_DECO_KEY("ConSpec");
  STATE_TA_STD_CODE_SPEC(WtSigSpec);
private:
  void  Initialize()    {  Defaults_init(); }
  void  Defaults_init() {  gain = 6.0f;  off = 1.0f;  soft_bound = true; }
};


class STATE_CLASS(LeabraDwtNorm) : public STATE_CLASS(SpecMemberBase) {
  // ##INLINE ##NO_TOKENS ##CAT_Leabra implements dwt normalization, either on all dwts or only on error-driven dwt component, based on different levels of max_avg value -- slowly decays and instantly resets to any current max(abs) -- serves as an estimate of the variance in the weight changes, assuming zero net mean overall
INHERITED(SpecMemberBase)
public:
  enum  NormLevel {             // what level to aggregate normalization factor over
    SYN,                        // normalize each synapse separately
    RECV_CONS,                  // normalize across all receiving connections per projection, per unit -- updates with wt_bal factors, every Network.times.wt_bal_int trials -- adjust avg_tau accordingly!
    SEND_CONS,                  // normalize across all sending connections per projection, per unit -- updates every trial (fast, sender-based)
    RECV_UNIT,                  // normalize across all connections within recv unit -- updates with wt_bal factors, every Network.times.wt_bal_int trials -- adjust avg_tau accordingly!
    PRJN,                       // normalize across an entire projection -- sender-based, fast, but this seems to increase hogging..
  };
  
  bool          on;             // #DEF_true whether to use dwt normalization, only on error-driven dwt component, based on projection-level max_avg value -- slowly decays and instantly resets to any current max
  NormLevel     level;          // #CONDSHOW_ON_on what level to normalize across -- the max value is updated at this level -- for larger-scale levels, always ensures that normalization factor is max of current abs dwt too, so no weight change magnitude exceeds 1 ever
  bool          err_only;       // #CONDSHOW_ON_on #DEF_false only normalize the error-driven component of learning
  float         decay_tau;      // #CONDSHOW_ON_on #MIN_1 #DEF_1000;10000 time constant for decay of dwnorm factor -- generally should be long-ish, between 1000-10000 -- integration rate factor is 1/tau
  float         norm_min;       // #CONDSHOW_ON_on #MIN_0 #DEF_0.001 minimum effective value of the normalization factor -- provides a lower bound to how much normalization can be applied
  float         lr_comp;        // #MIN_0 #CONDSHOW_ON_on #DEF_0.1;0.2 overall learning rate multiplier to compensate for changes due to use of normalization -- allows for a common master learning rate to be used between different conditions -- 0.1 for synapse-level, maybe higher for other levels
  bool          stats;          // record the avg, max values of err, bcm hebbian, and overall dwt change per con group and per projection

  float         decay_dt;      // #READ_ONLY #EXPERT rate constant of decay = 1 / decay_tau
  float         decay_dt_c;    // #READ_ONLY #EXPERT complement rate constant of decay = 1 - (1 / decay_tau)

  INLINE bool RecvConsAgg() const {
    return (on && (level == RECV_CONS));
  }
  // is a recv-cons level aggregation required?  slow.. done during WtBal computation
  
  INLINE bool RecvUnitAgg() const {
    return (on && (level == RECV_UNIT));
  }
  // is a recv-unit level aggregation required?  slow.. done during WtBal computation
  
  INLINE bool SendConsAgg() const {
    return (on && (level == SEND_CONS));
  }
  // is a send-cons level aggregation required?
  
  INLINE bool PrjnAgg() const {
    return (on && (level == PRJN));
  }
  // is a projection level aggregation required? 
  
  INLINE void UpdateAvg(float& dwnorm, const float abs_dwt) const {
    dwnorm = fmaxf(decay_dt_c * dwnorm, abs_dwt);
  }
  // update the dwnorm running max_abs, slowly decaying value -- jumps up to max(abs_dwt) and slowly decays

  INLINE float EffNormFactor(float dwnorm) const {
    if(!on || dwnorm == 0.0f) return 1.0f;   // nothing going on
    dwnorm = fmaxf(dwnorm, norm_min);
    return lr_comp / dwnorm;
  }
  // get the effective normalization factor, as a multiplier, including lrate comp -- dwnorm already has been updated to be max(abs_dwt) so no dwt can be > 1
  
  INLINE void   UpdtVals() {
    decay_dt = 1.0f / decay_tau;  decay_dt_c = 1.0f - decay_dt;
  }
  // #IGNORE
  
  STATE_DECO_KEY("ConSpec");
  STATE_TA_STD_CODE_SPEC(LeabraDwtNorm);
  STATE_UAE( UpdtVals(); );
private:
  void  Initialize()    {  Defaults_init(); }
  void  Defaults_init() {
    on = true;  level = SYN;  err_only = false; decay_tau = 1000.0f; norm_min = 0.001f;
    lr_comp = 0.1f;  stats = false;
    UpdtVals();
  }
};


class STATE_CLASS(LeabraMomentum) : public STATE_CLASS(SpecMemberBase) {
  // ##INLINE ##NO_TOKENS ##CAT_Leabra implements standard simple momentum -- accentuates consistent directions of weight change and cancels out dithering -- biologically captures slower timecourse of longer-term plasticity mechanisms
INHERITED(SpecMemberBase)
public:
  bool          on;             // #DEF_true whether to use standard simple momentum 
  float         m_tau;          // #CONDSHOW_ON_on #MIN_1 #DEF_10 time constant factor for integration of momentum -- 1/tau is dt (e.g., .1), and 1-1/tau (e.g., .95 or .9) is traditional momentum time-integration factor
  float         lr_comp;        // #MIN_0 #CONDSHOW_ON_on #DEF_0.1 overall learning rate multiplier to compensate for changes due to JUST momentum without normalization -- allows for a common master learning rate to be used between different conditions -- generally should use .1 to compensate for just momentum itself

  float         m_dt;          // #READ_ONLY #EXPERT rate constant of momentum integration = 1 / m_tau
  float         m_dt_c;        // #READ_ONLY #EXPERT complement rate constant of momentum integration = 1 - (1 / m_tau)

  INLINE float ComputeMoment(float& moment, float new_dwt) {
    moment = m_dt_c * moment + new_dwt;
    return moment;
  }
  // compute momentum version of new dwt change

  INLINE void   UpdtVals() {
    m_dt = 1.0f / m_tau;  m_dt_c = 1.0f - m_dt;
  }
  // #IGNORE
  
  STATE_DECO_KEY("ConSpec");
  STATE_TA_STD_CODE_SPEC(LeabraMomentum);
  STATE_UAE( UpdtVals(); );
private:
  void  Initialize()    {  Defaults_init(); }
  void  Defaults_init() {
    on = true;  m_tau = 10.0f;  lr_comp = 0.1f;
    UpdtVals();
  }
};


class STATE_CLASS(WtBalanceSpec) : public STATE_CLASS(SpecMemberBase) {
  // ##INLINE ##NO_TOKENS ##CAT_Leabra weight balance soft renormalization spec: maintains overall weight balance by progressively penalizing weight increases as a function of how strong the weights are overall (subject to thresholding) and long time-averaged activation -- plugs into soft bounding function -- see network times.bal_int for interval in trials of updating
INHERITED(SpecMemberBase)
public:
  bool          on;             // perform weight balance soft normalization?  if so, maintains overall weight balance across units by progressively penalizing weight increases as a function of amount of averaged weight above a high threshold (hi_thr) and long time-average activation above an act_thr -- this is generally very beneficial for larger models where hog units are a problem, but not as much for smaller models where the additional constraints are not beneficial -- uses a sigmoidal function: wb_inc = 1 / (1 + hi_gain*(wb_avg - hi_thr) + act_gain * (act_avg - act_thr)))
  float         avg_thr;        // #CONDSHOW_ON_on #DEF_0.25 threshold on weight value for inclusion into the weight average that is then subject to the further hi_thr threshold for then driving a change in weight balance -- this avg_thr allows only stronger weights to contribute so that weakening of lower weights does not dilute sensitivity to number and strength of strong weights
  float         hi_thr;         // #CONDSHOW_ON_on #DEF_0.4 high threshold on weight average (subject to avg_thr) before it drives changes in weight increase vs. decrease factors
  float         hi_gain;        // #CONDSHOW_ON_on #DEF_4 gain multiplier applied to above-hi_thr thresholded weight averages -- higher values turn weight increases down more rapidly as the weights become more imbalanced 
  float         lo_thr;         // #CONDSHOW_ON_on #DEF_0.4 low threshold on weight average (subject to avg_thr) before it drives changes in weight increase vs. decrease factors
  float         lo_gain;        // #CONDSHOW_ON_on #DEF_6;0 gain multiplier applied to below-lo_thr thresholded weight averages -- higher values turn weight increases up more rapidly as the weights become more imbalanced -- generally beneficial but sometimes not -- worth experimenting with either 6 or 0
  float         act_thr;        // #CONDSHOW_ON_on #DEF_0.25 threshold for long time-average activation (act_avg) contribution to weight balance -- based on act_avg relative to act_thr -- same statistic that we use to measure hogging with default .3 threshold
  float         act_gain;       // #CONDSHOW_ON_on #DEF_0;2 gain multiplier applied to above-threshold weight averages -- higher values turn weight increases down more rapidly as the weights become more imbalanced -- see act_thr for equation
  bool          no_targ;        // #CONDSHOW_ON_on #DEF_true exclude receiving projections into TARGET layers where units are clamped and also TRC (Pulvinar) thalamic neurons -- typically for clamped layers you do not want to be applying extra constraints such as this weight balancing dynamic -- the BCM hebbian learning is also automatically turned off for such layers as well
  
  INLINE void   WtBal
    (float wb_avg, const float act_avg, float& wb_fact, float& wb_inc, float& wb_dec) {
    wb_fact = 0.0f;
    if(wb_avg < lo_thr) {
      if(wb_avg < avg_thr) wb_avg = avg_thr; // prevent extreme low if everyone below thr
      wb_fact = lo_gain * (lo_thr - wb_avg);
      wb_dec = 1.0f / (1.0f + wb_fact);
      wb_inc = 2.0f - wb_dec;
    }
    else {
      if(wb_avg > hi_thr)       wb_fact += hi_gain * (wb_avg - hi_thr);
      if(act_avg > act_thr)     wb_fact += act_gain * (act_avg - act_thr);
      wb_inc = 1.0f / (1.0f + wb_fact); // gets sigmoidally small toward 0 as wb_fact gets larger -- is quick acting but saturates -- apply pressure earlier..
      wb_dec = 2.0f - wb_inc; // as wb_inc goes down, wb_dec goes up..  sum to 2
    }
  }
  // compute weight balance factors for increase and decrease based on extent to which weights and average act exceed thresholds
  
  
  STATE_DECO_KEY("ConSpec");
  STATE_TA_STD_CODE_SPEC(WtBalanceSpec);
private:
  void        Initialize()      {   Defaults_init(); }
  void        Defaults_init() {
    on = true; no_targ = true; avg_thr = 0.25f; hi_thr = 0.4f; hi_gain = 4.0f;
    lo_thr = 0.4f; lo_gain = 6.0f;  act_thr = 0.25f; act_gain = 0.0f; 
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
  
  INLINE void   AdaptWtScale(float& scale, const float wt) {
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
  void  Initialize()     {   on = false;  Defaults_init(); }
  void  Defaults_init() {
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
  float         slow_dt;        // #CONDSHOW_ON_on #READ_ONLY #EXPERT rate constant of slow updating = 1 / slow_tau
  float         wt_dt;          // #CONDSHOW_ON_on #READ_ONLY #EXPERT rate constant of wt adaptation = 1 / wt_tau

  INLINE void   UpdtVals() {
    fwt_pct = 1.0f - swt_pct;  slow_dt = 1.0f /(float)slow_tau;  wt_dt = 1.0f / wt_tau;
  }
  // #IGNORE
  
  STATE_DECO_KEY("ConSpec");
  STATE_TA_STD_CODE_SPEC(SlowWtsSpec);
  STATE_UAE( UpdtVals(); );
private:
  void  Initialize()    {   on = false;  wt_tau = 1.0f;  Defaults_init(); }
  void  Defaults_init() {   swt_pct = 0.8f; slow_tau = 100; UpdtVals(); }

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
  void  Initialize() {  on = false;  bg_lrate = 1.0f;  fg_lrate = 0.0f;  Defaults_init(); }
  void  Defaults_init()  { };
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
  void  Initialize()    {  lrate_mod = false;  sign_dwt = false;  Defaults_init(); }
  void  Defaults_init() {  stable_lrate = 0.5f;  sign_lrn = 0.5f; }
};


class STATE_CLASS(DwtShareSpec) : public STATE_CLASS(SpecMemberBase) {
  // ##INLINE ##NO_TOKENS ##CAT_Leabra share dwt changes across different neighboring connections -- a kind of structured randomness within a long-term relationship -- has potential benefits for top-down projections to disrupt positive feedback loops and inject some randomness in learning directions -- not useful for feedforward projections
INHERITED(SpecMemberBase)
public:
  bool          on;             // enable dwt sharing -- share dwt changes across different neighboring connections -- a kind of structured randomness within a long-term relationship -- has potential benefits for top-down projections to disrupt positive feedback loops and inject some randomness in learning directions -- not useful for feedforward projections
  int           neigh;          // #CONDSHOW_ON_on #DEF_8 number of neighbors to share dwts with -- each sending synapse shares with neigh units *on either side* of itself (i.e., total number of shared synapses = 2*neigh -- just adds up all those dwt values in computing its own dwt) -- thus there is some amount of overlap and also non-overlap in the set of shared synapses -- this works slightly better than having a common group of shared synapses
  float         p_share;        // #CONDSHOW_ON_on #DEF_0.05 #MIN_0 #MAX_1 probability of sharing dwts -- per sending unit (all synapses for given sender share at the same time) -- can't do this too frequently without disrupting learning
  
  STATE_DECO_KEY("ConSpec");
  STATE_TA_STD_CODE_SPEC(DwtShareSpec);
private:
  void  Initialize()    { on = false;   Defaults_init(); }
  void  Defaults_init() { neigh = 8; p_share = 0.05f; }
};


