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

#ifndef LeabraUnit_Group_h
#define LeabraUnit_Group_h 1

// parent includes:
#include "network_def.h"
#include <Unit_Group>

// member includes:

// declare all other types mentioned but not required to include:

eTypeDef_Of(LeabraUnit_Group);

class E_API LeabraUnit_Group : public Unit_Group {
  // #STEM_BASE ##CAT_Leabra for independent subgroups of competing units within a single layer -- optional data structure given use of virt_groups
INHERITED(Unit_Group)
public:

  TA_BASEFUNS_NOCOPY(LeabraUnit_Group);
private:
  void	Initialize()	{ };
  void	Destroy()	{ };
};

#endif // LeabraUnit_Group_h
