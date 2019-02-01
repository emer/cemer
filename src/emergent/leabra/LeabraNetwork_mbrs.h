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
// it must be included directly in LeabraUnitSpec.h, _cpp.h, _cuda.h
// the STATE_CLASS macro will define a _cpp _cuda or plain (no suffix) version

// this pragma ensures that maketa properly grabs this type information even though
// this file is included in the other files -- we get ta info for main and _cpp, not cuda

#ifdef __MAKETA__
#pragma maketa_file_is_target LeabraNetwork
#pragma maketa_file_is_target LeabraNetworkState
#endif


class STATE_CLASS(LeabraTimes) : public STATE_CLASS(taOBase) {
  // ##INLINE ##NO_TOKENS ##CAT_Leabra phase parameters for what phases are run and how long the 
INHERITED(taOBase)
public:
  bool          cycle_qtr;      // #DEF_true #NO_SAVE one CycleRun runs for a full quarter number of actual cycles -- this greatly speeds up processing by reducing threading overhead, but prevents e.g., interactive viewing at the individual cycle level -- this is not saved -- have to re-engage it when needed, to prevent unintentionally slowing everything down
  int           quarter;        // #DEF_25 number of cycles to run per each quarter of a trial -- typically, a trial is one alpha cycle of 100 msec (10 Hz), and we run at 1 cycle = 1 msec, so that is 25 cycles per quarter, which gives the ubiquitous gamma frequency power of 40 Hz -- a traditional minus phase takes the first 3 quarters, and the last quarter is the plus phase -- use CycleRunMax_Minus and CycleRunMax_Plus to get proper minus and plus phase cycles values to use in programs, taking into account lthreads.quarter setting
  int           deep_cyc;       // #DEF_5 how often (in cycles) to perform deep layer updating -- typically not necessary to update as frequently as superficial activations -- and biologically driven by layer 5 ib neurons that emit just a few bursts
  float         time_inc;       // #DEF_0.001 in units of seconds -- how much to increment the network time variable every cycle -- this goes monotonically up from the last weight init or manual reset -- default is .001 which means one cycle = 1 msec -- MUST also coordinate this with LeabraUnitSpec.dt.integ for most accurate time constants -- also affects rate-code computed spiking intervals in unit spec
  int           wt_bal_int;     // #DEF_10 weight balance update interval: if weight balance soft normalization is being used, this is how frequently to update in terms of trials (1 = every trial, 2 = every other trial, etc) -- this is relatively computationally intensive and doesn't typically need to be done too frequently

  int           minus;          // #READ_ONLY computed total number of cycles per minus phase = 3 * quarter
  int           plus;           // #READ_ONLY computed total number of cycles per plus phase = quarter
  int           total_cycles;   // #READ_ONLY computed total number of cycles per trial
  int           thal_gate_cycle; // #READ_ONLY #SHOW BG / thalamus layers can be configured to update this value to record cycle when gating occurred -- other units by default will record their activations to act_g variable on this cycle

  STATE_DECO_KEY("Network");
  STATE_TA_STD_CODE(LeabraTimes);
  STATE_UAE(   minus = 3 * quarter;  plus = quarter;  total_cycles = minus + plus; );
private:
  void  Initialize() {
    quarter = 25;
    cycle_qtr = true;
    deep_cyc = 5;
    time_inc = 0.001f;
    wt_bal_int = 10;

    minus = 3 * quarter;
    plus = quarter;
    total_cycles = minus + plus;
    thal_gate_cycle = -2;
  }    
};


class STATE_CLASS(LeabraNetStats) : public STATE_CLASS(taOBase) {
  // ##INLINE ##NO_TOKENS ##CAT_Leabra leabra network-level statistics parameters
INHERITED(taOBase)
public:
  float         trg_max_act_crit; // #CAT_Statistic criterion for target-layer maximum activation (trg_max_act) -- rt_cycles is recorded when trg_max_act first exceeds this criterion
  bool          off_errs;       // #DEF_true #CAT_Statistic include in norm_err computation units that were incorrectly off (should have been on but were actually off) -- either 1 or both of off_errs and on_errs must be set
  bool          on_errs;        // #DEF_true #CAT_Statistic include in norm_err computation units that were incorrectly on (should have been off but were actually on) -- either 1 or both of off_errs and on_errs must be set
  bool          agg_unlearnable; // #DEF_false #CAT_Statistic should unlearnable trials be aggregated into epoch-level summary stats?  default is not to (i.e., false)
  bool          wt_bal;         // #DEF_false #CAT_Statistic aggregate weight balance statistics per projection when wt_balance mechanism is active (soft form of weight normalization) -- see times.wt_bal_int for interval when these are updated


  STATE_DECO_KEY("Network");
  STATE_TA_STD_CODE(LeabraNetStats);
private:
  void  Initialize() {
    trg_max_act_crit = 0.5f;
    off_errs = true;
    on_errs = true;
    agg_unlearnable = false;
    wt_bal = false;
  }
};


