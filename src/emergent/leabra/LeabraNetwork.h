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

#ifndef LeabraNetwork_h
#define LeabraNetwork_h 1

// parent includes:
#include <Network>

eTypeDef_Of(LeabraAvgMax);
eTypeDef_Of(LeabraInhibVals);
eTypeDef_Of(LeabraTwoDVals);
eTypeDef_Of(LeabraMarginVals);

// member includes:
#include <taMath_float>
#include <Average>
#include <LeabraUnit>
#include <LeabraLayer>
#include <LeabraPrjn>
#include <int_Array>

// declare all other types mentioned but not required to include:
class DataTable; // 

eTypeDef_Of(LeabraNetTiming);

class E_API LeabraNetTiming : public NetTiming {
  // timers for Leabra network functions
INHERITED(NetTiming)
public:
  TimeUsedHR   netin_integ;     // Compute_NetinInteg integrate net inputs
  TimeUsedHR   netin_stats;     // Compute_NetinStats netin stats
  TimeUsedHR   inhib;           // Compute_Inhib inhibition
  TimeUsedHR   act_post;        // Compute_Act_Post post integration
  TimeUsedHR   cycstats;        // Compute_CycleStats cycle statistics

  TA_SIMPLE_BASEFUNS(LeabraNetTiming);
private:
  void	Initialize()    { };
  void 	Destroy()	{ };
};


eTypeDef_Of(LeabraNetworkState_cpp);
eTypeDef_Of(LeabraLayerState_cpp);
eTypeDef_Of(LeabraPrjnState_cpp);
eTypeDef_Of(LeabraUnGpState_cpp);
eTypeDef_Of(LeabraUnitState_cpp);
eTypeDef_Of(LeabraConState_cpp);

eTypeDef_Of(LeabraLayerSpec_cpp);
eTypeDef_Of(LeabraUnitSpec_cpp);
eTypeDef_Of(LeabraConSpec_cpp);

eTypeDef_Of(LeabraTimes);
eTypeDef_Of(LeabraNetStats);
eTypeDef_Of(LeabraNetMisc);
eTypeDef_Of(LeabraNetDeep);
eTypeDef_Of(RelNetinSched);
eTypeDef_Of(LeabraNetwork);

#include <LeabraNetwork_mbrs>

class E_API LeabraNetwork : public Network {
  // #STEM_BASE ##CAT_Leabra network that uses the Leabra algorithms and objects
INHERITED(Network)
public:

#include <LeabraNetwork_core>
  
  String	minus_output_name; // #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Statistic #VIEW output_name in the minus phase -- for recording in logs as network's response (output_name in plus phase is clamped target value)

  inline LeabraNetworkState_cpp* LeabraNetState() { return (LeabraNetworkState_cpp*)net_state; }
  // #CAT_State leabra network state
 
  ///////////////////////////////////////////////////////////////////////
  //    Build functionality

  virtual void	CheckInhibCons();
  void	Build() override;
  void  BuildNullUnit() override;
  virtual void BuildLeabraThreadMem();
  // #IGNORE
  inline void InitLeabraThreadMem_Thr(int thr_no)  { LeabraNetState()->InitLeabraThreadMem_Thr(thr_no); }
  // #IGNORE

  ///////////////////////////////////////////////////////////////////////
  //	General Init functions

  void	Init_Acts() override;

  inline void  Init_AdaptInhib() { LeabraNetState()->Init_AdaptInhib(); }
  // #CAT_Activation Initialize adaptive inhibition gain value on all the layers -- undoes any adaptation that has taken place (including from loaded weights - the adaptive gain value is saved with the weights)

  inline void  Init_Acts_Layer() { LeabraNetState()->Init_Acts_Layer(); }
  // #CAT_TrialInit layer-level acts init
  
  virtual void  Init_Netins();
  // #CAT_Activation initialize netinput computation variables (delta-based requires several intermediate variables)
  inline void  Init_Netins_Thr(int thr_no) { LeabraNetState()->Init_Netins_Thr(thr_no); }
  // #IGNORE initialize netinput computation variables (delta-based requires several intermediate variables)

