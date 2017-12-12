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

#pragma maketa_file_is_target LeabraExtraUnitSpecs


class STATE_CLASS(TDRewIntegSpec) : public STATE_CLASS(SpecMemberBase) {
  // ##INLINE ##NO_TOKENS ##CAT_Leabra misc specs for TDRewIntegUnitSpec
INHERITED(SpecMemberBase)
public:
  float		discount;	// discount factor for V(t+1) from TDRewPredLayer
  bool		max_r_v;	// represent the maximum of extrew (r) and tdrewpred estimate of V(t+1) instead of the sum of these two factors -- produces a kind of "absorbing" reward function instead of a cumulative reward function

  STATE_DECO_KEY("UnitSpec");
  STATE_TA_STD_CODE_SPEC(TDRewIntegSpec);
private:
  void	Initialize()    { discount = .8f;  max_r_v = false; }
  void	Defaults_init() { };
};


class STATE_CLASS(BLAmygDaMod) : public STATE_CLASS(SpecMemberBase) {
  // ##INLINE ##NO_TOKENS ##CAT_Leabra specifications for da_mod gain factors and other learning parameters in BL Amygdala learning
INHERITED(SpecMemberBase)
public:
  float         burst_da_gain;  // #MIN_0 #DEF_0.1 multiplicative gain factor applied to positive dopamine signals -- this operates on the raw dopamine signal prior to any effect of D2 receptors in reversing its sign!
  float         dip_da_gain;    // #MIN_0 #DEF_0.1 multiplicative gain factor applied to negative dopamine signals -- this operates on the raw dopamine signal prior to any effect of D2 receptors in reversing its sign! should be small for acq, but roughly equal to burst_da_gain for ext 
  
  bool          lrn_mod_act;    // #DEF_false if true, phasic dopamine values effect learning by modulating net_syn values (Compute_NetinExtras() - and thus unit activations; - CAUTION - very brittle and hard to use due to unintended consequences!
  float         pct_act;        // #CONDSHOW_LRN_MOD_ACT_true #DEF_1;0.8 proportion of activation used for computing dopamine modulation value -- 1-pct_act comes from net-input -- activation is more differentiated and leads to more differentiated representations, but if there is no activation then dopamine modulation has no effect, so it depends on having that activation signal
  float         us_clamp_avg;   // #CONDSHOW_LRN_MOD_ACT_true #DEF_0.2 averaging factor for quasi-clamping US (PV) values when sent using a SendDeepRaw connection to modulate net_syn values which in turn modulates actual activation values -- more gradual form of clamping; requires SendDeepRawConSpec fm PosPV/NegPV layer
  
  STATE_DECO_KEY("UnitSpec");
  STATE_TA_STD_CODE_SPEC(BLAmygDaMod);
private:
  void	Initialize()    {   Defaults_init(); }
  void	Defaults_init() {
    pct_act = 1.0f;  burst_da_gain = 0.1f;  dip_da_gain = 0.1f;  us_clamp_avg = 0.2f;
  }
};


class STATE_CLASS(BLAmygAChMod) : public STATE_CLASS(SpecMemberBase) {
  // ##INLINE ##NO_TOKENS ##CAT_Leabra specifications for ach_mod gain factors
INHERITED(SpecMemberBase)
public:
  bool          on;             // whether to modulate activations as a function of ach levels
  float         mod_min;        // #DEF_0.8 minimum ach modulation factor -- net modulation is mod_mid + ach * (1 - mod_min)
  float         mod_min_c;      // #READ_ONLY #EXPERT 1 - mod_min
  
  STATE_DECO_KEY("UnitSpec");
  STATE_TA_STD_CODE_SPEC(BLAmygAChMod);
  STATE_UAE( mod_min_c = 1.0f - mod_min; );
private:
  void	Initialize()    {   Defaults_init(); }
  void	Defaults_init() {
    on = true;    mod_min = 0.8f;    mod_min_c = 1.0f - mod_min;
  }
};


class STATE_CLASS(CElAmygDaMod) : public STATE_CLASS(SpecMemberBase) {
  // ##INLINE ##NO_TOKENS ##CAT_Leabra specifications for da_mod gain factors and other learning parameters in CEl central amygdala lateral learning
INHERITED(SpecMemberBase)
public:
  
