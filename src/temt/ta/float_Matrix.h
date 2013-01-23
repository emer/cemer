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

#ifndef float_Matrix_h
#define float_Matrix_h 1

// parent includes:
#include <taMatrixT>

// member includes:

// declare all other types mentioned but not required to include:
class TypeDef; // 
class MatrixIndex; // 


TypeDef_Of(float_Matrix);

class TA_API float_Matrix: public taMatrixT<float> {
  // #INSTANCE a matrix of floats
INHERITED(taMatrixT<float>)
public:
  override TypeDef*     GetDataTypeDef() const {return &TA_float;}
  override ValType      GetDataValType() const {return VT_FLOAT;}

  override bool         StrValIsValid(const String& str, String* err_msg = NULL) const;
    // accepts valid format for float

  virtual void          InitVals(float val=0.0)
  { for(int i=0;i<size;i++) FastEl_Flat(i) = val; }
  // initialize values to given fixed value
  virtual void		InitFromFloats(float* vals, int n=-1)
  { int eff_n = size; if(n > 0) { eff_n = n; if(size < n) SetGeom(1,n); }
    for(int i=0;i<size;i++) FastEl_Flat(i) = vals[i]; }
  // initialize values from an array of floats, with optional number parameter n (if unspecified or -1, then array is assumed to be size of matrix)

  override bool         BinaryFile_Supported() { return true; }

  TA_MATRIX_FUNS_FAST(float_Matrix, float);
public:
  override float        El_GetFloat_(const void* it) const { return *((float*)it); } // #IGNORE
  override const String El_GetStr_(const void* it) const { return (String)*((float*)it); } // #IGNORE
  override void         El_SetFmStr_(void* it, const String& str) {*((float*)it) = (float)str;}  // #IGNORE
  override const Variant El_GetVar_(const void* it) const {return Variant(*((float*)it));} // #IGNORE
  override void         El_SetFmVar_(void* it, const Variant& var) {*((float*)it) = var.toFloat(); };  // #IGNORE
protected:
  STATIC_CONST float    blank; // #IGNORE
#ifndef __MAKETA__
  override void         Dump_Save_Item(std::ostream& strm, int idx);
  // stream in full precision
  override void         BinarySave_Item(std::ostream& strm, int idx)
  { strm.write((char*)&(FastEl_Flat(idx)), sizeof(float)); };
  override void         BinaryLoad_Item(std::istream& strm, int idx)
  { strm.read((char*)&(FastEl_Flat(idx)), sizeof(float)); };
#endif
private:
  void          Initialize() {}
  void          Destroy() { CutLinks(); }
};

TA_SMART_PTRS(float_Matrix);

#endif // float_Matrix_h
