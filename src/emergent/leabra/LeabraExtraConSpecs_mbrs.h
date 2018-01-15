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

// this must be included in a defined State_core/main/cuda context with appropriate
// #ifndef multiple-include protection AT THAT LEVEL not here..

#ifdef __MAKETA__
#pragma maketa_file_is_target LeabraExtraConSpecs
#endif


class STATE_CLASS(ChlSpecs) : public STATE_CLASS(SpecMemberBase) {
  // ##INLINE ##NO_TOKENS ##CAT_Leabra Leabra CHL mixture of learning factors (hebbian vs. error-driven) specification
INHERITED(SpecMemberBase)
public:
  bool          use;            // use CHL learning instead of standard XCAL learning -- allows easy exploration of CHL vs. XCAL
  float         hebb;           // [Default: .001] #MIN_0 amount of hebbian learning (should be relatively small, can be effective at .0001)
  float         err;            // #READ_ONLY #SHOW [Default: .999] amount of error driven learning, automatically computed to be 1-hebb
  float         savg_cor;       // #DEF_0.4:0.8 #MIN_0 #MAX_1 proportion of correction to apply to sending average activation for hebbian learning component (0=none, 1=all, .5=half, etc)
  float         savg_thresh;    // #DEF_0.001 #MIN_0 threshold of sending average activation below which learning does not occur (prevents learning when there is no input)

