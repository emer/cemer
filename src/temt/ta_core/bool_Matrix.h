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

#ifndef bool_Matrix_h
#define bool_Matrix_h 1

// parent includes:
#include <taMatrixT>

// member includes:
#include <BuiltinTypeDefs>

// declare all other types mentioned but not required to include:
class TypeDef; //
class MatrixIndex; //

taTypeDef_Of(bool_Matrix);

class TA_API bool_Matrix : public taMatrixT<bool> {
  // #INSTANCE a matrix of bools
  INHERITED(taMatrixT<bool>)
public:
  TypeDef*     GetDataTypeDef() const override {return &TA_bool;}
  ValType      GetDataValType() const override {return VT_BOOL;}
  
  bool         StrValIsValid(const String& str, String* err_msg = NULL) const override;
  // accepts > 0, t, T
  
  virtual void          InitVals(bool val=false)
  { for(int i=0;i<size;i++) FastEl_Flat(i) = val; }
  // initialize values to given fixed value
    
  bool         BinaryFile_Supported() override { return true; }
  
  TA_MATRIX_FUNS_FAST(bool_Matrix, bool);
  
public:
  float         El_GetFloat_(const void* it) const override { return (float)*((bool*)it); } // #IGNORE
  const String  El_GetStr_(const void* it) const override { return (String)*((bool*)it); } // #IGNORE note: implicit conversion avoids problems on some compilers
  void          El_SetFmStr_(void* it, const String& str) override {*((bool*)it) = str.toBool();}  // #IGNORE
  const Variant El_GetVar_(const void* it) const override {return Variant(*((bool*)it));} // #IGNORE
  void          El_SetFmVar_(void* it, const Variant& var) override {*((bool*)it) = var.toBool(); };  // #IGNORE
  int           El_Compare_(const void* a, const void* b) const override
  { int rval=-1; if(*((bool*)a) > *((bool*)b)) rval=1; else if(*((bool*)a) == *((bool*)b)) rval=0; return rval; }
protected:
  static const bool      blank; // #IGNORE
#ifndef __MAKETA__
  void         BinarySave_Item(std::ostream& strm, int idx) override
  { strm.write((char*)&(FastEl_Flat(idx)), sizeof(bool)); };
  void         BinaryLoad_Item(std::istream& strm, int idx) override
  { strm.read((char*)&(FastEl_Flat(idx)), sizeof(bool)); };
#endif
private:
  void          Initialize() {}
  void          Destroy() { CutLinks(); }
};

TA_SMART_PTRS(TA_API, bool_Matrix);

#endif // bool_Matrix_h
