// Copyright 2013-2018, Regents of the University of Colorado,
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

#ifndef double_Array_h
#define double_Array_h 1

// parent includes:
#include <taArray>

// member includes:

// declare all other types mentioned but not required to include:


taTypeDef_Of(double_Array);

class TA_API double_Array : public taArray<double> {
  // #NO_UPDATE_AFTER #NO_TOKENS array of doubles
INHERITED(taArray<double>)
public:
  static const double blank; // #HIDDEN #READ_ONLY
  TA_BASEFUNS_NOCOPY(double_Array);
  TA_ARRAY_FUNS(double_Array, double)
protected:
  Variant      El_GetVar_(const void* itm) const override
    { return (Variant)(*(double*)itm); }
  int           El_Compare_(const void* a, const void* b) const override
  { int rval=-1; if(*((double*)a) > *((double*)b)) rval=1; else if(*((double*)a) == *((double*)b)) rval=0; return rval; }
  bool          El_Equal_(const void* a, const void* b) const override
    { return (*((double*)a) == *((double*)b)); }
  String        El_GetStr_(const void* it) const override { return (*((double*)it)); }
  void          El_SetFmStr_(void* it, const String& val) override
  { double tmpv = (double)val; *((double*)it) = tmpv; }
private:
  void Initialize()     {err = 0.0;};
  void Destroy()        { };
};

TA_ARRAY_OPS(TA_API, double_Array)

#endif // double_Array_h
