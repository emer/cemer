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

#include "T3AnnotationNode.h"

#include <T3TransformBoxDragger>


#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoDrawStyle.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/engines/SoCalculator.h>
#include <Inventor/draggers/SoTranslate2Dragger.h>
#include <Inventor/draggers/SoTranslate1Dragger.h>
#include <Inventor/draggers/SoTransformBoxDragger.h>


SO_NODE_SOURCE(T3AnnotationNode);

void T3AnnotationNode::initClass()
{
  SO_NODE_INIT_CLASS(T3AnnotationNode, T3NodeLeaf, "T3NodeLeaf");
}

extern void T3AnnotationNode_DragFinishCB(void* userData, SoDragger* dragger);
// defined in T3AnnotationView

T3AnnotationNode::T3AnnotationNode(T3DataView* dataView_, bool show_drag)
  : inherited(dataView_)
{
  SO_NODE_CONSTRUCTOR(T3AnnotationNode);

  show_drag_ = show_drag;
  drag_ = NULL;
  if(show_drag_) {
    drag_ = new T3TransformBoxDragger(0.03f, .02f, .015f);

    txfm_shape()->translation.connectFrom(&drag_->dragger_->translation);
    txfm_shape()->rotation.connectFrom(&drag_->dragger_->rotation);
    txfm_shape()->scaleFactor.connectFrom(&drag_->dragger_->scaleFactor);

    drag_->dragger_->addFinishCallback(T3AnnotationNode_DragFinishCB, (void*)this);
    insertChildBefore(topSeparator(), drag_, txfm_shape());
  }
}

T3AnnotationNode::~T3AnnotationNode()
{
  
}

