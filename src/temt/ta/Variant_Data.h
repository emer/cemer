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


TypeDef_Of(Variant_Data);

class TA_API Variant_Data : public DataColT<Variant_Matrix> {
  // Variant data
INHERITED(DataColT<Variant_Matrix>)
friend class DataTable;
public:
  override ValType      valType() const  {return VT_VARIANT;}
  override TypeDef*     valTypeDef() const  {return &TA_Variant;}

  TA_BASEFUNS_NOCOPY(Variant_Data);

protected:
  override const Variant GetValAsVar_impl(int row, int cell) const
  { return ar.SafeEl_Flat(IndexOfEl_Flat(row, cell)); }
  override bool  SetValAsVar_impl(const Variant& val, int row, int cell)
  { ar.Set_Flat(val, IndexOfEl_Flat(row, cell)); return true; }

  override double GetValAsDouble_impl(int row, int cell) const
  { return GetValAsVar_impl(row, cell).toDouble(); }
  override float GetValAsFloat_impl(int row, int cell) const
  { return (float)GetValAsVar_impl(row, cell).toFloat(); }
  override int   GetValAsInt_impl(int row, int cell) const
  { return (int)GetValAsVar_impl(row, cell).toInt(); }

  override bool  SetValAsDouble_impl(double val, int row, int cell)
  { return SetValAsVar_impl(val, row, cell); }
  override bool  SetValAsFloat_impl(float val, int row, int cell)
  { return SetValAsVar_impl(val, row, cell); }
  override bool  SetValAsInt_impl(int val, int row, int cell)
  { return SetValAsVar_impl(val, row, cell); }

private:
  void  Initialize() {}
  void  Destroy() {}
};

TA_SMART_PTRS(Variant_Data); //

#endif // Variant_Data_h
