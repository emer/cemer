// Copyright, 1995-2007, Regents of the University of Colorado,
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


// spec.h

#ifndef SPEC_H
#define SPEC_H

#include "emergent_base.h"

// forwards this file

class BaseSpec;

class EMERGENT_API BaseSpec_Group: public taGroup<BaseSpec> {
  // ##EXT_spec ##CAT_Spec ##SCOPE_Network ##EXPAND_DEF_1 group of specs
INHERITED(taGroup<BaseSpec>)
public:
  static bool nw_itm_def_arg;	// #IGNORE default arg val for FindMake..


  virtual BaseSpec*	FindSpecType(TypeDef* td);
  // find a spec of eactly given type, searching in the children too
  virtual BaseSpec*	FindSpecInherits(TypeDef* td, taBase* for_obj = NULL);
  // find a spec that inherits from given type, searching in the children too

  virtual BaseSpec*	FindSpecTypeNotMe(TypeDef* td, BaseSpec* not_me);
  // find a spec of eactly given type, searching in the children too, but skip not_me
  virtual BaseSpec*	FindSpecInheritsNotMe(TypeDef* td, BaseSpec* not_me, taBase* for_obj = NULL);
  // find a spec that inherits from given type, searching in the children too

  virtual BaseSpec*	FindSpecName(const char* nm);
  // find a spec with given name, also searches in the children of each spec

  virtual BaseSpec* 	FindMakeSpec(const char* nm, TypeDef* td, bool& nw_itm = nw_itm_def_arg, const char* alt_nm = NULL);
  // find a given spec and if not found, make it  (if nm is not found and alt_nm != NULL, it is searched for)

  virtual bool 		RemoveSpec(const char* nm, TypeDef* td);
  // find a given spec and remove it

  virtual BaseSpec*	FindParent();
  // #MENU #USE_RVAL #MENU_ON_Actions Find the parent spec of this one

  override String 	GetTypeDecoKey() const;


  virtual void	Defaults();
  // #BUTTON #CONFIRM #CAT_ObjectMgmt for all specs in this group: restore specs to their default original parameter values, for parameters that have a strong default value -- WARNING: you will lose any unique parameters for anything that has a strong default value

  void	Initialize();
  void 	Destroy()		{ }
  TA_BASEFUNS_NOCOPY(BaseSpec_Group);
};
TA_SMART_PTRS(BaseSpec_Group)

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


class EMERGENT_API SpecMemberBase : public taOBase {
  // ##EXT_spec ##SCOPE_Network ##CAT_Spec base type for members of spec objects -- defines interface for default values
friend class BaseSpec;
INHERITED(taOBase)
public:

  virtual void	Defaults();
  // #BUTTON #CONFIRM #CAT_ObjectMgmt restore specs to their default original parameter values, for parameters that have a strong default value -- WARNING: you will lose any unique parameters for anything that has a strong default value
  // note: typically do NOT redefine basic Defaults function -- see SPEC_DEFAULTS comment above

  TA_BASEFUNS_NOCOPY(SpecMemberBase);
protected:
  virtual void	Defaults_impl() { };
  // #IGNORE this is base of impl class -- derived cases should call inherited then do their thing (usu just Defaults_init) -- very much like UAE_impl -- note that Defaults cals Defaults_Members
  virtual void	DefaultsMembers();
  // #IGNORE just a simple loop to call defaults on all the SpecMemberBase and BaseSpec member objects in a class

private:
  void 	Initialize() { };
  void	Destroy()    { };
  void	Defaults_init() { };
  // #IGNORE put basic default params init stuff in here -- can check other params that might have been set -- Initialize can call this + do other housekeeping inits, or this can call Initialize if the two are sufficiently synonymous
};

class EMERGENT_API BaseSpec : public taNBase {
  // ##EXT_spec ##MEMB_IN_GPMENU ##SCOPE_Network ##DEF_CHILD_children ##DEF_CHILDNAME_Sub_Specs ##CAT_Spec base specification class
friend class BaseSpec_Group;
friend class SpecMemberBase;
INHERITED(taNBase)
public:
  static bool nw_itm_def_arg;	// #IGNORE default arg val for FindMake..

