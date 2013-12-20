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

#ifndef DynEnumTypeFromDTColumn_h
#define DynEnumTypeFromDTColumn_h 1

// parent includes:
#include <DynEnumType>

// member includes:
#include <taSmartRef>

// declare all other types mentioned but not required to include:
class DataTable;
class DataCol;

taTypeDef_Of(DynEnumTypeFromDTColumn);

class TA_API DynEnumTypeFromDTColumn : public DynEnumType {
  // Enum that is generated from a datatable column and is updated based on notification of updates from the table
INHERITED(DynEnumType)
public:
      void  InitLinks();
      void  CutLinks();

      virtual bool    EnumsFromDataTable(DataTable* dt, const Variant& col);
      // #BUTTON initialize enum values from values in given data table column (can be number or name). Updates to column will automatically be reflected in the enum
      virtual DynEnumItem*  NewEnum() {return NULL;};
      // no manual creation of enums for this type - updates automatically with changes to data table

      override void   SmartRef_SigDestroying(taSmartRef* ref, taBase* obj);
      override void   SmartRef_SigEmit(taSmartRef* ref, taBase* obj, int sls, void* op1_, void* op2_);

   TA_BASEFUNS(DynEnumTypeFromDTColumn);

protected:
    taSmartRef        srcTable;
    String            srcColumn;

private:
    void Initialize();
    void Destroy() {CutLinks();}
};

#endif // DynEnumTypeFromDTColumn_h
