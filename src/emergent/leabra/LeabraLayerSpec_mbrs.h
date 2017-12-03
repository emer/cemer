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
// it must be included directly in LeabraLayerSpec.h, _cpp.h, _cuda.h
// the STATE_CLASS macro will define a _cpp _cuda or plain (no suffix) version

// this pragma ensures that maketa properly grabs this type information even though
// this file is included in the other files -- we get ta info for main and _cpp, not cuda
#pragma maketa_file_is_target LeabraLayerSpec


class STATE_CLASS(LeabraInhibSpec) : public STATE_CLASS(SpecMemberBase) {
  // ##INLINE ##NO_TOKENS ##CAT_Leabra specifies how inhibition is computed in Leabra system -- uses feedforward (FF) and feedback (FB) inhibition (FFFB) based on average (or maximum) netinput (FF) and activation (FB) -- any unit-level inhibition is just added on top of this computed inhibition
INHERITED(SpecMemberBase)
public:
  bool          on;             // enable this form of inhibition (layer or unit group) -- if only using inhibitory interneurons, both can be turned off
  float         gi;             // #CONDSHOW_ON_on #MIN_0 #AKA_lay_gi #DEF_1.8 [1.5-2.3 typical, can go lower or higher as needed] overall inhibition gain -- this is main paramter to adjust to change overall activation levels -- it scales both the the ff and fb factors uniformly -- also see inhib_adapt which will adapt an additional multiplier on this overall inhibition to keep layer near target activation value specified in avg_act.targ_init
  float		ff;		// #CONDSHOW_ON_on #MIN_0 #DEF_1 overall inhibitory contribution from feedforward inhibition -- multiplies average netinput (i.e., synaptic drive into layer) -- this anticipates upcoming changes in excitation, but if set too high, it can make activity slow to emerge -- see also ff0 for a zero-point for this value
  float		fb;		// #CONDSHOW_ON_on #MIN_0 #DEF_1 overall inhibitory contribution from feedback inhibition -- multiplies average activation -- this reacts to layer activation levels and works more like a thermostat (turning up when the 'heat' in the layer is too high)
  float         fb_tau;         // #CONDSHOW_ON_on #MIN_0 #DEF_1.4;3;5 time constant in cycles, which should be milliseconds typically (roughly, how long it takes for value to change significantly -- 1.4x the half-life) for integrating feedback inhibitory values -- prevents oscillations that otherwise occur -- typcially has not much effect on learning, and longer values (3 or higher) are more robust, but previous defaults were relatively rapid 1.4
  float         max_vs_avg;     // #CONDSHOW_ON_on #DEF_0;0.5;1 #AKA_ff_max_vs_avg what proportion of the maximum vs. average netinput to use in the feedforward inhibition computation -- 0 = all average, 1 = all max, and values in between = proportional mix between average and max (ff_netin = avg + ff_max_vs_avg * (max - avg)) -- including more max can be beneficial especially in situations where the average can vary significantly but the activity should not -- max is more robust in many situations but less flexible and sensitive to the overall distribution -- max is better for cases more closely approximating single or strictly fixed winner-take-all behavior -- 0.5 is a good compromize in many cases and generally requires a reduction of .1 or slightly more (up to .3-.5) from the gi value for 0
  float         ff0;            // #CONDSHOW_ON_on #DEF_0.1 feedforward zero point for average netinput -- below this level, no FF inhibition is computed based on avg netinput, and this value is subtraced from the ff inhib contribution above this value -- the 0.1 default should be good for most cases (and helps FF_FB produce k-winner-take-all dynamics), but if average netinputs are lower than typical, you may need to lower it

  float		fb_dt;		// #READ_ONLY #EXPERT rate = 1 / tau

  INLINE float    FFInhib(const float avg_netin, const float max_netin) {
    const float ff_netin = avg_netin + max_vs_avg * (max_netin - avg_netin);
    float ffi = 0.0f;
    if(ff_netin > ff0) ffi = ff * (ff_netin - ff0);
    return ffi;
  }
  // feedforward inhibition value as function of netinput

  INLINE float    FBInhib(const float avg_act) {
    float fbi = fb * avg_act;
    return fbi;
  }
  // feedback inhibition value as function of netinput

