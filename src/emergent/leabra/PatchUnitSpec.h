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

#ifndef PatchUnitSpec_h
#define PatchUnitSpec_h 1

// parent includes:
#include <LeabraUnitSpec>

// member includes:

// declare all other types mentioned but not required to include:

eTypeDef_Of(PatchUnitSpec);

class E_API PatchUnitSpec  : public LeabraUnitSpec {
  // Dorsal striatum patch neurons, which shunt dopamine values in all neurons that they project to (shortcut for shunting SNc dopamine which then projects to other target striatum neurons) -- typically driven by PFC maintenance inputs, blocking learning for anything with ongoing maintenance -- threshold for shunting is opt_thresh.send threshold
INHERITED(LeabraUnitSpec)
public:
  virtual void  Send_DAShunt(LeabraUnit* u, LeabraNetwork* net);
  // if activation is over opt_thresh.send threshold, we shunt dav in sending targets

  void	Compute_Act_Post(LeabraUnit* u, LeabraNetwork* net, int thread_no = -1) override;

  // no learning in this one -- just a readout of PFC activation
  void 	Compute_dWt(Unit* u, Network* net, int thread_no=-1) override { };
  void	Compute_dWt_Norm(LeabraUnit* u, LeabraNetwork* net, int thread_no=-1) override { };
  void	Compute_Weights(Unit* u, Network* net, int thread_no=-1) override { };

  TA_SIMPLE_BASEFUNS(PatchUnitSpec);
protected:
  SPEC_DEFAULTS;
private:
  void  Initialize();
  void  Destroy()     { };
  void  Defaults_init();
};


#endif // PatchUnitSpec_h
