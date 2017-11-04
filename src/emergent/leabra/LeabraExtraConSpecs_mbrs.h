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

// this must be included in a defined State_core/main/cuda context with appropriate
// #ifndef multiple-include protection AT THAT LEVEL not here..

#pragma maketa_file_is_target LeabraExtraConSpecs

class STATE_CLASS(ChlSpecs) : public STATE_CLASS(SpecMemberBase) {
  // ##INLINE ##NO_TOKENS ##CAT_Leabra Leabra CHL mixture of learning factors (hebbian vs. error-driven) specification
INHERITED(SpecMemberBase)
public:
  bool		use;	        // use CHL learning instead of standard XCAL learning -- allows easy exploration of CHL vs. XCAL
  float		hebb;		// [Default: .001] #MIN_0 amount of hebbian learning (should be relatively small, can be effective at .0001)
  float		err;		// #READ_ONLY #SHOW [Default: .999] amount of error driven learning, automatically computed to be 1-hebb
  float		savg_cor;       // #DEF_0.4:0.8 #MIN_0 #MAX_1 proportion of correction to apply to sending average activation for hebbian learning component (0=none, 1=all, .5=half, etc)
  float		savg_thresh;    // #DEF_0.001 #MIN_0 threshold of sending average activation below which learning does not occur (prevents learning when there is no input)

  STATE_DECO_KEY("ConSpec");
  STATE_TA_STD_CODE_SPEC(ChlSpecs);
  STATE_UAE( err = 1.0f - hebb; );
private:
  void	Initialize() {
    use = true;    hebb = .001f;    err = 1.0f - hebb;    savg_cor = .4f;
    savg_thresh = .001f;
  }
  void	Defaults_init() { };
};


class STATE_CLASS(LatAmygGains) : public STATE_CLASS(SpecMemberBase) {
  // ##INLINE ##NO_TOKENS ##CAT_Leabra specifications for lateral amygdala learning
INHERITED(SpecMemberBase)
public:
  enum DAReceptor {             // type of dopamine receptor expressed
    D1R,                        // Primarily expresses Dopamine D1 Receptors -- dopamine is excitatory and bursts of dopamine lead to increases in synaptic weight, while dips lead to decreases -- direct pathway in dorsal striatum
    D2R,                        // Primarily expresses Dopamine D2 Receptors -- dopamine is inhibitory and bursts of dopamine lead to decreases in synaptic weight, while dips lead to increases -- indirect pathway in dorsal striatum
    D1D2R,                      // Expresses both D1 and D2 -- learning is based on absolute-value of dopamine (magnitude)
  };

  DAReceptor    dar;            // dopamine receptor type for learning on these connections -- if specific case is selected (D1R, D2R) then negative learning rate factor enters into the equation (neg_lrate) for when effective da learning direction is negative (i.e., dips for D1R and bursts for D2R)
  float         neg_lrate;      // #CONDSHOW_OFF_dar:D1D2R how much to learn from the negative direction in dopamine-driven learning (i.e., dips for D1R and bursts for D2R) -- in principle this should be small so that CS's are not extinguished quickly -- see also wt_decay_rate and floor
  float         burst_da_gain;  // #MIN_0 multiplicative gain factor applied to positive dopamine signals -- this operates on the raw dopamine signal prior to any effect of D2 receptors in reversing its sign!
  float         dip_da_gain;    // #MIN_0 multiplicative gain factor applied to negative dopamine signals -- this operates on the raw dopamine signal prior to any effect of D2 receptors in reversing its sign!
  float         wt_decay_rate; // decay rate (each AlphaTrial) as percentage of the pos-rectified difference between the existing weight less the wt_decay_floor
  float         wt_decay_floor;  // #DEF_0.5 minimum weight value below which no decay occurs

  STATE_DECO_KEY("ConSpec");
  STATE_TA_STD_CODE_SPEC(LatAmygGains);
private:
  void	Initialize() {  Defaults_init();  dar = D1D2R; }
  void	Defaults_init() {
    burst_da_gain = 1.0f;    dip_da_gain= 1.0f;    wt_decay_rate = 0.001f;
    wt_decay_floor = 0.5f;    neg_lrate = 0.001f;
  }
};


class STATE_CLASS(BasAmygLearnSpec) : public STATE_CLASS(SpecMemberBase) {
  // ##INLINE ##NO_TOKENS ##CAT_Leabra basal amygdala learning specs
INHERITED(SpecMemberBase)
public:
  enum BaLearnRule {
    DELTA,                      // fully general delta rule learning: su * (ru->act_eq - ru->act_q0) -- delta relative to activation on prior trial -- use da mod of activation to also account for dopamine effects
    US_DELTA,                   // earlier form of delta rule based on difference between US (PV) and current activation -- only used when a PV signal is present  -- keeps BA units tracking actual US magnitude -- receives US as deep_raw_net from SendDeepRawConSpec from corresponding US unit
  };
    
