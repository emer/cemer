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

#ifndef ContextFlag_h
#define ContextFlag_h 1

// parent includes:
#include "ta_def.h"

// member includes:

// declare all other types mentioned but not required to include:

#ifdef __MAKETA__
#define ContextFlag unsigned char
#else
class TA_API ContextFlag {
  // ##SMART_INT replacement for is_xxx flags that retains bool test semantics, but does proper enter/exit counting; use in ++ ... -- pairs
public:
  operator bool() {return (cnt != 0);}
  signed char   operator++() {return ++cnt;}
  signed char   operator++(int) {return cnt++;} // post
  signed char   operator--() {return --cnt;}
  signed char   operator--(int)  {return cnt--;} // post

  ContextFlag() {cnt = 0;} // NOTE: default binary copy constructor and copy operator are fine
private:
  signed char   cnt; // keep same size as bool -- should never be nesting this deep
};
#endif // __MAKETA__

#endif // ContextFlag_h
