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

#ifndef bool_Array_h
#define bool_Array_h 1

// parent includes:
#include <taArray>

// member includes:

// declare all other types mentioned but not required to include:

taTypeDef_Of(bool_Array);

class TA_API bool_Array : public taArray<bool>  {
  // #NO_UPDATE_AFTER #NO_TOKENS array of bools
INHERITED(taArray<bool>)
public:
  static const bool blank; // #HIDDEN #READ_ONLY

  TA_BASEFUNS_NOCOPY(bool_Array);
  TA_ARRAY_FUNS(bool_Array, bool)
protected:
  Variant       El_GetVar_(const void* itm) const override
  { return (Variant)(*(bool*)itm); }
  int           El_Compare_(const void* a, const void* b) const
  { int rval=-1; if(*((bool*)a) > *((bool*)b)) rval=1; else if(*((bool*)a) == *((bool*)b)) rval=0; return rval; }
  bool          El_Equal_(const void* a, const void* b) const
  { return (*((bool*)a) == *((bool*)b)); }
  String        El_GetStr_(const void* it) const { return (*((bool*)it)); }
  void          El_SetFmStr_(void* it, const String& val)
  { bool tmp = (bool)val; *((bool*)it) = tmp; }
  
private:
  void Initialize()     {err = false; };
  void Destroy()        { };

};

TA_ARRAY_OPS(TA_API, bool_Array)

#endif // bool_Array_h
