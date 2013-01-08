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

#ifndef taFixedArray_impl_h
#define taFixedArray_impl_h 1

// parent includes:
#include "ta_def.h"

// member includes:
#include <taString>

// declare all other types mentioned but not required to include:


class TA_API  taFixedArray_impl {
  // #VIRT_BASE ##NO_INSTANCE #NO_TOKENS basic subtype for FixedArray, no tokens of which are ever kept
public:
  int           size;                   // #NO_SAVE #READ_ONLY number of elements in the array

  taFixedArray_impl()                   {size = 0;}
  virtual ~taFixedArray_impl()          {size = 0;}

  inline int            Index(int idx) const { if(idx < 0) idx += size; return idx; }
  // #EXPERT #CAT_Access get actual index from index value that can also be negative, meaning access from end of list
  inline bool           InRange(int idx) const {return ((idx >= 0) && (idx < size));}
  // #EXPERT #CAT_Access is the specified index within range of 0 >= idx < size
  virtual void          Reset()         {SetSize(0);}
  // #CAT_Modify

  ////////////////////////////////////////////////
  // functions that don't depend on the type    //
  ////////////////////////////////////////////////

  virtual void  SetSize(int sz);
  // #CAT_Modify #MENU #MENU_ON_Edit #INIT_ARGVAL_ON_size force array to be of given size by inserting blanks or removing

public: // accessible but generally not used implementation overrides
  virtual const void*   SafeEl_(int i) const;
  // #IGNORE element at posn; i is allowed to be out of bounds
  virtual void*         FastEl_(int) = 0;
  // #IGNORE element at posn; i must be in bounds

protected:
  virtual bool          Alloc_(uint n); // set capacity to n
  virtual void*         MakeArray_(int i) const = 0; // #IGNORE make a new array of item type
  virtual void          SetArray_(void* nw) = 0;

// compulsory element accessor and manip functions
  virtual const void*   FastEl_(int i) const
    {return const_cast<taFixedArray_impl*>(this)->FastEl_(i);}
  // #IGNORE element at posn; i must be in bounds

  virtual bool          El_Equal_(const void*, const void*) const = 0;
  // #IGNORE for finding
  virtual const void*   El_GetBlank_() const = 0;
  // #IGNORE address of a blank element, for initializing empty items -- can be STATIC_CONST
  // NOTE: this can be implemented by clearing the tmp item, then returning that addr
  virtual const void*   El_GetErr_() const      { return El_GetBlank_();}
  // #IGNORE address of an element to return when out of range -- defaults to blank el
  virtual void          El_Copy_(void*, const void*) = 0;
  // #IGNORE
  virtual uint          El_SizeOf_() const = 0;
  // #IGNORE size of element

  virtual void          Add_(const void* it); // #IGNORE
  virtual bool          AddUnique_(const void* it);             // #IGNORE
  void                  Copy_(const taFixedArray_impl& cp);
  // replace our array with the source items
  virtual void          Insert_(const void* it, int where, int n=1); // #IGNORE
  virtual bool          Equal_(const taFixedArray_impl& src) const;
    // 'true' if not null, and same size and els
  virtual int           Find_(const void* it, int where=0) const;       // #IGNORE -- based on El_Equal_; default is linear
  virtual void          InitVals_(const void* it, int start=0, int end=-1);// #IGNORE
  virtual void          ReclaimOrphans_(int start, int end) {}// #IGNORE called when array is shortened, leaving orphaned values; note 'size' may already be trimmed: NOT called when el[] is replaced

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
  T&            operator[](int i) { return el[i]; } \
  const T&      operator[](int i) const { return el[i]; } \
  y& operator=(const y& cp) {Copy(cp); return *this;} \
  bool          operator==(const y& src) const {return Equal_(src);} \
protected: \
  override const void*  El_GetBlank_() const    { return (const void*)&blank; }

#endif // taFixedArray_impl_h
