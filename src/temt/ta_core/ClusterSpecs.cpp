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

#include "ClusterSpecs.h"
#include <TypeDef>

void ClusterSpecs::SetFmStr(const String& val) {
  TA_ClusterSpecs.SetValStr_class_inline(val, (void*)this);
}

String ClusterSpecs::GetStr() const {
  return TA_ClusterSpecs.GetValStr_class_inline((void*)this);
}


int ClusterSpecs_PArray::FindName(const String& op, int start) const {
  int i;
  if(start < 0) {               // search backwards if start < 0
    for(i=size-start; i>=0; i--) {
      if(FastEl(i).name == op)
        return i;
    }
  }
  else {
    for(i=start; i<size; i++) {
      if(FastEl(i).name == op)
        return i;
    }
  }
  return -1;
}

int ClusterSpecs_PArray::FindNameContains(const String& op, int start) const {
  int i;
  if(start < 0) {               // search backwards if start < 0
    for(i=size-start; i>=0; i--) {
      if(FastEl(i).name.contains(op))
        return i;
    }
  }
  else {
    for(i=start; i<size; i++) {
      if(FastEl(i).name.contains(op))
        return i;
    }
  }
  return -1;
}
