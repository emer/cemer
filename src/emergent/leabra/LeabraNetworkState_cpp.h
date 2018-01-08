// Copyright 2017-22018 Regents of the University of Colorado,
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

#ifndef LeabraNetworkState_cpp_h
#define LeabraNetworkState_cpp_h 1

// raw C++ (cpp) version of state -- no emergent / ta dependencies

// parent includes:
#include <NetworkState_cpp> // replace with actual parent

#include <State_cpp>

#include <LeabraAvgMax_cpp>
#include <LeabraNetwork_mbrs>

#include <State_cpp>

// member includes:

// declare all other types mentioned but not required to include:

class STATE_CLASS(LeabraLayerState); //
class STATE_CLASS(LeabraPrjnState); //
class STATE_CLASS(LeabraUnGpState); //
class STATE_CLASS(LeabraUnitState); //
class STATE_CLASS(LeabraConState); // 
class STATE_CLASS(LeabraLayerSpec); //
class STATE_CLASS(LeabraUnitSpec); //
class STATE_CLASS(LeabraConSpec); //


class E_API LeabraNetworkState_cpp : public NetworkState_cpp {
  // #STEM_BASE ##CAT_Leabra network that uses the Leabra algorithms and objects
INHERITED(NetworkState)
public:

#include <LeabraNetwork_core>
#include <LeabraNetworkState_core>

  void  Init_Acts() override;
  virtual void  Init_Netins();
  // #CAT_Activation initialize netinput computation variables (delta-based requires several intermediate variables)
  virtual void	DecayState(float decay);
  // #CAT_Activation decay activation states towards initial values by given amount (0 = no decay, 1 = full decay)
  virtual void ResetSynTR();
  // #CAT_Activation Reset Synaptic transmitter availability to full (1.0)
  virtual void 	Trial_Init();
  // #CAT_TrialInit initialize at start of trial (init specs, Decay state)
    virtual void Trial_Init_Unit();
    // #CAT_TrialInit trial init units
  virtual void  Quarter_Init();
  // #CAT_QuarterInit initialize network for quarter-level processing (hard clamp, netscale)
    virtual void Quarter_Init_TargFlags();
    // #CAT_QuarterInit initialize at start of settling phase -- sets target external input flags based on phase -- not called by default -- direct to unit level function
    virtual void Quarter_Init_Deep();
    // #CAT_QuarterInit quarter deep leabra init: deep_ctxt compute and deep state update
  virtual void Compute_HardClamp();
  // #CAT_QuarterInit compute hard clamping from external inputs -- not called by default -- done directly in unit-level function
  virtual void ExtToComp();
  // #CAT_QuarterInit move external input values to comparison values (not currently used)
  virtual void TargExtToComp();
  // #CAT_QuarterInit move target and external input values to comparison
  virtual void  NewInputData_Init();
  // #CAT_QuarterInit perform initialization stuff needed to update external input data signals so they actually show up as activations in the network: Quarter_Init_Layer, Quarter_Init_TrgFlags, Compute_HardClamp
  virtual void	Cycle_Run();
  // #CAT_Cycle compute cycle(s) of updating: netinput, inhibition, activations -- multiple cycles can be run depending on lthreads.n_cycles setting and whether multiple threads are actually being used -- see lthreads.n_threads_act
  virtual void ClearDeepActs();
  // #CAT_Deep clear all the deep lamina variables -- can be useful to do at discontinuities of experience
  virtual void ClearMSNTrace();
  // #CAT_PBWM clear the synaptic trace for MSN connections (Medium Spiny Neurons in the Striatum)
  virtual void Quarter_Final();
  // #CAT_QuarterFinal do final processing after each quarter: 
    virtual void Quarter_Compute_dWt();
    // #CAT_QuarterFinal compute weight changes at end of each quarter -- units decide when this actually happens
  virtual void	Trial_Final();
  // #CAT_TrialFinal do final processing after trial: Compute_AbsRelNetin
  void Compute_dWt() override;
  void Compute_Weights() override;
  virtual void Compute_NormErr();
  // #CAT_Statistic compute normalized binary error between act_m and targ unit values: called in TrialStats -- per unit: if (net->lstats.on_errs && act_m > .5 && targ < .5) return 1; if (net->lstats.off_errs && act_m < .5 && targ > .5) return 1; else return 0; normalization is per layer based on k value: total possible err for both on and off errs is 2 * k (on or off alone is just k)
  virtual float	Compute_CosErr();
  // #CAT_Statistic compute cosine (normalized dot product) error between act_m and targ unit values
  virtual float	Compute_CosDiff();
  // #CAT_Statistic compute cosine (normalized dot product) phase difference between act_m and act_p unit values -- must be called after PostQuarter (QuarterFinal) for plus phase to get the act_p values
  virtual float	Compute_AvgActDiff();
  // #CAT_Statistic compute average act_diff (act_p - act_m) -- must be called after PostQuarter (QuarterFinal) for plus phase to get the act_p values -- this is an important statistic to track overall 'main effect' differences across phases 
  virtual float	Compute_TrialCosDiff();
  // #CAT_Statistic compute cosine (normalized dot product) trial activation difference between act_q0 and act_q4 unit values -- must be called after Quarter_Final for plus phase to get the act_q4 values
  virtual void	Compute_ActMargin();
  // #CAT_Statistic compute cosine (normalized dot product) trial activation difference between act_q0 and act_q4 unit values -- must be called after Quarter_Final for plus phase to get the act_q4 values
  virtual float	Compute_NetSd();
  // #CAT_Statistic compute standard deviation of the minus phase net inputs across the layers -- this is a key statistic to monitor over time for how much the units are gaining traction on the problem -- they should be getting more differentiated and sd should go up -- if not, then the network will likely fail -- MUST call this at end of minus phase!
  virtual void	Compute_HogDeadPcts();
  // #CAT_Statistic compute percentage of units in the network that have a long-time-averaged activitation level that is above or below hog / dead thresholds, indicating that they are either 'hogging' the representational space, or 'dead' and not participating in any representations
  virtual void  Compute_PhaseStats();
  // #CAT_Statistic compute MinusStats at the end of the minus phase, and PlusStats at the end of the plus phase -- this is preferred over the previous implementation of calling TrialStats only at the end of the minus phase, which required targets to be present in the minus phase, which is not always the case
  virtual void  Compute_MinusStats();
  // #CAT_Statistic compute the stats that should be computed at the end of the minus phase: minus_output_name -- typically call this using Compute_PhaseStats which does the appropriate call given the current network phase
  virtual void  Compute_PlusStats();
  // #CAT_Statistic compute the stats that should be computed at the end of the plus phase: all the error stats: SSE, PRerr, NormErr, CosErr, ExtRew -- typically call this using Compute_PhaseStats which does the appropriate call given the current network phase

  void	Compute_EpochStats() override;
  // #CAT_Statistic compute epoch-level statistics, including SSE, AvgExtRew and AvgCycles



  virtual void BuildLeabraThreadMem();
  // #IGNORE
  
  void  LayerSaveWeights_LayerVars(std::ostream& strm, LayerState_cpp* lay, WtSaveFormat fmt = TEXT) override;

  int   LayerLoadWeights_LayerVars(std::istream& strm, LayerState_cpp* lay,
                                   WtSaveFormat fmt = TEXT, bool quiet = false) override;

  LeabraNetworkState_cpp() { Initialize_core(); }
};

#endif // LeabraNetworkState_cpp_h
