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


#ifndef ta_group_h
#define ta_group_h 1

#include "ta_base.h"

#ifndef __MAKETA__
#include <sstream>
#endif

class 	TA_API taGroup_impl;
typedef taGroup_impl* 	TAGPtr;

typedef taList<taGroup_impl> TALOG; // list of groups (LOG)

class 	TA_API taSubGroup : public TALOG {
  // #INSTANCE ##NO_TOKENS ##NO_UPDATE_AFTER has the sub-groups for a group
public:
  override  void DataChanged(int dcr, void* op1 = NULL, void* op2 = NULL); // forward LIST events as GROUP events to owner

  bool	Transfer(taBase* item);

  void	Initialize()	{ };
  void	Destroy()	{ };
  TA_BASEFUNS(taSubGroup);
};

typedef int taGroupItr;

class	TA_API taLeafItr {		// contains the indicies for iterating over leafs
public:
  TAGPtr	cgp;		// pointer to current group
  int		g;		// index of current group
  int		i;		// index of current leaf element
};

#define FOR_ITR_EL(T, el, grp, itr) \
for(el = (T*) grp FirstEl(itr); el; el = (T*) grp NextEl(itr))

#define FOR_ITR_EL_REV(T, el, grp, itr) \
for(el = (T*) grp LastEl(itr); el; el = (T*) grp PrevEl(itr))

#define FOR_ITR_GP(T, el, grp, itr) \
for(el = (T*) grp FirstGp(itr); el; el = (T*) grp NextGp(itr))


class TA_API taGroup_impl : public taList_impl {
  // #INSTANCE #NO_UPDATE_AFTER implementation of a group
#ifndef __MAKETA__
typedef taList_impl inherited;
#endif
protected:
  virtual String	GetValStr(const TypeDef* td, void* par=NULL,
	MemberDef* memb_def = NULL) const;
  virtual TAGPtr LeafGp_(int leaf_idx) const; // #IGNORE the leaf group containing leaf item -- **NONSTANDARD FUNCTION** put here to try to flush out any use
#ifdef TA_GUI
protected:
  mutable TALOG*	leaf_gp; 	// #READ_ONLY #NO_SAVE cached 'flat' list of leaf-containing-gps for iter
  override void	ChildQueryEditActions_impl(const MemberDef* md, const taBase* child, const taiMimeSource* ms,
    int& allowed, int& forbidden);
  virtual void	ChildQueryEditActionsG_impl(const MemberDef* md, int subgrp_idx, taGroup_impl* subgrp, const taiMimeSource* ms,
    int& allowed, int& forbidden);
  override int	ChildEditAction_impl(const MemberDef* md, taBase* child, taiMimeSource* ms, int ea);
    // if child or ms is a group, dispatch to new G version
  virtual int	ChildEditActionGS_impl(const MemberDef* md, int subgrp_idx, taGroup_impl* subgrp, int ea);
  virtual int	ChildEditActionGD_impl_inproc(const MemberDef* md, int subgrp_idx, taGroup_impl* subgrp, taiMimeSource* ms, int ea);
  virtual int	ChildEditActionGD_impl_ext(const MemberDef* md, int subgrp_idx, taGroup_impl* subgrp, taiMimeSource* ms, int ea);
#endif
public:
  virtual TAGPtr GetSuperGp_();			// #IGNORE Parent super-group, or NULL
  virtual void	 UpdateLeafCount_(int no); 	// #IGNORE updates the leaves count

public:
  int 		leaves;		// #READ_ONLY #NO_SAVE total number of leaves
  taSubGroup	gp; 		// #HIDDEN #HIDDEN_TREE #NO_FIND #NO_SAVE sub-groups within this one
  TAGPtr	super_gp;	// #READ_ONLY #NO_SAVE #HIDDEN_TREE super-group above this
  TAGPtr	root_gp; 	// #READ_ONLY #NO_SAVE #HIDDEN_TREE the root group, 'this' for root group itself; never NULL