  virtual void	DecayState(float decay);
  // #CAT_Activation decay activation states towards initial values by given amount (0 = no decay, 1 = full decay)
    inline void DecayState_Thr(int thr_no)  { LeabraNetState()->DecayState_Thr(thr_no); }
    // #IGNORE decay activation states towards initial values by given amount (0 = no decay, 1 = full decay)
  
  virtual void ResetSynTR();
  // #CAT_Activation Reset Synaptic transmitter availability to full (1.0)
  inline  void ResetSynTR_Thr(int thr_no)  { LeabraNetState()->ResetSynTR_Thr(thr_no); }
  // #IGNORE

  ///////////////////////////////////////////////////////////////////////
  //	TrialInit -- at start of trial

  virtual void 	Trial_Init();
  // #CAT_TrialInit initialize at start of trial (init specs, Decay state)
  inline void Trial_Init_Counters() { LeabraNetState()->Trial_Init_Counters(); }
    // #CAT_TrialInit init counters -- network only
    virtual void Trial_Init_Specs();
    // #CAT_TrialInit initialize specs and specs update network flags

    virtual void Trial_Init_Unit();
    // #CAT_TrialInit trial unit-level initialization functions: Trial_Init_SRAvg, DecayState, NoiseInit
    inline void Trial_Init_Unit_Thr(int thr_no)  { LeabraNetState()->Trial_Init_Unit_Thr(thr_no); }
    // #IGNORE trial unit-level initialization functions: Trial_Init_SRAvg, DecayState, NoiseInit
    inline void Trial_Init_Layer()  { LeabraNetState()->Trial_Init_Layer(); }
    // #CAT_TrialInit layer-level trial init (used in base code to init layer-level sravg, can be overloaded)

  ///////////////////////////////////////////////////////////////////////
  //	QuarterInit -- at start of a given quarter trial of processing

  virtual void  Quarter_Init();
  // #CAT_QuarterInit initialize network for quarter-level processing (hard clamp, netscale)
    inline void Quarter_Init_Counters() { LeabraNetState()->Quarter_Init_Counters(); }
    // #CAT_QuarterInit initialize counters for upcoming quarter -- network only
    virtual void Quarter_Init_Unit();
    // #CAT_QuarterInit quarter unit-level initialization functions: Init_TargFlags, NetinScale
      inline void Quarter_Init_Unit_Thr(int thr_no)  { LeabraNetState()->Quarter_Init_Unit_Thr(thr_no); }
      //IGNORE quarter unit-level initialization functions: Init_TargFlags, NetinScale
    inline void Quarter_Init_Layer() { LeabraNetState()->Quarter_Init_Layer(); }
    // #CAT_QuarterInit quarter layer-level initialization hook -- default calls TargFlags_Layer, and can be used for hook for other guys
      virtual void Quarter_Init_TargFlags();
      // #CAT_QuarterInit initialize at start of settling phase -- sets target external input flags based on phase -- not called by default -- direct to unit level function
      inline void Quarter_Init_TargFlags_Thr(int thr_no)
      { LeabraNetState()->Quarter_Init_TargFlags_Thr(thr_no); }
      // #IGNORE initialize at start of settling phase -- sets target external input flags based on phase -- not called by default -- direct to unit level function
      inline void Quarter_Init_TargFlags_Layer() { LeabraNetState()->Quarter_Init_TargFlags_Layer(); }
      // #IGNORE initialize at start of settling phase -- sets target external input flags based on phase -- not called by default -- direct to unit level function
      virtual void Compute_NetinScale();
      // #CAT_QuarterInit compute netinput scaling values by projection -- not called by default -- direct to unit-level function
      inline void Compute_NetinScale_Thr(int thr_no)  { LeabraNetState()->Compute_NetinScale_Thr(thr_no); }
      // #IGNORE compute netinput scaling values by projection -- not called by default -- direct to unit-level function
      virtual void Compute_NetinScale_Senders();
      // #CAT_QuarterInit compute net input scaling values for sending cons -- copies from values computed in the recv guys -- has to be done as a second phase of the Quarter_Init_Unit stage after all the recv ones are computed
      inline void Compute_NetinScale_Senders_Thr(int thr_no)
      { LeabraNetState()->Compute_NetinScale_Senders_Thr(thr_no); }
      // #IGNORE compute net input scaling values for sending cons -- copies from values computed in the recv guys -- has to be done as a second phase of the
    virtual void Quarter_Init_Deep();
    // #CAT_QuarterInit quarter deep leabra init: deep_ctxt compute and deep state update
      inline void Quarter_Init_Deep_Thr(int thr_no)  { LeabraNetState()->Quarter_Init_Deep_Thr(thr_no); }
      // #IGNORE quarter deep leabra init: deep_ctxt compute and deep state update
      inline void InitDeepRawNetinTmp_Thr(int thr_no)  { LeabraNetState()->InitDeepRawNetinTmp_Thr(thr_no); }
      // #IGNORE initialize deep_raw netin temp buffer
      inline void Compute_DeepCtxtStats_Thr(int thr_no)
      { LeabraNetState()->Compute_DeepCtxtStats_Thr(thr_no); }
      // #IGNORE compute layer and unit-group level stats on deep_ctxt
      inline void Compute_DeepCtxtStats_Post()  { LeabraNetState()->Compute_DeepCtxtStats_Post(); }
      // #IGNORE compute layer and unit-group level stats on deep_ctxt

