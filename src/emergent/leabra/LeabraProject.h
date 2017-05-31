// Copyright 2017, Regents of the University of Colorado,
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
#include "network_def.h"
#include <ProjectBase>

// member includes:

// declare all other types mentioned but not required to include:

eTypeDef_Of(LeabraProject);

class E_API LeabraProject : public ProjectBase {
  // #STEM_BASE ##CAT_Leabra project for Leabra models
INHERITED(ProjectBase)
public:

  void  CopyToBackprop();
  // #BUTTON #CONFIRM copy this project to a backprop version of the project -- IMPORTANT: usually a good idea to turn Network auto_build = NO_BUILD before proceeding -- this works by replacing all the "Leabra" strings to "Bp" strings in an in-memory version of the project file, and then opening that project -- you will generally have to replace the Trial-level program with a Bp appropriate one, and things like monitors will likely need to be updated, recurrent connections removed, etc.
  
  TA_BASEFUNS_NOCOPY(LeabraProject);
private:
  void	Initialize();
  void 	Destroy()		{}
};

#endif // LeabraProject_h
