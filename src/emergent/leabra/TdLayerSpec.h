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

#ifndef TdLayerSpec_h
#define TdLayerSpec_h 1

// parent includes:
#include <LeabraLayerSpec>

// member includes:

// declare all other types mentioned but not required to include:

eTypeDef_Of(TdLayerSpec);

class E_API TdLayerSpec : public LeabraLayerSpec {
  // computes activation = temporal derivative (act_eq - act_m) of sending units in plus phases: note, act will go negative!
INHERITED(LeabraLayerSpec)
public:
  virtual void	Compute_ZeroAct(LeabraLayer* lay, LeabraNetwork* net);
  // compute a zero td value: in minus phase
  virtual void	Compute_Td(LeabraLayer* lay, LeabraNetwork* net);
  // compute the td value based on recv projections: every cycle in 1+ phases
  virtual void	Send_Td(LeabraLayer* lay, LeabraNetwork* net);
  // send the td value to sending projections: every cycle

  void	Compute_HardClamp(LeabraLayer* lay, LeabraNetwork* net) override;
  void Compute_CycleStats(LeabraLayer* lay, LeabraNetwork* net) override;

  // never learn
  bool	Compute_SRAvg_Test(LeabraLayer* lay, LeabraNetwork* net)  override { return false; }
  bool	Compute_dWt_FirstPlus_Test(LeabraLayer* lay, LeabraNetwork* net) override { return false; }
  bool	Compute_dWt_Nothing_Test(LeabraLayer* lay, LeabraNetwork* net) override { return false; }

  void	HelpConfig();	// #BUTTON get help message for configuring this spec
  bool  CheckConfig_Layer(Layer* lay, bool quiet=false);

  TA_SIMPLE_BASEFUNS(TdLayerSpec);
protected:
  SPEC_DEFAULTS;
private:
  void 	Initialize();
  void	Destroy()		{ };
  void	Defaults_init() { Initialize(); }
};

#endif // TdLayerSpec_h
