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

#ifndef taUndoDiffSrc_List_h
#define taUndoDiffSrc_List_h 1

// parent includes:
#include <taUndoDiffSrc>
#include <taList>

// member includes:

// declare all other types mentioned but not required to include:


TypeDef_Of(taUndoDiffSrc_List);

class TA_API taUndoDiffSrc_List : public taList<taUndoDiffSrc> {
  // ##CAT_Undo list of full source records for diff-based saving of undo save data -- managed using circular buffer logic per new functions
INHERITED(taList<taUndoDiffSrc>)
public:
  int           st_idx;         // #READ_ONLY index in underlying array where the list starts (i.e., the position of the logical 0 index) -- updated by functions and should not be set manually
  int           length;         // #READ_ONLY logical length of the list -- is controlled by adding and shifting, and should NOT be set manually

  int   CircIdx(int cidx) const
  { int rval = cidx+st_idx; if(rval >= size) rval -= size; return rval; }
  // #CAT_CircAccess gets physical index from logical circular index

  bool  CircIdxInRange(int cidx) const { return InRange(CircIdx(cidx)); }
  // #CAT_CircAccess check if logical circular index is in range

  taUndoDiffSrc*        CircSafeEl(int cidx) const { return SafeEl(CircIdx(cidx)); }
  // #CAT_CircAccess returns element at given logical circular index, or NULL if out of range

  taUndoDiffSrc*        CircPeek() const {return SafeEl(CircIdx(length-1));}
  // #CAT_CircAccess returns element at end of circular buffer

  /////////////////////////////////////////////////////////
  //    Special Modify Routines

  void          CircShiftLeft(int nshift)
  { nshift = MIN(length, nshift);
    if(nshift > 0) { st_idx = CircIdx(nshift); length -= nshift; } }
  // #CAT_CircModify shift the buffer to the left -- shift the first elements off the start of the list, making room at the end for more elements (decreasing length)

  void          CircAddExpand(taUndoDiffSrc* item) {
    if((st_idx == 0) && (length >= size)) {
      inherited::Add(item); length++;   // must be building up the list, so add it
    }
    else {
      ReplaceIdx(CircIdx(length++), item);      // expand the buffer length and set to the element at the end
    }
  }
  // #CAT_CircModify add a new item to the circular buffer, expanding the length of the list by 1 under all circumstances

  void          CircAddLimit(taUndoDiffSrc* item, int max_length) {
    if(length >= max_length) {
      CircShiftLeft(1 + length - max_length); // make room
      ReplaceIdx(CircIdx(length++), item);      // set to the element at the end
    }
    else {
      CircAddExpand(item);
    }
  }
  // #CAT_CircModify add a new item to the circular buffer, shifting it left if length is at or above max_length to ensure a fixed overall length list (otherwise expanding list up to max_length)

  override void Reset();

  void  Copy_(const taUndoDiffSrc_List& cp);
  TA_BASEFUNS(taUndoDiffSrc_List);
private:
  void  Initialize();
  void  Destroy()               { };
};

#endif // taUndoDiffSrc_List_h
