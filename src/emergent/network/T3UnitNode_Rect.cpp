// Copyright 2013-2017, Regents of the University of Colorado,
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

#include "T3UnitNode_Rect.h"
#include <iColor>

#ifdef TA_QT3D

#include <T3Cube>

T3UnitNode_Rect::T3UnitNode_Rect(Qt3DNode* parent, T3DataView* dataView_,
                                 float max_x, float max_y, float max_z,
				 float un_spc, float disp_sc)
  : inherited(parent, dataView_, max_x, max_y, max_z, un_spc, disp_sc )
  , cube(new T3Cube(this))
{
  float max_xy = MAX(max_x, max_y);
  float rad = disp_scale * ((.5f - spacing) / max_xy);
  cube->color_type = T3ColorEntity::TRANS;
  cube->setSize(disp_scale * ((1.0f - spacing) / max_x),
                0.0f,
                disp_scale * ((1.0f - spacing) / max_y));
}

T3UnitNode_Rect::~T3UnitNode_Rect() {
}

void T3UnitNode_Rect::setAppearance_impl(NetView* nv, float act, const iColor& clr,
                                         float max_z, bool act_invalid) 
{
  cube->setColor(clr, 0.2f, 0.02f, 150.0f);
}


#else // TA_QT3D

#include <Inventor/nodes/SoCube.h>

SO_NODE_SOURCE(T3UnitNode_Rect);

void T3UnitNode_Rect::initClass()
{
  SO_NODE_INIT_CLASS(T3UnitNode_Rect, T3UnitNode, "T3UnitNode");
}

T3UnitNode_Rect::T3UnitNode_Rect(T3DataView* dataView_, float max_x, float max_y, float max_z,
				 float un_spc, float disp_sc)
  : inherited(dataView_, max_x, max_y, max_z, un_spc, disp_sc)
{
  SO_NODE_CONSTRUCTOR(T3UnitNode_Rect);

  shape_ = new SoCube; // note: same shape is used for both styles
  shape_->width = disp_scale * ((1.0f - spacing) / max_x);
  shape_->depth = disp_scale * ((1.0f - spacing) / max_y);
  shape_->height = 0.01f;
  shapeSeparator()->addChild(shape_);

  // bake in the shape offset right into our origin offset, to avoid an additional txfm
//  SetOrigin(pos.x + 0.5f, pos.y + 0.5f, h/2.0f);
  // setAppearance(0.0f, iColor(.25f, .25f, .25f), 100.0, 0.0f); //dk gray
}

T3UnitNode_Rect::~T3UnitNode_Rect()
{
  shape_ = NULL;
}

#endif // TA_QT3D
