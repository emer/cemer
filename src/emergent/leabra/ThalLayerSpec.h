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

#ifndef ThalLayerSpec_h
#define ThalLayerSpec_h 1

// parent includes:
#include <LeabraLayerSpec>

// member includes:

// declare all other types mentioned but not required to include:

eTypeDef_Of(ThalLayerSpec);

class E_API ThalLayerSpec  : public LeabraLayerSpec {
  // Models the dorsal and ventral (TRN) thalamus as it interacts with cortex (mainly secondary cortical areas, not primary sensory areas)
INHERITED(LeabraLayerSpec)
public:
  virtual void  Send_Thal(LeabraLayer* lay, LeabraNetwork* net);
  // send the act value as thal to sending projections: every cycle

  void Compute_CycleStats(LeabraLayer* lay, LeabraNetwork* net, int thread_no=-1) override;

  // never learn
  bool Compute_dWt_Test(LeabraLayer* lay, LeabraNetwork* net) override { return false; }

  // void  HelpConfig();   // #BUTTON get help message for configuring this spec
  // bool  CheckConfig_Layer(Layer* lay, bool quiet=false);

  TA_SIMPLE_BASEFUNS(ThalLayerSpec);
protected:
  SPEC_DEFAULTS;
private:
  void  Initialize();
  void  Destroy()     { };
  void  Defaults_init() { Initialize(); }
};

#endif // ThalLayerSpec_h
