// Copyright, 1995-2007, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of The Emergent Toolkit
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

#ifndef ta_list_h
#define ta_list_h 1

#include "ta_def.h"
#include "ta_string.h"
#include "ta_variant.h"

// this is the amount of overhead (in 4 byte units) allowed by the alloc
// routines, which allocate a power of two minus this amount
#define TA_ALLOC_OVERHEAD 2

// since keys are mostly used in Qt trees, it is much more efficient to use QString
#ifdef TA_USE_QT
# define KeyString QString
# define _nilKeyString QString()
#else
# define KeyString taString
# define _nilKeyString _nilString
#endif


// externals
class TypeDef; //
class taDataLink; //

// forwards

// pointer list:  semantics of ownership determined by the list functions Own, Ref
// Add = put on list and "own"
// Push = put on list, refer to, but don't own (ie. "link")

class  taPtrList_impl;
typedef taPtrList_impl* TALPtr;

class  taHashTable; //


// GROUP_ITEM_ events are all the leaf LIST events of a group, sent to the root group

/*
  Batch Updating

  Batch updating is used in pairs around complex/compound updating operations, in order
  to defer client notification until the end of the operation. The implementation should
  count the BEGIN/END calls. In addition, a child notification can be omitted when a
  parent object is also inside a corresponding batch operations whose notification will
  trigger the child operation anyways.

  There are two kinds of batch updates: Structural (str), and parametric (data).
  Structural updates will or might cause some kind of major structural or configural change
  to the data item, such that views of the item will probably be reset then rebuilt.
  Parameteric updates involve only changes to the values of existing data items, and
  typically don't involve any structural changes, such as addition or deletion of items, etc.
  It is assumed that the type of update done by a Parameteric update is implied in the more
  comprehensive Structural update.

  Here are the rules that should be applied regarding notifications and state:

  Parent State	Child State	effect. Child State	Child signal end of child batch?
  none		str		str			yes, str
  none		data		data			yes, data
  str		str		str			no
  data		str		str			yes, str
  str		data		str			no
  data		data		data			no

  The type of update being performed only needs to be communicated at the beginning, since
  an end operation can use the current state and parent state to determine the new effective
  state and whether an update should be triggered.

*/

/* What happens on various DCRs

  DCR_ITEM_UPDATED
    taiEditDataHost: does a GetImage
    
  NOTE: you can use the test (dcr <= DCR_ITEM_UPDATED_ND) to test for both versions
  
  NOTE: for lists and groups, all the DCRs relating to item add/remove/reorder
    are sequential, and can be tested via ?

  PLEASE keep numbers updated as some debuggers do not record the enum symbols..
*/

enum DataChangedReason { /* reason why DataChanged being called, as well as defining ops (also used by taBase and other classes) -- some data change operations will emit multiple DataChanged calls */
  DCR_ITEM_UPDATED = 0, // 0 after user edits (or load) ex. taBase::UpdateAfterEdit call; ops not used
  DCR_ITEM_UPDATED_ND, 	// 1 same as IU, but doesn't invoke Dirty (to avoid circular dirtying)
  DCR_CHILD_ITEM_UPDATED = 3, // 3 op1=item; can optionally be invoked by an owned object (usually a member, usually not list/group items) -- owner can ignore this, or do something with it
  
  DCR_ARY_SIZE_CHANGED, // 4 this is the only notify we send from arrays
  
  DCR_LIST_INIT = 10,	// 10 
  DCR_LIST_ITEM_UPDATE,	// 11 op1=item
  DCR_LIST_ITEM_INSERT,	// 12 op1=item, op2=item_after, null=at beginning
  DCR_LIST_ITEM_REMOVE,	// 13 op1=item -- note, item not DisOwned yet, but has been removed from list
  DCR_LIST_ITEM_MOVED,	// 14 op1=item, op2=item_after, null=at beginning
  DCR_LIST_ITEMS_SWAP,	// 15 op1=item1, op2=item2
  DCR_LIST_SORTED,	// 16 after sorting; ops not used

  DCR_GROUP_UPDATE = 21,// 21 op1=group, typically called for group name change
  DCR_GROUP_INSERT,	// 22 op1=group, op2=group_after, null=at beginning
  DCR_GROUP_REMOVE,	// 23 op1=group -- note, item not DisOwned yet, but has been removed from list
  DCR_GROUP_MOVED,	// 24 op1=group, op2=group_after, null=at beginning
  DCR_GROUPS_SWAP,	// 25 op1=group1, op2=group2
  DCR_GROUPS_SORTED,	// 26 op1=group1, op2=group2
  
  DCR_GROUP_ITEM_UPDATE = 31, // 31 op1=item
  DCR_GROUP_ITEM_INSERT, // 32 op1=item, op2=item_after, null=at beginning
  DCR_GROUP_ITEM_REMOVE, // 33 op1=item -- note, item not DisOwned yet, but has been removed from list
  DCR_GROUP_ITEM_MOVED,	 // 34 op1=item, op2=item_after, null=at beginning
  DCR_GROUP_ITEMS_SWAP,	 // 35 op1=item1, op2=item2
  DCR_GROUP_LIST_SORTED, // 36 after sorting; ops not used

  DCR_UPDATE_VIEWS = 40, // 40 no ops; sent for UpdateAllViews
  DCR_REBUILD_VIEWS, 	 // 41 no ops; sent to DataViews for RebuildAllViews
  
  DCR_STRUCT_UPDATE_BEGIN = 50, // 50 for some updating, like doing Layer->Build, better for gui to just do one
  DCR_STRUCT_UPDATE_END,  // 51 update operation at the end of everything
  DCR_DATA_UPDATE_BEGIN = 60, // 60 for some data changes, like various log updates, better for gui to just do one
  DCR_DATA_UPDATE_END,  // 61 update operation at the end of everything
    
  DCR_ITEM_DELETING = 70,  // 70 NOTE: not used in standard DataChanged calls, but may be used by forwarders, ex. taDataMonitor
  
  DCR_USER_DATA_UPDATED	= 80, // called when we create user data, or when user data is updated -- helps gui snooping so it doesn't do detailed UserData checks on every ITEM_UPDATED or such
  
  DCR_RESOLVE_NOW = 90, // called ex by SomeMethod() buttons to request all gui clients of the guy to apply their outstanding changes

#ifndef __MAKETA__
  DCR_LIST_MIN		= DCR_LIST_INIT,
  DCR_LIST_MAX		= DCR_LIST_SORTED,
  DCR_GROUP_MIN		= DCR_GROUP_UPDATE,
  DCR_GROUP_MAX		= DCR_GROUPS_SWAP,
  DCR_LIST_ITEM_MIN	= DCR_LIST_ITEM_UPDATE,
  DCR_LIST_ITEM_MAX	= DCR_LIST_SORTED,
  DCR_LIST_ORDER_MIN	= DCR_LIST_ITEM_INSERT, // anything related to item ordering
  DCR_LIST_ORDER_MAX	= DCR_LIST_SORTED,
  DCR_GROUP_ITEM_MIN	= DCR_GROUP_ITEM_UPDATE,
  DCR_GROUP_ITEM_MAX	= DCR_GROUP_LIST_SORTED,
  DCR_LIST_ITEM_TO_GROUP_ITEM_MIN = DCR_LIST_ITEM_UPDATE, // for checking to translate for item->item
  DCR_LIST_ITEM_TO_GROUP_ITEM_MAX = DCR_LIST_SORTED, // for checking to translate for item->item
  DCR_LIST_ITEM_TO_GROUP_MIN = DCR_LIST_ITEM_UPDATE, // for checking to translate for item->item
  DCR_LIST_ITEM_TO_GROUP_MAX = DCR_LIST_SORTED, // for checking to translate for item->item
  DCR_XXX_UPDATE_MIN = DCR_STRUCT_UPDATE_BEGIN, // for forwarding these guys, ex. Matrix
  DCR_XXX_UPDATE_MAX = DCR_DATA_UPDATE_END, // for forwarding these guys, ex. Matrix
#endif
};

/* DataChangedReason Notes

   GROUP_xx notifications are simply the LIST_ITEM notifications of the gp list,
   passed on to the owning Group as GROUP_ ops
   (by adding the DCR_List_Group_Offset to the op code)
*/

#define DCR_ListItem_GroupItem_Offset	(DCR_GROUP_ITEM_MIN - DCR_LIST_ITEM_TO_GROUP_ITEM_MIN)
#define DCR_ListItem_Group_Offset	(DCR_GROUP_MIN - DCR_LIST_ITEM_TO_GROUP_MIN)

typedef int taListItr; // pseudo class, compatible with the FOR_ITR_EL macro in ta_group

