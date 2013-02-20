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

#ifndef MarkerConSpec_h
#define MarkerConSpec_h 1

// parent includes:
#include <LeabraConSpec>

// member includes:

// declare all other types mentioned but not required to include:

eTypeDef_Of(MarkerConSpec);

class E_API MarkerConSpec : public LeabraConSpec {
  // connection spec that marks special projections: doesn't send netin or adapt weights
INHERITED(LeabraConSpec)
public:
  // don't send regular net inputs or learn!
  override void Send_NetinDelta(LeabraSendCons*, LeabraNetwork* net, int thread_no, 
				float su_act_delta_eff) { };
  override float Compute_Netin(RecvCons* cg, Unit* ru) { return 0.0f; }
  override void Compute_dWt(RecvCons*, Unit*) { };
  override void Compute_dWt_LeabraCHL(LeabraSendCons*, LeabraUnit*) { };
  override void Compute_dWt_CtLeabraCAL(LeabraSendCons*, LeabraUnit*) { };
  override void Compute_dWt_CtLeabraXCAL(LeabraSendCons*, LeabraUnit*) { };
  override void Compute_SRAvg(LeabraSendCons*, LeabraUnit*, bool do_s) { };
  override void Trial_Init_SRAvg(LeabraSendCons*, LeabraUnit*) { };
  override void	Compute_Weights(RecvCons*, Unit*) { };
  override void	Compute_Weights_LeabraCHL(LeabraSendCons*, LeabraUnit*) { };
  override void	Compute_Weights_CtLeabraCAL(LeabraSendCons*, LeabraUnit*) { };
  override void	Compute_Weights_CtLeabraXCAL(LeabraSendCons*, LeabraUnit*) { };

  override bool	DMem_AlwaysLocal() { return true; }
  // these connections always need to be there on all nodes..

  TA_BASEFUNS_NOCOPY(MarkerConSpec);
protected:
  SPEC_DEFAULTS;
private:
  void 	Initialize();
  void	Destroy()		{ };
  void	Defaults_init();
};

#endif // MarkerConSpec_h
