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
#include <DataColTp>

// member includes:

// declare all other types mentioned but not required to include:
class TypeDef; // 


TypeDef_Of(byte_Data);

class TA_API byte_Data : public DataColTp<byte_Matrix> {
  // byte data
INHERITED(DataColTp<byte_Matrix>)
friend class DataTable;
public:
  override bool         isNumeric() const {return true;} //
  override int          maxColWidth() const {return 3;} // assumes 3 digs
  override ValType      valType() const {return VT_BYTE;}
  override TypeDef*     valTypeDef() const {return &TA_byte;}

  TA_BASEFUNS_NOCOPY(byte_Data);

protected:
  override double       GetValAsDouble_impl(int row, int cell) const
  { return (double)ar.SafeEl_Flat(IndexOfEl_Flat(row, cell)); }
  override float        GetValAsFloat_impl(int row, int cell) const
  { return (float)ar.SafeEl_Flat(IndexOfEl_Flat(row, cell)); }
  override int          GetValAsInt_impl(int row, int cell) const
  { return (int)ar.SafeEl_Flat(IndexOfEl_Flat(row, cell)); }
  override byte         GetValAsByte_impl(int row, int cell) const
  { return ar.SafeEl_Flat(IndexOfEl_Flat(row, cell)); }

  override bool         SetValAsDouble_impl(double val, int row, int cell)
  { ar.Set_Flat((byte)val, IndexOfEl_Flat(row, cell)); return true; }
  override bool         SetValAsFloat_impl(float val, int row, int cell)
  { ar.Set_Flat((byte)val, IndexOfEl_Flat(row, cell)); return true; }
  override bool         SetValAsInt_impl(int val, int row, int cell)
  { ar.Set_Flat((byte)val, IndexOfEl_Flat(row, cell)); return true; }
  override bool         SetValAsByte_impl(byte val, int row, int cell)
  { ar.Set_Flat(val, IndexOfEl_Flat(row, cell)); return true; }

private:
  void  Initialize() {}
  void  Destroy() {}
};

TA_SMART_PTRS(byte_Data); //

#endif // byte_Data_h