    virtual void Compute_HardClamp();
    // #CAT_QuarterInit compute hard clamping from external inputs
      inline void Compute_HardClamp_Thr(int thr_no)  { LeabraNetState()->Compute_HardClamp_Thr(thr_no); }
      // #IGNORE compute hard clamping from external inputs
      inline void Compute_HardClamp_Layer()  { LeabraNetState()->Compute_HardClamp_Layer(); }
      // #IGNORE compute hard clamping from external inputs

    virtual void ExtToComp();
    // #CAT_QuarterInit move external input values to comparison values (not currently used)
      inline void ExtToComp_Layer()  { LeabraNetState()->ExtToComp_Layer(); }
      // #IGNORE
      inline void ExtToComp_Thr(int thr_no)  { LeabraNetState()->ExtToComp_Thr(thr_no); }
      // #IGNORE
    virtual void TargExtToComp();
    // #CAT_QuarterInit move target and external input values to comparison
      inline void TargExtToComp_Layer()  { LeabraNetState()->TargExtToComp_Layer(); }
      // #IGNORE
      inline void TargExtToComp_Thr(int thr_no)  { LeabraNetState()->TargExtToComp_Thr(thr_no); }
      // #IGNORE

  virtual void  NewInputData_Init();
  // #CAT_QuarterInit perform initialization stuff needed to update external input data signals so they actually show up as activations in the network: Quarter_Init_Layer, Quarter_Init_TrgFlags, Compute_HardClamp

  ////////////////////////////////////////////////////////////////
  //	Cycle_Run
  
  inline  int   CycleRunMax()
  { if(times.cycle_qtr) return 1; return times.quarter; }
  // #CAT_Quarter max loop counter for running cycles in a gamma quarter of processing, taking into account the fact that threading can run multiple cycles per Cycle_Run call if quarter flag is set

  virtual void	Cycle_Run();
  // #CAT_Cycle compute cycle(s) of updating: netinput, inhibition, activations -- multiple cycles can be run depending on lthreads.n_cycles setting and whether multiple threads are actually being used -- see lthreads.n_threads_act
    inline void Cycle_Run_Thr(int thr_no)  { LeabraNetState()->Cycle_Run_Thr(thr_no); }
    // #IGNORE compute cycle(s) of updating: netinput, inhibition, activations -- multiple cycles can be run depending on lthreads.n_cycles setting and whether multiple 