  INLINE void     FBUpdt(float& fbi, const float nw_fbi) {
    fbi += fb_dt * (nw_fbi - fbi);
  }
  // update feedback inhibition using time-integration rate constant

  
  STATE_DECO_KEY("LayerSpec");
  STATE_TA_STD_CODE_SPEC(LeabraInhibSpec);

  STATE_UAE( fb_dt = 1.0f / fb_tau; );
  
private:
  void	Initialize() {
    on = true;
    Defaults_init();
  }
    
  void	Defaults_init() {
    gi = 1.8f;    ff = 1.0f;    fb = 1.0f;    fb_tau = 3.0f;
    max_vs_avg = 0.0f;    ff0 = 0.1f;
    fb_dt = 1.0f / fb_tau;
  }
    
};


class STATE_CLASS(LayerAvgActSpec) : public STATE_CLASS(SpecMemberBase) {
  // ##INLINE ##NO_TOKENS ##CAT_Leabra expected average activity levels in the layer -- used for computing running-average computation that is then used for netinput scaling (also specifies time constant for updating average), and for the target value for adapting inhibition in inhib_adapt
INHERITED(SpecMemberBase)
public:
  float		targ_init;	    // #AKA_init #MIN_0 [typically 0.1 - 0.2] target value for adapting inhibition (see inhib_adapt params) and initial estimated average activity level in the layer (see use_first option -- if that is off then it is used as a starting point for running average actual activity level, acts_m_avg and acts_p_avg) -- acts_p_avg is used primarily for automatic netinput scaling, to balance out layers that have different activity levels -- thus it is important that init be relatively accurate -- good idea to update from recorded acts_p_avg levels (see LayerAvgAct button, here and on network) -- see also adjust parameter
  bool          fixed;      // #DEF_false if true, then the init value is used as a constant for acts_p_avg_eff (the effective value used for netinput rescaling), instead of using the actual running average activation
  bool          use_ext_act;  // #DEF_false if true, then use the activation level computed from the external inputs to this layer (avg of targ or ext unit vars) -- this will only be applied to layers with INPUT or TARGET / OUTPUT layer types, and falls back on the targ_init value if external inputs are not available or have a zero average -- implies fixed behavior
  bool          use_first;    // #CONDSHOW_OFF_fixed||use_ext_act #DEF_true use the first actual average value to override targ_init value -- actual value is likely to be a better estimate than our guess
  float         tau;        // #CONDSHOW_OFF_fixed||use_ext_act #DEF_100 #MIN_1 time constant in trials for integrating time-average values at the layer level -- used for computing acts_m_avg and acts_p_avg
  float         adjust;     // #CONDSHOW_OFF_fixed||use_ext_act #DEF_1 adjustment multiplier on the computed acts_p_avg value that is used to compute acts_p_avg_eff, which is actually used for netinput rescaling -- if based on connectivity patterns or other factors the actual running-average value is resulting in netinputs that are too high or low, then this can be used to adjust the effective average activity value -- reducing the average activity with a factor < 1 will increase netinput scaling (stronger net inputs from layers that receive from this layer), and vice-versa for increasing (decreases net inputs)
  
  float		dt;		// #READ_ONLY #EXPERT rate = 1 / tau


  INLINE float  AvgEffInit() {
    if(fixed)   return targ_init;
    else        return adjust * targ_init;
  }
  // initializer for acts_p_avg_eff
  
  STATE_DECO_KEY("LayerSpec");
  STATE_TA_STD_CODE_SPEC(LayerAvgActSpec);
  
  STATE_UAE( dt = 1.0f / tau; use_ext_act = false; );
  
private:
  void	Initialize() { targ_init = 0.15f;  Defaults_init(); }

  void	Defaults_init() {
    fixed = false;
    use_ext_act = false;
    use_first = true;
    tau = 100.0f;
    adjust = 1.0f;
    dt = 1.0f / tau;
  }
};



