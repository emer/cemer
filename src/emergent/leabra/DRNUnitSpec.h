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
  float         se_base;        // baseline 5HT serotonin level
  float         se_gain;        // #DEF_0:2 #MIN_0 multiplier for 5HT serotonin values as they are sent to other layers
  float         se_pv_dt;       // #MIN_0 #MAX_1 time constant for integrating PV outcome values over time
  float         se_state_dt;    // #MIN_0 #MAX_1 time constant for integrating bodily state values over time -- typically much slower
  bool          sub_pos;        // subtract positive values (otherwise just use negative values)

  String       GetTypeDecoKey() const override { return "UnitSpec"; }

  TA_SIMPLE_BASEFUNS(DRN5htSpec);
protected:
  SPEC_DEFAULTS;
private:
  void  Initialize();
  void  Destroy()       { };
  void  Defaults_init() { Initialize(); }
};


eTypeDef_Of(DRNUnitSpec);

class E_API DRNUnitSpec : public LeabraUnitSpec {
  // Models the Dorsal Raphe Nucleus which drives serotonin (se, 5HT) as a long-term running average of bad primary value outcomes and bodily state variables (optionally as a the balance between good and bad)
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