//typedef uintptr_t taHashVal;
typedef unsigned long taHashVal;


/////////////////////////////////////////////////////////////////////
//			PtrList
/////////////////////////////////////////////////////////////////////

class TA_API  taPtrList_impl {
  // #NO_TOKENS implementation of the pointer list class
public:
  enum KeyType {
    KT_NAME,			// use item name or string value -- the default
    KT_PTR,			// use the item pointer
  };

  static taHashVal	HashCode_String(const String& string);
  // #CAT_XpertAccess get a hash code value from given string
  static taHashVal	HashCode_Ptr(const void* ptr);
  // #CAT_XpertAccess get a hash code value from given ptr

protected:
  static taPtrList_impl scratch_list;	// a list for any temporary processing needs

  // these define what to do with an individual item (overload them!)
  virtual String 	GetListName_() const 		{ return _nilString; }
  // name of the list
  virtual String 	El_GetName_(void*) const 	{ return _nilString; }
  // gets name from an element (for list)
  virtual void 		El_SetName_(void*, const String&)  {}
  // mostly for whole-list copies, where we copy the name too
  virtual void		El_SetDefaultName_(void*, int) {}
    // if added item has empty name, this will get called, enabling a name to be set; index has been set
  virtual taHashVal 	El_GetHashVal_(void* it) const;
  // gets hash code based on key type in hash table; default is for string-key'ed lists (v3.2 default)
  virtual String 	El_GetHashString_(void* it) const;
  // gets hash string if using KT_NAME, else _nilString -- for passing string arg to hash funs
  virtual TALPtr	El_GetOwnerList_(void*) const	{ return (TALPtr)this; }
  // who owns the el? -- only returns a list if the owner is a list
  virtual void*		El_GetOwnerObj_(void*) const	{ return NULL; }
  // who owns the el?
  virtual void*		El_SetOwner_(void* it) 		{ return it; }
  // set owner to this
  virtual void		El_SetIndex_(void*, int) 	{ };
  // sets the element's self-index
  virtual bool  	El_FindCheck_(void* it, const String& nm) const
  { return (El_GetName_(it) == nm); }
  virtual int		El_Compare_(void* a, void* b) const
  { int rval=-1; if(El_GetName_(a) > El_GetName_(b)) rval=1;
    else if(El_GetName_(a) == El_GetName_(b)) rval=0; return rval; }
  // compare two items for purposes of sorting

  virtual void*	El_Ref_(void* it)	{ return it; }	// when pushed
  virtual void* El_unRef_(void* it)  	{ return it; }	// when popped
  virtual void	El_Done_(void*)		{ };	// when "done" (delete)
  virtual void*	El_Own_(void* it)	{ El_SetOwner_(El_Ref_(it)); return it; }
  virtual void	El_disOwn_(void* it)	{ El_Done_(El_unRef_(it)); }

  virtual void*	El_MakeToken_(void* it) 	{ return it; }
  // how to make a token of the same type as the argument
  virtual void*	El_Copy_(void* trg, void*) { return trg; }
  // how to copy from given item (2nd arg); usually doesn't copy name
  virtual void*	El_CopyN_(void* trg, void* fm);
  // El_Copy_ + name copy; our version synthesizes, but you can override for efficiency
  enum ElKind {
    EK_NULL	= 0, // duh!
    EK_OWN	= 1, // owned by list, is an instance
    EK_LINK	= 2 // is a linked item
  };
  virtual ElKind	El_Kind_(void* it) const; // kind of item; normally not overidden
  
  void		InitList_();
  virtual int	Scratch_Find_(const String& it) const;
  // find item on the scratch_list using derived El_FindCheck_()
  void		UpdateIndex_(int idx);
  // update the index of the item at given index (i.e., which was just moved)

public:
  void**	el;		// #IGNORE the elements themselves
  int 		alloc_size;	// #READ_ONLY #NO_SAVE #CAT_taList allocation size
  taHashTable*	hash_table;	// #READ_ONLY #NO_SAVE #HIDDEN #CAT_taList a hash table (NULL if not used)
  int		size;		// #READ_ONLY #NO_SAVE #SHOW #CAT_taList number of elements in the list

  taPtrList_impl()			{ InitList_(); }
  taPtrList_impl(const taPtrList_impl& cp)	{ InitList_(); Duplicate(cp); }
  virtual ~taPtrList_impl();

  static ostream& Indenter(ostream& strm, const String& itm, int no, int prln, int tabs);
  // #IGNORE

  ////////////////////////////////////////////////
  // 	functions that return the type		//
  ////////////////////////////////////////////////

  void*   	GetTA_Element_(Variant i, TypeDef*& eltd) const; // #IGNORE 
  virtual TypeDef*	El_GetType_(void*) const {return GetElType();}
    // #IGNORE should usually override to provide per-item typing where applicable
  void*		SafeEl_(int i) const
  { void* rval=NULL; i=Index(i); if(InRange(i)) rval = el[i]; return rval; } 	// #IGNORE
  void*		PosSafeEl_(int i) const
  { void* rval=NULL; if(InRange(i)) rval = el[i]; return rval; } 	
    // #IGNORE -- for internal use only, where you want NULL if i < 0
  void*		FastEl_(int i)	const	{ return el[i]; } 	// #IGNORE
  virtual void*	FindName_(const String& it) const;	// #IGNORE
  void*		Pop_();	// #IGNORE -- NOTE: non-standard semantics, does not do a disown
  void*		TakeItem_(int idx); // #IGNORE -- WARNING: not for owned items!!!
  void*		First_() const
  { if (size > 0) return el[0]; else return NULL; }  // #IGNORE
  void*		Peek_() const
  { if(size > 0) return el[size-1]; else return NULL; }  // #IGNORE

  virtual void*	AddUniqNameOld_(void* it);
  // #IGNORE add so that name is unique, old used if dupl, returns one used

  virtual void*	LinkUniqNameOld_(void* it);		// #IGNORE


  ////////////////////////////////////////////////
  // 	functions that are passed el of type	//
  ////////////////////////////////////////////////

  virtual int	FindEl_(const void* it) const; 			// #IGNORE

  virtual void	AddOnly_(void* it);
  // #IGNORE just puts the el on the list, doesn't do anything else
  virtual void	Add_(void* it, bool no_notify = false); 	// #IGNORE
  virtual bool	AddUnique_(void* it);			// #IGNORE
  virtual bool	AddUniqNameNew_(void* it);
  // #IGNORE add a new object so that list only has one, new replaces existing
  virtual void*	DuplicateEl_(void* it);
  // #IGNORE duplicate given item and add to list, returning new item, or NULL if failed
  virtual bool	Insert_(void* it, int where, bool no_notify = false);		// #IGNORE
  virtual bool 	ReplaceEl_(void* ol, void* nw);		// #IGNORE
  virtual bool 	ReplaceName_(const String& ol, void* nw);	// #IGNORE
  virtual bool 	ReplaceIdx_(int ol, void* nw, bool no_notify_insert = false); // #IGNORE
  virtual bool 	Transfer_(void* it);			// #IGNORE
  virtual bool	RemoveEl_(void* it);			// #IGNORE
  virtual bool	MoveBefore_(void* trg, void* item); 	// #IGNORE
  virtual bool	MoveAfter_(void* trg, void* item); 	// #IGNORE

  virtual void 	Link_(void* it);			// #IGNORE
  virtual bool	LinkUnique_(void* it); 			// #IGNORE
  virtual bool	LinkUniqNameNew_(void* it);		// #IGNORE
  virtual bool	InsertLink_(void* it, int where);	// #IGNORE
  virtual bool 	ReplaceLinkEl_(void* ol, void* nw);	// #IGNORE
  virtual bool 	ReplaceLinkName_(const String& ol, void* nw);	// #IGNORE
  virtual bool 	ReplaceLinkIdx_(int ol, void* nw);		// #IGNORE

  virtual void	Push_(void* it);			// #IGNORE

  ////////////////////////////////////////////////
  // functions that don't depend on the type	//
  ////////////////////////////////////////////////

  virtual int	FindNameIdx(const String& it) const;
  // #CAT_Access return the index of the item with given name on the list