class STATE_CLASS(LeabraAdaptInhib) : public STATE_CLASS(SpecMemberBase) {
  // ##INLINE ##NO_TOKENS ##CAT_Leabra adapt the overal inhibitory gain value (adapt_gi on layer) to keep overall layer activation within a given target range as specified by avg_act.targ_init
INHERITED(SpecMemberBase)
public:
  bool          on;             // enable adaptive inhibition function to adapt overall layer inhibition gain as stored in layer adapt_gi value
  float         tol_pct;        // #CONDSHOW_ON_on #DEF_0.25 tolerance around target average activation of avg_act.targ_init as a proportion of that target value -- only once activations move outside this tolerance are inhibitory values adapted
  int           trial_interval; // #CONDSHOW_ON_on interval in trials between updates of the adaptive inhibition values -- only check and update this often -- typically the same order as the number of trials per epoch used in training the model
  float		tau;		// #CONDSHOW_ON_on #DEF_200;500 time constant for rate of updating the inhibitory gain value, in terms of trial_interval periods (e.g., 100 = adapt gain over 100 trial intervals) -- adaptation rate is 1/tau * (acts_m_avg - trg_avg_act) / trg_avg_act

  float		dt;		// #READ_ONLY #EXPERT rate = 1 / tau

  INLINE bool   AdaptInhib(float& gi, const float trg_avg_act, const float acts_m_avg) {
    float delta_pct = (acts_m_avg - trg_avg_act) / trg_avg_act;
    if(fabsf(delta_pct) >= tol_pct) {
      gi += dt * delta_pct;
      return true;
    }
    return false;
  }

  STATE_DECO_KEY("LayerSpec");
  STATE_TA_STD_CODE_SPEC(LeabraAdaptInhib);
  
  STATE_UAE( dt = 1.0f / tau; );
  
private:
  void	Initialize() {  on = false;  Defaults_init();  }
  
  void	Defaults_init() {
    tol_pct = 0.25f;
    trial_interval = 100;
    tau = 200.0f;
    dt = 1.0f / tau;
  }
};



class STATE_CLASS(LeabraActMargin) : public STATE_CLASS(SpecMemberBase) {
  // ##INLINE ##NO_TOKENS ##CAT_Leabra marginal activation computation -- detects those units that are on the edges of an attractor and focuses extra learning on them
INHERITED(SpecMemberBase)
public:
  float         pct_marg;       // #DEF_0.3 proportion of the total number of active units (defined by layer acts_p_avg value) that should fit between the low and high marginal thresholds, on average -- hi_thr is adapted over time to hit this target on average (actually (1-pct_marg) * acts_p_avg above the hi thr, to remove dependence on low_thr), while low_thr is adapted to capture the full set of acts_p_avg units, and med_thr is adapted to roughly split the pct_marg proportion in half
  float         avg_tau;        // #DEF_100 time constant in terms of trials for computing the average numbers of units in the different marginal categories
  float         adapt_tau;      // #DEF_500 time constant in terms of trials for adapting the thresholds based on average target values
  float         tol_pct;        // #DEF_0.25 tolerance around target values as a proportion of that target value -- don't adapt values if values are within this tolerance of their targets
  float         avg_act;        // #DEF_0.8 expected average activation level for active neurons -- this is a correction factor for converting acts_p_avg into target averages (divide by this avg_act)
  float	        low_thr;        // #DEF_0.501 initial low threshold for marginal activation, in terms of v_m_eq -- adapts from here so that roughly acts_p_avg units on average are above this low threshold
  float         med_thr;        // #DEF_0.506 initial medium threshold for marginal activation in terms of v_m_eq -- adapts from here so that the number of marginal units are split evenly on average
  float         hi_thr;         // #DEF_0.508 initial high threshold for marginal activation in terms of v_m_eq -- adapts from here so that roughly (1-pct_marg) * acts_p_avg are above the hi thr (and thus, pct_marg * acts_p_avg are between low and hi thresholds)
  
  float		avg_dt;		// #READ_ONLY #EXPERT rate = 1 / tau
  float		adapt_dt;	// #READ_ONLY #EXPERT rate = 1 / tau

