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

#ifndef InvertUnitSpec_h
#define InvertUnitSpec_h 1

// parent includes:
#include <LeabraUnitSpec>

// member includes:

// declare all other types mentioned but not required to include:

eTypeDef_Of(InvertUnitSpec);

class E_API InvertUnitSpec : public LeabraUnitSpec {
  // a unit that continuously copies activations from input (use one to one prjns, weights don't matter) and sets our activations to 1.0 - input->act
INHERITED(LeabraUnitSpec)
public:
  virtual void Compute_ActFmSource(LeabraUnitState_cpp* u, LeabraNetwork* net, int thr_no);
  // set current act 

  void	Compute_NetinInteg(LeabraUnitState_cpp* u, LeabraNetwork* net, int thr_no) override { };
  void	Compute_Act_Rate(LeabraUnitState_cpp* u, LeabraNetwork* net, int thr_no) override;
  void	Compute_Act_Spike(LeabraUnitState_cpp* u, LeabraNetwork* net, int thr_no) override;

  void 	Compute_dWt(UnitState* u, Network* net, int thr_no) override { };
  void	Compute_Weights(UnitState* u, Network* net, int thr_no) override { };

  bool  CheckConfig_Unit(Layer* lay, bool quiet=false) override;

  TA_SIMPLE_BASEFUNS(InvertUnitSpec);
protected:
  SPEC_DEFAULTS;
private:
  void  Initialize();
  void  Destroy()     { };
  void	Defaults_init();
};

#endif // InvertUnitSpec_h