  inline void  Cycle_IncrCounters()  { LeabraNetState()->Cycle_IncrCounters(); }
  // #CAT_Cycle increment the cycle-level counters -- called internally during Cycle_Run()

  ///////////////////////////////////////////////////////
  //	Cycle Stage 1: netinput

  inline void Compute_NetinInteg_Thr(int thr_no)  { LeabraNetState()->Compute_NetinInteg_Thr(thr_no); }
  // #IGNORE integrate newly-computed netinput delta values into a resulting complete netinput value for the network (does both excitatory and inhibitory)
  inline void Compute_NetinStats_Thr(int thr_no)  { LeabraNetState()->Compute_NetinStats_Thr(thr_no); }
  // #IGNORE compute layer and unit-group level stats on net input levels -- needed for inhibition
  inline void Compute_NetinStats_Post()  { LeabraNetState()->Compute_NetinStats_Post(); }
  // #IGNORE compute layer and unit-group level stats on net input levels -- needed for inhibition
  inline void Compute_DeepModStats_Thr(int thr_no)  { LeabraNetState()->Compute_DeepModStats_Thr(thr_no); }
  // #IGNORE compute layer and unit-group level stats on deep_mod_net input levels
  inline void Compute_DeepModStats_Post()  { LeabraNetState()->Compute_DeepModStats_Post(); }
  // #IGNORE compute layer and unit-group level stats on deep_mod_net input levels
  inline void InitCycleNetinTmp_Thr(int thr_no)  { LeabraNetState()->InitCycleNetinTmp_Thr(thr_no); }
  // #IGNORE initialize deep_raw netin temp buffer

  ///////////////////////////////////////////////////////////////////////
  //	Cycle Step 2: Inhibition

  inline void	Compute_Inhib()  { LeabraNetState()->Compute_Inhib(); }
  // #IGNORE compute inhibitory conductances via inhib functions (FFFB) -- calls Compute_NetinStats and LayInhibToGps to coordinate group-level inhibition sharing
    virtual void Compute_Inhib_LayGp();
    // #IGNORE compute inhibition across layer groups -- if layer spec lay_gp_inhib flag is on anywhere

  ///////////////////////////////////////////////////////////////////////
  //	Cycle Step 3: Activation

  inline void	Compute_Act_Rate_Thr(int thr_no)  { LeabraNetState()->Compute_Act_Rate_Thr(thr_no); }
  // #IGNORE rate coded activations
  inline void	Compute_Act_Spike_Thr(int thr_no)  { LeabraNetState()->Compute_Act_Spike_Thr(thr_no); }
  // #IGNORE spiking activations

  inline void	Compute_Act_Post_Thr(int thr_no)  { LeabraNetState()->Compute_Act_Post_Thr(thr_no); }
  // #IGNORE post processing after activations have been computed -- special algorithm code takes advantage of this stage to send modulator variables -- all such vars should EXCLUSIVELY be sent during this stage, and running average activations (SRAvg) also computed

  inline void  ThalGatedNow()  { LeabraNetState()-> ThalGatedNow(); }
  // #IGNORE record current cycle as gating cycle -- called by enabled thalamic gating layer(s)

  ///////////////////////////////////////////////////////////////////////
  //	Cycle Stats