  virtual TypeDef* GetElType() const {return NULL;}
  // #IGNORE Default type for objects in group
  virtual void	DataChanged(int dcr, void* op1 = NULL, void* op2 = NULL) {}
  // #IGNORE called when list has changed -- more fine-grained than Dirty(), and may be multiple calls per event
  inline int	Index(int idx) const { if(idx < 0) idx += size; return idx; }
  // #CAT_XpertAccess get actual index from index value that can also be negative, meaning access from end of list
  inline bool	InRange(int idx) const { return ((idx < size) && (idx >= 0)); }
  // #CAT_XpertAccess is the specified index within range of 0 >= idx < size
  virtual bool	Alloc(int sz);
  // #CAT_XpertModify allocate a list big enough for given number of elements (or current size) -- uses optimized memory allocation policies and generally allocates more than currently needed
  virtual bool	AllocExact(int sz);
  // #CAT_XpertModify allocate exact number specified
  void		Trim(int n); // #IGNORE if larger than n, trim to n (does NOT expand)
  virtual void 	Reset()			{ RemoveAll(); }
  // #CAT_Modify reset the list (remove all elements)
  virtual bool	IsEmpty() const	{ return (size == 0) ? true : false; }
  // #CAT_Access is the list empty of elements (i.e., size == 0)

  virtual void	BuildHashTable(int n_buckets, KeyType key_typ = KT_NAME);
  // #CAT_Modify build a hash table with given number of buckets and key type (not dynamic, so make it big)

  virtual bool	RemoveName(const String& item_nm);
  // #CAT_Modify remove (and delete) named element from list
  virtual bool	RemoveIdx(int idx);
  // #CAT_Modify remove (and delete) element from list at index
  virtual bool	RemoveLast();
  // #CAT_Modify remove the last element on the list
  virtual void	RemoveAll();
  // #MENU #MENU_ON_Edit #CONFIRM #CAT_Modify Remove all elements on the list

  virtual bool	MoveIdx(int from, int to);
  // #CAT_Modify Move element from index (from) to position (to) in list
  virtual bool	MoveBeforeIdx(int from, int to);
  // #CAT_Modify Move element from index (from) to just before position (to) in list; for end, use to=-1 or size
  virtual bool	SwapIdx(int pos1, int pos2);
  // #CAT_Modify Swap the elements in the two given positions on the list

  virtual void	PopAll();
  // #CAT_Modify pop all elements off the stack

  virtual void	Permute();
  // #MENU #CONFIRM #CAT_Order permute the items in the list into a random order
  virtual void	Sort(bool descending=false);
  // #MENU #CONFIRM #CAT_Order sort the items in the list in alpha order according to name (or El_Compare_)
  virtual void	Sort_(bool descending=false);	// #IGNORE implementation of sorting function
  virtual void 	UpdateAllIndicies();	// #IGNORE update all indices of elements in list

  void*		FirstEl(taListItr& itr) {itr = 0; return SafeEl_(0);}
  // #CAT_XpertAccess get the first item on the list, initialize iterator
  void*		NextEl(taListItr& itr) {return (++itr < size) ? FastEl_(itr) : NULL;}
  // #CAT_XpertAccess get the next item on the list according to iterator

  /////////////////////////////////////////////////////////////////////////
  // replicating items: either by clone/add (duplicate) or link (borrow) //
  /////////////////////////////////////////////////////////////////////////

  void	Duplicate(const taPtrList_impl& cp);
  // #CAT_Copy duplicate (clone & add) elements of given list into this one
  void	DupeUniqNameNew(const taPtrList_impl& cp);
  // #CAT_Copy duplicate so result is unique names, replacing with new ones where dupl
  void	DupeUniqNameOld(const taPtrList_impl& cp);
  // #CAT_Copy duplicate so result is unique names, using old ones where dupl

  void	Stealth_Borrow(const taPtrList_impl& cp);
  // #CAT_Copy borrow without referencing the borrowed elements (i.e. use AddEl_())
  void	Borrow(const taPtrList_impl& cp);
  // #CAT_Copy borrow (link) elements of given list into this one
  void	BorrowUnique(const taPtrList_impl& cp);
  // #CAT_Copy borrow so result is unique list
  void	BorrowUniqNameNew(const taPtrList_impl& cp);
  // #CAT_Copy borrow so result is unique names, replacing with new ones where dupl
  void	BorrowUniqNameOld(const taPtrList_impl& cp);
  // #CAT_Copy borrow so result is unique names, using old ones where dupl

  /////////////////////////////////
  // copying items between lists //
  /////////////////////////////////

  void	Copy_Common(const taPtrList_impl& cp);
  // #CAT_Copy apply copy operator to only those items in common between the two lists
  void	Copy_Duplicate(const taPtrList_impl& cp);
  // #CAT_Copy apply copy operator to items, use duplicate to add new ones from cp (if necc)
  void	Copy_Borrow(const taPtrList_impl& cp);
  // #CAT_Copy apply copy operator to items, use borrow to add new ones from cp (if necc)
  void	Copy_Exact(const taPtrList_impl& cp);
  // #CAT_Copy makes us basically identical to cp, in number, and type
  void	Hijack(taPtrList_impl& src);
  // #IGNORE specialized usage, transfers entire the memory to us, leaving src empty

  // browsing -- browse client lists must override
  virtual int		NumListCols() const {return 0;}
  // #IGNORE number of columns in a list view for this item type
  virtual const KeyString GetListColKey(int col) const {return _nilKeyString;}
  // #IGNORE col key for the default list column
  virtual String	GetColHeading(const KeyString& key) const {return _nilKeyString;} 
  // #IGNORE header text for the given key
  virtual String	ChildGetColText(void* child, TypeDef* typ, const KeyString& key, 
    int itm_idx = -1) const {return _nilKeyString;}
  // #IGNORE itm_idx is a hint from source, -1 means not specified or ignore

  // output
  virtual void 	List(ostream& strm=cout) const;
  // #CAT_Display List the group items
protected:
  virtual void		ItemRemoved_() {} // we overload this in groups to update the leaf counts
  void			Copy_Duplicate_impl(const taPtrList_impl& cp);
    // factored code
};

template<class T> 
class taPtrList : public taPtrList_impl { // #INSTANCE
public:
  taPtrList(bool=false)					{ };
  taPtrList(const taPtrList<T>& cp) 		{ Borrow(cp); }
  // borrow is guaranteed to work, others require EL_ functions..
  taPtrList(const taPtrList<T>& cp, bool) 		{ Borrow(cp); }

  ////////////////////////////////////////////////
  // 	functions that return the type		//
  ////////////////////////////////////////////////

  // operators
  T*		SafeEl(int i) const		{ return (T*)SafeEl_(i); }
  T*		PosSafeEl(int i) const		{ return (T*)PosSafeEl_(i); }
  // #IGNORE element at index
  T*		FastEl(int i) const		{ return (T*)el[i]; }
  // #CAT_Access fast element (no range checking)
  T* 		operator[](int i) const		{ return (T*)el[i]; }
  void		operator=(const taPtrList<T>& cp) { Reset(); Borrow(cp); }
  // borrow is guaranteed to work, others require EL_ functions..

  T*		Edit_El(T* item) const		{ return SafeEl(FindEl(item)); }
  // #CAT_Access #MENU #MENU_ON_Edit #USE_RVAL #ARG_ON_OBJ Edit given list item

  T*		FindName(const String& item_nm) const
  { return (T*)FindName_(item_nm); }
  // #CAT_Access find given named element (NULL = not here), sets idx
  T*		First()				{ return (T*)First_(); }
  // #CAT_Modify return first element, or NULL if list empty
  T*		TakeItem(int idx)	{ return (T*)TakeItem_(idx); }
  // #CAT_Modify remove the element from list, NULL if idx out of range -- ONLY FOR NON-OWNED ITEMS
  T*		Pop()				{ return (T*)Pop_(); }
  // #CAT_Modify pop the last element off the stack
  T*		Peek() const			{ return (T*)Peek_(); }
  // #CAT_Access peek at the last element on the stack

  virtual T*	AddUniqNameOld(T* item)		{ return (T*)AddUniqNameOld_((void*)item); }
  // #CAT_Modify add so that name is unique, old used if dupl, returns one used
  virtual T*	LinkUniqNameOld(T* item)		{ return (T*)LinkUniqNameOld_((void*)item); }
  // #CAT_Modify link so that name is unique, old used if dupl, returns one used

  ////////////////////////////////////////////////
  // 	functions that are passed el of type	//
  ////////////////////////////////////////////////

  virtual int	FindEl(const T* item) const	{ return FindEl_((const void*)item); }
  // #CAT_Access find element in list (-1 if not there)
  virtual void	AddOnly(T* item)		{ AddOnly_((void*)item); }
  // #IGNORE append a new pointer to end of list, does not own it or do anything else
  virtual void	Add(T* item)	      		{ Add_((void*)item); }
  // #CAT_Modify add element to the list and "own" item
  virtual bool	AddUnique(T* item)		{ return AddUnique_((void*)item); }
  // #CAT_Modify add so that object is unique, true if unique
  virtual bool	AddUniqNameNew(T* item)		{ return AddUniqNameNew_((void*)item); }
  // #CAT_XpertModify add so that name is unique, true if unique, new replaces existing

