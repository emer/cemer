/* -*- C++ -*- */
/*=============================================================================
//									      //
// This file is part of the PDP++ software package.			      //
//									      //
// Copyright (C) 1995 Randall C. O'Reilly, Chadley K. Dawson, 		      //
//		      James L. McClelland, and Carnegie Mellon University     //
//     									      //
// Permission to use, copy, and modify this software and its documentation    //
// for any purpose other than distribution-for-profit is hereby granted	      //
// without fee, provided that the above copyright notice and this permission  //
// notice appear in all copies of the software and related documentation.     //
//									      //
// Permission to distribute the software or modified or extended versions     //
// thereof on a not-for-profit basis is explicitly granted, under the above   //
// conditions. 	HOWEVER, THE RIGHT TO DISTRIBUTE THE SOFTWARE OR MODIFIED OR  //
// EXTENDED VERSIONS THEREOF FOR PROFIT IS *NOT* GRANTED EXCEPT BY PRIOR      //
// ARRANGEMENT AND WRITTEN CONSENT OF THE COPYRIGHT HOLDERS.                  //
// 									      //
// Note that the taString class, which is derived from the GNU String class,  //
// is Copyright (C) 1988 Free Software Foundation, written by Doug Lea, and   //
// is covered by the GNU General Public License, see ta_string.h.             //
// The iv_graphic library and some iv_misc classes were derived from the      //
// InterViews morpher example and other InterViews code, which is             //
// Copyright (C) 1987, 1988, 1989, 1990, 1991 Stanford University             //
// Copyright (C) 1991 Silicon Graphics, Inc.				      //
//									      //
// THE SOFTWARE IS PROVIDED "AS-IS" AND WITHOUT WARRANTY OF ANY KIND,         //
// EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY 	      //
// WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.  	      //
// 									      //
// IN NO EVENT SHALL CARNEGIE MELLON UNIVERSITY BE LIABLE FOR ANY SPECIAL,    //
// INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY KIND, OR ANY DAMAGES  //
// WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER OR NOT     //
// ADVISED OF THE POSSIBILITY OF DAMAGE, AND ON ANY THEORY OF LIABILITY,      //
// ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS        //
// SOFTWARE. 								      //
==============================================================================*/

// spec.h

#ifndef spec_h
#define spec_h 1

#include "pdpbase.h"
#include "pdp_TA_type.h" // for templates

#define MAX_SPEC_LONGS	(int)((256 / (sizeof(long) * 8)) + 1)

#ifdef TA_GUI
class taiMenuEl; //
#endif

// forwards this file

class BaseSpec;

class BaseSpec_MGroup: public taBase_Group {
#ifndef __MAKETA__
  typedef taBase_Group inherited;
#endif
  // group of specs
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

  const iColor* GetEditColor();

  void	Initialize();
  void 	Destroy()		{ }
  TA_BASEFUNS(BaseSpec_MGroup);

#ifdef TA_GUI
public:
  virtual void		NewChildSpec_mc(taiMenuEl* sel); // callback for new child spec
#endif
};



class BaseSpec : public taNBase {
  // ##EXT_spec ##MEMB_IN_GPMENU base specification class
public:
  static bool nw_itm_def_arg;	// #IGNORE default arg val for FindMake..

  unsigned long unique[MAX_SPEC_LONGS]; // #HIDDEN bits representing members unique
  TypeDef*		min_obj_type;
  // #HIDDEN #NO_SAVE #TYPE_taBase mimimal object type required for spec
  BaseSpec_MGroup 	children;
  // #NO_INHERIT #IN_GPMENU sub-specs descending from this one and inheriting values

  virtual BaseSpec*	FindParent();
  // #MENU #USE_RVAL #MENU_ON_Actions Find the parent spec of this one

  virtual void	SetUnique(int memb_no, bool onoff); // set inherit bit
  virtual void	SetUnique(char* memb_nm, bool onoff); // set inherit bit
  virtual bool	GetUnique(int memb_no);	     	// check inherit bit
  virtual bool	GetUnique(char* memb_nm);	// check inherit bit

  virtual void	UpdateMember(BaseSpec* from, int memb_no);
  // copy member from given parent
  virtual void	UpdateSpec();
  // update values from parent, and update children
  virtual void	UpdateChildren();	// update any children
  virtual void	UpdateSubSpecs() { };	// update any subspec objects (overload me)

  virtual BaseSpec* NewChild(); 		// create a new childspec

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

  // save the unique members
  int 		Dump_Save_Value(ostream& strm, TAPtr par=NULL, int indent = 0);
  int		Dump_Load_Value(istream& strm, TAPtr par=NULL);


  void	UpdateAfterEdit();
  void 	Initialize();
  void	Destroy();
  void	InitLinks();
  void	CutLinks();
  void 	Copy_(const BaseSpec& cp);
  COPY_FUNS(BaseSpec, taNBase);
  TA_BASEFUNS(BaseSpec);
};

class BaseSubSpec : public taNBase {
  // ##EXT_spec ##MEMB_IN_GPMENU specification class for sub-objects of specs
public:
  unsigned long unique[MAX_SPEC_LONGS]; // #HIDDEN bits representing members unique

