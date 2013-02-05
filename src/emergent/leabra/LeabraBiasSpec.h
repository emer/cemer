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

#ifndef LeabraBiasSpec_h
#define LeabraBiasSpec_h 1

// parent includes:
#include <LeabraConSpec>

// member includes:

// declare all other types mentioned but not required to include:
class LeabraCon; // 
class LeabraUnit; // 
class LeabraLayer; // 

TypeDef_Of(LeabraBiasSpec);

class LEABRA_API LeabraBiasSpec : public LeabraConSpec {
  // Leabra bias-weight connection specs (bias wts are a little bit special)
INHERITED(LeabraConSpec)
public:
  float		dwt_thresh;  // #DEF_0.1 #MIN_0 #CAT_Learning don't change if dwt < thresh, prevents buildup of small changes

  inline override void	B_Compute_dWt_LeabraCHL(LeabraCon* cn, LeabraUnit* ru);
  // #IGNORE
  inline override void	B_Compute_dWt_CtLeabraXCAL(LeabraCon* cn, LeabraUnit* ru,
						   LeabraLayer* rlay);
  // #IGNORE
  inline override void	B_Compute_dWt_CtLeabraCAL(LeabraCon* cn, LeabraUnit* ru,
						   LeabraLayer* rlay);
  // #IGNORE

  bool	CheckObjectType_impl(taBase* obj);

  TA_SIMPLE_BASEFUNS(LeabraBiasSpec);
protected:
  SPEC_DEFAULTS;
private:
  void 	Initialize();
  void	Destroy()		{ };
  void	Defaults_init();
};

TA_SMART_PTRS(LeabraBiasSpec)

#endif // LeabraBiasSpec_h
