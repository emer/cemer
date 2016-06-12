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

#ifndef LeabraMultCopyUnitSpec_h
#define LeabraMultCopyUnitSpec_h 1

// parent includes:
#include <LeabraUnitSpec>

// member includes:

// declare all other types mentioned but not required to include:

eTypeDef_Of(LeabraMultCopyUnitSpec);

class E_API LeabraMultCopyUnitSpec : public LeabraUnitSpec {
  // unit that copies activations from one layer and multiplies them by values from another -- i.e., multiplicative gating -- must recv from 2 prjns (any more ignored) -- first is copy activation, second is multiplication activation
INHERITED(LeabraUnitSpec)
public:
  bool		one_minus;	// if true, use 1-mult activation as the multiplier -- this is useful with mutually exclusive options in the multipliers, where you hook each up to the *other* alternative, such that this other alternative inhibits this option
  float		mult_gain;	// multiplier gain -- multiply the mult act value by this gain, with a max overall resulting net mult value of 1

  virtual void	Compute_MultCopy(LeabraUnitVars* u, LeabraNetwork* net, int thr_no);
  
  void	Compute_NetinInteg(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) override { };
  void	Compute_ApplyInhib
    (LeabraUnitVars* uv, LeabraNetwork* net, int thr_no, LeabraLayerSpec* lspec,
     LeabraInhib* thr, float ival) override { };
  void	Compute_Act_Rate(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) override;
  void	Compute_Act_Spike(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) override;

  void 	Compute_dWt(UnitVars* u, Network* net, int thr_no) override { };
  void	Compute_dWt_Norm(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) { };
  void	Compute_Weights(UnitVars* u, Network* net, int thr_no) override { };

  bool  CheckConfig_Unit(Layer* lay, bool quiet=false) override;

  TA_SIMPLE_BASEFUNS(LeabraMultCopyUnitSpec);
protected:
  SPEC_DEFAULTS;
private:
  void  Initialize();
  void  Destroy()     { };
  void  Defaults_init() { Initialize(); }
};

#endif // LeabraMultCopyUnitSpec_h
