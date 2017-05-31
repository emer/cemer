// Copyright 2017, Regents of the University of Colorado,
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

#ifndef VELightParams_h
#define VELightParams_h 1

// parent includes:
#include <taOBase>
#include "network_def.h"

// member includes:
#include <taColor>

// declare all other types mentioned but not required to include:


eTypeDef_Of(VELightParams);

class E_API VELightParams : public taOBase {
  // ##INLINE ##NO_TOKENS ##CAT_VirtEnv virtual env light parameters
INHERITED(taOBase)
public:
  bool          on;             // is the light turned on?
  float         intensity;      // #CONDSHOW_ON_on (0-1) how bright is the light
  taColor       color;          // #CONDSHOW_ON_on color of light

  TA_SIMPLE_BASEFUNS(VELightParams);
private:
  void  Initialize();
  void  Destroy()       { };
};

#endif // VELightParams_h
