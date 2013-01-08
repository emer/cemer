// Copyright, 1995-2013, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of The Emergent Toolkit
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

#ifndef taPtrList_impl_h
#define taPtrList_impl_h 1

// parent includes:
#include "ta_def.h"
#include <taString>

// member includes:
#include <taHashEl>
#include <DataChangedReason>

// declare all other types mentioned but not required to include:
class taHashTable; // 

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




/////////////////////////////////////////////////////////////////////
//                      PtrList
/////////////////////////////////////////////////////////////////////

class TA_API  taPtrList_impl {
  // #NO_TOKENS implementation of the pointer list class
public:
  enum KeyType {
    KT_NAME,                    // use item name or string value -- the default
    KT_PTR,                     // use the item pointer
  };

  static taHashVal      HashCode_String(const String& string);
  // #EXPERT #CAT_Access get a hash code value from given string
  static taHashVal      HashCode_Ptr(const void* ptr);
  // #EXPERT #CAT_Access get a hash code value from given ptr

protected:
  static taPtrList_impl scratch_list;   // a list for any temporary processing needs

  // these define what to do with an individual item (overload them!)
  virtual String        GetListName_() const            { return _nilString; }
  // name of the list
  virtual String        El_GetName_(void*) const        { return _nilString; }
  // gets name from an element (for list)
  virtual void          El_SetName_(void*, const String&)  {}
  // mostly for whole-list copies, where we copy the name too
  virtual void          El_SetDefaultName_(void*, int) {}
    // if added item has empty name, this will get called, enabling a name to be set; index has been set
  virtual taHashVal     El_GetHashVal_(void* it) const;
  // gets hash code based on key type in hash table; default is for string-key'ed lists (v3.2 default)
  virtual String        El_GetHashString_(void* it) const;
  // gets hash string if using KT_NAME, else _nilString -- for passing string arg to hash funs
  virtual taPtrList_impl* El_GetOwnerList_(void*) const { return (taPtrList_impl*)this; }
  // who owns the el? -- only returns a list if the owner is a list
  virtual void*         El_GetOwnerObj_(void*) const    { return NULL; }
  // who owns the el?
  virtual void*         El_SetOwner_(void* it)          { return it; }
  // set owner to this
  virtual void          El_SetIndex_(void*, int)        { };
  // sets the element's self-index
  virtual bool          El_FindCheck_(void* it, const String& nm) const
  { return (El_GetName_(it) == nm); }
  virtual int           El_Compare_(void* a, void* b) const
  { int rval=-1; if(El_GetName_(a) > El_GetName_(b)) rval=1;
    else if(El_GetName_(a) == El_GetName_(b)) rval=0; return rval; }
  // compare two items for purposes of sorting

  virtual void* El_Ref_(void* it)       { return it; }  // when pushed
  virtual void* El_unRef_(void* it)     { return it; }  // when popped
  virtual void  El_Done_(void*)         { };    // when "done" (delete)
  virtual void* El_Own_(void* it)       { El_SetOwner_(El_Ref_(it)); return it; }
  virtual void  El_disOwn_(void* it)    { El_Done_(El_unRef_(it)); }

  virtual void* El_MakeToken_(void* it)         { return it; }
  // how to make a token of the same type as the argument
  virtual void* El_Copy_(void* trg, void*) { return trg; }
  // how to copy from given item (2nd arg); usually doesn't copy name
  virtual void* El_CopyN_(void* trg, void* fm);
  // El_Copy_ + name copy; our version synthesizes, but you can override for efficiency
  enum ElKind {
    EK_NULL     = 0, // duh!
    EK_OWN      = 1, // owned by list, is an instance
    EK_LINK     = 2 // is a linked item
  };
  virtual ElKind        El_Kind_(void* it) const; // kind of item; normally not overidden

  void          InitList_();
  virtual int   Scratch_Find_(const String& it) const;
  // find item on the scratch_list using derived El_FindCheck_()
  void          UpdateIndex_(int idx);
  // update the index of the item at given index (i.e., which was just moved)

public:
  void**        el;             // #IGNORE the elements themselves
  int           alloc_size;     // #READ_ONLY #NO_SAVE #CAT_taList allocation size
  taHashTable*  hash_table;     // #READ_ONLY #NO_SAVE #HIDDEN #CAT_taList a hash table (NULL if not used)
  int           size;           // #READ_ONLY #NO_SAVE #SHOW #CAT_taList number of elements in the list

  taPtrList_impl()                      { InitList_(); }
  taPtrList_impl(const taPtrList_impl& cp)      { InitList_(); Duplicate(cp); }
  virtual ~taPtrList_impl();

  ////////////////////////////////////////////////
  //    functions that return the type          //
  ////////////////////////////////////////////////

