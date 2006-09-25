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

class 	TA_API taGroup_impl;		// pre-declare
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
  // #INSTANCE #NO_TOKENS #NO_UPDATE_AFTER implementation of a group
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

  TAPtr 	New(int no=0, TypeDef* typ = NULL);

  MemberDef* 	FindMembeR(const String& nm, void*& ptr) const;    // extended to search in the group
  MemberDef* 	FindMembeR(TypeDef* it, void*& ptr) const; // extended to search in the group

  // IO routines
  ostream& 	OutputR(ostream& strm, int indent = 0) const;

  override int	Dump_SaveR(ostream& strm, TAPtr par=NULL, int indent=0);
  override int	Dump_Save_PathR(ostream& strm, TAPtr par=NULL, int indent=0);
  override int	Dump_Save_PathR_impl(ostream& strm, TAPtr par=NULL, int indent=0);

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
  virtual TAPtr	 NewEl_(int no, TypeDef* typ=NULL);	// #IGNORE create sub groups

  virtual TAPtr FindLeafName_(const char* it, int& idx=Idx) const; 	// #IGNORE
  virtual TAPtr	FindLeafType_(TypeDef* it, int& idx=Idx) const;	// #IGNORE


  ////////////////////////////////////////////////
  // functions that don't depend on the type	//
  ////////////////////////////////////////////////

  virtual void	InitLeafGp() const;		// Initialize the leaf group iter list, always ok to call
  virtual void	InitLeafGp_impl(TALOG* lg) const; // #IGNORE impl of init leaf gp
  virtual void	AddEl_(void* it); 		// #IGNORE update leaf count
//  virtual bool	Remove(const char* item_nm)	{ return taList_impl::Remove(item_nm); }
//  virtual bool	Remove(TAPtr item)		{ return taList_impl::Remove(item); }

  virtual bool 	RemoveLeaf(TAPtr item); 	// remove given leaf element
  virtual bool	RemoveLeaf(const char* item_nm);
  virtual bool  RemoveLeaf(int idx);
  // Remove leaf element at leaf index
  virtual bool	RemoveLeafName(const char* item_nm)	{ return RemoveLeaf(item_nm); }
  // remove given named leaf element
  virtual bool	RemoveLeafEl(TAPtr item)		{ return RemoveLeaf(item); }
  // Remove given leaf element
  virtual void 	RemoveAll();
  // Remove all elements of the group

  virtual bool	RemoveGp(int idx) 			{ return gp.Remove(idx); }
  // remove group at given index
  virtual bool	RemoveGp(TAGPtr group)			{ return gp.Remove(group); }
  // #MENU #FROM_GROUP_gp #MENU_ON_Edit #UPDATE_MENUS remove given group
  virtual TALOG* EditSubGps() 				{ return &gp; }
  // #MENU #USE_RVAL edit the list of sub-groups (e.g., so you can move around subgroups)

  virtual void	EnforceLeaves(int sz);
  // ensure that sz leaves exits by adding new ones to top group and removing old ones from end
  void	EnforceSameStru(const taGroup_impl& cp);

  int	ReplaceType(TypeDef* old_type, TypeDef* new_type);
  int	ReplaceAllPtrsThis(TypeDef* obj_typ, void* old_ptr, void* new_ptr);

  virtual int	FindLeaf(TAPtr item) const;  // find given leaf element (-1 = not here)
  virtual int	FindLeaf(TypeDef* item) const;
  virtual int	FindLeaf(const char* item_nm) const;
  // find named leaf element
  virtual int	FindLeafEl(TAPtr item) const	{ return FindLeaf(item); }
  // find given leaf element -1 = not here.

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
  override void		ItemRemoved_(); // update the leaf counts (supercursively)
};

#ifdef DMEM_COMPILE

#include <mpi.h>

