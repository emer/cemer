// Copyright 2015, Regents of the University of Colorado,
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

#ifndef T3PrjnNode_h
#define T3PrjnNode_h 1

// parent includes:
#include "network_def.h"
#include <T3NodeParent>

// member includes:

// declare all other types mentioned but not required to include:

#ifdef TA_QT3D

#include <T3LineStrip>

class E_API T3PrjnNode: public T3NodeParent {
  Q_OBJECT
  INHERITED(T3NodeParent)
public:
  bool			projected;
  bool                  mode_2d;
  T3LineStrip*          line;

  void  SetEndPoint(float xp, float yp, float zp);
  // set the end point and draw the arrow..
  
  T3PrjnNode(Qt3DNode* par = NULL, T3DataView* dataView_ = NULL,
             bool projted = true, float rad = .01f, bool mode_2d = false);
  ~T3PrjnNode();
};


#else // TA_QT3D

class SoTransform; // #IGNORE
class SoComplexity; // #IGNORE
class SoCone; // #IGNORE
class SoCylinder; // #IGNORE
class SoMaterial; // #IGNORE


class E_API T3PrjnNode: public T3NodeParent {
#ifndef __MAKETA__
typedef T3NodeParent inherited;
  SO_NODE_HEADER(T3PrjnNode);
#endif
public:
  static void		initClass();

  void		setEndPoint(const SbVec3f& value); // #IGNORE sets endpoint, relative to its origin
  void		setArrowColor(const SbColor& clr, float transp);
  // #IGNORE set arrow color

  T3PrjnNode(T3DataView* dataView_ = NULL, bool projected = true, float rad = .01f);

protected:
  bool			projected_;
  float			radius;
  SoComplexity*		complexity;
  SoTransform*		trln_prjn; // #IGNORE
  SoTransform*		rot_prjn; // #IGNORE
  SoTransform*		trln_arr; // #IGNORE
  SoCone*		arr_prjn;  // #IGNORE arrow head
  SoCylinder*		line_prjn;  // #IGNORE line
  SoMaterial*		arr_mat;  // #IGNORE arrow material (color)

  ~T3PrjnNode();
private:
  void			init();
};

#endif // TA_QT3D

#endif // T3PrjnNode_h
