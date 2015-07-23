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
#include <iColor>

#ifdef TA_QT3D

#include <math.h>
#include <T3Cylinder>

T3UnitNode_Cylinder::T3UnitNode_Cylinder(Qt3DNode* parent, T3DataView* dataView_,
                                 float max_x, float max_y, float max_z,
				 float un_spc, float disp_sc)
  : inherited(parent, dataView_, max_x, max_y, max_z, un_spc, disp_sc )
  , cylinder(new T3Cylinder(this))
{
  float max_xy = MAX(max_x, max_y);
  float rad = disp_scale * ((.5f - spacing) / max_xy);
  cylinder->setGeom(T3Cylinder::LONG_Z, rad, 0.01f);
}

T3UnitNode_Cylinder::~T3UnitNode_Cylinder() {
}

void T3UnitNode_Cylinder::setAppearance_impl(NetView* nv, float act, const iColor& clr,
                                             float max_z, bool act_invalid) 
{
  float ht = (base_height + ((max_height - base_height) * fabs(act))) / max_z;
  cylinder->setColor(clr, .2f, 0.95f, 150.0f);
  cylinder->setLength(ht);

  float xp = translate->dx();
  float zp = translate->dz();
  float dy = ((max_height - base_height) * act * 0.5f) / max_z;
  // cylinder->Translate(xp, yp, dz);
  Translate(xp, dy, zp);
}

#else // TA_QT3D

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
  //setAppearance(0.0f, T3Color(.25f, .25f, .25f), 100.0, 0.0f); //dk gray
}

T3UnitNode_Cylinder::~T3UnitNode_Cylinder()
{
  shape_ = NULL;
}

void T3UnitNode_Cylinder::setAppearance_impl(NetView* nv, float act, const iColor& color,
                                             float max_z, bool act_invalid) 
{
  shape_->height = (base_height + ((max_height - base_height) * fabs(act))) / max_z;
  float dz; float x; float y;
  transform()->translation.getValue().getValue(x, dz, y);
  dz = (((max_height - base_height) * act) * 0.5f) / max_z;
  transform()->translation.setValue(x, dz, y);
  inherited::setAppearance_impl(nv, act, color, max_z, act_invalid);
}

#endif // TA_QT3D
