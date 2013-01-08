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

#ifndef VECameraDists_h
#define VECameraDists_h 1

// parent includes:
#include <taOBase>

// member includes:

// declare all other types mentioned but not required to include:


class TA_API VECameraDists : public taOBase {
  // ##INLINE ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_VirtEnv virtual env camera distances
INHERITED(taOBase)
public:
  float         near;           // #DEF_0.1 near distance of camera -- closest things can be seen
  float         focal;          // focal distance of camera -- where is it focused on in scene?
  float         far;            // far distance of camera -- furthest things that can be seen

  TA_SIMPLE_BASEFUNS(VECameraDists);
private:
  void  Initialize();
  void  Destroy()       { };
};

#endif // VECameraDists_h