  bool		IsEmpty() const	{ return (leaves == 0) ? true : false; }
  bool		IsRoot() const	{ return (root_gp == this); } // 'true' if this is the root
  override void	DataChanged(int dcr, void* op1 = NULL, void* op2 = NULL);

  taBase* 	New(int n_objs=1, TypeDef* typ = NULL);

  MemberDef* 	FindMembeR(const String& nm, void*& ptr) const;
  MemberDef* 	FindMembeR(TypeDef* it, void*& ptr) const;

  // IO routines
  ostream& 	OutputR(ostream& strm, int indent = 0) const;

  override int	Dump_SaveR(ostream& strm, TAPtr par=NULL, int indent=0);
  override int	Dump_Save_PathR(ostream& strm, TAPtr par=NULL, int indent=0);
  override int	Dump_Save_PathR_impl(ostream& strm, TAPtr par=NULL, int indent=0);

  override int	UpdatePointers_NewPar(taBase* old_par, taBase* new_par);
  override int	UpdatePointers_NewParType(TypeDef* par_typ, taBase* new_par);
  override int	UpdatePointers_NewObj(taBase* old_ptr, taBase* new_ptr);
  override int 	UpdatePointersToMyKids_impl(taBase* scope_obj, taBase* new_ptr);

  ////////////////////////////////////////////////
  // 	functions that return the type		//
  ////////////////////////////////////////////////

  TAGPtr	Gp_(int i) const	{ return gp.SafeEl(i); } // #IGNORE
  TAGPtr 	FastGp_(int i) const	{ return gp.FastEl(i); } // #IGNORE
  virtual TAPtr	Leaf_(int idx) const;	// #IGNORE DFS through all subroups for leaf i
  TAGPtr 	FastLeafGp_(int gp_idx) const // #IGNORE the flat leaf group, note: 0 is "this"
    { if (gp_idx == 0) return const_cast<TAGPtr>(this); if (!leaf_gp) InitLeafGp();
      return (TAGPtr)leaf_gp->el[gp_idx];}
  TAGPtr 	SafeLeafGp_(int gp_idx) const; // #IGNORE the flat leaf group, note: 0 is "this"
  
  // iterator-like functions
  TAGPtr 	FirstGp_(int& g) const	// #IGNORE first sub-gp
  { g = 0; if(leaf_gp == NULL) InitLeafGp(); return leaf_gp->SafeEl(0); }
  TAGPtr 	LastGp_(int& g) const	// #IGNORE last sub-gp (for rev iter)
  {if(leaf_gp == NULL) InitLeafGp(); g = leaf_gp->size - 1; return leaf_gp->Peek(); }
  TAGPtr 	NextGp_(int& g)	const	// #IGNORE next sub-gp
  { return leaf_gp->SafeEl(++g); }
  int	 	LeafGpCount()	const	// #IGNORE count of leaf groups **including self**; optimized for no subgroups
    { if (gp.size == 0) return 1; if (leaf_gp == NULL) InitLeafGp(); return leaf_gp->size; }

  TAPtr	 	FirstEl_(taLeafItr& lf) const	// #IGNORE first leaf iter init
  { TAPtr rval=NULL; lf.i = 0; lf.cgp = FirstGp_(lf.g);
    if(lf.cgp != NULL) rval=(TAPtr)lf.cgp->el[0]; return rval; }
  TAPtr	 	NextEl_(taLeafItr& lf)	const	// #IGNORE next leaf
  { if (++lf.i >= lf.cgp->size) {
    lf.i = 0; if (!(lf.cgp = leaf_gp->SafeEl(++lf.g))) return NULL; }
    return (TAPtr)lf.cgp->el[lf.i];}

  TAPtr	 	LastEl_(taLeafItr& lf) const	// #IGNORE last leaf iter init
  { if (!(lf.cgp = LastGp_(lf.g))) return NULL;
    lf.i = lf.cgp->size - 1; return (TAPtr)lf.cgp->el[lf.i];  }
  TAPtr	 	PrevEl_(taLeafItr& lf)	const	// #IGNORE prev leaf
  { if (--lf.i < 0) {
      if (!(lf.cgp = leaf_gp->SafeEl(--lf.g))) return NULL; 
      lf.i = lf.cgp->size - 1;}
    return (TAPtr)lf.cgp->el[lf.i];}