class STATE_CLASS(LeabraNetMisc) : public STATE_CLASS(taOBase) {
  // ##INLINE ##NO_TOKENS ##CAT_Leabra misc network-level parameters for Leabra
INHERITED(taOBase)
public:
  bool          spike;         // #GUI_READ_ONLY #SHOW using discrete spiking -- all units must be either rate code or spiking, to optimize the computation -- updated in Trial_Init_Specs call
  bool          deep;         // #GUI_READ_ONLY #SHOW deep processing is active -- updated in Trial_Init_Specs call
  bool          bias_learn;     // #GUI_READ_ONLY #SHOW do any of the bias connections have learning enabled?  if true, then an extra unit-level computational step is required -- bias learning is now OFF by default, as it has no obvious benefits in large models, but may be useful for smaller networks
  bool          trial_decay;   // #GUI_READ_ONLY #SHOW at least one layer spec has a non-zero level of trial decay -- if all layers have 0 trial decay, then the net input does not need to be reset between trials, yielding significantly faster performance
  bool          diff_scale_p;   // #GUI_READ_ONLY #SHOW a unitspec such as the hippocampus ThetaPhase units rescales inputs in plus phase -- this requires initializing the net inputs between these phases
  bool          diff_scale_q1;  // #GUI_READ_ONLY #SHOW at least one unit spec rescales inputs at start of second quarter, such as hippocampus ThetaPhase units -- this requires initializing the net inputs at this point
  bool          wt_bal;       // #GUI_READ_ONLY #SHOW wt_bal weight balancing is being used -- this must be done as a separate step -- LeabraConSpec will set this flag if LeabraConSpec::wt_bal.on flag is on, and off if not -- updated in Trial_Init_Specs call
  bool          lay_gp_inhib;   // #GUI_READ_ONLY #SHOW layer group level inhibition is active for some layer groups -- may cause some problems with asynchronous threading operation -- updated in Trial_Init_Specs call
  bool          inhib_cons;     // #GUI_READ_ONLY #SHOW inhibitory connections are being used in this network -- detected during buildunits_threads to determine how netinput is computed -- sets NETIN_PER_PRJN flag

  INLINE void   ResetInitSpecsFlags() {
    spike = false;
    bias_learn = false;
    trial_decay = false;
    diff_scale_p = false;
    diff_scale_q1 = false;
    wt_bal = false;
    lay_gp_inhib = false;
  }
  // reset flags before Trial_InitSpecs call -- everything except inhib_cons usually

    
  STATE_DECO_KEY("Network");
  STATE_TA_STD_CODE(LeabraNetMisc);
private:
  void  Initialize() {
    ResetInitSpecsFlags();
    inhib_cons = false;
  }
};


class STATE_CLASS(LeabraNetDeep) : public STATE_CLASS(taOBase) {
  // ##INLINE ##NO_TOKENS ##CAT_Leabra misc network-level parameters for DeepLeabra
INHERITED(taOBase)
public:
  enum Quarters {               // #BITS specifies gamma frequency quarters within an alpha-frequency trial on which to do things
    QNULL = 0x00,              // #NO_BIT no quarter (yeah..)
    Q1 = 0x01,                 // first quarter
    Q2 = 0x02,                 // second quarter
    Q3 = 0x04,                 // third quarter -- posterior cortical minus phase
    Q4 = 0x08,                 // fourth quarter -- posterior cortical plus phase
    Q2_Q4 = Q2 | Q4,           // #NO_BIT standard beta frequency option, for bg, pfc
    QALL = Q1 | Q2 | Q3 | Q4,  // #NO_BIT all quarters
  };

  bool         on;           // #GUI_READ_ONLY #SHOW deep.on was on in some units
  bool         ctxt;         // #GUI_READ_ONLY #SHOW DeepCtxtConSpec's were found -- deep context values will be updated
  bool         raw_net;      // #GUI_READ_ONLY #SHOW SendDeepRawConSpec's were found -- deep_raw_net values will be updated
  bool         mod_net;      // #GUI_READ_ONLY #SHOW SendDeepModConSpec's were found -- deep_mod_net values will be updated
  Quarters     raw_qtr;      // #GUI_READ_ONLY #SHOW aggregated from LeabraUnitSpec deep_raw_qtr values: quarter(s) during which deep_raw layer 5 intrinsic bursting activations should be updated -- deep_raw is updated and sent to deep_raw_net during this quarter, and deep_ctxt is updated right after this quarter (wrapping around to the first quarter for the 4th quarter)

  INLINE  bool Quarter_DeepRawNow(int qtr)
  { return raw_qtr & (1 << qtr); }

  INLINE  bool Quarter_DeepRawPrevQtr(int qtr)
  { if(qtr == 0) qtr = 3; else qtr--; return raw_qtr & (1 << qtr); }

  STATE_DECO_KEY("Network");
  STATE_TA_STD_CODE(LeabraNetDeep);
private:
  void  Initialize() {
    on = false;
    ctxt = false;
    raw_net = false;
    mod_net = false;
    raw_qtr = QNULL;
  }
};


class STATE_CLASS(RelNetinSched) : public STATE_CLASS(taOBase) {
  // ##INLINE ##NO_TOKENS ##CAT_Leabra schedule for computing relative netinput values for each projection -- this is very important data for tuning the network to ensure that each layer has the relative impact it should on other layers -- however it is expensive (only if not using NETIN_PER_PRJN, otherwise it is automatic and these options are disabled), so this schedules it to happen just enough to get the results you want
INHERITED(taOBase)
public:
  bool          on;             // #DEF_true whether to compute relative netinput at all
  int           trl_skip;       // #DEF_10 #MIN_1 #CONDSHOW_ON_on skip every this many trials for epochs where it is being computed -- typically do not need sample all the trials -- adjust this depending on how many trials are typical per epoch
  int           epc_skip;       // #DEF_10 #MIN_1 #CONDSHOW_ON_on skip every this many epochs -- typically just need to see rel_netin stats for 1st epoch and then every so often thereafter

  INLINE bool   ComputeNow(int net_epc, int net_trl)
  { if(on && (net_epc % epc_skip == 0) && (net_trl % trl_skip == 0)) return true;
    return false; }
  // should we compute relative netin now, based on network epoch and trial counters?

  STATE_DECO_KEY("Network");
  STATE_TA_STD_CODE(RelNetinSched);
private:
  void  Initialize() {
    on = true;
    trl_skip = 10;
    epc_skip = 10;
  }

};