  virtual bool	Insert(T* item, int idx)	{ return Insert_((void*)item, idx); }
  // #CAT_Modify Add or insert element at idx (-1 for end)
  virtual bool 	ReplaceEl(T* old_it, T* new_it)	{ return ReplaceEl_((void*)old_it, (void*)new_it); }
  // #CAT_Modify Replace old element with new element
  virtual bool 	ReplaceName(const String& old_nm, T* new_it)	{ return ReplaceName_(old_nm, (void*)new_it); }
  // #CAT_Modify replace element with given name with the new one
  virtual bool 	ReplaceIdx(int old_idx, T* new_it)	{ return ReplaceIdx_(old_idx, (void*)new_it); }
  // #CAT_Modify replace element at index with the new one

  virtual bool	RemoveEl(T* item)	{ return RemoveEl_(item); }
  // #CAT_Modify #MENU #ARG_ON_OBJ Remove given item from list
  // note: folowing not virt, because we hide in taList with stronger typed version
  virtual T* 	DuplicateEl(const T* item) { return (T*)DuplicateEl_((void*)item); }
  // #CAT_Modify #MENU #ARG_ON_OBJ Duplicate given list item and Add to list; returns item added, or NULL if failed

  virtual void 	Link(T* item)			{ Link_((void*)item); }
  // #CAT_Modify Link an item to list without owning it
  virtual bool	LinkUnique(T* item)		{ return LinkUnique_((void*)item); }
  // #CAT_Modify link so that object is unique, true if unique
  virtual bool	LinkUniqNameNew(T* item)		{ return LinkUniqNameNew_((void*)item); }
  // #CAT_XpertModify link so that name is unique, true if unique, new replaces existing
  virtual bool	InsertLink(T* item, int idx= -1) { return InsertLink_((void*)item, idx);}
  // #CAT_Modify #MENU Insert a link at index (-1 for end)
  virtual bool 	ReplaceLinkEl(T* old_it, T* new_it)	{ return ReplaceLinkEl_((void*)old_it, (void*)new_it); }
  // #CAT_Modify replace given element (if on list) with the new one
  virtual bool 	ReplaceLinkName(const String& old_nm, T* new_it) { return ReplaceLinkName_(old_nm, (void*)new_it); }
  // #CAT_Modify replace given named element (if on list) with the new one
  virtual bool 	ReplaceLinkIdx(int old_idx, T* new_it)	{ return ReplaceLinkIdx_(old_idx, (void*)new_it); }
  // #CAT_Modify replace element with a link to the new one

  virtual void	Push(T* item)			{ Push_((void*)item); }
  // #CAT_Modify push item on stack (for temporary use, not "owned")

  virtual bool 	MoveEl(T* from, T* to)		{ return MoveIdx(FindEl(from), FindEl(to)); }
  // #CAT_Modify #MENU #ARG_ON_OBJ Move item (from) to position of (to)
  virtual bool 	Transfer(T* item)  		{ return Transfer_((void*)item); }
  // #CAT_Modify #MENU #MENU_ON_Edit #NO_SCOPE Transfer item to this list

  virtual bool	MoveBefore(T* trg, T* item) { return MoveBefore_((void*)trg, (void*)item); }
  // #CAT_Modify move item so that it appears just before the target item trg in the list
  virtual bool	MoveAfter(T* trg, T* item) { return MoveAfter_((void*)trg, (void*)item); }
  // #CAT_Modify move item so that it appears just after the target item trg in the list
};


#define taPtrList_of(T)						      \
class TA_API T ## _List : public taPtrList<T> {				      \
protected:								      \
  void	El_Done_(void* item)	{ delete (T*)item; }			      \
public:                                                                       \
  ~ ## T ## _List()             { Reset(); }                                  \
}

template<class T> 
class taPtrList_base : public taPtrList_impl { // #INSTANCE
public:
  taPtrList_base()					{ };
  taPtrList_base(const taPtrList_base<T>& cp) 		{ Duplicate(cp); };

  ////////////////////////////////////////////////
  // 	functions that are passed el of type	//
  ////////////////////////////////////////////////

  virtual int	FindEl(const T* item) const	{ return FindEl_((const void*)item); }
  // #CAT_Access find element in list (-1 if not there)

  virtual void	AddOnly(T* item)			{ AddOnly_((void*)item); }
  // #CAT_Modify append a new pointer to end of list -- does not own
  virtual void	Add(T* item)	      		{ Add_((void*)item); }
  // #CAT_Modify add element to the list and "own" item
  virtual bool	AddUnique(T* item)		{ return AddUnique_((void*)item); }
  // #CAT_Modify add so that object is unique, true if unique
  virtual bool	AddUniqNameNew(T* item)		{ return AddUniqNameNew_((void*)item); }
  // #CAT_Modify add so that name is unique, true if unique, new replaces existing

  virtual bool	Insert(T* item, int where)	{ return Insert_((void*)item, where); }
  // #CAT_Modify insert element at index (-1 for end)
  virtual bool 	ReplaceEl(T* old_it, T* new_it)	{ return ReplaceEl_((void*)old_it, (void*)new_it); }
  // #CAT_Modify replace given element with the new one
  virtual bool 	ReplaceName(const String& old_nm, T* new_it)	{ return ReplaceName_(old_nm, (void*)new_it); }
  // #CAT_Modify replace named element with the new one
  virtual bool 	ReplaceIdx(int old_idx, T* new_it)		{ return ReplaceIdx_(old_idx, (void*)new_it); }
  // #CAT_Modify replace element at index with the new one

  virtual bool	RemoveEl(T* item)		{ return RemoveEl_((void*)item); }
  // #MENU #ARG_ON_OBJ #CAT_Modify Remove given item from list
  T* 		DuplicateEl(const T* item) { return (T*)DuplicateEl_((void*)item); }
  // #MENU #ARG_ON_OBJ #CAT_Modify Duplicate given list item and Add to list

  virtual void 	Link(T* item)			{ Link_((void*)item); }
  // #CAT_Modify Link an item to list without owning it
  virtual bool	LinkUnique(T* item)		{ return LinkUnique_((void*)item); }
  // #CAT_Modify link so that object is unique, true if unique
  virtual bool	LinkUniqNameNew(T* item)		{ return LinkUniqNameNew_((void*)item); }
  // #CAT_Modify link so that name is unique, true if unique, new replaces existing
  virtual bool	InsertLink(T* item, int idx= -1)	{ return InsertLink_((void*)item, idx);}
  // #MENU #CAT_Modify Insert a link at index (-1 for end)
  virtual bool 	ReplaceLinkEl(T* old_it, T* new_it)	{ return ReplaceLinkEl_((void*)old_it, (void*)new_it); }
  // #CAT_Modify replace given element (if on list) with the new one
  virtual bool 	ReplaceLinkName(const String& old_nm, T* new_it) { return ReplaceLinkName_(old_nm, (void*)new_it); }
  // #CAT_Modify replace given named element (if on list) with the new one
  virtual bool 	ReplaceLinkIdx(int old_idx, T* new_it)	{ return ReplaceLinkIdx_(old_idx, (void*)new_it); }
  // #CAT_Modify replace element with a link to the new one

  virtual void	Push(T* item)			{ Push_((void*)item); }
  // #CAT_Modify push item on stack (for temporary use, not "owned")

  virtual bool 	MoveEl(T* from, T* to)		{ return MoveIdx(FindEl(from), FindEl(to)); }
  // #MENU #ARG_ON_OBJ Move #CAT_Modify item (from) to position of (to)
  virtual bool 	Transfer(T* item)  		{ return Transfer_((void*)item); }
  // #MENU #MENU_ON_Edit #NO_SCOPE #CAT_Modify Transfer element to this list

  virtual bool	MoveBefore(T* trg, T* item) { return MoveBefore_((void*)trg, (void*)item); }
  // #CAT_Modify move item so that it appears just before the target item trg in the list
  virtual bool	MoveAfter(T* trg, T* item) { return MoveAfter_((void*)trg, (void*)item); }
  // #CAT_Modify move item so that it appears just after the target item trg in the list
};


/////////////////////////////////////////////////////////////////////
//			HashTable
/////////////////////////////////////////////////////////////////////

// the hash table is for use in looking up things in PtrList's by name
// it contains hash codes and corresponding indicies for items in the list

