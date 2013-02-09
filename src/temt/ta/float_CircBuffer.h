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

#ifndef float_CircBuffer_h
#define float_CircBuffer_h 1

// parent includes:
#include <float_Array>

// member includes:

// declare all other types mentioned but not required to include:


taTypeDef_Of(float_CircBuffer);

class TA_API float_CircBuffer : public float_Array {
  // ##NO_TOKENS Circular buffer for holding state information -- efficient way to store a fixed window of state information without actually shifting memory around -- use CircAdd to initially populate and CircShiftLeft to make room for new items
INHERITED(float_Array)
public:
  int           st_idx;         // #READ_ONLY index in underlying array where the list starts (i.e., the position of the logical 0 index) -- updated by functions and should not be set manually
  int           length;         // #READ_ONLY logical length of the list -- is controlled by adding and shifting, and should NOT be set manually

  /////////////////////////////////////////////////////////
  //    Special Access Routines

  int   CircIdx(int cidx) const
  { int rval = cidx+st_idx; if(rval >= size) rval -= size; return rval; }
  // #CAT_CircAccess gets physical index from logical circular index

  bool  CircIdxInRange(int cidx) const { return InRange(CircIdx(cidx)); }
  // #CAT_CircAccess check if logical circular index is in range

  const float&  CircSafeEl(int cidx) const { return SafeEl(CircIdx(cidx)); }
  // #CAT_CircAccess returns element at given logical circular index, or err value which is 0.0

  const float&  CircPeek() const {return SafeEl(CircIdx(length-1));}
  // #CAT_CircAccess returns element at end of circular buffer

  /////////////////////////////////////////////////////////
  //    Special Modify Routines

  void          CircShiftLeft(int nshift)
  { st_idx = CircIdx(nshift); length -= nshift; }
  // #CAT_CircModify shift the buffer to the left -- shift the first elements off the start of the list, making room at the end for more elements (decreasing length)

  void          CircAddExpand(const float& item) {
    if((st_idx == 0) && (length >= size)) {
      inherited::Add(item); length++;   // must be building up the list, so add it
    }
    else {
      Set(CircIdx(length++), item);     // expand the buffer length and set to the element at the end
    }
  }
  // #CAT_CircModify add a new item to the circular buffer, expanding the length of the list by 1 under all circumstances

  void          CircAddLimit(const float& item, int max_length) {
    if(length >= max_length) {
      CircShiftLeft(1 + length - max_length); // make room
      Set(CircIdx(length++), item);     // set to the element at the end
    }
    else {
      CircAddExpand(item);
    }
  }
  // #CAT_CircModify add a new item to the circular buffer, shifting it left if length is at or above max_length to ensure a fixed overall length list (otherwise expanding list up to max_length)

  override void Reset();

  void  Copy_(const float_CircBuffer& cp);
  TA_BASEFUNS(float_CircBuffer);
private:
  void  Initialize();
  void  Destroy()               { };
};

#endif // float_CircBuffer_h
