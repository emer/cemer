// Copyright 2013-2018, Regents of the University of Colorado,
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

#ifndef ControlPanelItem_h
#define ControlPanelItem_h 1

// parent includes:
#include <taOBase>

// member includes:

// declare all other types mentioned but not required to include:
class taBase; // 
class taGroup_impl; // 
class TypeItem; // 


taTypeDef_Of(ControlPanelItem);

class TA_API ControlPanelItem: public taOBase {
  // #STEM_BASE ##CAT_Display ##DIFF_STRING base class for membs/meths in a ControlPanel
  INHERITED(taOBase)
public:
  // some convenience functions
    
  static bool           StatCheckBase(ControlPanelItem* itm, const taBase* base);
  // #IGNORE check if either itm->base == base or itm->mbr->GetOff(itm->base) == base
  static ControlPanelItem* StatFindItemBase
    (const taGroup_impl* grp, const taBase* base, TypeItem* ti, int& idx);
  // #IGNORE find the item with indicated base and mth/mbr in the group
  static ControlPanelItem* StatFindItemBase_List
    (const taGroup_impl* grp, const taBase* base, TypeItem* ti, int& idx);
  // #IGNORE find the item with indicated base and mth/mbr in the main list elements ONLY (not in any subgroups)
  static bool           StatGetBase_Flat(const taGroup_impl* grp, int idx, taBase*& base);
  // #IGNORE gets the flat (leaf) base
  static bool           StatHasBase(taGroup_impl* grp, const taBase* base);
  // #IGNORE see if anyone has this base
  static bool           StatRemoveItemBase(taGroup_impl* grp, taBase* base);
  // #IGNORE remove any items with this base

  String                label;          // full display label for item in edit dialog
  bool                  short_label;    // the label only includes the direct name of the item, and does not include a longer prefix of names of the object(s) that own this item -- if the label by itself is sufficiently unique, the short form can be used -- otherwise it is a good idea to use the long form to keep the labels unique
  bool                  cust_label;     // the label is customized over the default and thus protected from automatic updates -- this flag is automatically set by editing, but can also be set manually if desired
  String                desc;           // #EDIT_DIALOG description (appears as tooltip for item)
  bool                  cust_desc;      // the description (desc) is customized over the default and thus protected from automatic updates -- otherwise desc is obtained from the member or method type information -- this flag is automatically set by editing, but can also be set manually if desired
  taBase*               base;           // #READ_ONLY #SHOW #NO_SET_POINTER #UPDATE_POINTER the mbr/mth base (not ref'ed)
  String       prv_desc;                // #IGNORE previous description -- for checking for changes
  String       prv_label;               // #IGNORE previous label -- for checking changes

  String                caption() const; // the string used in the editor
  virtual TypeItem*     typeItem() const {return NULL;} // the mbr or mth

  virtual void  SetLabel(const String& new_label, bool custom_lbl = false, bool short_label = false);
  // use this method to set a new label, cust label flag, and also set the prv_label
  virtual void  SetDesc(const String& new_desc, bool custom_desc = false);
  // use this method to set a new desc, cust desc flag, and also set the prv_desc

  virtual bool  IsSameItem(const ControlPanelItem* itm) const
  { return (base == itm->base) && (typeItem() == itm->typeItem()); }
  // is this pointing to the same item as the other guy?
  
  String       GetName() const override { return label; }
  bool         SetName(const String& nm) override;
  bool         HasName() const override { return true; }
  String       GetColText(const KeyString& key, int itm_idx = -1) const override;
  String       GetDesc() const override;
  TA_BASEFUNS(ControlPanelItem);
protected:
  void         UpdateAfterEdit_impl() override;

private:

  void  Initialize();
  void  Destroy();
  void  Copy_(const ControlPanelItem& cp);
};

#endif // ControlPanelItem_h