  float         burst_da_gain;  // #MIN_0 #DEF_0;0.04 multiplicative gain factor applied to positive dopamine signals -- this operates on the raw dopamine signal prior to any effect of D2 receptors in reversing its sign!
  float         dip_da_gain;    // #MIN_0 #DEF_0.1 multiplicative gain factor applied to negative dopamine signals -- this operates on the raw dopamine signal prior to any effect of D2 receptors in reversing its sign! should be small for acq, but roughly equal to burst_da_gain for ext 
  bool          acq_deep_mod;   // #DEF_true use deep_mod_net for value from acquisition / go units, instead of inhibition current (otherwise use gi_syn) -- allows simpler parameter setting without titrating inhibition and this learning modulation signal
  
  bool          lrn_mod_act;  // #DEF_false if true, phasic dopamine values effect learning by modulating net_syn values (Compute_NetinExtras() - and thus unit activations; - CAUTION - very brittle and hard to use due to unintended consequences!
  float         us_clamp_avg;   // #CONDSHOW_LRN_MOD_ACT_true #DEF_0.2 averaging factor for quasi-clamping US (PV) values when sent using a SendDeepRaw connection to modulate net_syn values which in turn modulates actual activation values -- more graded form of clamping..
  
  STATE_DECO_KEY("UnitSpec");
  STATE_TA_STD_CODE_SPEC(CElAmygDaMod);
private:
  void	Initialize() {
    burst_da_gain = 0.0f;  dip_da_gain = 0.1f;  acq_deep_mod = true;  us_clamp_avg = 0.2f;
    Defaults_init();
  }
  void	Defaults_init() { }
};


class STATE_CLASS(LHbRMTgSpecs) : public STATE_CLASS(SpecMemberBase) {
  // ##INLINE ##NO_TOKENS ##CAT_Leabra misc specs and params for LHbRMTg inputs
INHERITED(SpecMemberBase)
public:
  bool          patch_cur;      // #DEF_true use current trial patch activations -- otherwise use previous trial -- current trial is appropriate for simple discrete trial environments (e.g., with some PBWM models), whereas previous is more approprate for trials with more realistic temporal structure
  float         pvneg_discount;  // #DEF_0.8 #MIN_0 #MAX_1 reduction in effective PVNeg net value (when positive) so that negative outcomes can never be completely predicted away -- still allows for positive da for less-bad outcomes
  bool          rec_data;       // #DEF_true record all the separate input values for each component

  STATE_DECO_KEY("UnitSpec");
  STATE_TA_STD_CODE_SPEC(LHbRMTgSpecs);
private:
  void  Initialize() {   patch_cur = true;  Defaults_init(); }
  void  Defaults_init() {
    pvneg_discount = 0.8f;  rec_data = false;
  }
};


class STATE_CLASS(LHbRMTgGains) : public STATE_CLASS(SpecMemberBase) {
  // ##INLINE ##NO_TOKENS ##CAT_Leabra gains for LHbRMTg inputs
INHERITED(SpecMemberBase)
public:
  float         all;            // #MIN_0 #DEF_1 final overall gain on everything
  float         vspatch_pos_D1;      // #MIN_0 #DEF_1 VS patch D1 APPETITIVE pathway - versus pos PV outcomes
  float         vspatch_pos_D2;      // #MIN_0 #DEF_1 VS patch D2 APPETITIVE pathway versus vspatch_pos_D1
  float         vspatch_pos_disinhib_gain; // #DEF_0.2 proportion of positive reward prediction error (RPE) to use if RPE results from a predicted omission of positive reinforcement - e.g., conditioned inhibitor
  float         vsmatrix_pos_D1;  // #MIN_0 #DEF_1 gain on VS matrix D1 APPETITIVE guys
  float         vsmatrix_pos_D2;  // #MIN_0 #DEF_1 - VS matrix D2 APPETITIVE
  
  float         vspatch_neg_D1;      // #MIN_0 #DEF_1 VS patch D1 pathway versus neg PV outcomes
  float         vspatch_neg_D2;      // #MIN_0 #DEF_1 VS patch D2 pathway versus vspatch_neg_D1
  
