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

#ifndef String_Data_h
#define String_Data_h 1

// parent includes:
#include <String_Matrix>
#include <DataColT>

// member includes:

// declare all other types mentioned but not required to include:
class TypeDef; // 


taTypeDef_Of(String_Data);

class TA_API String_Data : public DataColT<String_Matrix> {
  // string data
INHERITED(DataColT<String_Matrix>)
friend class DataTable;
public:
  bool         isString() const override {return true;}
  ValType      valType() const  override {return VT_STRING;}
  TypeDef*     valTypeDef() const  override {return &TA_taString;}

  TA_BASEFUNS_NOCOPY(String_Data);

protected:
  double       GetValAsDouble_impl(int row, int cell) const override
  { return (double)GetValAsString_impl(row, cell); }
  float        GetValAsFloat_impl(int row, int cell) const override
  { return (float)GetValAsString_impl(row, cell); }
  int          GetValAsInt_impl(int row, int cell) const override
  { return (int)GetValAsString_impl(row, cell); }
  byte         GetValAsByte_impl(int row, int cell) const override
  { return GetValAsString_impl(row, cell)[0]; }

  bool  SetValAsDouble_impl(double val, int row, int cell) override
  { return SetValAsString_impl((String)val, row, cell); }
  bool  SetValAsFloat_impl(float val, int row, int cell) override
  { return SetValAsString_impl((String)val, row, cell); }
  bool  SetValAsInt_impl(int val, int row, int cell) override
  { return SetValAsString_impl((String)val, row, cell); }
  bool  SetValAsByte_impl(byte val, int row, int cell) override
  { return SetValAsString_impl((String)val, row, cell); }

private:
  void  Initialize() {}
  void  Destroy() {}
};

TA_SMART_PTRS(String_Data);

#endif // String_Data_h
