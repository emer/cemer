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

#include "T3UnitNode.h"
#include <T3Color>
#include <iVec3f>

#include <Inventor/nodes/SoBaseColor.h>
#include <Inventor/nodes/SoDrawStyle.h>
#include <Inventor/nodes/SoAsciiText.h>
#include <Inventor/nodes/SoMaterial.h>



#define T3_UNIT_BASE_H	0.1f
#define T3_UNIT_MAX_H	0.8f

SO_NODE_ABSTRACT_SOURCE(T3UnitNode);

float T3UnitNode::base_height = 0.08f;
float T3UnitNode::max_height = 0.46f;

void T3UnitNode::initClass()
{
  SO_NODE_INIT_ABSTRACT_CLASS(T3UnitNode, T3NodeLeaf, "T3NodeLeaf");
}

T3UnitNode::T3UnitNode(T3DataView* dataView_, float max_x, float max_y, float max_z,
		       float un_spc, float disp_sc)
  : inherited(dataView_)
{
  SO_NODE_CONSTRUCTOR(T3UnitNode);
  spacing = un_spc;
  disp_scale = disp_sc;
}

T3UnitNode::~T3UnitNode()
{
}

void T3UnitNode::setAppearance(float act, const T3Color& color, float max_z, float trans) {
  bool act_invalid = false;
  if (isnan(act) || isinf(act)) {
    act_invalid = true;
    act = 0.0f;
  }
  else if (act < -1.0f) act = -1.0f;
  else if (act > 1.0f) act = 1.0f;
  setAppearance_impl(act, color, max_z, trans, act_invalid);
}

void T3UnitNode::setAppearance_impl(float act, const T3Color& color,
  float max_z, float trans, bool act_invalid) 
{
  material()->diffuseColor = (SbColor)color;
  //  material()->specularColor = (SbColor)color;
  //  material()->emissiveColor = (SbColor)color;
  //  material()->ambientColor = (SbColor)color;
  material()->transparency = (1.0f - fabsf(act)) * trans;
}



void T3UnitNode::setDefaultCaptionTransform() {
  //note: this is the one for 3d objects -- 2d replace this
  captionNode_->justification = SoAsciiText::CENTER;
  //  transformCaption(SbVec3f(0.0f, 0.1f, 0.45f));
  transformCaption(SbVec3f(0.0f, 0.0f, 0.0f));
}

void T3UnitNode::setPicked(bool value) {
  SoSeparator* ss = shapeSeparator(); //cache
  SoDrawStyle* ds = (SoDrawStyle*)getNodeByName(ss, "drawStyle");
  if (value) { // picking
    if (ds) return; // already there
    ds = new SoDrawStyle();
    ds->setName("drawStyle");
    ds->style.setValue(SoDrawStyle::LINES);
    insertChildAfter(ss, ds, material());
  }
  else { // unpicking
    if (ds)
      ss->removeChild(ds);
  }
}

