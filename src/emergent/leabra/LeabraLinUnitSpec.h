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

#ifndef LeabraLinUnitSpec_h
#define LeabraLinUnitSpec_h 1

// parent includes:
#include <LeabraUnitSpec>

// member includes:

// declare all other types mentioned but not required to include:
class LeabraUnit; // 
class LeabraNetwork; // 

TypeDef_Of(LeabraLinUnitSpec);

class LEABRA_API LeabraLinUnitSpec : public LeabraUnitSpec {
  // a pure linear unit (suitable for an AC unit spec unit)
INHERITED(LeabraUnitSpec)
public:
  void 	Compute_ActFmVm(LeabraUnit* u, LeabraNetwork* net);
  
  TA_BASEFUNS(LeabraLinUnitSpec);
protected:
  SPEC_DEFAULTS;
private:
  void 	Initialize();
  void	Destroy()		{ };
  void	Defaults_init() 	{ }
};

#endif // LeabraLinUnitSpec_h
