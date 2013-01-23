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

#ifndef T3VEBody_h
#define T3VEBody_h 1

// parent includes:
#include <T3NodeLeaf>

// member includes:

// declare all other types mentioned but not required to include:
class T3TransformBoxDragger; // 


TypeDef_Of(T3VEBody);

class TA_API T3VEBody : public T3NodeLeaf {
  // body for virtual environment
#ifndef __MAKETA__
typedef T3NodeLeaf inherited;
  SO_NODE_HEADER(T3VEBody);
#endif // def __MAKETA__
public:
  static void   initClass();

  T3VEBody(T3DataView* bod = NULL, bool show_drag = false, float drag_size = 0.06f);

  T3TransformBoxDragger* getDragger() { return drag_; } // my position dragger

protected:
  bool                   show_drag_;
  T3TransformBoxDragger* drag_; // my position dragger

  ~T3VEBody();
};

#endif // T3VEBody_h
