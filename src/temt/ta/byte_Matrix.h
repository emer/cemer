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
  TypeDef*     GetDataTypeDef() const override {return &TA_unsigned_char;}
  ValType      GetDataValType() const override {return VT_BYTE;}

  bool         StrValIsValid(const String& str, String* err_msg = NULL) const override;
    // accepts 0-255 or octal or hex forms
  bool         BinaryFile_Supported() override { return true; }

  TA_MATRIX_FUNS_FAST(byte_Matrix, byte);

public: //
  float        El_GetFloat_(const void* it) const override { return (float)*((byte*)it); } // #IGNORE
  const String El_GetStr_(const void* it) const override { return String(((int)*((byte*)it))); } // #IGNORE
  void         El_SetFmStr_(void* it, const String& str) override {*((byte*)it) = (byte)str.toInt();}       // #IGNORE
  const Variant El_GetVar_(const void* it) const override {return Variant(*((byte*)it));} // #IGNORE
  void         El_SetFmVar_(void* it, const Variant& var) override {*((byte*)it) = var.toByte(); };  // #IGNORE
  int          El_Compare_(const void* a, const void* b) const override
  { int rval=-1; if(*((byte*)a) > *((byte*)b)) rval=1; else if(*((byte*)a) == *((byte*)b)) rval=0; return rval; }
protected:
  static const byte     blank; // #IGNORE
  void         BinarySave_Item(std::ostream& strm, int idx) override
  { strm.write((char*)&(FastEl_Flat(idx)), sizeof(byte)); };
  void         BinaryLoad_Item(std::istream& strm, int idx) override
  { strm.read((char*)&(FastEl_Flat(idx)), sizeof(byte)); };
private:
  void          Initialize() {}
  void          Destroy()  { CutLinks(); }
};

TA_SMART_PTRS(byte_Matrix);

#endif // byte_Matrix_h
