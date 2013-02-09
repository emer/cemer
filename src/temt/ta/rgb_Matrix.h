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
  override TypeDef*     GetDataTypeDef() const {return &TA_rgb_t;}
  override ValType      GetDataValType() const {return VT_INT;} // note: not quite right.

  override bool         StrValIsValid(const String& str, String* err_msg = NULL) const;
    // accepts in form: "r g b" or RRGGBB in hex
  override bool         BinaryFile_Supported() { return true; }

  TA_MATRIX_FUNS_FAST(rgb_Matrix, rgb_t);

public: //
  //note: for streaming, we use web RGB hex value
  override const String El_GetStr_(const void* it) const { return *((rgb_t*)it); } // #IGNORE implicit, to hex web format
  override void         El_SetFmStr_(void* it, const String& str) {((rgb_t*)it)->setString(str);}       // #IGNORE
  override const Variant El_GetVar_(const void* it) const {return Variant(((rgb_t*)it)->toInt());} // #IGNORE we use the int rep for variants
  override void         El_SetFmVar_(void* it, const Variant& var) {((rgb_t*)it)->setInt(var.toInt()); };  // #IGNORE
protected:
  STATIC_CONST rgb_t    blank; // #IGNORE
#ifndef __MAKETA__
  override void         BinarySave_Item(std::ostream& strm, int idx)
  { strm.write((char*)&(FastEl_Flat(idx)), sizeof(rgb_t)); };
  override void         BinaryLoad_Item(std::istream& strm, int idx)
  { strm.read((char*)&(FastEl_Flat(idx)), sizeof(rgb_t)); };
#endif
private:
  void          Initialize() {}
  void          Destroy() { CutLinks(); }
};

TA_SMART_PTRS(rgb_Matrix);

#endif // rgb_Matrix_h
