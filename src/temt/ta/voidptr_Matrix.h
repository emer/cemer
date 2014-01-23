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

#ifndef voidptr_Matrix_h
#define voidptr_Matrix_h 1

// parent includes:
#include <taMatrixT>

// member includes:

// declare all other types mentioned but not required to include:
class TypeDef; // 
class MatrixIndex; // 


taTypeDef_Of(voidptr_Matrix);

class TA_API voidptr_Matrix: public taMatrixT<voidptr> {
  // #INSTANCE a matrix of void*s (generic pointers)
INHERITED(taMatrixT<void*>)
public:
  TypeDef*     GetDataTypeDef() const CPP11_OVERRIDE {return &TA_unsigned_char;}
  ValType      GetDataValType() const CPP11_OVERRIDE {return VT_VOIDPTR;}

  bool         StrValIsValid(const String& str, String* err_msg = NULL) const CPP11_OVERRIDE;
  bool         BinaryFile_Supported() CPP11_OVERRIDE { return true; }

  TA_MATRIX_FUNS_FAST(voidptr_Matrix, void*);

public: //
  float        El_GetFloat_(const void* it) const CPP11_OVERRIDE { return (float)(ta_uintptr_t)*((void**)it); } // #IGNORE
  const String El_GetStr_(const void* it) const CPP11_OVERRIDE { return String(((ta_uintptr_t)*((void**)it))); } // #IGNORE
#if (TA_POINTER_SIZE == 4)
  void         El_SetFmStr_(void* it, const String& str) CPP11_OVERRIDE {*((void**)it) = (void*)str.toInt();}       // #IGNORE
#else
  void         El_SetFmStr_(void* it, const String& str) CPP11_OVERRIDE {*((void**)it) = (void*)str.toInt64();}       // #IGNORE
#endif
  const Variant El_GetVar_(const void* it) const CPP11_OVERRIDE {return Variant(*((void**)it));} // #IGNORE
  void         El_SetFmVar_(void* it, const Variant& var) CPP11_OVERRIDE {*((void**)it) = var.toPtr(); };  // #IGNORE
  int          El_Compare_(const void* a, const void* b) const CPP11_OVERRIDE
  { int rval=-1; if(*((void**)a) > *((void**)b)) rval=1; else if(*((void**)a) == *((void**)b)) rval=0; return rval; }
protected:
  static const void*    blank; // #IGNORE
  void         BinarySave_Item(std::ostream& strm, int idx) CPP11_OVERRIDE
  { strm.write((char*)&(FastEl_Flat(idx)), sizeof(void*)); };
  void         BinaryLoad_Item(std::istream& strm, int idx) CPP11_OVERRIDE
  { strm.read((char*)&(FastEl_Flat(idx)), sizeof(void*)); };
private:
  void          Initialize() {}
  void          Destroy() { CutLinks(); }
};

TA_SMART_PTRS(voidptr_Matrix);

#endif // voidptr_Matrix_h
