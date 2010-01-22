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
class DataTable;

class TA_API SelectEditItem: public taOBase {
  // #STEM_BASE ##CAT_Display base class for membs/meths in a SelectEdit
  INHERITED(taOBase)
public:
// some convenience functions
  static SelectEditItem*	StatFindItemBase(const taGroup_impl* grp,
    taBase* base, TypeItem* ti, int& idx);
    // find the item with indicated base and mth/mbr in the group
  static bool		StatGetBase_Flat(const taGroup_impl* grp, int idx, taBase*& base);
    // gets the flat (leaf) base
  static bool		StatHasBase(taGroup_impl* grp, taBase* base);
    // see if anyone has this base
  static bool		StatRemoveItemBase(taGroup_impl* grp, taBase* base);
    // remove any items with this base
  
  String		label;	 	// full display label for item in edit dialog
  String		desc; 		// #EDIT_DIALOG description (appears as tooltip for item)
  bool			cust_desc;	// the description (desc) is customized over the default and thus protected from automatic updates -- otherwise desc is obtained from the member or method type information -- this flag is automatically set by editing, but can also be set manually if desired
  taBase*		base; 		// #READ_ONLY #SHOW #NO_SET_POINTER #UPDATE_POINTER the mbr/mth base (not ref'ed)
  String		item_nm; 	// #READ_ONLY #NO_SHOW #NO_SAVE #OBSOLETE name of the member or method
  
  String 		caption() const; // the string used in the editor
  virtual TypeItem*	typeItem() const {return NULL;} // the mbr or mth
  
  override String	GetName() const;
  override String 	GetColText(const KeyString& key, int itm_idx = -1) const;
  override String	GetDesc() const;
  TA_BASEFUNS(SelectEditItem);
protected:
  void			UpdateAfterEdit_impl();
  String 		prv_desc; // previous description -- for checking for changes

private:

  void	Initialize();
  void	Destroy();
  void	Copy_(const SelectEditItem& cp);
};

class TA_API EditParamSearch: public taOBase {
  // #INLINE parameter searching values
  INHERITED(taOBase)
public:
  bool			search; 	// include this item in parameter search
  double		min_val;	// #CONDSHOW_ON_search minimum value to consider for parameter searching purposes
  double		max_val;	// #CONDSHOW_ON_search maximum value to consider for parameter searching purposes
  double		next_val;	// #CONDSHOW_ON_search computed next value to assign to this item in the parameter search
  double		incr;		// #CONDSHOW_ON_search suggested increment to use in searching this parameter (e.g., for grid search)

  TA_SIMPLE_BASEFUNS(EditParamSearch);
  void	Initialize();
  void	Destroy();
};

class TA_API EditMbrItem: public SelectEditItem {
  // a member select edit item -- allows editing of just one member from a class
  INHERITED(SelectEditItem)
public:
  MemberDef*		mbr; // #READ_ONLY #SHOW the mbr type
  bool			is_numeric;   // #READ_ONLY is this a single numeric type (float, double, int)
  EditParamSearch	param_search; // #CONDSHOW_ON_is_numeric specifications for parameter searching, only for numeric items

  ///////////////////////////////////
  //	    Param Search

  virtual bool		PSearchValidTest();
  // #CAT_ParamSearch test if parameter searching is valid on this item -- emits error if not
  virtual Variant	PSearchCurVal();
  // #CAT_ParamSearch get current value of item, as a variant
  virtual bool		PSearchCurVal_Set(const Variant& cur_val);
  // #CAT_ParamSearch set current value of item from a variant
  virtual bool		PSearchMinToCur();
  // #CAT_ParamSearch set current value of item to value stored in min_val field (called at start)
  virtual bool		PSearchNextIncr();
  // #CAT_ParamSearch set next_val to current value plus incr -- iterate to next level -- returns false if new next_val > max_val, and sets next_val = min_val in this case
  virtual bool		PSearchNextToCur();
  // #CAT_ParamSearch set current value of item to value stored in next_val field -- call this after updating the next values, before running the evaluation with the current parameters
  