  float         vsmatrix_neg_D1; // #MIN_0 #DEF_1 - VS matrix D1 AVERSIVE
  float         vsmatrix_neg_D2; // #MIN_0 #DEF_1 - VS matrix D2 AVERSIVE

  STATE_DECO_KEY("UnitSpec");
  STATE_TA_STD_CODE_SPEC(LHbRMTgGains);
private:
  void  Initialize()    { Defaults_init(); }
  void  Defaults_init() {
    all = 1.0f;    vspatch_pos_D1 = 1.0f;    vspatch_pos_D2 = 1.0f;
    vspatch_pos_disinhib_gain = 0.2f;    vsmatrix_pos_D1 = 1.0f;
    vsmatrix_pos_D2 = 1.0f;    vspatch_neg_D1 = 1.0f;
    vspatch_neg_D2 = 1.0f;    vsmatrix_neg_D1 = 1.0f;
    vsmatrix_neg_D2 = 1.0f;
  }
};


class STATE_CLASS(PVLVDaSpec) : public STATE_CLASS(SpecMemberBase) {
  // ##INLINE ##NO_TOKENS ##CAT_Leabra specs for PVLV da parameters
INHERITED(SpecMemberBase)
public:
  float         tonic_da;       // #DEF_0 set a tonic 'dopamine' (DA) level (offset to add to da values)
  bool          patch_cur;      // #DEF_true use current trial patch activations -- otherwise use previous trial -- current trial is appropriate for simple discrete trial environments (e.g., with some PBWM models), whereas previous is more approprate for trials with more realistic temporal structure
  float         se_gain;        // strength of the serotonin 5HT inputs on the dopamine outputs -- sev values when present subtract from the dopamine value otherwise computed
  bool          rec_data;       // #DEF_true record all the internal computations in user data on the VTA layer

  STATE_DECO_KEY("UnitSpec");
  STATE_TA_STD_CODE_SPEC(PVLVDaSpec);
private:
  void  Initialize()    {
    patch_cur = true;  rec_data = false;  se_gain = 0.1f;  plus_only = true; Defaults_init();
  }
  void  Defaults_init() { tonic_da = 0.0f; }
};



class STATE_CLASS(PVLVDaGains) : public STATE_CLASS(SpecMemberBase) {
  // ##INLINE ##NO_TOKENS ##CAT_Leabra gains for various VTA inputs
INHERITED(SpecMemberBase)
public:
  float         da_gain;      // #DEF_0:2 #MIN_0 overall multiplier for dopamine values
  float         pptg_gain;    // #DEF_1 gain on bursts from PPTg
  float         lhb_gain;     // #DEF_1.1 gain on dips/bursts from LHbRMTg
  float         pv_gain;      // #DEF_1 gain on positive PV component of total phasic DA signal (net after subtracting VSPatchIndir (PVi) shunt signal)
  float         pvi_burst_shunt_gain;  // #DEF_1.05 gain on VSPatch projection that shunts bursting in VTA (for VTAp = VSPatchPosD1, for VTAn = VSPatchNegD2)
  float         pvi_anti_burst_shunt_gain;  // #DEF_1 gain on VSPatch projection that opposes shunting of bursting in VTA (for VTAp = VSPatchPosD2, for VTAn = VSPatchNegD1)
  float         pvi_dip_shunt_gain;  // #DEF_0 gain on VSPatch projection that shunts dipping of VTA (currently only VTAp supported = VSPatchNegD2) -- optional and somewhat controversial 
  float         pvi_anti_dip_shunt_gain;  // #DEF_0 gain on VSPatch projection that opposes the shunting of dipping in VTA (currently only VTAp supported = VSPatchNegD1)

  STATE_DECO_KEY("UnitSpec");
  STATE_TA_STD_CODE_SPEC(PVLVDaGains);
private:
  void  Initialize()    {   Defaults_init(); }
  void  Defaults_init() {
    da_gain = 1.0f;    pptg_gain = 1.0f;    lhb_gain = 1.0f;    pv_gain = 1.0f;
    pvi_burst_shunt_gain = 1.05f;    pvi_anti_burst_shunt_gain = 1.0f;
    pvi_anti_dip_shunt_gain = 0.0f;  pvi_dip_shunt_gain = 0.0f;
  }
};


