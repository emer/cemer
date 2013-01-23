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

#ifndef T3TransformBoxRotatorRotator_h
#define T3TransformBoxRotatorRotator_h 1

// parent includes:
#include "ta_def.h"
#ifndef __MAKETA__
#include <Inventor/nodes/SoSeparator.h>
#endif

// member includes:

// declare all other types mentioned but not required to include:
TypeDef_Of(T3TransformBoxRotatorRotator);

class TA_API T3TransformBoxRotatorRotator: public SoSeparator { 
  // ##NO_INSTANCE ##NO_TOKENS ##NO_CSS big version of the transformBoxRotatorRotator
#ifndef __MAKETA__
typedef SoSeparator inherited;
  SO_NODE_HEADER(T3TransformBoxRotatorRotator);
#endif // def __MAKETA__
public:
  static void		initClass();
  T3TransformBoxRotatorRotator(bool active = false, float half_size=1.1f, float line_width=.4f);
};

#endif // T3TransformBoxRotatorRotator_h
