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

#ifndef Deep5bUnitSpec_h
#define Deep5bUnitSpec_h 1

// parent includes:
#include <LeabraUnitSpec>

// member includes:

// declare all other types mentioned but not required to include:

// todo: actually change this to Deep5bUnitSpec!

eTypeDef_Of(Deep5bUnitSpec);

class E_API Deep5bUnitSpec : public LeabraUnitSpec {
  // units that continuously copy deep5b activation values from a one-to-one receiving projection -- should typically just be used for visualization convenience -- use Deep5bConSpec to send deep5b activations to the d5b_net of other layers
INHERITED(LeabraUnitSpec)
public:
  virtual void Compute_ActFmSource(LeabraUnit* u, LeabraNetwork* net);
  // set current act of deep unit to sending super unit activation

  void	Compute_NetinInteg(LeabraUnit* u, LeabraNetwork* net, int thread_no=-1) override { };
  void	Compute_ApplyInhib(LeabraUnit* u, LeabraLayerSpec* lspec, 
                           LeabraNetwork* net, LeabraInhib* thr, float ival) override { };
  void	Compute_Act(Unit* u, Network* net, int thread_no=-1) override;

  void 	Compute_dWt(Unit* u, Network* net, int thread_no=-1) override { };
  void	Compute_dWt_Norm(LeabraUnit* u, LeabraNetwork* net, int thread_no=-1) override { };
  void	Compute_Weights(Unit* u, Network* net, int thread_no=-1) override { };

  bool  CheckConfig_Unit(Unit* u, bool quiet=false) override;

  TA_SIMPLE_BASEFUNS(Deep5bUnitSpec);
private:
  void  Initialize();
  void  Destroy()     { };
  void	Defaults_init();
};

#endif // Deep5bUnitSpec_h
