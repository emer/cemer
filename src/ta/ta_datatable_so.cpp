// Copyright, 1995-2005, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of TA/PDP++
//
//   TA/PDP++ is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   TA/PDP++ is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.


#include "ta_datatable_so.h"

//incl the coin header to get gl.h #include <GL/gl.h>
#include <Inventor/system/gl.h>
#include <Inventor/SbLinear.h>
#include <Inventor/SoDB.h>

#include <Inventor/fields/SoSFVec3f.h>
#include <Inventor/actions/SoAction.h>
#include <Inventor/actions/SoGLRenderAction.h>
#include <Inventor/nodes/SoAsciiText.h>
#include <Inventor/nodes/SoBaseColor.h>
#include <Inventor/nodes/SoCallback.h>
#include <Inventor/nodes/SoCube.h>
#include <Inventor/nodes/SoCylinder.h>
//#include <Inventor/nodes/SoDirectionalLight.h>
#include <Inventor/nodes/SoDrawStyle.h>
#include <Inventor/nodes/SoFont.h>
#include <Inventor/nodes/SoMaterial.h>
//#include <Inventor/nodes/SoPerspectiveCamera.h>
//#include <Inventor/nodes/SoSelection.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/nodes/SoTranslation.h>
#include <Inventor/draggers/SoTransformBoxDragger.h>
#include <Inventor/engines/SoCalculator.h>

#include <math.h>
#include <limits.h>

//////////////////////////
//   T3GridViewNode	//
//////////////////////////

float T3GridViewNode::drag_size = .04f;

extern void T3GridViewNode_DragFinishCB(void* userData, SoDragger* dragger);
// defined in qtso

SO_NODE_SOURCE(T3GridViewNode);

void T3GridViewNode::initClass()
{
  SO_NODE_INIT_CLASS(T3GridViewNode, T3NodeLeaf, "T3NodeLeaf");
}

T3GridViewNode::T3GridViewNode(void* dataView_)
:inherited(dataView_)
{
  SO_NODE_CONSTRUCTOR(T3GridViewNode);
  
  drag_sep_ = new SoSeparator;
  drag_xf_ = new SoTransform;
  drag_xf_->scaleFactor.setValue(drag_size, drag_size, drag_size);
  drag_xf_->translation.setValue(-0.04f, -1.05f, 0.0f);
  drag_sep_->addChild(drag_xf_);
  dragger_ = new SoTransformBoxDragger;
  drag_sep_->addChild(dragger_);

  // super-size me so stuff is actually grabable!
  dragger_->setPart("scaler.scaler", new SoBigScaleUniformScaler(.6f));
  dragger_->setPart("rotator1.rotator", new SoBigTransformBoxRotatorRotator(.4f));
  dragger_->setPart("rotator2.rotator", new SoBigTransformBoxRotatorRotator(.4f));
  dragger_->setPart("rotator3.rotator", new SoBigTransformBoxRotatorRotator(.4f));

  drag_trans_calc_ = new SoCalculator;
  drag_trans_calc_->ref();
  drag_trans_calc_->A.connectFrom(&dragger_->translation);

  String expr = "oA = vec3f(.5 + " + String(drag_size) + " * A[0], -.5 + " +
    String(drag_size) + " * A[1], 0.0 + " + String(drag_size) + " * A[2])";

  drag_trans_calc_->expression = expr.chars();

  txfm_shape()->translation.connectFrom(&drag_trans_calc_->oA);
  txfm_shape()->rotation.connectFrom(&dragger_->rotation);
  txfm_shape()->scaleFactor.connectFrom(&dragger_->scaleFactor);

  dragger_->addFinishCallback(T3GridViewNode_DragFinishCB, (void*)this);

  stage_ = new SoSeparator;
  // stage_->addChild(drag_sep_);
  mat_stage_ = new SoMaterial;
  mat_stage_->diffuseColor.setValue(0, 0, 0); // black
  stage_->addChild(mat_stage_);
  txlt_stage_ = new SoTranslation;
  stage_->addChild(txlt_stage_);
  header_ = new SoSeparator;
  stage_->addChild(header_);
  body_ = new SoSeparator;
  stage_->addChild(body_);
  insertChildAfter(topSeparator(), stage_, transform());

  insertChildAfter(topSeparator(), drag_sep_, transform());
  
  SoMaterial* mat = material(); //cache
  mat->diffuseColor.setValue(0.0f, 0.5f, 0.5f); // blue/green
  mat->transparency.setValue(0.5f);

  SoSeparator* ss = shapeSeparator(); // cache
  frame_ = new SoFrame(SoFrame::Ver);
  insertChildAfter(ss, frame_, material());
  txlt_grid_ = new SoTranslation;
  insertChildAfter(ss, txlt_grid_, frame_);
  grid_ = new SoGroup;
  insertChildAfter(ss, grid_, txlt_grid_);
}

T3GridViewNode::~T3GridViewNode()
{
  stage_ = NULL;
  mat_stage_ = NULL;
  txlt_stage_ = NULL;
  header_ = NULL;
  body_ = NULL;
  frame_ = NULL;
  txlt_grid_ = NULL;
  grid_ = NULL;
}

void T3GridViewNode::render() {
  txlt_stage_->translation.setValue(0.0f, 0.0f, 0.0f);
  frame_->setDimensions(1.1f, 1.1f, 0.02f, .02f);
  txfm_shape()->translation.setValue(.5f * 1.05f, -.5f * 1.05f, 0.0f);
//   txlt_grid_->translation.setValue(-(geom_.x/2.0f - inset), geom_.y/2.0f - inset, 0.0f);
  SoFont* font = captionFont(true);
  transformCaption(iVec3f(0.1f, -((float)font->size.getValue()) -1.05f, 0.0f)); // move caption below the frame
}

//////////////////////////
//   T3GraphViewNode	//
//////////////////////////

SO_NODE_SOURCE(T3GraphViewNode);

void T3GraphViewNode::initClass()
{
  SO_NODE_INIT_CLASS(T3GraphViewNode, T3NodeParent, "T3NodeParent");
}

T3GraphViewNode::T3GraphViewNode(void* dataView_)
:inherited(dataView_)
{
  SO_NODE_CONSTRUCTOR(T3GraphViewNode);
/*  header_ = new SoGroup();
  canvas_->addChild(header_);
  body_ = new SoGroup();
  canvas_->addChild(body_); */
}

T3GraphViewNode::~T3GraphViewNode()
{
/*  header_ = NULL;
  body_ = NULL; */
}