  override TypeItem*	typeItem() const {return mbr;} // the mbr or mth

  override String 	GetColText(const KeyString& key, int itm_idx = -1) const;
  TA_BASEFUNS(EditMbrItem);
  void	InitLinks();
protected:
  void			UpdateAfterEdit_impl();
private:
  void	Initialize();
  void	Destroy();
  void	Copy_(const EditMbrItem& cp);
};

class TA_API EditMthItem: public SelectEditItem {
  // a method select edit item -- allows access via menu or button to just one method from a class
  INHERITED(SelectEditItem)
public:
  MethodDef*		mth; // #READ_ONLY #SHOW the mbr type
  
  override TypeItem*	typeItem() const {return mth;} // the mbr or mth
  
//  override String 	GetColText(const KeyString& key, int itm_idx = -1) const;
  TA_BASEFUNS(EditMthItem);
protected:
  void			UpdateAfterEdit_impl();
private:
  void	Initialize();
  void	Destroy();
  void	Copy_(const EditMthItem& cp);
};


class TA_API EditMbrItem_Group : public taGroup<EditMbrItem> {
  // ##CAT_Display group of select edit dialog objects
INHERITED(taGroup<EditMbrItem>)
public:
  taBase*		GetBase_Flat(int idx) const;
    // gets the flat (leaf) base -- NULL if out of bounds or doesn't exist
  EditMbrItem*		FindItemBase(taBase* base,
    MemberDef* md, int& idx) const
    {return (EditMbrItem*)SelectEditItem::StatFindItemBase(this, base, md, idx);}
  
  override int		NumListCols() const {return 5;}
  // base name, base type, memb name, memb type, memb label
  override String	GetColHeading(const KeyString& key) const;
  // header text for the indicated column
  override const KeyString GetListColKey(int col) const;
  override String 	GetTypeDecoKey() const { return "SelectEdit"; }
  override void		DataChanged(int dcr, void* op1 = NULL, void* op2 = NULL);
  TA_BASEFUNS_NOCOPY(EditMbrItem_Group);
#ifdef __MAKETA__ // dummy to supress New
  taBase* 	New_gui(int n_objs=1, TypeDef* typ = NULL); // #NO_SHOW
#endif

  //////////////////////////////////////////////////
  // 	Parameter Searching Interface

  virtual EditMbrItem*	FindMbrName(const String& mbr_nm, const String& label = "");
  // #CAT_Access find an item based on member name and, optionally if non-empty, the associated label
  virtual EditMbrItem*	PSearchFind(const String& mbr_nm, const String& label = "");
  // #CAT_ParamSearch find a param_search item based on member name and, optionally if non-empty, the associated label -- must be is_numeric -- issues error if not found -- used for other psearch functions
  virtual EditMbrItem*	PSearchNext(int& st_idx);
  // #CAT_ParamSearch get the next active param search item starting from the given start index -- null if this was the last one -- must have param_search.search = true -- increments st_idx to next item to search so it can be called continuously

  virtual bool		PSearchMinToCur_All();
  // #CAT_ParamSearch set current value to stored minimum value for all items in active parameter search -- call at start of searching
  virtual bool		PSearchNextIncr_Grid();
  // #CAT_ParamSearch increment the next_val for next param search item using a simple grid search algorithm -- first item is searched as an inner-loop, followed by next item, etc -- returns false when last item has been incremented to its max value (time to stop)
  virtual bool		PSearchNextToCur_All();
  // #CAT_ParamSearch set current value to stored next value for all items in active parameter search 

  virtual void		PSearchConfigTable(DataTable* dat);
  // #CAT_ParamSearch configure data table to hold results of the parameter search -- see PSearchRecord function -- makes a column for each active search variable, plus an "eval" column to hold the overal evaluation value for this set of parameters
  virtual void		PSearchRecord(DataTable* dat, double eval_val);
  // #CAT_ParamSearch record current search parameters in data table (configured with PSearchConfigTable), along with the evaluation value for this set of parameters as provided

