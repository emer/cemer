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

#ifndef PFCUnitSpec_h
#define PFCUnitSpec_h 1

// parent includes:
#include <LeabraUnitSpec>

// member includes:

// declare all other types mentioned but not required to include:
class PBWMUnGpData; //
class PFCLayerSpec; //

eTypeDef_Of(PFCUnitSpec);

class E_API PFCUnitSpec : public LeabraUnitSpec {
  // PFC unit spec -- works with PFCLayerSpec to implement gated deep-layer context updating dynamics -- p_act_p contains gating time activation state, and misc_1 is 1 if network gated last trial
INHERITED(LeabraUnitSpec)
public:

  virtual PBWMUnGpData*  PFCUnGpData(LeabraUnit* u, LeabraNetwork* net, 
                                     PFCLayerSpec*& pfcls);
  // returns true if my stripe gated this trial -- this is only valid in PostSettle for phase_no == 1 or thereafter (TI_ComputeCtxtAct)

  override void	Trial_Init_Unit(LeabraUnit* u, LeabraNetwork* net, int thread_no=-1);
  override void	TI_Compute_CtxtAct(LeabraUnit* u, LeabraNetwork* net);
  override void PostSettle(LeabraUnit* u, LeabraNetwork* net);

  TA_SIMPLE_BASEFUNS(PFCUnitSpec);
protected:
  SPEC_DEFAULTS;
private:
  void	Initialize();
  void	Destroy()		{ };
  void	Defaults_init();
};

#endif // PFCUnitSpec_h
