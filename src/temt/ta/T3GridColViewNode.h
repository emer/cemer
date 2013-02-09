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

#ifndef T3GridColViewNode_h
#define T3GridColViewNode_h 1

// parent includes:
#include <T3NodeLeaf>

// member includes:

// declare all other types mentioned but not required to include:


taTypeDef_Of(T3GridColViewNode);

class TA_API T3GridColViewNode: public T3NodeLeaf {
  // grid view column -- mainly just for editing column spec ifno
#ifndef __MAKETA__
typedef T3NodeLeaf inherited;
  SO_NODE_HEADER(T3GridColViewNode);
#endif // def __MAKETA__
public:
  static void	initClass();

  T3GridColViewNode(T3DataView* colspec = NULL);

protected:
  ~T3GridColViewNode();
};

#endif // T3GridColViewNode_h