  inline void	Compute_CycleStats_Pre()  { LeabraNetState()->Compute_CycleStats_Pre(); }
  // #CAT_Cycle compute cycle-level stats -- acts AvgMax, OutputName, etc -- network-level pre-step -- happens after Compute_Act and prior to Compute_Act_Post -- good place to insert any layer-level modification of unit-level activations
  inline void	Compute_CycleStats_Thr(int thr_no)  { LeabraNetState()->Compute_CycleStats_Thr(thr_no); }
  // #IGNORE compute cycle-level stats -- acts AvgMax -- fast layer level computation
  inline void	Compute_ActEqStats_Thr(int thr_no)  { LeabraNetState()->Compute_ActEqStats_Thr(thr_no); }
  // #IGNORE compute cycle-level stats -- acts AvgMax -- fast layer level computation
  inline void	Compute_CycleStats_Post()  { LeabraNetState()->Compute_CycleStats_Post(); }
  // #CAT_Cycle compute cycle-level stats -- acts AvgMax, OutputName, etc -- network-level post-step
    virtual void  Compute_OutputName();
    // #CAT_Statistic compute the output name for layers and the network, based on the unit names of the most-active units in each target layer -- only works if you provide names to the units -- called automatically in Compute_CycleStats_Post()
    inline void  Compute_RTCycles()  { LeabraNetState()-> Compute_RTCycles(); }
    // #CAT_Statistic compute the rt_cycles statistic based on trg_max_act and trg_max_act_crit criterion, only in the minus phase -- this is a good measure for computing the reaction time (RT) of the network, as in a psychological experiment -- called automatically in Compute_CycleStats_Post()

  inline void	Compute_GcIStats_Thr(int thr_no)  { LeabraNetState()->Compute_GcIStats_Thr(thr_no); }
  // #IGNORE compute cycle-level stats -- inhibitory conductance AvgMax -- fast layer level computation
  inline void	Compute_GcIStats_Post()  { LeabraNetState()->Compute_GcIStats_Post(); }
  // #CAT_Cycle compute cycle-level stats -- inhibitory conductance AvgMax -- single thread post-step

  ///////////////////////////////////////////////////////////////////////
  //	DeepLeabra deep_raw Updating -- called after superficial layer updating

  inline void Compute_DeepRaw_Thr(int thr_no)  { LeabraNetState()->Compute_DeepRaw_Thr(thr_no); }
  // #IGNORE update deep_raw variables, using the proper sequence of unit-level calls

    inline void Compute_DeepRawStats_Thr(int thr_no)  { LeabraNetState()->Compute_DeepRawStats_Thr(thr_no); }
    // #IGNORE compute layer and unit-group level stats on deep_raw vars
    inline void Compute_DeepRawStats_Post()  { LeabraNetState()->Compute_DeepRawStats_Post(); }
    // #IGNORE compute layer and unit-group level stats on deep_raw vars
    
  virtual void ClearDeepActs();
  // #CAT_Deep clear all the deep lamina variables -- can be useful to do at discontinuities of experience
    inline void ClearDeepActs_Thr(int thr_no)  { LeabraNetState()->ClearDeepActs_Thr(thr_no); }
    // #IGNORE clear all the deep lamina variables -- can be useful to do at discontinuities of experience

  virtual void ClearMSNTrace();
  // #CAT_PBWM clear the synaptic trace for MSN connections (Medium Spiny Neurons in the Striatum)
    inline void ClearMSNTrace_Thr(int thr_no)  { LeabraNetState()->ClearMSNTrace_Thr(thr_no); }
    // #IGNORE clear the synaptic trace for MSN connections (Medium Spiny Neurons in the Striatum)

  ///////////////////////////////////////////////////////////////////////
  //	Quarter Final

  virtual void	 Quarter_Final();
  // #CAT_QuarterFinal do final processing after each quarter: 
    inline void Quarter_Final_Pre()  { LeabraNetState()->Quarter_Final_Pre(); }
    // #CAT_QuarterFinal perform computations in layers at end of quarter -- this is a pre-stage that occurs prior to final Quarter_Final_impl -- use this for anything that needs to happen prior to the standard Quarter_Final across units and layers (called by Quarter_Final)
    inline void Quarter_Final_Unit_Thr(int thr_no)  { LeabraNetState()->Quarter_Final_Unit_Thr(thr_no); }
    // #IGNORE #CAT_QuarterFinal perform Quarter_Final computations in units at end of quarter (called by Quarter_Final) -- also does CosDiff_Thr
    inline void Quarter_Final_Layer()  { LeabraNetState()->Quarter_Final_Layer(); }
    // #CAT_QuarterFinal perform computations in layers at end of quarter (called by Quarter_Final)
    virtual void Quarter_Compute_dWt();
    // #CAT_QuarterFinal compute weight changes at end of each quarter -- units decide when this actually happens
    inline void Quarter_Final_Counters()   { LeabraNetState()->Quarter_Final_Counters(); }
    // #CAT_QuarterFinal update counters at end of quarter