class TA_API  taHashEl {
  // ##NO_TOKENS holds information for one entry of the hash table
public:
  taHashVal	hash_code;	// hash-code for looking up
  int		value;		// value associated with hash code (e.g., index of item in list)
  String	hashed_str;	// source value for the hash code if a string -- this is necessary because the hash code is not guaranteed to be unique..

  void	Initialize()	{ hash_code = 0; value = -1; }
  taHashEl()		{ Initialize(); }
  taHashEl(taHashVal hash, int val, const String& str = _nilString)
    { hash_code = hash; value = val; hashed_str = str; }
};

class TA_API  taHashBucket : public taPtrList<taHashEl> {
  // holds a set of hash table entries that all have the same hash_code modulo value
protected:
  void	El_Done_(void* it)	{ delete (taHashEl*)it; }
public:
  virtual int	FindBucketIndex(taHashVal hash, const String& str = _nilString) const;
  // find index of item in the bucket with given hash code, and string if using
  virtual int	FindHashVal(taHashVal hash, const String& str = _nilString) const;
  // find hash value associated with given hash code, and string if using

  ~taHashBucket()               { Reset(); }
};

class TA_API  taHashTable : public taPtrList<taHashBucket> {
  // table has a number of buckets, each with some hash values
protected:
  void	El_Done_(void* it)	{ delete (taHashBucket*)it; }
public:
  static int	n_bucket_primes[]; // prime numbers for number of buckets
  static int	n_primes;	 // number of prime numbers (86)

  int		bucket_max;	// maximum size of any bucket
  KeyType	key_type;	// type of key to use by default (name/string or pointer)

  // allocate in prime-number increments
  override bool Alloc(int sz);
  override void	RemoveAll();

  virtual int	FindHashVal(taHashVal hash, const String& str = _nilString) const;
  // find value associated with given hash code (-1 if not found)
  virtual int	FindHashValString(const String& str) const
  { return FindHashVal(HashCode_String(str), str); }
  // find value associated with given string (-1 if not found)
  virtual int	FindHashValPtr(const void* ptr) const
  { return FindHashVal(HashCode_Ptr(ptr)); }
  // find value associated with given pointer (-1 if not found)

  virtual void 	AddHash(taHashVal hash, int val, const String& str = _nilString);
  // add a new item to the hash table
  virtual void 	AddHashString(const String& str, int val)
  { AddHash(HashCode_String(str), val, str); }
  // add a new string item to the hash table
  virtual void 	AddHashPtr(const void* ptr, int val)
  { AddHash(HashCode_Ptr(ptr), val); }
  // add a new pointer item to the hash table

  virtual bool	RemoveHash(taHashVal hash, const String& str = _nilString);
  // remove given hash code from table
  virtual bool	RemoveHashString(const String& str)
  { return RemoveHash(HashCode_String(str), str); }
  // remove given string from table
  virtual bool	RemoveHashPtr(const void* ptr)
  { return RemoveHash(HashCode_Ptr(ptr)); }
  // remove given pointer from table

  virtual bool	UpdateHashVal(taHashVal hash, int val, const String& str = _nilString);
  // update value associated with hash item
  virtual bool	UpdateHashValString(const String& str, int val)
  { return UpdateHashVal(HashCode_String(str), val, str); }
  // update value associated with string item
  virtual bool	UpdateHasValPtr(const void* ptr, int val)
  { return UpdateHashVal(HashCode_Ptr(ptr), val); }
  // update value associated with ptr item

  void		InitList_();
  taHashTable()			{ key_type = KT_NAME; InitList_(); }
  taHashTable(const taHashTable& cp)	{ key_type = cp.key_type; InitList_(); Duplicate(cp); }
  ~taHashTable()                { Reset(); }
};

/////////////////////////////////////////////////////////////////////
//			Arrays
/////////////////////////////////////////////////////////////////////

class TA_API  taArray_impl {
  // #NO_TOKENS Base Type for Arrays: physically contiguous allocation of an array of objects
public:
  int 		size;			// #NO_SAVE #READ_ONLY #CAT_taArray number of elements in the array
  int		alloc_size;		// #READ_ONLY #NO_SAVE #EXPERT #CAT_taArray allocated (physical) size

  taArray_impl()			{ alloc_size = 0; size = 0; }
  virtual ~taArray_impl()		{ alloc_size = 0; size = 0; }

  virtual void		DataChanged(int dcr, void* op1 = NULL, void* op2 = NULL) {}
  // #IGNORE only called when size changes 
  inline int		Index(int idx) const { if(idx < 0) idx += size; return idx; }
  // #CAT_XpertAccess get actual index from index value that can also be negative, meaning access from end of array
  inline bool		InRange(int idx) const {return ((idx < size) && (idx >= 0));}
  // #CAT_XpertAccess is the specified index within range of 0 >= idx < size
  virtual bool		Alloc(int n);
  // #CAT_Modify allocate storage for at least the given size
  virtual void		Reset()	{ Reset_impl(); DataChanged(DCR_ARY_SIZE_CHANGED); };
  // #CAT_Modify reset the list to zero size (does not free memory)

  ////////////////////////////////////////////////
  // 	internal functions that depend on type	//
  ////////////////////////////////////////////////

  virtual const void*	SafeEl_(int) const;
  // #IGNORE element at posn
  virtual void*		FastEl_(int)	{ return NULL; }
  // #IGNORE element at posn
  virtual const void*	FastEl_(int i) const 
    {return const_cast<taArray_impl*>(this)->FastEl_(i);}
  virtual bool		El_Equal_(const void*, const void*) const { return false; }
  // #IGNORE for value equality testing
  virtual bool		Equal_(const taArray_impl& ar) const; // value equality, implements == and != operators, ar must be same type
  virtual int		El_Compare_(const void*, const void*) const	{ return 0; }
  // #IGNORE for sorting
  virtual void		El_Copy_(void*,const void*)		{ };
  // #IGNORE
  virtual uint		El_SizeOf_() const		{ return 1; }
  // #IGNORE size of element
  virtual void*		El_GetTmp_() const		{ return NULL; }
  // #IGNORE return ptr to Tmp of type
  virtual const void*	El_GetErr_() const {return NULL;}	 // #IGNORE
  virtual String	El_GetStr_(const void*) const		{ return _nilString; } // #IGNORE
  virtual void		El_SetFmStr_(void*, const String&) 	{ };       // #IGNORE
  virtual void*		BlankEl_() const		{ return NULL; }
  // address of a blank element, for initializing empty items
  virtual void		Clear_Tmp_();				       // #IGNORE

  virtual void		Add_(const void* it);			// #IGNORE
  virtual bool		AddUnique_(const void* it);		// #IGNORE
  virtual void		Insert_(const void* it, int where, int n=1); // #IGNORE
  virtual int		FindEl_(const void* it, int where=0) const; 	// #IGNORE
  virtual bool		RemoveEl_(const void* it);		// #IGNORE
  virtual void		InitVals_(const void* it, int start=0, int end=-1);// #IGNORE

  ////////////////////////////////////////////////
  // functions that don't depend on the type	//
  ////////////////////////////////////////////////

  virtual String	SafeElAsStr(int idx) const
    {return InRange(idx) ? El_GetStr_(SafeEl_(idx)) : _nilString;}
  // #IGNORE element at idx, as a string
  virtual String	FastElAsStr(int idx) const {return El_GetStr_(FastEl_(idx));}
  // #IGNORE element at idx, as a string
  virtual void	SetSize(int sz);
  // #CAT_Modify #MENU #MENU_ON_Edit #INIT_ARGVAL_ON_size force array to be of given size by inserting blanks or removing

  virtual void	AddBlank(int n_els = 1);
  // #CAT_Modify #MENU #MENU_ON_Edit Add n_els empty elements to the end of the array
  virtual bool	RemoveIdx(uint idx, int n_els=1);
  // #CAT_Modify #MENU #MENU_ON_Edit Remove (n_els) item(s) at idx, returns success
  virtual bool	MoveIdx(int from, int to);
  // #CAT_Modify #MENU move item from index to index
  virtual bool	SwapIdx(int pos1, int pos2);
  // #CAT_Modify #CAT_Modify Swap the elements in the two given positions on the list
  virtual void	Permute();
  // #CAT_Modify #MENU permute the items in the list into a random order
  virtual void	Sort(bool descending=false);
  // #CAT_Modify #MENU sort the list in ascending order (or descending if switched)
  virtual void	ShiftLeft(int nshift);
  // #CAT_Modify shift all the elements in the array to the left by given number of items
  virtual void	ShiftLeftPct(float pct);
  // #CAT_Modify shift the array to the left by given percentage of current size
  virtual int	V_Flip(int width);
  // #CAT_XpertModify vertically flip the array as if it was arrange in a matrix of width

