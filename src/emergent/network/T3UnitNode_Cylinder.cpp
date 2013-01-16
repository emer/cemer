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

#include "T3UnitNode_Cylinder.h"
#include <T3Color>

#include <Inventor/nodes/SoCylinder.h>
#include <Inventor/nodes/SoTransform.h>

SO_NODE_SOURCE(T3UnitNode_Cylinder);

void T3UnitNode_Cylinder::initClass()
{
  SO_NODE_INIT_CLASS(T3UnitNode_Cylinder, T3UnitNode, "T3UnitNode");
}

T3UnitNode_Cylinder::T3UnitNode_Cylinder(T3DataView* dataView_, float max_x, float max_y,
					 float max_z, float un_spc, float disp_sc)
  : inherited(dataView_, max_x, max_y, max_z, un_spc, disp_sc)
{
  SO_NODE_CONSTRUCTOR(T3UnitNode_Cylinder);

  float max_xy = MAX(max_x, max_y);

  shape_ = new SoCylinder; // note: same shape is used for both styles
  shape_->radius = disp_scale * ((.5f - spacing) / max_xy);
  shapeSeparator()->addChild(shape_);

  // bake in the shape offset right into our origin offset, to avoid an additional txfm
//  SetOrigin(pos.x + 0.5f, pos.y + 0.5f, h/2.0f);
  setAppearance(0.0f, T3Color(.25f, .25f, .25f), 100.0, 0.0f); //dk gray
}

T3UnitNode_Cylinder::~T3UnitNode_Cylinder()
{
  shape_ = NULL;
}

void T3UnitNode_Cylinder::setAppearance_impl(float act, const T3Color& color, float max_z,
  float trans, bool act_invalid) 
{
  shape_->height = (base_height + ((max_height - base_height) * fabs(act))) / max_z;
  float dz; float x; float y;
  transform()->translation.getValue().getValue(x, dz, y);
  dz = (((max_height - base_height) * act) * 0.5f) / max_z;
  transform()->translation.setValue(x, dz, y);
  inherited::setAppearance_impl(act, color, max_z, trans, act_invalid);
}

