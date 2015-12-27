// Copyright, 1995-2013, Regents of the University of Colorado,
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

#ifndef PFCUnitSpec_h
#define PFCUnitSpec_h 1

// parent includes:
#include <LeabraUnitSpec>

// member includes:
#include <DataTable>

// declare all other types mentioned but not required to include:

eTypeDef_Of(PFCGateSpec);

class E_API PFCGateSpec : public SpecMemberBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS ##CAT_Leabra specifications for PFC gating functions
INHERITED(SpecMemberBase)
public:
  bool          out_gate;       // if true, this PFC layer is an output gate layer, which means that it only has transient activation during gating
  float         mnt_thal;       // #DEF_1 effective thal activation to use in computing the deep_raw activation sent from super to deep layers, for continued maintenance beyond the initial thal signal provided by the BG -- also sets and effective minimum thal value regardless of the actual gating thal value
  float         gate_thr;       // #DEF_0.2 threshold on thalamic gating signal to drive gating -- when using GpiInvUnitSpec gpi, this parameter ususally doesn't matter!  set the gpi.gate_thr value instead -- the only constraint is that this value be <= gpi.min_thal as that determines the minimum thalamic value for gated stripes
  bool          prv_qtr;        // does gating happen in the previous quarter or otherwise in the same quarter as the deep_raw_qtr?  for maintenance layers, typically you set Q2_Q4 for deep_raw_qtr, and gating happens in quarters 1 and 3, so prv_qtr should be true -- for output layers, gating in Q1 and prv_qtr = false allows gating to happen after first quarter instead of waiting until later -- this allows the output gating to happen quicker to influence responses within the trial
  
  String        GetTypeDecoKey() const override { return "UnitSpec"; }

  TA_SIMPLE_BASEFUNS(PFCGateSpec);
protected:
  SPEC_DEFAULTS;
  //  void  UpdateAfterEdit_impl();
private:
  void	Initialize();
  void	Destroy()	{ };
  void	Defaults_init();
};

eTypeDef_Of(PFCMaintSpec);

class E_API PFCMaintSpec : public SpecMemberBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS ##CAT_Leabra specifications for PFC maintenance functions
INHERITED(SpecMemberBase)
public:
  float         s_mnt_min;      // #DEF_0.3 for superficial neurons, how much of deep_lrn to add into excitatory net input to support maintenance, from deep maintenance signal -- 0.25 is generally minimum to support maintenance -- this is the minimum amount to add as a function of the strength of afferent netinput (less maint current with stronger afferents) -- see
  float         s_mnt_max;      // #DEF_0.5 for superficial neurons, how much of deep_lrn to add into excitatory net input to support maintenance, from deep maintenance signal -- 0.25 is generally minimum to support maintenance -- this is the maximum amount to add as a function of the strength of afferent netinput (less maint current with stronger afferents)
  float         mnt_net_max;    // #DEF_0.5 maximum netin_raw.max value required to drive the minimum s_mnt_min maintenance current from deep -- anything above this drives the same s_mnt_min value -- below this value scales the effective mnt current between s_mnt_min to s_mnt_max in reverse proportion to netin_raw.max value
  float         clear;          // #MIN_0 #MAX_1 #DEF_0.5 how much to clear out (decay) super activations when the stripe itself gates and was previously maintaining something, or for maint pfc stripes, when output go fires and clears
  bool          use_dyn;        // use fixed dynamics for updating deep_ctxt activations -- defined in dyn_table -- this also preserves the initial gating deep_ctxt value in misc_1 -- otherwise it is up to the recurrent loops between super and deep for maintenance
  int           max_mnt;        // #MIN_1 #DEF_1;100 maximum duration of maintenance for any stripe -- beyond this limit, the maintenance is just automatically cleared -- typically 1 for output gating and 100 for maintenance gating
  
  String        GetTypeDecoKey() const override { return "UnitSpec"; }

  TA_SIMPLE_BASEFUNS(PFCMaintSpec);
protected:
  SPEC_DEFAULTS;
  //  void  UpdateAfterEdit_impl();
private:
  void	Initialize();
  void	Destroy()	{ };
  void	Defaults_init();
};

eTypeDef_Of(PFCUnitSpec);

class E_API PFCUnitSpec : public LeabraUnitSpec {
  // PFC unit spec -- thal signal during deep_raw_qtr drives updating of deep_raw as gated deep5b activations, sent as deep_ctxt to DEEP units that send back deep_lrn to SUPER, and drive top-down outputs -- output gating is transient
INHERITED(LeabraUnitSpec)
public:

