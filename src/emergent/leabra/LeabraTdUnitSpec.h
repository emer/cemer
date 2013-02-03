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

#ifndef LeabraTdUnitSpec_h
#define LeabraTdUnitSpec_h 1

// parent includes:
#include <LeabraUnitSpec>

// member includes:

// declare all other types mentioned but not required to include:

TypeDef_Of(LeabraTdUnitSpec);

class LEABRA_API LeabraTdUnitSpec : public LeabraUnitSpec {
  // Leabra unit with temporal-differences variables for prior activation states
INHERITED(LeabraUnitSpec)
public:
  float		lambda;		// exponential decay parameter for updating activation trace values over time: these trace values are used in learning.  in principle this should also include the effects of the discount (gamma) parameter from the TdRewInteg layer

  override void	Init_Acts(Unit* u, Network* net);
  override void Init_Weights(Unit* u, Network* net);
  override void EncodeState(LeabraUnit* u, LeabraNetwork* net);

  TA_SIMPLE_BASEFUNS(LeabraTdUnitSpec);
protected:
  SPEC_DEFAULTS;
private:
  void	Initialize();
  void	Destroy()		{ };
  void	Defaults_init() 	{ };
};

#endif // LeabraTdUnitSpec_h
