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

#ifndef LeabraProject_h
#define LeabraProject_h 1

// parent includes:
#include "leabra_def.h"
#include <ProjectBase>

// member includes:

// declare all other types mentioned but not required to include:

TypeDef_Of(LeabraProject);

class LEABRA_API LeabraProject : public ProjectBase {
  // #STEM_BASE ##CAT_Leabra project for Leabra models
INHERITED(ProjectBase)
public:

  TA_BASEFUNS_NOCOPY(LeabraProject);
private:
  void	Initialize();
  void 	Destroy()		{}
};

#endif // LeabraProject_h