  ///////////////////////////////////////////////////////////////////////
  //	Trial Update and Final

  virtual void	Trial_Final();
  // #CAT_TrialFinal do final processing after trial: Compute_AbsRelNetin

  ///////////////////////////////////////////////////////////////////////
  //	Learning

  inline void	Compute_dWt_Layer_pre()  { LeabraNetState()->Compute_dWt_Layer_pre(); }
  // #IGNORE do special computations at layer level prior to standard unit-level thread dwt computation -- not used in base class but is in various derived classes

  // virtual void  Compute_dWt_VecVars_Thr(int thr_no);
  // // #IGNORE copy over the vectorized variables for learning

  void	Compute_dWt() override;
    inline void	Compute_WtBal_Thr(int thr_no)  { LeabraNetState()->Compute_WtBal_Thr(thr_no); }
    // #IGNORE compute weight balance factors
    inline void	Compute_WtBalStats()  { LeabraNetState()->Compute_WtBalStats(); }
    // #IGNORE compute weight balance statistics

  void Compute_Weights() override;

  ///////////////////////////////////////////////////////////////////////
  //	Stats

  virtual void  LayerAvgAct(DataTable* report_table = NULL,
                            LeabraLayerSpec* lay_spec = NULL);
  // #BUTTON #ARGC_1 #NULL_OK #NULL_TEXT_NewReportData create a data table with the current layer average activations (acts_m_avg) and the values specified in the layerspec avg_act.init -- this is useful for setting the .init values accurately based on actual levels 