class STATE_CLASS(LVBlockSpec) : public STATE_CLASS(SpecMemberBase) {
  // ##INLINE ##NO_TOKENS ##CAT_Leabra how LV signals are blocked by PV and LHbRMTg dip signals -- there are good reasons for these signals to block LV, because they reflect a stronger overall signal about outcomes, compared to the more "speculative" LV signal
INHERITED(SpecMemberBase)
public:
  float         pos_pv;   // #DEF_1 down-regulate LV by factor of: (1 - pos_pv * pv) for positive pv signals (e.g., from LHA etc) -- the larger this value, the more LV is blocked -- if it is 0, then there is no LV block at all -- net actual block is 1 - sum over both sources of block
  float         lhb_dip;  // #DEF_2 down-regulate LV by factor of: (1 - dip * lhb_rmtg) for da dip signals coming from the LHbRMTg sytem -- the larger this value, the more LV is blocked -- if it is 0, then there is no LV block at all -- net actual block is 1 - sum over both sources of block

  STATE_DECO_KEY("UnitSpec");
  STATE_TA_STD_CODE_SPEC(LVBlockSpec);
private:
  void  Initialize()    {   Defaults_init(); }
  void  Defaults_init() {   pos_pv = 1.0f;  lhb_dip = 2.0f; }
};


class STATE_CLASS(DRN5htSpec) : public STATE_CLASS(SpecMemberBase) {
  // ##INLINE ##NO_TOKENS ##CAT_Leabra specs for DRN 5ht parameters
INHERITED(SpecMemberBase)
public:
  float         se_out_gain;    // #DEF_0:2 #MIN_0 multiplier for 5HT serotonin values as they are sent to other layers
  float         se_base;        // baseline initial 5HT serotonin level -- should be a low value -- can be 0 
  float         se_inc_tau;     // #MIN_1 time constant for rate at which 5HT intrinsically ramps upward toward the maximum value of 1 -- this reflects the cost of time passing in the absence of anything otherwise good happening (which would work to push the value back down)
  float         da_pos_tau;     // #MIN_1 time constant for effect of positive dopamine values on 5HT levels -- positive dopamine has a negative effect on 5HT, pulling it back down toward baseline at this rate
  float         da_neg_tau;     // #MIN_1 gain on effect of negative dopamine values on 5HT levels -- negative dopamine has a positive effect on 5HT, pulling it upward toward 1 at this rate
  float         se_pv_tau;      // #MIN_1 time constant for integrating primary outcome values (PV's) over time -- when a negative (or neg - pos if sub_pos) PV value occurs, it pushes the sev value toward that PV value with this time constant
  float         se_state_tau;   // #MIN_1 time constant for integrating bodily state values over time -- typically much slower -- when a negative (or neg - pos if sub_pos) body state value occurs, it pushes the sev value toward that body state value
  bool          sub_pos;        // subtract positive values (otherwise just use negative values)

  float         se_inc_dt;     // #READ_ONLY rate = 1 / tau
  float         da_pos_dt;     // #READ_ONLY rate = 1 / tau
  float         da_neg_dt;     // #READ_ONLY rate = 1 / tau
  float         se_pv_dt;      // #READ_ONLY rate = 1 / tau
  float         se_state_dt;   // #READ_ONLY rate = 1 / tau

  STATE_DECO_KEY("UnitSpec");
  STATE_TA_STD_CODE_SPEC(DRN5htSpec);
  STATE_UAE( UpdateVals(); );
private:
  void  UpdateVals() {
    se_inc_dt = 1.0f / se_inc_tau;
    da_pos_dt = 1.0f / da_pos_tau;
    da_neg_dt = 1.0f / da_neg_tau;
    se_pv_dt = 1.0f / se_pv_tau;
    se_state_dt = 1.0f / se_state_tau;
  }
  void  Initialize()    { Defaults_init(); }
  void  Defaults_init() {
    se_out_gain = 1.0f;    se_base = 0.0f;    se_inc_tau = 50.0f;
    da_pos_tau = 10.0f;    da_neg_tau = 10.0f;    se_pv_tau = 20.0f;
    se_state_tau = 200.0f;    sub_pos = true;
    UpdateVals();
  }
};



