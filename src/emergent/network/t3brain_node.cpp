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

#include "t3brain_node.h"

SO_NODE_SOURCE(T3BrainNode);

void T3BrainNode::initClass() 
{
  SO_NODE_INIT_CLASS(T3BrainNode, T3NodeLeaf, "T3NodeLeaf");
}

T3BrainNode::T3BrainNode(T3DataView* dataView_) 
: inherited(dataView_)
{
  SO_NODE_CONSTRUCTOR(T3BrainNode);
}

T3BrainNode::~T3BrainNode()
{
}
