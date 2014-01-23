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

#ifndef LeabraMultCopyLayerSpec_h
#define LeabraMultCopyLayerSpec_h 1

// parent includes:
#include <LeabraLayerSpec>

// member includes:

// declare all other types mentioned but not required to include:

eTypeDef_Of(LeabraMultCopyLayerSpec);

class E_API LeabraMultCopyLayerSpec : public LeabraLayerSpec {
  // layer that copies activations from one layer and multiplies them by values from another -- i.e., multiplicative gating -- must recv from 2 prjns (any more ignored) -- first is copy activation, second is multiplication activation
INHERITED(LeabraLayerSpec)
public:
  bool		one_minus;	// if true, use 1-mult activation as the multiplier -- this is useful with mutually exclusive options in the multipliers, where you hook each up to the *other* alternative, such that this other alternative inhibits this option
  float		mult_gain;	// multiplier gain -- multiply the mult act value by this gain, with a max overall resulting net mult value of 1

  virtual void	Compute_MultCopyAct(LeabraLayer* lay, LeabraNetwork* net);
  // compute mult copy activations -- replaces std act fun -- called in Compute_CycleStats -- just overwrites whatever the regular funs compute

  void	Compute_Inhib(LeabraLayer* lay, LeabraNetwork* net) CPP11_OVERRIDE { };
  void Compute_CycleStats(LeabraLayer* lay, LeabraNetwork* net) CPP11_OVERRIDE;

  // don't do any learning:
  bool	Compute_dWt_FirstPlus_Test(LeabraLayer* lay, LeabraNetwork* net) CPP11_OVERRIDE
  { return false; }
  bool	Compute_dWt_Nothing_Test(LeabraLayer* lay, LeabraNetwork* net) CPP11_OVERRIDE
  { return false; }

  bool  CheckConfig_Layer(Layer* lay, bool quiet=false);

  TA_SIMPLE_BASEFUNS(LeabraMultCopyLayerSpec);
protected:
  SPEC_DEFAULTS;
private:
  void 	Initialize();
  void	Destroy()		{ };
  void	Defaults_init()		{ };
};

#endif // LeabraMultCopyLayerSpec_h
