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

#ifndef NetViewObj_Group_h
#define NetViewObj_Group_h 1

// parent includes:
#include <taGroup>
#include <NetViewObj>

// member includes:

// declare all other types mentioned but not required to include:

TypeDef_Of(NetViewObj_Group);

class EMERGENT_API NetViewObj_Group : public taGroup<NetViewObj> {
  // ##CAT_Network a group of network view objects
INHERITED(taGroup<NetViewObj>)
public:
  TA_BASEFUNS_NOCOPY(NetViewObj_Group);
private:
  void  Initialize()            { SetBaseType(&TA_NetViewObj); }
  void  Destroy()               { };
};

#endif // NetViewObj_Group_h
