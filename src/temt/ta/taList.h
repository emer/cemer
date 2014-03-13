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

#ifndef taList_h
#define taList_h 1

// parent includes:
#include <taList_impl>

// member includes:

// declare all other types mentioned but not required to include:

taTypeDef_Of(taList);

template<class T>
class taList: public taList_impl {
  // #NO_TOKENS #INSTANCE #NO_UPDATE_AFTER a base list template
INHERITED(taList_impl)
public:
  #ifndef __MAKETA__
  typedef IteratorTaPtrListImpl<T, T&, T*>                     iterator;
  typedef IteratorTaPtrListImpl<T, const T&, const T*>   const_iterator;
  typedef std::reverse_iterator<iterator>              reverse_iterator;
  typedef std::reverse_iterator<const_iterator>  const_reverse_iterator;

                iterator begin()         { return               iterator(this, 0); }
          const_iterator begin()   const { return         const_iterator(this, 0); }
          const_iterator cbegin()  const { return         const_iterator(this, 0); }
                iterator end()           { return               iterator(this, size); }
          const_iterator end()     const { return         const_iterator(this, size); }
          const_iterator cend()    const { return         const_iterator(this, size); }
        reverse_iterator rbegin()        { return       reverse_iterator(      iterator(this, size)); }
  const_reverse_iterator rbegin()  const { return const_reverse_iterator(const_iterator(this, size)); }
  const_reverse_iterator crbegin() const { return const_reverse_iterator(const_iterator(this, size)); }
        reverse_iterator rend()          { return       reverse_iterator(      iterator(this, 0)); }
  const_reverse_iterator rend()    const { return const_reverse_iterator(const_iterator(this, 0)); }
  const_reverse_iterator crend()   const { return const_reverse_iterator(const_iterator(this, 0)); }
  #endif // __MAKETA__

  T*            SafeEl(int idx) const           { return (T*)SafeEl_(idx); }
  // #CAT_Access get element at index -- does safe range checking -- if index is negative, access is from the back of the list (-1 = last item, -2 = second to last, etc)
  T*            PosSafeEl(int idx) const        { return (T*)PosSafeEl_(idx); }
  // #IGNORE positive only, internal use
  T*            FastEl(int i) const             { return (T*)el[i]; }
  // #CAT_Access fast element (no range checking) -- if index is negative, access is from the back of the list (-1 = last item, -2 = second to last, etc)
  T*            operator[](int i) const { return (T*)el[i]; }

  T*            DefaultEl() const               { return (T*)DefaultEl_(); }
  // #CAT_Access returns the element specified as the default for this list

  T*            Edit_El(T* item) const          { return SafeEl(FindEl((taBase*)item)); }
  // #MENU #MENU_ON_Edit #USE_RVAL #ARG_ON_OBJ #CAT_Access Edit given list item
  T*            FindName(const String& item_nm) const
  { return (T*)FindName_(item_nm); }
  // #MENU #USE_RVAL #ARGC_1 #CAT_Access Find element with given name (item_nm)
  T*            FindNameContains(const String& item_nm) const
  { return (T*)FindNameContains_(item_nm); }
  // #MENU #USE_RVAL #ARGC_1 #CAT_Access Find element whose name contains given name sub-string
  T*            FindType(TypeDef* item_tp) const
  { return (T*)FindType_(item_tp); }
  // #CAT_Access find given type element (NULL = not here), sets idx
  T*            FindNameType(const String& item_nm) const
  { return (T*)FindNameType_(item_nm); }
  // #CAT_Access Find element with given object name or type name (item_nm)
  T*            FindMakeNameType(const String& item_nm, TypeDef* td, bool& made_new)
  { return (T*)FindMakeNameType_(item_nm, td, made_new); }
  // #CAT_Modify find an object with the given name and type (if not NULL) -- if not found or type was different, makes a new one and sets made_new = true -- in any case returns object

  T*            First() const                   { return (T*)First_(); }
  // #CAT_Access look at the first element; NULL if none
  T*            Pop()                           { return (T*)Pop_(); }
  // #CAT_Modify pop the last element off the stack
  T*            Peek() const                    { return (T*)Peek_(); }
  // #CAT_Access peek at the last element on the stack, if any

  virtual T*    AddUniqNameOld(T* item)         { return (T*)AddUniqNameOld_((void*)item); }
  // #CAT_Modify add so that name is unique, old used if dupl, returns one used
  virtual T*    LinkUniqNameOld(T* item)        { return (T*)LinkUniqNameOld_((void*)item); }
  // #CAT_Modify link so that name is unique, old used if dupl, returns one used

//   virtual bool       MoveBefore(T* trg, T* item) { return MoveBefore_((void*)trg, (void*)item); }
//   // #CAT_Modify move item so that it appears just before the target item trg in the list
//   virtual bool       MoveAfter(T* trg, T* item) { return MoveAfter_((void*)trg, (void*)item); }
//   // #CAT_Modify move item so that it appears just after the target item trg in the list

  TA_TMPLT_BASEFUNS(taList,T);
private:
  TMPLT_NOCOPY(taList,T);
  void  Initialize()                    { }; // NOTE: not calling SetBaseType here -- MUST do this in the class that derives from this -- this means that any use of template directly, instead of derived class of template, must do SetBaseType itself.
  void  Destroy()                       { };
};

// do not use this macro, since you typically will want ##NO_TOKENS, #NO_UPDATE_AFTER
// which cannot be put inside the macro!
//
// #define taList_of(T)
// class T ## _List : public taList<T> {
// public:
//   void Initialize()  { SetBaseType(&TA_T); }
//   void Destroy()     { };
//   TA_BASEFUNS(T ## _List);
// }

#endif // taList_h
