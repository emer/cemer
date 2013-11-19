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

#ifndef taArray_h
#define taArray_h 1

// parent includes:
#include <taArray_base>

// member includes:

// declare all other types mentioned but not required to include:

taTypeDef_Of(taArray);

template<class T>
class taArray : public taArray_base {
  // #VIRT_BASE #NO_TOKENS #NO_UPDATE_AFTER
INHERITED(taArray_base)
public:
  T*            el;             // #NO_SHOW #NO_SAVE Pointer to actual array memory
  T             err;            // #NO_SHOW what is returned when out of range; MUST INIT IN CONSTRUCTOR

  ////////////////////////////////////////////////
  //    functions that return the type          //
  ////////////////////////////////////////////////

  const T&      SafeEl(int i) const {return *(T*)SafeEl_(i);}
  // #MENU #MENU_ON_Edit #USE_RVAL the element at the given index
  T&            FastEl(int i)  {return el[i]; }
  // fast element (no range checking)
  const T&      FastEl(int i) const     {return el[i]; }
  // fast element (no range checking)
  const T&      RevEl(int idx) const    { return SafeEl(size - idx - 1); }
  // reverse (index) element (ie. get from the back of the list first)

  const T       Pop()
    {if (size == 0) return *(static_cast<const T*>(El_GetErr_()));
     else return el[--size]; }
  // pop the last item in the array off
  const T& Peek() const {return SafeEl(size - 1);}
  // peek at the last item on the array

  ////////////////////////////////////////////////
  //    functions that are passed el of type    //
  ////////////////////////////////////////////////

  void  Set(int i, const T& item)
    { if (InRange(i)) el[i] = item; }
  // use this for safely assigning values to items in the array (Set should update if needed)
  void  Add(const T& item)              { Add_((void*)&item); }
  // #MENU add the item to the array
  bool  AddUnique(const T& item)        { return AddUnique_((void*)&item); }
  // add the item to the array if it isn't already on it, returns true if unique
  void  Push(const T& item)             { Add(item); }
  // #CAT_Modify push the item on the end of the array (same as add)
  void  Insert(const T& item, int indx, int n_els=1)    { Insert_((void*)&item, indx, n_els); }
  // #MENU Insert (n_els) item(s) at indx (-1 for end) in the array
  int   FindEl(const T& item, int indx=0) const { return FindEl_((void*)&item, indx); }
  // #MENU #USE_RVAL Find item starting from indx in the array (-1 if not there)
  virtual bool  RemoveEl(const T& item)         { return RemoveEl_((void*)&item); }
  // remove given item, returns success
  virtual void  InitVals(const T& item, int start=0, int end=-1) { InitVals_((void*)&item, start, end); }
  // set array elements to specified value starting at start through end (-1 = size)

  TA_TMPLT_BASEFUNS_NOCOPY(taArray,T); //
public:
  void*         FastEl_(int i)          { return &(el[i]); }// #IGNORE
  const void*   FastEl_(int i) const    { return (const void*)&(el[i]); } // #IGNORE
protected:
  mutable T             tmp; // #IGNORE temporary item

  override void*        MakeArray_(int n) const { return new T[n]; }
  override void         SetArray_(void* nw) {if (el) delete [] el; el = (T*)nw;}
  void          El_Copy_(void* to, const void* fm) { *((T*)to) = *((T*)fm); }
  uint          El_SizeOf_() const              { return sizeof(T); }
  const void*   El_GetErr_() const              { return (void*)&err; }
  void*         El_GetTmp_() const              { return (void*)&tmp; } //
private:
  void  Initialize() { el = NULL; }
  void  Destroy() { SetArray_(NULL); }
};

#endif // taArray_h
