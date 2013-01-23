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

#ifndef voidptr_PArray_h
#define voidptr_PArray_h 1

// parent includes:
#include <taPlainArray>

// member includes:

// declare all other types mentioned but not required to include:


TypeDef_Of(voidptr_PArray);

class TA_API voidptr_PArray: public taPlainArray<voidptr> {
  // #NO_TOKENS #NO_MEMBERS #NO_CSS a plain-array of void* pointers
public:
  void  operator=(const voidptr_PArray& cp)     { Copy_Duplicate(cp); }
  voidptr_PArray()                              { };
  voidptr_PArray(const voidptr_PArray& cp)      { Copy_Duplicate(cp); }

protected:
  int           El_Compare_(const void* a, const void* b) const
  { int rval=-1; if(*((void**)a) > *((void**)b)) rval=1; else if(*((void**)a) == *((void**)b)) rval=0; return rval; }
  bool          El_Equal_(const void* a, const void* b) const
    { return (*((void**)a) == *((void**)b)); }
  String        El_GetStr_(const void* it) const { return *((ta_uintptr_t*)it); }
  void          El_SetFmStr_(void* it, const String& val)
  { ta_uintptr_t tmp = (ta_uintptr_t)val; *((ta_uintptr_t*)it) = tmp; }
  //note: dangerous, but needed to clear (if str empty)
};

#endif // voidptr_PArray_h
