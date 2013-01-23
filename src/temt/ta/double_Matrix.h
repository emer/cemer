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


TypeDef_Of(double_Matrix);

class TA_API double_Matrix: public taMatrixT<double> {
  // #INSTANCE a matrix of doubles
INHERITED(taMatrixT<double>)
public:
  override TypeDef*     GetDataTypeDef() const {return &TA_double;}
  override ValType      GetDataValType() const {return VT_DOUBLE;}

  override bool         StrValIsValid(const String& str, String* err_msg = NULL) const;
    // accepts valid format for double

  virtual void          InitVals(double val=0.0) { for(int i=0;i<size;i++) FastEl_Flat(i) = val; }
  // initialize values to given fixed value

  virtual void		InitFromDoubles(double* vals, int n=-1)
  { int eff_n = size; if(n > 0) { eff_n = n; if(size < n) SetGeom(1,n); }
    for(int i=0;i<size;i++) FastEl_Flat(i) = vals[i]; }
  // initialize values from an array of doubles, with optional number parameter n (if unspecified or -1, then array is assumed to be size of matrix)

  override bool         BinaryFile_Supported() { return true; }

  TA_MATRIX_FUNS_FAST(double_Matrix, double);

public:
  override float        El_GetFloat_(const void* it) const
    { return (float)*((double*)it); }
    // #IGNORE warning: loss of precision and/or under/overflow possible
  override double       El_GetDouble_(const void* it) const
    { return *((double*)it); } // #IGNORE
  override const String El_GetStr_(const void* it) const
    { return (String)*((double*)it); } // #IGNORE
  override void         El_SetFmStr_(void* it, const String& str)
    {*((double*)it) = (double)str;}  // #IGNORE
  override const Variant El_GetVar_(const void* it) const
    {return Variant(*((double*)it));} // #IGNORE
  override void         El_SetFmVar_(void* it, const Variant& var)
    {*((double*)it) = var.toDouble(); };  // #IGNORE
protected:
  STATIC_CONST double   blank; // #IGNORE
#ifndef __MAKETA__
  override void         Dump_Save_Item(std::ostream& strm, int idx); // stream in full precision
  override void         BinarySave_Item(std::ostream& strm, int idx)
  { strm.write((char*)&(FastEl_Flat(idx)), sizeof(double)); };
  override void         BinaryLoad_Item(std::istream& strm, int idx)
  { strm.read((char*)&(FastEl_Flat(idx)), sizeof(double)); };
#endif
private:
  void          Initialize() {}
  void          Destroy() { CutLinks(); }
};

TA_SMART_PTRS(double_Matrix);

#endif // double_Matrix_h
