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
#include <DynEnumBase>

// member includes:
#include <taSmartRef>
#include <DataTable>

// declare all other types mentioned but not required to include:

taTypeDef_Of(DynEnumTypeFromDTColumn);

class TA_API DynEnumTypeFromDTColumn : public DynEnumBase {
  // Enum that is generated from a datatable column and is updated based on notification of updates from the table
INHERITED(DynEnumBase)
public:
      DataTableRef        srcTable;
      String              srcColumn;

      void  InitLinks();
      void  CutLinks();

      virtual bool    EnumsFromDataTable(DataTable* dt, const Variant& col);
      // #BUTTON initialize enum values from values in given data table column (can be number or name). Updates to column will automatically be reflected in the enum

      override void   SmartRef_SigDestroying(taSmartRef* ref, taBase* obj);
      override void   SmartRef_SigEmit(taSmartRef* ref, taBase* obj, int sls, void* op1_, void* op2_);

   TA_BASEFUNS(DynEnumTypeFromDTColumn);

protected:
   override void      UpdateAfterEdit_impl();

private:
   void Copy_(const DynEnumTypeFromDTColumn& cp);
   void Initialize();
   void Destroy() {CutLinks();}
};

SmartRef_Of(DynEnumTypeFromDTColumn); // DynEnumTypeRef

#endif // DynEnumTypeFromDTColumn_h
