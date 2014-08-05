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

#ifndef LeabraTICtxtLayerSpec_h
#define LeabraTICtxtLayerSpec_h 1

// parent includes:
#include <LeabraLayerSpec>

// member includes:

// declare all other types mentioned but not required to include:

eTypeDef_Of(DeepActSpec);

class E_API DeepActSpec : public SpecMemberBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra specs for Cortical Information Flow via Extra Range theory, simulating effects of thalamic drive on cortical neurons, including superficial and deep components of a Unit-level microcolumn -- thalamic input modulates superficial netin and is used thresholded to determine deep activation
INHERITED(SpecMemberBase)
public:
  int          off_start_cyc;     // #MIN_0 cycle to start turning off the deep value 
  int          off_fade_cyc;      // #MIN_1 number of cycles to fade off over -- e.g., if 2 then it goes .5, 0 -- <= 1 is instantaneous

  String       GetTypeDecoKey() const override { return "LayerSpec"; }

  TA_SIMPLE_BASEFUNS(DeepActSpec);
protected:
  SPEC_DEFAULTS;
private:
  void	Initialize();
  void 	Destroy()	{ };
  void	Defaults_init();
};

eTypeDef_Of(LeabraTICtxtLayerSpec);

class E_API LeabraTICtxtLayerSpec : public LeabraLayerSpec {
  // a layer that continuously copies context values from associated layer that is using the TI algorithm to compute context activations -- this makes it possible to connect those context values to other layers in more flexible ways -- must receive one-to-one prjn from source layer and have same configuration
INHERITED(LeabraLayerSpec)
public:
  enum TIActVal {
    DEEP,			// copy the deep value from the source network -- this is the driver of the context values for that unit / microcolumn -- see deep_act params for time course information
    P_ACT_P,			// copy the p_act_p value from the source network -- this is the activation at the time of gating (e.g., for PFCLayers)
    ACT_CTXT,			// copy the act_ctxt value from the source network -- this is the already-mixed and normalized context netinput value
  };

  TIActVal	act_val;	// which activation value to get from the source layer?
  DeepActSpec   deep_act;       // #CONDSHOW_ON_act_val:DEEP -- parameters controlling the dynamics of deep layer activations for DEEP
  bool          pfc_gate_dynamic_updt; // if true, and we recv from a PFCLayerSpec layer, then when that PFC layer has gated on a given trial, it will dynamically update the activation directly from the current activation of those 'superficial' pfc units -- this allows gating effects to propagate within one trial, instead of requiring multiple trials

  virtual void Compute_ActFmSource(LeabraLayer* lay, LeabraNetwork* net);
  // set current act of deep unit to sending super unit activation

  void Compute_CycleStats(LeabraLayer* lay, LeabraNetwork* net) override;

  bool	Compute_dWt_FirstPlus_Test(LeabraLayer* lay, LeabraNetwork* net) override
  { return false; }
  bool	Compute_dWt_Nothing_Test(LeabraLayer* lay, LeabraNetwork* net) override
  { return false; }

  bool  CheckConfig_Layer(Layer* lay, bool quiet=false) override;

  TA_SIMPLE_BASEFUNS(LeabraTICtxtLayerSpec);
private:
  void  Initialize();
  void  Destroy()     { };
  void	Defaults_init();
};

#endif // LeabraTICtxtLayerSpec_h
