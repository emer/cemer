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

#ifndef taPtrList_base_h
#define taPtrList_base_h 1

// parent includes:
#include <taPtrList_impl>

// member includes:

// declare all other types mentioned but not required to include:

taTypeDef_Of(taPtrList_base);

template<class T>
class taPtrList_base : public taPtrList_impl { // #INSTANCE
public:
  taPtrList_base()                                      { };
  taPtrList_base(const taPtrList_base<T>& cp)           { Duplicate(cp); };

  ////////////////////////////////////////////////
  //    functions that are passed el of type    //
  ////////////////////////////////////////////////

  virtual int   FindEl(const T* item) const     { return FindEl_((const void*)item); }
  // #CAT_Access find element in list (-1 if not there)

  virtual void  AddOnly(T* item)                        { AddOnly_((void*)item); }
  // #CAT_Modify append a new pointer to end of list -- does not own
  virtual void  Add(T* item)                    { Add_((void*)item); }
  // #CAT_Modify add element to the list and "own" item
  virtual bool  AddUnique(T* item)              { return AddUnique_((void*)item); }
  // #CAT_Modify add so that object is unique, true if unique
  virtual bool  AddUniqNameNew(T* item)         { return AddUniqNameNew_((void*)item); }
  // #CAT_Modify add so that name is unique, true if unique, new replaces existing

  virtual bool  Insert(T* item, int where)      { return Insert_((void*)item, where); }
  // #CAT_Modify insert element at index (-1 for end)
  virtual bool  ReplaceEl(T* old_it, T* new_it) { return ReplaceEl_((void*)old_it, (void*)new_it); }
  // #CAT_Modify replace given element with the new one
  virtual bool  ReplaceName(const String& old_nm, T* new_it)    { return ReplaceName_(old_nm, (void*)new_it); }
  // #CAT_Modify replace named element with the new one
  virtual bool  ReplaceIdx(int old_idx, T* new_it)              { return ReplaceIdx_(old_idx, (void*)new_it); }
  // #CAT_Modify replace element at index with the new one

  virtual bool  RemoveEl(T* item)               { return RemoveEl_((void*)item); }
  // #MENU #ARG_ON_OBJ #CAT_Modify Remove given item from list
  T*            DuplicateEl(const T* item) { return (T*)DuplicateEl_((void*)item); }
  // #MENU #ARG_ON_OBJ #CAT_Modify Duplicate given list item and Add to list

  virtual void  Link(T* item)                   { Link_((void*)item); }
  // #CAT_Modify Link an item to list without owning it
  virtual bool  LinkUnique(T* item)             { return LinkUnique_((void*)item); }
  // #CAT_Modify link so that object is unique, true if unique
  virtual bool  LinkUniqNameNew(T* item)                { return LinkUniqNameNew_((void*)item); }
  // #CAT_Modify link so that name is unique, true if unique, new replaces existing
  virtual bool  InsertLink(T* item, int idx= -1)        { return InsertLink_((void*)item, idx);}
  // #MENU #CAT_Modify Insert a link at index (-1 for end)
  virtual bool  ReplaceLinkEl(T* old_it, T* new_it)     { return ReplaceLinkEl_((void*)old_it, (void*)new_it); }
  // #CAT_Modify replace given element (if on list) with the new one
  virtual bool  ReplaceLinkName(const String& old_nm, T* new_it) { return ReplaceLinkName_(old_nm, (void*)new_it); }
  // #CAT_Modify replace given named element (if on list) with the new one
  virtual bool  ReplaceLinkIdx(int old_idx, T* new_it)  { return ReplaceLinkIdx_(old_idx, (void*)new_it); }
  // #CAT_Modify replace element with a link to the new one

  virtual void  Push(T* item)                   { Push_((void*)item); }
  // #CAT_Modify push item on stack (for temporary use, not "owned")

  virtual bool  MoveEl(T* from, T* to)          { return MoveIdx(FindEl(from), FindEl(to)); }
  // #MENU #ARG_ON_OBJ Move #CAT_Modify item (from) to position of (to)
  virtual bool  Transfer(T* item)               { return Transfer_((void*)item); }
  // #MENU #MENU_ON_Edit #PROJ_SCOPE #CAT_Modify Transfer element to this list

  virtual bool  MoveBefore(T* trg, T* item) { return MoveBefore_((void*)trg, (void*)item); }
  // #CAT_Modify move item so that it appears just before the target item trg in the list
  virtual bool  MoveAfter(T* trg, T* item) { return MoveAfter_((void*)trg, (void*)item); }
  // #CAT_Modify move item so that it appears just after the target item trg in the list
};

#endif // taPtrList_base_h
