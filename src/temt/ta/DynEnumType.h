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
#include <ProgType>

// member includes:
#include <DynEnumItem_List>

// declare all other types mentioned but not required to include:
class DynEnumItem; // 
class DataTable; // 


taTypeDef_Of(DynEnumType);

class TA_API DynEnumType : public ProgType {
  // #NO_UPDATE_AFTER ##DEF_CHILD_enums ##CAT_Program ##SCOPE_Program dynamic enumerated type -- user-definable list of labeled values that make code easier to read and write
INHERITED(ProgType)
public:
  DynEnumItem_List      enums;  // enumerated values for this type
  bool                  bits;   // each item represents a separate bit value, which can be orthogonally set from each other, instead of mutually exclusive alternatives

  virtual DynEnumItem*  NewEnum();
  // #BUTTON create a new enum item
  virtual DynEnumItem*  AddEnum(const String& nm, int val);
  // add a new enum item with given name/label and value
  virtual void          SeqNumberItems(int first_val = 0);
  // #BUTTON assign values to items sequentially, starting with given first value
  virtual bool          CopyToAllProgs();
  // #BUTTON #CONFIRM copy this type information to all programs that have an enum with this same name in their types section -- provides a convenient way to update when multiple programs use the same dynamic enum types

  virtual int   FindNumIdx(int val) const { return enums.FindNumIdx(val); }
  // find index of given numerical value
  virtual int   FindNameIdx(const String& nm) const { return enums.FindNameIdx(nm); }
  // find index of given name value

  virtual std::ostream& OutputType(std::ostream& fh) const;
  // output type information in C++ syntax

  virtual bool  EnumsFromDataTable(DataTable* dt, const Variant& col);
  // #BUTTON initialize enum values from values in given data table column (can be number or name) -- WARNING: replaces any existing enums.  handy for providing chooser access to column values

  override taList_impl* children_() {return &enums;}
  override Variant      Elem(const Variant& idx, IndexMode mode = IDX_UNK) const
  { return enums.Elem(idx, mode); }

  override void SigEmit(int sls, void* op1 = NULL, void* op2 = NULL);

  override taBase*      FindTypeName(const String& nm) const;
  override String       GetDisplayName() const;

  override bool         BrowserSelectMe();
  override bool         BrowserExpandAll();
  override bool         BrowserCollapseAll();

  TA_SIMPLE_BASEFUNS(DynEnumType);
protected:
  override void         CheckChildConfig_impl(bool quiet, bool& rval);
  override void         GenCssPre_impl(Program* prog);
  override void         GenCssBody_impl(Program* prog);
  override void         GenCssPost_impl(Program* prog);

private:
  void  Initialize();
  void  Destroy()       {}
};

SmartRef_Of(DynEnumType); // DynEnumTypeRef

#endif // DynEnumType_h
