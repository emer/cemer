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

#ifndef ECoutUnitSpec_h
#define ECoutUnitSpec_h 1

// parent includes:
#include <LeabraUnitSpec>

// member includes:

// declare all other types mentioned but not required to include:

eTypeDef_Of(ECoutUnitSpec);

class E_API ECoutUnitSpec : public LeabraUnitSpec {
  // unit spec for EC out layers that implements ThetaPhase learning -- automatically clamps to ECin activations in plus phase, based on MarkerConSpec one-to-one prjn from ECin, -- must use HippoEncoderConSpec for connections to learn based on encoder phase of theta cycle -- records encoder error as misc_1 unit variable
INHERITED(LeabraUnitSpec)
public:
  virtual void 	ClampFromECin(LeabraUnitVars* u, LeabraNetwork* net, int thr_no);
  // clamp ECout values from ECin values, in plus phase

  void	Compute_Act_Rate(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) override;
  void	Compute_Act_Spike(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) override;

  float Compute_SSE(UnitVars* uv, Network* net, int thr_no, bool& has_targ) override;

  bool CheckConfig_Unit(Layer* lay, bool quiet=false) override; 

  TA_SIMPLE_BASEFUNS(ECoutUnitSpec);
protected:
  SPEC_DEFAULTS;

private:
  void 	Initialize();
  void	Destroy()		{ };
  void	Defaults_init();
};

#endif // ECoutUnitSpec_h
