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

#ifndef SelectEditItem_h
#define SelectEditItem_h 1

// parent includes:
#include <taOBase>

// member includes:

// declare all other types mentioned but not required to include:
class taBase; // 
class taGroup_impl; // 
class TypeItem; // 


taTypeDef_Of(SelectEditItem);

class TA_API SelectEditItem: public taOBase {
  // #STEM_BASE ##CAT_Display base class for membs/meths in a SelectEdit
  INHERITED(taOBase)
public:
// some convenience functions
  static SelectEditItem*        StatFindItemBase(const taGroup_impl* grp,
    taBase* base, TypeItem* ti, int& idx);
    // find the item with indicated base and mth/mbr in the group
  static bool           StatGetBase_Flat(const taGroup_impl* grp, int idx, taBase*& base);
    // gets the flat (leaf) base
  static bool           StatHasBase(taGroup_impl* grp, taBase* base);
    // see if anyone has this base
  static bool           StatRemoveItemBase(taGroup_impl* grp, taBase* base);
    // remove any items with this base

  String                label;          // full display label for item in edit dialog
  String                desc;           // #EDIT_DIALOG description (appears as tooltip for item)
  bool                  cust_desc;      // the description (desc) is customized over the default and thus protected from automatic updates -- otherwise desc is obtained from the member or method type information -- this flag is automatically set by editing, but can also be set manually if desired
  taBase*               base;           // #READ_ONLY #SHOW #NO_SET_POINTER #UPDATE_POINTER the mbr/mth base (not ref'ed)
  String                item_nm;        // #READ_ONLY #NO_SHOW #NO_SAVE #OBSOLETE name of the member or method

  String                caption() const; // the string used in the editor
  virtual TypeItem*     typeItem() const {return NULL;} // the mbr or mth

  String       GetName() const override;
  String       GetColText(const KeyString& key, int itm_idx = -1) const override;
  String       GetDesc() const override;
  TA_BASEFUNS(SelectEditItem);
protected:
  void                  UpdateAfterEdit_impl();
  String                prv_desc; // previous description -- for checking for changes

private:

  void  Initialize();
  void  Destroy();
  void  Copy_(const SelectEditItem& cp);
};

#endif // SelectEditItem_h