  virtual TAGPtr NewGp_(int no, TypeDef* typ=NULL);	// #IGNORE create sub groups
  virtual TAPtr	 NewEl_(int no, TypeDef* typ=NULL);	// #IGNORE create items

  virtual TAPtr FindLeafName_(const char* it, int& idx=Idx) const; 	// #IGNORE
  virtual TAPtr	FindLeafType_(TypeDef* it, int& idx=Idx) const;	// #IGNORE

  virtual TAGPtr FindMakeGpName(const String& gp_nm, TypeDef* typ=NULL);
  // #IGNORE find subgroup of given name; if it doesn't exist, then make it (using type if specified, else default type for subgroup)

  ////////////////////////////////////////////////
  // functions that don't depend on the type	//
  ////////////////////////////////////////////////

  virtual void	InitLeafGp() const;
  // #CAT_Access Initialize the leaf group iter list, always ok to call
  virtual void	InitLeafGp_impl(TALOG* lg) const; // #IGNORE impl of init leaf gp
  virtual void	AddOnly_(void* it); 		// #IGNORE update leaf count
//  virtual bool	Remove(const char* item_nm)	{ return taList_impl::Remove(item_nm); }
//  virtual bool	Remove(TAPtr item)		{ return taList_impl::Remove(item); }

  virtual bool 	RemoveLeafEl(TAPtr item);
  // #CAT_Modify remove given leaf element
  virtual bool	RemoveLeafName(const char* item_nm);
  // #CAT_Modify remove given named leaf element
  virtual bool  RemoveLeafIdx(int idx);
  // #CAT_Modify Remove leaf element at leaf index
  virtual void 	RemoveAll();
  // #CAT_Modify Remove all elements of the group

  virtual bool	RemoveGpIdx(int idx) 			{ return gp.RemoveIdx(idx); }
  // #CAT_Modify remove group at given index
  virtual bool	RemoveGpEl(TAGPtr group)		{ return gp.RemoveEl(group); }
  // #MENU #FROM_GROUP_gp #MENU_ON_Edit #CAT_Modify remove given group
  virtual TALOG* EditSubGps() 				{ return &gp; }
  // #MENU #USE_RVAL #CAT_Access edit the list of sub-groups (e.g., so you can move around subgroups)

  virtual void	EnforceLeaves(int sz);
  // #CAT_Modify ensure that sz leaves exits by adding new ones to top group and removing old ones from end
  void	EnforceSameStru(const taGroup_impl& cp);
  // #CAT_Modify enforce this group to have same structure as cp

  int	ReplaceType(TypeDef* old_type, TypeDef* new_type);

  virtual int	FindLeafEl(TAPtr item) const;  // find given leaf element (-1 = not here)
  // #CAT_Access find given leaf element -1 = not here.

  void	Duplicate(const taGroup_impl& cp);
  void	DupeUniqNameOld(const taGroup_impl& cp);
  void	DupeUniqNameNew(const taGroup_impl& cp);

  void	Borrow(const taGroup_impl& cp);
  void	BorrowUnique(const taGroup_impl& cp);
  void	BorrowUniqNameOld(const taGroup_impl& cp);
  void	BorrowUniqNameNew(const taGroup_impl& cp);

  void	Copy_Common(const taGroup_impl& cp);
  void	Copy_Duplicate(const taGroup_impl& cp);
  void	Copy_Borrow(const taGroup_impl& cp);

  virtual void 	List(ostream& strm=cout) const; // Display list of elements in the group

  void 	Initialize();
  void 	InitLinks();		// inherit the el_typ from parent group..
  void 	CutLinks();
  void  Destroy();
  void 	Copy(const taGroup_impl& cp);
  TA_BASEFUNS(taGroup_impl);

protected:
  override void 	CheckChildConfig_impl(bool quiet, bool& rval);
  override void		ItemRemoved_(); // update the leaf counts (supercursively)
};

