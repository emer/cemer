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

#ifndef Variant_Data_h
#define Variant_Data_h 1

// parent includes:
#include <Variant_Matrix>
#include <DataColT>

// member includes:

// declare all other types mentioned but not required to include:
class TypeDef; // 


taTypeDef_Of(Variant_Data);

class TA_API Variant_Data : public DataColT<Variant_Matrix> {
  // Variant data
INHERITED(DataColT<Variant_Matrix>)
friend class DataTable;
public:
  ValType      valType() const  CPP11_OVERRIDE {return VT_VARIANT;}
  TypeDef*     valTypeDef() const  CPP11_OVERRIDE {return &TA_Variant;}

  TA_BASEFUNS_NOCOPY(Variant_Data);

protected:
  const Variant GetValAsVar_impl(int row, int cell) const CPP11_OVERRIDE
  { return ar.SafeEl_Flat(IndexOfEl_Flat(row, cell)); }
  bool  SetValAsVar_impl(const Variant& val, int row, int cell) CPP11_OVERRIDE
  { ar.Set_Flat(val, IndexOfEl_Flat(row, cell)); return true; }

  double GetValAsDouble_impl(int row, int cell) const CPP11_OVERRIDE
  { return GetValAsVar_impl(row, cell).toDouble(); }
  float GetValAsFloat_impl(int row, int cell) const CPP11_OVERRIDE
  { return (float)GetValAsVar_impl(row, cell).toFloat(); }
  int   GetValAsInt_impl(int row, int cell) const CPP11_OVERRIDE
  { return (int)GetValAsVar_impl(row, cell).toInt(); }

  bool  SetValAsDouble_impl(double val, int row, int cell) CPP11_OVERRIDE
  { return SetValAsVar_impl(val, row, cell); }
  bool  SetValAsFloat_impl(float val, int row, int cell) CPP11_OVERRIDE
  { return SetValAsVar_impl(val, row, cell); }
  bool  SetValAsInt_impl(int val, int row, int cell) CPP11_OVERRIDE
  { return SetValAsVar_impl(val, row, cell); }

private:
  void  Initialize() {}
  void  Destroy() {}
};

TA_SMART_PTRS(Variant_Data); //

#endif // Variant_Data_h
