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

#ifndef iPoint_h
#define iPoint_h 1

#include "taiqtso_def.h"

/* Notes:
   1. all classes below are value-semantics, and use the implicit copy and = operators.
   2. MAKETA cannot boggle the : class(): x(), y() {} form of constructor initialialization,
      so they have been moved to the .cc file.
   3. DO NOT add any virtual methods to these value-semantic classes
*/

#ifdef TA_USE_INVENTOR
class SbVec2s;          // #IGNORE
#endif

class TAIQTSO_API iPoint { // #NO_TOKENS #INSTANCE #EDIT_INLINE
public:
  int   x;
  int   y;

  int           getArrayIndex(int x_, int y_); // assumes x and y are array sizes -- gets the row-major array index, or -1 if out of bounds
  bool          isEqual(int x_, int y_) {return ((x == x_) && (y == y_));}
  void          getValue(int& x_, int& y_) {x_ = x; y_ = y;}
  void          setValue(int x_, int y_) {x = x_; y = y_;}

  iPoint() {x = 0; y = 0;}
  iPoint(int x_, int y_) {x = x_; y = y_;}
  iPoint(int v) {x = v; y = v;}
  iPoint(const iPoint& cp) {x = cp.x; y = cp.y;}

  iPoint& operator =(const iPoint& cp) {x = cp.x; y = cp.y; return *this;}
  iPoint& operator =(int v) {x = v; y = v; return *this;}

#ifdef TA_GUI
  iPoint(const QPoint& val);
  iPoint& operator=(const QPoint& val);
  operator QPoint() const;
#endif
#ifdef TA_USE_INVENTOR
  iPoint(const SbVec2s& src);
  iPoint&       operator=(const SbVec2s& src);
  operator SbVec2s() const; // note: will always be in range in context of gui
#endif
};

typedef iPoint iVec2i; // synonym

#endif // iPoint_h
