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

#include "T3GridViewNode.h"
#include <SoScrollBar>
#include <T3TransformBoxDragger>
#include <SoFrame>
#include <taString>
#include <T3Misc>
#include <taMath_float>
#include <iVec3f>

#include <Inventor/nodes/SoAsciiText.h>
#include <Inventor/nodes/SoBaseColor.h>
#include <Inventor/nodes/SoCallback.h>
#include <Inventor/nodes/SoComplexity.h>
#include <Inventor/nodes/SoFont.h>
#include <Inventor/nodes/SoLineSet.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoMarkerSet.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/nodes/SoTranslation.h>
#include <Inventor/draggers/SoTransformBoxDragger.h>
#include <Inventor/engines/SoCalculator.h>

float T3GridViewNode::frame_margin = .05f;
float T3GridViewNode::frame_width = .02f;

extern void T3GridViewNode_DragFinishCB(void* userData, SoDragger* dragger);
// defined in qtso

SO_NODE_SOURCE(T3GridViewNode);

void T3GridViewNode::initClass()
{
  SO_NODE_INIT_CLASS(T3GridViewNode, T3NodeParent, "T3NodeParent");
}

T3GridViewNode::T3GridViewNode(T3DataView* dataView_, float wdth, bool show_draggers)
  : inherited(dataView_)
{
  SO_NODE_CONSTRUCTOR(T3GridViewNode);

  width_ = wdth;
  show_drag_ = show_draggers;

  if(show_drag_) {
    drag_ = new T3TransformBoxDragger(0.06f, .04f, .03f);
    drag_->xf_->translation.setValue(-frame_margin, -frame_margin, 0.0f);

    txfm_shape()->translation.connectFrom(&(drag_->trans_calc_->oA));
    txfm_shape()->rotation.connectFrom(&(drag_->dragger_->rotation));
    txfm_shape()->scaleFactor.connectFrom(&(drag_->dragger_->scaleFactor));

    drag_->dragger_->addFinishCallback(T3GridViewNode_DragFinishCB, (void*)this);

    topSeparator()->addChild(drag_);
  }
  else {
    drag_ = NULL;
  }

  SoSeparator* chld = childNodes();
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
  grid_ = new SoSeparator;
  stage_->addChild(grid_);
  chld->addChild(stage_);

  if(show_drag_) {
    SoMaterial* mat = material(); //cache
    mat->diffuseColor.setValue(T3Misc::frame_clr_r, T3Misc::frame_clr_g, T3Misc::frame_clr_b); // blue/green
    mat->transparency.setValue(T3Misc::frame_clr_tr);

    SoSeparator* ss = shapeSeparator(); // cache
    frame_ = new SoFrame(SoFrame::Ver);
    insertChildAfter(ss, frame_, material());
  }

  float vert_off = frame_margin + 2.0f * frame_width;

  col_sb_tx_ = new SoTransform;
  //  col_sb_tx_->translation.setValue(0.0f, -vert_off, -frame_width); // Z is to go below label
  // from graph:
  col_sb_tx_->translation.setValue(0.5f * width_, -vert_off, -frame_width);
  col_sb_tx_->scaleFactor.setValue(width_, 1.0f, 1.0f);
  topSeparator()->addChild(col_sb_tx_);
  col_scroll_bar_ = new SoScrollBar;
  topSeparator()->addChild(col_scroll_bar_);

  SoTransform* scrtx = new SoTransform;
  scrtx->translation.setValue(.5f + frame_margin + 2.0f * frame_width,
			      0.5f + vert_off, frame_width);
  scrtx->rotation.setValue(SbVec3f(0.0f, 0.0f, 1.0f), (float)(-.5f * taMath_float::pi)); // vertical!
  topSeparator()->addChild(scrtx);
  row_scroll_bar_ = new SoScrollBar;
  topSeparator()->addChild(row_scroll_bar_);
}

T3GridViewNode::~T3GridViewNode() {
  stage_ = NULL;
  mat_stage_ = NULL;
  txlt_stage_ = NULL;
  header_ = NULL;
  body_ = NULL;
  frame_ = NULL;
  grid_ = NULL;
}

void T3GridViewNode::setWidth(float wdth) {
  width_ = wdth;
  render();
}

void T3GridViewNode::render() {
  float frmg2 = 2.0f * frame_margin;
  float vert_off = frame_margin + frmg2;

  if(show_drag_) {
    String expr = "oA = vec3f(" + String(.5f * width_) + " + A[0], "
      + String(.5f) + " + A[1], A[2])";

    drag_->trans_calc_->expression = expr.chars();
    frame_->setDimensions(width_ + frmg2, 1.0f + frmg2,
                          frame_width, frame_width);
    txfm_shape()->translation.setValue(.5f * width_, .5f, 0.0f);
  }

  txlt_stage_->translation.setValue(0.0f, 1.0f, 0.0f);
  col_sb_tx_->scaleFactor.setValue(width_, 1.0f, 1.0f);
  // note: also change in DragFinishCB in qtso
  SoFont* font = captionFont(true);
  transformCaption(iVec3f(0.1f, -((float)font->size.getValue() + frame_margin), 0.02f)); // move caption below the frame
}

