// Copyright 2013-2017, Regents of the University of Colorado,
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

// member includes:
#include <taMath_float>
#include <Average>
#include <LeabraLayer>
#include <LeabraPrjn>
#include <int_Array>

// declare all other types mentioned but not required to include:
class DataTable; // 

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

  void	Build() override;
  void  CheckSpecs() override;
  virtual void CheckInhibCons();
  // #IGNORE
  virtual void BuildLeabraThreadMem();
  // #IGNORE

  ///////////////////////////////////////////////////////////////////////
  //	General Init functions

  void  SetCycle(int cyc) { cycle = cyc; ((LeabraNetworkState_cpp*)net_state)->cycle = cyc;  }
  // #CAT_Counter set the cycle number -- this must be coordinated between State and the main object, and State normally updates this value, so both must be set together
  void  SetQuarter(int qtr) { quarter = qtr; ((LeabraNetworkState_cpp*)net_state)->quarter = qtr;  }
  // #CAT_Counter set the quarter number -- this must be coordinated between State and the main object, and State normally updates this value, so both must be set together
  
  void  SyncLayerState_Layer(Layer* lay) override;
  void	Init_Acts() override;

  inline void  Init_AdaptInhib() { LeabraNetState()->Init_AdaptInhib(); }
  // #CAT_Activation Initialize adaptive inhibition gain value on all the layers -- undoes any adaptation that has taken place (including from loaded weights - the adaptive gain value is saved with the weights)

  virtual void  Init_Netins();
  // #CAT_Activation initialize netinput computation variables (delta-based requires several intermediate variables)
  virtual void	DecayState(float decay);
  // #CAT_Activation decay activation states towards initial values by given amount (0 = no decay, 1 = full decay)
  virtual void ResetSynTR();
  // #CAT_Activation Reset Synaptic transmitter availability to full (1.0)

  ///////////////////////////////////////////////////////////////////////
  //	TrialInit -- at start of trial

  virtual void 	Trial_Init();
  // #CAT_TrialInit initialize at start of trial (init specs, Decay state)
    inline void Trial_Init_Counters() { LeabraNetState()->Trial_Init_Counters(); }
    // #CAT_TrialInit init counters -- network only
    virtual void SetCurLrate();
    // #CAT_TrialInit set current learning rate based on network epoch and schedule

  ///////////////////////////////////////////////////////////////////////
  //	QuarterInit -- at start of a given quarter trial of processing

  virtual void  Quarter_Init();
  // #CAT_QuarterInit initialize network for quarter-level processing (hard clamp, netscale)
    inline void Quarter_Init_Counters() { LeabraNetState()->Quarter_Init_Counters(); }
    // #CAT_QuarterInit initialize counters for upcoming quarter -- network only
      virtual void Quarter_Init_TargFlags();
      // #CAT_QuarterInit initialize at start of settling phase -- sets target external input flags based on phase -- not called by default -- direct to unit level function
    virtual void Quarter_Init_Deep();
    // #CAT_QuarterInit quarter deep leabra init: deep_ctxt compute and deep state update

    virtual void Compute_HardClamp();
    // #CAT_QuarterInit compute hard clamping from external inputs

    virtual void ExtToComp();
    // #CAT_QuarterInit move external input values to comparison values (not currently used)
    virtual void TargExtToComp();
    // #CAT_QuarterInit move target and external input values to comparison

  virtual void  NewInputData_Init();
  // #CAT_QuarterInit perform initialization stuff needed to update external input data signals so they actually show up as activations in the network: Quarter_Init_Layer, Quarter_Init_TrgFlags, Compute_HardClamp

  ////////////////////////////////////////////////////////////////
  //	Cycle_Run
  
  inline  int   CycleRunMax()
  { if(times.cycle_qtr) return 1; return times.quarter; }
  // #CAT_Quarter max loop counter for running cycles in a gamma quarter of processing, taking into account the fact that threading can run multiple cycles per Cycle_Run call if quarter flag is set

  virtual void	Cycle_Run();
  // #CAT_Cycle compute cycle(s) of updating: netinput, inhibition, activations -- multiple cycles can be run depending on lthreads.n_cycles setting and whether multiple threads are actually being used -- see lthreads.n_threads_act

  inline void  Cycle_IncrCounters()  { LeabraNetState()->Cycle_IncrCounters(); }
  // #CAT_Cycle increment the cycle-level counters -- called internally during Cycle_Run()

  ///////////////////////////////////////////////////////
  //	Cycle Stage 1: netinput


  ///////////////////////////////////////////////////////////////////////
  //	Cycle Step 2: Inhibition

  inline void	Compute_Inhib()  { LeabraNetState()->Compute_Inhib(); }
  // #IGNORE compute inhibitory conductances via inhib functions (FFFB) -- calls Compute_NetinStats and LayInhibToGps to coordinate group-level inhibition sharing

  ///////////////////////////////////////////////////////////////////////
  //	Cycle Step 3: Activation

  inline void  ThalGatedNow()  { LeabraNetState()-> ThalGatedNow(); }
  // #IGNORE record current cycle as gating cycle -- called by enabled thalamic gating layer(s)

  ///////////////////////////////////////////////////////////////////////
  //	Cycle Stats

  inline void	Compute_CycleStats_Pre()  { LeabraNetState()->Compute_CycleStats_Pre(); }
  // #CAT_Cycle compute cycle-level stats -- acts AvgMax, OutputName, etc -- network-level pre-step -- happens after Compute_Act and prior to Compute_Act_Post -- good place to insert any layer-level modification of unit-level activations
    virtual void  Compute_OutputName();
    // #CAT_Statistic compute the output name for layers and the network, based on the unit names of the most-active units in each target layer -- only works if you provide names to the units -- called automatically in Compute_CycleStats_Post()
    inline void  Compute_RTCycles()  { LeabraNetState()-> Compute_RTCycles(); }
    // #CAT_Statistic compute the rt_cycles statistic based on trg_max_act and trg_max_act_crit criterion, only in the minus phase -- this is a good measure for computing the reaction time (RT) of the network, as in a psychological experiment -- called automatically in Compute_CycleStats_Post()

  ///////////////////////////////////////////////////////////////////////
  //	DeepLeabra deep_raw Updating -- called after superficial layer updating

  virtual void ClearDeepActs();
  // #CAT_Deep clear all the deep lamina variables -- can be useful to do at discontinuities of experience

  virtual void ClearMSNTrace();
  // #CAT_PBWM clear the synaptic trace for MSN connections (Medium Spiny Neurons in the Striatum)

  ///////////////////////////////////////////////////////////////////////
  //	Quarter Final

  virtual void	 Quarter_Final();
  // #CAT_QuarterFinal do final processing after each quarter: 
    virtual void Quarter_Compute_dWt();
    // #CAT_QuarterFinal compute weight changes at end of each quarter -- units decide when this actually happens


  ///////////////////////////////////////////////////////////////////////
  //	Trial Update and Final

  virtual void	Trial_Final();
  // #CAT_TrialFinal do final processing after trial: Compute_AbsRelNetin

  virtual void	Compute_AbsRelNetin();
  // #CAT_Statistic compute the absolute layer-level and relative netinput from different projections into layers in network -- this should NOT be called from programs (although previously it was) -- it is automatically called in Trial_Final now, and projection-level netin data is subjected to settings of rel_netin if NETIN_PER_PRJN flag is not set
  virtual void	Compute_AvgAbsRelNetin();
  // #CAT_Statistic compute time-average absolute layer-level and relative netinput from different projections into layers in network (e.g. over epoch timescale)

  ///////////////////////////////////////////////////////////////////////
  //	Learning

  ///////////////////////////////////////////////////////////////////////
  //	Stats

  virtual void  LayerAvgAct(DataTable* report_table = NULL,
                            LeabraLayerSpec* lay_spec = NULL);
  // #MENU #MENU_ON_State #ARGC_1 #NULL_OK #NULL_TEXT_NewReportData create a data table with the current layer average activations (acts_m_avg) and the values specified in the layerspec avg_act.init -- this is useful for setting the .init values accurately based on actual levels 

  virtual void	Set_ExtRew(bool avail, float ext_rew_val);
  // #CAT_Statistic set ext_rew_avail and ext_rew value -- for script access to these values
  virtual void	Compute_ExtRew();
  // #CAT_Statistic compute external reward information: called in plus phase stats
  virtual void	Compute_NormErr();
  // #CAT_Statistic compute normalized binary error between act_m and targ unit values: called in TrialStats -- per unit: if (net->lstats.on_errs && act_m > .5 && targ < .5) return 1; if (net->lstats.off_errs && act_m < .5 && targ > .5) return 1; else return 0; normalization is per layer based on k value: total possible err for both on and off errs is 2 * k (on or off alone is just k)
  virtual void	Compute_CosErr();
  // #CAT_Statistic compute cosine (normalized dot product) error between act_m and targ unit values
  virtual void	Compute_CosDiff();
  // #CAT_Statistic compute cosine (normalized dot product) phase difference between act_m and act_p unit values -- must be called after PostQuarter (QuarterFinal) for plus phase to get the act_p values
  virtual void	Compute_AvgActDiff();
  // #CAT_Statistic compute average act_diff (act_p - act_m) -- must be called after PostQuarter (QuarterFinal) for plus phase to get the act_p values -- this is an important statistic to track overall 'main effect' differences across phases 
  virtual void	Compute_TrialCosDiff();
  // #CAT_Statistic compute cosine (normalized dot product) trial activation difference between act_q0 and act_q4 unit values -- must be called after Quarter_Final for plus phase to get the act_q4 values
  virtual void	Compute_ActMargin();
  // #CAT_Statistic compute cosine (normalized dot product) trial activation difference between act_q0 and act_q4 unit values -- must be called after Quarter_Final for plus phase to get the act_q4 values
  virtual void	Compute_NetSd();
  // #CAT_Statistic compute standard deviation of the minus phase net inputs across the layers -- this is a key statistic to monitor over time for how much the units are gaining traction on the problem -- they should be getting more differentiated and sd should go up -- if not, then the network will likely fail -- MUST call this at end of minus phase!
  virtual void	Compute_HogDeadPcts();
  // #CAT_Statistic compute percentage of units in the network that have a long-time-averaged activitation level that is above or below hog / dead thresholds, indicating that they are either 'hogging' the representational space, or 'dead' and not participating in any representations
  void	Compute_TrialStats() override;
  // #CAT_Statistic #OBSOLETE do not call this function anymore -- it is obsolete -- please use Compute_PhaseStats or Compute_MinusStats / Compute_PlusStats for more appropriate stats computation at the right time

  virtual void  Compute_PhaseStats();
  // #CAT_Statistic compute MinusStats at the end of the minus phase, and PlusStats at the end of the plus phase -- this is preferred over the previous implementation of calling TrialStats only at the end of the minus phase, which required targets to be present in the minus phase, which is not always the case
  virtual void  Compute_MinusStats();
  // #CAT_Statistic compute the stats that should be computed at the end of the minus phase: minus_output_name -- typically call this using Compute_PhaseStats which does the appropriate call given the current network phase
  virtual void  Compute_PlusStats();
  // #CAT_Statistic compute the stats that should be computed at the end of the plus phase: all the error stats: SSE, PRerr, NormErr, CosErr, ExtRew -- typically call this using Compute_PhaseStats which does the appropriate call given the current network phase

  virtual void  Compute_EpochWeights();
  // #CAT_Learning perform any epoch-level weight updates or adjustments..


  NetworkState_cpp* NewNetworkState() const override;
  TypeDef* NetworkStateType() const override;
  TypeDef* LayerStateType() const override;
  TypeDef* PrjnStateType() const override;
  TypeDef* UnGpStateType() const override;
  TypeDef* UnitStateType() const override;
  TypeDef* ConStateType() const override;

  void TimingReportInitNames() override;

  void	SetProjectionDefaultTypes(Projection* prjn) override;

  String       GetToolbarName() const override { return "network"; }

  TA_SIMPLE_BASEFUNS(LeabraNetwork);
protected:
  void 	UpdateAfterEdit_impl() override;
private:
  void	Initialize();
  void 	Destroy()		{}
};

#endif // LeabraNetwork_h
