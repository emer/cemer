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
  canvasSeparator_ = new SoSeparator();
  txfm_canvas_ = new SoTransform();
  canvasSeparator_->addChild(txfm_canvas_);
  canvas_ = new SoSeparator();
  canvasSeparator_->addChild(canvas_);
  insertChildAfter(topSeparator(), canvasSeparator_, shapeSeparator());
  geom_.setValue(1, 1, 1); // puts in valid state
}

T3TableViewNode::~T3TableViewNode()
{
  canvas_ = NULL;
  txfm_canvas_ = NULL;
  canvasSeparator_ = NULL;
  frame_ = NULL;
}

void T3TableViewNode::render() {
  float inset = (frame_) ? frame_->inset : 0.0f;
  if (frame_) {
    frame_->setDimensions(geom_.x, geom_.z, 0.1f, 0.05f);
    // compensate canvas area for inset of frame
    txfm_canvas_->scaleFactor.setValue(
      (geom_.x - (2 * inset)) / geom_.x,
      (geom_.z - (2 * inset)) / geom_.z,
      1.0f);
  } else {
    txfm_canvas_->scaleFactor.setValue(1.0f, 1.0f, 1.0f);
  }
  txfm_shape()->translation.setValue(geom_.x/2.0f, geom_.z/2.0f, 0.0f);
  txfm_canvas_->translation.setValue(inset, inset, 0.0f);
}

void T3TableViewNode::setGeom(int px, int py, int pz) {
  if (px < 1) px = 1;  if (pz < 1) py = 1;  if (pz < 1) pz = 1;
  if (geom_.isEqual(px, py, pz)) return;
  geom_.setValue(px, py, pz);
  render();
}

void T3TableViewNode::setShowFrame(bool value) {
  if (showFrame() == value) return;
  SoSeparator* ss = shapeSeparator(); // cache
  if (value) {
    frame_ = new SoFrame(SoFrame::Ver);
    //frame_ = new SoFrame(SoFrame::Hor);
    insertChildAfter(ss, frame_, material());
  } else {
    ss->removeChild(frame_);
    frame_ = NULL;
  }
  render();
}

bool T3TableViewNode::showFrame() {
  return frame_;
}


//////////////////////////
//   T3GridTableViewNode	//
//////////////////////////

SO_NODE_SOURCE(T3GridTableViewNode);

void T3GridTableViewNode::initClass()
{
  SO_NODE_INIT_CLASS(T3GridTableViewNode, T3TableViewNode, "T3TableViewNode");
}

T3GridTableViewNode::T3GridTableViewNode(void* dataView_)
:inherited(dataView_)
{
  SO_NODE_CONSTRUCTOR(T3GridTableViewNode);
  header_ = new SoGroup();
  canvas_->addChild(header_);
  body_ = new SoGroup();
  canvas_->addChild(body_);
}

T3GridTableViewNode::~T3GridTableViewNode()
{
  header_ = NULL;
  body_ = NULL;
}


//////////////////////////
//   T3GraphTableViewNode	//
//////////////////////////

SO_NODE_SOURCE(T3GraphTableViewNode);

void T3GraphTableViewNode::initClass()
{
  SO_NODE_INIT_CLASS(T3GraphTableViewNode, T3TableViewNode, "T3TableViewNode");
}

T3GraphTableViewNode::T3GraphTableViewNode(void* dataView_)
:inherited(dataView_)
{
  SO_NODE_CONSTRUCTOR(T3GraphTableViewNode);
/*  header_ = new SoGroup();
  canvas_->addChild(header_);
  body_ = new SoGroup();
  canvas_->addChild(body_); */
}

T3GraphTableViewNode::~T3GraphTableViewNode()
{
/*  header_ = NULL;
  body_ = NULL; */
}