  enum DynVals {                // the different values stored in dyn_table -- for rapid access
    DYN_NAME,
    DYN_DESC,
    DYN_INIT,                   // initial value at point when gating starts
    DYN_RISE_TAU,               // time constant for linear rise in maintenance activation (per quarter when deep is updated) -- use integers -- if both rise and decay then rise comes first
    DYN_DECAY_TAU,              // time constant for linear decay in maintenance activation (per quarter when deep is updated) -- use integers -- if both rise and decay then rise comes first
  };

  PFCGateSpec   gate;           // PFC specifications for gating-related parameters
  PFCMaintSpec  maint;          // PFC specifications for maintenance-related parameters
  int           n_dyns;         // number of different temporal dynamic profiles for different PFC units, all triggered by a single gating event -- each row of units within a PFC unit group shares the same dynamics -- there should be an even multiple of n_dyns rows (y unit group size) per unit group
  DataTable     dyn_table;      // #SHOW_TREE #EXPERT #HIDDEN_CHOOSER table of dynamics parameters for response of deep_raw over time after gating has taken place -- update occurs once each quarter that deep_ctxt is computed -- one set of params per each row, n_dyns rows total (see n_dyns)


  static int    PFCGatingCycle(LeabraNetwork* net, bool pfc_out_gate, int& qtr_cyc);
  // get the cycle on which the PFC will gate -- pfc_out gates 1 cycle earlier -- also fills in the current cycle within quarter in qtr_cyc -- compare against return value to determine if it is gating time..
  
  inline float  GetDynVal(DynVals val, int row) {
    return dyn_table.GetValAsFloat(val, row);
  }
  // get specific dyn value for given row
  inline void   SetDynVal(const Variant& vl, DynVals val, int row) {
    dyn_table.SetVal(vl, val, row);
  }
  // set specific dyn value for given row

  virtual float UpdtDynVal(int row, float time_step);
  // get update dynamic value as function of current value and previous value
  
  virtual void  FormatDynTable();
  // #IGNORE format the dyn table
  virtual void  DefaultDynTable(float std_tau = 10);
  // #BUTTON default initial dynamics table -- specifies flat maint for output gating and all combinations of up / down dynamics for maint gating -- the std_tau parameter defines the number of update steps over which the dynamics occur (e.g., rises in std_tau time steps)
  virtual void  UpdtDynTable();
  // check and make sure table is all OK

  virtual void GetThalCntFromSuper(LeabraUnitVars* u, LeabraNetwork* net, int thr_no);
  // #IGNORE deep guys get thal_cnt from super
  virtual void Compute_PFCGating(LeabraUnitVars* u, LeabraNetwork* net, int thr_no);
  // called 1/2 way through phase prior to official gating phase, determines if gating happened and sets thal_cnt to 0 if so, and clears existing activations per params -- output gating layers call this 1 cycle prior to maint layers, to allow maint to override clear with new gating if present
  virtual void ClearOtherMaint(LeabraUnitVars* u, LeabraNetwork* net, int thr_no);
  // clear maintenance in other layers we project to using MarkerConSpec

  float Compute_NetinExtras(LeabraUnitVars* uv, LeabraNetwork* net,
                            int thr_no, float& net_syn) override;
  void Compute_DeepRaw(LeabraUnitVars* uv, LeabraNetwork* net, int thr_no) override;
  void Quarter_Init_Deep(LeabraUnitVars* uv, LeabraNetwork* net, int thr_no) override;
  void Compute_Act_Post(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) override;
  void Send_DeepCtxtNetin(LeabraUnitVars* uv, LeabraNetwork* net, int thr_no) override;
  void Compute_DeepStateUpdt(LeabraUnitVars* uv, LeabraNetwork* net, int thr_no) override;

  virtual void	GraphPFCDyns(DataTable* graph_data, int n_trials=20);
  // #MENU_BUTTON #MENU_ON_Graph #NULL_OK #NULL_TEXT_NewGraphData graph the pfc dynamics for response of deep_raw over time after gating has taken place -- update occurs once each quarter that deep_raw is computed (typically once per trial)
  
  TA_SIMPLE_BASEFUNS(PFCUnitSpec);
protected:
  SPEC_DEFAULTS;
  void  UpdateAfterEdit_impl();
  
private:
  void	Initialize();
  void	Destroy()		{ };
  void	Defaults_init();
};

#endif // PFCUnitSpec_h