  INLINE void   IncrAvgVal(float& avg, const float new_val) {
    avg += avg_dt * (new_val - avg);
  }
  // increment running average based on new value
  INLINE float  AdaptThr(float& thr, const float avg, const float trg, const float sgn) {
    float delta_pct = (avg - trg) / trg;
    if(fabsf(delta_pct) >= tol_pct) {
      float del = sgn * adapt_dt * delta_pct;
      thr += del;
      return del;
    }
    return 0.0f;
  }
  // adadpt a threshold as function of difference between avg and target, in direction given by sign
  INLINE float  HiTarg(const float acts_p_avg) {
    return (1.0f - pct_marg) * acts_p_avg;
  }
  // the target value for hi threshold as function of acts_p_avg level in layer
  INLINE float  MedTarg(const float acts_p_avg) {
    return (0.5f * pct_marg) * acts_p_avg;
  }
  // the target value for med threshold as function of acts_p_avg level in layer
  
  
  STATE_DECO_KEY("LayerSpec");
  STATE_TA_STD_CODE_SPEC(LeabraActMargin);
  
  STATE_UAE( avg_dt = 1.0f / avg_tau;
                     adapt_dt = 1.0f / adapt_tau; );
  
private:
  void	Initialize() { Defaults_init(); }

  void	Defaults_init() {
    pct_marg = 0.3f;
    avg_tau = 100.0f;
    adapt_tau = 500.0f;
    tol_pct = 0.25f;
    avg_act = 0.8f;
    low_thr = 0.501f;
    med_thr = 0.506f;
    hi_thr = 0.508f;

    avg_dt = 1.0f / avg_tau;
    adapt_dt = 1.0f / adapt_tau;
  }
};



class STATE_CLASS(LeabraInhibMisc) : public STATE_CLASS(SpecMemberBase) {
  // ##INLINE ##NO_TOKENS ##CAT_Leabra extra specifications for how inhibition is computed in Leabra system -- these apply across layer and unit group levels
INHERITED(SpecMemberBase)
public:
  float		net_thr;	// #DEF_0;0.2 threshold on net input for inclusion in the computation of the average netinput, which in turn drives feedforward inhibition -- this is important for preventing units that don't have any significant input from diluting the overall level of inhibition -- prior to version 7.8.7 this was effectively 0 -- set to 0.2 if inhibition is being inappropriately diluted by these off units (not all such cases benefit from this filtering -- experiment) -- see also thr_rel and thr_act options for whether this is a relative vs. absolute threshold, and whether it applies to activations or not
  bool          thr_rel;        // #CONDSHOW_OFF_net_thr:0 #DEF_true if true, net_thr is relative to the maximum net input within the layer (from the previous cycle) -- this is useful because net input values evolve over time and a fixed threshold prevents any ff inhibition early in settling when net inputs are low
  float		self_fb;	// #MIN_0 #DEF_0.5;0.02;0;1 individual unit self feedback inhibition -- can produce proportional activation behavior in individual units for specialized cases (e.g., scalar val or BG units), but not so good for typical hidden layers
 float          self_tau;       // #CONDSHOW_OFF_self_fb:0 #MIN_0 #DEF_1.4 time constant in cycles, which should be milliseconds typically (roughly, how long it takes for value to change significantly -- 1.4x the half-life) for integrating unit self feedback inhibitory values -- prevents oscillations that otherwise occur -- relatively rapid 1.4 typically works, but may need to go longer if oscillations are a problem
  bool          fb_up_immed;    // should the feedback inhibition rise immediately to the driving value, and then decay with fb_tau time constant?  this is important for spiking activation function.  otherwise, all feedback component changes are goverened by fb_tau, which works better for rate-code case

  float		self_dt;        // #READ_ONLY #EXPERT rate = 1 / tau


  STATE_DECO_KEY("LayerSpec");
  STATE_TA_STD_CODE_SPEC(LeabraInhibMisc);
  
  STATE_UAE( self_dt = 1.0f / self_tau; );
  
private:
  void	Initialize() {
    net_thr = 0.0f;
    thr_rel = true;
    self_fb = 0.0f;
    fb_up_immed = false;

    Defaults_init();
  }
  void	Defaults_init() {
    self_tau = 1.4f;
    self_dt = 1.0f / self_tau;
  }
};