  String		desc;	// #EDIT_DIALOG Description of what this variable is for
  String_Array unique; // #HIDDEN string list of unique members
  TypeDef*		min_obj_type;
  // #READ_ONLY #HIDDEN #NO_SAVE #TYPE_taBase mimimal object type required for spec
  BaseSpec_Group 	children;
  // #NO_INHERIT #IN_GPMENU #DIFF_LAST sub-specs descending from this one and inheriting values
  
  override taList_impl*	children_() {return &children;}	

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

  override bool	    UAEProgramDefault() { return true; }
  override String GetDesc() const { return desc; }

  virtual void	Defaults();
  // #BUTTON #CONFIRM #CAT_ObjectMgmt restore specs to their default original parameter values, for parameters that have a strong default value -- WARNING: you will lose any unique parameters for anything that has a strong default value
  // note: typically do NOT redefine basic Defaults function -- see SPEC_DEFAULTS comment above

  void	InitLinks();
  void	CutLinks();
  TA_BASEFUNS(BaseSpec);
protected:
  void	UpdateAfterEdit_impl();
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

class EMERGENT_API BaseSubSpec : public taNBase {
  // ##EXT_spec ##MEMB_IN_GPMENU ##CAT_Spec specification class for sub-objects of specs
INHERITED(taNBase)
public:
  String_Array unique; // #HIDDEN string list of unique members
  virtual BaseSubSpec*	FindParent();
  // #MENU #USE_RVAL #MENU_ON_Actions Find the corresponding parent subspec of this one
  virtual BaseSpec*	FindParentBaseSpec();
  // #MENU #USE_RVAL Find the parent spec of this one

  virtual void	SetUnique(int memb_no, bool on); // set inherit bit
  virtual void	SetUnique(const char* memb_nm, bool on); // set inherit bit
  virtual bool	GetUnique(int memb_no);	     	// check inherit bit
  virtual bool	GetUnique(const char* memb_nm);	// check inherit bit

  virtual void	UpdateMember(BaseSubSpec* from, int memb_no);
  // copy member from given sub spec if not unique
  virtual void	UpdateSpec();
  // update from parent sub spec, if one exists

  void	InitLinks();
  TA_BASEFUNS(BaseSubSpec);
protected:
  void	UpdateAfterEdit_impl();
private:
  void 	Copy_(const BaseSubSpec& cp);
  void 	Initialize();
  void	Destroy();
};

class EMERGENT_API SpecPtr_impl : public taOBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS ##NO_UPDATE_AFTER ##CAT_Spec ##SMART_POINTER magic pointer to a spec
INHERITED(taOBase)
public:
  TypeDef*	base_type;	// #TYPE_BaseSpec #HIDDEN #NO_SAVE base type for type field
  TypeDef*	type;		// #TYPE_ON_base_type The type of the spec to use

  virtual BaseSpec* GetSpec() const	{ return NULL; } // get the spec pointer
  virtual bool	SetSpec(BaseSpec*)	{ return false; } // set the spec pointer

  BaseSpec* 	operator=(BaseSpec* cp)	{ SetSpec(cp); return cp; }

  virtual void	SetDefaultSpec(taBase* ownr, TypeDef* td);
  // for class that owns ptr
  virtual void	SetBaseType(TypeDef* td);
  // for overloaded classes
  virtual BaseSpec_Group* GetSpecGroup();
  // get the group where specs go
  virtual void	GetSpecOfType(bool verbose = false);
  // get a spec of type type in GetSpecGroup group -- if existing one is not found, make a new one -- verbose = report about it

