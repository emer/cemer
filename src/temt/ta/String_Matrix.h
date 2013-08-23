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

#ifndef String_Matrix_h
#define String_Matrix_h 1

// parent includes:
#include <taString>
#include <taMatrixT>

// member includes:

// declare all other types mentioned but not required to include:
class TypeDef; // 
class MatrixIndex; // 


taTypeDef_Of(String_Matrix);

class TA_API String_Matrix: public taMatrixT<String> {
  // #INSTANCE a matrix of strings
INHERITED(taMatrixT<String>)
public:
  override int          defAlignment() const;
  override TypeDef*     GetDataTypeDef() const {return &TA_taString;}
  override ValType      GetDataValType() const {return VT_STRING;}

  String        ToDelimString(const String& delim = " ");
  // generates a string of all the items on the list, using given delimiter between items
  void          FmDelimString(const String& str, const String& delim = " ");
  // fill this matrix by parsing given string using given delimiter separating strings -- increases size as necessary to fit everything

  virtual void		InitFromChars(const char** vals, int n=-1)
  { int eff_n = size; if(n > 0) { eff_n = n; if(size < n) SetGeom(1,n); }
    for(int i=0;i<eff_n;i++) FastEl_Flat(i) = vals[i]; }
  // initialize values from an array of char* strings, with optional number parameter n (if unspecified or -1, then array is assumed to be size of matrix)

  TA_MATRIX_FUNS_SLOW(String_Matrix, String)

public:
  override float        El_GetFloat_(const void* it) const
    { return ((String*)it)->toFloat(); } // #IGNORE
  override const String El_GetStr_(const void* it) const {return *((String*)it); } // #IGNORE
  override void         El_SetFmStr_(void* it, const String& str) {*((String*)it) = str;}  // #IGNORE
  override const Variant El_GetVar_(const void* it) const {return Variant(*((String*)it));} // #IGNORE
  override void         El_SetFmVar_(void* it, const Variant& var) {*((String*)it) = var.toString(); };  // #IGNORE
  override int          El_Compare_(const void* a, const void* b) const
  { int rval=-1; if(*((String*)a) > *((String*)b)) rval=1; else if(*((String*)a) == *((String*)b)) rval=0; return rval; }
protected:
  static const String   blank; // #IGNORE
#ifndef __MAKETA__
  override void         Dump_Save_Item(std::ostream& strm, int idx);
  override int          Dump_Load_Item(std::istream& strm, int idx);
#endif
  override void         ReclaimOrphans_(int from, int to); // called when elements can be reclaimed, ex. for strings

private:
  void          Initialize() {}
  void          Destroy() { CutLinks(); }
};

TA_SMART_PTRS(String_Matrix);

#endif // String_Matrix_h
