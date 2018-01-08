// Copyright 2013-2018, Regents of the University of Colorado,
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
#include <NameVar_Array>

// declare all other types mentioned but not required to include:
class SpecUser; //

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
  void Defaults_impl() override { inherited::Defaults_impl(); Defaults_init(); }


eTypeDef_Of(BaseSpec);

class E_API BaseSpec : public taNBase {
  // ##EXT_spec ##MEMB_IN_GPMENU ##SCOPE_Network ##DEF_CHILD_children ##DEF_CHILDNAME_Sub_Specs ##CAT_Spec ##UAE_IN_PROGRAM ##EXPAND base specification class
friend class BaseSpec_Group;
friend class SpecMemberBase;
INHERITED(taNBase)
public:
  enum UsedStatus {
    UNUSED,         // this spec has no children and is unused OR this spec and none of its children are used by any objects
    USED,           // this spec has no children and is used OR this spec and all of its children are used
    PARENT_USED,    // this spec is used but one or more of its child specs are unused
    CHILD_USED,     // this spec is not used but one or more of its child specs are used
  };
  static bool       nw_itm_def_arg;	// #IGNORE default arg val for FindMake..

  bool              is_used;    // #READ_ONLY #HIDDEN #NO_INHERIT is THIS spec used - ignore usage of children
  UsedStatus        used_status;  // #HIDDEN #NO_INHERIT for gui use only to indicate to user the usage of this spec and child specs
  
  bool              is_new;     // #READ_ONLY #HIDDEN #NO_INHERIT #NO_SAVE is this spec newly created?  if so, prompt user to apply it to some objects..
  String            desc;	// #EDIT_DIALOG #NO_INHERIT Description of what this variable is for
  String_Array      unique; // #HIDDEN string list of unique members
  NameVar_Array     saved; // #HIDDEN values of members that are not overriding parent values - reset to these values if override is click
  TypeDef*          min_obj_type;
  // #READ_ONLY #HIDDEN #NO_SAVE #TYPE_taBase mimimal object type required for spec -- for object that the spec actually operates on
  TypeDef*          min_user_type;
  // #READ_ONLY #HIDDEN #NO_SAVE #TYPE_taBase mimimal object type required for spec -- for object that manages the spec and the user applies it to
  BaseSpec_Group    children;
  // #NO_INHERIT #IN_GPMENU #DIFF_LAST sub-specs descending from this one and inheriting values
  
  taList_impl*      children_() override {return &children;}
  Variant           Elem(const Variant& idx, IndexMode mode = IDX_UNK) const override
            { return children.Elem(idx, mode); }

  bool              SpecInheritsFrom(BaseSpec* spec) const;
    // returns true if this spec is the spec, or is one of its children
  virtual BaseSpec* FindParent() const;
  // #MENU #USE_RVAL #MENU_ON_Actions Find the parent spec of this one

  virtual void	SetUnique(int memb_no, bool on); // set inherit bit
  virtual void	SetUnique(const String& memb_nm, bool on); // set inherit bit
  virtual bool	GetUnique(int memb_no) const;	     	  // check inherit bit
  virtual bool	GetUnique(const String& memb_nm) const; // check inherit bit

  virtual bool  IsInheritedAndHasParent(const String& memb_nm) const;
  // is given member inherited from its parent (i.e., NOT unique) and parent actually has this member ot inherit from

  virtual void	UpdateMember(BaseSpec* from, int memb_no);
  // copy member from given parent
  virtual void	UpdateSpec();
  // update values from parent, and update children
  virtual void	UpdateChildren();	// update any children
  virtual void	UpdateSubSpecs() { };	// update any subspec objects (overload me)

  virtual BaseSpec* NewChild(TypeDef* child_type);
  // #BUTTON #TYPE_ON_children.el_base #NO_SAVE_ARG_VAL #CAT_Modify create a new child spec that inherits from this one but can have its own unique parameters
  bool          ChooseMe() override;
  // #IGNORE pop chooser for selecting type of spec to create
  
