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

#ifndef SelectEdit_h
#define SelectEdit_h 1

// parent includes:
#include <taNBase>
#include <IRefListClient>

// smartptr, ref includes
#include <taSmartRefT>
#include <taSmartPtrT>

// member includes:
#include <EditMbrItem_Group>
#include <EditMthItem_Group>
#include <SelectEditConfig>
#include <taBase_List>
#include <String_Array>
#include <taBase_RefList>

// declare all other types mentioned but not required to include:
class taGroup_impl; // 
class EditMbrItem; // 
class EditMthItem; // 
class DataTable; // 


TypeDef_Of(SelectEdit);

class TA_API SelectEdit: public taNBase, public virtual IRefListClient {
  // #STEM_BASE ##EXT_edit ##CAT_Display Selectively edit members from different objects
  INHERITED(taNBase)
public:
  static void           StatSigEmit_Group(taGroup_impl* grp, int sls, void* op1, void* op2);

  bool                  auto_edit; // automatically bring up edit dialog upon loading
  String                desc;   // #EDIT_DIALOG description of what this edit contains
  EditMbrItem_Group     mbrs; // #TREE_EXPERT the members of the edit
  EditMthItem_Group     mths; // #TREE_EXPERT the mthods of the edit

  EditMbrItem*          mbr(int i) const; // convenience accessor for flat access
  int                   mbrSize(int i) const {return mbrs.leaves;} // flat size
  EditMthItem*          mth(int i) const; // convenience accessor for flat access
  int                   mthSize(int i) const {return mths.leaves;} // flat size

  override bool         autoEdit() const {return auto_edit;}

  override String       GetTypeDecoKey() const { return "SelectEdit"; }
  override String       GetDesc() const { return desc; } //
  override int          UpdatePointers_NewPar(taBase* old_par, taBase* new_par);
  SIMPLE_LINKS(SelectEdit);
  TA_BASEFUNS(SelectEdit);

public: // public API
  virtual void  RemoveField(int idx);
  //  remove edit data item at given index and update dialog
  virtual void  RemoveFun(int idx);
  //  remove function at given index

  virtual int   SearchMembers(taNBase* obj, const String& memb_contains);
  // #MENU #MENU_ON_SelectEdit search given object for member names that contain given string, and add them to this editor
  virtual int   CompareObjs(taBase* obj_a, taBase* obj_b, bool no_ptrs = true);
  // #MENU #TYPE_taNBase compare two objects (must be of same type) and add the differences in this select edit; no_ptrs = ignore pointer fields
  virtual bool  SelectMember(taBase* base, MemberDef* md,
                     const String& xtra_lbl = _nilString, const String& desc = _nilString,
                     const String& sub_gp_nm = _nilString);
  // add new member to edit if it isn't already here (returns true), optionally in a sub group
  virtual bool  SelectMemberNm(taBase* base, const String& md_nm,
                               const String& xtra_lbl, const String& desc = _nilString,
                               const String& sub_gp_nm = _nilString);
  // add new member to edit if it isn't already here (returns true), optionally in a sub group
  virtual bool  SelectMethod(taBase* base, MethodDef* md,
                             const String& xtra_lbl, const String& desc = _nilString,
                             const String& sub_gp_nm = _nilString);
  // add new method to edit if it isn't already here (returns true), optionally in a sub group
  virtual bool  SelectMethodNm(taBase* base, const String& md,
                               const String& xtra_lbl, const String& desc = _nilString,
                               const String& sub_gp_nm = _nilString);
  // add new method to edit if it isn't already here (returns true), optionally in a sub group

  virtual int   FindMbrBase(taBase* base, MemberDef* md);
  // find a given base and member, returns index

  virtual EditMbrItem*  FindMbrName(const String& mbr_nm, const String& label = "")
  { return mbrs.FindMbrName(mbr_nm, label); }
  // #CAT_Access find an item based on member name and, optionally if non-empty, the associated label

  virtual int   FindMethBase(taBase* base, MethodDef* md);
  // find a given base and method, returns index

  virtual void  Reset();
  // #MENU #CONFIRM reset (remove all) current members and methods

