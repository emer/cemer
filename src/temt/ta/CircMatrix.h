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

#ifndef CircMatrix_h
#define CircMatrix_h 1

// parent includes:
#include <taNBase>

// member includes:
#include <taMatrix>

// declare all other types mentioned but not required to include:


taTypeDef_Of(CircMatrix);

class TA_API CircMatrix : public taNBase {
  // ##TOKENS #CAT_Data Circular buffer logic on top of a matrix -- efficient way to store a fixed window of state information without actually shifting memory around -- use CircAdd to initially populate and CircShiftLeft to make room for new items -- the *LAST* dimension is the circular one (i.e., the "frame" dimension)
INHERITED(taNBase)
public:
  taMatrixRef   matrix;         // the matrix to control using circ buffer logic -- must be set -- routines do not ensure validity of this pointer!
  int           st_idx;         // #READ_ONLY index in matrix frame where the list starts (i.e., the position of the logical 0 index) -- updated by functions and should not be set manually
  int           length;         // #READ_ONLY logical length of the list -- is controlled by adding and shifting, and should NOT be set manually

  /////////////////////////////////////////////////////////
  //    Special Access Routines

  int   CircIdx(int cidx) const
  { int rval = cidx+st_idx; if(rval >= matrix->Frames()) rval -= matrix->Frames(); return rval; }
  // #CAT_CircAccess gets physical index from logical circular index

  int   CircIdx_Last() const
  { return CircIdx(length-1); }
  // #CAT_CircAccess gets physical index from logical circular index for the last item on the list (most recently added)

  /////////////////////////////////////////////////////////
  //    Special Modify Routines

  void          CircShiftLeft(int nshift)
  { st_idx = CircIdx(nshift); length -= nshift; }
  // #CAT_CircModify shift the buffer to the left -- shift the first elements off the start of the list, making room at the end for more elements (decreasing length)

  int           CircAddExpand() {
    if((st_idx == 0) && (length >= matrix->Frames())) {
      matrix->AddFrame(); length++;     // must be building up the list, so add it
    }
    else {
      length++; // expand the buffer length and set to the element at the end
    }
    return length-1;
  }
  // #CAT_CircModify add a new frame to the circular buffer, expanding the length of the list by 1 under all circumstances -- returns logical circidx for frame to set data at

  int           CircAddLimit(int max_length) {
    if(length >= max_length) {
      CircShiftLeft(1 + length - max_length); // make room
      length++;
    }
    else {
      CircAddExpand();
    }
    return length-1;
  }
  // #CAT_CircModify add a new frame to the circular buffer, shifting it left if length is at or above max_length to ensure a fixed overall length list (otherwise expanding list up to max_length)

  virtual void  Reset();
  // resets *JUST* the indicies in this object -- does not reset matrix

  TA_SIMPLE_BASEFUNS(CircMatrix);
private:
  void  Initialize();
  void  Destroy()               { };
};

#endif // CircMatrix_h
