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

#ifndef DynEnumTableCol_h
#define DynEnumTableCol_h 1

// parent includes:
#include <DynEnumBase>

// member includes:
#include <taSmartRef>
#include <DataTableRef>

// declare all other types mentioned but not required to include:

taTypeDef_Of(DynEnumTableCol);

class TA_API DynEnumTableCol : public DynEnumBase {
  // #AKA_DynEnumTypeFromDTColumn Enum that is generated from a datatable column and is updated based on notification of updates from the table
INHERITED(DynEnumBase)
public:
  DataTableRef        src_table; // #AKA_srcTable table to get enum data from
  String              src_column; // #AKA_srcColumn column within src_table to get data from

  void  InitLinks();
  void  CutLinks();

  virtual bool    EnumsFromDataTable(DataTable* dt, const Variant& col);
  // #BUTTON initialize enum values from values in given data table column (can be number or name). Updates to column will automatically be reflected in the enum

  void   SmartRef_SigDestroying(taSmartRef* ref, taBase* obj) override;
  void   SmartRef_SigEmit(taSmartRef* ref, taBase* obj, int sls, void* op1_, void* op2_) override;

   TA_BASEFUNS(DynEnumTableCol);

protected:
   void      UpdateAfterEdit_impl() override;

private:
   void Copy_(const DynEnumTableCol& cp);
   void Initialize();
   void Destroy() {CutLinks();}
};

SmartRef_Of(DynEnumTableCol); // DynEnumTypeRef

#endif // DynEnumTableCol_h