  STATE_DECO_KEY("ConSpec");
  STATE_TA_STD_CODE_SPEC(ChlSpecs);
  STATE_UAE( err = 1.0f - hebb; );
private:
  INLINE void   Initialize() {
    use = true;    hebb = .001f;    err = 1.0f - hebb;    savg_cor = .4f;
    savg_thresh = .001f;
  }
  INLINE void   Defaults_init() { };
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
  INLINE void   Initialize() {  Defaults_init();  dar = D1D2R; }
  INLINE void   Defaults_init() {
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
  INLINE void  Initialize() {   dip_da_gain = 1.0f;  Defaults_init(); }
  INLINE void  Defaults_init()  {
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
  INLINE void   Initialize()    {   Defaults_init(); }
  INLINE void   Defaults_init() {
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
  INLINE void   Initialize()    { Defaults_init(); }
  INLINE void   Defaults_init() {
    dalr_gain = 1.0f;  dalr_base = 0.0f; da_lrn_thr = 0.02f;  act_delta_thr = 0.02f;
  }
};


class STATE_CLASS(MSNTraceSpec) : public STATE_CLASS(SpecMemberBase) {
  // ##INLINE ##NO_TOKENS ##CAT_Leabra specifications for trace-based learning in the MSN's
INHERITED(SpecMemberBase)
public:
  float         ach_reset_thr;  // #MIN_0 #DEF_0.5 threshold on receiving unit ach value, sent by TAN units, for reseting the trace -- only applicable for trace-based learning
  bool          msn_deriv;      // #DEF_true use the sigmoid derivative factor msn * (1-msn) in modulating learning -- otherwise just multiply by msn activation directly -- this is generally beneficial for learning to prevent weights from continuing to increase when activations are already strong (and vice-versa for decreases)
  float         max_vs_deep_mod; // for VS matrix TRACE_NO_THAL_VS and DA_HEBB_VS learning rules, this is the maximum value that the deep_mod_net modulatory inputs from the basal amygdala (up state enabling signal) can contribute to learning

  INLINE float  MsnActLrnFactor(const float msn_act) {
    if(!msn_deriv) return msn_act;
    return 2.0f * msn_act * (1.0f - msn_act);
  }
  // learning factor for level of msn activation, of the general form of msn * (1-msn) -- the factor of 2 compensates for otherwise reduction in learning from these factors
  
  STATE_DECO_KEY("ConSpec");
  STATE_TA_STD_CODE_SPEC(MSNTraceSpec);
private:
  INLINE void   Initialize()    { Defaults_init(); }
  INLINE void   Defaults_init() {
    ach_reset_thr = 0.5f;  msn_deriv = true;  max_vs_deep_mod = 0.5f;
  }
};


class STATE_CLASS(MSNTraceThalLrates) : public STATE_CLASS(SpecMemberBase) {
  // ##INLINE ##NO_TOKENS ##CAT_Leabra gains for trace-based thalamic gated learning in the MSN's -- learning is also modulated typically by msn*(1-msn) activation of msn receiving units, to drive learning most in the sensitive middle range of activations (see msn_deriv option)
INHERITED(SpecMemberBase)
public:
  bool          full_params;    // #DEF_false use the full set of 2x2x2=8 parameters for all combinations of gating, go/nogo, and positive/negative dopamine -- otherwise just use the two key parameters that typically matter (other values are all 1.0f)
  float         not_gated;      // #DEF_0.7 #MIN_0 #CONDSHOW_OFF_full_params learning rate for all not-gated stripes, which learn in the opposite direction to the gated stripes, and typically with a slightly lower learning rate -- although there are different learning logics associated with each of these different not-gated cases (click full_params on to see each of them), it turns out in practice that the same learning rate for all works best, and is simplest
 
  float         gate_go_pos;    // #DEF_1 #MIN_0 #CONDSHOW_ON_full_params learning rate for gated, Go (D1), positive dopamine (weights increase) -- this is main direct pathway learning for positive reinforcement (outcomes better than expected), and defaults to 1 as the 'reference' learning rate -- per Thorndike's Law of Effect, actions that result in positive outcomes should be reinforced -- even though the action is already successful, it should be strengthened to better compete with other possible actions in the future, and make the action more vigorous
  float         gate_go_neg;    // #DEF_1 #MIN_0 #CONDSHOW_ON_full_params learning rate for gated, Go (D1), negative dopamine (weights decrease) -- this is the complementary main direct pathway learning for negative reinforcement (outcomes worse than expected), and defaults to 1 to balance the positive case, and allow learning here to track rate of success essentially linearly in an unbiased manner
  float         gate_nogo_pos;  // #DEF_0.1 #MIN_0 learning rate for gated, NoGo (D2), positive dopamine (weights decrease) -- this is the single most important learning parameter here -- by making this relatively small (but non-zero), an asymmetry in the role of Go vs. NoGo is established, whereby the NoGo pathway focuses largely on punishing and preventing actions associated with negative outcomes, while those assoicated with positive outcomes only very slowly get relief from this NoGo pressure -- this is critical for causing the model to explore other possible actions even when a given action SOMETIMES produces good results -- NoGo demands a very high, consistent level of good outcomes in order to have a net decrease in these avoidance weights.  Note that the gating signal applies to both Go and NoGo MSN's for gated stripes, ensuring learning is about the action that was actually selected (see not_ cases for logic for actions that were close but not taken)
  float         gate_nogo_neg;  // #DEF_1 #MIN_0 #CONDSHOW_ON_full_params learning rate for gated, NoGo (D2), negative dopamine (weights increase) -- strong (1.0) learning here to drive more NoGo for actions associated with negative outcomes -- the asymmetry with gate_nogo_pos is key as described there -- this remains at the default 1 maximal learning rate
  float         not_go_pos;     // #DEF_0.7 #MIN_0 #CONDSHOW_ON_full_params learning rate for not-gated, Go (D1), positive dopamine (weights decrease) -- serves to 'preventatively' tune the timing of Go firing, by decreasing weights to the extent that the Go unit fires but does not win the competition, and yet performance is still good (i.e., positive dopamine)
  float         not_go_neg;     // #DEF_0.7 #MIN_0 #CONDSHOW_ON_full_params learning rate for not-gated, Go (D1), negative dopamine (weights increase) -- increases weights to alternative Go firing pathways during errors, to help explore alternatives that work better, given that there are still errors -- because learning is proportional to level of MSN activation, those neurons that are most active, while still not winning the overall gating competition at the GPi stripe level, learn the most
  float         not_nogo_pos;   // #DEF_0.7 #MIN_0 #CONDSHOW_ON_full_params learning rate for not-gated, NoGo (D2), positive dopamine (weights increase) -- these are NoGo units that were active and, because this stripe was not gated, effectively blocked the gating of this action, and performance was overall successful (positive dopamine) -- thus, this learning reinforces that successful blocking to maintain and reinforce it further to the extent that it continues to be successful -- overall this learning must be well balanced with the not_nogo_neg learning -- having the same assymmetry that is present in the gated nogo pos vs. neg is NOT beneficial here -- because these are non-gated stripes, there are presumably a larger population of them and the non-gated nature means that we don't have that good of a credit assignment signal about how critical these are, so the strong punishment-oriented asymmetry doesn't work here -- instead a basic balanced accounting of pos vs. neg for these stripes (using the same values for pos vs. neg) works best
  float         not_nogo_neg;   // #DEF_0.7 #MIN_0 #CONDSHOW_ON_full_params learning rate for not-gated, NoGo (D2), negative dopamine (weights decrease) -- these are NoGo units that were active and therefore caused the corresponding stripe to NOT win the gating competition, and yet the outcome was NOT successful, so weights decrease here to STOP blocking these actions and explore more whether this action might be useful -- see not_nogo_pos for logic about these being balanced values, not strongly asymmetric as in the gated case

  INLINE float FullLrates(const bool gated, const bool d2r, const bool pos_da) {
    if(gated) {
      if(d2r) {               // nogo
        if(pos_da)
          return gate_nogo_pos;
        else
          return gate_nogo_neg;
      }
      else {                  // go
        if(pos_da)
          return gate_go_pos;
        else
          return gate_go_neg;
      }
    }
    else {                    // not-gated trace
      if(d2r) {               // nogo
        if(pos_da)
          return not_nogo_pos;
        else
          return not_nogo_neg;
      }
      else {                  // go
        if(pos_da)
          return not_go_pos;
        else
          return not_go_neg;
      }
    }
  }
  // get learning rate factor using full set of 2x2x2 parameters

  INLINE float Lrate(const bool gated, const bool d2r, const bool pos_da) {
    if(full_params) return FullLrates(gated, d2r, pos_da);
    if(!gated) return not_gated;
    if(d2r && pos_da) return gate_nogo_pos;
    return 1.0f;
  }
  // get learning rate using current parameter settings -- call this method
  
  STATE_DECO_KEY("ConSpec");
  STATE_TA_STD_CODE_SPEC(MSNTraceThalLrates);
private:
  INLINE void   Initialize()            { Defaults_init(); }
  INLINE void   Defaults_init() {
    full_params = false;    not_gated = 0.7f;
    gate_go_pos = 1.0f;    gate_go_neg = 1.0f;    gate_nogo_pos = 0.1f;
    gate_nogo_neg = 1.0f;  not_go_pos = 0.7f;     not_go_neg = 0.7f;
    not_nogo_pos = 0.7f;   not_nogo_neg = 0.7f;
  }
};