template<class T> class taGroup : public taGroup_impl {
  // #INSTANCE #NO_UPDATE_AFTER
public:
  ////////////////////////////////////////////////
  // 	functions that return the type		//
  ////////////////////////////////////////////////

  // operators
  T*		SafeEl(int idx) const		{ return (T*)SafeEl_(idx); }
  // #CAT_Access get element at index
  T*		FastEl(int i) const		{ return (T*)el[i]; }
  // #CAT_Access fast element (no checking)
  T* 		operator[](int i) const		{ return (T*)el[i]; }

  T*		DefaultEl() const		{ return (T*)DefaultEl_(); }
  // #CAT_Access returns the element specified as the default for this group

  // note that the following is just to get this on the menu (it doesn't actually edit)
  T*		Edit_El(T* item) const		{ return SafeEl(FindEl((TAPtr)item)); }
  // #MENU #MENU_ON_Edit #USE_RVAL #ARG_ON_OBJ #CAT_Access Edit given group item

  taGroup<T>*	SafeGp(int idx) const		{ return (taGroup<T>*)gp.SafeEl(idx); }
  // #CAT_Access get group at index
  taGroup<T>*	FastGp(int i) const		{ return (taGroup<T>*)gp.FastEl(i); }
  // #CAT_Access the sub group at index
  taGroup<T>* 	FastLeafGp(int gp_idx) const	{ return (taGroup<T>*)FastLeafGp_(gp_idx); } 
  // #CAT_Access the leaf sub group at index, note: 0 is always "this"
  taGroup<T>* 	SafeLeafGp(int gp_idx) const	{ return (taGroup<T>*)SafeLeafGp_(gp_idx); } 
  // #CAT_Access the leaf sub group at index, note: 0 is always "this"

  T*		Leaf(int idx) const		{ return (T*)Leaf_(idx); }
  // #CAT_Access get leaf element at index
  taGroup<T>* 	RootGp() const 			{ return (taGroup<T>*)root_gp;  }
  // #CAT_Access the root group ('this' for the root group)

  // iterator-like functions
  inline T*	FirstEl(taLeafItr& lf) const	{ return (T*)FirstEl_(lf); }
  // #CAT_Access returns first leaf element and inits indexes
  inline T*	NextEl(taLeafItr& lf) const 	{ return (T*)NextEl_(lf); }
  // #CAT_Access returns next leaf element and incs indexes
  inline T*	LastEl(taLeafItr& lf) const	{ return (T*)LastEl_(lf); }
  // #CAT_Access returns first leaf element and inits indexes
  inline T*	PrevEl(taLeafItr& lf) const 	{ return (T*)PrevEl_(lf); }
  // #CAT_Access returns next leaf element and incs indexes


  taGroup<T>*	FirstGp(int& g)	const		{ return (taGroup<T>*)FirstGp_(g); }
  // #CAT_Access returns first leaf group and inits index
  taGroup<T>*	NextGp(int& g) const		{ return (taGroup<T>*)NextGp_(g); }
  // #CAT_Access returns next leaf group and incs index

  virtual T* 	NewEl(int n_els=1, TypeDef* typ=NULL) { return (T*)NewEl_(n_els, typ);}
  // #CAT_Modify Create and add n_els new element(s) of given type to the group (NULL = default type, el_typ)
  virtual taGroup<T>* NewGp(int n_gps=1, TypeDef* typ=NULL) { return (taGroup<T>*)NewGp_(n_gps, typ);}
  // #CAT_Modify #MENU #MENU_CONTEXT #MENU_ON_Edit #TYPE_this Create and add n_gps new sub group(s) of given type (NULL = same type as this group)

  virtual T*	FindName(const char* item_nm, int& idx=Idx)  const { return (T*)FindName_(item_nm, idx); }
  // #CAT_Access Find element with given name (nm) (NULL = not here), sets idx
  virtual T* 	FindType(TypeDef* item_tp, int& idx=Idx) const { return (T*)FindType_(item_tp, idx); }
  // #CAT_Access find given type element (NULL = not here), sets idx

  virtual T*	Pop()				{ return (T*)Pop_(); }
  // #CAT_Modify pop the last element off the stack
  virtual T*	Peek()				{ return (T*)Peek_(); }
  // #CAT_Access peek at the last element on the stack

  virtual T*	AddUniqNameOld(T* item)		{ return (T*)AddUniqNameOld_((void*)item); }
  // #CAT_Modify add so that name is unique, old used if dupl, returns one used
  virtual T*	LinkUniqNameOld(T* item)	{ return (T*)LinkUniqNameOld_((void*)item); }
  // #CAT_Modify link so that name is unique, old used if dupl, returns one used

  virtual bool	MoveBefore(T* trg, T* item) { return MoveBefore_((void*)trg, (void*)item); }
  // #CAT_Modify move item so that it appears just before the target item trg in the list
  virtual bool	MoveAfter(T* trg, T* item) { return MoveAfter_((void*)trg, (void*)item); }
  // #CAT_Modify move item so that it appears just after the target item trg in the list

  virtual T* 	FindLeafName(const char* item_nm, int& idx=Idx) const { return (T*)FindLeafName_(item_nm, idx); }
  // #MENU #MENU_ON_Edit #USE_RVAL #ARGC_1 #LABEL_Find #CAT_Access Find element with given name (el_nm)
  virtual T* 	FindLeafType(TypeDef* item_tp, int& idx=Idx) const { return (T*)FindLeafType_(item_tp, idx);}
  // #CAT_Access find given type leaf element (NULL = not here), sets idx

  void Initialize() 			{ SetBaseType(T::StatTypeDef(1));}

  taGroup() 				{ Register(); Initialize(); }
  taGroup(const taGroup<T>& cp)		{ Register(); Initialize(); Copy(cp); }
  ~taGroup() 				{ unRegister(); Destroy(); }
  TAPtr Clone() 			{ return new taGroup<T>(*this); }
  void  UnSafeCopy(TAPtr cp) 		{ if(cp->InheritsFrom(GetTypeDef())) Copy(*((taGroup<T>*)cp));
                                          else if(InheritsFrom(cp->GetTypeDef())) cp->CastCopyTo(this); }
  void  CastCopyTo(TAPtr cp)            { taGroup<T>& rf = *((taGroup<T>*)cp); rf.Copy(*this); }
  TAPtr MakeToken()			{ return (TAPtr)(new taGroup<T>); }
  TAPtr MakeTokenAry(int no)		{ return (TAPtr)(new taGroup<T>[no]); }
  void operator=(const taGroup<T>& cp)	{ Copy(cp); }
  TA_TMPLT_TYPEFUNS(taGroup,T);
protected:
  taGroup<T>* 	LeafGp(int leaf_idx) const		{ return (taGroup<T>*)LeafGp_(leaf_idx); }
  // the group containing given leaf; NOTE: **don't confuse this with the Safe/FastLeafGp funcs*** -- moved here to try to flush out any use, since it is so confusing and nonstandard and likely to be mixed up with the XxxLeafGp funcs 
};


// do not use this macro, since you typically will want ##NO_TOKENS, #NO_UPDATE_AFTER
// which cannot be put inside the macro!
//
// #define taGroup_of(T)
// class T ## _Group : public taGroup<T> {
// public:
//   void Initialize()	{ };
//   void Destroy()	{ };
//   TA_BASEFUNS(T ## _Group);
// }

// use the following as a template instead..

// define default base group to not keep tokens
class TA_API taBase_Group : public taGroup<taBase> {
  // ##NO_TOKENS ##NO_UPDATE_AFTER group of objects
public:
  void	Initialize() 		{ SetBaseType(&TA_taBase); }
  void 	Destroy()		{ };
  TA_BASEFUNS(taBase_Group);
};

#define BaseGroup_of(T)							      \
class T ## _Group : public taBase_Group {				      \
public:									      \
  void	Initialize() 		{ SetBaseType(T::StatTypeDef(0)); }		      \
  void 	Destroy()		{ };					      \
  TA_BASEFUNS(T ## _Group);					      \
}


#endif // ta_group_h
