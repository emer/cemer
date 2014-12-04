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

#ifndef LearnModUnitSpec_h
#define LearnModUnitSpec_h 1

// parent includes:
#include <LeabraUnitSpec>

// member includes:

// declare all other types mentioned but not required to include:

eTypeDef_Of(LearnModUnitSpec);

class E_API LearnModUnitSpec : public LeabraUnitSpec {
  // activity on this unit drives special lrnmod value in the units of connections that it projects to (subject to threshold) -- used for Primary Value layers in gdPVLV architecture, to drive learning at time of PV in other layers
INHERITED(LeabraUnitSpec)
public:
  float         learn_thr;      // #DEF_0.1 Threshold value of unit activation to set the learning flag on

  virtual void  Send_LearnMod(LeabraUnitVars* u, LeabraNetwork* net, int thr_no);
  // send the unit lrnmod value to all units that we project to, based on unit activation thresholds

  void	Compute_Act_Rate(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) override;
  void	Compute_Act_Spike(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) override;

  TA_SIMPLE_BASEFUNS(LearnModUnitSpec);
protected:
  SPEC_DEFAULTS;
private:
  void  Initialize();
  void  Destroy()     { };
  void	Defaults_init();
};

#endif // LearnModUnitSpec_h
