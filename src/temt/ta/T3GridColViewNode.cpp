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

#include "T3GridColViewNode.h"

// this guy is managed by the _qtso and is only for selecting the column spec!

SO_NODE_SOURCE(T3GridColViewNode);

void T3GridColViewNode::initClass()
{
  SO_NODE_INIT_CLASS(T3GridColViewNode, T3NodeLeaf, "T3NodeLeaf");
}

T3GridColViewNode::T3GridColViewNode(T3DataView* colspec)
  : inherited(colspec)
{
  SO_NODE_CONSTRUCTOR(T3GridColViewNode);
}

T3GridColViewNode::~T3GridColViewNode()
{
}


