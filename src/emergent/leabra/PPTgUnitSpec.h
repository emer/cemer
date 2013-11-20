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

#ifndef PPTgUnitSpec_h
#define PPTgUnitSpec_h 1

// parent includes:
#include <LeabraUnitSpec>

// member includes:

// declare all other types mentioned but not required to include:

eTypeDef_Of(PPTgUnitSpec);

class E_API PPTgUnitSpec : public LeabraUnitSpec {
  // represents the PPTg -- drives burst firing of dopamine neurons as a function of the positive-only temporal derivative of its net input from one trial to the next -- misc_1 holds the prior trial net input
INHERITED(LeabraUnitSpec)
public:
  float         d_net_gain;     // extra multiplicative gain factor to apply to the net input delta -- brings this delta back into the typical range so that more standard activation functions can be used
  bool          clamp_act;      // clamp the activation directly instead of driving net input -- still applies d_net_gain -- provides a purely linear y-dot reference signal

  override void	Compute_Act(Unit* u, Network* net, int thread_no=-1);
  override void	PostSettle(LeabraUnit* u, LeabraNetwork* net);

  TA_SIMPLE_BASEFUNS(PPTgUnitSpec);
protected:
  SPEC_DEFAULTS;
private:
  void  Initialize();
  void  Destroy()     { };
  void	Defaults_init();
};

#endif // PPTgUnitSpec_h
