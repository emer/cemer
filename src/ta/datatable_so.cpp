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


#include "datatable_so.h"

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
#include <Inventor/nodes/SoCylinder.h>
//#include <Inventor/nodes/SoDirectionalLight.h>
#include <Inventor/nodes/SoDrawStyle.h>
#include <Inventor/nodes/SoFont.h>
#include <Inventor/nodes/SoMaterial.h>
//#include <Inventor/nodes/SoPerspectiveCamera.h>
//#include <Inventor/nodes/SoSelection.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/nodes/SoTranslation.h>

#include <math.h>
#include <limits.h>


//////////////////////////
//   T3TableViewNode	//
//////////////////////////

SO_NODE_SOURCE(T3TableViewNode);

void T3TableViewNode::initClass()
{
  SO_NODE_INIT_CLASS(T3TableViewNode, T3NodeParent, "T3NodeParent");
}

T3TableViewNode::T3TableViewNode(void* dataView_)
:inherited(dataView_)
{
  SO_NODE_CONSTRUCTOR(T3TableViewNode);

  frame_ = NULL;
  geom_.setValue(1, 1, 1); // puts in valid state
}

T3TableViewNode::~T3TableViewNode()
{
  frame_ = NULL;
}

void T3TableViewNode::render(float inset) {
  if (frame_) {
    frame_->setDimensions(geom_.x, geom_.y, 0.1f, inset);
  }
  txfm_shape()->translation.setValue(geom_.x/2.0f, geom_.y/2.0f, 0.0f);
}

void T3TableViewNode::setGeom(int px, int py, int pz) {
  setGeom(px, py, pz, (frame_) ? frame_->inset : 0.0f); 
}

void T3TableViewNode::setGeom(int px, int py, int pz, float inset) {
  if (px < 1) px = 1;  if (py < 1) py = 1;  if (pz < 1) pz = 1;
  if (geom_.isEqual(px, py, pz) && 
    (!frame_ ||  (frame_->inset == inset)) ) return;
  geom_.setValue(px, py, pz);
  render(inset);
}

void T3TableViewNode::setShowFrame(bool value, float inset) {
  if (showFrame() != value) {
    SoSeparator* ss = shapeSeparator(); // cache
    if (value) {
      frame_ = new SoFrame(SoFrame::Ver, inset);
      //frame_ = new SoFrame(SoFrame::Hor);
      insertChildAfter(ss, frame_, material());
    } else {
      ss->removeChild(frame_);
      frame_ = NULL;
    }
  }
  render(inset);
}

bool T3TableViewNode::showFrame() {
  return frame_;
}


//////////////////////////
//   T3GridViewNode	//
//////////////////////////

SO_NODE_SOURCE(T3GridViewNode);

void T3GridViewNode::initClass()
{
  SO_NODE_INIT_CLASS(T3GridViewNode, T3NodeLeaf, "T3NodeLeaf");
}

T3GridViewNode::T3GridViewNode(void* dataView_)
:inherited(dataView_)
{
  SO_NODE_CONSTRUCTOR(T3GridViewNode);
  
  stage_ = new SoSeparator;
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

void T3GridViewNode::render(float inset) {
  txlt_stage_->translation.setValue(inset, geom_.y - inset, 0.0f);
  frame_->setDimensions(geom_.x, geom_.y, 0.1f, inset);
  txfm_shape()->translation.setValue(geom_.x/2.0f, geom_.y/2.0f, 0.0f);
  txlt_grid_->translation.setValue(-(geom_.x/2.0f - inset), geom_.y/2.0f - inset, 0.0f);
}

void T3GridViewNode::setGeom(int px, int py) {
  setGeom(px, py, frame_->inset); 
}

void T3GridViewNode::setGeom(int px, int py, float inset) {
  if (px < 1) px = 1;  if (py < 1) py = 1;
  if (inset < 0.0f) inset = 0.0f; // TODO: make sure 0-inset is legal!
  if (geom_.isEqual(px, py) && (frame_->inset == inset) ) return;
  geom_.setValue(px, py);
  render(inset);
}

void T3GridViewNode::setInset(float value) {
  if (frame_->inset == value) return;
  render(value);
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


