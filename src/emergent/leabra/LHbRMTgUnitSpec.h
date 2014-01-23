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

#ifndef LHbRMTgUnitSpec_h
#define LHbRMTgUnitSpec_h 1

// parent includes:
#include <LeabraUnitSpec>

// member includes:

// declare all other types mentioned but not required to include:

eTypeDef_Of(LHbRMTgGains);

class E_API LHbRMTgGains : public SpecMemberBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra gains for LHbRMTg inputs
INHERITED(SpecMemberBase)
public:
  float         all;            // #MIN_0 final overall gain on everything
  float         patch_dir;      // #MIN_0 VS patch direct pathway versus negative PV outcomes
  float         patch_ind;      // #MIN_0 VS patch indirect pathway versus positive PV outcomes
  float         matrix;         // #MIN_0 VS gain on matrix pathway 
  bool          matrix_td;      // compute temporal derivative over matrix pos inputs to produce a dip when LV values go down (misc_1 holds the prior trial net input) -- otherwise matrix is matrix_ind - matrix_dir difference between NoGo and Go (dips driven by greater NoGo than Go balance)

  String       GetTypeDecoKey() const override { return "UnitSpec"; }

  TA_SIMPLE_BASEFUNS(LHbRMTgGains);
protected:
  SPEC_DEFAULTS;
private:
  void  Initialize();
  void  Destroy()       { };
  void  Defaults_init();
};


eTypeDef_Of(LHbRMTgUnitSpec);

class E_API LHbRMTgUnitSpec : public LeabraUnitSpec {
  // combined lateral habenula and RMTg units -- receives from Patch and Matrix Direct and Indirect pathways, along with primary value (PV) positive and negative valence drivers, and computes dopamine dip signals, represented as positive activations in these units, which are then translated into dips through a projection to the VTALayerSpec
INHERITED(LeabraUnitSpec)
public:
  LHbRMTgGains   gains;         // gain parameters (multiplicative constants) for various sources of inputs

  void	Compute_NetinInteg(LeabraUnit* u, LeabraNetwork* net, int thread_no=-1) override;
  void	PostSettle(LeabraUnit* u, LeabraNetwork* net) override;

  bool  CheckConfig_Unit(Unit* un, bool quiet=false) override;
  void  HelpConfig();   // #BUTTON get help message for configuring this spec

  TA_SIMPLE_BASEFUNS(LHbRMTgUnitSpec);
protected:
  SPEC_DEFAULTS;
private:
  void  Initialize();
  void  Destroy()     { };
  void	Defaults_init();
};

#endif // LHbRMTgUnitSpec_h
