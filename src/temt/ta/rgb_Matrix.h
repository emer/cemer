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

#ifndef rgb_Matrix_h
#define rgb_Matrix_h 1

// parent includes:
#include <taMatrixT>
#include <iColor>

// member includes:

// declare all other types mentioned but not required to include:
class TypeDef; // 
class MatrixIndex; // 


taTypeDef_Of(rgb_Matrix);

class TA_API rgb_Matrix: public taMatrixT<rgb_t> {
  // #INSTANCE a matrix of rgb values
INHERITED(taMatrixT<rgb_t>)
public:
  TypeDef*     GetDataTypeDef() const CPP11_OVERRIDE {return &TA_rgb_t;}
  ValType      GetDataValType() const CPP11_OVERRIDE {return VT_INT;} // note: not quite right.

  bool         StrValIsValid(const String& str, String* err_msg = NULL) const CPP11_OVERRIDE;
    // accepts in form: "r g b" or RRGGBB in hex
  bool         BinaryFile_Supported() CPP11_OVERRIDE { return true; }

  TA_MATRIX_FUNS_FAST(rgb_Matrix, rgb_t);

public: //
  //note: for streaming, we use web RGB hex value
  const String El_GetStr_(const void* it) const CPP11_OVERRIDE { return *((rgb_t*)it); } // #IGNORE implicit, to hex web format
  void         El_SetFmStr_(void* it, const String& str) CPP11_OVERRIDE {((rgb_t*)it)->setString(str);}       // #IGNORE
  const Variant El_GetVar_(const void* it) const CPP11_OVERRIDE {return Variant(((rgb_t*)it)->toInt());} // #IGNORE we use the int rep for variants
  void         El_SetFmVar_(void* it, const Variant& var) CPP11_OVERRIDE {((rgb_t*)it)->setInt(var.toInt()); };  // #IGNORE
  int          El_Compare_(const void* a, const void* b) const CPP11_OVERRIDE
  { int rval=-1; if(*((rgb_t*)a) > *((rgb_t*)b)) rval=1; else if(*((rgb_t*)a) == *((rgb_t*)b)) rval=0; return rval; }
protected:
  static const rgb_t    blank; // #IGNORE
#ifndef __MAKETA__
  void         BinarySave_Item(std::ostream& strm, int idx) CPP11_OVERRIDE
  { strm.write((char*)&(FastEl_Flat(idx)), sizeof(rgb_t)); };
  void         BinaryLoad_Item(std::istream& strm, int idx) CPP11_OVERRIDE
  { strm.read((char*)&(FastEl_Flat(idx)), sizeof(rgb_t)); };
#endif
private:
  void          Initialize() {}
  void          Destroy() { CutLinks(); }
};

TA_SMART_PTRS(rgb_Matrix);

#endif // rgb_Matrix_h