  virtual bool  CheckType(TypeDef* td);
  // checks typedef type, issues error and returns false if not sufficient
  virtual bool	CheckObjectType(taBase* obj);
  // checks object type, issues error and returns false if not sufficient
  virtual void	SpecSet(taBase* obj);
  // #IGNORE called just after the spec was set but before obj->UAE
  virtual void	SpecUnSet(taBase* obj);
  // #IGNORE called just after the spec was un-set from given object

  virtual BaseSpec* FindMakeChild(const String& nm, TypeDef* td = NULL, bool& nw_itm = nw_itm_def_arg, const String& alt_nm = NULLStr);
  // find a child spec of given name, and if not, make it (if nm is not found and alt_nm != NULL, it is searched for)
  virtual bool 	    RemoveChild(const String& nm, TypeDef* td = NULL);
  // remove a child based on name or type

  void            MemberUpdateAfterEdit(MemberDef* md, bool edit_dialog = false) override;
  String          GetDesc() const override { return desc; }

  virtual void    ApplyTo(SpecUser* obj1, SpecUser* obj2=NULL, SpecUser* obj3=NULL,
                          SpecUser* obj4=NULL);
  // #BUTTON #NULL_OK #TYPE_ON_min_user_type #PREFER_NULL apply this spec to given object(s) (leave NULL any that are not needed)
  
  virtual void	  Defaults();
  // #BUTTON #CONFIRM #CAT_ObjectMgmt restore specs to their default original parameter values, for parameters that have a strong default value -- WARNING: you will lose any unique parameters for anything that has a strong default value
  // note: typically do NOT redefine basic Defaults function -- see SPEC_DEFAULTS comment above

  virtual void    WhereUsed();
  // #BUTTON #ARGC_0 display a clickable list of objects that use this spec
  virtual void    WhereUsed_impl(taBase_PtrList& spec_user_list, String_Array& spec_name_list, bool child);
  // fill the list of objects where this spec is used - if searching for child specs of the original spec pass true (additional feedback to user)
  virtual bool    UpdtIsUsed(bool gui_updt = true);
  // update the is_used flag and used_status variable -- returns is_used flag

  String          GetStateDecoKey() const override;
  
  void            SetMember(const String& member, const String& value) override;
  // #DYN1 . This also automatically sets the spec unique flag to 'On' so that this value is unique to this spec, and updates all the children of this spec so they can inherit this change (or not depending on their unique flags).
  bool            IsMemberEditable(const String& memb_name) const override;

  virtual void    CompareWithChildren();
  // #BUTTON calls network::SpecCompareWithChildren to populate a table that compares this specs member values with its children's values - this data does not update - call again if you edit spec values!
  virtual void    ComparePeers(BaseSpec* spec);
  // #DYN12N calls network::SpecComparePeers to populate a table that compares this specs member values with its children's values - this data does not update - call again if you edit spec values!

  taBase*         ChooseNew(taBase* origin, const String& choice_text) override;
  bool            HasChooseNew() override { return true; }

  virtual void    CopyToState(void* state_spec, const char* state_suffix);
  // #IGNORE use typedef info to copy all of our parameters to corresponding State version of this spec type
  virtual void    UpdateStateSpecs() { };
  // Update our corresponding state-side specs -- called in UAE, and call when specifically updating parameters -- each base class needs to define this, getting the proper specs etc
  virtual void    ResetAllSpecIdxs();
  // #IGNORE reset all the state spec_idx spec indexes to -1 -- called prior to building so only used specs have a valid spec_idx
  
  void	InitLinks() override;
  void	CutLinks() override;
  TA_BASEFUNS(BaseSpec);
protected:
  void	UpdateAfterEdit_impl() override;
  void  CheckThisConfig_impl(bool quiet, bool& ok) override;
  void  CheckChildConfig_impl(bool quiet, bool& rval) override;
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
