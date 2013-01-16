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

#ifndef Layer_PtrList_h
#define Layer_PtrList_h 1

// parent includes:
#include "network_def.h"
#include <taPtrList>

// member includes:

// declare all other types mentioned but not required to include:
class Layer; //

TypeDef_Of(Layer_PtrList);

class EMERGENT_API Layer_PtrList : public taPtrList<Layer> {
  // simple layer list used in lookaside lists
INHERITED(taPtrList<Layer>)
public:
  Layer_PtrList() {}
};

#endif // Layer_PtrList_h
