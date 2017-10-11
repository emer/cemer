// Copyright 2017, Regents of the University of Colorado,
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

#ifndef TDRewPredUnitSpec_h
#define TDRewPredUnitSpec_h 1

// parent includes:
#include <LeabraUnitSpec>

// member includes:

// declare all other types mentioned but not required to include:

eTypeDef_Of(TDRewPredUnitSpec);

class E_API TDRewPredUnitSpec : public LeabraUnitSpec {
  // predicts rewards: minus phase = clamped prior expected reward V^(t), plus = settles on expectation of future reward V^(t+1) -- cons should learn based on da_p * prev sending act (act_q0) -- should recv da_p from TDDeltaUnitSpec
INHERITED(LeabraUnitSpec)
public:
  void	Compute_Act_Rate(LeabraUnitState_cpp* u, LeabraNetwork* net, int thr_no) override;
  void	Compute_Act_Spike(LeabraUnitState_cpp* u, LeabraNetwork* net, int thr_no) override;

  void  Init_Acts(UnitState* uv, Network* net, int thr_no) override;
  void  Quarter_Final(LeabraUnitState_cpp* u, LeabraNetwork* net, int thr_no) override;

  void	HelpConfig();	// #BUTTON get help message for configuring this spec
  bool  CheckConfig_Unit(Layer* lay, bool quiet=false) override;

  TA_BASEFUNS_NOCOPY(TDRewPredUnitSpec);
protected:
  SPEC_DEFAULTS;
private:
  void 	Initialize();
  void	Destroy()		{ };
  void	Defaults_init();
};

#endif // TDRewPredUnitSpec_h