  virtual bool&		PSearchOn(const String& mbr_nm, const String& label = "");
  // #CAT_PSearch_Access gets a reference to the param_search flag for given member name and, optionally if non-empty, the associated label -- indicates whether to include item in overall search process
  virtual bool		PSearchOn_Set(bool psearch, const String& mbr_nm, const String& label = "");
  // #CAT_PSearch_Access set the param_search flag for given member name and, optionally if non-empty, the associated label -- indicates whether to include item in overall search process
  virtual double&	PSearchMinVal(const String& mbr_nm, const String& label = "");
  // #CAT_PSearch_Access gets a reference to the param search min_val for given member name and, optionally if non-empty, the associated label
  virtual bool		PSearchMinVal_Set(double min_val, const String& mbr_nm, const String& label = "");
  // #CAT_PSearch_Access set param search min_val for given member name and, optionally if non-empty, the associated label
  virtual double&	PSearchMaxVal(const String& mbr_nm, const String& label = "");
  // #CAT_PSearch_Access gets a reference to the param search max_val for given member name and, optionally if non-empty, the associated label
  virtual bool		PSearchMaxVal_Set(double max_val, const String& mbr_nm, const String& label = "");
  // #CAT_PSearch_Access set param search max_val for given member name and, optionally if non-empty, the associated label
  virtual double&	PSearchNextVal(const String& mbr_nm, const String& label = "");
  // #CAT_PSearch_Access gets a reference to the param search next_val for given member name and, optionally if non-empty, the associated label
  virtual bool		PSearchNextVal_Set(double next_val, const String& mbr_nm, const String& label = "");
  // #CAT_PSearch_Access set param search next_val for given member name and, optionally if non-empty, the associated label
  virtual double&	PSearchIncrVal(const String& mbr_nm, const String& label = "");
  // #CAT_PSearch_Access gets a reference to the param search incr_val for given member name and, optionally if non-empty, the associated label
  virtual bool		PSearchIncrVal_Set(double incr_val, const String& mbr_nm, const String& label = "");
  // #CAT_PSearch_Access set param search incr_val for given member name and, optionally if non-empty, the associated label
  virtual Variant	PSearchCurVal(const String& mbr_nm, const String& label = "");
  // #CAT_PSearch_Access gets current value for given member name and, optionally if non-empty, the associated label
  virtual bool		PSearchCurVal_Set(const Variant& cur_val, const String& mbr_nm, const String& label = "");
  // #CAT_PSearch_Access set current value for given member name and, optionally if non-empty, the associated label
  virtual bool		PSearchNextToCur(const String& mbr_nm, const String& label = "");
  // #CAT_PSearch_Access set current value to stored next value for given member name and, optionally if non-empty, the associated label

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
  
  void			SetGroupType(MthGroupType group_type);
   // #MENU #MENU_CONTEXT set how the methods will be displayed in the SelectEdit dialog
  
  taBase*		GetBase_Flat(int idx) const;
    // gets the flat (leaf) base -- NULL if out of bounds or doesn't exist
  
