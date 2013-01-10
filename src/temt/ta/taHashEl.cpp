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

#include "taHashEl.h"

taHashVal taHashEl::HashCode_String(const String& string_) {
  // now using the one that Qt uses, comment is:
  // These functions are based on Peter J. Weinberger's hash function (from the
  // Dragon Book). The constant 24 in the original function was replaced with 23
  // to produce fewer collisions on input such as "a", "aa", "aaa", "aaaa", ...

  taHashVal hash = 0;
  const char* string = string_.chars();
  taHashVal g;
  while (1) {
    unsigned int c = *string;
    string++;
    if (c == 0) {
      break;
    }
    hash += (hash<<4) + c;
    if ((g = (hash & 0xf0000000)) != 0)
      hash ^= g >> 23;
    hash &= ~g;
  }
  return hash;
}