class STATE_CLASS(BFCSAChSpec) : public STATE_CLASS(SpecMemberBase) {
  // ##INLINE ##NO_TOKENS ##CAT_Leabra basal forebrain cholinergic system (BFCS) specs for ach computation 
INHERITED(SpecMemberBase)
public:
  float         tonic_ach;      // #DEF_0.5 set a tonic basline level of ach
  float         tau;            // time constant of integration over trials for delta input signals to drive changes in ach levels -- updates are computed at the trial time scale so that is the operative scale for this time scale 
  float         cea_gain;       // central nucleus of the amygdala input gain -- how strongly delta factor here contributes to overall BFCS activation
  float         vs_gain;        // ventral striatum input gain -- how strongly delta factor here contributes to overall BFCS activation
  float         dt;             // #READ_ONLY #EXPERT rate = 1 / tau
  
  STATE_DECO_KEY("UnitSpec");
  STATE_TA_STD_CODE_SPEC(BFCSAChSpec);
  STATE_UAE(  dt = 1.0f / tau; );
private:
  void  Initialize()    {   Defaults_init(); }
  void  Defaults_init() {
    tonic_ach = 0.5f;    tau = 10.0f;    cea_gain = 1.0f;    vs_gain = 1.0f;   dt = 1.0f / tau;
  }
};



class STATE_CLASS(MatrixActSpec) : public STATE_CLASS(SpecMemberBase) {
  // ##INLINE ##NO_TOKENS ##CAT_Leabra specs for matrix MSN unit activation
INHERITED(SpecMemberBase)
public:
  float         patch_shunt;    // #DEF_0.2;0.5 #MIN_0 #MAX_1 how much the patch shunt activation multiplies the dopamine values -- 0 = complete shunting, 1 = no shunting -- should be a factor < 1.0
  bool          shunt_ach;      // #DEF_true also shunt the ACh value driven from TAN units -- this prevents clearing of MSNConSpec traces -- more plausibly the patch units directly interfere with the effects of TAN's rather than through ach, but it is easier to implement with ach shunting here.
  float         out_ach_inhib;  // #DEF_0:0.3 how much does the LACK of ACh from the TAN units drive extra inhibition to output-gating Matrix units -- gi += out_ach_inhib * (1-ach) -- provides a bias for output gating on reward trials -- do NOT apply to NoGo, only Go -- this is a key param -- between 0.1-0.3 usu good -- see how much output gating happening and change accordingly
  int           n_mnt_x; // #DEF_-1 number of maintenance gating stripes along the x axis -- -1 = divide evenly between mnt and out -- if uneven numbers or only one or the other is present then you need to set this

  STATE_DECO_KEY("UnitSpec");
  STATE_TA_STD_CODE_SPEC(MatrixActSpec);
private:
  void  Initialize()    {   n_mnt_x = -1;  Defaults_init(); }
  void  Defaults_init() {
    patch_shunt = 0.2f;  shunt_ach = true;  out_ach_inhib = 0.3f;
  }
};



class STATE_CLASS(TANActSpec) : public STATE_CLASS(SpecMemberBase) {
  // ##INLINE ##NO_TOKENS ##CAT_Leabra specs for TAN activation 
INHERITED(SpecMemberBase)
public:
  bool          plus_fm_pv_vs;  // #DEF_true drive plus phase activation from the MAX sending activation over any MarkerConSpec inputs -- allows TAN's to learn to fire for expected or actual outcomes conveyed from PV and VSPatch outcome-predicting neurons
  bool          send_plus;      // #CONDSHOW_ON_plus_fm_pv_vs send the plus phase training values to ach on receivers (otherwise only send the learned minus phase activation)
  
  STATE_DECO_KEY("UnitSpec");
  STATE_TA_STD_CODE_SPEC(TANActSpec);
private:
  void  Initialize()    { Defaults_init(); }
  void  Defaults_init() { plus_fm_pv_vs = true;  send_plus = true; }
};



