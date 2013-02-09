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

#ifndef SoRect_h
#define SoRect_h 1

// parent includes:
#include "ta_def.h"
#ifndef __MAKETA__
#include <Inventor/nodes/SoTriangleStripSet.h>

// member includes:
#include <Inventor/fields/SoMFVec3f.h>
#endif

// declare all other types mentioned but not required to include:

taTypeDef_Of(SoRect);

class TA_API SoRect: public SoTriangleStripSet {
  // ##NO_INSTANCE ##NO_TOKENS ##NO_CSS  2d rectangle, primarily for images, table images, etc.
#ifndef __MAKETA__
typedef SoTriangleStripSet inherited;

  SO_NODE_HEADER(SoRect);
#endif // def __MAKETA__
public:
  static void		initClass();

  float		width;
  float		height;

#ifndef __MAKETA__
  SoMFVec3f& 	vertex(); //  #IGNORE accessor shortcut for vertices
  SoMFVec3f& 	normal(); // #IGNORE accessor shortcut for normals
#endif

  void		setDimensions(float wd, float ht);
  SoRect();

protected:
  const char*  	getFileFormatName() const {return "TriangleStripSet"; } 
  void 		render(); // #IGNORE
  void 		renderH(); // #IGNORE
  void 		renderV(); // #IGNORE
};

#endif // SoRect_h
