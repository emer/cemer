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

#ifndef VSPatchUnitSpec_h
#define VSPatchUnitSpec_h 1

// parent includes:
#include <LeabraUnitSpec>

// member includes:

// declare all other types mentioned but not required to include:

eTypeDef_Of(VSPatchUnitSpec);

class E_API VSPatchUnitSpec : public LeabraUnitSpec {
  // simulates the ventral striatum patch units, both direct (recv from negative valence) and indirect (positive valence) -- should recv a MarkerConSpec projection PV layer units that send the lrnmod variable -- use this to set the plus phase clamped activation -- other connections from sensory and BLA can learn to predict -- use LeabraDeltaConSpec
INHERITED(LeabraUnitSpec)
public:

  void	Quarter_Final(LeabraUnit* u, LeabraNetwork* net) override;

  TA_SIMPLE_BASEFUNS(VSPatchUnitSpec);
protected:
  SPEC_DEFAULTS;
private:
  void  Initialize();
  void  Destroy()     { };
  void  Defaults_init();
};

#endif // VSPatchUnitSpec_h
