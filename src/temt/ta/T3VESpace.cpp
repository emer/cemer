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

#include "T3VESpace.h"

SO_NODE_SOURCE(T3VESpace);

void T3VESpace::initClass()
{
  SO_NODE_INIT_CLASS(T3VESpace, T3NodeParent, "T3NodeParent");
}

T3VESpace::T3VESpace(T3DataView* world)
  : inherited(world)
{
  SO_NODE_CONSTRUCTOR(T3VESpace);
}

T3VESpace::~T3VESpace()
{
  
}

