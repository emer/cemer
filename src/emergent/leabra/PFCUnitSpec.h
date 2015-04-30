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
  float         gate_thr;       // #DEF_0.1 threshold on thalamic gating signal to drive gating
  bool          out_gate;       // if true, this PFC layer is an output gate layer, which means that it only has transient activation during gating
  int           out_mnt;        // #CONDSHOW_ON_out_gate #DEF_1:2 number of trials (updates of deep_norm state, following deep_qtr updates) to maintain output gating signals
  
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
  // PFC unit spec -- thal signal during deep updating quarter drives updating of deep_raw activations as maintenance activations -- these activations evolve over time across units according to the dyn_table settings -- each unit has stereotyped dynamic responses to the gating event -- activation at time of gating stored in misc_1 var -- deep_norm at end of deep updating is updated to reflect new maint currents, and this feeds back using deep.d_to_s to drive superficial maint activation as well (super acts don't feel gating update until next alpha or beta trial to prevent learning effects, but deep_raw updates in prior quarter) -- thal_cnt increments for continuing maintenance
INHERITED(LeabraUnitSpec)
public:

  enum DynVals {                // the different values stored in dyn_table -- for rapid access
    DYN_NAME,
    DYN_DESC,
    DYN_INIT,
    DYN_RISE_TAU,
    DYN_RISE_DT,
    DYN_DECAY_TAU,
    DYN_DECAY_DT,
  };

  PFCMiscSpec   pfc;            // misc PFC specifications
  int           n_dyns;         // #DEF_5 number of different temporal dynamic profiles for different PFC units, all triggered by a single gating event -- each row of units within a PFC unit group shares the same dynamics -- there should be an even multiple of n_dyns rows (y unit group size) per unit group
  DataTable     dyn_table;      // #SHOW_TREE #EXPERT #HIDDEN_CHOOSER table of dynamics parameters for response of deep_raw over time after gating has taken place -- update occurs once each quarter that deep_raw is computed -- one set of params per each row, n_dyns rows total (see n_dyns)

  inline float  GetDynVal(DynVals val, int row) {
    return dyn_table.GetValAsFloat(val, row);
  }
  // get specific dyn value for given row
  inline void   SetDynVal(const Variant& vl, DynVals val, int row) {
    dyn_table.SetVal(vl, val, row);
  }
  // set specific dyn value for given row

  inline float   UpdtDynVal(const float cur_val, const float prv_val,
                            const float max_act, int row) {
    float rise_tau = GetDynVal(DYN_RISE_TAU, row);
    float rise_dt = GetDynVal(DYN_RISE_DT, row);
    float decay_dt = GetDynVal(DYN_DECAY_DT, row);
    float nw = cur_val;
    float del = cur_val - prv_val;
    if(rise_tau > 0.0f && cur_val < max_act && del >= 0.0f) {
      nw += max_act * rise_dt; 
    }
    else {
      nw -= max_act * decay_dt;
    }
    if(nw > max_act) nw = max_act;
    if(nw < 0.001f) nw = 0.001f; // non-zero indicates gated..
    return nw;
  }
  // update dynamic value as function of current value and time since gating
  
  virtual void  FormatDynTable();
  // #IGNORE format the dyn table
  virtual void  InitDynTable();
  // default initial dynamics table
  virtual void  UpdtDynTable();
  // #BUTTON update the dt values from the tau values

  float Compute_NetinExtras(LeabraUnitVars* uv, LeabraNetwork* net,
                            int thr_no, float& net_syn) override;
  void	Send_DeepRawNetin(LeabraUnitVars* uv, LeabraNetwork* net, int thr_no) override;
  void	Send_DeepRawNetin_Post(LeabraUnitVars* uv, LeabraNetwork* net, int thr_no) override;
  void  Compute_DeepRaw(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) override;
  void  Compute_DeepNorm(LeabraUnitVars* uv, LeabraNetwork* net, int thr_no) override;
  void  Send_DeepNormNetin(LeabraUnitVars* uv, LeabraNetwork* net, int thr_no) override;

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
