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

#ifndef int_Array_h
#define int_Array_h 1

// parent includes:
#include <taArray>

// member includes:

// declare all other types mentioned but not required to include:


TypeDef_Of(int_Array);

class TA_API int_Array : public taArray<int> {
  // #NO_UPDATE_AFTER #NO_TOKENS array of ints
INHERITED(taArray<int>)
public:
  STATIC_CONST int blank; // #HIDDEN #READ_ONLY
  virtual void  FillSeq(int start=0, int inc=1);
  // fill array with sequential values starting at start, incrementing by inc

  NOCOPY(int_Array)
  void Initialize()     {err = 0; };
  void Destroy()        { }; //
  //note: Register() is not necessary for arrays, so we omit in these convenience constructors
  int_Array(int num, int i0) {Initialize(); SetSize(1); el[0] = i0;}
  int_Array(int num, int i0, int i1) {Initialize(); SetSize(2); el[0] = i0; el[1] = i1;}
  int_Array(int num, int i0, int i1, int i2)
    {Initialize(); SetSize(3); el[0] = i0; el[1] = i1; el[2] = i2;}
  int_Array(int num, int i0, int i1, int i2, int i3)
    {Initialize(); SetSize(4); el[0] = i0; el[1] = i1; el[2] = i2; el[3] = i3;}
  TA_BASEFUNS(int_Array);
  TA_ARRAY_FUNS(int_Array, int)
protected:
  override Variant      El_GetVar_(const void* itm) const
    { return (Variant)(*(int*)itm); }
  int           El_Compare_(const void* a, const void* b) const
  { int rval=-1; if(*((int*)a) > *((int*)b)) rval=1; else if(*((int*)a) == *((int*)b)) rval=0; return rval; }
  bool          El_Equal_(const void* a, const void* b) const
    { return (*((int*)a) == *((int*)b)); }
  String        El_GetStr_(const void* it) const { return (*((int*)it)); }
  void          El_SetFmStr_(void* it, const String& val)
  { int tmp = (int)val; *((int*)it) = tmp; }
};

TA_ARRAY_OPS(int_Array)

#endif // int_Array_h
