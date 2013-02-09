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

#ifndef LeabraTdUnit_h
#define LeabraTdUnit_h 1

// parent includes:
#include <LeabraUnit>

// member includes:

// declare all other types mentioned but not required to include:

eTypeDef_Of(LeabraTdUnit);

class E_API LeabraTdUnit : public LeabraUnit {
  // Leabra unit with dopamine-like modulation of minus phase activation for learning
INHERITED(LeabraUnit)
public:
  float 	p_act_m;	// previous minus phase activation 
  float		p_act_p;	// previous plus phase activation
  float		trace;		// the trace of activation states that is used for learning

  void	Copy_(const LeabraTdUnit& cp);
  TA_BASEFUNS(LeabraTdUnit);
private:
  void	Initialize();
  void	Destroy()	{ };
};

#endif // LeabraTdUnit_h
