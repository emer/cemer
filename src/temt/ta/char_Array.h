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

#ifndef char_Array_h
#define char_Array_h 1

// parent includes:
#include <taArray>

// member includes:

// declare all other types mentioned but not required to include:


taTypeDef_Of(char_Array);

class TA_API char_Array : public taArray<char> {
  // #NO_UPDATE_AFTER #NO_TOKENS array of chars (bytes)
INHERITED(taArray<char>)
public:
  static const char blank; // #HIDDEN #READ_ONLY
  TA_BASEFUNS(char_Array);
  TA_ARRAY_FUNS(char_Array, char)
protected:
  Variant      El_GetVar_(const void* itm) const override
    { return (Variant)(*(char*)itm); }
  int           El_Compare_(const void* a, const void* b) const
  { int rval=-1; if(*((char*)a) > *((char*)b)) rval=1; else if(*((char*)a) == *((char*)b)) rval=0; return rval; }
  bool          El_Equal_(const void* a, const void* b) const
    { return (*((char*)a) == *((char*)b)); }
  String        El_GetStr_(const void* it) const { return String((int)*((char*)it)); }
  void          El_SetFmStr_(void* it, const String& val)
  { int tmp = (int)val; *((char*)it) = (char)tmp; }
private:
  NOCOPY(char_Array)
  void Initialize()     {err = ' ';};
  void Destroy()        { };
};

TA_ARRAY_OPS(TA_API, char_Array)

#endif // char_Array_h