class STATE_CLASS(LeabraClampSpec) : public STATE_CLASS(SpecMemberBase) {
  // ##INLINE ##NO_TOKENS ##CAT_Leabra specs for clamping external inputs on INPUT or TARGET layers
INHERITED(SpecMemberBase)
public:
  bool		hard;		// #DEF_true whether to hard clamp inputs where activation is directly set to external input value (act = ext, computed once at start of quarter) or do soft clamping where ext is added into net input (net += gain * ext)
  float		gain;		// #CONDSHOW_OFF_hard #DEF_0.02:0.5 soft clamp gain factor (net += gain * ext)
  bool		avg;		// #CONDSHOW_OFF_hard compute soft clamp as the average of current and target netins, not the sum -- prevents some of the main effect problems associated with adding
  float         avg_gain;       // #CONDSHOW_OFF_hard||!avg gain factor for averaging the netinput -- clamp value contributes with avg_gain and current netin as (1-avg_gain)


  INLINE  float ClampAvgNetin(const float ext, const float net_syn)
  { return avg_gain * gain * ext + (1.0f - avg_gain) * net_syn; }

  STATE_DECO_KEY("LayerSpec");
  STATE_TA_STD_CODE_SPEC(LeabraClampSpec);
  
private:
  
  void	Initialize() {
    hard = true;
    avg = false;
    avg_gain = 0.2f;
    Defaults_init();
  }
  void	Defaults_init() { gain = 0.2f; }
};



class STATE_CLASS(LayerDecaySpec) : public STATE_CLASS(SpecMemberBase) {
  // ##INLINE ##NO_TOKENS ##CAT_Leabra holds decay values and other layer-level time constants
INHERITED(SpecMemberBase)
public:
  float		trial;		// #AKA_event #MIN_0 #MAX_1 [1 to clear] proportion decay of state vars between trials -- if all layers have 0 trial decay, then the net input does not need to be reset between trials, yielding significantly faster performance

  STATE_DECO_KEY("LayerSpec");
  STATE_TA_STD_CODE_SPEC(LayerDecaySpec);
 
private:
  void	Initialize() { trial = 1.0f; Defaults_init(); }

  void	Defaults_init() { };
};



class STATE_CLASS(LeabraDelInhib) : public STATE_CLASS(SpecMemberBase) {
  // ##INLINE ##NO_TOKENS ##CAT_Leabra delayed inhibition, as a function of per-unit net input on prior trial and/or phase -- produces temporal derivative effects
INHERITED(SpecMemberBase)
public:
  bool          on;             // enable delayed inhibition 
  float		prv_trl;	// #CONDSHOW_ON_on proportion of per-unit net input on previous trial to add in as inhibition 
  float		prv_q;	        // #CONDSHOW_ON_on proportion of per-unit net input on previous gamma-frequency quarter to add in as inhibition 


  INLINE float  DelInhib(float net_prv_trl, float net_prv_q) {
    return prv_trl * net_prv_trl + prv_q * net_prv_q;
  }
  // get delayed inhibition
  
  STATE_DECO_KEY("LayerSpec");
  STATE_TA_STD_CODE_SPEC(LeabraDelInhib);
  
private:
  void	Initialize() {
    on = false;
    prv_trl = 0.0f;
    prv_q = 0.0f;
    Defaults_init();
  }

  void	Defaults_init() { };
};



