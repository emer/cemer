/* -*- C++ -*- */
/*=============================================================================
//									      //
// This file is part of the TypeAccess/C-Super-Script software package.	      //
//									      //
// Copyright (C) 1995 Randall C. O'Reilly, Chadley K. Dawson, 		      //
//		      James L. McClelland, and Carnegie Mellon University     //
//     									      //
// Permission to use, copy, modify, and distribute this software and its      //
// documentation for any purpose is hereby granted without fee, provided that //
// the above copyright notice and this permission notice appear in all copies //
// of the software and related documentation.                                 //
// 									      //
// Note that the PDP++ software package, which contains this package, has a   //
// more restrictive copyright, which applies only to the PDP++-specific       //
// portions of the software, which are labeled as such.			      //
//									      //
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

#ifndef ta_list_h
#define ta_list_h 1

#include "ta_def.h"
#include "ta_string.h"

// this is the amount of overhead (in 4 byte units) allowed by the alloc
// routines, which allocate a power of two minus this amount
#define TA_ALLOC_OVERHEAD 2


// externals
class TypeDef; //#IGNORE
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
enum DataChangedReason { /* reason why DataChanged being called, as well as defining ops (also used by taBase and other classes) --
 some data change operations will emit multiple DataChanged calls */
  DCR_UPDATE_AFTER_EDIT, // after user edits (or load) ex. taBase::UpdateAfterEdit call; ops not used
  DCR_UPDATE_VIEWS, 	// after data has changed ex. taBase::UpdateAllViews call; ops not used

  DCR_LIST_INIT,
  DCR_LIST_ITEM_INSERT,	// op1=item, op2=item_after, null=at beginning
  DCR_LIST_ITEM_REMOVE,	// op1=item -- note, item not DisOwned yet, but has been removed from list
  DCR_LIST_ITEM_MOVED,	// op1=item, op2=item_after, null=at beginning
  DCR_LIST_ITEMS_SWAP,	// op1=item1, op2=item2
  DCR_LIST_SORTED,	// after sorting; ops not used

  DCR_GROUP_INSERT,	// op1=group, op2=group_after, null=at beginning
  DCR_GROUP_REMOVE,	// op1=group -- note, item not DisOwned yet, but has been removed from list
  DCR_GROUP_MOVED,	// op1=group, op2=group_after, null=at beginning
  DCR_GROUPS_SWAP,	// op1=group1, op2=group2

  DCR_GROUP_ITEM_INSERT,	// op1=item, op2=item_after, null=at beginning
  DCR_GROUP_ITEM_REMOVE,	// op1=item -- note, item not DisOwned yet, but has been removed from list
  DCR_GROUP_ITEM_MOVED,	// op1=item, op2=item_after, null=at beginning
  DCR_GROUP_ITEMS_SWAP,	// op1=item1, op2=item2
  DCR_GROUP_LIST_SORTED,	// after sorting; ops not used

  DCR_STRUCT_UPDATE_BEGIN, // for some updating, like doing Layer->Build, better for gui to just do one
  DCR_STRUCT_UPDATE_END,  // update operation at the end of everything
  DCR_DATA_UPDATE_BEGIN, // for some data changes, like various log updates, better for gui to just do one
  DCR_DATA_UPDATE_END  // update operation at the end of everything

#ifndef __MAKETA__
 ,DCR_LIST_MIN		= DCR_LIST_INIT,
  DCR_LIST_MAX		= DCR_LIST_SORTED,
  DCR_GROUP_MIN		= DCR_GROUP_INSERT,
  DCR_GROUP_MAX		= DCR_GROUPS_SWAP,
  DCR_GROUP_ITEM_MIN	= DCR_GROUP_ITEM_INSERT,
  DCR_GROUP_ITEM_MAX	= DCR_GROUP_LIST_SORTED
#endif
};

/* DataChangedReason Notes

   GROUP_xx notifications are simply the LIST_ITEM notifications of the gp list, passed on to the
   owning Group as GROUP_ ops (by adding the DCR_List_Group_Offset to the op code)

*/
#define DCR_LIST_ITEM_MIN 	DCR_LIST_ITEM_INSERT
#define DCR_LIST_ITEM_MAX 	DCR_LIST_ITEMS_SWAP
#define DCR_List_Group_Offset	(DCR_GROUP_INSERT - DCR_LIST_ITEM_INSERT)

typedef int taListItr; // pseudo class, compatible with the FOR_ITR_EL macro in ta_group

typedef unsigned long taHashVal;

class  taPtrList_impl { // ##NO_TOKENS implementation of the pointer list class
protected:
//  static String		 no_el_name;	// when the el has no name..
  static taPtrList_impl scratch_list;	// a list for any temporary processing needs

  static taHashVal	HashCode_String(const char* string);
  // get a hash code value from given string
  static taHashVal	HashCode_Ptr(const void* ptr);
  // get a hash code value from given ptr

  // these define what to do with an individual item (overload them!)
  virtual String GetListName_() const 		{ return _nilString; }
  // name of the list
  virtual String El_GetName_(void*) const 	{ return _nilString; }
  // gets name from an element (for list)
  virtual taHashVal El_GetHashVal_(void* it) const;
  // gets hash code based on key type in hash table; default is for string-key'ed lists (v3.2 default)
  virtual TALPtr El_GetOwner_(void*) const	{ return (TALPtr)this; }
  // who owns the el?
  virtual void*	El_SetOwner_(void* it) 		{ return it; }
  // set owner to this
  virtual void	El_SetIndex_(void*, int) 	{ };
  // sets the element's self-index
  virtual bool  El_FindCheck_(void* it, const char* nm) const
  { return (El_GetName_(it) == nm); }
  virtual int	El_Compare_(void* a, void* b) const
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
  // how to copy from given item (2nd arg)

