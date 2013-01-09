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

#ifndef Variant_Matrix_h
#define Variant_Matrix_h 1

// parent includes:
#include <Variant>
#include <taMatrixT>

// member includes:

// declare all other types mentioned but not required to include:
class TypeDef; // 
class MatrixIndex; // 


class TA_API Variant_Matrix: public taMatrixT<Variant> {
  // #INSTANCE a matrix of variants
INHERITED(taMatrixT<Variant>)
public:
  override TypeDef*     GetDataTypeDef() const {return &TA_Variant;}
  override ValType      GetDataValType() const {return VT_VARIANT;}

  TA_MATRIX_FUNS_SLOW(Variant_Matrix, Variant);

public:
  //NOTE: setString may not be exactly what is wanted -- that will change variant to String
  // what we may want is to set the current value as its type, from a string
  override float        El_GetFloat_(const void* it) const { return ((Variant*)it)->toFloat(); } // #IGNORE
  override const String El_GetStr_(const void* it) const { return ((Variant*)it)->toString(); } // #IGNORE
  override void         El_SetFmStr_(void* it, const String& str) {((Variant*)it)->setString(str);}  // #IGNORE
  override const Variant El_GetVar_(const void* it) const {return *((Variant*)it);} // #IGNORE
  override void         El_SetFmVar_(void* it, const Variant& var) {*((Variant*)it) = var; };  // #IGNORE
protected:
  STATIC_CONST Variant  blank; // #IGNORE
  override void         Dump_Save_Item(ostream& strm, int idx);
  override int          Dump_Load_Item(istream& strm, int idx); // ret is last char read, s/b ;
  override void         ReclaimOrphans_(int from, int to); // called when elements can be reclaimed, ex. for strings

private:
  void          Initialize() {}
  void          Destroy() { CutLinks(); }
};

TA_SMART_PTRS(Variant_Matrix);

#endif // Variant_Matrix_h
