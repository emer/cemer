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

#ifndef int_Matrix_h
#define int_Matrix_h 1

// parent includes:
#include <taMatrixT>

// member includes:
#include <BuiltinTypeDefs>

// declare all other types mentioned but not required to include:
class TypeDef; // 
class MatrixIndex; // 


TypeDef_Of(int_Matrix);

class TA_API int_Matrix: public taMatrixT<int> {
  // #INSTANCE a matrix of ints
INHERITED(taMatrixT<int>)
public:
  override TypeDef*     GetDataTypeDef() const {return &TA_int;}
  override ValType      GetDataValType() const {return VT_INT;}

  override bool         StrValIsValid(const String& str, String* err_msg = NULL) const;
    // accepts in-range for 32bit int

  virtual void          InitVals(int val=0)
  { for(int i=0;i<size;i++) FastEl_Flat(i) = val; }
  // initialize values to given fixed value

  virtual void		InitFromInts(int* vals, int n=-1)
  { int eff_n = size; if(n > 0) { eff_n = n; if(size < n) SetGeom(1,n); }
    for(int i=0;i<size;i++) FastEl_Flat(i) = vals[i]; }
  // initialize values from an array of ints, with optional number parameter n (if unspecified or -1, then array is assumed to be size of matrix)

  override bool         BinaryFile_Supported() { return true; }

  TA_MATRIX_FUNS_FAST(int_Matrix, int);

public:
  override float        El_GetFloat_(const void* it) const { return (float)*((int*)it); } // #IGNORE
  override double       El_GetDouble_(const void* it) const
    { return (double)*((int*)it); } // #IGNORE
  override const String El_GetStr_(const void* it) const { return *((int*)it); } // #IGNORE note: implicit conversion avoids problems on some compilers
  override void         El_SetFmStr_(void* it, const String& str) {*((int*)it) = (int)str;}  // #IGNORE
  override const Variant El_GetVar_(const void* it) const {return Variant(*((int*)it));} // #IGNORE
  override void         El_SetFmVar_(void* it, const Variant& var) {*((int*)it) = var.toInt(); };  // #IGNORE
protected:
  STATIC_CONST int      blank; // #IGNORE
#ifndef __MAKETA__
  override void         BinarySave_Item(std::ostream& strm, int idx)
  { strm.write((char*)&(FastEl_Flat(idx)), sizeof(int)); };
  override void         BinaryLoad_Item(std::istream& strm, int idx)
  { strm.read((char*)&(FastEl_Flat(idx)), sizeof(int)); };
#endif
private:
  void          Initialize() {}
  void          Destroy() { CutLinks(); }
};

TA_SMART_PTRS(int_Matrix);

#endif // int_Matrix_h
