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

#include "T3NetNode.h"

extern void T3NetNode_DragFinishCB(void* userData, SoDragger* dragger);
// defined in qtso
extern void T3NetText_DragFinishCB(void* userData, SoDragger* dragger);
// defined in qtso

SO_NODE_SOURCE(T3NetNode);

void T3NetNode::initClass()
{
  SO_NODE_INIT_CLASS(T3NetNode, T3NodeParent, "T3NodeParent");
}

T3NetNode::T3NetNode(T3DataView* dataView_, bool show_draggers,
		     bool show_net_text, bool show_nt_drag,
		     bool md_2d)
  : inherited(dataView_)
{
  SO_NODE_CONSTRUCTOR(T3NetNode);

  show_drag_ = show_draggers;
  show_net_text_drag_ = show_nt_drag && show_net_text;
  mode_2d_ = md_2d;

  float net_margin = 0.05f;
  float two_net_marg = 2.0f * net_margin;
  txfm_shape()->translation.setValue(-net_margin, -net_margin, -net_margin);

  if(show_drag_) {
    drag_ = new T3TransformBoxDragger(0.06f, .04f, .03f);

    if(mode_2d_) {
      drag_->xf_->translation.setValue(-net_margin, net_margin, -net_margin);
    }
    else {
      drag_->xf_->translation.setValue(-net_margin, -net_margin, -net_margin);
    }

    String expr = "oA = vec3f(-" + String(net_margin) + " + A[0], -"
      + String(net_margin) + " + A[1], -" + String(net_margin) + " + A[2])";
    drag_->trans_calc_->expression = expr.chars();

    txfm_shape()->translation.connectFrom(&drag_->trans_calc_->oA);
    txfm_shape()->rotation.connectFrom(&drag_->dragger_->rotation);
    txfm_shape()->scaleFactor.connectFrom(&drag_->dragger_->scaleFactor);

    drag_->dragger_->addFinishCallback(T3NetNode_DragFinishCB, (void*)this);
    topSeparator()->addChild(drag_);
    SoSeparator* ss = shapeSeparator(); // cache
    shape_draw_ = new SoDrawStyle;
    shape_draw_->style = SoDrawStyle::LINES;
    ss->addChild(shape_draw_);
    if(mode_2d_) {
      shape_ = new SoLineBox3d(1.0f + two_net_marg, two_net_marg,
			       1.0f + two_net_marg, false); // false = no center
    }
    else {
      shape_ = new SoLineBox3d(1.0f + two_net_marg, 1.0f + two_net_marg,
			       1.0f + two_net_marg, false); // false = no center
    }

    ss->addChild(shape_);
  }
  else {
    shape_ = NULL;
    shape_draw_ = NULL;
  }

  net_text_ = NULL;
  net_text_drag_ = NULL;
  net_text_xform_ = NULL;
  net_text_drag_xform_ = NULL;
  if(show_net_text) {
    net_text_ = new SoSeparator;
    addChild(net_text_);
    net_text_xform_ = new SoTransform;
    net_text_xform_->translation.setValue(0.0f, 0.0f, 0.0f);
    net_text_->addChild(net_text_xform_);
    if(show_net_text_drag_) {
      net_text_drag_xform_ = new SoTransform;
      net_text_drag_ = new T3TransformBoxDragger(0.04f, .03f, .02f);

      net_text_drag_->xf_->translation.setValue(1.0f, 0.0f, 0.0f);
      String expr = "oA = vec3f(1.0 + A[0], A[1], A[2])";
      net_text_drag_->trans_calc_->expression = expr.chars();

      net_text_drag_xform_->translation.connectFrom(&net_text_drag_->dragger_->translation);
      net_text_drag_xform_->rotation.connectFrom(&net_text_drag_->dragger_->rotation);
      net_text_drag_xform_->scaleFactor.connectFrom(&net_text_drag_->dragger_->scaleFactor);

      net_text_drag_->dragger_->addFinishCallback(T3NetText_DragFinishCB, (void*)this);
      net_text_->addChild(net_text_drag_); // dragger then the xform it affects
      net_text_->addChild(net_text_drag_xform_);
      // we then copy the drag_xform back into the main xform in the CB, to effect the change
    }
  }
  wt_lines_ = new SoSeparator;
  wt_lines_->setName("WtLines");
  wt_lines_draw_ = new SoDrawStyle;
  wt_lines_->addChild(wt_lines_draw_);
  wt_lines_set_ = new SoIndexedLineSet;
  wt_lines_vtx_prop_ = new SoVertexProperty;
  wt_lines_set_->vertexProperty.setValue(wt_lines_vtx_prop_); // does ref/unref
  wt_lines_->addChild(wt_lines_set_);
  addChild(wt_lines_);
}

T3NetNode::~T3NetNode()
{
}

void T3NetNode::setDefaultCaptionTransform() {
  SbVec3f tran(0.0f, -.1f, 0.03f);
  transformCaption(tran);
}