  virtual void	Duplicate(const taArray_impl& cp);
  // #CAT_Copy duplicate the items in the list
  virtual void	DupeUnique(const taArray_impl& cp);
  // #CAT_Copy duplicate so result is unique list
  virtual void	Copy_Common(const taArray_impl& cp);
  // #CAT_Copy copy elements in common
  virtual void	Copy_Duplicate(const taArray_impl& cp);
  // #CAT_Copy copy elements in common, duplicating (if necc) any extra on cp
  virtual void 	CopyVals(const taArray_impl& from, int start=0, int end=-1, int at=0);
  // #CAT_Copy copy values from other array at given start and end points, and putting at given point in this
  virtual void	List(ostream& strm = cout) const;
  // #CAT_Display print out all of the elements in the array
  virtual String GetArrayStr() const;
  // #CAT_Display get a string representation of the value of the array (list of items)
  virtual void	InitFromString(const String& val);
  // #CAT_Modify initialize an array from given string (does reset first)
protected:

  void			AddOnly_(const void* it); // don't notify
  bool			AddUniqueOnly_(const void* it);	// don't notify
  void 			RemoveIdxOnly(int i); //  don't notify, no checks
  virtual void		Reset_impl() {SetSize(0);} // don't notify
  virtual void*		MakeArray_(int i) const	{ return NULL; } // #IGNORE make a new array of item type
  virtual void		SetArray_(void* nw) {}
  virtual void		ReclaimOrphans_(int start, int end) {}// #IGNORE called when array is shortened, leaving orphaned values; note 'size' may already be trimmed: NOT called when el[] is replaced
  void			Copy_(const taArray_impl& cp);
  // replace our array with the source items -- note: added in 4.0 for new uses, ex. in Matrix_Array
};

// the plainarray is not a taBase..
/*nuke
// Default string converter class

template<class T> 
class DefaultStringConverter {
public:
  String s;
  DefaultStringConverter(const T& val) {s = val;}
  DefaultStringConverter(String& val) {s = val;}
  DefaultStringConverter(const DefaultStringConverter& val) {s = val.s;}
  DefaultStringConverter& operator =(const DefaultStringConverter& val) {s = val.s; return *this;} 
  operator String() {return s;} 
  operator T() const { return s;} // forces implicit conversion to T
}; */

template<class T> 
class taPlainArray : public taArray_impl {
  // #INSTANCE ##NO_TOKENS a plain array
public:
  T*		el;		// #HIDDEN #NO_SAVE #CAT_taArray Pointer to actual array memory
  T		err;		// #HIDDEN #CAT_taArray what is returned when out of range -- MUST INIT IN CONSTRUCTOR

  void* 	FastEl_(int i)		{ return &(el[i]); } // #IGNORE
  const void*	FastEl_(int i) const  	{ return (const void*)&(el[i]); } // #IGNORE

  int		El_Compare_(const void* a, const void* b) const
  { int rval=-1; if(*((T*)a) > *((T*)b)) rval=1; else if(*((T*)a) == *((T*)b)) rval=0; return rval; }
  // #IGNORE
  void		El_Copy_(void* to, const void* fm)	{ *((T*)to) = *((T*)fm); } // #IGNORE
  uint		El_SizeOf_() const		{ return sizeof(T); }	 // #IGNORE
  void*		El_GetTmp_() const		{ return (void*)&tmp; }	 // #IGNORE
  const void*	El_GetErr_() const		{ return (void*)&err; }	 // #IGNORE
/* #ifndef __MAKETA__  
  String	El_GetStr_(const void* it) const	{ return SC((*((T*)it))); } // #IGNORE
  void		El_SetFmStr_(void* it, const String& val) { *((T*)it) = (T)SC(val); } // #IGNORE
//  String	El_GetStr_(const void* it) const	{ return String(*((T*)it)); } // #IGNORE
//  void		El_SetFmStr_(void* it, const String& val) { *((T*)it) = (T)val; } // #IGNORE
#endif */
  taPlainArray(int init_alloc)			{el = NULL; Alloc(init_alloc); }
  taPlainArray()				{el = NULL;}
  taPlainArray(const taPlainArray<T>& cp)	{el = NULL; Alloc(cp.size); Duplicate(cp); }
  virtual ~taPlainArray()			{ SetArray_(NULL); } //

  ////////////////////////////////////////////////
  // 	functions that return the type		//
  ////////////////////////////////////////////////

  T&		SafeEl(int i) const
  { T* rval=(T*)&err; i=Index(i); if((i >= 0) && (i < size)) rval=&(el[i]); return *rval; }
  // #CAT_Access the element at the given index (index can be - meaning from end of list)
  T&		FastEl(int i)		{ return el[i]; }
  // #CAT_Access fast element (no range checking)
  const T&	FastEl(int i) const	{ return el[i]; }
  // #CAT_Access fast element (no range checking)
  T&		RevEl(int idx) const		{ return SafeEl(size - idx - 1); }
  // #CAT_Access reverse (index) element (ie. get from the back of the list first)
  T&		operator[](int i) const		{ return el[i]; }
  void		operator=(const taPlainArray<T>& cp)	{ Reset(); Duplicate(cp); }

  virtual T	Pop()
  { T* rval=(T*)&err; if(size>0) rval=&(el[--size]); return *rval; }
  // #CAT_Modify pop the last item in the array off
  virtual T&	Peek() const
  { T* rval=(T*)&err; if(size>0) rval=&(el[size-1]); return *rval; }
  // #CAT_Modify peek at the last item on the array

  ////////////////////////////////////////////////
  // 	functions that are passed el of type	//
  ////////////////////////////////////////////////

  virtual void	Set(int i, const T& item) 	{ SafeEl(i) = item; }
  // #CAT_Modify use this for assigning values to items in the array (Set should update if needed)
  virtual void	Add(const T& item)		{ Add_((void*)&item); }
  // #CAT_Modify #MENU add the item to the array
  virtual bool	AddUnique(const T& item)	{ return AddUnique_((void*)&item); }
  // #CAT_Modify add the item to the array if it isn't already on it, returns true if unique
  virtual void	Push(const T& item)		{ Add(item); }
  // #CAT_Modify push the item on the end of the array (same as add)
  virtual void	Insert(const T& item, int idx, int n_els=1) { Insert_((void*)&item, idx, n_els); }
  // #CAT_Modify #MENU Insert (n_els) item(s) at idx (-1 for end) in the array
  virtual int	FindEl(const T& item, int i=0) const { return FindEl_((void*)&item, i); }
  // #CAT_Access #MENU #USE_RVAL Find item starting from idx in the array (-1 if not there)
  virtual bool	RemoveEl(const T& item)		{ return RemoveEl_((void*)&item); }
  // #CAT_Modify remove given item, returns success
protected:
  T		tmp;
  override void*	MakeArray_(int n) const	{ return new T[n]; }
  override void		SetArray_(void* nw) {if (el) delete [] el; el = (T*)nw;}
};


class TA_API  taFixedArray_impl {
  // #VIRT_BASE ##NO_INSTANCE #NO_TOKENS basic subtype for FixedArray, no tokens of which are ever kept
public:
  int 		size;			// #NO_SAVE #READ_ONLY number of elements in the array

  taFixedArray_impl()			{size = 0;}
  virtual ~taFixedArray_impl()		{size = 0;} 

  inline int		Index(int idx) const { if(idx < 0) idx += size; return idx; }
  // #CAT_XpertAccess get actual index from index value that can also be negative, meaning access from end of list
  inline bool		InRange(int idx) const {return ((idx >= 0) && (idx < size));}
  // #CAT_XpertAccess is the specified index within range of 0 >= idx < size
  virtual void		Reset()		{SetSize(0);}
  // #CAT_Modify
 
  ////////////////////////////////////////////////
  // functions that don't depend on the type	//
  ////////////////////////////////////////////////

  virtual void	SetSize(int sz);
  // #CAT_Modify #MENU #MENU_ON_Edit #INIT_ARGVAL_ON_size force array to be of given size by inserting blanks or removing

public: // accessible but generally not used implementation overrides
  virtual const void*	SafeEl_(int i) const;
  // #IGNORE element at posn; i is allowed to be out of bounds
  virtual void*		FastEl_(int) = 0;
  // #IGNORE element at posn; i must be in bounds 

protected: 
  virtual bool		Alloc_(uint n); // set capacity to n
  virtual void*		MakeArray_(int i) const = 0; // #IGNORE make a new array of item type
  virtual void		SetArray_(void* nw) = 0;

// compulsory element accessor and manip functions 
  virtual const void*	FastEl_(int i) const 
    {return const_cast<taFixedArray_impl*>(this)->FastEl_(i);}
  // #IGNORE element at posn; i must be in bounds 
  
