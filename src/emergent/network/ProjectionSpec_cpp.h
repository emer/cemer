// Co2018ght 2017-22018 Regents of the University of Colorado,
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

#ifndef ProjectionSpec_cpp_h
#define ProjectionSpec_cpp_h 1

// parent includes:
#include <NetworkState_cpp>
#include <UnitState_cpp>

#include <State_cpp>

#include <ProjectionSpec_mbrs>

class E_API ProjectionSpec_cpp : public BaseSpec_cpp {
  // #STEM_BASE #VIRT_BASE ##CAT_Projection Specifies the connectivity between layers (ie. full vs. partial)
INHERITED(BaseSpec)
public:

#include <ProjectionSpec_core>

  ProjectionSpec_cpp() { Initialize_core_base(); }
};

#endif // ProjectionSpec_cpp_h
