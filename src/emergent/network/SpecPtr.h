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

#ifndef SpecPtr_h
#define SpecPtr_h 1

// parent includes:
#include <taOBase>
#include <BaseSpec>

// member includes:

// declare all other types mentioned but not required to include:

eTypeDef_Of(SpecPtr_impl);

class E_API SpecPtr_impl : public taOBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS ##NO_UPDATE_AFTER ##CAT_Spec magic pointer to a spec
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

  taBase* UpdatePointers_NewPar_FindNew(taBase* old_guy, taBase* old_par,
						 taBase* new_par) override;
  int	UpdatePointers_NewObj(taBase* old_ptr, taBase* new_ptr) override;

  TA_BASEFUNS(SpecPtr_impl);
protected:
  void	UpdateAfterEdit_impl() override;
private:
  void	Initialize();
  void 	Destroy()		{ };
  void	Copy_(const SpecPtr_impl& cp);
};

eTypeDef_Of(SpecPtr);

template<class T> 
class SpecPtr : public SpecPtr_impl {
INHERITED(SpecPtr_impl)
public:
  taSmartRefT<T>	spec;		// #TYPE_ON_type the actual spec itself

  inline T*		SPtr() const		{ return spec.ptr(); }
  // use this call to access the spec pointer value in all client calls -- fast!
  BaseSpec*	GetSpec() const	override { return SPtr(); }

  bool		SetSpec(BaseSpec* es) override  {
    if (spec.ptr() == es) return true; // low level setting, ex. streaming, handled in UAE
    if(!owner) return false;
    if(!es || (es->InheritsFrom(base_type) && es->CheckObjectType(owner))) {
      spec.set(es);
      if(es) { type = es->GetTypeDef(); es->SpecSet(owner);}
      owner->UpdateAfterEdit();	// owner might need to apply this change to all sub guys
      return true;
    }
    TestError(true, "SetSpec", "incorrect type of Spec:",
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
  
  TA_TMPLT_BASEFUNS_LITE(SpecPtr,T);
private:
  void	Copy_(const SpecPtr<T>& cp) { spec.set(cp.SPtr()); } 
  void 	Initialize()		{ }
  void	Destroy()		{ CutLinks(); }
};

#define SPECPTREX_OF(API, T)            \
SMARTREF_OF(API, T); \
class API T ## _SPtr : public SpecPtr<T> { \
private: \
  typedef SpecPtr<T> inherited;\
  void  Copy_(const T ## _SPtr&) {} \
  void 	Initialize() { }; \
  void	Destroy() { }; \
public: \
  TA_BASEFUNS_LITE(T ## _SPtr); \
}

#define SPECPTR_OF(T) eTypeDef_Of(T ## _SPtr ); SPECPTREX_OF(E_API, T);

#endif // SpecPtr_h
