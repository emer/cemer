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

#ifndef SoLineBox3d_h
#define SoLineBox3d_h 1

// parent includes:
#include "ta_def.h"
#include <Inventor/nodes/SoIndexedLineSet.h>

// member includes:

// declare all other types mentioned but not required to include:

class TA_API SoLineBox3d : public SoIndexedLineSet {
  // ##NO_INSTANCE ##NO_TOKENS ##NO_CSS a 3d box draw with lines, starting at 0,0,0 and going to 1,-1,1 (note: uses TA Y coords, not GL's)
#ifndef __MAKETA__
typedef SoIndexedLineSet inherited;

 SO_NODE_HEADER(SoLineBox3d);
#endif // def __MAKETA__
public:
  static void		initClass();

  bool		center;		// if true, location is at center, else at lower-left-corner
  float		width;		// x
  float		height;		// y
  float		depth;		// z

  void 		render();

  SoLineBox3d(float wd = 1.0f, float ht = 1.0f, float dp = 1.0f, bool ctr = true);
protected:
  const char*  	getFileFormatName() const {return "IndexedLineSet"; } 

};

#endif // SoLineBox3d_h
