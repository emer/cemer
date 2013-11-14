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

#ifndef VSPatchLayerSpec_h
#define VSPatchLayerSpec_h 1

// parent includes:
#include <LeabraLayerSpec>

// member includes:

// declare all other types mentioned but not required to include:

eTypeDef_Of(VSPatchLayerSpec);

class E_API VSPatchLayerSpec : public LeabraLayerSpec {
  // simulates the ventral striatum patch units, both direct (recv from negative valence) and indirect (positive valence) -- get a MarkerConSpec from PV layer units which drive plus phase clamped value at all times -- other connections from sensory and BLA can learn to predict -- use LeabraDeltaConSpec
INHERITED(LeabraLayerSpec)
public:

  virtual void  Compute_PVPlus(LeabraLayer* lay, LeabraNetwork* net);
  // compute the PV-driven plus phase activations

  override void PostSettle(LeabraLayer* lay, LeabraNetwork* net);

  void  HelpConfig();   // #BUTTON get help message for configuring this spec
  bool  CheckConfig_Layer(Layer* lay, bool quiet=false);

  TA_SIMPLE_BASEFUNS(VSPatchLayerSpec);
protected:
  SPEC_DEFAULTS;
private:
  void  Initialize();
  void  Destroy()     { };
  void  Defaults_init();
};

#endif // VSPatchLayerSpec_h