  virtual void	Set_ExtRew(bool avail, float ext_rew_val);
  // #CAT_Statistic set ext_rew_avail and ext_rew value -- for script access to these values
  virtual void	Compute_ExtRew();
  // #CAT_Statistic compute external reward information: called in plus phase stats
  virtual void	Compute_NormErr();
  // #CAT_Statistic compute normalized binary error between act_m and targ unit values: called in TrialStats -- per unit: if (net->lstats.on_errs && act_m > .5 && targ < .5) return 1; if (net->lstats.off_errs && act_m < .5 && targ > .5) return 1; else return 0; normalization is per layer based on k value: total possible err for both on and off errs is 2 * k (on or off alone is just k)
    inline void Compute_NormErr_Thr(int thr_no)  { LeabraNetState()->Compute_NormErr_Thr(thr_no); }
    // #IGNORE
    inline void Compute_NormErr_Agg()  { LeabraNetState()->Compute_NormErr_Agg(); }
    // #IGNORE
  virtual float	Compute_CosErr();
  // #CAT_Statistic compute cosine (normalized dot product) error between act_m and targ unit values
    inline void Compute_CosErr_Thr(int thr_no)  { LeabraNetState()->Compute_CosErr_Thr(thr_no); }
    // #IGNORE
    inline float Compute_CosErr_Agg()  { return LeabraNetState()->Compute_CosErr_Agg(); }
    // #IGNORE
  virtual float	Compute_CosDiff();
  // #CAT_Statistic compute cosine (normalized dot product) phase difference between act_m and act_p unit values -- must be called after PostQuarter (QuarterFinal) for plus phase to get the act_p values
    inline void Compute_CosDiff_Thr(int thr_no)  { LeabraNetState()->Compute_CosDiff_Thr(thr_no); }
    // #IGNORE
    inline float Compute_CosDiff_Agg()  { return LeabraNetState()->Compute_CosDiff_Agg(); }
    // #IGNORE
  virtual float	Compute_AvgActDiff();
  // #CAT_Statistic compute average act_diff (act_p - act_m) -- must be called after PostQuarter (QuarterFinal) for plus phase to get the act_p values -- this is an important statistic to track overall 'main effect' differences across phases 
    inline void Compute_AvgActDiff_Thr(int thr_no)  { LeabraNetState()->Compute_AvgActDiff_Thr(thr_no); }
    // #IGNORE
    inline float Compute_AvgActDiff_Agg()  { return LeabraNetState()->Compute_AvgActDiff_Agg(); }
    // #IGNORE
  virtual float	Compute_TrialCosDiff();
  // #CAT_Statistic compute cosine (normalized dot product) trial activation difference between act_q0 and act_q4 unit values -- must be called after Quarter_Final for plus phase to get the act_q4 values
    inline void Compute_TrialCosDiff_Thr(int thr_no)  { LeabraNetState()->Compute_TrialCosDiff_Thr(thr_no); }
    // #IGNORE
    inline float Compute_TrialCosDiff_Agg()  { return LeabraNetState()->Compute_TrialCosDiff_Agg(); }
    // #IGNORE
  virtual void	Compute_ActMargin();
  // #CAT_Statistic compute cosine (normalized dot product) trial activation difference between act_q0 and act_q4 unit values -- must be called after Quarter_Final for plus phase to get the act_q4 values
    inline void Compute_ActMargin_Thr(int thr_no)  { LeabraNetState()->Compute_ActMargin_Thr(thr_no); }
    // #IGNORE
    inline void Compute_ActMargin_Agg()  { LeabraNetState()->Compute_ActMargin_Agg(); }
    // #IGNORE
    inline void Compute_RTCycles_Agg()  { LeabraNetState()->Compute_RTCycles_Agg(); }
    // #IGNORE
  virtual float	Compute_NetSd();
  // #CAT_Statistic compute standard deviation of the minus phase net inputs across the layers -- this is a key statistic to monitor over time for how much the units are gaining traction on the problem -- they should be getting more differentiated and sd should go up -- if not, then the network will likely fail -- MUST call this at end of minus phase!
    inline void Compute_NetSd_Thr(int thr_no)  { LeabraNetState()->Compute_NetSd_Thr(thr_no); }
    // #IGNORE
    inline float Compute_NetSd_Agg()  { return LeabraNetState()->Compute_NetSd_Agg(); }
    // #IGNORE
  virtual void	Compute_HogDeadPcts();
  // #CAT_Statistic compute percentage of units in the network that have a long-time-averaged activitation level that is above or below hog / dead thresholds, indicating that they are either 'hogging' the representational space, or 'dead' and not participating in any representations
    inline void Compute_HogDeadPcts_Thr(int thr_no)  { LeabraNetState()->Compute_HogDeadPcts_Thr(thr_no); }
    // #IGNORE
    inline void Compute_HogDeadPcts_Agg()  { LeabraNetState()->Compute_HogDeadPcts_Agg(); }
    // #IGNORE
  void	Compute_TrialStats() override;
  // #CAT_Statistic #OBSOLETE do not call this function anymore -- it is obsolete -- please use Compute_PhaseStats or Compute_MinusStats / Compute_PlusStats for more appropriate stats computation at the right time

  virtual void  Compute_PhaseStats();
  // #CAT_Statistic compute MinusStats at the end of the minus phase, and PlusStats at the end of the plus phase -- this is preferred over the previous implementation of calling TrialStats only at the end of the minus phase, which required targets to be present in the minus phase, which is not always the case
  virtual void  Compute_MinusStats();
  // #CAT_Statistic compute the stats that should be computed at the end of the minus phase: minus_output_name -- typically call this using Compute_PhaseStats which does the appropriate call given the current network phase
  virtual void  Compute_PlusStats();
  // #CAT_Statistic compute the stats that should be computed at the end of the plus phase: all the error stats: SSE, PRerr, NormErr, CosErr, ExtRew -- typically call this using Compute_PhaseStats which does the appropriate call given the current network phase
    inline void Compute_PlusStats_Thr(int thr_no)  { LeabraNetState()->Compute_PlusStats_Thr(thr_no); }
    // #IGNORE
    inline void Compute_PlusStats_Agg()  { LeabraNetState()->Compute_PlusStats_Agg(); }
    // #IGNORE