  void		InitList_();
  virtual int	Scratch_Find_(const char* it) const;
  // find item on the scratch_list using derived El_FindCheck_()
  void		UpdateIndex_(int idx);
  // update the index of the item at given index (i.e., which was just moved)

public:
  void**	el;		// #READ_ONLY #NO_SAVE #HIDDEN the elements themselves
  int 		alloc_size;	// #READ_ONLY #NO_SAVE allocation size
  taHashTable*	hash_table;	// #READ_ONLY #NO_SAVE #HIDDEN a hash table (NULL if not used)
  int		size;		// #READ_ONLY #NO_SAVE #SHOW number of elements in the list

  taPtrList_impl()			{ InitList_(); }
  taPtrList_impl(const taPtrList_impl& cp)	{ InitList_(); Duplicate(cp); }
  virtual ~taPtrList_impl();

  static ostream& Indenter(ostream& strm, const char* itm, int no, int prln, int tabs);
  static int	Idx;			// #HIDDEN pass to find if you don't want one


  ////////////////////////////////////////////////
  // 	functions that return the type		//
  ////////////////////////////////////////////////

  void*   	GetTA_Element_(int i, TypeDef*& eltd)
  { eltd = GetElType(); return FastEl_(i); }
  void*		SafeEl_(int i) const
  { void* rval=NULL; if((i >= 0) && (i < size)) rval = el[i]; return rval; } 	// #IGNORE
  void*		FastEl_(int i)	const	{ return el[i]; } 	// #IGNORE
  virtual void*	FindName_(const char* it, int& idx=Idx) const;	// #IGNORE
  virtual void*	Pop_();					// #IGNORE
  void*		Peek_() const
  { void* rval=NULL; if(size > 0) rval = el[size-1]; return rval; }  // #IGNORE

  virtual void*	AddUniqNameOld_(void* it);
  // #IGNORE add so that name is unique, old used if dupl, returns one used

  virtual void*	LinkUniqNameOld_(void* it);		// #IGNORE


  ////////////////////////////////////////////////
  // 	functions that are passed el of type	//
  ////////////////////////////////////////////////

  virtual int	Find_(const void* it) const; 			// #IGNORE

  virtual void	AddEl_(void* it);
  // #IGNORE just puts the el on the list, doesn't do anything else
  virtual void	Add_(void* it);				// #IGNORE
  virtual bool	AddUnique_(void* it);			// #IGNORE
  virtual bool	AddUniqNameNew_(void* it);
  // #IGNORE add a new object so that list only has one, new replaces existing
  virtual bool	DuplicateEl_(void* it);
  // #IGNORE duplicate given item and add to list
  virtual bool	Insert_(void* it, int where);		// #IGNORE
  virtual bool 	Replace_(void* ol, void* nw);		// #IGNORE
  virtual bool 	Replace_(const char* ol, void* nw);	// #IGNORE
  virtual bool 	Replace_(int ol, void* nw);		// #IGNORE
  virtual bool 	Transfer_(void* it);			// #IGNORE
  virtual bool	Remove_(void* it);			// #IGNORE
  virtual bool	MoveBefore_(void* trg, void* item); 	// #IGNORE
  virtual bool	MoveAfter_(void* trg, void* item); 	// #IGNORE

  virtual void 	Link_(void* it);			// #IGNORE
  virtual bool	LinkUnique_(void* it); 			// #IGNORE
  virtual bool	LinkUniqNameNew_(void* it);		// #IGNORE
  virtual bool	InsertLink_(void* it, int where);	// #IGNORE
  virtual bool 	ReplaceLink_(void* ol, void* nw);	// #IGNORE
  virtual bool 	ReplaceLink_(const char* ol, void* nw);	// #IGNORE
  virtual bool 	ReplaceLink_(int ol, void* nw);		// #IGNORE

  virtual void	Push_(void* it);			// #IGNORE

  ////////////////////////////////////////////////
  // functions that don't depend on the type	//
  ////////////////////////////////////////////////

  virtual TypeDef* 	GetElType() const {return NULL;}		// #IGNORE Default type for objects in group
  virtual taDataLink** GetDataLinkRef() {return NULL;}
  // #IGNORE address in which IDataLinks pointer is stored
  virtual void	DataChanged(int dcr, void* op1 = NULL, void* op2 = NULL); // called when list has changed -- more fine-grained than Dirty(), and may be multiple calls per event
  virtual void	Dirty()		{ };	// #IGNORE what to do when the list is modified (called after all DataChanged() are issued)
  virtual void	Alloc(int sz);
  // allocate a list big enough for given number of elements (or current size)
  virtual void 	Reset()			{ RemoveAll(); }
  // reset the list (remove all elements)
  virtual bool	IsEmpty()	{ return (size == 0) ? true : false; }

  virtual void	BuildHashTable(int n_buckets);
  // build a hash table with given number of buckets (not dynamic, so make it big)

  virtual int	Find(const char* nm) const;
  // find named element in list

  virtual bool	Remove(const char* item_nm);
  virtual bool	Remove(int idx);
  // remove (and delete) element from list at index
  virtual bool	RemoveName(const char* item_nm)		{ return Remove(item_nm); }
  // remove given named element from list (if on list)
  virtual bool	RemoveLast();
  // remove the last element on the list
  virtual void	RemoveAll();
  // #MENU #MENU_ON_Edit #CONFIRM #UPDATE_MENUS Remove all elements on the list

  virtual bool	Move(int from, int to);
  // Move element from (from) to position (to) in list
  virtual bool	Swap(int pos1, int pos2);
  // Swap the elements in the two given positions on the list

