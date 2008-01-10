// Copyright, 1995-2007, Regents of the University of Colorado,
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


#ifndef TA_SELEDIT_H
#define TA_SELEDIT_H

#include "ta_def.h"
#include "ta_base.h"
#include "ta_group.h"

class SelectEditItem;
class EditMbrItem;
class EditMthItem;

class TA_API SelectEditItem: public taOBase {
  //  ##CAT_Display base class for membs/meths in a ControlEdit
  INHERITED(taOBase)
public:
// some convenience functions
  static SelectEditItem*	StatFindItemBase(taGroup_impl* grp, taBase* base,
    TypeItem* ti, int& idx);
    // find the item iwth indicated base and mth/mbr in the group
  static bool		StatHasBase(taGroup_impl* grp, taBase* base);
    // see if anyone has this base
  static bool		StatRemoveItemBase(taGroup_impl* grp, taBase* base);
    // remove any items with this base
  
  String		desc; // #EDIT_DIALOG description (appears as tooltip for item)
  String		label;	 // full display label for item in edit dialog
  taBase*		base; // #READ_ONLY #SHOW #NO_SET_POINTER the mbr/mth base (not ref'ed)
  String		item_nm; // #READ_ONLY #SHOW name of the member or method
  
  String 		caption() const; // the string used in the editor
  virtual TypeItem*	typeItem() const {return NULL;} // the mbr or mth
  
  override String	GetName() const;
  override String 	GetColText(const KeyString& key, int itm_idx = -1) const;
  override String	GetDesc() const;
  TA_BASEFUNS(SelectEditItem);
private:
  void	Initialize();
  void	Destroy();
  void	Copy_(const SelectEditItem& cp);
};


class TA_API EditMbrItem: public SelectEditItem {
  //  ##CAT_Display base class for membs/meths in a Edit
  INHERITED(SelectEditItem)
public:
  MemberDef*		mbr; // #READ_ONLY #SHOW #NO_SAVE the mbr type
  
  override TypeItem*	typeItem() const {return mbr;} // the mbr or mth
  
  override String 	GetColText(const KeyString& key, int itm_idx = -1) const;
  TA_BASEFUNS(EditMbrItem);
private:
  void	Initialize();
  void	Destroy();
  void	Copy_(const EditMbrItem& cp);
};

class TA_API EditMthItem: public SelectEditItem {
  //  ##CAT_Display base class for meths in a Edit
  INHERITED(SelectEditItem)
public:
  MethodDef*		mth; // #READ_ONLY #SHOW #NO_SAVE the mbr type
  
  override TypeItem*	typeItem() const {return mth;} // the mbr or mth
  
//  override String 	GetColText(const KeyString& key, int itm_idx = -1) const;
  TA_BASEFUNS(EditMthItem);
private:
  void	Initialize();
  void	Destroy();
  void	Copy_(const EditMthItem& cp);
};


class TA_API EditMbrItem_Group : public taGroup<EditMbrItem> {
  // ##CAT_Display group of select edit dialog objects
INHERITED(taGroup<EditMbrItem>)
public:

  void			GetMembsFmStrs(); // #IGNORE get members from strings (upon loading)
  
  override int		NumListCols() const {return 5;}
  // base name, base type, memb name, memb type, memb label
  override String	GetColHeading(const KeyString& key) const;
  // header text for the indicated column
  override const KeyString GetListColKey(int col) const;
  override String 	GetTypeDecoKey() const { return "SelectEdit"; }
  override void		DataChanged(int dcr, void* op1 = NULL, void* op2 = NULL);
  TA_BASEFUNS_NOCOPY(EditMbrItem_Group);
private:
  void	Initialize() { SetBaseType(&TA_EditMbrItem);}
  void 	Destroy()		{ };
};


class TA_API EditMthItem_Group : public taGroup<EditMthItem> {
  // ##CAT_Display group of select edit dialog objects
INHERITED(taGroup<EditMthItem>)
public:
  enum MthGroupType {
    GT_BUTTONS,		// make a buttons in the dialog for aach method
    GT_MENU_BUTTON,	// make a button group for the methods (subgroups only)
    GT_MENU		// make a menu for the methods (subgroups only)
  };
  
  bool			is_root; // #NO_SHOW #READ_ONLY #NO_SAVE
  MthGroupType		group_type; // #GHOST_ON_is_root how to organize and display the methods in this group
  
  void			GetMethsFmStrs(); // #IGNORE get methods from strings (upon loading)
  
  void			SetGroupType(MthGroupType group_type);
   // #MENU #MENU_CONTEXT set how the methods will be displayed in the SelectEdit dialog
  
  override int		NumListCols() const {return 4;}
  // base name, base type, meth name, memb label
  override String	GetColHeading(const KeyString& key) const;
  // header text for the indicated column
  override const KeyString GetListColKey(int col) const;
  override String 	GetTypeDecoKey() const { return "SelectEdit"; }
  override void		DataChanged(int dcr, void* op1 = NULL, void* op2 = NULL);
  void	InitLinks();
  TA_BASEFUNS(EditMthItem_Group);
private:
  void	Initialize();
  void 	Destroy() { };
  void	Copy_(const EditMthItem_Group& cp);
};


class TA_API SelectEditConfig : public taBase {
  // #EDIT_INLINE ##NO_TOKENS ##CAT_Display #OBSOLETE special parameters for controlling the select edit display
  INHERITED(taBase)
public:
  bool		auto_edit;	// automatically bring up edit dialog upon loading
  String_Array	mbr_labels;	// extra labels at the start of each member label for the selected fields
  String_Array	meth_labels;	// extra labels at the start of each method label for the selected functions

