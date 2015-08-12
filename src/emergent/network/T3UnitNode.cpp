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
#include <iColor>
#include <NetView>

#include <math.h>

#define T3_UNIT_BASE_H	0.1f
#define T3_UNIT_MAX_H	0.8f

#ifdef TA_QT3D

float T3UnitNode::base_height = 0.08f;
float T3UnitNode::max_height = 0.46f;

T3UnitNode::T3UnitNode(Qt3DNode* parent, T3DataView* dataView_,
                       float max_x, float max_y, float max_z,
                       float un_spc, float disp_sc)
  : inherited(parent, dataView_)
  , spacing(un_spc)
  , disp_scale(disp_sc)
{
}

T3UnitNode::~T3UnitNode() {
}

void T3UnitNode::setAppearance(NetView* nv, float act, const iColor& clr, float max_z) {
  bool act_invalid = false;
  if (taMath_float::isnan(act)) {
    act_invalid = true;
    act = 0.0f;
  }
  else if (act < -1.0f) act = -1.0f;
  else if (act > 1.0f) act = 1.0f;
  setAppearance_impl(nv, act, clr, max_z, act_invalid);
}

void T3UnitNode::setAppearance_impl(NetView* nv, float act, const iColor& clr,
  float max_z, bool act_invalid) 
{
  // happens all in derived
}



void T3UnitNode::setDefaultCaptionTransform() {
  // //note: this is the one for 3d objects -- 2d replace this
  // captionNode_->justification = SoAsciiText::CENTER;
  // //  transformCaption(SbVec3f(0.0f, 0.1f, 0.45f));
  // transformCaption(SbVec3f(0.0f, 0.0f, 0.0f));
}

void T3UnitNode::setPicked(bool value) {
  // SoSeparator* ss = shapeSeparator(); //cache
  // SoDrawStyle* ds = (SoDrawStyle*)getNodeByName(ss, "drawStyle");
  // if (value) { // picking
  //   if (ds) return; // already there
  //   ds = new SoDrawStyle();
  //   ds->setName("drawStyle");
  //   ds->style.setValue(SoDrawStyle::LINES);
  //   insertChildAfter(ss, ds, material());
  // }
  // else { // unpicking
  //   if (ds)
  //     ss->removeChild(ds);
  // }
}


#else // TA_QT3D

#include <T3Color>
#include <iVec3f>
#include <Inventor/nodes/SoBaseColor.h>
#include <Inventor/nodes/SoDrawStyle.h>
#include <Inventor/nodes/SoAsciiText.h>
#include <Inventor/nodes/SoMaterial.h>

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

void T3UnitNode::setAppearance(NetView* nv, float act, const iColor& color, float max_z) {
  bool act_invalid = false;
  if (taMath_float::isnan(act)) {
    act_invalid = true;
    act = 0.0f;
  }
  else if (act < -1.0f) act = -1.0f;
  else if (act > 1.0f) act = 1.0f;
  setAppearance_impl(nv, act, color, max_z, act_invalid);
}

void T3UnitNode::setAppearance_impl(NetView* nv, float act, const iColor& color,
                                    float max_z, bool act_invalid) 
{
  material()->diffuseColor = (SbColor)(T3Color)color;
  //  material()->specularColor = (SbColor)color;
  //  material()->emissiveColor = (SbColor)color;
  //  material()->ambientColor = (SbColor)color;
  float trans = nv->view_params.unit_trans;
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

#endif // TA_QT3D
