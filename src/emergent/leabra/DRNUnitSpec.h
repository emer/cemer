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

#ifndef DRNUnitSpec_h
#define DRNUnitSpec_h 1

// parent includes:
#include <LeabraUnitSpec>

// member includes:

// declare all other types mentioned but not required to include:

eTypeDef_Of(DRN5htSpec);

class E_API DRN5htSpec : public SpecMemberBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra specs for DRN 5ht parameters
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
  
  String       GetTypeDecoKey() const override { return "UnitSpec"; }

  TA_SIMPLE_BASEFUNS(DRN5htSpec);
protected:
  SPEC_DEFAULTS;
  void  UpdateAfterEdit_impl();
private:
  void  Initialize();
  void  Defaults_init();
  void  Destroy()       { };
};


eTypeDef_Of(DRNUnitSpec);

class E_API DRNUnitSpec : public LeabraUnitSpec {
  // Models the Dorsal Raphe Nucleus which drives serotonin (se, 5HT) as a long-term running average of bad primary value outcomes and bodily state variables (optionally as a the balance between good and bad), and also integrates dopamine values over time if given
INHERITED(LeabraUnitSpec)
public:
  DRN5htSpec      se;             // parameters for computing serotonin values

  virtual void  Send_Se(LeabraUnitVars* u, LeabraNetwork* net, int thr_no);
  // send the se value to sending projections: every cycle
  virtual void  Compute_Se(LeabraUnitVars* u, LeabraNetwork* net, int thr_no);
  // compute the se value based on recv projections from PV and bodily state layers

  void  Init_Weights(UnitVars* ru, Network* rnet, int thr_no) override;

  void	Compute_NetinInteg(LeabraUnitVars* u, LeabraNetwork* net, int thread_no) override { };
  void	Compute_ApplyInhib
    (LeabraUnitVars* uv, LeabraNetwork* net, int thr_no, LeabraLayerSpec* lspec,
     LeabraInhib* thr, float ival) override { };
  void	Compute_Act_Rate(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) override;
  void	Compute_Act_Spike(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) override;

  void 	Compute_dWt(UnitVars* u, Network* net, int thr_no) override { };
  void	Compute_dWt_Norm(UnitVars* u, Network* net, int thr_no) { };
  void	Compute_Weights(UnitVars* u, Network* net, int thr_no) override { };

  void  HelpConfig();   // #BUTTON get help message for configuring this spec

  TA_SIMPLE_BASEFUNS(DRNUnitSpec);
protected:
  SPEC_DEFAULTS;
private:
  void  Initialize();
  void  Destroy()     { };
  void  Defaults_init() { Initialize(); }
};

#endif // DRNUnitSpec_h