  virtual bool		El_Equal_(const void*, const void*) const = 0;
  // #IGNORE for finding
  virtual const void*	El_GetBlank_() const = 0;
  // #IGNORE address of a blank element, for initializing empty items -- can be STATIC_CONST
  // NOTE: this can be implemented by clearing the tmp item, then returning that addr
  virtual const void*	El_GetErr_() const	{ return El_GetBlank_();}
  // #IGNORE address of an element to return when out of range -- defaults to blank el
  virtual void		El_Copy_(void*, const void*) = 0;
  // #IGNORE
  virtual uint		El_SizeOf_() const = 0;
  // #IGNORE size of element
  
  virtual void		Add_(const void* it); // #IGNORE
  virtual bool		AddUnique_(const void* it);		// #IGNORE
  void			Copy_(const taFixedArray_impl& cp);
  // replace our array with the source items
  virtual void		Insert_(const void* it, int where, int n=1); // #IGNORE
  virtual bool		Equal_(const taFixedArray_impl& src) const; 
    // 'true' if not null, and same size and els
  virtual int		Find_(const void* it, int where=0) const; 	// #IGNORE -- based on El_Equal_; default is linear
  virtual void		InitVals_(const void* it, int start=0, int end=-1);// #IGNORE
  virtual void		ReclaimOrphans_(int start, int end) {}// #IGNORE called when array is shortened, leaving orphaned values; note 'size' may already be trimmed: NOT called when el[] is replaced
  
};

#define TA_FIXED_ARRAY_FUNS(y,T) \
public: \
  STATIC_CONST T blank; \
  explicit y(int init_size) {SetSize(init_size); } \
  y(int init_size, const T& i0) {SetSize(init_size); el[0] = i0;} \
  y(int init_size, const T& i1, const T& i0) \
    {SetSize(init_size); el[0] = i0; el[1] = i1;} \
  y(int init_size, const T& i2, const T& i1, const T& i0) \
    {SetSize(init_size); el[0] = i0; el[1] = i1; el[2] = i2;} \
  y(int init_size, const T& i3, const T& i2, const T& i1, const T& i0) \
    {SetSize(init_size); el[0] = i0; el[1] = i1; el[2] = i2; el[3] = i3;} \
  y(int init_size, T init[]) {SetSize(init_size); \
    for (int j = 0; j < init_size; ++j) el[j] = init[j];} \
  y() {} \
  y(const y& cp) {Copy(cp); } \
  T&		operator[](int i) { return el[i]; } \
  const T&	operator[](int i) const	{ return el[i]; } \
  y& operator=(const y& cp) {Copy(cp); return *this;} \
  bool 		operator==(const y& src) const {return Equal_(src);} \
protected: \
  override const void*	El_GetBlank_() const	{ return (const void*)&blank; }

template<class T> 
class taFixedArray : public taFixedArray_impl {
  // #INSTANCE #NO_TOKENS rudimentary array, primarily intended as an OO replacement for C arrays
public:
  T*		el;		// #HIDDEN #NO_SAVE Pointer to actual array memory

  taFixedArray()	{el = NULL;}
  virtual ~taFixedArray()	{ SetArray_(NULL); } //

  ////////////////////////////////////////////////
  // 	functions that return the type		//
  ////////////////////////////////////////////////

  const T& SafeEl(int i) const { return *(static_cast<const T*>(SafeEl_(i))); }
  // the element at the given index
  const T&	FastEl(int i) const	{ return el[i]; }
  // fast element (no range checking)
  T&		FastEl(int i) 		{ return el[i]; }
  // fast element (no range checking)
  void		Copy(const taFixedArray<T>& cp)	{Copy_(cp);} // #IGNORE buggy maketa

  ////////////////////////////////////////////////
  // 	functions that are passed el of type	//
  ////////////////////////////////////////////////

  virtual void	Set(int i, const T& item) 	{ FastEl(i) = item; }
  // use this for assigning values to items in the array (Set should update if needed)
  virtual void	Add(const T& item)		{ Add_((const void*)&item); }
  // #MENU add the item to the array
  virtual bool	AddUnique(const T& item)	{ return AddUnique_((void*)&item); }
  // add the item to the array if it isn't already on it, returns true if unique
  virtual void	Insert(const T& item, int idx, int n_els=1) 
  { Insert_((void*)&item, idx, n_els); }
  // #MENU Insert (n_els) item(s) at idx (-1 for end) in the array
  virtual int	Find(const T& item, int i=0) const { return Find_((void*)&item, i); }
  // #MENU #USE_RVAL Find item starting from idx in the array (-1 if not there)
public:
  override void*	FastEl_(int i)		{ return &(el[i]); } 
  override const void*	FastEl_(int i) const  	{ return (const void*)&(el[i]); }
protected:
  override void*	MakeArray_(int n) const	{ return new T[n]; }
  override void		SetArray_(void* nw) {if (el) delete [] el; el = (T*)nw;}
  override bool		El_Equal_(const void* a, const void* b) const
    { return (*((T*)a) == *((T*)b)); }
  override void		El_Copy_(void* to, const void* fm) { *((T*)to) = *((T*)fm); } 
  override uint		El_SizeOf_() const	{ return sizeof(T); }
};

class TA_API int_FixedArray: public taFixedArray<int> {
  // #INLINE #INLINE_DUMP #NO_TOKENS
  TA_FIXED_ARRAY_FUNS(int_FixedArray, int)
};

class TA_API String_PArray : public taPlainArray<String> {
  // #NO_TOKENS a plain-array of strings
INHERITED(taPlainArray<String>)
public:
  static const String	def_sep; // ", "
  
  int	FindContains(const String& op, int start=0) const;
  // find item that contains string -- start < 0 = start from end of array (not strings!)
  int	FindStartsWith(const String& op, int start=0) const;
  // find item that starts with string -- start < 0 = start from end of array (not strings!)

  const String 	AsString(const String& sep = def_sep) const;
  void		SetFromString(String str, const String& sep = def_sep);
  void	operator=(const String_PArray& cp)	{ Copy_Duplicate(cp); }
  String_PArray()				{ };
  String_PArray(const String_PArray& cp)	{ Copy_Duplicate(cp); }
  // returns first item which contains given string (-1 if none)
protected:
  int		El_Compare_(const void* a, const void* b) const
  { int rval=-1; if(*((String*)a) > *((String*)b)) rval=1; else if(*((String*)a) == *((String*)b)) rval=0; return rval; }
  bool		El_Equal_(const void* a, const void* b) const
    { return (*((String*)a) == *((String*)b)); }
  String	El_GetStr_(const void* it) const { return (*((String*)it)); }
  void		El_SetFmStr_(void* it, const String& val)
  {*((String*)it) = val; }
};

class TA_API int_PArray: public taPlainArray<int> {
  // #NO_TOKENS a plain-array of ints
public:
  void	InitVals(int val = 0)
  { for(int i=0; i<size; i++) FastEl(i) = val; }

  void	operator=(const int_PArray& cp)	{ Copy_Duplicate(cp); }
  int_PArray()				{ };
  int_PArray(const int_PArray& cp)	{ Copy_Duplicate(cp); }

protected:
  int		El_Compare_(const void* a, const void* b) const
  { int rval=-1; if(*((int*)a) > *((int*)b)) rval=1; else if(*((int*)a) == *((int*)b)) rval=0; return rval; }
  bool		El_Equal_(const void* a, const void* b) const
    { return (*((int*)a) == *((int*)b)); }
  String	El_GetStr_(const void* it) const { return (*((int*)it)); }
  void		El_SetFmStr_(void* it, const String& val)
  { int tmp = (int)val; *((int*)it) = tmp; }
};

