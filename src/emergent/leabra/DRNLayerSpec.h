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

#ifndef DRNLayerSpec_h
#define DRNLayerSpec_h 1

// parent includes:
#include <LeabraLayerSpec>

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

  String       GetTypeDecoKey() const override { return "LayerSpec"; }

  TA_SIMPLE_BASEFUNS(DRN5htSpec);
protected:
  SPEC_DEFAULTS;
private:
  void  Initialize();
  void  Destroy()       { };
  void  Defaults_init() { Initialize(); }
};


eTypeDef_Of(DRNLayerSpec);

class E_API DRNLayerSpec : public LeabraLayerSpec {
  // Models the Dorsal Raphe Nucleus which drives serotonin (se, 5HT) as a long-term running average of bad primary value outcomes and bodily state variables (optionally as a the balance between good and bad)
INHERITED(LeabraLayerSpec)
public:
  DRN5htSpec      se;             // parameters for computing serotonin values

  virtual void  Send_Se(LeabraLayer* lay, LeabraNetwork* net);
  // send the se value to sending projections: every cycle
  virtual void  Compute_Se(LeabraLayer* lay, LeabraNetwork* net);
  // compute the se value based on recv projections from PV and bodily state layers

  void	Init_Weights(LeabraLayer* lay, LeabraNetwork* net) override;
  void PostSettle(LeabraLayer* lay, LeabraNetwork* net) override;
  void Compute_NetinStats(LeabraLayer* lay, LeabraNetwork* net) override { };
  void Compute_Inhib(LeabraLayer* lay, LeabraNetwork* net) override { };
  void Compute_CycleStats(LeabraLayer* lay, LeabraNetwork* net) override;

  // never learn
  bool Compute_SRAvg_Test(LeabraLayer* lay, LeabraNetwork* net)  override { return false; }
  bool Compute_dWt_FirstPlus_Test(LeabraLayer* lay, LeabraNetwork* net) override { return false; }
  bool Compute_dWt_Nothing_Test(LeabraLayer* lay, LeabraNetwork* net) override { return false; }

  void  HelpConfig();   // #BUTTON get help message for configuring this spec
  bool  CheckConfig_Layer(Layer* lay, bool quiet=false);

  TA_SIMPLE_BASEFUNS(DRNLayerSpec);
protected:
  SPEC_DEFAULTS;
private:
  void  Initialize();
  void  Destroy()     { };
  void  Defaults_init() { Initialize(); }
};

#endif // DRNLayerSpec_h
