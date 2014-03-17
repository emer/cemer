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

#ifndef DynEnumBase_h
#define DynEnumBase_h 1

// parent includes:
#include <ProgType>

// member includes:
#include <DynEnumItem_List>

// declare all other types mentioned but not required to include:
class DynEnumItem; //

taTypeDef_Of(DynEnumBase);

class TA_API DynEnumBase : public ProgType {
  // #VIRT_BASE ##DEF_CHILD_enums ##CAT_Program ##SCOPE_Program base class for a user-definable list of labeled values that make code easier to read and write
INHERITED(ProgType)
public:
  DynEnumItem_List      enums;  // enumerated values for this type
  bool                  bits;   // each item represents a separate bit value, which can be orthogonally set from each other, instead of mutually exclusive alternatives

  virtual DynEnumItem*  AddEnum(const String& nm, int val);
  // add a new enum item with given name/label and value

  virtual int   FindNumIdx(int val) const { return enums.FindNumIdx(val); }
  // find index of given numerical value
  virtual int   FindNameIdx(const String& nm) const { return enums.FindNameIdx(nm); }
  // find index of given name value

  virtual std::ostream& OutputType(std::ostream& fh) const;
  // output type information in C++ syntax

  taList_impl* children_() override {return &enums;}
  Variant      Elem(const Variant& idx, IndexMode mode = IDX_UNK) const override
  { return enums.Elem(idx, mode); }

  void SigEmit(int sls, void* op1 = NULL, void* op2 = NULL) override;

  taBase*      FindTypeName(const String& nm) const override;
  String       GetDisplayName() const override;

  bool         BrowserEditSet(const String& code, int move_after = 0) override;

  bool         BrowserSelectMe() override;
  bool         BrowserExpandAll() override;
  bool         BrowserCollapseAll() override;

  TA_SIMPLE_BASEFUNS(DynEnumBase);
protected:
  void         CheckChildConfig_impl(bool quiet, bool& rval) override;
  void         GenCssPre_impl(Program* prog) override;
  void         GenCssBody_impl(Program* prog) override;
  void         GenCssPost_impl(Program* prog) override;

private:
  void  Initialize();
  void  Destroy()       {}
};

SMARTREF_OF(TA_API, DynEnumBase); // DynEnumBaseRef

#endif // DynEnumBase_h
