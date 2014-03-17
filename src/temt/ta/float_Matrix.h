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


taTypeDef_Of(float_Matrix);

class TA_API float_Matrix: public taMatrixT<float> {
  // #INSTANCE a matrix of floats
INHERITED(taMatrixT<float>)
public:
  TypeDef*     GetDataTypeDef() const override {return &TA_float;}
  ValType      GetDataValType() const override {return VT_FLOAT;}

  bool         StrValIsValid(const String& str, String* err_msg = NULL) const override;
    // accepts valid format for float

  virtual void          InitVals(float val=0.0)
  { for(int i=0;i<size;i++) FastEl_Flat(i) = val; }
  // initialize values to given fixed value
  virtual void		InitFromFloats(float* vals, int n=-1)
  { int eff_n = size; if(n > 0) { eff_n = n; if(size < n) SetGeom(1,n); }
    for(int i=0;i<eff_n;i++) FastEl_Flat(i) = vals[i]; }
  // initialize values from an array of floats, with optional number parameter n (if unspecified or -1, then array is assumed to be size of matrix)

  bool         BinaryFile_Supported() override { return true; }

  TA_MATRIX_FUNS_FAST(float_Matrix, float);
public:
  float        El_GetFloat_(const void* it) const override { return *((float*)it); } // #IGNORE
  const String El_GetStr_(const void* it) const override { return (String)*((float*)it); } // #IGNORE
  void         El_SetFmStr_(void* it, const String& str) override {*((float*)it) = (float)str;}  // #IGNORE
  const Variant El_GetVar_(const void* it) const override {return Variant(*((float*)it));} // #IGNORE
  void         El_SetFmVar_(void* it, const Variant& var) override {*((float*)it) = var.toFloat(); };  // #IGNORE
  int          El_Compare_(const void* a, const void* b) const override
  { int rval=-1; if(*((float*)a) > *((float*)b)) rval=1; else if(*((float*)a) == *((float*)b)) rval=0; return rval; }
protected:
  static const float    blank; // #IGNORE
  void         Dump_Save_Item(std::ostream& strm, int idx) override;
  // stream in full precision
  void         BinarySave_Item(std::ostream& strm, int idx) override
  { strm.write((char*)&(FastEl_Flat(idx)), sizeof(float)); };
  void         BinaryLoad_Item(std::istream& strm, int idx) override
  { strm.read((char*)&(FastEl_Flat(idx)), sizeof(float)); };
private:
  void          Initialize() {}
  void          Destroy() { CutLinks(); }
};

TA_SMART_PTRS(TA_API, float_Matrix);

#endif // float_Matrix_h
