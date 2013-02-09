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

#ifndef PFCsUnitSpec_h
#define PFCsUnitSpec_h 1

// parent includes:
#include <LeabraUnitSpec>

// member includes:

// declare all other types mentioned but not required to include:

eTypeDef_Of(PFCsUnitSpec);

class E_API PFCsUnitSpec : public LeabraUnitSpec {
  // superficial layer PFC unit spec -- drives maintenance activation values
INHERITED(LeabraUnitSpec)
public:

  override void Compute_ActFmVm_rate(LeabraUnit* u, LeabraNetwork* net);
  override void  Compute_NetinScale(LeabraUnit* u, LeabraNetwork* net);

  TA_SIMPLE_BASEFUNS(PFCsUnitSpec);
protected:
  SPEC_DEFAULTS;
private:
  void	Initialize();
  void	Destroy()		{ };
  void	Defaults_init();
};

#endif // PFCsUnitSpec_h
