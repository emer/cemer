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

#ifndef CA3UnitSpec_h
#define CA3UnitSpec_h 1

// parent includes:
#include <LeabraUnitSpec>

// member includes:

// declare all other types mentioned but not required to include:

eTypeDef_Of(CA3UnitSpec);

class E_API CA3UnitSpec : public LeabraUnitSpec {
  // unit spec for ECin layers, for ThetaPhase learning -- only special behavior is to run Init_Netins in Compute_NetinScale at appropriate quarters where CA1 modulates its projections, so that the netinput is communicated
INHERITED(LeabraUnitSpec)
public:

  void Compute_NetinScale(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) override;

  TA_SIMPLE_BASEFUNS(CA3UnitSpec);
private:
  void Initialize()  { };
  void Destroy()     { };
};

#endif // CA3UnitSpec_h