  virtual void	CheckSpec(TypeDef* obj_td = NULL);
  // check the spec for !NULL and type match, and for proper type of object (obj_td) that is using this spec (if obj_td is null, it is set to owner type -- can be diff for various other cases) -- fix if out of whack -- this is what used to be called in the UpdateAfterEdit and is now called in the network CheckSpecs prior to build

  virtual bool	CheckObjTypeForSpec(TypeDef* obj_td = NULL);
  // check for proper type of object (obj_td) that is using this spec (if obj_td is null, it is set to owner type -- can be diff for various other cases) -- just a check -- no message or anything (message is in CheckSpec

  override taBase* UpdatePointers_NewPar_FindNew(taBase* old_guy, taBase* old_par,
						 taBase* new_par);
  override int	UpdatePointers_NewObj(taBase* old_ptr, taBase* new_ptr);

  TA_BASEFUNS(SpecPtr_impl);
protected:
  override void	UpdateAfterEdit_impl();
private:
  void	Initialize();
  void 	Destroy()		{ };
  void	Copy_(const SpecPtr_impl& cp);
};

template<class T> 
class SpecPtr : public SpecPtr_impl {
INHERITED(SpecPtr_impl)
public:
  taSmartRefT<T>	spec;		// #TYPE_ON_type the actual spec itself

  inline T*		SPtr() const		{ return spec.ptr(); }
  // use this call to access the spec pointer value in all client calls -- fast!
  override BaseSpec*	GetSpec() const		{ return SPtr(); }

  override bool		SetSpec(BaseSpec* es)   {
    if (spec.ptr() == es) return true; // low level setting, ex. streaming, handled in UAE
    if(!owner) return false;
    if(!es || (es->InheritsFrom(base_type) && es->CheckObjectType(owner))) {
      spec.set(es);
      if(es) { type = es->GetTypeDef(); es->SpecSet(owner);}
      owner->UpdateAfterEdit();	// owner might need to apply this change to all sub guys
      return true;
    }
    taMisc::Error("SetSpec: incorrect type of Spec:",
		  es->GetPath(), "of type:", es->GetTypeDef()->name,
		  "should be at least:", base_type->name,"in object:",owner->GetPath());
    return false;
  }

  void		SetDefaultSpec(taBase* ownr, TypeDef* td)
  { SpecPtr_impl::SetDefaultSpec(ownr, td); }
  void		SetDefaultSpec(taBase* ownr)
  { SetDefaultSpec(ownr, T::StatTypeDef(1)); }

  virtual T* 	NewChild()
  {  T* rval = (T*)spec->children.NewEl(1); rval->UpdateSpec(); return rval; }

  T* 		operator->() const	{ return SPtr(); }
  T* 		operator=(T* cp)	{ SetSpec(cp); return cp; }
  bool 		operator!=(T* cp) const	{ return (spec.ptr() != cp); }
  bool 		operator==(T* cp) const	{ return (spec.ptr() == cp); }
  
  operator T*()	const		{ return SPtr(); }
  operator BaseSpec*() const	{ return SPtr(); }
  operator bool() const 	{ return (bool)spec; }

  void  InitLinks()		{ SpecPtr_impl::InitLinks(); taBase::Own(spec, this); }
  void  CutLinks()		{ spec.CutLinks(); SpecPtr_impl::CutLinks(); }
  
  TA_TMPLT_BASEFUNS_LITE(SpecPtr,T)
private:
  void	Copy_(const SpecPtr<T>& cp) { spec.set(cp.SPtr()); } 
  void 	Initialize()		{ }
  void	Destroy()		{ CutLinks(); }
};

#define SpecPtrEx_of(T, xxx_API) \
class xxx_API T ## _SPtr : public SpecPtr<T> { \
private: \
  typedef SpecPtr<T> inherited;\
  void Copy_(const T ## _SPtr&) {} \
  void 	Initialize() { }; \
  void	Destroy() { }; \
public: \
  TA_BASEFUNS_LITE(T ## _SPtr); \
}

#define SpecPtr_of(T) SpecPtrEx_of(T,EMERGENT_API) 

#endif // spec_h
