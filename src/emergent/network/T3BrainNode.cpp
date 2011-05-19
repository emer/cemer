// Copyright, 1995-2011, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of The Emergent Toolkit
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

#include "T3BrainNode.h"
#include "BrainView.h"

#include <Inventor/nodes/SoTransform.h>
#include <Inventor/nodes/SoDrawStyle.h>
#include <Inventor/nodes/SoIndexedFaceSet.h>

SO_NODE_SOURCE(T3BrainNode);

void T3BrainNode::initClass() {
  SO_NODE_INIT_CLASS(T3BrainNode, T3NodeLeaf, "T3NodeLeaf");
}

SoIndexedFaceSet*
T3BrainNode::shape() {
  return shape_;
}
SoSeparator*
T3BrainNode::netText() {
  return net_text_;
}
SoTransform*
T3BrainNode::netTextXform() {
  return net_text_xform_;
}
SoTransform*
T3BrainNode::netTextDragXform() {
  return net_text_drag_xform_;
}
bool
T3BrainNode::netTextDrag() {
  return show_net_text_drag_;
}

// anon namespace for dragger callbacks
namespace {
  // callback for netview transformer dragger
  void T3BrainNode_DragFinishCB(void* userData, SoDragger* dragr) {
    SoTransformBoxDragger* dragger = (SoTransformBoxDragger*)dragr;
    T3BrainNode* brain = (T3BrainNode*)userData;
    BrainView* bv = static_cast<BrainView*>(brain->dataView());

    SbRotation cur_rot;
    cur_rot.setValue(SbVec3f(bv->main_xform.rotate.x, bv->main_xform.rotate.y, 
                             bv->main_xform.rotate.z), bv->main_xform.rotate.rot);

    SbVec3f trans = dragger->translation.getValue();
    cur_rot.multVec(trans, trans); // rotate the translation by current rotation
    trans[0] *= bv->main_xform.scale.x;
    trans[1] *= bv->main_xform.scale.y;
    trans[2] *= bv->main_xform.scale.z;
    FloatTDCoord tr(trans[0], trans[1], trans[2]);
    bv->main_xform.translate += tr;

    const SbVec3f& scale = dragger->scaleFactor.getValue();
    FloatTDCoord sc(scale[0], scale[1], scale[2]);
    if(sc < .1f) sc = .1f;        // prevent scale from going to small too fast!!
    bv->main_xform.scale *= sc;

    SbVec3f axis;
    float angle;
    dragger->rotation.getValue(axis, angle);

    if(axis[0] != 0.0f || axis[1] != 0.0f || axis[2] != 1.0f || angle != 0.0f) {
      SbRotation rot;
      rot.setValue(SbVec3f(axis[0], axis[1], axis[2]), angle);
      SbRotation nw_rot = rot * cur_rot;
      nw_rot.getValue(axis, angle);
      bv->main_xform.rotate.SetXYZR(axis[0], axis[1], axis[2], angle);
    }

    float h = 0.04f; // nominal amount of height, so we don't vanish
    brain->txfm_shape()->scaleFactor.setValue(1.0f, 1.0f, 1.0f);
    brain->txfm_shape()->rotation.setValue(SbVec3f(0.0f, 0.0f, 1.0f), 0.0f);
    brain->txfm_shape()->translation.setValue(.5f, .5f * h, -.5f);
    dragger->translation.setValue(0.0f, 0.0f, 0.0f);
    dragger->rotation.setValue(SbVec3f(0.0f, 0.0f, 1.0f), 0.0f);
    dragger->scaleFactor.setValue(1.0f, 1.0f, 1.0f);

    bv->UpdateDisplay();
  }