  override int		NumListCols() const {return 4;}
  // base name, base type, meth name, memb label
  override String	GetColHeading(const KeyString& key) const;
  // header text for the indicated column
  override const KeyString GetListColKey(int col) const;
  override String 	GetTypeDecoKey() const { return "SelectEdit"; }
  override void		DataChanged(int dcr, void* op1 = NULL, void* op2 = NULL);
  void	InitLinks();
  TA_BASEFUNS(EditMthItem_Group);
#ifdef __MAKETA__ // dummy to supress New
  taBase* 	New_gui(int n_objs=1, TypeDef* typ = NULL); // #NO_SHOW
#endif
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


class TA_API SelectEdit: public taNBase, public virtual IRefListClient {
  // #STEM_BASE ##EXT_edit ##CAT_Display Selectively edit members from different objects
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
  virtual bool	SelectMember(taBase* base, MemberDef* md,
		     const String& xtra_lbl = _nilString, const String& desc = _nilString,
		     const String& sub_gp_nm = _nilString);
  // add new member to edit if it isn't already here (returns true), optionally in a sub group
  virtual bool	SelectMemberNm(taBase* base, const String& md_nm,
			       const String& xtra_lbl, const String& desc = _nilString,
			       const String& sub_gp_nm = _nilString);
  // add new member to edit if it isn't already here (returns true), optionally in a sub group
  virtual bool	SelectMethod(taBase* base, MethodDef* md,
			     const String& xtra_lbl, const String& desc = _nilString,
			     const String& sub_gp_nm = _nilString);
  // add new method to edit if it isn't already here (returns true), optionally in a sub group
  virtual bool	SelectMethodNm(taBase* base, const String& md,
			       const String& xtra_lbl, const String& desc = _nilString,
			       const String& sub_gp_nm = _nilString);
  // add new method to edit if it isn't already here (returns true), optionally in a sub group
  
  virtual int	FindMbrBase(taBase* base, MemberDef* md);
  // find a given base and member, returns index

  virtual EditMbrItem*	FindMbrName(const String& mbr_nm, const String& label = "") 
  { return mbrs.FindMbrName(mbr_nm, label); }
  // #CAT_Access find an item based on member name and, optionally if non-empty, the associated label

  virtual int	FindMethBase(taBase* base, MethodDef* md);
  // find a given base and method, returns index

  virtual void	Reset();
  // #MENU #CONFIRM reset (remove all) current members and methods


  //////////////////////////////////////////////////
  // 	Parameter Searching Interface

  virtual EditMbrItem*	PSearchFind(const String& mbr_nm, const String& label = "")
  { return mbrs.PSearchFind(mbr_nm, label); }
  // #CAT_ParamSearch find a param_search item based on member name and, optionally if non-empty, the associated label -- must be is_numeric -- issues error if not found -- used for other psearch functions

  virtual EditMbrItem*	PSearchNext(int& st_idx)
  { return mbrs.PSearchNext(st_idx); }
  // #CAT_ParamSearch get the next active param search item starting from the given start index -- null if this was the last one -- must have param_search.search = true -- increments st_idx to next item to search so it can be called continuously

  virtual bool		PSearchMinToCur_All()
  { return mbrs.PSearchMinToCur_All(); }
  // #CAT_ParamSearch set current value to stored minimum value for all items in active parameter search -- call at start of searching
  virtual bool		PSearchNextIncr_Grid()
  { return mbrs.PSearchNextIncr_Grid(); }
  // #CAT_ParamSearch increment the next_val for next param search item using a simple grid search algorithm -- first item is searched as an inner-loop, followed by next item, etc -- returns false when last item has been incremented to its max value (time to stop)
  virtual bool		PSearchNextToCur_All()
  { return mbrs.PSearchNextToCur_All(); }
  // #CAT_ParamSearch set current value to stored next value for all items in active parameter search 

  virtual void		PSearchConfigTable(DataTable* dat)
  { return mbrs.PSearchConfigTable(dat); }
  // #CAT_ParamSearch configure data table to hold results of the parameter search -- see PSearchRecord function -- makes a column for each active search variable, plus an "eval" column to hold the overal evaluation value for this set of parameters
  virtual void		PSearchRecord(DataTable* dat, double eval_val)
  { return mbrs.PSearchRecord(dat, eval_val); }
  // #CAT_ParamSearch record current search parameters in data table (configured with PSearchConfigTable), along with the evaluation value for this set of parameters as provided

