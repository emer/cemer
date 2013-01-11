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

#include "NameVar_Array.h"
#include <String_Array>

const String NameVar_Array::def_sep(", ");
const NameVar NameVar_Array::blank;

int NameVar_Array::FindName(const String& op, int start) const {
  int i;
  if(start < 0) {               // search backwards if start < 0
    for(i=size-1; i>=0; i--) {
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

int NameVar_Array::FindNameContains(const String& op, int start) const {
  int i;
  if(start < 0) {               // search backwards if start < 0
    for(i=size-1; i>=0; i--) {
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

int NameVar_Array::FindValue(const Variant& op, int start) const {
  int i;
  if(start < 0) {               // search backwards if start < 0
    for(i=size-1; i>=0; i--) {
      if(FastEl(i).value == op)
        return i;
    }
  }
  else {
    for(i=start; i<size; i++) {
      if(FastEl(i).value == op)
        return i;
    }
  }
  return -1;
}

int NameVar_Array::FindValueContains(const String& op, int start) const {
  int i;
  if(start < 0) {               // search backwards if start < 0
    for(i=size-1; i>=0; i--) {
      if(FastEl(i).value.toString().contains(op))
        return i;
    }
  }
  else {
    for(i=start; i<size; i++) {
      if(FastEl(i).value.toString().contains(op))
        return i;
    }
  }
  return -1;
}

Variant NameVar_Array::GetVal(const String& nm) {
  int idx = FindName(nm);
  if(idx < 0) return _nilVariant;
  return FastEl(idx).value;
}

bool NameVar_Array::GetAllVals(const String& nm, String_Array& vals) {
  for(int i=0;i<size; i++) {
    NameVar& nv = FastEl(i);
    if(nv.name != nm) continue;
    vals.Add(nv.value.toString());
  }
  return (vals.size == 0);
}

bool NameVar_Array::SetVal(const String& nm, const Variant& vl) {
  int idx = FindName(nm);
  if(idx < 0) {
    Add(NameVar(nm, vl));
    return true;
  }
  else {
    FastEl(idx).value = vl;
    return false;
  }
}
