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

#include "taVersion.h"

void taVersion::setFromString(String ver) {
  Clear();
  // parse, mj.mn.st-build -- just blindly go through, harmless if missings
  major = BeforeOrOf('.', ver);
  minor = BeforeOrOf('.', ver);
  step = BeforeOrOf('-', ver);
  build = BeforeOrOf(' ', ver); //dummy
}

int taVersion::BeforeOrOf(char sep, String& in) {
  String s = in.before(sep);
  if (s.nonempty()) {
    in = in.after(sep);
  } else {
    s = in;
    in = _nilString;
  }
  // strip any gunk, which may be at end, ex "b1" etc.
  int i = 0;
  while (i < s.length()) {
    char c = s[i];
    if (!isdigit(c)) break;
    ++i;
  }
  return s.left(i).toInt();
}
