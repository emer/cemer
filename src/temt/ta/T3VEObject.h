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

#ifndef T3VEObject_h
#define T3VEObject_h 1

// parent includes:
#include <T3NodeParent>

// member includes:

// declare all other types mentioned but not required to include:


class TA_API T3VEObject : public T3NodeParent {
  // object parent for virtual environment
#ifndef __MAKETA__
typedef T3NodeParent inherited;
  SO_NODE_HEADER(T3VEObject);
#endif // def __MAKETA__
public:
  static void   initClass();

  T3VEObject(T3DataView* obj = NULL);

protected:
  ~T3VEObject();
};

#endif // T3VEObject_h
