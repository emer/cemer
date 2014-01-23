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

eTypeDef_Of(MatrixBiasSpec);

class E_API MatrixBiasSpec : public LeabraBiasSpec {
  // Matrix bias spec: special learning parameters for matrix units
INHERITED(LeabraBiasSpec)
public:

  inline void B_Compute_dWt_LeabraCHL(RecvCons* bias, LeabraUnit* ru) CPP11_OVERRIDE {
    float err = ru->act_p * ru->dav;
    if(fabsf(err) >= dwt_thresh) {
      bias->OwnCn(0,DWT) += cur_lrate * err;
    }
  }

  inline void B_Compute_dWt_CtLeabraXCAL(RecvCons* bias, LeabraUnit* ru,
						  LeabraLayer* rlay) CPP11_OVERRIDE {
    B_Compute_dWt_LeabraCHL(bias, ru);
  }

  inline void B_Compute_dWt_CtLeabraCAL(RecvCons* bias, LeabraUnit* ru,
						 LeabraLayer* rlay) CPP11_OVERRIDE {
    B_Compute_dWt_LeabraCHL(bias, ru);
  }
  
  TA_BASEFUNS(MatrixBiasSpec);
protected:
  SPEC_DEFAULTS;
private:
  void 	Initialize();
  void	Destroy()		{ };
  void	Defaults_init();
};

#ifdef __TA_COMPILE__
// get the inlines
#include <LeabraNetwork>
#endif

#endif // MatrixBiasSpec_h
