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

#ifndef double_Matrix_h
#define double_Matrix_h 1

// parent includes:
#include <taMatrixT>

// member includes:

// declare all other types mentioned but not required to include:
class TypeDef; // 
class MatrixIndex; // 


taTypeDef_Of(double_Matrix);

class TA_API double_Matrix: public taMatrixT<double> {
  // #INSTANCE a matrix of doubles
INHERITED(taMatrixT<double>)
public:
  TypeDef*     GetDataTypeDef() const override {return &TA_double;}
  ValType      GetDataValType() const override {return VT_DOUBLE;}

  bool         StrValIsValid(const String& str, String* err_msg = NULL) const override;
    // accepts valid format for double

  virtual void          InitVals(double val=0.0) { for(int i=0;i<size;i++) FastEl_Flat(i) = val; }
  // initialize values to given fixed value

  virtual void		InitFromDoubles(double* vals, int n=-1)
  { int eff_n = size; if(n > 0) { eff_n = n; if(size < n) SetGeom(1,n); }
    for(int i=0;i<eff_n;i++) FastEl_Flat(i) = vals[i]; }
  // initialize values from an array of doubles, with optional number parameter n (if unspecified or -1, then array is assumed to be size of matrix)

  bool         BinaryFile_Supported() override { return true; }

  TA_MATRIX_FUNS_FAST(double_Matrix, double);

public:
  float        El_GetFloat_(const void* it) const override
    { return (float)*((double*)it); }
    // #IGNORE warning: loss of precision and/or under/overflow possible
  double       El_GetDouble_(const void* it) const override
    { return *((double*)it); } // #IGNORE
  const String El_GetStr_(const void* it) const override
    { return (String)*((double*)it); } // #IGNORE
  void         El_SetFmStr_(void* it, const String& str) override
    {*((double*)it) = (double)str;}  // #IGNORE
  const Variant El_GetVar_(const void* it) const override
    {return Variant(*((double*)it));} // #IGNORE
  void         El_SetFmVar_(void* it, const Variant& var) override
    {*((double*)it) = var.toDouble(); };  // #IGNORE
  int          El_Compare_(const void* a, const void* b) const override
  { int rval=-1; if(*((double*)a) > *((double*)b)) rval=1; else if(*((double*)a) == *((double*)b)) rval=0; return rval; }
protected:
  static const double   blank; // #IGNORE
  void         Dump_Save_Item(std::ostream& strm, int idx) override; // stream in full precision
  void         BinarySave_Item(std::ostream& strm, int idx) override
  { strm.write((char*)&(FastEl_Flat(idx)), sizeof(double)); };
  void         BinaryLoad_Item(std::istream& strm, int idx) override
  { strm.read((char*)&(FastEl_Flat(idx)), sizeof(double)); };
private:
  void          Initialize() {}
  void          Destroy() { CutLinks(); }
};

TA_SMART_PTRS(double_Matrix);

#endif // double_Matrix_h
