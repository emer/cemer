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

#ifndef Deep5bClampUnitSpec_h
#define Deep5bClampUnitSpec_h 1

// parent includes:
#include <LeabraUnitSpec>

// member includes:

// declare all other types mentioned but not required to include:

eTypeDef_Of(Deep5bClampUnitSpec);

class E_API Deep5bClampUnitSpec : public LeabraUnitSpec {
  // When deep5b_qtr is active (typically the plus phase), these units are exclusively driven by their d5b_net netinput, and otherwise they are just like regular neurons -- used for simulating thalamic relay cells as auto-encoder layers hard-clamped to deep5b drivers in the plus phase
INHERITED(LeabraUnitSpec)
public:
  void	Compute_NetinRaw(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) override;

  TA_SIMPLE_BASEFUNS(Deep5bClampUnitSpec);
private:
  void  Initialize();
  void  Destroy()     { };
  void	Defaults_init();
};

#endif // Deep5bClampUnitSpec_h
