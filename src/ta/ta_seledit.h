// Copyright, 1995-2005, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of TA/CSS
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


//////////////////////////////////
//	select edit dialog	//
//////////////////////////////////

class TA_API SelectEditConfig : public taBase {
  // #EDIT_INLINE ##NO_TOKENS ##CAT_Display special parameters for controlling the select edit display
  INHERITED(taBase)
public:
  bool		auto_edit;	// automatically bring up edit dialog upon loading
  String_Array	mbr_labels;	// extra labels at the start of each member label for the selected fields
  String_Array	meth_labels;	// extra labels at the start of each method label for the selected functions

  void	Initialize();
  void	Destroy();
  void	InitLinks();
  void	Copy_(const SelectEditConfig& cp);
  TA_BASEFUNS(SelectEditConfig);
};


class TA_API SelectEdit : public taNBase {
  // ##EXT_edit ##CAT_Display Selectively edit members from different objects
  INHERITED(taNBase)
public:
  static void		BaseClosingAll(taBase* obj);
  static void		BaseDataChangedAll(taBase*, int dcr, void* obj1, void* obj2);
  
  String	desc;		// #EDIT_DIALOG description of what this edit contains
  SelectEditConfig config;	// special parameters for controlling the display

  taBase_List	mbr_bases;	// #LINK_GROUP #READ_ONLY #AKA_bases the bases for each element in the list
  String_Array	mbr_strs;	// #READ_ONLY #AKA_member_strs string names of mbrs on bases -- used for saving
  Member_List	members;	// #READ_ONLY #NO_SAVE member defs
  String_Array	mbr_base_paths; // #READ_ONLY #NO_SAVE paths to base objects for BaseChangeSave

  taBase_List	meth_bases;	// #LINK_GROUP #READ_ONLY the bases for each element in the list
  String_Array	meth_strs;	// #READ_ONLY string names of meths on bases -- used for saving
  Method_List	methods;	// #READ_ONLY #NO_SAVE method defs
  String_Array	meth_base_paths; // #READ_ONLY #NO_SAVE paths to base objects for BaseChangeSave

  virtual int	FindMbrBase(taBase* base, MemberDef* md);
  // find a given base and member, returns index
  virtual bool	SelectMember_impl(taBase* base, MemberDef* md, const char* lbl);
  // #IGNORE add new member to edit if it isn't already here (returns true), otherwise update lbl (returns false)
  virtual bool	SelectMember(taBase* base, MemberDef* md, const char* lbl);
  // add new member to edit if it isn't already here (returns true), otherwise update lbl (returns false)
  virtual bool	SelectMemberNm(taBase* base, const char* md, const char* lbl);
  // add new member to edit if it isn't already here (returns true), otherwise update lbl (returns false)

  virtual int	FindMethBase(taBase* base, MethodDef* md);
  // find a given base and method, returns index
  virtual bool	SelectMethod_impl(taBase* base, MethodDef* md, const char* lbl);
  // #IGNORE add new method to edit if it isn't already here (returns true), otherwise remove (returns false)
  virtual bool	SelectMethod(taBase* base, MethodDef* md, const char* lbl);
  // add new method to edit if it isn't already here (returns true), otherwise remove (returns false)
  virtual bool	SelectMethodNm(taBase* base, const char* md, const char* lbl);
  // add new method to edit if it isn't already here (returns true), otherwise remove (returns false)

  virtual void	UpdateAllBases();	// perform update-after-edit on all base objects

  virtual void	RemoveField(int idx);
  //  remove edit data item at given index and update dialog
  virtual void	MoveField(int from, int to);
  // #MENU #MENU_ON_SelectEdit move member to edit from index to index
  virtual void	RemoveField_impl(int idx);
  // #IGNORE just remove the fields

  virtual void	RemoveFun(int idx);
  //  remove function at given index
  virtual void	MoveFun(int from, int to);
  // #MENU move function to edit from index to index
  virtual void	RemoveFun_impl(int idx);
  // #IGNORE just remove the fields

  virtual void	NewEdit();
  // closes current edit dialog and makes a new one (with any changes)

  // todo: following should be #BUTTON but it doesn't work..
  virtual void	Reset();
  // #MENU #MENU_SEP_BEFORE #CONFIRM reset (remove all) current members and methods
  virtual int	SearchMembers(taNBase* obj, const String& memb_contains);
  // #MENU search given object for member names that contain given string, and add them to this editor
  virtual int	CompareObjs(taBase* obj_a, taBase* obj_b);
  // #MENU #TYPE_taNBase compare two objects (must be of the same type) and add the differences in this select edit

  virtual void	GetMembsFmStrs(); // #IGNORE get members from strings (upon loading)
  virtual void	GetMethsFmStrs(); // #IGNORE get methods from strings (upon loading)
  virtual void	GetAllPaths();	// #IGNORE get paths for all current objects

  virtual void	BaseChangeSave(); // #IGNORE close edit dialog and save paths to current bases
  virtual void	BaseChangeReShow(); // #IGNORE re-show the edit dialog loading bases from saved paths

  virtual String GetMbrLabel(int idx); // #IGNORE get full label for member
  virtual String GetMethLabel(int idx);	// #IGNORE get full label for method

  override int	UpdatePointers_NewPar(taBase* old_par, taBase* new_par);
  override int	Dump_Load_Value(istream& strm, taBase* par=NULL);
  // reset everything before loading
  override int	Dump_Save_Value(ostream& strm, taBase* par=NULL, int indent = 0);
  // get paths before saving

  override String 	GetTypeDecoKey() const { return "SelectEdit"; }
  override String	GetDesc() const { return desc; }

  // note: this is intentionally not _impl:
  void	UpdateAfterEdit();
  void 	InitLinks();
  void	Copy_(const SelectEdit& cp);
  TA_BASEFUNS(SelectEdit);
protected:  
  void			BaseAdded(taBase* ta); // called when we add a memb or meth; we unique add to notify list
  void			BaseRemoved(taBase* ta); // called after we remove a memb or meth; if no more of this base, we unmonitor
  virtual bool		BaseClosing(taBase* base);
  // #IGNORE this base object is about to be closed (removed), if i edit it, then I need to save and reopen (returns true if edited)
  virtual bool		BaseDataChanged(taBase* obj,
    int dcr, void* op1_, void* op2_);
  
private:
  void	Initialize();
  void	Destroy() {}
};


#endif
