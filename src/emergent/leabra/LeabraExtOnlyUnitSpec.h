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

#ifndef LeabraExtOnlyUnitSpec_h
#define LeabraExtOnlyUnitSpec_h 1

// parent includes:
#include <LeabraUnitSpec>

// member includes:

// declare all other types mentioned but not required to include:

eTypeDef_Of(LeabraExtOnlyUnitSpec);

class E_API LeabraExtOnlyUnitSpec : public LeabraUnitSpec {
  // only units with an above-threshold (opt_thresh.send) of ext input are allowed to get active -- soft clamping inputs provide a multiplicative mask on the input dynamics -- kind of a special form of sigma-pi unit -- useful e.g., for visual inputs computed by V2 layer, where network dynamics then operate to disambiguate, but cannot hallucinate -- if no ext input, uses first prjn, first con sending act
INHERITED(LeabraUnitSpec)
public:
  
  void  Compute_NetinInteg(LeabraUnit* u, LeabraNetwork* net, int thread_no) CPP11_OVERRIDE;

  TA_BASEFUNS(LeabraExtOnlyUnitSpec);
protected:
  SPEC_DEFAULTS;
private:
  void 	Initialize();
  void	Destroy()		{ };
  void	Defaults_init() { Initialize(); }
};

#endif // LeabraExtOnlyUnitSpec_h