  virtual void	PopAll();
  // pop all elements off the stack

  virtual void	Permute();
  // #MENU #CONFIRM #UPDATE_MENUS permute the items in the list into a random order
  virtual void	Sort(bool descending=false);
  // #MENU #CONFIRM #UPDATE_MENUS sort the items in the list in alpha order according to name (or El_Compare_)
  virtual void	Sort_(bool descending=false);	// implementation of sorting function
  virtual void 	UpdateAllIndicies();	// update all indices of elements in list


  void*		FirstEl(taListItr& itr) {itr = 0; return SafeEl_(0);} // for ta_group::FOR_ITR_EL macro
  void*		NextEl(taListItr& itr) {return (++itr < size) ? FastEl_(itr) : NULL;} // for ta_group::FOR_ITR_EL macro
  /////////////////////////////////////////////////////////////////////////
  // replicating items: either by clone/add (duplicate) or link (borrow) //
  /////////////////////////////////////////////////////////////////////////

  void	Duplicate(const taPtrList_impl& cp);
  // duplicate (clone & add) elements of given list into this one
  void	DupeUniqNameNew(const taPtrList_impl& cp);
  // duplicate so result is unique names, replacing with new ones where dupl
  void	DupeUniqNameOld(const taPtrList_impl& cp);
  // duplicate so result is unique names, using old ones where dupl

  void	Stealth_Borrow(const taPtrList_impl& cp);
  // borrow without referencing the borrowed elements (i.e. use AddEl_())
  void	Borrow(const taPtrList_impl& cp);
  // borrow (link) elements of given list into this one
  void	BorrowUnique(const taPtrList_impl& cp);
  // borrow so result is unique list
  void	BorrowUniqNameNew(const taPtrList_impl& cp);
  // borrow so result is unique names, replacing with new ones where dupl
  void	BorrowUniqNameOld(const taPtrList_impl& cp);
  // borrow so result is unique names, using old ones where dupl

  /////////////////////////////////
  // copying items between lists //
  /////////////////////////////////

  void	Copy_Common(const taPtrList_impl& cp);
  // apply copy operator to only those items in common between the two lists
  void	Copy_Duplicate(const taPtrList_impl& cp);
  // apply copy operator to items, use duplicate to add new ones from cp (if necc)
  void	Copy_Borrow(const taPtrList_impl& cp);
  // apply copy operator to items, use borrow to add new ones from cp (if necc)

  // browsing -- browse client lists must override
  virtual int		NumListCols() const {return 0;} // number of columns in a list view for this item type
  virtual String	GetColHeading(int col) const {return String("");} // header text for the indicated column
  virtual String	ChildGetColText(void* child, TypeDef* typ, int col, int itm_idx = -1) const {return String("");}
    // itm_idx is a hint from source, -1 means not specified or ignore

  // output
  virtual void 	List(ostream& strm=cout) const; 	// List the group items
};

