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

#ifndef InvertLayerSpec_h
#define InvertLayerSpec_h 1

// parent includes:
#include <LeabraLayerSpec>

// member includes:

// declare all other types mentioned but not required to include:

eTypeDef_Of(InvertLayerSpec);

class E_API InvertLayerSpec : public LeabraLayerSpec {
  // a layer that continuously copies activations from input layer (use one to one prjns, weights dont' matter) and sets our activations to 1.0 - input->act
INHERITED(LeabraLayerSpec)
public:
  virtual void Compute_ActFmSource(LeabraLayer* lay, LeabraNetwork* net);
  // set current act 

  void Compute_CycleStats(LeabraLayer* lay, LeabraNetwork* net, int thread_no=-1) override;

  bool	Compute_dWt_Test(LeabraLayer* lay, LeabraNetwork* net) override
  { return false; }

  bool  CheckConfig_Layer(Layer* lay, bool quiet=false) override;

  TA_SIMPLE_BASEFUNS(InvertLayerSpec);
private:
  void  Initialize();
  void  Destroy()     { };
  void	Defaults_init();
};

#endif // InvertLayerSpec_h
