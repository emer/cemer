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

#ifndef iSize_h
#define iSize_h 1

#include "taiqtso_def.h"

class QSize; //

TypeDef_Of(iSize);

class TAIQTSO_API iSize { // #NO_CSS #NO_TOKENS #INSTANCE #EDIT_INLINE
public:
  int w;
  int h;

  void          set(int w_, int h_) {w = w_; h = h_;}

  iSize();
  iSize(int w_, int h_);

  int height() {return h;}
  int width() {return w;}

#ifdef TA_GUI
  iSize(const QSize& val);
  iSize& operator=(const QSize& val);
  operator QSize() const;
#endif
};

#endif // iSize_h
