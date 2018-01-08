// Co2018ght 2013-2017, Regents of the University of Colorado,
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
#include "network_def.h"

// member includes:

// declare all other types mentioned but not required to include:

#ifdef TA_QT3D

class E_API T3VESpace : public T3NodeParent {
  // space parent for virtual environment
  Q_OBJECT
  INHERITED(T3NodeParent)
public:

  T3VESpace(Qt3DNode* par = NULL, T3DataView* obj = NULL);
  ~T3VESpace();

};

#else // TA_QT3D

eTypeDef_Of(T3VESpace);

class E_API T3VESpace : public T3NodeParent {
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

#endif // TA_QT3D

#endif // T3VESpace_h
