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

#ifndef taArray_impl_h
#define taArray_impl_h 1

// parent includes:
#include "ta_def.h"

// member includes:
#include <taString>

// declare all other types mentioned but not required to include:


class TA_API  taArray_impl {
  // #NO_TOKENS Base Type for Arrays: physically contiguous allocation of an array of objects
public:
  int           size;                   // #NO_SAVE #READ_ONLY #CAT_taArray number of elements in the array
  int           alloc_size;             // #READ_ONLY #NO_SAVE #EXPERT #CAT_taArray allocated (physical) size

  taArray_impl()                        { alloc_size = 0; size = 0; }
  virtual ~taArray_impl()               { alloc_size = 0; size = 0; }

  virtual void          DataChanged(int dcr, void* op1 = NULL, void* op2 = NULL) {}
  // #IGNORE only called when size changes
  inline int            Index(int idx) const { if(idx < 0) idx += size; return idx; }
  // #EXPERT #CAT_Access get actual index from index value that can also be negative, meaning access from end of array
  inline bool           InRange(int idx) const {return ((idx < size) && (idx >= 0));}
  // #EXPERT #CAT_Access is the specified index within range of 0 >= idx < size
  virtual bool          Alloc(int n);
  // #EXPERT #CAT_Modify allocate storage for at least the given size (allocates extra to allow for future expansion -- see AllocExact for exact alloc)
  virtual bool          AllocExact(int n);
  // #EXPERT #CAT_Modify allocate storage for exactly the given size
  virtual void          Reset();
  // #CAT_Modify reset the list to zero size (does not free memory)

  ////////////////////////////////////////////////
  //    internal functions that depend on type  //
  ////////////////////////////////////////////////

  virtual const void*   SafeEl_(int) const;
  // #IGNORE element at posn
  virtual void*         FastEl_(int)    { return NULL; }
  // #IGNORE element at posn
  virtual const void*   FastEl_(int i) const
    {return const_cast<taArray_impl*>(this)->FastEl_(i);}
  virtual bool          El_Equal_(const void*, const void*) const { return false; }
  // #IGNORE for value equality testing
  virtual bool          Equal_(const taArray_impl& ar) const; // value equality, implements == and != operators, ar must be same type
  virtual int           El_Compare_(const void*, const void*) const     { return 0; }
  // #IGNORE for sorting
  virtual void          El_Copy_(void*,const void*)             { };
  // #IGNORE
  virtual uint          El_SizeOf_() const              { return 1; }
  // #IGNORE size of element
  virtual void*         El_GetTmp_() const              { return NULL; }
  // #IGNORE return ptr to Tmp of type
  virtual const void*   El_GetErr_() const {return NULL;}        // #IGNORE
  virtual String        El_GetStr_(const void*) const           { return _nilString; } // #IGNORE
  virtual void          El_SetFmStr_(void*, const String&)      { };       // #IGNORE
  virtual void*         BlankEl_() const                { return NULL; }
  // address of a blank element, for initializing empty items
  virtual void          Clear_Tmp_();                                  // #IGNORE

  virtual void          Add_(const void* it);                   // #IGNORE
  virtual bool          AddUnique_(const void* it);             // #IGNORE
  virtual void          Insert_(const void* it, int where, int n=1); // #IGNORE
  virtual int           FindEl_(const void* it, int where=0) const;     // #IGNORE
  virtual bool          RemoveEl_(const void* it);              // #IGNORE
  virtual void          InitVals_(const void* it, int start=0, int end=-1);// #IGNORE

  ////////////////////////////////////////////////
  // functions that don't depend on the type    //
  ////////////////////////////////////////////////

  virtual String        SafeElAsStr(int idx) const
    {return InRange(idx) ? El_GetStr_(SafeEl_(idx)) : _nilString;}
  // #IGNORE element at idx, as a string
  virtual String        FastElAsStr(int idx) const {return El_GetStr_(FastEl_(idx));}
  // #IGNORE element at idx, as a string
  virtual void  SetSize(int sz);
  // #CAT_Modify #MENU #MENU_ON_Edit #INIT_ARGVAL_ON_size force array to be of given size by inserting blanks or removing

  virtual void  AddBlank(int n_els = 1);
  // #CAT_Modify #MENU #MENU_ON_Edit Add n_els empty elements to the end of the array
  virtual bool  RemoveIdx(uint idx, int n_els=1);
  // #CAT_Modify #MENU #MENU_ON_Edit Remove (n_els) item(s) at idx, returns success
  virtual bool  MoveIdx(int from, int to);
  // #CAT_Modify #MENU move item from index to index
  virtual bool  SwapIdx(int pos1, int pos2);
  // #CAT_Modify #CAT_Modify Swap the elements in the two given positions on the list
  virtual void  Permute();
  // #CAT_Modify #MENU permute the items in the list into a random order
  virtual void  Sort(bool descending=false);
  // #CAT_Modify #MENU sort the list in ascending order (or descending if switched)
  virtual void  ShiftLeft(int nshift);
  // #CAT_Modify shift all the elements in the array to the left by given number of items
  virtual void  ShiftLeftPct(float pct);
  // #CAT_Modify shift the array to the left by given percentage of current size
  virtual int   V_Flip(int width);
  // #EXPERT #CAT_Modify vertically flip the array as if it was arrange in a matrix of width

  virtual void  Duplicate(const taArray_impl& cp);
  // #CAT_Copy duplicate the items in the list
  virtual void  DupeUnique(const taArray_impl& cp);
  // #CAT_Copy duplicate so result is unique list
  virtual void  Copy_Common(const taArray_impl& cp);
  // #CAT_Copy copy elements in common
  virtual void  Copy_Duplicate(const taArray_impl& cp);
  // #CAT_Copy copy elements in common, duplicating (if necc) any extra on cp
  virtual void  CopyVals(const taArray_impl& from, int start=0, int end=-1, int at=0);
  // #CAT_Copy copy values from other array at given start and end points, and putting at given point in this
  virtual String& Print(String& strm, int indent=0) const;
  // #CAT_Display print out all of the elements in the array
  virtual String GetArrayStr() const;
  // #CAT_Display get a string representation of the value of the array (list of items)
  virtual void  InitFromString(const String& val);
  // #CAT_Modify initialize an array from given string (does reset first)
protected:

  void                  AddOnly_(const void* it); // don't notify
  bool                  AddUniqueOnly_(const void* it); // don't notify
  void                  RemoveIdxOnly(int i); //  don't notify, no checks
  virtual void          Reset_impl() {SetSize(0);} // don't notify
  virtual void*         MakeArray_(int i) const { return NULL; } // #IGNORE make a new array of item type
  virtual void          SetArray_(void* nw) {}
  virtual void          ReclaimOrphans_(int start, int end) {}// #IGNORE called when array is shortened, leaving orphaned values; note 'size' may already be trimmed: NOT called when el[] is replaced
  void                  Copy_(const taArray_impl& cp);
  // replace our array with the source items -- note: added in 4.0 for new uses, ex. in Matrix_Array
};

#endif // taArray_impl_h