template<class T> class taPtrList : public taPtrList_impl { // #INSTANCE
public:
  taPtrList()					{ };
  taPtrList(const taPtrList<T>& cp) 		{ Borrow(cp); }
  // borrow is guaranteed to work, others require EL_ functions..

  ////////////////////////////////////////////////
  // 	functions that return the type		//
  ////////////////////////////////////////////////

  // operators
  T*		SafeEl(int i) const		{ return (T*)SafeEl_(i); }
  // element at index
  T*		FastEl(int i) const		{ return (T*)el[i]; }
  // fast element (no range checking)
  T* 		operator[](int i) const		{ return (T*)el[i]; }
  void		operator=(const taPtrList<T>& cp) { Reset(); Borrow(cp); }
  // borrow is guaranteed to work, others require EL_ functions..

  T*		Edit_El(T* item) const		{ return SafeEl(Find(item)); }
  // #MENU #MENU_ON_Edit #USE_RVAL #ARG_ON_OBJ Edit given list item

  virtual T*	FindName(const char* item_nm, int& idx=Idx) const { return (T*)FindName_(item_nm, idx); }
  // find given named element (NULL = not here), sets idx

  virtual T*	Pop()				{ return (T*)Pop_(); }
  // pop the last element off the stack
  virtual T*	Peek() const			{ return (T*)Peek_(); }
  // peek at the last element on the stack

  virtual T*	AddUniqNameOld(T* item)		{ return (T*)AddUniqNameOld_((void*)item); }
  // add so that name is unique, old used if dupl, returns one used
  virtual T*	LinkUniqNameOld(T* item)		{ return (T*)LinkUniqNameOld_((void*)item); }
  // link so that name is unique, old used if dupl, returns one used

  ////////////////////////////////////////////////
  // 	functions that are passed el of type	//
  ////////////////////////////////////////////////

  virtual int	Find(const T* item) const	{ return Find_((const void*)item); }
  // find element in list (-1 if not there)
  virtual int	Find(const char* item_nm) const	{ return taPtrList_impl::Find(item_nm); }
  virtual int	FindEl(const T* item) const	{ return Find(item); }
  // find given element in list (-1 if not there)

  virtual void	AddEl(T* item)			{ AddEl_((void*)item); }
  // append a new pointer to end of list
  virtual void	Add(T* item)	      		{ Add_((void*)item); }
  // add element to the list and "own" item
  virtual bool	AddUnique(T* item)		{ return AddUnique_((void*)item); }
  // add so that object is unique, true if unique
  virtual bool	AddUniqNameNew(T* item)		{ return AddUniqNameNew_((void*)item); }
  // add so that name is unique, true if unique, new replaces existing

  virtual bool	Insert(T* item, int idx)	{ return Insert_((void*)item, idx); }
  // Add or insert element at idx (-1 for end)
  virtual bool 	Replace(T* old_it, T* new_it)		{ return Replace_((void*)old_it, (void*)new_it); }
  virtual bool 	Replace(const char* old_nm, T* new_it)	{ return Replace_(old_nm, (void*)new_it); }
  virtual bool 	Replace(int old_idx, T* new_it)	{ return Replace_(old_idx, (void*)new_it); }
  // replace element at index with the new one
  virtual bool 	ReplaceEl(T* old_it, T* new_it)		{ return Replace(old_it, new_it); }
  // replace given element with the new one
  virtual bool 	ReplaceName(const char* old_nm, T* new_it) { return Replace(old_nm, new_it); }
  // replace named element with the new one

  virtual bool	Remove(const char* item_nm) 	{ return taPtrList_impl::Remove(item_nm);}
  virtual bool	Remove(T* item)			{ return Remove_((void*)item); }
  virtual bool	Remove(int idx)			{ return taPtrList_impl::Remove(idx); }
  // Remove element at given index
  virtual bool	RemoveEl(T* item)		{ return Remove(item); }
  // #MENU #LABEL_Remove #ARG_ON_OBJ #UPDATE_MENUS Remove given item from list

  virtual bool 	DuplicateEl(T* item)		{ return DuplicateEl_((void*)item); }
  // #MENU #ARG_ON_OBJ #UPDATE_MENUS Duplicate given list item and Add to list

  virtual void 	Link(T* item)			{ Link_((void*)item); }
  // Link an item to list without owning it
  virtual bool	LinkUnique(T* item)		{ return LinkUnique_((void*)item); }
  // link so that object is unique, true if unique
  virtual bool	LinkUniqNameNew(T* item)		{ return LinkUniqNameNew_((void*)item); }
  // link so that name is unique, true if unique, new replaces existing
  virtual bool	InsertLink(T* item, int idx= -1) { return InsertLink_((void*)item, idx);}
  // #MENU #LABEL_Link #UPDATE_MENUS Insert a link at index (-1 for end)
  virtual bool 	ReplaceLink(T* old_it, T* new_it)	{ return ReplaceLink_((void*)old_it, (void*)new_it); }
  virtual bool 	ReplaceLink(const char* old_nm, T* new_it) { return ReplaceLink_(old_nm, (void*)new_it); }
  virtual bool 	ReplaceLink(int old_idx, T* new_it)	{ return ReplaceLink_(old_idx, (void*)new_it); }
  // replace element with a link to the new one
  virtual bool 	ReplaceLinkEl(T* old_it, T* new_it)	{ return ReplaceLink(old_it, new_it); }
  // replace given element (if on list) with the new one
  virtual bool	ReplaceLinkName(const char* old_nm, T* new_it){ return ReplaceLink(old_nm, new_it); }
  // replace given named element (if on list) with the new one

  virtual void	Push(T* item)			{ Push_((void*)item); }
  // push item on stack (for temporary use, not "owned")

  virtual bool 	MoveEl(T* from, T* to)		{ return Move(Find(from), Find(to)); }
  // #MENU #LABEL_Move #ARG_ON_OBJ #UPDATE_MENUS Move item (from) to position of (to)
  virtual bool 	Transfer(T* item)  		{ return Transfer_((void*)item); }
  // #MENU #MENU_ON_Edit #NO_SCOPE #UPDATE_MENUS Transfer item to this list

  virtual bool	MoveBefore(T* trg, T* item) { return MoveBefore_((void*)trg, (void*)item); }
  // move item so that it appears just before the target item trg in the list
  virtual bool	MoveAfter(T* trg, T* item) { return MoveAfter_((void*)trg, (void*)item); }
  // move item so that it appears just after the target item trg in the list
};


#define taPtrList_of(T)						      \
class T ## _List : public taPtrList<T> {				      \
protected:								      \
  void	El_Done_(void* item)	{ delete (T*)item; }			      \
public:                                                                       \
  ~ ## T ## _List()             { Reset(); }                                  \
}

// the hash table is for use in looking up things in PtrList's by name
// it contains hash codes and corresponding indicies for items in the list

class  taHashEl {
  // ##NO_TOKENS holds information for one entry of the hash table
public:
  taHashVal	hash_code;	// hash-coded value of name
  int		list_idx;	// index of item in list

  void	Initialize()	{ hash_code = 0; list_idx = -1; }
  taHashEl()		{ Initialize(); }
  taHashEl(taHashVal hash, int idx)	{ hash_code = hash; list_idx = idx; }
};

class  taHashBucket : public taPtrList<taHashEl> {
  // holds a set of hash table entries that all have the same hash_code modulo value
protected:
  void	El_Done_(void* it)	{ delete (taHashEl*)it; }
public:
  int		Find(taHashEl* itm) const
  { return taPtrList<taHashEl>::Find(itm); }
  int		Find(const char* itm) const
  { return taPtrList<taHashEl>::Find(itm); }

  int		Find(taHashVal hash) const;
  // find index of item (in the bucket) with given hash code value

  int		FindIndex(taHashVal hash) const;
  // find index of item (list_idx) with given hash code value

  ~taHashBucket()               { Reset(); }
};

class  taHashTable : public taPtrList<taHashBucket> {
  // table has a number of buckets, each with some hash values
protected:
  void	El_Done_(void* it)	{ delete (taHashBucket*)it; }
public:
  enum KeyType {
    KT_NAME  = 0, 	// use item name; the legacy default
    KT_PTR 		// use the item pointer
  };

  static int		n_bucket_primes[]; // prime numbers for number of buckets
  static int		n_primes;	 // number of prime numbers (86)

  int			bucket_max;	// maximum size of any bucket
  KeyType		key_type;

