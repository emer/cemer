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

#include "String_PArray.h"

const String String_PArray::def_sep(", ");

const String String_PArray::AsString(const String& sep) const {
  if (size == 0) return _nilString;
  // more efficient to know the length, so we don't resize...
  int xlen = MAX(0, (size - 1) * sep.length()); // for seps
  int i;
  for (i=0; i < size; ++i) xlen += FastEl(i).length();
  String rval(0, xlen, '\0');
  for (i = 0; i < size; ++i) {
    if (i > 0)
      rval.cat(sep);
    rval.cat(FastEl(i));
  }
  return rval;
}

int String_PArray::FindContains(const String& op, int start) const {
  int i;
  if(start < 0) {               // search backwards if start < 0
    for(i=size-1; i>=0; i--) {
      if(FastEl(i).contains(op))
        return i;
    }
  }
  else {
    for(i=start; i<size; i++) {
      if(FastEl(i).contains(op))
        return i;
    }
  }
  return -1;
}

int String_PArray::FindStartsWith(const String& op, int start) const {
  int i;
  if(start < 0) {               // search backwards if start < 0
    for(i=size-1; i>=0; i--) {
      if(FastEl(i).startsWith(op))
        return i;
    }
  }
  else {
    for(i=start; i<size; i++) {
      if(FastEl(i).startsWith(op))
        return i;
    }
  }
  return -1;
}

void String_PArray::SetFromString(String str, const String& sep) {
  Reset();
  int pos = str.index(sep);
  while (pos >= 0) {
    Add(str.left(pos)); // could be empty
    str = str.after(pos + sep.length() - 1);
    pos = str.index(sep);
  }
  if (str.nonempty())
    Add(str);
}

