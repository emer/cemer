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

#ifndef Voxel_List_h
#define Voxel_List_h 1

// parent includes:
#include "network_def.h"
#include <taList>
#include <Voxel>

// member includes:

// declare all other types mentioned but not required to include:
class Voxel;

eTypeDef_Of(Voxel_List);

class E_API Voxel_List : public taList<Voxel> {
  // #NO_TOKENS #NO_UPDATE_AFTER List of voxels
  INHERITED(taList<Voxel>)
public:
  TA_BASEFUNS_SC(Voxel_List);
private:
  void  Initialize()            { SetBaseType(&TA_Voxel); }
  void  Destroy()               { };
};

#endif // Voxel_List_h
