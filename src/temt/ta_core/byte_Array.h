// Co2018ght 2005-2017, Regents of the University of Colorado,
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

#ifndef byte_Array_h
#define byte_Array_h 1

// parent includes:
#include <taArray>

// member includes:

// declare all other types mentioned but not required to include:

taTypeDef_Of(byte_Array);

class TA_API byte_Array : public taArray<byte> {
  // #NO_UPDATE_AFTER #NO_TOKENS array of bytes (unsigned chars)
INHERITED(taArray<byte>)
public:
  static const byte blank; // #HIDDEN #READ_ONLY

  NOCOPY(byte_Array)
  void Initialize()     {err = 0; };
  void Destroy()        { }; //
  //note: Register() is not necessary for arrays, so we omit in these convenience constructors
  TA_BASEFUNS(byte_Array);
  TA_ARRAY_FUNS(byte_Array, byte)
protected:
  Variant      El_GetVar_(const void* itm) const override
    { return (Variant)(*(byte*)itm); }
  int           El_Compare_(const void* a, const void* b) const override
  { int rval=-1; if(*((byte*)a) > *((byte*)b)) rval=1; else if(*((byte*)a) == *((byte*)b)) rval=0; return rval; }
  bool          El_Equal_(const void* a, const void* b) const override
    { return (*((byte*)a) == *((byte*)b)); }
  String        El_GetStr_(const void* it) const override { return String((int)(*((byte*)it))); }
  void          El_SetFmStr_(void* it, const String& val) override
  { *((byte*)it) = (byte)val.toInt(); }
};

TA_ARRAY_OPS(TA_API, byte_Array)

#endif // byte_Array_h