  virtual bool&		PSearchOn(const String& mbr_nm, const String& label = "")
  { return mbrs.PSearchOn(mbr_nm, label); }
  // #CAT_PSearch_Access gets a reference to the param_search flag for given member name and, optionally if non-empty, the associated label -- indicates whether to include item in overall search process
  virtual bool		PSearchOn_Set(bool psearch, const String& mbr_nm, const String& label = "")
  { return mbrs.PSearchOn_Set(psearch, mbr_nm, label); }
  // #CAT_PSearch_Access set the param_search flag for given member name and, optionally if non-empty, the associated label -- indicates whether to include item in overall search process
  virtual double&	PSearchMinVal(const String& mbr_nm, const String& label = "")
  { return mbrs.PSearchMinVal(mbr_nm, label); }
  // #CAT_PSearch_Access gets a reference to the param search min_val for given member name and, optionally if non-empty, the associated label
  virtual bool		PSearchMinVal_Set(double min_val, const String& mbr_nm, const String& label = "")
  { return mbrs.PSearchMinVal_Set(min_val, mbr_nm, label); }
  // #CAT_PSearch_Access set param search min_val for given member name and, optionally if non-empty, the associated label
  virtual double&	PSearchMaxVal(const String& mbr_nm, const String& label = "")
  { return mbrs.PSearchMaxVal(mbr_nm, label); }
  // #CAT_PSearch_Access gets a reference to the param search max_val for given member name and, optionally if non-empty, the associated label
  virtual bool		PSearchMaxVal_Set(double max_val, const String& mbr_nm, const String& label = "")
  { return mbrs.PSearchMaxVal_Set(max_val, mbr_nm, label); }
  // #CAT_PSearch_Access set param search max_val for given member name and, optionally if non-empty, the associated label
  virtual double&	PSearchNextVal(const String& mbr_nm, const String& label = "")
  { return mbrs.PSearchNextVal(mbr_nm, label); }
  // #CAT_PSearch_Access gets a reference to the param search next_val for given member name and, optionally if non-empty, the associated label
  virtual bool		PSearchNextVal_Set(double next_val, const String& mbr_nm, const String& label = "")
  { return mbrs.PSearchNextVal_Set(next_val, mbr_nm, label); }
  // #CAT_PSearch_Access set param search next_val for given member name and, optionally if non-empty, the associated label
  virtual double&	PSearchIncrVal(const String& mbr_nm, const String& label = "")
  { return mbrs.PSearchIncrVal(mbr_nm, label); }
  // #CAT_PSearch_Access gets a reference to the param search incr_val for given member name and, optionally if non-empty, the associated label
  virtual bool		PSearchIncrVal_Set(double incr_val, const String& mbr_nm, const String& label = "")
  { return mbrs.PSearchIncrVal_Set(incr_val, mbr_nm, label); }
  // #CAT_PSearch_Access set param search incr_val for given member name and, optionally if non-empty, the associated label
  virtual Variant	PSearchCurVal(const String& mbr_nm, const String& label = "")
  { return mbrs.PSearchCurVal(mbr_nm, label); }
  // #CAT_PSearch_Access gets current value for given member name and, optionally if non-empty, the associated label
  virtual bool		PSearchCurVal_Set(const Variant& cur_val, const String& mbr_nm, const String& label = "")
  { return mbrs.PSearchCurVal_Set(cur_val, mbr_nm, label); }
  // #CAT_PSearch_Access set current value for given member name and, optionally if non-empty, the associated label
  virtual bool		PSearchNextToCur(const String& mbr_nm, const String& label = "")
  { return mbrs.PSearchNextToCur(mbr_nm, label); }
  // #CAT_PSearch_Access set current value to stored next value for given member name and, optionally if non-empty, the associated label
 
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
		  const String& lbl, const String& desc, const String& sub_gp_nm = _nilString);
  virtual bool		SelectMethod_impl(taBase* base, MethodDef* md,
					  const String& lbl, const String& desc,
					  const String& sub_gp_nm = _nilString);
  bool			ReShowEdit(bool force = false); // this is just really a synonym for doing a DataChanged
  
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
};

#endif
