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

#ifndef iVec2i_h
#define iVec2i_h 1

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

class QPoint; //


TypeDef_Of(iVec2i);

class TAIQTSO_API iVec2i { // #NO_TOKENS #INSTANCE #EDIT_INLINE
public:
  int   x;
  int   y;

  int           getArrayIndex(int x_, int y_); // assumes x and y are array sizes -- gets the row-major array index, or -1 if out of bounds
  bool          isEqual(int x_, int y_) {return ((x == x_) && (y == y_));}
  void          getValue(int& x_, int& y_) {x_ = x; y_ = y;}
  void          setValue(int x_, int y_) {x = x_; y = y_;}

  iVec2i() {x = 0; y = 0;}
  iVec2i(int x_, int y_) {x = x_; y = y_;}
  iVec2i(int v) {x = v; y = v;}
  iVec2i(const iVec2i& cp) {x = cp.x; y = cp.y;}

  iVec2i& operator =(const iVec2i& cp) {x = cp.x; y = cp.y; return *this;}
  iVec2i& operator =(int v) {x = v; y = v; return *this;}

#ifdef TA_GUI
  iVec2i(const QPoint& val);
  iVec2i& operator=(const QPoint& val);
  operator QPoint() const;
#endif
#ifdef TA_USE_INVENTOR
  iVec2i(const SbVec2s& src);
  iVec2i&       operator=(const SbVec2s& src);
  operator SbVec2s() const; // note: will always be in range in context of gui
#endif
};

typedef iVec2i iPoint; // synonym

#endif // iVec2i_h
