// Copyright, 1995-2013, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of The Emergent Toolkit
//
//   This library is free software; you can redistribute it and/or
//   modify it under the terms of the GNU Lesser General Public
//   License as published by the Free Software Foundation; either
//   version 2.1 of the License, or (at your option) any later version.
//
//   This library is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//   Lesser General Public License for more details.

#ifndef VESurface_h
#define VESurface_h 1

// parent includes:
#include <taOBase>

// member includes:

// declare all other types mentioned but not required to include:


TypeDef_Of(VESurface);

class TA_API VESurface : public taOBase {
  // ##INLINE ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_VirtEnv surface properties for collisions
INHERITED(taOBase)
public:

  float         friction;       // (0-1e22) coulomb friction coefficient (mu). 0 = frictionless, 1e22 = infinity = no slipping
  float         bounce;         // (0-1) how bouncy is the surface (0 = hard, 1 = maximum bouncyness)
  float         bounce_vel;     // minimum incoming velocity necessary for bounce -- incoming velocities below this will have a bounce parameter of 0

  TA_SIMPLE_BASEFUNS(VESurface);
private:
  void  Initialize();
  void  Destroy()       { };
};

#endif // VESurface_h
