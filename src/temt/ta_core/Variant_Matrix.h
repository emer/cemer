// Copyright 2014, Regents of the University of Colorado,
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

#ifndef Variant_Matrix_h
#define Variant_Matrix_h 1

// parent includes:
#include <Variant>
#include <taMatrixT>

// member includes:

// declare all other types mentioned but not required to include:
class TypeDef; // 
class MatrixIndex; // 


taTypeDef_Of(Variant_Matrix);

class TA_API Variant_Matrix: public taMatrixT<Variant> {
  // #INSTANCE a matrix of variants
INHERITED(taMatrixT<Variant>)
public:
  TypeDef*     GetDataTypeDef() const override {return &TA_Variant;}
  ValType      GetDataValType() const override {return VT_VARIANT;}

  TA_MATRIX_FUNS_SLOW(Variant_Matrix, Variant);

public:
  //NOTE: setString may not be exactly what is wanted -- that will change variant to String
  // what we may want is to set the current value as its type, from a string
  float        El_GetFloat_(const void* it) const override { return ((Variant*)it)->toFloat(); } // #IGNORE
  const String El_GetStr_(const void* it) const override { return ((Variant*)it)->toString(); } // #IGNORE
  void         El_SetFmStr_(void* it, const String& str) override {((Variant*)it)->setString(str);}  // #IGNORE
  const Variant El_GetVar_(const void* it) const override {return *((Variant*)it);} // #IGNORE
  void         El_SetFmVar_(void* it, const Variant& var) override {*((Variant*)it) = var; };  // #IGNORE
  int          El_Compare_(const void* a, const void* b) const override
  { int rval=-1; if(*((Variant*)a) > *((Variant*)b)) rval=1; else if(*((Variant*)a) == *((Variant*)b)) rval=0; return rval; }
protected:
  static const Variant  blank; // #IGNORE
  void         Dump_Save_Item(std::ostream& strm, int idx) override;
  int          Dump_Load_Item(std::istream& strm, int idx) override; // ret is last char read, s/b ;
  void         ReclaimOrphans_(int from, int to) override; // called when elements can be reclaimed, ex. for strings

private:
  void          Initialize() {}
  void          Destroy() { CutLinks(); }
};

TA_SMART_PTRS(TA_API, Variant_Matrix);

#endif // Variant_Matrix_h