  // callback for netview net text transformer dragger
  void T3NetText_DragFinishCB(void* userData, SoDragger* dragr) {
    SoTransformBoxDragger* dragger = (SoTransformBoxDragger*)dragr;
    T3BrainNode* brain = (T3BrainNode*)userData;
    BrainView* bv = static_cast<BrainView*>(brain->dataView());

    SbRotation cur_rot;
    cur_rot.setValue(SbVec3f(bv->net_text_xform.rotate.x, bv->net_text_xform.rotate.y, 
                             bv->net_text_xform.rotate.z), bv->net_text_xform.rotate.rot);

    SbVec3f trans = dragger->translation.getValue();
    cur_rot.multVec(trans, trans); // rotate the translation by current rotation
    trans[0] *= bv->net_text_xform.scale.x;
    trans[1] *= bv->net_text_xform.scale.y;
    trans[2] *= bv->net_text_xform.scale.z;
    FloatTDCoord tr(trans[0], trans[1], trans[2]);
    bv->net_text_xform.translate += tr;

    const SbVec3f& scale = dragger->scaleFactor.getValue();
    FloatTDCoord sc(scale[0], scale[1], scale[2]);
    if(sc < .1f) sc = .1f;        // prevent scale from going to small too fast!!
    bv->net_text_xform.scale *= sc;

    SbVec3f axis;
    float angle;
    dragger->rotation.getValue(axis, angle);

    if(axis[0] != 0.0f || axis[1] != 0.0f || axis[2] != 1.0f || angle != 0.0f) {
      SbRotation rot;
      rot.setValue(SbVec3f(axis[0], axis[1], axis[2]), angle);
      SbRotation nw_rot = rot * cur_rot;
      nw_rot.getValue(axis, angle);
      bv->net_text_xform.rotate.SetXYZR(axis[0], axis[1], axis[2], angle);
    }

    // reset the drag guy: note that drag is still connected to the drag xform so you
    // need to reset dragger first, then the xform!
    dragger->translation.setValue(0.0f, 0.0f, 0.0f);
    dragger->rotation.setValue(SbVec3f(0.0f, 0.0f, 1.0f), 0.0f);
    dragger->scaleFactor.setValue(1.0f, 1.0f, 1.0f);
    brain->netTextDragXform()->scaleFactor.setValue(1.0f, 1.0f, 1.0f);
    brain->netTextDragXform()->rotation.setValue(SbVec3f(0.0f, 0.0f, 1.0f), 0.0f);
    brain->netTextDragXform()->translation.setValue(0.0f, 0.0f, 0.0f);

    bv->UpdateDisplay();
  }
}


T3BrainNode::T3BrainNode(T3DataView* dataView_, bool show_draggers,
		     bool show_net_text, bool show_nt_drag)
  : inherited(dataView_)
  , shape_(0)
  , net_text_(0)
  , show_drag_(show_draggers)
  , show_net_text_drag_(show_nt_drag && show_net_text)
  , drag_(0)
  , net_text_drag_(0)
  , net_text_xform_(0)
  , net_text_drag_xform_(0)
{
  SO_NODE_CONSTRUCTOR(T3BrainNode);

  float net_margin = 0.05f;
  txfm_shape()->translation.setValue(-net_margin, -net_margin, -net_margin);

  if (show_drag_) {
    drag_ = new T3TransformBoxDragger(0.06f, .04f, .03f);
    drag_->xf_->translation.setValue(-net_margin, -net_margin, -net_margin);
    String expr = "oA = vec3f(-" + String(net_margin) + " + A[0], -"
      + String(net_margin) + " + A[1], -" + String(net_margin) + " + A[2])";
    drag_->trans_calc_->expression = expr.chars();

    txfm_shape()->translation.connectFrom(&drag_->trans_calc_->oA);
    txfm_shape()->rotation.connectFrom(&drag_->dragger_->rotation);
    txfm_shape()->scaleFactor.connectFrom(&drag_->dragger_->scaleFactor);

    drag_->dragger_->addFinishCallback(T3BrainNode_DragFinishCB, (void*)this);
    topSeparator()->addChild(drag_);

    SoSeparator* ss = shapeSeparator();
    shape_ = new SoIndexedFaceSet();
    ss->addChild(shape_);
  }

  if (show_net_text) {
    net_text_ = new SoSeparator;
    addChild(net_text_);

    net_text_xform_ = new SoTransform;
    net_text_xform_->translation.setValue(0.0f, 0.0f, 0.0f);
    net_text_->addChild(net_text_xform_);

    if (show_net_text_drag_) {
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
}

T3BrainNode::~T3BrainNode()
{
}
