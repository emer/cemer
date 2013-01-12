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

#ifndef iRect_h
#define iRect_h 1

#include "taiqtso_def.h"

/* Notes:
   1. all classes below are value-semantics, and use the implicit copy and = operators.
   2. MAKETA cannot boggle the : class(): x(), y() {} form of constructor initialialization,
      so they have been moved to the .cc file.
   3. DO NOT add any virtual methods to these value-semantic classes
*/

#ifdef TA_USE_INVENTOR
class SbVec2s;          // #IGNORE
class SbVec3s;          // #IGNORE
#endif

class TAIQTSO_API iRect { // #NO_CSS #NO_TOKENS #INSTANCE #EDIT_INLINE
public:
  int x;
  int y;
  int w;
  int h;

  iRect();
  iRect(const iPoint& topLeft, const iPoint& bottomRight);
  iRect(const iPoint& topLeft, const iSize& size );
  iRect(int left, int top, int width, int height);

  iPoint topLeft() const {return iPoint(x, y);}
  iSize size() const {return iSize(w, h);}
  int left() const {return x;}
  int top() const {return y;}
  int width() const {return w;}
  int height() const {return h;}

#ifdef TA_GUI
  iRect(const QRect& val);
  iRect& operator=(const QRect& val);
  operator QRect() const;
#endif

};

#endif // iRect_h
