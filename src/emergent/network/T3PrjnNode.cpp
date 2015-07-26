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

#include "T3PrjnNode.h"

#ifdef TA_QT3D

T3PrjnNode::T3PrjnNode(Qt3DNode* parent, T3DataView* dataView_, bool prjcted, float rad,
                       bool md2d)
  : inherited(parent, dataView_)
  , projected(prjcted)
  , mode_2d(md2d)
  , line(new T3LineStrip(this))
{
  line->line_width = rad;

  //     2
  // 0---1 3
  //     4
  
  line->moveTo(0.0f, 0.0f, 0.0f);
  int p1idx = line->pointCount();
  line->lineTo(1.0f, 1.0f, 1.0f);  
  line->lineTo(1.0f, 1.0f, 1.0f);
  line->lineTo(1.0f, 1.0f, 1.0f);
  line->lineTo(1.0f, 1.0f, 1.0f);
  line->lineToIdx(p1idx); // 5 = 1
  
  if(mode_2d) {
    line->Translate(-0.5f, 0.0f, 0.5f);
  }
  else {
    line->TranslateLLFSz1To(QVector3D(0.0f, 0.0f, 0.5f));
  }
}

T3PrjnNode::~T3PrjnNode() {
}

void T3PrjnNode::SetEndPoint(float xp, float yp, float zp) {
  //     2
  // 0---1 3
  //     4

  line->setNodeUpdating(true);
  
  const float arrow_prop = .02f;
  const float apm1 = 1.0f - arrow_prop;
  
  taVector3f pos3(xp, yp, zp);
  taVector3f pos1 = pos3 * apm1;
  taVector3f pos2 = pos1;
  pos2.y += xp * arrow_prop;
  pos2.x -= yp * arrow_prop;
  taVector3f pos4 = pos1;
  pos4.y -= xp * arrow_prop;
  pos4.x += yp * arrow_prop;

  line->setPoint(1, pos1);
  line->setPoint(2, pos2);
  line->setPoint(3, pos3);
  line->setPoint(4, pos4);

  line->setNodeUpdating(false);
}

#else // TA_QT3D

#include <Inventor/nodes/SoCylinder.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoComplexity.h>
#include <Inventor/nodes/SoCone.h>


SO_NODE_SOURCE(T3PrjnNode);

void T3PrjnNode::initClass()
{
  SO_NODE_INIT_CLASS(T3PrjnNode, T3NodeParent, "T3NodeParent");
}

T3PrjnNode::T3PrjnNode(T3DataView* dataView_, bool proj, float rad)
  : inherited(dataView_)
{
  SO_NODE_CONSTRUCTOR(T3PrjnNode);
  projected_ = proj;
  radius = rad;
  init();
}

T3PrjnNode::~T3PrjnNode()
{
  trln_prjn = NULL;
  rot_prjn = NULL;
  trln_arr = NULL;
  arr_prjn = NULL;
  line_prjn = NULL;
}

void T3PrjnNode::init() {
  SoSeparator* ss = shapeSeparator();

  complexity = new SoComplexity;
  //  complexit->type = SoComplexity::SCREEN_SPACE;
  complexity->value.setValue(.1f); // don't need much..
  ss->addChild(complexity);

  rot_prjn = new SoTransform();
  ss->addChild(rot_prjn);
  trln_prjn = new SoTransform();
  ss->addChild(trln_prjn);

  line_prjn = new SoCylinder();
  line_prjn->radius = radius; //height is variable, depends on length
  ss->addChild(line_prjn);

  trln_arr = new SoTransform();
  ss->addChild(trln_arr);

  arr_mat = new SoMaterial;
  // overwritten in qtso..
  arr_mat->diffuseColor.setValue(SbColor(1.0f, .8f, 0.0f)); // orange
  if(projected_)
    arr_mat->transparency.setValue(.5f);
  else
    arr_mat->transparency.setValue(.8f);
  ss->addChild(arr_mat);

  arr_prjn = new SoCone();
  arr_prjn->height =  6.0f * radius;
  arr_prjn->bottomRadius = 4.0f * radius;
  ss->addChild(arr_prjn);
}

void T3PrjnNode::setEndPoint(const SbVec3f& ep) {
  float len = ep.length() - 6.0f * radius;
  if(len < radius) len = radius;

  float hlf_len = .5f *len;

  line_prjn->height = len;
  // txfm
  rot_prjn->rotation.setValue(SbRotation(SbVec3f(0, 1.0f, 0), ep));
  trln_prjn->translation.setValue(0.0f, hlf_len, 0.0f);
  trln_arr->translation.setValue(0.0f, hlf_len + 3.0f * radius, 0.0f);
  //note: already txlted by 1/2 height -- adds arrow len offset
}

void T3PrjnNode::setArrowColor(const SbColor& clr, float transp) {
  arr_mat->diffuseColor.setValue(clr);
  arr_mat->transparency.setValue(transp);
}

#endif // TA_QT3D
