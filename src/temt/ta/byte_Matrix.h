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

#ifndef byte_Matrix_h
#define byte_Matrix_h 1

// parent includes:
#include <taMatrixT>

// member includes:

// declare all other types mentioned but not required to include:
class TypeDef; // 
class MatrixIndex; // 


taTypeDef_Of(byte_Matrix);

class TA_API byte_Matrix: public taMatrixT<byte> {
  // #INSTANCE a matrix of bytes
INHERITED(taMatrixT<byte>)
public:
  override TypeDef*     GetDataTypeDef() const {return &TA_unsigned_char;}
  override ValType      GetDataValType() const {return VT_BYTE;}

  override bool         StrValIsValid(const String& str, String* err_msg = NULL) const;
    // accepts 0-255 or octal or hex forms
  override bool         BinaryFile_Supported() { return true; }

  TA_MATRIX_FUNS_FAST(byte_Matrix, byte);

public: //
  override float        El_GetFloat_(const void* it) const { return (float)*((byte*)it); } // #IGNORE
  override const String El_GetStr_(const void* it) const { return String(((int)*((byte*)it))); } // #IGNORE
  override void         El_SetFmStr_(void* it, const String& str) {*((byte*)it) = (byte)str.toInt();}       // #IGNORE
  override const Variant El_GetVar_(const void* it) const {return Variant(*((byte*)it));} // #IGNORE
  override void         El_SetFmVar_(void* it, const Variant& var) {*((byte*)it) = var.toByte(); };  // #IGNORE
protected:
  STATIC_CONST byte     blank; // #IGNORE
#ifndef __MAKETA__
  override void         BinarySave_Item(std::ostream& strm, int idx)
  { strm.write((char*)&(FastEl_Flat(idx)), sizeof(byte)); };
  override void         BinaryLoad_Item(std::istream& strm, int idx)
  { strm.read((char*)&(FastEl_Flat(idx)), sizeof(byte)); };
#endif
private:
  void          Initialize() {}
  void          Destroy()  { CutLinks(); }
};

TA_SMART_PTRS(byte_Matrix);

#endif // byte_Matrix_h
