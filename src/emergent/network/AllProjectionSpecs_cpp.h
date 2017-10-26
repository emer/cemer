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

#ifndef AllProjectionSpecs_cpp_h
#define AllProjectionSpecs_cpp_h 1

// add all projection specs to this file!

// parent includes:
#include <ProjectionSpec_cpp>

#include <State_cpp>

class FullPrjnSpec_cpp : public ProjectionSpec_cpp {
  // Full connectivity between layers
INHERITED(ProjectionSpec)
public:

#include <FullPrjnSpec>

  FullPrjnSpec_cpp() { Initialize_core(); }

};



#endif // AllProjectionSpecs_cpp_h