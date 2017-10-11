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

#ifndef TDDeltaUnitSpec_h
#define TDDeltaUnitSpec_h 1

// parent includes:
#include <LeabraUnitSpec>

// member includes:

// declare all other types mentioned but not required to include:

eTypeDef_Of(TDDeltaUnitSpec);

class E_API TDDeltaUnitSpec : public LeabraUnitSpec {
  // computes activation = temporal derivative (act_eq - act_m) of sending units in plus phases: note, act will go negative!
INHERITED(LeabraUnitSpec)
public:
  virtual void	Compute_TD(LeabraUnitState_cpp* u, LeabraNetwork* net, int thr_no);
  // compute the td value based on recv projections: every cycle in 1+ phases

  virtual void  Send_TD(LeabraUnitState_cpp* u, LeabraNetwork* net, int thr_no);
  // send the TD value to sending projections in da_p: every cycle

  void	Compute_Act_Rate(LeabraUnitState_cpp* u, LeabraNetwork* net, int thr_no) override;
  void	Compute_Act_Spike(LeabraUnitState_cpp* u, LeabraNetwork* net, int thr_no) override;
  void  Compute_Act_Post(LeabraUnitState_cpp* uv, LeabraNetwork* net, int thr_no) override;

  void	HelpConfig();	// #BUTTON get help message for configuring this spec
  bool  CheckConfig_Unit(Layer* lay, bool quiet=false) override;

  TA_SIMPLE_BASEFUNS(TDDeltaUnitSpec);
protected:
  SPEC_DEFAULTS;
private:
  void 	Initialize();
  void	Destroy()		{ };
  void	Defaults_init();
};

#endif // TDDeltaUnitSpec_h
