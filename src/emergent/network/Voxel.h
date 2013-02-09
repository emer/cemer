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

#ifndef Voxel_h
#define Voxel_h 1

// parent includes:
#include "network_def.h"
#include <taOBase>

// member includes:
#include <taVector3f>

// declare all other types mentioned but not required to include:

eTypeDef_Of(Voxel);

class E_API Voxel : public taOBase {
  // #NO_TOKENS #NO_UPDATE_AFTER A single voxel.
INHERITED(taOBase)
public:
  taVector3f    coord;  // #NO_SAVE Voxel coordinates.
  float         size;   // #NO_SAVE Size of the voxel.
  TA_BASEFUNS_SC(Voxel);
private:
  void  Initialize();
  void  Destroy()               { };
};

std::ostream & operator<<(std::ostream &os, const Voxel &voxel);

#endif // Voxel_h
