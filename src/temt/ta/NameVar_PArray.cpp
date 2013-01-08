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

#include "NameVar_PArray.h"
#include <String_PArray>

const String NameVar_PArray::def_sep(", ");

const String NameVar_PArray::AsString(const String& sep) const {
  if (size == 0) return _nilString;
  int i;
  String rval;
  for (i = 0; i < size; ++i) {
    if (i > 0)
      rval.cat(sep);
    rval.cat(FastEl(i).GetStr());
  }
  return rval;
}

int NameVar_PArray::FindName(const String& op, int start) const {
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

int NameVar_PArray::FindNameContains(const String& op, int start) const {
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

int NameVar_PArray::FindValue(const Variant& op, int start) const {
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

int NameVar_PArray::FindValueContains(const String& op, int start) const {
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

Variant NameVar_PArray::GetVal(const String& nm) {
  int idx = FindName(nm);
  if(idx < 0) return _nilVariant;
  return FastEl(idx).value;
}

Variant NameVar_PArray::GetValDef(const String& nm, const Variant& def) {
  int idx = FindName(nm);
  if (idx < 0) return def;
  return FastEl(idx).value;
}


bool NameVar_PArray::GetAllVals(const String& nm, String_PArray& vals) {
  for(int i=0;i<size; i++) {
    NameVar& nv = FastEl(i);
    if(nv.name != nm) continue;
    vals.Add(nv.value.toString());
  }
  return (vals.size == 0);
}

bool NameVar_PArray::SetVal(const String& nm, const Variant& vl) {
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