// add the following code into any object that is going to be shared across processors
// #ifdef DMEM_COMPILE
//   int 		dmem_local_proc; // #IGNORE processor on which these units are local
//   static int	dmem_this_proc;	// #IGNORE processor rank for this processor RELATIVE TO COMMUNICATOR for the network
//   virtual bool 	DMem_IsLocalProc(int proc)   	{ return dmem_local_proc == proc; } // #IGNORE
//   virtual bool 	DMem_IsLocal()       		{ return dmem_local_proc == dmem_proc; }  // #IGNORE
//   virtual int 	DMem_GetLocalProc() 		{ return dmem_local_proc; } // #IGNORE
//   virtual void 	DMem_SetLocalProc(int lproc) 	{ dmem_local_proc = lproc; } // #IGNORE
//   virtual void 	DMem_SetThisProc(int proc) 	{ dmem_this_proc = proc; } // #IGNORE
// #endif

#define DMEM_MPICALL(mpicmd, fun, mpi_call) \
  DMemShare::DebugCmd(fun, mpi_call); \
  DMemShare::ProcErr(mpicmd, fun, mpi_call)

// use the following to conditionalize running of functions directly as opposed
// to having them be called later via the cmdstream script calls
// it is necessary to do this to sychronize all dmem procs so they all call the
// exact same function scripts at exactly the same time!
#define DMEM_GUI_RUN_IF if(taMisc::dmem_nprocs == 1)

class TA_API DMemShareVar : public taBase {
  // ##NO_TOKENS ##NO_CSS ##NO_MEMBERS definition of a variable of a given type (FLOAT, DOUBLE, INT) that each proc has some instances of (can be multiple members of a given object) -- these can all be Allgather'ed to sync across procs
public:
  MPI_Comm	comm;		// #IGNORE communicator associated with these shared objs
  int		mpi_type;	// #IGNORE mpi's type for this variable
  int		max_per_proc;	// #IGNORE maximum number of vars per any one proc
  int		n_procs;	// #IGNORE number of processors in this communicator (set during Compile_Var)
  int		this_proc;	// #IGNORE proc id (rank) of this processor in communicator (set during Compile_Var)

  // the following two must be initialized for all the data to be shared prior to calling Compile_Var
  voidptr_Array	addrs;		// #IGNORE addresses for each item to be shared (one for each individual data item)
  int_Array 	local_proc; 	// #IGNORE which proc each guy is local to (one for each individual data item)

  int_Array	n_local; 	// #IGNORE number of local variables for each process (size nprocs)
  int_Array	recv_idx;	// #IGNORE starting indicies into addrs_recv list for each proc (size nprocs)
  voidptr_Array addrs_recv; 	// #IGNORE addresses in recv format (size nprocs * max_per_proc; 000..111..222...)

  // the following are contiguous data arrays for sending and gathering from all other procs
  // (size for both = n_procs * max_per_proc; send only needs max_per_proc for allgather, but needs all for allreduce)
  float_Array	float_send; 	// #IGNORE
  float_Array	float_recv; 	// #IGNORE
  double_Array	double_send; 	// #IGNORE
  double_Array	double_recv; 	// #IGNORE
  int_Array	int_send; 	// #IGNORE
  int_Array	int_recv; 	// #IGNORE
  long_Array	long_send; 	// #IGNORE
  long_Array	long_recv; 	// #IGNORE

  virtual void	Compile_Var(MPI_Comm cm); // #IGNORE call this after updating the variable info
  virtual void 	SyncVar();	// #IGNORE synchronize variable across procs (allgather, so all procs have all data)
  virtual void 	AggVar(MPI_Op op); // #IGNORE aggregate variable across procs (allreduce: each sends and recvs all data using op to merge w/ existing vals)

  virtual void 	ResetVar();	// #IGNORE reset variable info

  void 	Initialize();
  void 	Destroy()	{ CutLinks(); }
  void	InitLinks();
  void	CutLinks();
  void	Copy_(const DMemShareVar& cp);
  COPY_FUNS(DMemShareVar, taBase);
  TA_BASEFUNS(DMemShareVar);
};


class TA_API DMemShare : public taBase_List {
  // ##NO_TOKENS ##NO_CSS ##NO_MEMBERS collection of objects that are shared across distributed processes: link the objects into this list to share them
public:
  taBase_List	vars;		// #IGNORE list of DMemShareVar variables, one for each share_set (set of variables to be shared at the same time)
  // NOTE: a share_set must all have the same variable type (e.g., all must be FLOAT or DOUBLE, etc)!

