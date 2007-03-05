// Copyright, 1995-2005, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of TA/PDP++
//
//   TA/PDP++ is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   TA/PDP++ is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.



// spec.h

#ifndef spec_h
#define spec_h 1

#include "pdp_base.h"
#include "pdp_TA_type.h" // for templates

// forwards this file

class BaseSpec;

class PDP_API BaseSpec_Group: public taBase_Group {
  // ##EXT_spec ##CAT_Spec group of specs
INHERITED(taBase_Group)
public:
  static bool nw_itm_def_arg;	// #IGNORE default arg val for FindMake..


  virtual BaseSpec*	FindSpecType(TypeDef* td);
  // find a spec of eactly given type, searching in the children too
  virtual BaseSpec*	FindSpecInherits(TypeDef* td, TAPtr for_obj = NULL);
  // find a spec that inherits from given type, searching in the children too

  virtual BaseSpec*	FindSpecTypeNotMe(TypeDef* td, BaseSpec* not_me);
  // find a spec of eactly given type, searching in the children too, but skip not_me
  virtual BaseSpec*	FindSpecInheritsNotMe(TypeDef* td, BaseSpec* not_me, TAPtr for_obj = NULL);
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

  void	Initialize();
  void 	Destroy()		{ }
  TA_BASEFUNS(BaseSpec_Group);
};

class PDP_API BaseSpec : public taNBase {
  // ##EXT_spec ##MEMB_IN_GPMENU ##SCOPE_Network ##DEF_CHILD_children ##CAT_Spec base specification class
INHERITED(taNBase)
public:
  static bool nw_itm_def_arg;	// #IGNORE default arg val for FindMake..

  String		desc;	// #EDIT_DIALOG Description of what this variable is for
  String_Array unique; // #HIDDEN string list of unique members
  TypeDef*		min_obj_type;
  // #READ_ONLY #HIDDEN #NO_SAVE #TYPE_taBase mimimal object type required for spec
  BaseSpec_Group 	children;
  // #NO_INHERIT #IN_GPMENU sub-specs descending from this one and inheriting values
  
  override taList_impl*	children_() {return &children;}	

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

  virtual BaseSpec* NewChild();  // #MENU #MENU_ON_Edit #MENU_CONTEXT #TYPE_ON_el_base #CAT_Modify create a new childspec

  virtual bool  CheckType(TypeDef* td);
  // checks typedef type, issues error and returns false if not sufficient
  virtual bool	CheckObjectType(TAPtr obj);
  // checks object type, issues error and returns false if not sufficient
  virtual bool  CheckType_impl(TypeDef* td);
  // #IGNORE actually does the check
  virtual bool	CheckObjectType_impl(TAPtr obj);
  // #IGNORE actually does the check

  virtual BaseSpec* FindMakeChild(const char* nm, TypeDef* td = NULL, bool& nw_itm = nw_itm_def_arg, const char* alt_nm = NULL);
  // find a child spec of given name, and if not, make it (if nm is not found and alt_nm != NULL, it is searched for)
  virtual bool 	    RemoveChild(const char* nm, TypeDef* td = NULL);
  // remove a child based on name or type

  override String GetDesc() const { return desc; }
  void 	Initialize();
  void	Destroy();
  void	InitLinks();
  void	CutLinks();
  void 	Copy_(const BaseSpec& cp);
  COPY_FUNS(BaseSpec, taNBase);
  TA_BASEFUNS(BaseSpec);
protected:
  void	UpdateAfterEdit_impl();
};

class PDP_API BaseSubSpec : public taNBase {
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

  void 	Initialize();
  void	Destroy();
  void	InitLinks();
  void 	Copy_(const BaseSubSpec& cp);
  COPY_FUNS(BaseSubSpec, taNBase);
  TA_BASEFUNS(BaseSubSpec);
protected:
  void	UpdateAfterEdit_impl();
};

class PDP_API SpecPtr_impl : public taOBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS ##NO_UPDATE_AFTER ##CAT_Spec magic pointer to a spec
INHERITED(taOBase)
public:
  TypeDef*	base_type;	// #TYPE_BaseSpec #HIDDEN #NO_SAVE base type for type field
  TypeDef*	type;		// #TYPE_ON_base_type The type of the spec to use

  virtual BaseSpec* GetSpec() const	{ return NULL; } // get the spec pointer
  virtual void	SetSpec(BaseSpec*)	{ };	 	// set the spec pointer

  BaseSpec* 	operator=(BaseSpec* cp)	{ SetSpec(cp); return cp; }

  virtual void	SetDefaultSpec(TAPtr ownr, TypeDef* td); // for class that owns ptr
  virtual void	SetBaseType(TypeDef* td);		 // for overloaded classes
  virtual BaseSpec_Group* GetSpecGroup();		 // get the group where specs go
  virtual void	GetSpecOfType();			 // get a spec of type type

  override int	UpdatePointers_NewObj(taBase* old_ptr, taBase* new_ptr);

  void	Initialize();
  void 	Destroy()		{ };
  void	Copy_(const SpecPtr_impl& cp);
  COPY_FUNS(SpecPtr_impl, inherited);
  TA_BASEFUNS(SpecPtr_impl);
