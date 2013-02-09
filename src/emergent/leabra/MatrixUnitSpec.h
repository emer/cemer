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

#ifndef MatrixUnitSpec_h
#define MatrixUnitSpec_h 1

// parent includes:
#include <LeabraUnitSpec>

// member includes:

// declare all other types mentioned but not required to include:

eTypeDef_Of(MatrixUnitSpec);

class E_API MatrixUnitSpec : public LeabraUnitSpec {
  // basal ganglia matrix units: fire actions or WM updates. modulated by da signals
INHERITED(LeabraUnitSpec)
public:
  virtual void Compute_MidMinusAct(LeabraUnit* u, LeabraNetwork* net);
  // save the effective mid-minus (gating) activation state for subsequent learning

  void	InitLinks();
  SIMPLE_COPY(MatrixUnitSpec);
  TA_BASEFUNS(MatrixUnitSpec);
protected:
  SPEC_DEFAULTS;
private:
  void	Initialize();
  void	Destroy()		{ };
  void	Defaults_init();
};

#endif // MatrixUnitSpec_h