  MPI_Comm	comm;		// #IGNORE communicator associated with these shared objs

#ifndef __MAKETA__
  static stringstream*	cmdstream;	// #IGNORE command stream: communicating commands across dmem procs
#endif

  static void 	InitCmdStream();	// #IGNORE initialize command stream
  static void 	CloseCmdStream(); 	// #IGNORE close command stream

  static void	DebugCmd(const char* fun, const char* mpi_call);
  // #IGNORE provide debugging trace at start of mpi command call
  static bool	ProcErr(int ercd, const char* fun, const char* mpi_call);
  // #IGNORE process any errors from command, and provide done message if debugging

  virtual void 	SetLocal_Sequential();	// #IGNORE set local processor on shared objects in sequence: 0 1 2..n 0 1 2..

  virtual void 	Compile_ShareVar(TypeDef* td, taBase* shr_item, MemberDef* par_md=NULL);
  // #IGNORE compile current set of objects and type info into set of types used in share/aggregate calls (MPI types)
  virtual void 	Compile_ShareTypes();   // #IGNORE compile current set of objects and type info into set of types used in share/aggregate calls (MPI types)

  virtual void 	DistributeItems(); // #IGNORE distribute the items across the nodes: calls above two functions

  virtual void 	Sync(int share_set);
  // #IGNORE synchronize across all processors for specific set of shared variables
  virtual void 	Aggregate(int share_set, MPI_Op op);
  // #IGNORE aggregate across all processors for specific set of shared variables: this only works for one floating point variable per object

  static void 	ExtractLocalFromList(taPtrList_impl& global_list, taPtrList_impl& local_list);
  // #IGNORE

  void 	Initialize();
  void 	Destroy()	{ CutLinks(); }
  void	InitLinks();
  void	CutLinks();
  void	Copy_(const DMemShare& cp);
  COPY_FUNS(DMemShare, taBase_List);
  TA_BASEFUNS(DMemShare);
};

#else

#define DMEM_GUI_RUN_IF

// dummy version to keep the _TA files the same..

class TA_API DMemShareVar : public taBase {
  // ##NO_TOKENS ##NO_CSS ##NO_MEMBERS definition of a variable of a given type (FLOAT, DOUBLE, INT) that each proc has some instances of (can be multiple members of a given object) -- these can all be Allgather'ed to sync across procs
public:

  void	Dummy(const DMemShareVar&) { };
  void 	Initialize() { };
  void 	Destroy()	{ };
  TA_BASEFUNS(DMemShareVar);
};

class TA_API DMemShare : public taBase_List {
  // ##NO_TOKENS ##NO_CSS ##NO_MEMBERS collection of objects that are shared across distributed processes: link the objects into this list to share them
public:
  static void 	ExtractLocalFromList(taPtrList_impl&, taPtrList_impl&) { };
  // #IGNORE

  void	Dummy(const DMemShare&) { };
  void 	Initialize() 	{ };
  void 	Destroy()	{ };
  TA_BASEFUNS(DMemShare);
};

#endif

