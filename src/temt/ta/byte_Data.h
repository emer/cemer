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

#ifndef byte_Data_h
#define byte_Data_h 1

// parent includes:
#include <byte_Matrix>
#include <DataColT>

// member includes:

// declare all other types mentioned but not required to include:
class TypeDef; // 
taTypeDef_Of(byte);

taTypeDef_Of(byte_Data);

class TA_API byte_Data : public DataColT<byte_Matrix> {
  // byte data
INHERITED(DataColT<byte_Matrix>)
friend class DataTable;
public:
  bool         isNumeric() const override {return true;} //
  int          maxColWidth() const override {return 3;} // assumes 3 digs
  ValType      valType() const override {return VT_BYTE;}
  TypeDef*     valTypeDef() const override {return &TA_byte;}

  TA_BASEFUNS_NOCOPY(byte_Data);

protected:
  double       GetValAsDouble_impl(int row, int cell) const override
  { return (double)ar.SafeEl_Flat(IndexOfEl_Flat(row, cell)); }
  float        GetValAsFloat_impl(int row, int cell) const override
  { return (float)ar.SafeEl_Flat(IndexOfEl_Flat(row, cell)); }
  int          GetValAsInt_impl(int row, int cell) const override
  { return (int)ar.SafeEl_Flat(IndexOfEl_Flat(row, cell)); }
  byte         GetValAsByte_impl(int row, int cell) const override
  { return ar.SafeEl_Flat(IndexOfEl_Flat(row, cell)); }

  bool         SetValAsDouble_impl(double val, int row, int cell) override
  { ar.Set_Flat((byte)val, IndexOfEl_Flat(row, cell)); return true; }
  bool         SetValAsFloat_impl(float val, int row, int cell) override
  { ar.Set_Flat((byte)val, IndexOfEl_Flat(row, cell)); return true; }
  bool         SetValAsInt_impl(int val, int row, int cell) override
  { ar.Set_Flat((byte)val, IndexOfEl_Flat(row, cell)); return true; }
  bool         SetValAsByte_impl(byte val, int row, int cell) override
  { ar.Set_Flat(val, IndexOfEl_Flat(row, cell)); return true; }

private:
  void  Initialize() {}
  void  Destroy() {}
};

TA_SMART_PTRS(TA_API, byte_Data); //

#endif // byte_Data_h