  void			Alloc(int sz);
  // allocate in prime-number increments
  void			RemoveAll();

  int			FindIndex(taHashVal hash) const;
  // find index from given hash value (-1 if not found)

  bool			UpdateIndex(taHashVal hash, int index);
  // update index associated with item
  bool			UpdateIndex(const char* string, int index)
  { return UpdateIndex(HashCode_String(string), index); }
  // update index associated with item

  void			Add(taHashBucket* itm)
  { taPtrList<taHashBucket>::Add(itm); }
  void 			Add(taHashVal hash, int index);
  // add a new item to the hash table

  bool			Remove(taHashBucket* itm)
  { return taPtrList<taHashBucket>::Remove(itm); }
  bool			Remove(int idx)
  { return taPtrList<taHashBucket>::Remove(idx); }
  bool			Remove(taHashVal hash);
  // remove given hash code from table
  bool			Remove(const char* string)
  { return Remove(HashCode_String(string)); }
  // remove given string from table


  void			InitList_();
  taHashTable()			{ key_type = KT_NAME; InitList_(); }
  taHashTable(const taHashTable& cp)	{ key_type = cp.key_type; InitList_(); Duplicate(cp); }
  ~taHashTable()                { Reset(); }
};


template<class T> class taPtrList_base : public taPtrList_impl { // #INSTANCE
public:
  taPtrList_base()					{ };
  taPtrList_base(const taPtrList_base<T>& cp) 		{ Duplicate(cp); };

  ////////////////////////////////////////////////
  // 	functions that are passed el of type	//
  ////////////////////////////////////////////////

  virtual int	Find(const char* item_nm) const	{ return taPtrList_impl::Find(item_nm); }
  virtual int	Find(const T* item) const	{ return Find_((const void*)item); }
  // find element in list (-1 if not there)
  virtual int	FindEl(T* item) const		{ return Find(item); }
  // find given element in list (-1 if not there)

  virtual void	AddEl(T* item)			{ AddEl_((void*)item); }
  // append a new pointer to end of list
  virtual void	Add(T* item)	      		{ Add_((void*)item); }
  // add element to the list and "own" item
  virtual bool	AddUnique(T* item)		{ return AddUnique_((void*)item); }
  // add so that object is unique, true if unique
  virtual bool	AddUniqNameNew(T* item)		{ return AddUniqNameNew_((void*)item); }
  // add so that name is unique, true if unique, new replaces existing

  virtual bool	Insert(T* item, int where)	{ return Insert_((void*)item, where); }
  // insert element at index (-1 for end)
  virtual bool 	Replace(T* old_it, T* new_it)		{ return Replace_((void*)old_it, (void*)new_it); }
  virtual bool 	Replace(const char* old_nm, T* new_it)	{ return Replace_(old_nm, (void*)new_it); }
  virtual bool 	Replace(int old_idx, T* new_it)		{ return Replace_(old_idx, (void*)new_it); }
  // replace element at index with the new one
  virtual bool 	ReplaceEl(T* old_it, T* new_it)		{ return Replace(old_it, new_it); }
  // replace given element with the new one
  virtual bool 	ReplaceName(const char* old_nm, T* new_it) { return Replace(old_nm, new_it); }
  // replace named element with the new one

  virtual bool	Remove(const char* item_nm) 	{ return taPtrList_impl::Remove(item_nm);}
  virtual bool	Remove(T* item)			{ return Remove_((void*)item); }
  virtual bool	Remove(int i)			{ return taPtrList_impl::Remove(i); }
  virtual bool	RemoveEl(T* item)		{ return Remove(item); }
  // #MENU #LABEL_Remove #ARG_ON_OBJ #UPDATE_MENUS Remove given item from list

  virtual bool 	DuplicateEl(T* item)		{ return DuplicateEl_((void*)item); }
  // #MENU #ARG_ON_OBJ #UPDATE_MENUS Duplicate given list item and Add to list

  virtual void 	Link(T* item)			{ Link_((void*)item); }
  // Link an item to list without owning it
  virtual bool	LinkUnique(T* item)		{ return LinkUnique_((void*)item); }
  // link so that object is unique, true if unique
  virtual bool	LinkUniqNameNew(T* item)		{ return LinkUniqNameNew_((void*)item); }
  // link so that name is unique, true if unique, new replaces existing
  virtual bool	InsertLink(T* item, int idx= -1)	{ return InsertLink_((void*)item, idx);}
  // #MENU #LABEL_Link #UPDATE_MENUS Insert a link at index (-1 for end)
  virtual bool 	ReplaceLink(T* old_it, T* new_it)	{ return ReplaceLink_((void*)old_it, (void*)new_it); }
  virtual bool 	ReplaceLink(const char* old_nm, T* new_it) { return ReplaceLink_(old_nm, (void*)new_it); }
  virtual bool 	ReplaceLink(int old_idx, T* new_it)	{ return ReplaceLink_(old_idx, (void*)new_it); }
  // replace element with a link to the new one
  virtual bool 	ReplaceLinkEl(T* old_it, T* new_it)	{ return ReplaceLink(old_it, new_it); }
  // replace given element (if on list) with the new one
  virtual bool	ReplaceLinkName(const char* old_nm, T* new_it){ return ReplaceLink(old_nm, new_it); }
  // replace given named element (if on list) with the new one

  virtual void	Push(T* item)			{ Push_((void*)item); }
  // push item on stack (for temporary use, not "owned")

  virtual bool 	MoveEl(T* from, T* to)		{ return Move(Find(from), Find(to)); }
  // #MENU #LABEL_Move #ARG_ON_OBJ #UPDATE_MENUS Move item (from) to position of (to)
  virtual bool 	Transfer(T* item)  		{ return Transfer_((void*)item); }
  // #MENU #MENU_ON_Edit #NO_SCOPE #UPDATE_MENUS Transfer element to this list

  virtual bool	MoveBefore(T* trg, T* item) { return MoveBefore_((void*)trg, (void*)item); }
  // move item so that it appears just before the target item trg in the list
  virtual bool	MoveAfter(T* trg, T* item) { return MoveAfter_((void*)trg, (void*)item); }
  // move item so that it appears just after the target item trg in the list
};


