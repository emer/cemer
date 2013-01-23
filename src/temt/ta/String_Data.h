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
#include <DataColTp>

// member includes:

// declare all other types mentioned but not required to include:
class TypeDef; // 


TypeDef_Of(String_Data);

class TA_API String_Data : public DataColTp<String_Matrix> {
  // string data
INHERITED(DataColTp<String_Matrix>)
friend class DataTable;
public:
  override bool         isString() const {return true;}
  override ValType      valType() const  {return VT_STRING;}
  override TypeDef*     valTypeDef() const  {return &TA_taString;}

  TA_BASEFUNS_NOCOPY(String_Data);

protected:
  override double       GetValAsDouble_impl(int row, int cell) const
  { return (double)GetValAsString_impl(row, cell); }
  override float        GetValAsFloat_impl(int row, int cell) const
  { return (float)GetValAsString_impl(row, cell); }
  override int          GetValAsInt_impl(int row, int cell) const
  { return (int)GetValAsString_impl(row, cell); }
  override byte         GetValAsByte_impl(int row, int cell) const
  { return GetValAsString_impl(row, cell)[0]; }

  override bool  SetValAsDouble_impl(double val, int row, int cell)
  { return SetValAsString_impl((String)val, row, cell); }
  override bool  SetValAsFloat_impl(float val, int row, int cell)
  { return SetValAsString_impl((String)val, row, cell); }
  override bool  SetValAsInt_impl(int val, int row, int cell)
  { return SetValAsString_impl((String)val, row, cell); }
  override bool  SetValAsByte_impl(byte val, int row, int cell)
  { return SetValAsString_impl((String)val, row, cell); }

private:
  void  Initialize() {}
  void  Destroy() {}
};

TA_SMART_PTRS(String_Data);

#endif // String_Data_h
