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

#include "iSize.h"

#ifdef TA_GUI
  #include "qsize.h"

iSize::iSize(const QSize& val)
: w(val.width()), h(val.height())
{
}

iSize& iSize::operator=(const QSize& val) {
  w = val.width();
  h = val.height();
  return *this;
}

iSize::operator QSize() const {
  return QSize(w, h);
}

#endif

iSize::iSize(): w(0), h(0) {}
iSize::iSize(int w_, int h_): w(w_), h(h_) {}

