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

#ifndef int_PArray_h
#define int_PArray_h 1

// parent includes:
#include <taPlainArray>

// member includes:

// declare all other types mentioned but not required to include:


taTypeDef_Of(int_PArray);

class TA_API int_PArray: public taPlainArray<int> {
  // #NO_TOKENS a plain-array of ints
public:
  void  InitVals(int val = 0)
  { for(int i=0; i<size; i++) FastEl(i) = val; }

  void  operator=(const int_PArray& cp) { Copy_Duplicate(cp); }
  int_PArray()                          { };
  int_PArray(const int_PArray& cp)      { Copy_Duplicate(cp); }

protected:
  int           El_Compare_(const void* a, const void* b) const
  { int rval=-1; if(*((int*)a) > *((int*)b)) rval=1; else if(*((int*)a) == *((int*)b)) rval=0; return rval; }
  bool          El_Equal_(const void* a, const void* b) const
    { return (*((int*)a) == *((int*)b)); }
  String        El_GetStr_(const void* it) const { return (*((int*)it)); }
  void          El_SetFmStr_(void* it, const String& val)
  { int tmp = (int)val; *((int*)it) = tmp; }
};

#endif // int_PArray_h
