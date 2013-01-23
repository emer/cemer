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

#ifndef taFixedArray_h
#define taFixedArray_h 1

// parent includes:
#include <taFixedArray_impl>

// member includes:

// declare all other types mentioned but not required to include:

TypeDef_Of(taFixedArray);

template<class T>
class taFixedArray : public taFixedArray_impl {
  // #INSTANCE #NO_TOKENS rudimentary array, primarily intended as an OO replacement for C arrays
public:
  T*            el;             // #HIDDEN #NO_SAVE Pointer to actual array memory

  taFixedArray()        {el = NULL;}
  virtual ~taFixedArray()       { SetArray_(NULL); } //

  ////////////////////////////////////////////////
  //    functions that return the type          //
  ////////////////////////////////////////////////

  const T& SafeEl(int i) const { return *(static_cast<const T*>(SafeEl_(i))); }
  // the element at the given index
  const T&      FastEl(int i) const     { return el[i]; }
  // fast element (no range checking)
  T&            FastEl(int i)           { return el[i]; }
  // fast element (no range checking)
  void          Copy(const taFixedArray<T>& cp) {Copy_(cp);} // #IGNORE buggy maketa

  ////////////////////////////////////////////////
  //    functions that are passed el of type    //
  ////////////////////////////////////////////////

  virtual void  Set(int i, const T& item)       { FastEl(i) = item; }
  // use this for assigning values to items in the array (Set should update if needed)
  virtual void  Add(const T& item)              { Add_((const void*)&item); }
  // #MENU add the item to the array
  virtual bool  AddUnique(const T& item)        { return AddUnique_((void*)&item); }
  // add the item to the array if it isn't already on it, returns true if unique
  virtual void  Insert(const T& item, int idx, int n_els=1)
  { Insert_((void*)&item, idx, n_els); }
  // #MENU Insert (n_els) item(s) at idx (-1 for end) in the array
  virtual int   Find(const T& item, int i=0) const { return Find_((void*)&item, i); }
  // #MENU #USE_RVAL Find item starting from idx in the array (-1 if not there)
public:
  override void*        FastEl_(int i)          { return &(el[i]); }
  override const void*  FastEl_(int i) const    { return (const void*)&(el[i]); }
protected:
  override void*        MakeArray_(int n) const { return new T[n]; }
  override void         SetArray_(void* nw) {if (el) delete [] el; el = (T*)nw;}
  override bool         El_Equal_(const void* a, const void* b) const
    { return (*((T*)a) == *((T*)b)); }
  override void         El_Copy_(void* to, const void* fm) { *((T*)to) = *((T*)fm); }
  override uint         El_SizeOf_() const      { return sizeof(T); }
};

#endif // taFixedArray_h
