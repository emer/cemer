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

#include "T3GraphBar.h"

#ifdef TA_QT3D

#include <iColor>

T3GraphBar::T3GraphBar(Qt3DNode* parent, T3DataView* dataView_, bool zon)
  : inherited(parent, dataView_)
  , z_on(zon)
  , cube(new T3Cube(this))
{
  
}

T3GraphBar::~T3GraphBar() {
}

void T3GraphBar::SetBar(iVec3f& pos, iVec3f& sz, const iColor& clr) {
  cube->setColor(clr);
  if(z_on) {
    cube->Translate(-0.5f + pos.x, -0.5f + pos.y+0.5f*sz.y, -1.0f + pos.z+0.5f*sz.z);
  }
  else {
    cube->Translate(-0.5f + pos.x, -0.5f + pos.y+0.5f*sz.y, pos.z+0.5f*sz.z);
  }
  cube->setSize(sz);
}

#else // TA_QT3D

#include <T3Color>
#include <Inventor/nodes/SoBaseColor.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/nodes/SoCube.h>
#include <Inventor/nodes/SoCylinder.h>


SO_NODE_SOURCE(T3GraphBar);

void T3GraphBar::initClass()
{
  SO_NODE_INIT_CLASS(T3GraphBar, T3NodeLeaf, "T3NodeLeaf");
}

T3GraphBar::T3GraphBar(T3DataView* dataView_)
  : inherited(dataView_)
{
  SO_NODE_CONSTRUCTOR(T3GraphBar);
}

T3GraphBar::~T3GraphBar() {
}

void T3GraphBar::SetBar(iVec3f& pos, iVec3f& size, const T3Color& color) {
  SoMaterial* mat = material();
  mat->diffuseColor.setValue(color.r, color.g, color.b);
  // mat->transparency.setValue(1.0f - color.a);
  SoTransform* tx = transform();
  tx->translation.setValue(pos.x, pos.y+0.5f*size.y, pos.z+0.5f*size.z);
  SoCube* sp = new SoCube;
  sp->width = size.x;
  sp->depth = size.z;
  sp->height = size.y;
  shapeSeparator()->addChild(sp);
}

#endif // TA_QT3D
