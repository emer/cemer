// Co2018ght 2013-2017, Regents of the University of Colorado,
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
 
#ifndef LeabraPrjn_h
#define LeabraPrjn_h 1

// parent includes:
#include "network_def.h"
#include <Projection>

#include <State_main>

// member includes:
#include <LeabraAvgMax>

// declare all other types mentioned but not required to include:
class LeabraNetwork; //
class LeabraPrjnState_cpp; //
class LeabraLayerState_cpp; //
class LeabraConSpec_cpp; //

eTypeDef_Of(LeabraPrjn);

class E_API LeabraPrjn: public Projection {
  // #STEM_BASE ##CAT_Leabra leabra specific projection -- has special variables at the projection-level
INHERITED(Projection)
public:

#include <LeabraPrjnState_core>
  
  TA_BASEFUNS(LeabraPrjn);
  SIMPLE_COPY(LeabraPrjn);
private:
  void 	Initialize();
  void 	Destroy();
};

#endif // LeabraPrjn_h
