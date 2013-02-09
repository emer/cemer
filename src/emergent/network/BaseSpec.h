// Copyright, 1995-2013, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of Emergent
//
//   Emergent is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   Emergent is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.

#ifndef BaseSpec_h
#define BaseSpec_h 1

// parent includes:
#include <taBase>
#include "network_def.h"

// member includes:
#include <BaseSpec_Group>
#include <String_Array>

// declare all other types mentioned but not required to include:

// this should be included in all specs to allow user to restore default params
// put class specific Initialize code into Defaults_impl and have Initialize call that
// or define Defaults_impl to be Initialize if there are no incompatibilities --
// Defaults_impl should NOT init any housekeeping or other params that might have been
// set according to a normal specialization of function depending on different uses
// Defaults_impl can check for these different states etc as necc (in which case make
// sure it is all ok if done within the Initialize step if it is called there!)
// In no case shoudl Defaults_impl call ANY inherited functions!!
// MemberDefaults only calls Defaults for the type that owns the member, and only
// for things above the BaseSpec level
#define SPEC_DEFAULTS \
  void Defaults_impl() { inherited::Defaults_impl(); Defaults_init(); }


eTypeDef_Of(BaseSpec);

class E_API BaseSpec : public taNBase {
  // ##EXT_spec ##MEMB_IN_GPMENU ##SCOPE_Network ##DEF_CHILD_children ##DEF_CHILDNAME_Sub_Specs ##CAT_Spec base specification class
friend class BaseSpec_Group;
friend class SpecMemberBase;
INHERITED(taNBase)
public:
  static bool nw_itm_def_arg;	// #IGNORE default arg val for FindMake..

  String		desc;	// #EDIT_DIALOG Description of what this variable is for
  String_Array          unique; // #HIDDEN string list of unique members
  TypeDef*		min_obj_type;
  // #READ_ONLY #HIDDEN #NO_SAVE #TYPE_taBase mimimal object type required for spec
  BaseSpec_Group 	children;
  // #NO_INHERIT #IN_GPMENU #DIFF_LAST sub-specs descending from this one and inheriting values
  
  override taList_impl*	children_() {return &children;}	
  override Variant      Elem(const Variant& idx, IndexMode mode = IDX_UNK) const
  { return children.Elem(idx, mode); }

  bool			SpecInheritsFrom(BaseSpec* spec) const;
    // returns true if this spec is the spec, or is one of its children
  virtual BaseSpec*	FindParent();
  // #MENU #USE_RVAL #MENU_ON_Actions Find the parent spec of this one

  virtual void	SetUnique(int memb_no, bool on); // set inherit bit
  virtual void	SetUnique(const char* memb_nm, bool on); // set inherit bit
  virtual bool	GetUnique(int memb_no);	     	// check inherit bit
  virtual bool	GetUnique(const char* memb_nm);	// check inherit bit

  virtual void	UpdateMember(BaseSpec* from, int memb_no);
  // copy member from given parent
  virtual void	UpdateSpec();
  // update values from parent, and update children
  virtual void	UpdateChildren();	// update any children
  virtual void	UpdateSubSpecs() { };	// update any subspec objects (overload me)

  virtual BaseSpec* NewChild(TypeDef* child_type);
  // #BUTTON #TYPE_ON_children.el_base #NO_SAVE_ARG_VAL #CAT_Modify create a new child spec that inherits from this one but can have its own unique parameters

  virtual bool  CheckType(TypeDef* td);
  // checks typedef type, issues error and returns false if not sufficient
  virtual bool	CheckObjectType(taBase* obj);
  // checks object type, issues error and returns false if not sufficient
  virtual void	SpecSet(taBase* obj) {}
  // #IGNORE called just after the spec was was set but before obj->UAE

  virtual BaseSpec* FindMakeChild(const char* nm, TypeDef* td = NULL, bool& nw_itm = nw_itm_def_arg, const char* alt_nm = NULL);
  // find a child spec of given name, and if not, make it (if nm is not found and alt_nm != NULL, it is searched for)
  virtual bool 	    RemoveChild(const char* nm, TypeDef* td = NULL);
  // remove a child based on name or type

  override bool	  UAEProgramDefault() { return true; }
  override void	  MemberUpdateAfterEdit(MemberDef* md, bool edit_dialog = false);
  override String GetDesc() const { return desc; }

  virtual void	  Defaults();
  // #BUTTON #CONFIRM #CAT_ObjectMgmt restore specs to their default original parameter values, for parameters that have a strong default value -- WARNING: you will lose any unique parameters for anything that has a strong default value
  // note: typically do NOT redefine basic Defaults function -- see SPEC_DEFAULTS comment above

  virtual String  WhereUsed();
  // #BUTTON #USE_RVAL returns a list of objects where this spec is used

  void	InitLinks();
  void	CutLinks();
  TA_BASEFUNS(BaseSpec);
protected:
  override void	UpdateAfterEdit_impl();
  override void CheckChildConfig_impl(bool quiet, bool& rval);
  virtual bool  CheckType_impl(TypeDef* td);
  // #IGNORE actually does the check
  virtual bool	CheckObjectType_impl(taBase* obj);
  // #IGNORE actually does the check
  virtual void	Defaults_impl() { };
  // #IGNORE this is base of impl class -- derived cases should call inherited then do their thing (usu just Defaults_init) -- very much like UAE_impl -- note that Defaults cals Defaults_Members
  virtual void	DefaultsMembers();
  // #IGNORE just a simple loop to call defaults on all the member objects in a class

private:
  void 	Initialize();
  void	Destroy();
  void 	Copy_(const BaseSpec& cp);
  void	Defaults_init() { };
  // #IGNORE put basic default params init stuff in here -- can check other params that might have been set -- Initialize can call this + do other housekeeping inits, or this can call Initialize if the two are sufficiently synonymous
};

#endif // BaseSpec_h