template<class T> class taGroup : public taGroup_impl {
  // #NO_TOKENS #INSTANCE #NO_UPDATE_AFTER
public:
  ////////////////////////////////////////////////
  // 	functions that return the type		//
  ////////////////////////////////////////////////

  // operators
  T*		SafeEl(int idx) const		{ return (T*)SafeEl_(idx); }
  // get element at index
  T*		FastEl(int i) const		{ return (T*)el[i]; }
  // fast element (no checking)
  T* 		operator[](int i) const		{ return (T*)el[i]; }

  T*		DefaultEl() const		{ return (T*)DefaultEl_(); }
  // returns the element specified as the default for this group

  // note that the following is just to get this on the menu (it doesn't actually edit)
  T*		Edit_El(T* item) const		{ return SafeEl(Find((TAPtr)item)); }
  // #MENU #MENU_ON_Edit #USE_RVAL #ARG_ON_OBJ Edit given group item

  taGroup<T>*	SafeGp(int idx) const		{ return (taGroup<T>*)gp.SafeEl(idx); }
  // get group at index
  taGroup<T>*	FastGp(int i) const		{ return (taGroup<T>*)gp.FastEl(i); }
  // the sub group at index
  taGroup<T>* 	FastLeafGp(int gp_idx) const	{ return (taGroup<T>*)FastLeafGp_(gp_idx); } 
  // the leaf sub group at index, note: 0 is always "this"
  taGroup<T>* 	SafeLeafGp(int gp_idx) const	{ return (taGroup<T>*)SafeLeafGp_(gp_idx); } 
  // the leaf sub group at index, note: 0 is always "this"

  T*		Leaf(int idx) const		{ return (T*)Leaf_(idx); }
  // get leaf element at index
  taGroup<T>* 	RootGp() const 			{ return (taGroup<T>*)root_gp;  }
    // the root group ('this' for the root group)

  // iterator-like functions
  inline T*	FirstEl(taLeafItr& lf) const	{ return (T*)FirstEl_(lf); }
  // returns first leaf element and inits indexes
  inline T*	NextEl(taLeafItr& lf) const 	{ return (T*)NextEl_(lf); }
  // returns next leaf element and incs indexes
  inline T*	LastEl(taLeafItr& lf) const	{ return (T*)LastEl_(lf); }
  // returns first leaf element and inits indexes
  inline T*	PrevEl(taLeafItr& lf) const 	{ return (T*)PrevEl_(lf); }
  // returns next leaf element and incs indexes


  taGroup<T>*	FirstGp(int& g)	const		{ return (taGroup<T>*)FirstGp_(g); }
  // returns first leaf group and inits index
  taGroup<T>*	NextGp(int& g) const		{ return (taGroup<T>*)NextGp_(g); }
  // returns next leaf group and incs index

  virtual T* 	NewEl(int n_els=0, TypeDef* typ=NULL) { return (T*)NewEl_(n_els, typ);}
  // Create and add (n_els) new element(s) of given type
  virtual taGroup<T>* NewGp(int n_gps=0, TypeDef* typ=NULL) { return (taGroup<T>*)NewGp_(n_gps, typ);}
  // Create and add (n_gps) new group(s) of given type

  virtual T*	FindName(const char* item_nm, int& idx=Idx)  const { return (T*)FindName_(item_nm, idx); }
  // Find element with given name (nm) (NULL = not here), sets idx
  virtual T* 	FindType(TypeDef* item_tp, int& idx=Idx) const { return (T*)FindType_(item_tp, idx); }
  // find given type element (NULL = not here), sets idx

  virtual T*	Pop()				{ return (T*)Pop_(); }
  // pop the last element off the stack
  virtual T*	Peek()				{ return (T*)Peek_(); }
  // peek at the last element on the stack

  virtual T*	AddUniqNameOld(T* item)		{ return (T*)AddUniqNameOld_((void*)item); }
  // add so that name is unique, old used if dupl, returns one used
  virtual T*	LinkUniqNameOld(T* item)	{ return (T*)LinkUniqNameOld_((void*)item); }
  // link so that name is unique, old used if dupl, returns one used

  virtual bool	MoveBefore(T* trg, T* item) { return MoveBefore_((void*)trg, (void*)item); }
  // move item so that it appears just before the target item trg in the list
  virtual bool	MoveAfter(T* trg, T* item) { return MoveAfter_((void*)trg, (void*)item); }
  // move item so that it appears just after the target item trg in the list

  virtual T* 	FindLeafName(const char* item_nm, int& idx=Idx) const { return (T*)FindLeafName_(item_nm, idx); }
  // #MENU #MENU_ON_Edit #USE_RVAL #ARGC_1 #LABEL_Find Find element with given name (el_nm)
  virtual T* 	FindLeafType(TypeDef* item_tp, int& idx=Idx) const { return (T*)FindLeafType_(item_tp, idx);}
  // find given type leaf element (NULL = not here), sets idx

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
  void	Initialize() 		{ };
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
