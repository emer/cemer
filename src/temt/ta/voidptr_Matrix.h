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
  override TypeDef*     GetDataTypeDef() const {return &TA_unsigned_char;}
  override ValType      GetDataValType() const {return VT_VOIDPTR;}

  override bool         StrValIsValid(const String& str, String* err_msg = NULL) const;
  override bool         BinaryFile_Supported() { return true; }

  TA_MATRIX_FUNS_FAST(voidptr_Matrix, void*);

public: //
  override float        El_GetFloat_(const void* it) const { return (float)(ta_uintptr_t)*((void**)it); } // #IGNORE
  override const String El_GetStr_(const void* it) const { return String(((ta_uintptr_t)*((void**)it))); } // #IGNORE
#if (TA_POINTER_SIZE == 4)
  override void         El_SetFmStr_(void* it, const String& str) {*((void**)it) = (void*)str.toInt();}       // #IGNORE
#else
  override void         El_SetFmStr_(void* it, const String& str) {*((void**)it) = (void*)str.toInt64();}       // #IGNORE
#endif
  override const Variant El_GetVar_(const void* it) const {return Variant(*((void**)it));} // #IGNORE
  override void         El_SetFmVar_(void* it, const Variant& var) {*((void**)it) = var.toPtr(); };  // #IGNORE
protected:
  static const void*    blank; // #IGNORE
  override void         BinarySave_Item(std::ostream& strm, int idx)
  { strm.write((char*)&(FastEl_Flat(idx)), sizeof(void*)); };
  override void         BinaryLoad_Item(std::istream& strm, int idx)
  { strm.read((char*)&(FastEl_Flat(idx)), sizeof(void*)); };
private:
  void          Initialize() {}
  void          Destroy() { CutLinks(); }
};

TA_SMART_PTRS(voidptr_Matrix);

#endif // voidptr_Matrix_h