  //////////////////////////////////////////////////
  //    Parameter Searching Interface

  virtual EditMbrItem*  PSearchFind(const String& mbr_nm, const String& label = "")
  { return mbrs.PSearchFind(mbr_nm, label); }
  // #CAT_ParamSearch find a param_search item based on member name and, optionally if non-empty, the associated label -- must be is_numeric -- issues error if not found -- used for other psearch functions

  virtual EditMbrItem*  PSearchNext(int& st_idx)
  { return mbrs.PSearchNext(st_idx); }
  // #CAT_ParamSearch get the next active param search item starting from the given start index -- null if this was the last one -- must have param_search.search = true -- increments st_idx to next item to search so it can be called continuously

  virtual bool          PSearchMinToCur_All()
  { return mbrs.PSearchMinToCur_All(); }
  // #CAT_ParamSearch set current value to stored minimum value for all items in active parameter search -- call at start of searching
  virtual bool          PSearchNextIncr_Grid()
  { return mbrs.PSearchNextIncr_Grid(); }
  // #CAT_ParamSearch increment the next_val for next param search item using a simple grid search algorithm -- first item is searched as an inner-loop, followed by next item, etc -- returns false when last item has been incremented to its max value (time to stop)
  virtual bool          PSearchNextToCur_All()
  { return mbrs.PSearchNextToCur_All(); }
  // #CAT_ParamSearch set current value to stored next value for all items in active parameter search

  virtual void          PSearchConfigTable(DataTable* dat, bool all_nums=false,
                                           bool add_eval=true)
  { return mbrs.PSearchConfigTable(dat, all_nums, add_eval); }
  // #CAT_ParamSearch configure data table to hold results of the parameter search -- see PSearchRecord function -- makes a column for each active search variable (or all numeric values if specified), plus an optional "eval" column to hold the overal evaluation value for this set of parameters
  virtual void          PSearchRecordData(DataTable* dat, bool all_nums=false,
                                          bool add_eval=true, double eval_val=0.0)
  { return mbrs.PSearchRecordData(dat, all_nums, add_eval, eval_val); }
  // #CAT_ParamSearch record current search parameters (or all numeric values if specified) in data table (configured with PSearchConfigTable), along with the evaluation value for this set of parameters as provided -- does not add a new row to table -- can be used in conjunction with other net monitor data etc

