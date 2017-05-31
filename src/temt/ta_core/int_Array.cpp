// Copyright 2015, Regents of the University of Colorado,
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

#include "int_Array.h"

TA_BASEFUNS_CTORS_DEFN(int_Array);
SMARTREF_OF_CPP(int_Array)


void int_Array::FillSeq(int start, int inc) {
  int i, v;
  for(i=0,v=start; i<size; i++, v += inc)
    FastEl(i) = v;
}

const int int_Array::blank = 0;
