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

#ifndef taPtrList_h
#define taPtrList_h 1

// parent includes:
#include <taPtrList_impl>

// member includes:

// declare all other types mentioned but not required to include:


template<class T>
class taPtrList : public taPtrList_impl { // #INSTANCE
public:
  taPtrList(bool=false)                                 { };
  // borrow is guaranteed to work, others require EL_ functions..
  taPtrList(const taPtrList<T>& cp, bool=false)         { Borrow(cp); }

  ////////////////////////////////////////////////
  //    functions that return the type          //
  ////////////////////////////////////////////////

  // operators
  T*            SafeEl(int i) const             { return (T*)SafeEl_(i); }
  T*            PosSafeEl(int i) const          { return (T*)PosSafeEl_(i); }
  // #IGNORE element at index
  T*            FastEl(int i) const             { return (T*)el[i]; }
  // #CAT_Access fast element (no range checking)
  T*            operator[](int i) const         { return (T*)el[i]; }
  void          operator=(const taPtrList<T>& cp) { Reset(); Borrow(cp); }
  // borrow is guaranteed to work, others require EL_ functions..

  T*            Edit_El(T* item) const          { return SafeEl(FindEl(item)); }
  // #CAT_Access #MENU #MENU_ON_Edit #USE_RVAL #ARG_ON_OBJ Edit given list item

  T*            FindName(const String& item_nm) const
  { return (T*)FindName_(item_nm); }
  // #CAT_Access find given named element (NULL = not here), sets idx
  T*            First()                         { return (T*)First_(); }
  // #CAT_Modify return first element, or NULL if list empty
  T*            TakeItem(int idx)       { return (T*)TakeItem_(idx); }
  // #CAT_Modify remove the element from list, NULL if idx out of range -- ONLY FOR NON-OWNED ITEMS
  T*            Pop()                           { return (T*)Pop_(); }
  // #CAT_Modify pop the last element off the stack
  T*            Peek() const                    { return (T*)Peek_(); }
  // #CAT_Access peek at the last element on the stack

  virtual T*    AddUniqNameOld(T* item)         { return (T*)AddUniqNameOld_((void*)item); }
  // #CAT_Modify add so that name is unique, old used if dupl, returns one used
  virtual T*    LinkUniqNameOld(T* item)                { return (T*)LinkUniqNameOld_((void*)item); }
  // #CAT_Modify link so that name is unique, old used if dupl, returns one used

  ////////////////////////////////////////////////
  //    functions that are passed el of type    //
  ////////////////////////////////////////////////

  virtual int   FindEl(const T* item) const     { return FindEl_((const void*)item); }
  // #CAT_Access find element in list (-1 if not there)
  virtual void  AddOnly(T* item)                { AddOnly_((void*)item); }
  // #IGNORE append a new pointer to end of list, does not own it or do anything else
  virtual void  Add(T* item)                    { Add_((void*)item); }
  // #CAT_Modify add element to the list and "own" item
  virtual bool  AddUnique(T* item)              { return AddUnique_((void*)item); }
  // #CAT_Modify add so that object is unique, true if unique
  virtual bool  AddUniqNameNew(T* item)         { return AddUniqNameNew_((void*)item); }
  // #EXPERT #CAT_Modify add so that name is unique, true if unique, new replaces existing

  virtual bool  Insert(T* item, int idx)        { return Insert_((void*)item, idx); }
  // #CAT_Modify Add or insert element at idx (-1 for end)
  virtual bool  ReplaceEl(T* old_it, T* new_it) { return ReplaceEl_((void*)old_it, (void*)new_it); }
  // #CAT_Modify Replace old element with new element
  virtual bool  ReplaceName(const String& old_nm, T* new_it)    { return ReplaceName_(old_nm, (void*)new_it); }
  // #CAT_Modify replace element with given name with the new one
  virtual bool  ReplaceIdx(int old_idx, T* new_it)      { return ReplaceIdx_(old_idx, (void*)new_it); }
  // #CAT_Modify replace element at index with the new one

  virtual bool  RemoveEl(T* item)       { return RemoveEl_(item); }
  // #CAT_Modify #MENU #ARG_ON_OBJ Remove given item from list
  // note: folowing not virt, because we hide in taList with stronger typed version
  virtual T*    DuplicateEl(const T* item) { return (T*)DuplicateEl_((void*)item); }
  // #CAT_Modify #MENU #ARG_ON_OBJ Duplicate given list item and Add to list; returns item added, or NULL if failed

  virtual void  Link(T* item)                   { Link_((void*)item); }
  // #CAT_Modify Link an item to list without owning it
  virtual bool  LinkUnique(T* item)             { return LinkUnique_((void*)item); }
  // #CAT_Modify link so that object is unique, true if unique
  virtual bool  LinkUniqNameNew(T* item)                { return LinkUniqNameNew_((void*)item); }
  // #EXPERT #CAT_Modify link so that name is unique, true if unique, new replaces existing
  virtual bool  InsertLink(T* item, int idx= -1) { return InsertLink_((void*)item, idx);}
  // #CAT_Modify #MENU Insert a link at index (-1 for end)
  virtual bool  ReplaceLinkEl(T* old_it, T* new_it)     { return ReplaceLinkEl_((void*)old_it, (void*)new_it); }
  // #CAT_Modify replace given element (if on list) with the new one
  virtual bool  ReplaceLinkName(const String& old_nm, T* new_it) { return ReplaceLinkName_(old_nm, (void*)new_it); }
  // #CAT_Modify replace given named element (if on list) with the new one
  virtual bool  ReplaceLinkIdx(int old_idx, T* new_it)  { return ReplaceLinkIdx_(old_idx, (void*)new_it); }
  // #CAT_Modify replace element with a link to the new one

  virtual void  Push(T* item)                   { Push_((void*)item); }
  // #CAT_Modify push item on stack (for temporary use, not "owned")

  virtual bool  MoveEl(T* from, T* to)          { return MoveIdx(FindEl(from), FindEl(to)); }
  // #CAT_Modify #MENU #ARG_ON_OBJ Move item (from) to position of (to)
  virtual bool  Transfer(T* item)               { return Transfer_((void*)item); }
  // #CAT_Modify #MENU #MENU_ON_Edit #PROJ_SCOPE Transfer item to this list

  virtual bool  MoveBefore(T* trg, T* item) { return MoveBefore_((void*)trg, (void*)item); }
  // #CAT_Modify move item so that it appears just before the target item trg in the list
  virtual bool  MoveAfter(T* trg, T* item) { return MoveAfter_((void*)trg, (void*)item); }
  // #CAT_Modify move item so that it appears just after the target item trg in the list
};

#define taPtrList_of(T)                                               \
class TA_API T ## _List : public taPtrList<T> {                               \
protected:                                                                    \
  void  El_Done_(void* item)    { delete (T*)item; }                          \
public:                                                                       \
  ~ ## T ## _List()             { Reset(); }                                  \
}

#endif // taPtrList_h