/* Array Notes
  Here is the class hierarchy for arrays:
  
  taFixedArray_impl -- basic data handling, semi-fixed size arrays
      - primarily for known-size use; doesn't have separate alloc from size
      - no removal-style ops
      - no ops that require a tmp item (ex. Move_)
      - no string conversions
      - no ordinal comparisons (ex. Sort not supported)
      
      FixedArray<T> -- concrete implementations of fixed arrays
      
    taBasicArray_impl -- basic data handling api
      - adds alloc for efficient dynamic data adding/removing
      
      taArray_impl -- advanced data handling
      - can overallocate
      - supports moving, sorting, etc.
      - supports string operations 
      - supports ordinal operations
      
      taMatrix_impl -- multi-dimensional data handling
      
      
        Matrix<T> -- ref-counted impl of Matrix_impl

  C++ rules for making a static template member are arcane, and seem not
  to work properly in our context. Therefore, arrays need to be created as
  subclasses of the template, and the additional routines/members are
  supplied by a macro of the same uppercase name as the template
*/


class  taFixedArray_impl {
  // #VIRT_BASE ##NO_INSTANCE ##NO_TOKENS basic shared subtype for Arrays and Matrixes, no tokens of which are ever kept
public:
  int 		size;			// #NO_SAVE #READ_ONLY number of elements in the array
  virtual int		alloc() {return size;}

  taFixedArray_impl()			{size = 0;}
  virtual ~taFixedArray_impl()		{size = 0;} 

  inline bool		InRange(int idx) const {return ((idx >= 0) && (idx < size));}
  virtual void		Reset()		{EnforceSize(0);}
 
  ////////////////////////////////////////////////
  // functions that don't depend on the type	//
  ////////////////////////////////////////////////

  virtual void	EnforceSize(int sz);
  // #MENU #MENU_ON_Edit force array to be of given size by inserting blanks or removing

  virtual void	AddBlank(int n_els = 1);
  // #MENU #MENU_ON_Edit Add n_els empty elements to the end of the array

public: // accessible but generally not used implementation overrides
  virtual const void*	SafeEl_(int i) const;
  // #IGNORE element at posn; i is allowed to be out of bounds
  virtual void*		FastEl_(int) = 0;
  // #IGNORE element at posn; i must be in bounds 

protected: 
  virtual bool 		AdjustAndSetAlloc_(uint& new_alloc) 
    {return (new_alloc > static_cast<uint>(size));} 
    // return 'true' if more storage needed; also tweaks size of storage based on a grow algorithm (none here in fixed base)
  virtual void		Alloc_(uint n); // set capacity to n
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
  virtual void		Copy_(const taFixedArray_impl& cp);
  // replace our array with the source items
  virtual void		Duplicate_(const taFixedArray_impl& cp);
  // append the source items to our array

  virtual void		Insert_(const void* it, int where, int n=1); // #IGNORE
  virtual bool		Equal_(const taFixedArray_impl& src) const; 
    // 'true' if not null, and same size and els
  virtual int		Find_(const void* it, int where=0) const; 	// #IGNORE -- based on El_Equal_; default is linear
  virtual void		InitVals_(const void* it, int start=0, int end=-1);// #IGNORE
  
  // Aggregate-maintenance helper routines
  virtual void		ItemAdded_(const void*, int n = 1) {} // #IGNORE called after any single item insert or add, including multiple inserts/adds of the same-valued item
  virtual void		ItemRemoved_(const void*, int n = 1) {} // #IGNORE called after any single item removal, including multiple removals of the same-valued item
  virtual void		ItemsChanged_() {} // #IGNORE called on changes that can't call one of the other two routines 
};

