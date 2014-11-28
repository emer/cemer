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

#ifndef ThalUnitSpec_h
#define ThalUnitSpec_h 1

// parent includes:
#include <LeabraUnitSpec>

// member includes:

// declare all other types mentioned but not required to include:

eTypeDef_Of(ThalUnitSpec);

class E_API ThalUnitSpec  : public LeabraUnitSpec {
  // Models the dorsal and ventral (TRN) thalamus as it interacts with cortex (mainly secondary cortical areas, not primary sensory areas) -- simply sends current activation to thal variable in units we project to
INHERITED(LeabraUnitSpec)
public:
  virtual void  Send_Thal(LeabraUnitVars* u, LeabraNetwork* net, int thr_no);
  // send the act value as thal to sending projections: every cycle

  void	Compute_Act(UnitVars* u, Network* net, int thr_no) override;

  // no learning in this one..
  void 	Compute_dWt(UnitVars* u, Network* net, int thr_no) override { };
  void	Compute_dWt_Norm(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) override { };
  void	Compute_Weights(UnitVars* u, Network* net, int thr_no) override { };

  TA_SIMPLE_BASEFUNS(ThalUnitSpec);
protected:
  SPEC_DEFAULTS;
private:
  void  Initialize();
  void  Destroy()     { };
  void  Defaults_init();
};

#endif // ThalUnitSpec_h