  virtual bool&         PSearchOn(const String& mbr_nm, const String& label = "")
  { return mbrs.PSearchOn(mbr_nm, label); }
  // #CAT_PSearch_Access gets a reference to the param_search flag for given member name and, optionally if non-empty, the associated label -- indicates whether to include item in overall search process
  virtual bool          PSearchOn_Set(bool psearch, const String& mbr_nm, const String& label = "")
  { return mbrs.PSearchOn_Set(psearch, mbr_nm, label); }
  // #CAT_PSearch_Access set the param_search flag for given member name and, optionally if non-empty, the associated label -- indicates whether to include item in overall search process
  virtual double&       PSearchMinVal(const String& mbr_nm, const String& label = "")
  { return mbrs.PSearchMinVal(mbr_nm, label); }
  // #CAT_PSearch_Access gets a reference to the param search min_val for given member name and, optionally if non-empty, the associated label
  virtual bool          PSearchMinVal_Set(double min_val, const String& mbr_nm, const String& label = "")
  { return mbrs.PSearchMinVal_Set(min_val, mbr_nm, label); }
  // #CAT_PSearch_Access set param search min_val for given member name and, optionally if non-empty, the associated label
  virtual double&       PSearchMaxVal(const String& mbr_nm, const String& label = "")
  { return mbrs.PSearchMaxVal(mbr_nm, label); }
  // #CAT_PSearch_Access gets a reference to the param search max_val for given member name and, optionally if non-empty, the associated label
  virtual bool          PSearchMaxVal_Set(double max_val, const String& mbr_nm, const String& label = "")
  { return mbrs.PSearchMaxVal_Set(max_val, mbr_nm, label); }
  // #CAT_PSearch_Access set param search max_val for given member name and, optionally if non-empty, the associated label
  virtual double&       PSearchNextVal(const String& mbr_nm, const String& label = "")
  { return mbrs.PSearchNextVal(mbr_nm, label); }
  // #CAT_PSearch_Access gets a reference to the param search next_val for given member name and, optionally if non-empty, the associated label
  virtual bool          PSearchNextVal_Set(double next_val, const String& mbr_nm, const String& label = "")
  { return mbrs.PSearchNextVal_Set(next_val, mbr_nm, label); }
  // #CAT_PSearch_Access set param search next_val for given member name and, optionally if non-empty, the associated label
  virtual double&       PSearchIncrVal(const String& mbr_nm, const String& label = "")
  { return mbrs.PSearchIncrVal(mbr_nm, label); }
  // #CAT_PSearch_Access gets a reference to the param search incr_val for given member name and, optionally if non-empty, the associated label
  virtual bool          PSearchIncrVal_Set(double incr_val, const String& mbr_nm, const String& label = "")
  { return mbrs.PSearchIncrVal_Set(incr_val, mbr_nm, label); }
  // #CAT_PSearch_Access set param search incr_val for given member name and, optionally if non-empty, the associated label
  virtual Variant       PSearchCurVal(const String& mbr_nm, const String& label = "")
  { return mbrs.PSearchCurVal(mbr_nm, label); }
  // #CAT_PSearch_Access gets current value for given member name and, optionally if non-empty, the associated label
  virtual bool          PSearchCurVal_Set(const Variant& cur_val, const String& mbr_nm, const String& label = "")
  { return mbrs.PSearchCurVal_Set(cur_val, mbr_nm, label); }
  // #CAT_PSearch_Access set current value for given member name and, optionally if non-empty, the associated label
  virtual bool          PSearchNextToCur(const String& mbr_nm, const String& label = "")
  { return mbrs.PSearchNextToCur(mbr_nm, label); }
  // #CAT_PSearch_Access set current value to stored next value for given member name and, optionally if non-empty, the associated label

public: // IRefListClient i/f
  override void*        This() {return this;}
  override void         SigDestroying_Ref(taBase_RefList* src, taBase* ta);
    // note: item will already have been removed from list
  override void         SigEmit_Ref(taBase_RefList* src, taBase* ta,
    int sls, void* op1, void* op2);

protected:
  int                   m_changing; // flag so we don't recursively delete bases
  taBase_RefList        base_refs; // all bases notify us via this list

  override void         UpdateAfterEdit_impl();
  virtual void          SigEmit_Group(taGroup_impl* grp, int sls, void* op1, void* op2);
    // mostly for detecting asynchronous deletes
  virtual void          BaseAdded(taBase* ta);
  virtual void          BaseRemoved(taBase* ta);
  virtual void          RemoveField_impl(int idx);
  virtual void          RemoveFun_impl(int idx);
  virtual bool          SelectMember_impl(taBase* base, MemberDef* md,
                  const String& lbl, const String& desc, const String& sub_gp_nm = _nilString);
  virtual bool          SelectMethod_impl(taBase* base, MethodDef* md,
                                          const String& lbl, const String& desc,
                                          const String& sub_gp_nm = _nilString);
  bool                  ReShowEdit(bool force = false); // this is just really a synonym for doing a SigEmit

private:
  void  Initialize();
  void  Destroy();
  void  Copy_(const SelectEdit& cp); //

public: // legacy routines/members
  SelectEditConfig config;      // #NO_SHOW #NO_SAVE special parameters for controlling the display

  taBase_List   mbr_bases;      // #NO_SHOW #NO_SAVE #LINK_GROUP #READ_ONLY #AKA_bases the bases for each element in the list
  String_Array  mbr_strs;       // #NO_SHOW #NO_SAVE #READ_ONLY #AKA_member_strs string names of mbrs on bases -- used for saving

  taBase_List   meth_bases;     // #NO_SHOW #NO_SAVE #LINK_GROUP #READ_ONLY the bases for each element in the list
  String_Array  meth_strs;      // #NO_SHOW #NO_SAVE #READ_ONLY string names of meths on bases -- used for saving
  void          ConvertLegacy();
};

TA_SMART_PTRS(SelectEdit); //

#endif // SelectEdit_h
