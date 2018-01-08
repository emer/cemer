// Copyright 2013-2017, Regents of the University of Colorado,
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

#ifndef VETexture_List_h
#define VETexture_List_h 1

// parent includes:
#include <VETexture>
#include <taList>

// member includes:

// declare all other types mentioned but not required to include:


eTypeDef_Of(VETexture_List);

class E_API VETexture_List : public taList<VETexture> {
  // ##CAT_VirtEnv a list of virtual environment textures
INHERITED(taList<VETexture>)
public:
  TA_BASEFUNS_NOCOPY(VETexture_List);
private:
  void  Initialize()            { SetBaseType(&TA_VETexture); }
  void  Destroy()               { };
};

#endif // VETexture_List_h