  virtual TypeDef*      El_GetType_(void*) const {return GetElType();}
    // #IGNORE should usually override to provide per-item typing where applicable
  void*         SafeEl_(int i) const
  { void* rval=NULL; i=Index(i); if(InRange(i)) rval = el[i]; return rval; }    // #IGNORE
  void*         PosSafeEl_(int i) const
  { void* rval=NULL; if(InRange(i)) rval = el[i]; return rval; }
    // #IGNORE -- for internal use only, where you want NULL if i < 0
  void*         FastEl_(int i)  const   { return el[i]; }       // #IGNORE
  virtual void* FindName_(const String& it) const;      // #IGNORE
  void*         Pop_(); // #IGNORE -- NOTE: non-standard semantics, does not do a disown
  void*         TakeItem_(int idx); // #IGNORE -- WARNING: not for owned items!!!
  void*         First_() const
  { if (size > 0) return el[0]; else return NULL; }  // #IGNORE
  void*         Peek_() const
  { if(size > 0) return el[size-1]; else return NULL; }  // #IGNORE

  virtual void* AddUniqNameOld_(void* it);
  // #IGNORE add so that name is unique, old used if dupl, returns one used

  virtual void* LinkUniqNameOld_(void* it);             // #IGNORE


  ////////////////////////////////////////////////
  //    functions that are passed el of type    //
  ////////////////////////////////////////////////

  virtual int   FindEl_(const void* it) const;                  // #IGNORE

  virtual void  AddOnly_(void* it);
  // #IGNORE just puts the el on the list, doesn't do anything else
  virtual void  Add_(void* it, bool no_notify = false);         // #IGNORE
  virtual bool  AddUnique_(void* it);                   // #IGNORE
  virtual bool  AddUniqNameNew_(void* it);
  // #IGNORE add a new object so that list only has one, new replaces existing
  virtual void* DuplicateEl_(void* it);
  // #IGNORE duplicate given item and add to list, returning new item, or NULL if failed
  virtual bool  Insert_(void* it, int where, bool no_notify = false);           // #IGNORE
  virtual bool  ReplaceEl_(void* ol, void* nw);         // #IGNORE
  virtual bool  ReplaceName_(const String& ol, void* nw);       // #IGNORE
  virtual bool  ReplaceIdx_(int ol, void* nw, bool no_notify_insert = false); // #IGNORE
  virtual bool  Transfer_(void* it);                    // #IGNORE
  virtual bool  RemoveEl_(void* it);                    // #IGNORE
  virtual bool  MoveBefore_(void* trg, void* item);     // #IGNORE
  virtual bool  MoveAfter_(void* trg, void* item);      // #IGNORE

  virtual void  Link_(void* it);                        // #IGNORE
  virtual bool  LinkUnique_(void* it);                  // #IGNORE
  virtual bool  LinkUniqNameNew_(void* it);             // #IGNORE
  virtual bool  InsertLink_(void* it, int where);       // #IGNORE
  virtual bool  ReplaceLinkEl_(void* ol, void* nw);     // #IGNORE
  virtual bool  ReplaceLinkName_(const String& ol, void* nw);   // #IGNORE
  virtual bool  ReplaceLinkIdx_(int ol, void* nw);              // #IGNORE

  virtual void  Push_(void* it);                        // #IGNORE

  ////////////////////////////////////////////////
  // functions that don't depend on the type    //
  ////////////////////////////////////////////////

  virtual int   FindNameIdx(const String& it) const;
  // #CAT_Access return the index of the item with given name on the list

  virtual TypeDef* GetElType() const {return NULL;}
  // #IGNORE Default type for objects in group
  virtual void  DataChanged(int dcr, void* op1 = NULL, void* op2 = NULL) {}
  // #IGNORE called when list has changed -- more fine-grained than Dirty(), and may be multiple calls per event
  inline int    Index(int idx) const { if(idx < 0) idx += size; return idx; }
  // #EXPERT #CAT_Access get actual index from index value that can also be negative, meaning access from end of list
  inline bool   InRange(int idx) const { return ((idx < size) && (idx >= 0)); }
  // #EXPERT #CAT_Access is the specified index within range of 0 >= idx < size
  virtual bool  Alloc(int sz);
  // #EXPERT #CAT_Modify allocate a list big enough for given number of elements (or current size) -- uses optimized memory allocation policies and generally allocates more than currently needed
  virtual bool  AllocExact(int sz);
  // #EXPERT #CAT_Modify allocate exact number specified
  void          Trim(int n); // #IGNORE if larger than n, trim to n (does NOT expand)
  virtual void  Reset()                 { RemoveAll(); }
  // #CAT_Modify reset the list (remove all elements)
  virtual bool  IsEmpty() const { return (size == 0) ? true : false; }
  // #CAT_Access is the list empty of elements (i.e., size == 0)

  virtual void  BuildHashTable(int n_buckets, KeyType key_typ = KT_NAME);
  // #CAT_Modify build a hash table with given number of buckets and key type (not dynamic, so make it big)

