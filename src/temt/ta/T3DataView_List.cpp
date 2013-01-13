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

#include "T3DataView_List.h"
#include <T3DataView>

T3DataView* T3DataView_List::FindData(taBase* dat, int& idx) {
  for(idx=0; idx<size; idx++) {
    T3DataView* dv = FastEl(idx);
    if(dv->data() == dat) {
      return dv;
    }
  }
  idx = -1;
  return NULL;
}