#ifdef __MAKETA__
class TA_API void_PArray {
#else
class TA_API void_PArray: public taPlainArray<void*> {
#endif
  // #NO_TOKENS #NO_MEMBERS #NO_CSS a plain-array of void* pointers
public:
  void	operator=(const void_PArray& cp)	{ Copy_Duplicate(cp); }
  void_PArray()				{ };
  void_PArray(const void_PArray& cp)	{ Copy_Duplicate(cp); }

protected:
  int		El_Compare_(const void* a, const void* b) const
  { int rval=-1; if(*((void**)a) > *((void**)b)) rval=1; else if(*((void**)a) == *((void**)b)) rval=0; return rval; }
  bool		El_Equal_(const void* a, const void* b) const
    { return (*((void**)a) == *((void**)b)); }
  String	El_GetStr_(const void* it) const { return *((ta_uintptr_t*)it); }
  void		El_SetFmStr_(void* it, const String& val)
  { ta_uintptr_t tmp = (ta_uintptr_t)val; *((ta_uintptr_t*)it) = tmp; }
  //note: dangerous, but needed to clear (if str empty)
};

/////////////////////////////////////////////// 
// String-Based Text Diff Algorithm

// C++ version of C# code by Matthias Hertel: 
// http://www.mathertel.de/Diff
// see .cpp file for more information

class TA_API taStringDiffItem {
  // ##NO_TOKENS one item of difference between the strings
public:
  int 		start_a;     	// start line number in string A
  int 		start_b;     	// start line number in string B
  int 		delete_a;    	// number of lines deleted in string A
  int 		insert_b;    	// number of lines inserted in string B
  String	insert_b_str;	// actual string value of lines inserted into string B

  taStringDiffItem() { start_a = start_b = delete_a = insert_b = 0; }

  bool operator==(taStringDiffItem& cmp) { return start_a == cmp.start_a; }
  bool operator>(taStringDiffItem& cmp) { return start_a > cmp.start_a; }
  bool operator<(taStringDiffItem& cmp) { return start_a < cmp.start_a; }
};

class TA_API taStringDiffItem_PArray : public taPlainArray<taStringDiffItem> {
  // #NO_TOKENS a plain-array of string diff items
INHERITED(taPlainArray<taStringDiffItem>)
public:

  void	operator=(const taStringDiffItem_PArray& cp)	{ Copy_Duplicate(cp); }
  taStringDiffItem_PArray()				{ };
  taStringDiffItem_PArray(const taStringDiffItem_PArray& cp)	{ Copy_Duplicate(cp); }
protected:
  String	El_GetStr_(const void* it) const { return _nilString; }
  void		El_SetFmStr_(void* it, const String& val) {  };
};

class TA_API taStringDiffData {
  // ##NO_TOKENS #IGNORE one data record for each String item being compared in taStringDiff
public:
  int 		lines;		// Number of elements (lines)
  int_PArray 	data; 	 	// Buffer of numbers that will be compared (hash codes of items).
  int_PArray 	modified;	// bit-mapped flags for modification, which is the basic result of the diff -- if set for dataA it means deleted and for dataB it means inserted -- has 2 extra bits in it.
  int_PArray 	line_st; 	// starting position within data string for each line
  
  inline void SetModified(int idx, bool flag)
  { int aidx = idx / sizeof(int);  int bit = idx % sizeof(int); int mask = 1 << bit;
    int& curval = modified.FastEl(aidx); if(flag) curval |= mask; else curval &= ~mask; }

  inline bool GetModified(int idx)
  { int aidx = idx / sizeof(int);  int bit = idx % sizeof(int); int mask = 1 << bit;
    int curval = modified.FastEl(aidx); return curval & mask; }

  inline void AllocModified()
  { int trg_n = (lines+2) / sizeof(int); modified.SetSize(trg_n+1); }

  inline void InitFmData()
  { lines = data.size; AllocModified(); modified.InitVals(0); }

  String      GetLine(const String& str, int st_ln, int ed_ln=-1)
  { int st = line_st[st_ln];  if(ed_ln < 0) ed_ln = st_ln;
    int ed; if(ed_ln == line_st.size-1) ed = str.length()-1; else ed = line_st[ed_ln+1]-1;
    return str.at(st, ed-st);
  }

  void	Reset()
  { lines = 0; data.Reset(); modified.Reset(); line_st.Reset(); }


  taStringDiffData() { lines = 0; }
};

class TA_API taStringDiffEdits {
  // #NO_TOKENS a set of diffs between string A and string B -- can be used to convert string A into string B, given only string A and these edits
public:
  taStringDiffItem_PArray	diffs; 	// the raw diffs
  int_PArray 			line_st; // line starting positions for string A

  String      GetLine(const String& str, int st_ln, int ed_ln=-1)
  { int st = line_st[st_ln];  if(ed_ln < 0) ed_ln = st_ln;
    int ed; if(ed_ln == line_st.size-1) ed = str.length()-1; else ed = line_st[ed_ln+1]-1;
    return str.at(st, ed-st);
  }

  String	GenerateB(const String& str_a);
  // generate (return value) from str_a and the stored diff information

  String 	GetDiffStr(const String& str_a);
  // get a string representation of the diffs -- if str_a is passed, then it produces normal diff format, otherwise a bit more minimal

  int GetLinesChanged();
  // get total count of lines changed (inserts + deletes) in the diffs

};

class TA_API taStringDiff {
  // #NO_TOKENS computes differences between strings, on a line-by-line basis, using the SES/LCS algorithm of Myers (1986), which is used by diff -- converts strings to unique int hash codes first
public:
  enum	OutputFmt {		// format to output diff information ifor GetDiffStr function
    NORMAL,			// default output of the unix diff utility
    CONTEXT,			// context diff -- shows some preceeding and trailing lines
  };

  taStringDiffItem_PArray	diffs;
  // #READ_ONLY the resulting differences, as a record of changes
  taHashTable			hash_codes;
  // #READ_ONLY hash codes used to convert lines into unique integer numbers, which are then used for diff op
  int				cur_hash_idx;
  // #READ_ONLY current hash value index -- just counts up as hash guys are added
  taStringDiffData		data_a;
  // #READ_ONLY data for first string
  taStringDiffData		data_b;
  // #READ_ONLY data for second string
  int_PArray			down_vector;
  // #READ_ONLY vector for the (0,0) to (x,y) search
  int_PArray			up_vector;
  // #READ_ONLY vector for the (u,v) to (N,M) search

  void	DiffStrings(const String& str_a, const String& str_b,
	    bool trimSpace = false, bool ignoreSpace = false, bool ignoreCase = false); 
  // computes the difference between two Strings, in terms of lines (as in the usual line-based diff utility) -- trimSpace removes leading and trailing space from lines, ignoreSpace ignores all space chars entirely, and ignoreCase downcases everything before comparing
  void ReDiffB(const String& str_a, const String& str_b,
	    bool trimSpace = false, bool ignoreSpace = false, bool ignoreCase = false); 
  // *after* running previously on str_a and an previous str_b, you can now run the diff again on a *new* str_b -- this will save a fair amount of setup computation on str_a and the hash code table -- str_a *must* be the same as last time obviously..
  bool DiffFiles(const String& fname_a, const String& fname_b,
			 String& str_a, String& str_b,
	 bool trimSpace = false, bool ignoreSpace = false, bool ignoreCase = false);
  // loads in two files into the given str_a,b strings, based on the given file names, then runs DiffStrings on them -- trimSpace removes leading and trailing space from lines, ignoreSpace ignores all space chars entirely, and ignoreCase downcases everything before comparing

  void	GetEdits(taStringDiffEdits& edits);
  // *after* running DiffStrings, call this to store the edits so that str_b can be reconstructed from str_b + the diffs..

  String GetDiffStr(const String& str_a, const String& str_b, OutputFmt fmt = NORMAL);
  // *after* running DiffStrings, call this to get a string representation of the differences, using the specified format

  String GetDiffStr_normal(const String& str_a, const String& str_b);
  // #IGNORE
  String GetDiffStr_context(const String& str_a, const String& str_b);
  // #IGNORE

  int GetLinesChanged();
  // *after* running DiffStrings, call this to get total count of lines changed (inserts + deletes) in the diffs

  void	DiffInts(const int_PArray& array_a, const int_PArray& array_b); 
  // computes the difference between two arrays of integers -- underlying algorithm uses ints so this is easy..

  void	GetLines(taStringDiffData& ddata, const String& str);
  // find line starting positions in the text strings

  void DiffCodes(taStringDiffData& ddata, const String& str, 
	 bool trimSpace = false, bool ignoreSpace = false, bool ignoreCase = false);
  // convert text string into integer codes

  void 	Optimize(taStringDiffData& ddata);
  // optimize sequences of modified strings to make more readable

  void	CreateDiffs(const String& str_a, const String& str_b);
  // create diff list in diffs based on results of LCS/SMS computation, stored in data_a and B

  void	Reset();
  // reset all the current data settings

protected:
  void SMS(int& sms_x, int& sms_y, int lower_a, int upper_a, int lower_b, int upper_b);
  // This is the algorithm to find the Shortest Middle Snake (SMS).
  void LCS(int lower_a, int upper_a, int lower_b, int upper_b);
  // recursive divide-and-conquer routine -- operates on data_a,b and down/up_vector structures
};


#endif // ta_list_h