class STATE_CLASS(GPiGateSpec) : public STATE_CLASS(SpecMemberBase) {
  // ##INLINE ##NO_TOKENS ##CAT_Leabra timing of gating 
INHERITED(SpecMemberBase)
public:
  int           gate_cyc;       // #DEF_18 cycle within quarter to apply gating -- see gating_qtr for quarters when gating is computed -- we send thal_gate on this cycle in those quarters, regardless of whether our activation is over gating threshold
  bool          updt_net;        // we update the LeabraNetwork.times.thal_gate_cycle value whenver our gating timing is activated -- if there are multiple gating layers then might want to only listen to one of them -- importantly, this should not affect actual functioning of PBWM system, which depends on direct thal_gate signals -- only affects recording of act_g gating values for units outside of PBWM
  
  STATE_DECO_KEY("UnitSpec");
  STATE_TA_STD_CODE_SPEC(GPiGateSpec);
private:
  void  Initialize()    {   updt_net = true;  Defaults_init(); }
  void  Defaults_init() {   gate_cyc = 18; }
};


class STATE_CLASS(GPiMiscSpec) : public STATE_CLASS(SpecMemberBase) {
  // ##INLINE ##NO_TOKENS ##CAT_Leabra weighting of Go vs. NoGo inputs and other GPi params
INHERITED(SpecMemberBase)
public:
  float         net_gain;        // #DEF_3 extra netinput gain factor to compensate for reduction in netinput from subtracting away nogo -- this is IN ADDITION to adding the nogo factor as an extra gain: net = (net_gain + nogo) * (go_in - nogo * nogo_in)
  float         nogo;            // #MIN_0 #DEF_1;0.1 how much to weight NoGo inputs relative to Go inputs (which have an implied weight of 1.0)
  float         gate_thr;        // #DEF_0.2 threshold applied to activation to drive gating -- when any unit activation gets above this threshold, it sends the activation through sending projections to the thal field in Matrix units, otherwise it sends a 0
  bool          thr_act;         // #DEF_true apply threshold to unit act_eq activations (i.e., turn off units that are below threshold) -- this is what is sent to the InvertUnitSpec, so we effectively threshold the gating output
  float         min_thal;        // #DEF_0.2 minimum thal value to send to thal on layers we project to -- range between gate_thr and 1.0 is mapped into min_thal:1.0 range -- e.g., setting to 1.0 will effectively produce binary gating outputs -- set to 0 (or < gate_thr) to retain raw gate_thr to 1.0 range

  float         tot_gain;        // #EXPERT net_gain + nogo
  float         thal_rescale;    // #EXPERT (1.0 - min_thal) / (1.0 - gate_thr) -- multiplier for rescaling thal value from act_eq >= gate_thr onto min_thal range
  
  STATE_DECO_KEY("UnitSpec");
  STATE_TA_STD_CODE_SPEC(GPiMiscSpec);
  STATE_UAE( UpdateVals(); );
private:
  void  UpdateVals()    {
    tot_gain = net_gain + nogo;
    thal_rescale = (1.0f - min_thal) / (1.0f - gate_thr);
  }
  void  Initialize()    {   Defaults_init();  }
  void  Defaults_init() {
    net_gain = 3.0f;  nogo = 1.0f;  gate_thr = 0.2f;  thr_act = true;  min_thal = 0.2f;
    UpdateVals();
  }
};


class STATE_CLASS(PFCGateSpec) : public STATE_CLASS(SpecMemberBase) {
  // ##INLINE ##NO_TOKENS ##CAT_Leabra specifications for PFC gating functions
INHERITED(SpecMemberBase)
public:
  bool          out_gate;       // if true, this PFC layer is an output gate layer, which means that it only has transient activation during gating
  bool          out_q1only;     // #DEF_true #CONDSHOW_ON_out_gate for output gating, only compute gating in first quarter -- do not compute in 3rd quarter -- this is typically true, and deep_raw_qtr is typically set to only Q1 as well -- does deep raw updating immediately after first quarter gating signal -- allows gating signals time to influence performance within a single trial
  float         mnt_thal;       // #DEF_1 effective thal activation to use in computing the deep_raw activation sent from super to deep layers, for continued maintenance beyond the initial thal signal provided by the BG -- also sets and effective minimum thal value regardless of the actual gating thal value
  float         gate_thr;       // #DEF_0.2 threshold on thalamic gating signal to drive gating -- when using GpiInvUnitSpec gpi, this parameter ususally doesn't matter!  set the gpi.gate_thr value instead -- the only constraint is that this value be <= gpi.min_thal as that determines the minimum thalamic value for gated stripes
  