protected:
  override void	UpdateAfterEdit_impl();	// check, update the spec type
};

template<class T, TypeDef& typ> 
class SpecPtr : public SpecPtr_impl {
public:
  taSmartRefT<T,typ>	spec;		// #TYPE_ON_type the actual spec itself

  inline T*		SPtr() const		{ return spec.ptr(); }
  // use this call to access the spec pointer value in all client calls -- fast!
  override BaseSpec*	GetSpec() const		{ return SPtr(); }
  void		SetSpec(BaseSpec* es)   {
    if (spec.ptr() == es) return; // low level setting, ex. streaming, handled in UAE
    if(!owner) return;
    if(!es || (es->InheritsFrom(base_type) && es->CheckObjectType(owner))) {
      spec.set(es);
      if(es) { type = es->GetTypeDef(); }
      owner->UpdateAfterEdit();	// owner might need to apply this change to all sub guys
    }
    else {
      taMisc::Error("SetSpec: incorrect type of Spec:",
		    es->GetPath(), "of type:", es->GetTypeDef()->name,
		    "should be at least:", base_type->name,"in object:",owner->GetPath());
    }
  }
  bool		CheckSpec(TAPtr own_obj)   {
    if(own_obj == NULL) return false;
    if(!spec) {
      taMisc::CheckError("CheckSpec: spec is NULL in", own_obj->GetPath());
      return false;
    }
    if(!spec->InheritsFrom(base_type)) {
      taMisc::CheckError("CheckSpec: incorrect type of spec:", spec->GetPath(),
			 "of type:", spec->GetTypeDef()->name,
			 "should be at least:", base_type->name,"in object:",own_obj->GetPath());
      return false;
    }
    return spec->CheckObjectType(own_obj);
  }
  bool		CheckSpec() { return CheckSpec(owner); }

  void		SetDefaultSpec(TAPtr ownr, TypeDef* td)
  { SpecPtr_impl::SetDefaultSpec(ownr, td); }
  void		SetDefaultSpec(TAPtr ownr)
  { SetDefaultSpec(ownr, T::StatTypeDef(1)); }

  virtual T* 	NewChild()
  {  T* rval = (T*)spec->children.NewEl(1); rval->UpdateSpec(); return rval; }

  T* 		operator->() const	{ return SPtr(); }
  T* 		operator=(T* cp)	{ SetSpec(cp); return cp; }
  bool 		operator!=(T* cp) const	{ return (spec != cp); }
  bool 		operator==(T* cp) const	{ return (spec == cp); }
  
  operator T*()	const		{ return SPtr(); }
  operator BaseSpec*() const	{ return SPtr(); }
  operator bool() const 	{ return (bool)spec; }

  void 	Initialize()		{ }
  void	Destroy()		{ CutLinks(); }
  void  InitLinks()		{ SpecPtr_impl::InitLinks(); taBase::Own(spec, this); }
  void  CutLinks()		{ spec.CutLinks(); SpecPtr_impl::CutLinks(); }
  void	Copy_(const SpecPtr<T,typ>& cp) { spec.set(cp.SPtr()); } 
  void  Copy(const SpecPtr<T,typ>& cp)  { StructUpdate(true); SpecPtr_impl::Copy(cp); Copy_(cp); StructUpdate(false); }

  SpecPtr () { Initialize(); }
  SpecPtr (const SpecPtr<T,typ>& cp) { Initialize(); Copy(cp); }
  ~SpecPtr () { Destroy(); }
  taBase* Clone() { return new SpecPtr<T,typ>(*this); }
  void  UnSafeCopy(TAPtr cp) { if(cp->InheritsFrom(&TA_SpecPtr)) Copy(*((SpecPtr<T,typ>*)cp));
    else if(InheritsFrom(cp->GetTypeDef())) cp->CastCopyTo(this); }
  void  CastCopyTo(TAPtr cp) { SpecPtr<T,typ>& rf = *((SpecPtr<T,typ>*)cp); rf.Copy(*this); }
  TAPtr MakeToken(){ return (TAPtr)(new SpecPtr<T,typ>); }
  TAPtr MakeTokenAry(int no){ return (TAPtr)(new SpecPtr<T,typ>[no]); }
  void operator=(const SpecPtr<T,typ>& cp) { Copy(cp); }
  TypeDef* GetTypeDef() const { return &TA_SpecPtr; }
  static TypeDef* StatTypeDef(int) { return &TA_SpecPtr; }
};

#define SpecPtr_of(T) \
  class PDP_API T ## _SPtr : public SpecPtr<T, TA_ ## T> { \
public: \
  void 	Initialize() 		{ }; \
  void	Destroy()		{ }; \
  TA_BASEFUNS(T ## _SPtr); \
}

#endif // spec_h
