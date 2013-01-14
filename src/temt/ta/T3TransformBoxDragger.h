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

#ifndef T3TransformBoxDragger_h
#define T3TransformBoxDragger_h 1

// parent includes:
#include "ta_def.h"
#include <Inventor/nodes/SoSeparator.h>

// member includes:

// declare all other types mentioned but not required to include:
class SoTransformBoxDragger;
class SoTransform;
class SoCalculator;

class TA_API T3TransformBoxDragger: public SoSeparator { 
  // ##NO_INSTANCE ##NO_TOKENS ##NO_CSS my custom transform box dragger kit for positioning T3 view guys
#ifndef __MAKETA__
typedef SoSeparator inherited;
  SO_NODE_HEADER(T3TransformBoxDragger);
#endif // def __MAKETA__
public:
  static void		initClass();
  T3TransformBoxDragger(float half_size=1.1f, float cube_size=0.6f, float line_width=0.4f);

  // elements within me:
  SoTransform*		xf_;		// transform -- set as you need
  SoTransformBoxDragger* dragger_;	// dragger
  SoCalculator*		trans_calc_;    // translation calculator -- gets from translation of dragger
protected:
  const char*  	getFileFormatName() const {return "Separator"; } 
};

#endif // T3TransformBoxDragger_h
