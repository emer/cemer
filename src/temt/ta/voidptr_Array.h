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

#ifndef voidptr_Array_h
#define voidptr_Array_h 1

// parent includes:
#include <taArray>

// member includes:

// declare all other types mentioned but not required to include:


TypeDef_Of(voidptr_Array);

class TA_API voidptr_Array : public taArray<voidptr> {
  // #NO_UPDATE_AFTER #NO_TOKENS array of void pointers
INHERITED(taArray<voidptr>)
public:
  STATIC_CONST voidptr blank; // #HIDDEN #READ_ONLY

  TA_BASEFUNS_NOCOPY(voidptr_Array);
  TA_ARRAY_FUNS(voidptr_Array, voidptr)
protected:
  override Variant      El_GetVar_(const void* itm) const
    { return (Variant)(*(voidptr*)itm); }
  bool          El_Equal_(const void* a, const void* b) const
    { return (*((voidptr*)a) == *((voidptr*)b)); }
private:
  void Initialize()     {err = 0; };
  void Destroy()        { };
};

#endif // voidptr_Array_h
