// Copyright 2016, Regents of the University of Colorado,
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
  bool  IsMarkerCon() override { return true; }
  bool  DoesStdNetin() override { return false; }
  bool  DoesStdDwt() override { return true; }

  // don't send regular net inputs or learn!
  void Send_NetinDelta(LeabraConGroup*, LeabraNetwork* net, int thr_no, 
				const float su_act_delta_eff) override { };
  float Compute_Netin(ConGroup* cg, Network* net, int thr_no) override
  { return 0.0f; }
  void  Compute_dWt(ConGroup*, Network* net, int thr_no) override { };
  void	Compute_Weights(ConGroup*, Network* net, int thr_no) override { };

  void  GetPrjnName(Projection& prjn, String& nm) override;

  TA_BASEFUNS_NOCOPY(MarkerConSpec);
protected:
  SPEC_DEFAULTS;
private:
  void 	Initialize();
  void	Destroy()		{ };
  void	Defaults_init();
};

#endif // MarkerConSpec_h
