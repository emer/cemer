// Copyright, 1995-2005, Regents of the University of Colorado,
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

#ifndef IMISC_SO_H
#define IMISC_SO_H

#include "taiqtso_def.h"
#include "igeometry.h"

/* T3RenderHelper
 * This class is used to help rendering, since you almost always require to issue a series
 * of translations when drawing objects -- this helper lets you think and render in terms
 * of the absolute coordinate of an element with respect to the starting frame.
 *
 * Example usage:

 T3RenderHelper rh(par_so);
 SoNode* nd1 = //create some shape node
 rh.render(nd1);
 // now draw a second instance, 2 units right
 rh.render(nd1, 2, 0, 0);

*/

class TAIQTSO_API T3RenderHelper { // #IGNORE class used to help rendering -- issues the translation transforms
public:
  iVec3f	cur; // current position (initially 0,0,0)
  SoGroup*	par; // default parent
  void		moveTo(float x, float y, float z); // move to (maybe) new position
  void		render(SoNode* node); // render at current point (no translation)
  void		render(SoNode* node, float x, float y, float z); // render at new position
  void 		setOrigin(float x, float y, float z); // used to (re)set origin -- doesn't issue a transform
  void		setParent(SoGroup* par_)  {par = par_;}
  T3RenderHelper(SoGroup* par_ = NULL) {par = par_;}
protected:
  void		checkIssueTransform(float x, float y, float z);
};


#endif
