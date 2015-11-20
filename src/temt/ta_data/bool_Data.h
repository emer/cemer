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

#ifndef bool_Data_h
#define bool_Data_h 1

// parent includes:
#include <bool_Matrix>
#include <DataColT>

// member includes:

// declare all other types mentioned but not required to include:
class TypeDef; //

taTypeDef_Of(bool_Data);

class TA_API bool_Data : public DataColT<bool_Matrix> {
  // bool data
INHERITED(DataColT<bool_Matrix>)
  friend class DataTable;
public:
  bool         isNumeric() const override {return false;} //
  bool         isBool() const override {return true;} //
  int          maxColWidth() const override {return 5;} // good for 0/1 and true/false
  ValType      valType() const override {return VT_BOOL;}
  TypeDef*     valTypeDef() const override {return &TA_bool;}
  
  TA_BASEFUNS_NOCOPY(bool_Data);
  
protected:
  int          GetValAsInt_impl(int row, int cell) const override
  { return (int)ar.SafeEl_Flat(IndexOfEl_Flat(row, cell)); }
  byte         GetValAsByte_impl(int row, int cell) const override
  { return ar.SafeEl_Flat(IndexOfEl_Flat(row, cell)); }
  
  bool         SetValAsInt_impl(int val, int row, int cell) override
  { ar.Set_Flat((byte)val, IndexOfEl_Flat(row, cell)); return true; }
  bool         SetValAsByte_impl(byte val, int row, int cell) override
  { ar.Set_Flat(val, IndexOfEl_Flat(row, cell)); return true; }
  
private:
  void  Initialize() {}
  void  Destroy() {}
};

TA_SMART_PTRS(TA_API, bool_Data); //
#endif // bool_Data_h