  virtual BaseSubSpec*	FindParent();
  // #MENU #USE_RVAL #MENU_ON_Actions Find the corresponding parent subspec of this one
  virtual BaseSpec*	FindParentBaseSpec();
  // #MENU #USE_RVAL Find the parent spec of this one

  virtual void	SetUnique(int memb_no, bool onoff); // set inherit bit
  virtual void	SetUnique(char* memb_nm, bool onoff); // set inherit bit
  virtual bool	GetUnique(int memb_no);	     	// check inherit bit
  virtual bool	GetUnique(char* memb_nm);	// check inherit bit

  virtual void	UpdateMember(BaseSubSpec* from, int memb_no);
  // copy member from given sub spec if not unique
  virtual void	UpdateSpec();
  // update from parent sub spec, if one exists

  // save the unique members
  int 		Dump_Save_Value(ostream& strm, TAPtr par=NULL, int indent = 0);
  int		Dump_Load_Value(istream& strm, TAPtr par=NULL);

  void	UpdateAfterEdit();
  void 	Initialize();
  void	Destroy();
  void	InitLinks();
  void 	Copy_(const BaseSubSpec& cp);
  COPY_FUNS(BaseSubSpec, taNBase);
  TA_BASEFUNS(BaseSubSpec);
};

class SpecPtr_impl : public taBase {
  // ##INLINE ##NO_TOKENS ##NO_UPDATE_AFTER magic pointer to a spec
public:
  TAPtr		owner;		// #NO_SAVE #READ_ONLY to get to proj..
  TypeDef*	base_type;	// #TYPE_BaseSpec #HIDDEN #NO_SAVE base type for type field
  TypeDef*	type;		// #TYPE_ON_base_type The type of the spec to use

  virtual BaseSpec* GetSpec() const	{ return NULL; } // get the spec pointer
  virtual void	SetSpec(BaseSpec*)	{ };	 	// set the spec pointer

  BaseSpec* 	operator=(BaseSpec* cp)	{ SetSpec(cp); return cp; }

  virtual void	SetDefaultSpec(TAPtr ownr, TypeDef* td); // for class that owns ptr
  virtual void	SetBaseType(TypeDef* td);		 // for overloaded classes
  virtual BaseSpec_MGroup* GetSpecGroup();		 // get the group where specs go
  virtual void	GetSpecOfType();			 // get a spec of type type

  void	UpdateAfterEdit();	// check, update the spec type
  void	Initialize();
  void 	Destroy()		{ };
  void	Copy_(const SpecPtr_impl& cp);
  COPY_FUNS(SpecPtr_impl, taBase);
  TA_BASEFUNS(SpecPtr_impl);
};


template<class T> class SpecPtr : public SpecPtr_impl {
public:
  T*		spec;		// the actual spec itself

  BaseSpec*	GetSpec() const		{ return spec; }
  void		SetSpec(BaseSpec* es)   {
    if((es == NULL) || (es->InheritsFrom(base_type) && es->CheckObjectType(owner))) {
      taBase::SetPointer((TAPtr*)&spec,es);
      if(es != NULL) type = es->GetTypeDef();
    }
    else {
      taMisc::Error("SetSpec: incorrect type of Spec:",
		    es->GetPath(), "of type:", es->GetTypeDef()->name,
		    "should be at least:", base_type->name,"in object:",owner->GetPath());
    }
  }
  bool		CheckSpec(TAPtr own_obj)   {
    if(own_obj == NULL) return false;
    if(spec == NULL) {
      taMisc::Error("CheckSpec: spec is NULL in", own_obj->GetPath());
      return false;
    }
    if(!spec->InheritsFrom(base_type)) {
      taMisc::Error("CheckSpec: incorrect type of spec:", spec->GetPath(),
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

  T* 		operator->() const	{ return spec; }
  T* 		operator=(T* cp)	{ SetSpec(cp); return cp; }
  bool 		operator!=(T* cp) const	{ return (spec != cp); }
  bool 		operator==(T* cp) const	{ return (spec == cp); }

  operator T*()	const		{ return spec; }
  operator BaseSpec*() const	{ return spec; }

  void 	Initialize()		{ spec = NULL; }
  void	Destroy()		{ CutLinks(); }
  void  CutLinks()
  { SpecPtr_impl::CutLinks(); taBase::DelPointer((TAPtr*)&spec); }
  void	Copy_(const SpecPtr<T>& cp)
  { taBase::SetPointer((TAPtr*)&spec, cp.spec); UpdateAfterEdit(); }
  COPY_FUNS(SpecPtr<T>, SpecPtr_impl);
  TA_TMPLT_BASEFUNS(SpecPtr, T);
};


#define SpecPtr_of(T)							      \
class T ## _SPtr : public SpecPtr<T> {					      \
public:									      \
  void 	Initialize() 		{ };					      \
  void	Destroy()		{ };					      \
  TA_BASEFUNS(T ## _SPtr);						      \
}



#endif // spec_h



