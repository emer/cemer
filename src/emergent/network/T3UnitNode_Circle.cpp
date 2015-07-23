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

#include "T3UnitNode_Circle.h"
#include <iColor>

#ifdef TA_QT3D

#include <T3Cylinder>

T3UnitNode_Circle::T3UnitNode_Circle(Qt3DNode* parent, T3DataView* dataView_,
                                 float max_x, float max_y, float max_z,
				 float un_spc, float disp_sc)
  : inherited(parent, dataView_, max_x, max_y, max_z, un_spc, disp_sc )
  , cylinder(new T3Cylinder(this))
{
  float max_xy = MAX(max_x, max_y);
  float rad = disp_scale * ((.5f - spacing) / max_xy);
  cylinder->setGeom(T3Cylinder::LONG_Z, rad, 0.01f);
}

T3UnitNode_Circle::~T3UnitNode_Circle() {
}

void T3UnitNode_Circle::setAppearance_impl(NetView* nv, float act, const iColor& clr,
                                           float max_z, bool act_invalid) 
{
  cylinder->setColor(clr, .2f, 0.95f, 150.0f);
}

#else // TA_QT3D

#include <Inventor/nodes/SoCylinder.h>

SO_NODE_SOURCE(T3UnitNode_Circle);

void T3UnitNode_Circle::initClass()
{
  SO_NODE_INIT_CLASS(T3UnitNode_Circle, T3UnitNode, "T3UnitNode");
}

T3UnitNode_Circle::T3UnitNode_Circle(T3DataView* dataView_, float max_x, float max_y,
				     float max_z, float un_spc, float disp_sc)
  : inherited(dataView_, max_x, max_y, max_z, un_spc, disp_sc)
{
  SO_NODE_CONSTRUCTOR(T3UnitNode_Circle);

  float max_xy = MAX(max_x, max_y);

  shape_ = new SoCylinder; // note: same shape is used for both styles
  shape_->radius = disp_scale * ((.5f - spacing) / max_xy); // always the same radius
  shape_->height = 0.01f;
  shapeSeparator()->addChild(shape_);

  // bake in the shape offset right into our origin offset, to avoid an additional txfm
//  SetOrigin(pos.x + 0.5f, pos.y + 0.5f, h/2.0f);
  // setAppearance(0.0f, T3Color(.25f, .25f, .25f), 100.0, 0.0f); //dk gray
}

T3UnitNode_Circle::~T3UnitNode_Circle()
{
  shape_ = NULL;
}

/*void T3UnitNode_Circle::setDefaultCaptionTransform() {
  //note: this is the one for 3d objects -- 2d replace this
  captionNode_->justification = SoAsciiText::CENTER;
  transformCaption(SbRotation(SbVec3f(1, 0, 0), (.5 * PI)));
} */

#endif // TA_QT3D
