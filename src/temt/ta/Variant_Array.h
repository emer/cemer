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

#ifndef Variant_Array_h
#define Variant_Array_h 1

// parent includes:
#include <Variant>
#include <taArray>

// member includes:

// declare all other types mentioned but not required to include:


taTypeDef_Of(Variant_Array);

class TA_API Variant_Array : public taArray<Variant> {
  // #NO_UPDATE_AFTER #NO_TOKENS array of variants
INHERITED(taArray<Variant>)
public:
  static const Variant blank; // #HIDDEN #READ_ONLY
  TA_BASEFUNS_NOCOPY(Variant_Array);
  TA_ARRAY_FUNS(Variant_Array, Variant)
protected:
  Variant      El_GetVar_(const void* itm) const override
    { return (*(Variant*)itm); }
  int           El_Compare_(const void* a, const void* b) const
  { int rval=-1; if(*((Variant*)a) > *((Variant*)b)) rval=1; else if(*((Variant*)a) == *((Variant*)b)) rval=0; return rval; }
  bool          El_Equal_(const void* a, const void* b) const
    { return (*((Variant*)a) == *((Variant*)b)); }
  String        El_GetStr_(const void* it) const { return ((Variant*)it)->toString(); }
  void          El_SetFmStr_(void* it, const String& val)
  { Variant tmp = (Variant)val; *((Variant*)it) = tmp; }
private:
  void Initialize()     {err = 0.0;};
  void Destroy()        { };
};

TA_ARRAY_OPS(Variant_Array)

#endif // Variant_Array_h
