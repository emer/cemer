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

#ifndef T3NetViewObj_h
#define T3NetViewObj_h 1

// parent includes:
#include <T3NodeLeaf>

// member includes:

// declare all other types mentioned but not required to include:

class EMERGENT_API T3NetViewObj : public T3NodeLeaf {
  // network view object
typedef T3NodeLeaf inherited;
  SO_NODE_HEADER(T3NetViewObj);
public:
  static void	initClass();

  T3NetViewObj(T3DataView* dataView_ = NULL, bool show_drag = false);

protected:
  bool			 show_drag_;
  T3TransformBoxDragger* drag_;	// my position dragger

  ~T3NetViewObj();
};

#endif // T3NetViewObj_h
