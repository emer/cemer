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

#ifndef taPlainArray_h
#define taPlainArray_h 1

// parent includes:
#include <taArray_impl>

// member includes:

// declare all other types mentioned but not required to include:

taTypeDef_Of(taPlainArray);

template<class T>
class taPlainArray : public taArray_impl {
  // #INSTANCE ##NO_TOKENS a plain array
public:
  T*            el;             // #HIDDEN #NO_SAVE #CAT_taArray Pointer to actual array memory
  T             err;            // #HIDDEN #CAT_taArray what is returned when out of range -- MUST INIT IN CONSTRUCTOR

  void*         FastEl_(int i)          { return &(el[i]); } // #IGNORE
  const void*   FastEl_(int i) const    { return (const void*)&(el[i]); } // #IGNORE

  int           El_Compare_(const void* a, const void* b) const
  { int rval=-1; if(*((T*)a) > *((T*)b)) rval=1; else if(*((T*)a) == *((T*)b)) rval=0; return rval; }
  // #IGNORE
  void          El_Copy_(void* to, const void* fm)      { *((T*)to) = *((T*)fm); } // #IGNORE
  uint          El_SizeOf_() const              { return sizeof(T); }    // #IGNORE
  void*         El_GetTmp_() const              { return (void*)&tmp; }  // #IGNORE
  const void*   El_GetErr_() const              { return (void*)&err; }  // #IGNORE
/* #ifndef __MAKETA__
  String        El_GetStr_(const void* it) const        { return SC((*((T*)it))); } // #IGNORE
  void          El_SetFmStr_(void* it, const String& val) { *((T*)it) = (T)SC(val); } // #IGNORE
//  String      El_GetStr_(const void* it) const        { return String(*((T*)it)); } // #IGNORE
//  void                El_SetFmStr_(void* it, const String& val) { *((T*)it) = (T)val; } // #IGNORE
#endif */
  taPlainArray(int init_alloc)                  {el = NULL; Alloc(init_alloc); }
  taPlainArray()                                {el = NULL;}
  taPlainArray(const taPlainArray<T>& cp)       {el = NULL; Alloc(cp.size); Duplicate(cp); }
  virtual ~taPlainArray()                       { SetArray_(NULL); } //

  ////////////////////////////////////////////////
  //    functions that return the type          //
  ////////////////////////////////////////////////

  T&            SafeEl(int i) const
  { T* rval=(T*)&err; i=Index(i); if((i >= 0) && (i < size)) rval=&(el[i]); return *rval; }
  // #CAT_Access the element at the given index (index can be - meaning from end of list)
  T&            FastEl(int i)           { return el[i]; }
  // #CAT_Access fast element (no range checking)
  const T&      FastEl(int i) const     { return el[i]; }
  // #CAT_Access fast element (no range checking)
  T&            RevEl(int idx) const            { return SafeEl(size - idx - 1); }
  // #CAT_Access reverse (index) element (ie. get from the back of the list first)
  T&            operator[](int i) const         { return el[i]; }
  void          operator=(const taPlainArray<T>& cp)    { Reset(); Duplicate(cp); }

  virtual T     Pop()
  { T* rval=(T*)&err; if(size>0) rval=&(el[--size]); return *rval; }
  // #CAT_Modify pop the last item in the array off
  virtual T&    Peek() const
  { T* rval=(T*)&err; if(size>0) rval=&(el[size-1]); return *rval; }
  // #CAT_Modify peek at the last item on the array

  ////////////////////////////////////////////////
  //    functions that are passed el of type    //
  ////////////////////////////////////////////////

  virtual void  Set(int i, const T& item)       { SafeEl(i) = item; }
  // #CAT_Modify use this for assigning values to items in the array (Set should update if needed)
  virtual void  Add(const T& item)              { Add_((void*)&item); }
  // #CAT_Modify #MENU add the item to the array
  virtual bool  AddUnique(const T& item)        { return AddUnique_((void*)&item); }
  // #CAT_Modify add the item to the array if it isn't already on it, returns true if unique
  virtual void  Push(const T& item)             { Add(item); }
  // #CAT_Modify push the item on the end of the array (same as add)
  virtual void  Insert(const T& item, int idx, int n_els=1) { Insert_((void*)&item, idx, n_els); }
  // #CAT_Modify #MENU Insert (n_els) item(s) at idx (-1 for end) in the array
  virtual int   FindEl(const T& item, int i=0) const { return FindEl_((void*)&item, i); }
  // #CAT_Access #MENU #USE_RVAL Find item starting from idx in the array (-1 if not there)
  virtual bool  RemoveEl(const T& item)         { return RemoveEl_((void*)&item); }
  // #CAT_Modify remove given item, returns success
protected:
  T             tmp;
  override void*        MakeArray_(int n) const { return new T[n]; }
  override void         SetArray_(void* nw) {if (el) delete [] el; el = (T*)nw;}
};

#endif // taPlainArray_h
