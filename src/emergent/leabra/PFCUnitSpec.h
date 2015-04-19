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

eTypeDef_Of(PFCMaintSpec);

class E_API PFCMaintSpec : public SpecMemberBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS ##CAT_Leabra specifications for maintenance in PFC, based on deep_raw activations, which are gated by thalamic circuit
INHERITED(SpecMemberBase)
public:
  float        d5b_updt_tau;    // time constant for updating deep5b activations (at every phase or trial, depending on cifer.phase) where continuing maintenance is enabled -- set to a large number to preserve initial gating information, and to a low number to allow rapid updating / drift of representations based on current superficial layer activation

  float         d5b_updt_dt;    // #READ_ONLY #EXPERT rate = 1 / tau

  String        GetTypeDecoKey() const override { return "UnitSpec"; }

  TA_SIMPLE_BASEFUNS(PFCMaintSpec);
protected:
  SPEC_DEFAULTS;
  void  UpdateAfterEdit_impl();
private:
  void	Initialize();
  void	Destroy()	{ };
  void	Defaults_init();
};

eTypeDef_Of(PFCUnitSpec);

class E_API PFCUnitSpec : public LeabraUnitSpec {
  // PFC unit spec -- PFC layer is organized into unit groups arranged by rows and columns -- the columns are all treated the same (redundant channels for maintenance and learning), whereas rows alternate by maintenance duration (even = no maint, odd = maint) -- the prototypical organization has 4 rows, with the first set of two being input / maintenance and the second set being output / maintenance -- extra maintenance net input is added in proportion to deep5b activations, which in turn are thalamically gated -- automatically a localist, one-to-one form of maintenance -- also has special logic for updating deep5b activations, where repeated thalamic activation causes deep5b to update with the d5b_updt_tau time constant, capturing intrinsic maintenance properties of these neurons
INHERITED(LeabraUnitSpec)
public:

  enum DynVals {                // the different values stored in dyn_table -- for rapid access
    DYN_NAME,
    DYN_DESC,
    DYN_GAIN,
    DYN_INIT,
    DYN_RISE_TAU,
    DYN_RISE_DT,
    DYN_DECAY_TAU,
    DYN_DECAY_DT,
  };
  

  // PFCMaintSpec          pfc_maint; // specifications for maintenance in PFC, based on 
  // deep5b activations, which are in turn gated by thalamic circuit
  int           n_dyns;         // number of different temporal dynamic profiles for different PFC units, all triggered by a single gating event -- each row of units within a PFC unit group shares the same dynamics -- there should be an even multiple of n_dyns rows (y unit group size) per unit group
  DataTable     dyn_table;      // #SHOW_TREE #EXPERT #HIDDEN_CHOOSER table of dynamics parameters for response of deep_raw over time after gating has taken place -- update occurs once each quarter that deep_raw is computed -- one set of params per each row, n_dyns rows total (see n_dyns)

  inline float  GetDynVal(DynVals val, int row) {
    return dyn_table.GetValAsFloat(val, row);
  }
  // get specific dyn value for given row
  inline void   SetDynVal(const Variant& vl, DynVals val, int row) {
    dyn_table.SetVal(vl, val, row);
  }
  // set specific dyn value for given row
  
  virtual void  FormatDynTable();
  // #IGNORE format the dyn table
  virtual void  InitDynTable();
  // default initial dynamics table
  virtual void  UpdtDynTable();
  // update the dt values from the tau values
  // todo: graph dynamics

  float Compute_NetinExtras(LeabraUnitVars* uv, LeabraNetwork* net,
                            int thr_no, float& net_syn) override;
  void  Compute_Act_ThalDeep5b(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) override;

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