  STATE_DECO_KEY("UnitSpec");
  STATE_TA_STD_CODE_SPEC(PFCGateSpec);
private:
  void	Initialize()    {   out_gate = false;  Defaults_init(); }
  void	Defaults_init() {   out_q1only = true;  mnt_thal = 1.0f;  gate_thr = 0.2f; }
};


class STATE_CLASS(PFCMaintSpec) : public STATE_CLASS(SpecMemberBase) {
  // ##INLINE ##NO_TOKENS ##CAT_Leabra specifications for PFC maintenance functions
INHERITED(SpecMemberBase)
public:
  float         s_mnt_min;      // #DEF_0.3 for superficial neurons, how much of deep_lrn to add into excitatory net input to support maintenance, from deep maintenance signal -- 0.25 is generally minimum to support maintenance -- this is the minimum amount to add as a function of the strength of afferent netinput (less maint current with stronger afferents) -- see
  float         s_mnt_max;      // #DEF_0.5 for superficial neurons, how much of deep_lrn to add into excitatory net input to support maintenance, from deep maintenance signal -- 0.25 is generally minimum to support maintenance -- this is the maximum amount to add as a function of the strength of afferent netinput (less maint current with stronger afferents)
  float         mnt_net_max;    // #DEF_0.5 maximum netin_raw.max value required to drive the minimum s_mnt_min maintenance current from deep -- anything above this drives the same s_mnt_min value -- below this value scales the effective mnt current between s_mnt_min to s_mnt_max in reverse proportion to netin_raw.max value
  float         clear;          // #MIN_0 #MAX_1 #DEF_0.5 how much to clear out (decay) super activations when the stripe itself gates and was previously maintaining something, or for maint pfc stripes, when output go fires and clears
  bool          use_dyn;        // use fixed dynamics for updating deep_ctxt activations -- defined in dyn_table -- this also preserves the initial gating deep_ctxt value in misc_1 -- otherwise it is up to the recurrent loops between super and deep for maintenance
  int           max_mnt;        // #MIN_1 #DEF_1;100 maximum duration of maintenance for any stripe -- beyond this limit, the maintenance is just automatically cleared -- typically 1 for output gating and 100 for maintenance gating
  
  STATE_DECO_KEY("UnitSpec");
  STATE_TA_STD_CODE_SPEC(PFCMaintSpec);
private:
  void	Initialize()    {   max_mnt = 100;  Defaults_init(); }
  void	Defaults_init() {
    s_mnt_min = 0.3f;  s_mnt_max = 0.5f;  mnt_net_max = 0.5f;  clear = 0.5f;
    use_dyn = true;
  }
};


class STATE_CLASS(PFCDynEl) : public STATE_CLASS(taOBase) {
  // ##INLINE ##NO_TOKENS ##CAT_Leabra PFC dynamic behavior element -- defines the dynamic behavior of deep layer PFC units
INHERITED(taOBase)
public:
  float         init;      // initial value at point when gating starts
  float         rise_tau;  // time constant for linear rise in maintenance activation (per quarter when deep is updated) -- use integers -- if both rise and decay then rise comes first
  float         decay_tau; // time constant for linear decay in maintenance activation (per quarter when deep is updated) -- use integers -- if both rise and decay then rise comes first

  INLINE void SetVals(float in, float rise, float decay) {
    init = in; rise_tau = rise; decay_tau = decay;
  }
  // set values 
  
  STATE_DECO_KEY("UnitSpec");
  STATE_TA_STD_CODE(PFCDynEl);
private:
  void	Initialize()    { init = 1.0f; rise_tau = 0.0f;  decay_tau = 0.0f; }
};