#define TA_FIXED_ARRAY_FUNS(y,T) \
public: \
  STATIC_CONST T blank; \
  explicit y(int init_size) {EnforceSize(init_size); } \
  y(int init_size, const T& i0) {EnforceSize(init_size); el[0] = i0;} \
  y(int init_size, const T& i0, const T& i1) \
    {EnforceSize(init_size); el[0] = i0; el[1] = i1;} \
  y(int init_size, const T& i0, const T& i1, const T& i2) \
    {EnforceSize(init_size); el[0] = i0; el[1] = i1; el[2] = i2;} \
  y(int init_size, const T& i0, const T& i1, const T& i2, const T& i3) \
    {EnforceSize(init_size); el[0] = i0; el[1] = i1; el[2] = i2; el[3] = i3;} \
  y(int init_size, T init[]) {EnforceSize(init_size); \
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
class taFixedArray : public taFixedArray_impl { // #INSTANCE rudimentary array, primarily intended as an OO replacement for C arrays
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
protected:
  override void*	MakeArray_(int n) const	{ return new T[n]; }
  override void		SetArray_(void* nw) {if (el) delete [] el; el = (T*)nw;}
  override bool		El_Equal_(const void* a, const void* b) const
    { return (*((T*)a) == *((T*)b)); }
  override void		El_Copy_(void* to, const void* fm) { *((T*)to) = *((T*)fm); } 
  override uint		El_SizeOf_() const	{ return sizeof(T); }
};


class int_FixedArray: public taFixedArray<int> {
  TA_FIXED_ARRAY_FUNS(int_FixedArray, int)
};


class  taBasicArray_impl: public taFixedArray_impl {
  // #VIRT_BASE Basic shared subtype for Arrays and Matrixes, no tokens of which are ever kept
public:
  taBasicArray_impl()			{ alloc_size = 0;}
  virtual ~taBasicArray_impl()		{ alloc_size = 0;}

  virtual void		Alloc(uint n)    {Alloc_(n);} // allocate storage for at least the given size
 
  ////////////////////////////////////////////////
  // functions that don't depend on the type	//
  ////////////////////////////////////////////////

  virtual bool		Remove(int idx, int n_els=1);
  // #MENU #MENU_ON_Edit Remove (n_els) item(s) at idx, returns success

protected: 
  uint			alloc_size;		// #READ_ONLY #NO_SAVE #DETAIL allocated (physical) size

  override bool 	AdjustAndSetAlloc_(uint& new_alloc); // we manage extra storage, and impl grow algorithm
  
// compulsory element accessor and manip functions 
  virtual bool		Remove_(void* it);		// #IGNORE
};


#define TA_BASIC_ARRAY_FUNS(y,T) \
public: \
  STATIC_CONST T blank; \
  explicit y(int init_alloc) {Alloc(init_alloc); } \
  y() {} \
  y(const y& cp) {Copy(cp); } \
  T&		operator[](int i) { return el[i]; } \
  const T&	operator[](int i) const	{ return el[i]; } \
  y& operator=(const y& cp) {Copy(cp); return *this;} \
  bool 		operator==(const y& src) const {return Equal_(src);} \
protected: \
  override const void*	El_GetBlank_() const	{ return (const void*)&blank; }

template<class T> 
class taBasicArray : public taBasicArray_impl { // #INSTANCE rudimentary array, primarily intended as an OO replacement for C arrays
public:
  T*		el;		// #HIDDEN #NO_SAVE Pointer to actual array memory

  taBasicArray()				{el = NULL;}
  virtual ~taBasicArray()			{ SetArray_(NULL); } //

  ////////////////////////////////////////////////
  // 	functions that return the type		//
  ////////////////////////////////////////////////

  const T& SafeEl(int i) const { return *(static_cast<const T*>(SafeEl_(i))); }
  // the element at the given index
  const T&	FastEl(int i) const		{ return el[i]; }
  // fast element (no range checking)
  T&		FastEl(int i) 		{ return el[i]; }
  // fast element (no range checking)
  const T&	RevEl(int idx) const		{ return SafeEl(size - idx - 1); }
  // reverse (index) element (ie. get from the back of the list first)
  void		Copy(const taBasicArray<T>& cp)	{Copy_(cp);} // #IGNORE buggy maketa

  const T	Pop() {if (size == 0) return *(static_cast<const T*>(El_GetErr_()));
   else return el[--size]; }
  // pop the last item in the array off
  const T& 	Peek() const {return SafeEl(size - 1);}
  // peek at the last item on the array

  ////////////////////////////////////////////////
  // 	functions that are passed el of type	//
  ////////////////////////////////////////////////

  void	Set(int i, const T& item) 	{ FastEl(i) = item; }
  // use this for assigning values to items in the array (Set should update if needed)
  void	Add(const T& item)		{ Add_((const void*)&item); }
  // #MENU add the item to the array
  bool	AddUnique(const T& item)	{ return AddUnique_((void*)&item); }
  // add the item to the array if it isn't already on it, returns true if unique
  void	Push(const T& item)		{ Add(item); }
  // push the item on the end of the array (same as add)
  void	Insert(const T& item, int idx, int n_els=1) 
    { Insert_((void*)&item, idx, n_els); }
    // #MENU Insert (n_els) item(s) at idx (-1 for end) in the array
  int	Find(const T& item, int i=0) const { return Find_((void*)&item, i); }
    // #MENU #USE_RVAL Find item starting from idx in the array (-1 if not there)
//  virtual bool	Remove(const T& item)		{ return Remove_((void*)&item); } //
  //using taBasicArray_impl::Remove;
  USING(taBasicArray_impl::Remove)
  bool	RemoveEl(const T& item)		{ return Remove_((void*)&item); }
  // remove given item, returns success
public:
  override void*	FastEl_(int i)		{ return &(el[i]); } 
protected:
  override void*	MakeArray_(int n) const	{ return new T[n]; }
  override void		SetArray_(void* nw)  {if (el) delete [] el; el = (T*)nw;}
  override bool		El_Equal_(const void* a, const void* b) const
    { return (*((T*)a) == *((T*)b)); }
  override void		El_Copy_(void* to, const void* fm) { *((T*)to) = *((T*)fm); } 
  override uint		El_SizeOf_() const	{ return sizeof(T); }
};


class  taArray_impl: public taBasicArray_impl {
  // #VIRT_BASE Base Type for Arrays, no tokens of which are ever kept
public:
  ////////////////////////////////////////////////
  // functions that don't depend on the type	//
  ////////////////////////////////////////////////

  virtual String	SafeElAsStr(int idx) const
    {return InRange(idx) ? El_GetStr_(FastEl_(idx)) : _nilString;}
  // #IGNORE element at idx, as a string
  virtual String	FastElAsStr(int idx) const {return El_GetStr_(FastEl_(idx));}
  // #IGNORE element at idx, as a string
  
  // Advanced sorting and permuting
  virtual void	Permute();
  // #MENU permute the items in the list into a random order
  virtual void	Sort(bool descending=false);
  // #MENU sort the list in ascending order (or descending if switched)
  virtual void	ShiftLeft(int nshift);
  // shift all the elements in the array to the left by given number of items
  virtual void	ShiftLeftPct(float pct);
  // shift the array to the left by given percentage of current size
  virtual int	V_Flip(int width);
  // vertically flip the array as if it was arrange in a matrix of width

  // Advanced duplication and copying
  
  virtual void	DupeUnique(const taArray_impl& cp);
  // duplicate so result is unique list
  virtual void	Copy_Common(const taArray_impl& cp);
  // copy elements in common
  virtual void	Copy_Duplicate(const taArray_impl& cp);
  // copy elements in common, duplicating (if necc) any extra on cp
  virtual void 	CopyVals(const taArray_impl& from, int start=0, int end=-1, int at=0);
  // copy values from other array at given start and end points, and putting at given point in this
  virtual bool	Move(int from, int to);
  // #MENU move item from index to index
  
  // Listing
  virtual void	List(ostream& strm = cout) const;
  // print out all of the elements in the array
  virtual void	InitFromString(const char* val);
  // initialize an array from given string (does reset first)
protected:
  virtual void*		El_GetTmp_() const = 0;
  virtual int		El_Compare_(const void* a, const void* b) const = 0;
  virtual String	El_GetStr_(const void*) const		{ return _nilString; } // #IGNORE
  virtual void		El_SetFmStr_(void*, const String&) 	{ };       // #IGNORE
};


// the plainarray is not a taBase..

#define TA_PLAIN_ARRAY_FUNS(y,T) \
public: \
  STATIC_CONST T blank; \
  explicit y(int init_alloc) {Alloc(init_alloc); } \
  y() {} \
  y(const y& cp) {Copy(cp); } \
  T&		operator[](int i) { return el[i]; } \
  const T&	operator[](int i) const	{ return el[i]; } \
  y& operator=(const y& cp) {Copy(cp); return *this;} \
  bool 		operator==(const y& src) const {return Equal_(src);} \
protected: \
  override const void*	El_GetBlank_() const	{ return (const void*)&blank; }

template<class T> 
class taPlainArray : public taArray_impl { // #INSTANCE
public:
  T*	el;		// #HIDDEN #NO_SAVE Pointer to actual array memory

  taPlainArray()		{el = NULL;}
  ~taPlainArray()		{ SetArray_(NULL); } //

  ////////////////////////////////////////////////
  // 	functions that return the type		//
  ////////////////////////////////////////////////

  const T&	SafeEl(int i) const {return *(static_cast<const T*>(SafeEl_(i)));}
  // the element at the given index
  const T&	FastEl(int i) const		{ return el[i]; }
  // fast element (no range checking)
  T&		FastEl(int i) 	{ return el[i]; }
  // fast element (no range checking)
  const T&	RevEl(int idx) const		{ return SafeEl(size - idx - 1); }
  // reverse (index) element (ie. get from the back of the list first)
  
  void 		Copy(const taPlainArray<T>& cp)	{Copy_(cp);} // #IGNORE buggy maketa
  void 		Duplicate(const taPlainArray<T>& cp)	{Duplicate_(cp);} // #IGNORE buggy maketa

  const T	Pop() 
    {if (size == 0) return *(static_cast<const T*>(El_GetErr_()));
     else return el[--size]; }
  // pop the last item in the array off
  const T&	Peek() const {return SafeEl(size - 1); }
  // peek at the last item on the array

  ////////////////////////////////////////////////
  // 	functions that are passed el of type	//
  ////////////////////////////////////////////////

  void	Set(int i, const T& item) 	{ FastEl(i) = item; }
  // use this for assigning values to items in the array (Set should update if needed)
  void	Add(const T& item)		{ Add_((const void*)&item); }
  // #MENU add the item to the array
  bool	AddUnique(const T& item)	{ return AddUnique_((const void*)&item); }
  // add the item to the array if it isn't already on it, returns true if unique
  void	Push(const T& item)		{ Add(item); }
  // push the item on the end of the array (same as add)
  void	Insert(const T& item, int idx, int n_els=1) { Insert_((void*)&item, idx, n_els); }
  // #MENU Insert (n_els) item(s) at idx (-1 for end) in the array
  int	Find(const T& item, int i=0) const { return Find_((void*)&item, i); }
  // #MENU #USE_RVAL Find item starting from idx in the array (-1 if not there)
//  virtual bool	Remove(const T& item)		{ return Remove_((void*)&item); }
//  USING(taArray_impl::Remove)
  // Remove (n_els) item(s) at idx, returns success
  bool	RemoveEl(const T& item)		{ return Remove_((void*)&item); }
  // remove given item, returns success
public:
  override void*	FastEl_(int i)	{ return &(el[i]); } // #IGNORE
protected:
  mutable T		tmp;		// #HIDDEN
  
  override void*	MakeArray_(int n) const	{ return new T[n]; }
  override void		SetArray_(void* nw) {if (el) delete [] el; el = (T*)nw;}
  override int		El_Compare_(const void* a, const void* b) const
  { int rval=-1; if(*((T*)a) > *((T*)b)) rval=1; else if(*((T*)a) == *((T*)b)) rval=0; return rval; }
  override bool		El_Equal_(const void* a, const void* b) const
    {return (*((T*)a) == *((T*)b));} 
  override void		El_Copy_(void* to, const void* fm)	{ *((T*)to) = *((T*)fm); } 
  override uint		El_SizeOf_() const	{ return sizeof(T); }
  override void*	El_GetTmp_() const	{ return (void*)&tmp; }	
  override String	El_GetStr_(void* it) const {return String(*((T*)it)); } 
  override void		El_SetFmStr_(void* it, const String& val)
     { *((T*)it) = (T)val; } 
};



#endif // ta_list_h
