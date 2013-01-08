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

#ifndef String_PArray_h
#define String_PArray_h 1

// parent includes:
#include <taString>
#include <taPlainArray>

// member includes:

// declare all other types mentioned but not required to include:


class TA_API String_PArray : public taPlainArray<String> {
  // #NO_TOKENS a plain-array of strings
INHERITED(taPlainArray<String>)
public:
  static const String   def_sep; // ", "

  int   FindContains(const String& op, int start=0) const;
  // find item that contains string -- start < 0 = start from end of array (not strings!)
  int   FindStartsWith(const String& op, int start=0) const;
  // find item that starts with string -- start < 0 = start from end of array (not strings!)

  const String  AsString(const String& sep = def_sep) const;
  void          SetFromString(String str, const String& sep = def_sep);
  void  operator=(const String_PArray& cp)      { Copy_Duplicate(cp); }
  String_PArray()                               { };
  String_PArray(const String_PArray& cp)        { Copy_Duplicate(cp); }
  // returns first item which contains given string (-1 if none)
protected:
  int           El_Compare_(const void* a, const void* b) const
  { int rval=-1; if(*((String*)a) > *((String*)b)) rval=1; else if(*((String*)a) == *((String*)b)) rval=0; return rval; }
  bool          El_Equal_(const void* a, const void* b) const
    { return (*((String*)a) == *((String*)b)); }
  String        El_GetStr_(const void* it) const { return (*((String*)it)); }
  void          El_SetFmStr_(void* it, const String& val)
  {*((String*)it) = val; }
};

#endif // String_PArray_h
