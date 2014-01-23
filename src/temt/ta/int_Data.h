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

#ifndef int_Data_h
#define int_Data_h 1

// parent includes:
#include <int_Matrix>
#include <DataColT>

// member includes:

// declare all other types mentioned but not required to include:
class TypeDef; // 


taTypeDef_Of(int_Data);

class TA_API int_Data : public DataColT<int_Matrix> {
  // int data
INHERITED(DataColT<int_Matrix>)
friend class DataTable;
public:
  bool         isNumeric() const CPP11_OVERRIDE {return true;} //
  int          maxColWidth() const CPP11_OVERRIDE {return 11;} // assumes sign, 10 digs
  ValType      valType() const CPP11_OVERRIDE {return VT_INT;}
  TypeDef*     valTypeDef() const CPP11_OVERRIDE {return &TA_int;}

  TA_BASEFUNS_NOCOPY(int_Data);

protected:
  double       GetValAsDouble_impl(int row, int cell) const CPP11_OVERRIDE
  { return (double)ar.SafeEl_Flat(IndexOfEl_Flat(row, cell)); }
  float        GetValAsFloat_impl(int row, int cell) const CPP11_OVERRIDE
  { return (float)ar.SafeEl_Flat(IndexOfEl_Flat(row, cell)); }
  int          GetValAsInt_impl(int row, int cell) const CPP11_OVERRIDE
  { return ar.SafeEl_Flat(IndexOfEl_Flat(row, cell)); }

  bool         SetValAsDouble_impl(double val, int row, int cell) CPP11_OVERRIDE
  { ar.Set_Flat((int)val, IndexOfEl_Flat(row, cell)); return true; }
  bool         SetValAsFloat_impl(float val, int row, int cell) CPP11_OVERRIDE
  { ar.Set_Flat((int)val, IndexOfEl_Flat(row, cell)); return true; }
  bool         SetValAsInt_impl(int val, int row, int cell) CPP11_OVERRIDE
  { ar.Set_Flat(val, IndexOfEl_Flat(row, cell)); return true; }

private:
  void  Initialize() {}
  void  Destroy() {}
};

TA_SMART_PTRS(int_Data); //

#endif // int_Data_h
