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

#ifndef LHbRMTgLayerSpec_h
#define LHbRMTgLayerSpec_h 1

// parent includes:
#include <LeabraLayerSpec>

// member includes:

// declare all other types mentioned but not required to include:

eTypeDef_Of(LHbRMTgLayerSpec);

class E_API LHbRMTgLayerSpec : public LeabraLayerSpec {
  // lateral habenula and RMTg layers -- computes dopamine dip signals, based on differences between ventral striatal inputs
INHERITED(LeabraLayerSpec)
public:

  TA_SIMPLE_BASEFUNS(LHbRMTgLayerSpec);
private:
  void  Initialize() { };
  void  Destroy()     { };
};

#endif // LHbRMTgLayerSpec_h
