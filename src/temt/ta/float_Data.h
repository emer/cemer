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

#ifndef float_Data_h
#define float_Data_h 1

// parent includes:
#include <float_Matrix>
#include <DataColTp>

// member includes:

// declare all other types mentioned but not required to include:
class TypeDef; // 


TypeDef_Of(float_Data);

class TA_API float_Data : public DataColTp<float_Matrix> {
  // floating point data
INHERITED(DataColTp<float_Matrix>)
friend class DataTable;
public:
  override bool         isNumeric() const {return true;}
  override bool         isFloat() const {return true;}
  override int          maxColWidth() const {return 7;} // assumes sign, int: 6 dig's; float: 5 dig's, decimal point
  override ValType      valType() const {return VT_FLOAT;}
  override TypeDef*     valTypeDef() const {return &TA_float;}

  TA_BASEFUNS_NOCOPY(float_Data);

protected:
  override double       GetValAsDouble_impl(int row, int cell) const
  { return (double)ar.SafeEl_Flat(IndexOfEl_Flat(row, cell)); }
  override float        GetValAsFloat_impl(int row, int cell) const
  { return ar.SafeEl_Flat(IndexOfEl_Flat(row, cell)); }
  override int          GetValAsInt_impl(int row, int cell) const
  { return (int)ar.SafeEl_Flat(IndexOfEl_Flat(row, cell)); }

  override bool         SetValAsDouble_impl(double val, int row, int cell)
  { ar.Set_Flat((float)val, IndexOfEl_Flat(row, cell)); return true; }
  override bool         SetValAsFloat_impl(float val, int row, int cell)
  { ar.Set_Flat(val, IndexOfEl_Flat(row, cell)); return true; }
  override bool         SetValAsInt_impl(int val, int row, int cell)
  { ar.Set_Flat((float)val, IndexOfEl_Flat(row, cell)); return true; }

private:
  void  Initialize() {}
  void  Destroy() {}
};

TA_SMART_PTRS(float_Data); //

#endif // float_Data_h
