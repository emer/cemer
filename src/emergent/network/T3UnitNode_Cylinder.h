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

#ifndef T3UnitNode_Cylinder_h
#define T3UnitNode_Cylinder_h 1

// parent includes:
#include <T3UnitNode>

// member includes:

// declare all other types mentioned but not required to include:

class EMERGENT_API T3UnitNode_Cylinder: public T3UnitNode { // 2d color
typedef T3UnitNode inherited;
  SO_NODE_HEADER(T3UnitNode_Cylinder);
friend class T3UnitGroupNode;
public:
  static void	initClass();

  T3UnitNode_Cylinder(T3DataView* dataView_ = NULL, float max_x = 1.0f, float max_y = 1.0f,
		      float max_z = 1.0f, float un_spc = .01f, float disp_sc = 1.0f);

protected:
  override void	 setAppearance_impl(float act, const T3Color& color,
    float max_z, float trans, bool act_invalid);
  ~T3UnitNode_Cylinder();
private:
  SoCylinder*		shape_; //#IGNORE
};

#endif // T3UnitNode_Cylinder_h