  BaLearnRule   learn_rule;     // #DEF_DELTA learning rule -- delta is most general and deals with second-order conditioning and contrastive learning of CS-US associations -- US_DELTA is now obsolete but avail for backwards compatibility
  float         delta_neg_lrate; // #CONDSHOW_ON_learn_rule:DELTA learning rate factor for negative delta 
  float         delta_da_gain;   // #CONDSHOW_ON_learn_rule:DELTA how much does absolute value of dopamine contribute to learning
  float         delta_da_base;  // #CONDSHOW_ON_learn_rule:DELTA constant baseline learning rate -- da_gain adds to this
  bool          delta_da;       // #CONDSHOW_ON_learn_rule:US_DELTA multiply us_delta by absolute value of dopamine 
  float         burst_da_gain;  // #MIN_0 multiplicative gain factor applied to positive dopamine signals -- this operates on the raw dopamine signal prior to any effect of D2 receptors in reversing its sign!
  float         dip_da_gain;    // #MIN_0 multiplicative gain factor applied to negative dopamine signals -- this operates on the raw dopamine signal prior to any effect of D2 receptors in reversing its sign! should be small for acq, but roughly equal to burst_da_gain for ext 
  
  STATE_DECO_KEY("ConSpec");
  STATE_TA_STD_CODE_SPEC(BasAmygLearnSpec);
private:
  void  Initialize() {   dip_da_gain = 1.0f;  Defaults_init(); }
  void  Defaults_init()  {
    burst_da_gain = 1.0f;  learn_rule = US_DELTA;  delta_neg_lrate = 0.1f;
    delta_da_gain = 0.5f;  delta_da_base = 0.5f;   delta_da = true;
  }
};


class STATE_CLASS(BLAmygLearnSpec) : public STATE_CLASS(SpecMemberBase) {
  // ##INLINE ##NO_TOKENS ##CAT_Leabra specifications for gain factors and other learning parameters in Basal Lateral Amygdala learning
INHERITED(SpecMemberBase)
public:
  float         dalr_gain;      // #DEF_1 gain multiplier on abs(da) learning rate multiplier
  float         dalr_base;      // #DEF_0 constant baseline amount of learning prior to abs(da) factor -- should be near zero otherwise offsets in activation will drive learning in the absence of da significance
  
  float         da_lrn_thr;     // #DEF_0.02 minimum threshold for phasic abs(da) signals to count as non-zero;  useful to screen out spurious da signals due to tiny VSPatch-to-LHb signals on t2 & t4 timesteps that can accumulate over many trials - 0.02f seems to work okay
  float         act_delta_thr; // minimum threshold for delta activation to count as non-zero;  useful to screen out spurious learning due to unintended delta activity - 0.05f seems to work okay for acquisition guys, 0.02 for extinction guys
  
  bool          deep_lrn_mod; // #DEF_true if true, recv unit deep_lrn value modulates learning
  
  float         deep_lrn_thr; // #CONDSHOW_ON_deep_lrn_mod #DEF_0.05 only ru->deep_lrn values > this get to learn - 0.05f seems to work okay
  
  STATE_DECO_KEY("ConSpec");
  STATE_TA_STD_CODE_SPEC(BLAmygLearnSpec);
private:
  void	Initialize()    {   Defaults_init(); }
  void	Defaults_init() {
    dalr_gain = 1.0f;  dalr_base = 0.0f;  da_lrn_thr = 0.02f;  act_delta_thr = 0.05f;
    deep_lrn_mod = true;  deep_lrn_thr = 0.05f;
  }
};


class STATE_CLASS(CElAmygLearnSpec) : public STATE_CLASS(SpecMemberBase) {
  // ##INLINE ##NO_TOKENS ##CAT_Leabra specifications for gain factors and other learning parameters in CEl central amygdala lateral learning
INHERITED(SpecMemberBase)
public:
  float         dalr_gain;      // #DEF_1 gain multiplier on abs(da) learning rate multiplier
  float         dalr_base;      // #DEF_0 constant baseline amount of learning prior to abs(da) factor -- should be near zero otherwise offsets in activation will drive learning in the absence of da significance
  
  // NEW GUYS
  float         da_lrn_thr;    // #DEF_0.02 minimum threshold for phasic abs(da) signals to count as non-zero;  useful to screen out spurious da signals due to tiny VSPatch-to-LHb signals on t2 & t4 timesteps that can accumulate over many trials - 0.02f seems to work okay
  float         act_delta_thr; // #DEF_0.02 minimum threshold for delta activation to count as non-zero;  useful to screen out spurious learning due to unintended delta activity - 0.02f seems to work okay for both acquisition and extinction guys
  // END NEW GUYS
  
  STATE_DECO_KEY("ConSpec");
  STATE_TA_STD_CODE_SPEC(CElAmygLearnSpec);
private:
  void	Initialize()    { Defaults_init(); }
  void	Defaults_init() {
    dalr_gain = 1.0f;  dalr_base = 0.0f; da_lrn_thr = 0.02;  act_delta_thr = 0.02;
  }
};


