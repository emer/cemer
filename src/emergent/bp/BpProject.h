// Copyright 2017-2018, Regents of the University of Colorado,
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

#ifndef BpProject_h
#define BpProject_h 1

// parent includes:
#include <ProjectBase>

// member includes:

// declare all other types mentioned but not required to include:

eTypeDef_Of(BpProject);

class E_API BpProject : public ProjectBase {
  // #STEM_BASE ##CAT_Bp project for backpropagation networks
INHERITED(ProjectBase)
public:

  TA_BASEFUNS_NOCOPY(BpProject);
private:
  void  Initialize();
  void  Destroy()               {}
};

#endif // BpProject_h
