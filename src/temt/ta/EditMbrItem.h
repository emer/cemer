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

#ifndef EditMbrItem_h
#define EditMbrItem_h 1

// parent includes:
#include <SelectEditItem>

// member includes:
#include <EditParamSearch>
#include <MemberDef>

// declare all other types mentioned but not required to include:

TypeDef_Of(EditMbrItem);

class TA_API EditMbrItem: public SelectEditItem {
  // a member select edit item -- allows editing of just one member from a class
  INHERITED(SelectEditItem)
public:
  MemberDef*            mbr; // #READ_ONLY #SHOW the mbr type
  bool                  is_numeric;   // #READ_ONLY is this a single numeric type (float, double, int)
  EditParamSearch       param_search; // #CONDSHOW_ON_is_numeric specifications for parameter searching, only for numeric items

  ///////////////////////////////////
  //        Param Search

  virtual bool          PSearchValidTest();
  // #CAT_ParamSearch test if parameter searching is valid on this item -- emits error if not
  virtual Variant       PSearchCurVal();
  // #CAT_ParamSearch get current value of item, as a variant
  virtual bool          PSearchCurVal_Set(const Variant& cur_val);
  // #CAT_ParamSearch set current value of item from a variant
  virtual bool          PSearchMinToCur();
  // #CAT_ParamSearch set current value of item to value stored in min_val field (called at start)
  virtual bool          PSearchNextIncr();
  // #CAT_ParamSearch set next_val to current value plus incr -- iterate to next level -- returns false if new next_val > max_val, and sets next_val = min_val in this case
  virtual bool          PSearchNextToCur();
  // #CAT_ParamSearch set current value of item to value stored in next_val field -- call this after updating the next values, before running the evaluation with the current parameters

  override TypeItem*    typeItem() const {return mbr;} // the mbr or mth

  override String       GetColText(const KeyString& key, int itm_idx = -1) const;
  TA_BASEFUNS(EditMbrItem);
  void  InitLinks();
protected:
  void                  UpdateAfterEdit_impl();
private:
  void  Initialize();
  void  Destroy();
  void  Copy_(const EditMbrItem& cp);
};

#endif // EditMbrItem_h
