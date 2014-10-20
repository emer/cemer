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

#ifndef Deep5bLayerSpec_h
#define Deep5bLayerSpec_h 1

// parent includes:
#include <LeabraLayerSpec>

// member includes:

// declare all other types mentioned but not required to include:

// todo: actually change this to Deep5bUnitSpec!

eTypeDef_Of(Deep5bLayerSpec);

class E_API Deep5bLayerSpec : public LeabraLayerSpec {
  // a layer that continuously copies deep5b activation values from associated layer -- should typically just be used for visualization convenience -- use Deep5bConSpec to send deep5b activations to the d5b_net of other layers -- this must receive one-to-one prjn from source layer and have same configuration
INHERITED(LeabraLayerSpec)
public:
  virtual void Compute_ActFmSource(LeabraLayer* lay, LeabraNetwork* net);
  // set current act of deep unit to sending super unit activation

  void Compute_CycleStats(LeabraLayer* lay, LeabraNetwork* net, int thread_no=-1) override;

  bool  CheckConfig_Layer(Layer* lay, bool quiet=false) override;

  TA_SIMPLE_BASEFUNS(Deep5bLayerSpec);
private:
  void  Initialize();
  void  Destroy()     { };
  void	Defaults_init();
};

#endif // Deep5bLayerSpec_h