  virtual void	Compute_AbsRelNetin();
  // #CAT_Statistic compute the absolute layer-level and relative netinput from different projections into layers in network -- this should NOT be called from programs (although previously it was) -- it is automatically called in Trial_Final now, and projection-level netin data is subjected to settings of rel_netin if NETIN_PER_PRJN flag is not set
  virtual void	Compute_AvgAbsRelNetin();
  // #CAT_Statistic compute time-average absolute layer-level and relative netinput from different projections into layers in network (e.g. over epoch timescale)

  inline void	Compute_AvgCycles()  { LeabraNetState()->Compute_AvgCycles(); }
  // #CAT_Statistic compute average cycles (at an epoch-level timescale)
  inline void	Compute_AvgExtRew()  { LeabraNetState()->Compute_AvgExtRew(); }
  // #CAT_Statistic compute average external reward information (at an epoch-level timescale)
  inline void	Compute_AvgNormErr()  { LeabraNetState()->Compute_AvgNormErr(); }
  // #CAT_Statistic compute average norm_err (at an epoch-level timescale)
  inline void	Compute_AvgCosErr()  { LeabraNetState()->Compute_AvgCosErr(); }
  // #CAT_Statistic compute average cos_err (at an epoch-level timescale)
  inline void	Compute_AvgSendPct()  { LeabraNetState()->Compute_AvgSendPct(); }
  // #CAT_Statistic compute average sending pct (at an epoch-level timescale)
  inline void	Compute_AvgCosDiff()  { LeabraNetState()->Compute_AvgCosDiff(); }
  // #CAT_Statistic compute average cos_diff (at an epoch-level timescale)
  inline void	Compute_AvgTrialCosDiff()  { LeabraNetState()->Compute_AvgTrialCosDiff(); }
  // #CAT_Statistic compute average trial_cos_diff (at an epoch-level timescale)
  inline void	Compute_AvgAvgActDiff()  { LeabraNetState()->Compute_AvgAvgActDiff(); }
  // #CAT_Statistic compute average avg_act_diff (at an epoch-level timescale)
  inline void	Compute_AvgNetSd()  { LeabraNetState()->Compute_AvgNetSd(); }
  // #CAT_Statistic compute average net_sd (at an epoch-level timescale)
  void	Compute_EpochStats() override;
  // #CAT_Statistic compute epoch-level statistics, including SSE, AvgExtRew and AvgCycles

  virtual void  Compute_EpochWeights();
  // #CAT_Learning perform any epoch-level weight updates or adjustments..
  inline void  Compute_EpochWeights_Thr(int thr_no) { LeabraNetState()-> Compute_EpochWeights_Thr(thr_no); }
  // #IGNORE

  
  NetworkState_cpp* NewNetworkState() const override;
  TypeDef* NetworkStateType() const override;
  TypeDef* LayerStateType() const override;
  TypeDef* PrjnStateType() const override;
  TypeDef* UnGpStateType() const override;
  TypeDef* UnitStateType() const override;
  TypeDef* ConStateType() const override;


  void	SetProjectionDefaultTypes(Projection* prjn) override;

  virtual String   TimingReport(DataTable& dt, bool print = true);
  // #CAT_Statistic report detailed timing data to data table, and print a summary -- only collected if thread.get_timing engaged (e.g., call threads.get_timing)

  String       GetToolbarName() const override { return "network"; }

  TA_SIMPLE_BASEFUNS(LeabraNetwork);
protected:
  void 	UpdateAfterEdit_impl() override;
private:
  void	Initialize();
  void 	Destroy()		{}
};

#endif // LeabraNetwork_h
