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

#include "T3VEJoint.h"
#include <T3TransformBoxDragger>

#include <Inventor/nodes/SoTransform.h>
#include <Inventor/nodes/SoTranslation.h>
#include <Inventor/draggers/SoTransformBoxDragger.h>
#include <Inventor/engines/SoCalculator.h>

extern void T3VEJoint_DragFinishCB(void* userData, SoDragger* dragger);
// defined in qtso

SO_NODE_SOURCE(T3VEJoint);

void T3VEJoint::initClass()
{
  SO_NODE_INIT_CLASS(T3VEJoint, T3NodeLeaf, "T3NodeLeaf");
}

T3VEJoint::T3VEJoint(T3DataView* bod, bool show_drag, float drag_size)
  : inherited(bod)
{
  SO_NODE_CONSTRUCTOR(T3VEJoint);

  show_drag_ = show_drag;
  drag_ = NULL;
  if(show_drag_) {
    drag_ = new T3TransformBoxDragger(drag_size, drag_size * 0.6f, drag_size * .5f);

    txfm_shape()->translation.connectFrom(&drag_->dragger_->translation);
    txfm_shape()->rotation.connectFrom(&drag_->dragger_->rotation);
    txfm_shape()->scaleFactor.connectFrom(&drag_->dragger_->scaleFactor);

    drag_->dragger_->addFinishCallback(T3VEJoint_DragFinishCB, (void*)this);
    insertChildBefore(topSeparator(), drag_, txfm_shape());
  }
}

T3VEJoint::~T3VEJoint()
{
  
}

