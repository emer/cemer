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

#ifndef SoFrame_h
#define SoFrame_h 1

// parent includes:
#include "ta_def.h"
#ifndef __MAKETA__
#include <Inventor/nodes/SoTriangleStripSet.h>

// member includes:
#include <Inventor/fields/SoMFVec3f.h>
#endif

// declare all other types mentioned but not required to include:

TypeDef_Of(SoFrame);

class TA_API SoFrame: public SoTriangleStripSet {
  // ##NO_INSTANCE ##NO_TOKENS ##NO_CSS quadraloidal frame
#ifndef __MAKETA__
typedef SoTriangleStripSet inherited;

  SO_NODE_HEADER(SoFrame);
#endif // def __MAKETA__
public:
  enum Orientation {
    Hor,	// lies flat in x-z plane
    Ver		// lies flat in x-y plan
  };

  static void		initClass();

  float		base; //note: these could be fields
  float		height;
  float		depth;
  float		inset; // #DEF_0.05
  Orientation	orientation; // #DEF_Hor

#ifndef __MAKETA__
  SoMFVec3f& 	vertex(); //  #IGNORE accessor shortcut for vertices
  SoMFVec3f& 	normal(); // #IGNORE accessor shortcut for normals
#endif

  void		setDimensions(float bs, float ht, float dp); // use existing inset
  void		setDimensions(float bs, float ht, float dp, float in);
  void		setInset(float value);
  void		setOrientation(Orientation ori);
  SoFrame(Orientation ori = Hor, float in = 0.05f);

protected:
  const char*  	getFileFormatName() const {return "TriangleStripSet"; } 
  void 		render(); // #IGNORE
  void 		renderH(); // #IGNORE
  void 		renderV(); // #IGNORE
};

#endif // SoFrame_h
