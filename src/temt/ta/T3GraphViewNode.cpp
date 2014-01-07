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

#include "T3GraphViewNode.h"
#include <SoScrollBar>
#include <T3TransformBoxDragger>
#include <SoFrame>
#include <taString>
#include <T3Misc>
#include <iVec3f>

#include <Inventor/nodes/SoBaseColor.h>
#include <Inventor/nodes/SoCallback.h>
#include <Inventor/nodes/SoComplexity.h>
#include <Inventor/nodes/SoFont.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/nodes/SoTranslation.h>
#include <Inventor/draggers/SoTransformBoxDragger.h>
#include <Inventor/engines/SoCalculator.h>

float T3GraphViewNode::frame_margin = .2f;
float T3GraphViewNode::frame_width = .02f;

extern void T3GraphViewNode_DragFinishCB(void* userData, SoDragger* dragger);
// defined in qtso

SO_NODE_SOURCE(T3GraphViewNode);

void T3GraphViewNode::initClass()
{
  SO_NODE_INIT_CLASS(T3GraphViewNode, T3NodeParent, "T3NodeParent");
}

T3GraphViewNode::T3GraphViewNode(T3DataView* dataView_, float wdth, bool show_draggers)
  : inherited(dataView_)
{
  SO_NODE_CONSTRUCTOR(T3GraphViewNode);

  width_ = wdth;
  show_drag_ = show_draggers;

  if(show_drag_) {
    drag_ = new T3TransformBoxDragger(0.06f, .04f, .03f);
    drag_->xf_->translation.setValue(-frame_margin, -frame_margin, 0.0f);

    txfm_shape()->translation.connectFrom(&drag_->trans_calc_->oA);
    txfm_shape()->rotation.connectFrom(&drag_->dragger_->rotation);
    txfm_shape()->scaleFactor.connectFrom(&drag_->dragger_->scaleFactor);

    drag_->dragger_->addFinishCallback(T3GraphViewNode_DragFinishCB, (void*)this);

    topSeparator()->addChild(drag_);
  }
  else {
    drag_ = NULL;
  }

  SoSeparator* chld = childNodes();
  chld_mat_ = new SoMaterial;
  chld_mat_->diffuseColor.setValue(0, 0, 0); // black
  chld->addChild(chld_mat_);

  x_axis_sep_ = new SoSeparator;
  chld->addChild(x_axis_sep_);
  z_axis_sep_ = new SoSeparator;
  chld->addChild(z_axis_sep_);
  y_axes_sep_ = new SoSeparator;
  chld->addChild(y_axes_sep_);
  legend_sep_ = new SoSeparator;
  chld->addChild(legend_sep_);
  graphs_sep_ = new SoSeparator;
  chld->addChild(graphs_sep_);

  if(show_drag_) {		  // don't show frame either!
    SoMaterial* mat = material(); //cache
    mat->diffuseColor.setValue(T3Misc::frame_clr_r, T3Misc::frame_clr_g, T3Misc::frame_clr_b); // blue/green
    mat->transparency.setValue(T3Misc::frame_clr_tr);

    SoSeparator* ss = shapeSeparator(); // cache
    frame_ = new SoFrame(SoFrame::Ver);
    insertChildAfter(ss, frame_, material());
  }

  float vert_off = frame_margin; // + 2.0f * frame_width;

  row_sb_tx_ = new SoTransform;
  row_sb_tx_->translation.setValue(0.5f * width_, -vert_off, -frame_width); // Z is to go below label
  row_sb_tx_->scaleFactor.setValue(width_, 1.0f, 1.0f);
  topSeparator()->addChild(row_sb_tx_);
  row_scroll_bar_ = new SoScrollBar;
  topSeparator()->addChild(row_scroll_bar_);
}

T3GraphViewNode::~T3GraphViewNode()
{
  chld_mat_ = NULL;
  x_axis_sep_ = NULL;
  z_axis_sep_ = NULL;
  y_axes_sep_ = NULL;
  legend_sep_ = NULL;
  graphs_sep_ = NULL;
}

void T3GraphViewNode::render() {
  float frmg2 = 2.0f * frame_margin;

  if(show_drag_) {
    String expr = "oA = vec3f(" + String(.5f * width_) + " + A[0], " +
      String(.5f - (0.2f * frame_margin)) + " + A[1], A[2])";

    drag_->trans_calc_->expression = expr.chars();

    frame_->setDimensions(width_ + frmg2, 1.0f + frmg2 * 1.4f, frame_width, frame_width);
    txfm_shape()->translation.setValue(.5f * width_, .5f- (0.2f * frame_margin), 0.0f);
  }
  // note: also change in DragFinishCB in qtso
  SoFont* font = captionFont(true);
  transformCaption(iVec3f(0.0f, -((float)font->size.getValue() + frame_margin), 0.02f)); // move caption below the frame
  float vert_off = frame_margin; // + 2.0f * frame_width;
  row_sb_tx_->translation.setValue(0.5f * width_, -vert_off, -frame_width); // Z is to go below label
  row_sb_tx_->scaleFactor.setValue(width_, 1.0f, 1.0f);
}

void T3GraphViewNode::setWidth(float wdth) {
  width_ = wdth;
  render();
}