  SIMPLE_LINKS(SelectEditConfig);
  TA_BASEFUNS_NOCOPY(SelectEditConfig);
private:
  void	Initialize() {auto_edit = false;}
  void	Destroy() {CutLinks();}
};


class TA_API SelectEdit: public taNBase, public IRefListClient {
  // ##EXT_edit ##CAT_Display Selectively edit members from different objects
  INHERITED(taNBase)
public:
  static void		StatDataChanged_Group(taGroup_impl* grp, int dcr, void* op1, void* op2);
  
  bool			auto_edit; // automatically bring up edit dialog upon loading
  String		desc;	// #EDIT_DIALOG description of what this edit contains
  EditMbrItem_Group	mbrs; // #TREE_EXPERT the members of the edit
  EditMthItem_Group	mths; // #TREE_EXPERT the mthods of the edit
  
  EditMbrItem*		mbr(int i) const; // convenience accessor for flat access
  int			mbrSize(int i) const {return mbrs.leaves;} // flat size
  EditMthItem*		mth(int i) const; // convenience accessor for flat access
  int			mthSize(int i) const {return mths.leaves;} // flat size
  
  override bool		autoEdit() const {return auto_edit;}
  
  override String 	GetTypeDecoKey() const { return "SelectEdit"; }
  override String	GetDesc() const { return desc; } //
  override int		UpdatePointers_NewPar(taBase* old_par, taBase* new_par);
  SIMPLE_LINKS(SelectEdit);
  TA_BASEFUNS(SelectEdit);
    
public: // public API
  virtual void	RemoveField(int idx);
  //  remove edit data item at given index and update dialog
  virtual void	RemoveFun(int idx);
  //  remove function at given index
  
  virtual int	SearchMembers(taNBase* obj, const String& memb_contains);
  // #MENU #MENU_ON_SelectEdit search given object for member names that contain given string, and add them to this editor
  virtual int	CompareObjs(taBase* obj_a, taBase* obj_b, bool no_ptrs = true);
  // #MENU #TYPE_taNBase compare two objects (must be of same type) and add the differences in this select edit; no_ptrs = ignore pointer fields
  virtual bool	SelectMember(taBase* base, MemberDef* md, const char* lbl);
  // add new member to edit if it isn't already here (returns true), otherwise update lbl (returns false)
  virtual bool	SelectMemberNm(taBase* base, const char* md, const char* lbl);
  // add new member to edit if it isn't already here (returns true), otherwise update lbl (returns false)
  virtual bool	SelectMethod(taBase* base, MethodDef* md, const char* lbl);
  // add new method to edit if it isn't already here (returns true), otherwise remove (returns false)
  virtual bool	SelectMethodNm(taBase* base, const char* md, const char* lbl);
  // add new method to edit if it isn't already here (returns true), otherwise remove (returns false)
  
  virtual int	FindMbrBase(taBase* base, MemberDef* md);
  // find a given base and member, returns index

  virtual int	FindMethBase(taBase* base, MethodDef* md);
  // find a given base and method, returns index
 
  virtual void	Reset();
  // #MENU #CONFIRM #MENU_SEP_BEFORE reset (remove all) current members and methods
 
public: // IRefListClient i/f
  override void*	This() {return this;}
  override void		DataDestroying_Ref(taBase_RefList* src, taBase* ta);
    // note: item will already have been removed from list
  override void		DataChanged_Ref(taBase_RefList* src, taBase* ta,
    int dcr, void* op1, void* op2);

protected:
  int			m_changing; // flag so we don't recursively delete bases
  taBase_RefList	base_refs; // all bases notify us via this list
  
  override void		UpdateAfterEdit_impl();
  virtual void		DataChanged_Group(taGroup_impl* grp, int dcr, void* op1, void* op2);
    // mostly for detecting asynchronous deletes
  virtual void		BaseAdded(taBase* ta);
  virtual void		BaseRemoved(taBase* ta);
  virtual void		RemoveField_impl(int idx);
  virtual void		RemoveFun_impl(int idx);
  virtual bool		SelectMember_impl(taBase* base, MemberDef* md,
    const char* lbl);
  virtual bool		SelectMethod_impl(taBase* base, MethodDef* md,
    const char* lbl);
  
private:
  void	Initialize();
  void	Destroy();
  void	Copy_(const SelectEdit& cp); //
  
public: // legacy routines/members
  SelectEditConfig config;	// #NO_SHOW #NO_SAVE special parameters for controlling the display

  taBase_List	mbr_bases;	// #NO_SHOW #NO_SAVE #LINK_GROUP #READ_ONLY #AKA_bases the bases for each element in the list
  String_Array	mbr_strs;	// #NO_SHOW #NO_SAVE #READ_ONLY #AKA_member_strs string names of mbrs on bases -- used for saving

  taBase_List	meth_bases;	// #NO_SHOW #NO_SAVE #LINK_GROUP #READ_ONLY the bases for each element in the list
  String_Array	meth_strs;	// #NO_SHOW #NO_SAVE #READ_ONLY string names of meths on bases -- used for saving
  void		ConvertLegacy();
//  void	UpdateAllBases();	// perform update-after-edit on all base objects
//  virtual void	NewEdit();
  // closes current edit dialog and makes a new one (with any changes)
//  void		GetAllPaths();	// #IGNORE get paths for all current objects
//  void		BaseChangeSave(); // #IGNORE close edit dialog and save paths to current bases
//  void		BaseChangeReShow(); // #IGNORE re-show the edit dialog loading bases from saved paths
//  bool		BaseDataChanged(taBase* obj,int dcr, void* op1_, void* op2_);
};





#endif
