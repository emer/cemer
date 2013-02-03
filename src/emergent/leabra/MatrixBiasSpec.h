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

#ifndef MatrixBiasSpec_h
#define MatrixBiasSpec_h 1

// parent includes:
#include <LeabraBiasSpec>

// member includes:
#include <LeabraUnit>

// declare all other types mentioned but not required to include:

TypeDef_Of(MatrixBiasSpec);

class LEABRA_API MatrixBiasSpec : public LeabraBiasSpec {
  // Matrix bias spec: special learning parameters for matrix units
INHERITED(LeabraBiasSpec)
public:

  inline override void B_Compute_dWt_LeabraCHL(LeabraCon* cn, LeabraUnit* ru) {
    float err = ru->act_mid * ru->dav;
    if(fabsf(err) >= dwt_thresh)
      cn->dwt += cur_lrate * err;
  }

  inline override void B_Compute_dWt_CtLeabraXCAL(LeabraCon* cn, LeabraUnit* ru,
						  LeabraLayer* rlay) {
    B_Compute_dWt_LeabraCHL(cn, ru);
  }

  inline override void B_Compute_dWt_CtLeabraCAL(LeabraCon* cn, LeabraUnit* ru,
						 LeabraLayer* rlay) {
    B_Compute_dWt_LeabraCHL(cn, ru);
  }
  
  TA_BASEFUNS(MatrixBiasSpec);
protected:
  SPEC_DEFAULTS;
private:
  void 	Initialize();
  void	Destroy()		{ };
  void	Defaults_init();
};

#endif // MatrixBiasSpec_h
