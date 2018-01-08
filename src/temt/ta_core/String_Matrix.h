// Co2018ght 2013-2017, Regents of the University of Colorado,
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
  // #INSTANCE a Matrix of string values
INHERITED(taMatrixT<String>)
public:
  int          defAlignment() const override;
  TypeDef*     GetDataTypeDef() const override {return &TA_taString;}
  ValType      GetDataValType() const override {return VT_STRING;}

  String        ToDelimString(const String& delim = " ");
  // generates a string of all the items on the list, using given delimiter between items
  String        Join(const String& delim = " ")
  { return ToDelimString(delim); }
  // generates a string of all the items on the list, using given delimiter between items (same as ToDelimString)

  void          FmDelimString(const String& str, const String& delim = " ");
  // fill this matrix by parsing given string using given delimiter separating strings -- increases size as necessary to fit everything
  void          Split(const String& str, const String& delim = "");
  // Convenience method that calls FmDelimString for splitting a string into an array. Splits on every character by default.

  virtual void		InitFromChars(const char** vals, int n=-1)
  { int eff_n = size; if(n > 0) { eff_n = n; if(size < n) SetGeom(1,n); }
    for(int i=0;i<eff_n;i++) FastEl_Flat(i) = vals[i]; }
  // #IGNORE initialize values from an array of char* strings, with optional number parameter n (if unspecified or -1, then array is assumed to be size of matrix)

  TA_MATRIX_FUNS_SLOW(String_Matrix, String)

public:
  float        El_GetFloat_(const void* it) const override
    { return ((String*)it)->toFloat(); } // #IGNORE
  const String El_GetStr_(const void* it) const override {return *((String*)it); } // #IGNORE
  void         El_SetFmStr_(void* it, const String& str) override {*((String*)it) = str;}  // #IGNORE
  const Variant El_GetVar_(const void* it) const override {return Variant(*((String*)it));} // #IGNORE
  void         El_SetFmVar_(void* it, const Variant& var) override {*((String*)it) = var.toString(); };  // #IGNORE
  int          El_Compare_(const void* a, const void* b) const override
  { int rval=-1; if(*((String*)a) > *((String*)b)) rval=1; else if(*((String*)a) == *((String*)b)) rval=0; return rval; }
protected:
  static const String   blank; // #IGNORE
#ifndef __MAKETA__
  void         Dump_Save_Item(std::ostream& strm, int idx) override;
  int          Dump_Load_Item(std::istream& strm, int idx) override;
#endif
  void         ReclaimOrphans_(int from, int to) override; // called when elements can be reclaimed, ex. for strings

private:
  void          Initialize() {}
  void          Destroy() { CutLinks(); }
};

TA_SMART_PTRS(TA_API, String_Matrix);

#endif // String_Matrix_h
