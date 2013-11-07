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

#ifndef LHbRMTgUnitSpec_h
#define LHbRMTgUnitSpec_h 1

// parent includes:
#include <LeabraUnitSpec>

// member includes:

// declare all other types mentioned but not required to include:

eTypeDef_Of(LHbRMTgUnitSpec);

class E_API LHbRMTgUnitSpec : public LeabraUnitSpec {
  // combined lateral habenula and RMTg units -- receives from Patch and Matrix Direct and Indirect pathways, along with primary value (PV) positive and negative valence drivers, and computes dopamine dip signals, represented as positive activations in these units, which are then translated into dips through a projection to the VTALayerSpec
INHERITED(LeabraUnitSpec)
public:

  override void	Compute_NetinInteg(LeabraUnit* u, LeabraNetwork* net, int thread_no=-1);

  TA_SIMPLE_BASEFUNS(LHbRMTgUnitSpec);
protected:
  SPEC_DEFAULTS;
private:
  void  Initialize();
  void  Destroy()     { };
  void	Defaults_init();
};

#endif // LHbRMTgUnitSpec_h
