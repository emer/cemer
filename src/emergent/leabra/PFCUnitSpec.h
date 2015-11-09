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

eTypeDef_Of(PFCMiscSpec);

class E_API PFCMiscSpec : public SpecMemberBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS ##CAT_Leabra misc specifications for PFC function
INHERITED(SpecMemberBase)
public:
  bool          out_gate;       // if true, this PFC layer is an output gate layer, which means that it only has transient activation during gating
  int           out_mnt;        // #CONDSHOW_ON_out_gate #DEF_1:2 number of effective trials (updates of deep state, following deep_qtr updates) to maintain output gating signals
  int           max_mnt;        // #CONDSHOW_OFF_out_gate maximum duration of maintenance for any stripe -- beyond this limit, the maintenance is just automatically cleared
  float         s_mnt_gain;     // #DEF_0.05;0.1 for superficial neurons, how much of deep_lrn to add into excitatory net input to support maintenance, from deep maintenance signal
  float         clear_decay;    // how much to clear out maintenance activations when an output go signal fires and clears maintenance
  float         mnt_thal;       // #DEF_1 effective thal activation to use for continued maintenance beyond the initial thal signal provided by the BG -- also sets and effective minimum thal value regardless of the actual gating thal value
  bool          use_dyn;        // use fixed dynamics for updating deep_ctxt activations -- defined in dyn_table -- this also preserves the initial gating deep_ctxt value in misc_1 -- otherwise it is up to the recurrent loops between super and deep for maintenance
  float         gate_thr;       // #DEF_0.1 threshold on thalamic gating signal to drive gating -- when using GpiInvUnitSpec gpi, this parameter ususally doesn't matter!  set the gpi.gate_thr value instead -- the only constraint is that this value be <= gpi.min_thal as that determines the minimum thalamic value for gated stripes
  
  String        GetTypeDecoKey() const override { return "UnitSpec"; }

  TA_SIMPLE_BASEFUNS(PFCMiscSpec);
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

  PFCMiscSpec   pfc;            // misc PFC specifications
  int           n_dyns;         // number of different temporal dynamic profiles for different PFC units, all triggered by a single gating event -- each row of units within a PFC unit group shares the same dynamics -- there should be an even multiple of n_dyns rows (y unit group size) per unit group
  DataTable     dyn_table;      // #SHOW_TREE #EXPERT #HIDDEN_CHOOSER table of dynamics parameters for response of deep_raw over time after gating has taken place -- update occurs once each quarter that deep_ctxt is computed -- one set of params per each row, n_dyns rows total (see n_dyns)

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
  virtual void  InitDynTable();
  // default initial dynamics table
  virtual void  UpdtDynTable();
  // #BUTTON update the dt values from the tau values

  float Compute_NetinExtras(LeabraUnitVars* uv, LeabraNetwork* net,
                            int thr_no, float& net_syn) override;

  void Compute_DeepRaw(LeabraUnitVars* uv, LeabraNetwork* net, int thr_no) override;
  void Quarter_Init_Deep(LeabraUnitVars* uv, LeabraNetwork* net, int thr_no) override;
  virtual void GetThalCntFromSuper(LeabraUnitVars* u, LeabraNetwork* net, int thr_no);
  // #IGNORE deep guys get thal_cnt from super
  void Send_DeepCtxtNetin(LeabraUnitVars* uv, LeabraNetwork* net, int thr_no) override;
  void Compute_DeepStateUpdt(LeabraUnitVars* uv, LeabraNetwork* net, int thr_no) override;

  virtual void ClearOtherMaint(LeabraUnitVars* u, LeabraNetwork* net, int thr_no);
  // clear maintenance in other layers we project to using MarkerConSpec
  
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
