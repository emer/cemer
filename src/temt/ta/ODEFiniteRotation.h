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

#ifndef ODEFiniteRotation_h
#define ODEFiniteRotation_h 1

// parent includes:
#include <taOBase>

// member includes:
#include <taVector3f>

// declare all other types mentioned but not required to include:


class TA_API ODEFiniteRotation : public taOBase {
  // ##INLINE ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_VirtEnv finite rotation mode settings
INHERITED(taOBase)
public:
  bool          on;             // #DEF_false whether to enable finite rotation mode -- False: An "infinitesimal" orientation update is used. This is fast to compute, but it can occasionally cause inaccuracies for bodies that are rotating at high speed, especially when those bodies are joined to other bodies. True: A finite orientation update is used. This is more costly to compute, but will be more accurate for high speed rotations. Note however that high speed rotations can result in many types of error in a simulation, and this mode will only fix one of those sources of error.
  taVector3f  axis; // #CONDSHOW_ON_on This sets the finite rotation axis for a body, if finite rotation mode is being used. If this axis is zero (0,0,0), full finite rotations are performed on the body. If this axis is nonzero, the body is rotated by performing a partial finite rotation along the axis direction followed by an infinitesimal rotation along an orthogonal direction. This can be useful to alleviate certain sources of error caused by quickly spinning bodies. For example, if a car wheel is rotating at high speed you can set the wheel's hinge axis here to try and improve its behavior.

  TA_SIMPLE_BASEFUNS(ODEFiniteRotation);
private:
  void  Initialize();
  void  Destroy()       { };
};

#endif // ODEFiniteRotation_h
