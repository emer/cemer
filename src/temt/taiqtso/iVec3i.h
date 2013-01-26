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

#ifndef iVec3i_h
#define iVec3i_h 1

#include "taiqtso_def.h"

#include <iVec2i>

/* Notes:
   1. all classes below are value-semantics, and use the implicit copy and = operators.
   2. MAKETA cannot boggle the : class(): x(), y() {} form of constructor initialialization,
      so they have been moved to the .cc file.
   3. DO NOT add any virtual methods to these value-semantic classes
*/

#ifdef TA_USE_INVENTOR
class SbVec3s;          // #IGNORE
#endif

TypeDef_Of(iVec3i);

class TAIQTSO_API iVec3i: public iVec2i { // #NO_TOKENS #INSTANCE #EDIT_INLINE
public:
  int   z;

  bool          isEqual(int x_, int y_, int z_) {return ((x == x_) && (y == y_) && (z == z_));}
  void          getValue(int& x_, int& y_, int& z_) {x_ = x; y_ = y; z_ = z;}
  void          setValue(int x_, int y_, int z_) {x = x_; y = y_; z = z_;}

  iVec3i() {z = 0;}
  iVec3i(int x_, int y_, int z_): iVec2i(x_, y_) {z = z_;}
  iVec3i(int v): iVec2i(v) {z = v;}
  iVec3i(const iVec3i& cp): iVec2i(cp) {z = cp.z;}

  iVec3i& operator =(const iVec3i& cp) {x = cp.x; y = cp.y; z = cp.z; return *this;}
  iVec3i& operator =(int v) {x = v; y = v; z = v; return *this;}

#ifdef TA_USE_INVENTOR
  iVec3i(const SbVec3s& src);
  iVec3i& operator=(const SbVec3s& src);
  operator SbVec3s() const; // note: will always be in range in context of gui
#endif
};


#endif // iVec3i_h
