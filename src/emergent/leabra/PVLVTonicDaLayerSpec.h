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

#ifndef PVLVTonicDaLayerSpec_h
#define PVLVTonicDaLayerSpec_h 1

// parent includes:
#include <PVLVLayerSpec>

// member includes:

// declare all other types mentioned but not required to include:

TypeDef_Of(PVLVTonicDaLayerSpec);

class LEABRA_API PVLVTonicDaLayerSpec : public PVLVLayerSpec {
  // display of tonic dopamine level -- just clamps the current value which is always stored in network->pvlv_tonic_da and is the definitive value (which can be manipulated by other layers) that is just reflected in this layer -- does not do any actual computation
INHERITED(PVLVLayerSpec)
public:
  virtual void  Clamp_Da(LeabraLayer* lay, LeabraNetwork* net);
  // clamp the current Da value on the layer activations

  override void BuildUnits_Threads(LeabraLayer* lay, LeabraNetwork* net);
  override void Compute_NetinStats(LeabraLayer* lay, LeabraNetwork* net) { };
  override void Compute_Inhib(LeabraLayer* lay, LeabraNetwork* net) { };
  override void Compute_ApplyInhib(LeabraLayer* lay, LeabraNetwork* net);

  // never learn
  override bool	Compute_SRAvg_Test(LeabraLayer* lay, LeabraNetwork* net)  { return false; }
  override bool	Compute_dWt_FirstPlus_Test(LeabraLayer* lay, LeabraNetwork* net) { return false; }
  override bool	Compute_dWt_Nothing_Test(LeabraLayer* lay, LeabraNetwork* net) { return false; }

  void	HelpConfig();	// #BUTTON get help message for configuring this spec
  bool  CheckConfig_Layer(Layer* lay, bool quiet=false);

  TA_SIMPLE_BASEFUNS(PVLVTonicDaLayerSpec);
protected:
  SPEC_DEFAULTS;
private:
  void  Initialize();
  void  Destroy()               { };
  void  Defaults_init()         { };
};

#endif // PVLVTonicDaLayerSpec_h
