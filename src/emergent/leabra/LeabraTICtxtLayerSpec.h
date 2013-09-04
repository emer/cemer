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

eTypeDef_Of(LeabraTICtxtLayerSpec);

class E_API LeabraTICtxtLayerSpec : public LeabraLayerSpec {
  // a layer that continuously copies context values from associated layer that is using the TI algorithm to compute context activations -- this makes it possible to connect those context values to other layers in more flexible ways -- must receive one-to-one prjn from source layer and have same configuration
INHERITED(LeabraLayerSpec)
public:
  enum TIActVal {
    P_ACT_P,			// copy the p_act_p value from the source network -- this is the activation at the time of gating (e.g., for PFCLayers)
    ACT_CTXT,			// copy the act_ctxt value from the source network -- this is the already-mixed and normalized context netinput value
  };

  TIActVal	act_val;	// which activation value to get from the source layer?
  bool          pfc_gate_dynamic_updt; // if true, and we recv from a PFCLayerSpec layer, then when that PFC layer has gated on a given trial, it will dynamically update the activation directly from the current activation of those 'superficial' pfc units -- this allows gating effects to propagate within one trial, instead of requiring multiple trials

  virtual void Compute_ActFmSource(LeabraLayer* lay, LeabraNetwork* net);
  // set current act of deep unit to sending super unit activation

  override void Compute_CycleStats(LeabraLayer* lay, LeabraNetwork* net);

  override bool	Compute_dWt_FirstPlus_Test(LeabraLayer* lay, LeabraNetwork* net)
  { return false; }
  override bool	Compute_dWt_Nothing_Test(LeabraLayer* lay, LeabraNetwork* net)
  { return false; }

  override bool  CheckConfig_Layer(Layer* lay, bool quiet=false);

  TA_SIMPLE_BASEFUNS(LeabraTICtxtLayerSpec);
private:
  void  Initialize();
  void  Destroy()     { };
  void	Defaults_init();
};

#endif // LeabraTICtxtLayerSpec_h
