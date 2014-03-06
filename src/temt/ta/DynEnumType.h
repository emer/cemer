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

#ifndef DynEnumType_h
#define DynEnumType_h 1

// parent includes:
#include <DynEnumBase>

// member includes:

// declare all other types mentioned but not required to include:
class DynEnumItem; //
class DataTable; //

taTypeDef_Of(DynEnumType);

class TA_API DynEnumType : public DynEnumBase {
//   #NO_UPDATE_AFTER  dynamic enumerated type -- user-definable list of labeled values that make code easier to read and write - for enums based on DataTable column values use DynEnumTableCol
INHERITED(DynEnumBase)
public:
  virtual DynEnumItem*  NewEnum();
  // #BUTTON create a new enum item
  virtual void          SeqNumberItems(int first_val = 0);
  // #BUTTON assign values to items sequentially, starting with given first value
  virtual bool          CopyToAllProgs();
  // #BUTTON #CONFIRM copy this type information to all programs that have an enum with this same name in their types section -- provides a convenient way to update when multiple programs use the same dynamic enum types
  virtual bool          EnumsFromDataTable(DataTable* dt, const Variant& col);
  // #BUTTON initialize enum values from values in given data table column (TIP: DynEnumTableCol is a better choice - updates automatically whenever the column values change or rows are added)

  TA_SIMPLE_BASEFUNS(DynEnumType);
protected:

private:
  void  Initialize()    {};
  void  Destroy()       {};
};

SmartRef_Of(DynEnumType); // DynEnumTypeRef

#endif // DynEnumType_h