class STATE_CLASS(LeabraCosDiffMod) : public STATE_CLASS(SpecMemberBase) {
  // ##INLINE ##NO_TOKENS ##CAT_Leabra leabra layer-level cosine of difference between plus and minus phase activations -- used to modulate amount of hebbian learning, and overall learning rate
INHERITED(SpecMemberBase)
public:
  float		avg_tau;  // #DEF_100 #MIN_1 time constant in trials (roughly how long significant change takes, 1.4 x half-life) for computing running average cos_diff value for the layer, cos_diff_avg = cosine difference between act_m and act_p -- this is an important statistic for how much phase-based difference there is between phases in this layer -- it is used in standard X_COS_DIFF modulation of l_mix in LeabraConSpec, and for modulating learning rate as a function of predictability in the DeepLeabra predictive auto-encoder learning -- running average variance also computed with this: cos_diff_var
  bool          lrate_mod; // modulate learning rate in this layer as a function of the cos_diff on this trial relative to running average cos_diff values (see avg_tau) -- lrate_mod = cos_diff_lrate_mult * (cos_diff / cos_diff_avg) -- if this layer is less predictable than previous trials, we don't learn as much
  bool          lrmod_fm_trc;   // #CONDSHOW_ON_lrate_mod get our learning rate modulation from our corresponding TRC layer, which has the strongest and most accurate cos_diff signals -- definitely recommended for non-TRC layers assocated with one -- must find a connection
  float         lrmod_z_thr; // #DEF_-1.5 #CONDSHOW_ON_lrate_mod&&!lrmod_fm_trc threshold for setting learning rate modulation to zero, as function of z-normalized cos_diff value on this trial -- normalization computed using incrementally computed average and variance values -- this essentially has the network ignoring trials where the diff was significantly below average -- replaces the manual unlearnable trial mechanism
  bool          set_net_unlrn;  // #CONDSHOW_ON_lrate_mod&&!lrmod_fm_trc set the network-level unlearnable_trial flag based on our learning rate modulation factor -- only makes sense for one layer to do this

  float         avg_dt; // #READ_ONLY #EXPERT rate constant = 1 / cos_diff_avg_tau
  float         avg_dt_c; // #READ_ONLY #EXPERT complement of rate constant = 1 - cos_diff_avg_dt

  INLINE void	UpdtDiffAvgVar(float& diff_avg, float& diff_var, const float cos_diff) {
    if(diff_avg == 0.0f) {        // first time -- set
      diff_avg = cos_diff;
      diff_var = 0.0f;
    }
    else {
      float del = cos_diff - diff_avg;
      float incr = avg_dt * del;
      diff_avg += incr;
      // following is magic exponentially-weighted incremental variance formula
      // derived by Finch, 2009: Incremental calculation of weighted mean and variance
      if(diff_var == 0.0f)
        diff_var = 2.0f * avg_dt_c * del * incr;
      else
        diff_var = avg_dt_c * (diff_var + del * incr);
    }
  }
  // update the running average diff value

  INLINE float  CosDiffLrateMod(const float cos_diff, const float diff_avg,
                                const float diff_var) {
    if(diff_var <= 0.0f) return 1.0f;
    float zval = (cos_diff - diff_avg) / sqrtf(diff_var); // stdev = sqrt of var
    // z-normal value is starting point for learning rate factor
    if(zval < lrmod_z_thr) return 0.0f;
    return 1.0f;
  }
  // get lrate modulation based on cos_diff level
  
  STATE_DECO_KEY("LayerSpec");
  STATE_TA_STD_CODE_SPEC(LeabraCosDiffMod);
  
  STATE_UAE( avg_dt = 1.0f / avg_tau; avg_dt_c = 1.0f - avg_dt; );
  
private:
  void	Initialize() {   Defaults_init(); }
  void	Defaults_init() {
    avg_tau = 100.0f;
    lrate_mod = false;
    lrmod_fm_trc = false;
    lrmod_z_thr = -1.5f;
    set_net_unlrn = false;

    avg_dt = 1.0f / avg_tau;
    avg_dt_c = 1.0f - avg_dt;
  }
};


class STATE_CLASS(LeabraLayStats) : public STATE_CLASS(SpecMemberBase) {
  // ##INLINE ##NO_TOKENS ##CAT_Leabra leabra layer-level statistics parameters
INHERITED(SpecMemberBase)
public:
  float         hog_thr;           // #MIN_0 #MAX_1 #DEF_0.3;0.2 threshold on unit avg_act (long time-averaged activation), above which the unit is considered to be a 'hog' that is dominating the representational space
  float         dead_thr;         // #MIN_0 #MAX_1 #DEF_0.01;0.005 threshold on unit avg_act (long time-averaged activation), below which the unit is considered to be inactive and not participating

  STATE_DECO_KEY("LayerSpec");
  STATE_TA_STD_CODE_SPEC(LeabraLayStats);

private:
  void	Initialize() {   Defaults_init(); }
  void	Defaults_init() {
    hog_thr = 0.3f;
    dead_thr = 0.01f;
  }
};