class STATE_CLASS(ThetaPhaseSpecs) : public STATE_CLASS(SpecMemberBase) {
  // ##INLINE ##NO_TOKENS ##CAT_Leabra ThetaPhase hippocampal dynamic specifications, affecting how the different inputs to the CA1 are modulated as a function of position within the theta phase cycle
INHERITED(SpecMemberBase)
public:
  bool          mod_ec_out;             // #DEF_true modulate ECout projection strength in same way as ECin projections -- i.e., when ECin is off and CA3 is on during recall mode, ECout projections to CA1 are also off 
  bool          ca3_on_p;               // #DEF_false are CA3 projections active into CA1 during the plus phase?  according to phase modulation these should be off, but on the other hand, error-driven learning theory says that they should be on in the plus phase, so that the plus is as similar to the minus as possible.  the need for this option results from a fundamental confusion of training signals due to the common plus phase used for both the EC-CA1 auto-encoder and the CA3->CA1 recall pathway
  float		recall_decay; 		// #DEF_0;1 proportion to decay activations at start of recall phase
  float		plus_decay; 		// #DEF_0;1 proportion to decay activations at start of plus phase
  bool		use_test_mode;		// #DEF_true if network train_mode == TEST, then don't modulate CA3 off in plus phase, and keep ECin -> CA1 on, and don't decay -- makes it more likely to at least get input parts right

  STATE_DECO_KEY("UnitSpec");
  STATE_TA_STD_CODE_SPEC(ThetaPhaseSpecs);
private:
  void	Initialize()    {   Defaults_init(); }
  void	Defaults_init() {
    mod_ec_out = true;  ca3_on_p = false;  recall_decay = 1.0f;  plus_decay = 0.0f;
    use_test_mode = true;
  }
};


class STATE_CLASS(CerebGranuleSpecs) : public STATE_CLASS(SpecMemberBase) {
  // ##INLINE ##NO_TOKENS ##CAT_Leabra parameters for the cerebellar granule unit
INHERITED(SpecMemberBase)
public:
  float         act_thr;        // #DEF_0.5 activation threshold for initiating eligibility for learning and triggering the inhibitory window
  int           inhib_start_time; // #DEF_10 how long after unit crosses act_thr do we start inhibiting
  int           lrn_start_time;  // #DEF_60 starting time window (in cycles) for learning -- how long after activity crosses act_thr threshold does the learning window start?
  int           lrn_end_time;    // #DEF_90 ending time window (in cycles) for learning -- the learning window closes after this time
  float         inhib_net_pct;   // #DEF_0.5 multiplier on net input to decrease it when a unit becomes inhibited -- going all the way to 0 reduces the effectiveness of FF_FB inhib..

  STATE_DECO_KEY("UnitSpec");
  STATE_TA_STD_CODE_SPEC(CerebGranuleSpecs);
private:
  void	Initialize()    {
    act_thr = 0.5f;  inhib_start_time = 10;  lrn_start_time = 60;  lrn_end_time = 90;
    inhib_net_pct = 0.5f;
  }
  void	Defaults_init() {  };
};



class STATE_CLASS(CtxtUpdateSpec) : public STATE_CLASS(SpecMemberBase) {
  // ##INLINE ##NO_TOKENS ##CAT_Leabra context updating specifications
INHERITED(SpecMemberBase)
public:
  float		fm_hid;		// from hidden (inputs to context layer)
  float		fm_prv;		// from previous context layer values (maintenance)
  float		to_out;		// outputs from context layer

  STATE_DECO_KEY("UnitSpec");
  STATE_TA_STD_CODE_SPEC(CtxtUpdateSpec);
private:
  void	Initialize()    {
    fm_hid = 1.0f;  fm_prv = 0.0f;  to_out = 1.0f;
  }
  void	Defaults_init() { }; 	// note: does NOT do any init -- these vals are not really subject to defaults in the usual way, so don't mess with them
};


class STATE_CLASS(CtxtNSpec) : public STATE_CLASS(SpecMemberBase) {
  // ##INLINE ##NO_TOKENS ##CAT_Leabra context counting specifications
INHERITED(SpecMemberBase)
public:
  int		n_trials;	// #MIN_1 update every n trials
  int		n_offs;		// #MIN_0 offset of n during count, ex using 2 lays with 0 and N/2 gives half alternating offset

  STATE_DECO_KEY("UnitSpec");
  STATE_TA_STD_CODE_SPEC(CtxtNSpec);
private:
  void	Initialize() 	{ n_trials = 2; n_offs = 0; }
  void	Defaults_init() { }; 	// note: does NOT do any init -- these vals are not really subject to defaults in the usual way, so don't mess with them
};



