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

// #include <Inventor/nodes/SoTransform.h>
// #include <Inventor/nodes/SoDrawStyle.h>
#include <Inventor/nodes/SoIndexedFaceSet.h>
#include <Inventor/nodes/SoVertexProperty.h>

SO_NODE_SOURCE(T3BrainNode);

void T3BrainNode::initClass() {
  SO_NODE_INIT_CLASS(T3BrainNode, T3NodeLeaf, "T3NodeLeaf");
}

SoIndexedFaceSet*
T3BrainNode::shape() {
  return shape_;
}

T3BrainNode::T3BrainNode(T3DataView* dataView_)
  : inherited(dataView_)
  , shape_(0)
  , vtx_prop_(0)
{
  SO_NODE_CONSTRUCTOR(T3BrainNode);

  SoSeparator* ss = shapeSeparator();

  shape_ = new SoIndexedFaceSet;
  vtx_prop_ = new SoVertexProperty;
  shape_->vertexProperty.setValue(vtx_prop_); // note: vp refs/unrefs automatically
  ss->addChild(shape_);
}

T3BrainNode::~T3BrainNode()
{
}