  virtual bool  RemoveName(const String& item_nm);
  // #CAT_Modify remove (and delete) named element from list
  virtual bool  RemoveIdx(int idx);
  // #CAT_Modify remove (and delete) element from list at index
  virtual bool  RemoveLast();
  // #CAT_Modify remove the last element on the list
  virtual void  RemoveAll();
  // #MENU #MENU_ON_Edit #CONFIRM #CAT_Modify Remove all elements on the list

  virtual bool  MoveIdx(int from, int to);
  // #CAT_Modify Move element from index (from) to position (to) in list
  virtual bool  MoveBeforeIdx(int from, int to);
  // #CAT_Modify Move element from index (from) to just before position (to) in list; for end, use to=-1 or size
  virtual bool  SwapIdx(int pos1, int pos2);
  // #CAT_Modify Swap the elements in the two given positions on the list

  virtual void  PopAll();
  // #CAT_Modify pop all elements off the stack

  virtual void  Permute();
  // #MENU #CONFIRM #CAT_Order permute the items in the list into a random order
  virtual void  Sort(bool descending=false);
  // #MENU #CONFIRM #CAT_Order sort the items in the list in alpha order according to name (or El_Compare_)
  virtual void  Sort_(bool descending=false);   // #IGNORE implementation of sorting function
  virtual void  UpdateAllIndicies();    // #IGNORE update all indices of elements in list

  void*         FirstEl(int& itr) {itr = 0; return SafeEl_(0);}
  // #EXPERT #CAT_Access get the first item on the list, initialize iterator
  void*         NextEl(int& itr) {return (++itr < size) ? FastEl_(itr) : NULL;}
  // #EXPERT #CAT_Access get the next item on the list according to iterator

  /////////////////////////////////////////////////////////////////////////
  // replicating items: either by clone/add (duplicate) or link (borrow) //
  /////////////////////////////////////////////////////////////////////////

  void  Duplicate(const taPtrList_impl& cp);
  // #CAT_Copy duplicate (clone & add) elements of given list into this one
  void  DupeUniqNameNew(const taPtrList_impl& cp);
  // #CAT_Copy duplicate so result is unique names, replacing with new ones where dupl
  void  DupeUniqNameOld(const taPtrList_impl& cp);
  // #CAT_Copy duplicate so result is unique names, using old ones where dupl

  void  Stealth_Borrow(const taPtrList_impl& cp);
  // #CAT_Copy borrow without referencing the borrowed elements (i.e. use AddEl_())
  void  Borrow(const taPtrList_impl& cp);
  // #CAT_Copy borrow (link) elements of given list into this one
  void  BorrowUnique(const taPtrList_impl& cp);
  // #CAT_Copy borrow so result is unique list
  void  BorrowUniqNameNew(const taPtrList_impl& cp);
  // #CAT_Copy borrow so result is unique names, replacing with new ones where dupl
  void  BorrowUniqNameOld(const taPtrList_impl& cp);
  // #CAT_Copy borrow so result is unique names, using old ones where dupl

  /////////////////////////////////
  // copying items between lists //
  /////////////////////////////////

  void  Copy_Common(const taPtrList_impl& cp);
  // #CAT_Copy apply copy operator to only those items in common between the two lists
  void  Copy_Duplicate(const taPtrList_impl& cp);
  // #CAT_Copy apply copy operator to items, use duplicate to add new ones from cp (if necc)
  void  Copy_Borrow(const taPtrList_impl& cp);
  // #CAT_Copy apply copy operator to items, use borrow to add new ones from cp (if necc)
  void  Copy_Exact(const taPtrList_impl& cp);
  // #CAT_Copy makes us basically identical to cp, in number, and type
  void  Hijack(taPtrList_impl& src);
  // #IGNORE specialized usage, transfers entire the memory to us, leaving src empty

  // browsing -- browse client lists must override
  virtual int           NumListCols() const {return 0;}
  // #IGNORE number of columns in a list view for this item type
  virtual const KeyString GetListColKey(int col) const {return _nilKeyString;}
  // #IGNORE col key for the default list column
  virtual String        GetColHeading(const KeyString& key) const {return _nilKeyString;}
  // #IGNORE header text for the given key
  virtual String        ChildGetColText(void* child, TypeDef* typ, const KeyString& key,
    int itm_idx = -1) const {return _nilKeyString;}
  // #IGNORE itm_idx is a hint from source, -1 means not specified or ignore

  // output
  virtual String&  	Print(String& strm, int indent=0) const;
  // #CAT_Display print the items on the list
protected:
  virtual void          ItemRemoved_() {} // we overload this in groups to update the leaf counts
  void                  Copy_Duplicate_impl(const taPtrList_impl& cp);
    // factored code
};

#endif // taPtrList_impl_h
