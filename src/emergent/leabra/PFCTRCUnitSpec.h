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

#ifndef PFCTRCUnitSpec_h
#define PFCTRCUnitSpec_h 1

// parent includes:
#include <LeabraUnitSpec>

// member includes:

// declare all other types mentioned but not required to include:

eTypeDef_Of(PFCTRCUnitSpec);

class E_API PFCTRCUnitSpec : public LeabraUnitSpec {
  // PFC TRC unit spec -- special dynamics for TRC units associated with PFC layers
INHERITED(LeabraUnitSpec)
public:
  void Compute_NetinInteg(LeabraUnitVars* uv, LeabraNetwork* net, int thr_no) override;
  
  TA_SIMPLE_BASEFUNS(PFCTRCUnitSpec);
protected:
  SPEC_DEFAULTS;
  
private:
  void	Initialize();
  void	Destroy()		{ };
  void	Defaults_init();
};

#endif // PFCTRCUnitSpec_h
