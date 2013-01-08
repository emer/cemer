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

#ifndef T3VESpace_h
#define T3VESpace_h 1

// parent includes:
#include <T3NodeParent>

// member includes:

// declare all other types mentioned but not required to include:


class TA_API T3VESpace : public T3NodeParent {
  // space parent for virtual environment
#ifndef __MAKETA__
typedef T3NodeParent inherited;
  SO_NODE_HEADER(T3VESpace);
#endif // def __MAKETA__
public:
  static void   initClass();

  T3VESpace(T3DataView* obj = NULL);

protected:
  ~T3VESpace();
};

#endif // T3VESpace_h
