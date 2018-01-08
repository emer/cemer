// Co2018ght 2016-2017, Regents of the University of Colorado,
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

#ifndef CircBufferIndex_h
#define CircBufferIndex_h 1

// parent includes:
#include "ta_def.h"

// member includes:

// declare all other types mentioned but not required to include:

taTypeDef_Of(CircBufferIndex);

class TA_API CircBufferIndex {
  // Static-only collection of functions that implement circular buffer indexing logic, for using a fixed amount of storage as a wrap-around circular buffer -- very efficient for first-in-first-out moving-window buffers of recent values -- requires a starting index (st_idx), length of buffer (number of items currently stored), and an overall buffer size (maximum number that can be stored), above which the items fall off the end of the buffer
public:

  static inline int   CircIdx(const int cidx, const int st_idx, const int size)
  { int rval = cidx+st_idx; if(rval >= size) rval -= size; return rval; }
  // #CAT_CircAccess gets physical index from logical circular index -- no range checking

  static inline bool  CircIdxInRange(const int cidx, const int length)
  { if(cidx < 0) return false; if(cidx >= length) return false; return true; }
  // #CAT_CircAccess is the given logical circular index within range

  static inline void  CircShiftLeft(const int nshift, int& st_idx, int& length,
                                    const int size)
  { st_idx = CircIdx(nshift, st_idx, size); length -= nshift; }
  // #CAT_CircModify shift the buffer to the left -- shift the first elements off the start of the list, making room at the end for more elements (decreasing length)

  static inline void Reset(int& st_idx, int& length)
  { st_idx = 0; length = 0; }
  // #CAT_CircModify reset the buffer (starting index and length to 0)

  static inline float  CircEl_float(const int cidx, float* buf, int st_idx,
                                   const int size) {
    return buf[CircIdx(cidx, st_idx, size)];
  }
  // #CAT_CircAccess get a float value from a circular buffer
  static inline int  CircEl_int(const int cidx, int* buf, const int st_idx,
                                 const int size) {
    return buf[CircIdx(cidx, st_idx, size)];
  }
  // #CAT_CircAccess get a int value from a circular buffer
  static inline double CircEl_double(const int cidx, double* buf, const int st_idx,
                                   const int size) {
    return buf[CircIdx(cidx, st_idx, size)];
  }
  // #CAT_CircAccess get a int value from a circular buffer

  static inline void  CircAddShift_float(const float& item, float* buf, int& st_idx,
                                         int& length, const int size) {
    if(length >= size) {
      CircShiftLeft((1 + length) - size, st_idx, length, size); // make room
    }
    buf[CircIdx(length++, st_idx, size)] = item;      // set to the element at the end
  }
  // #CAT_CircModify add a new float item to a circular buffer, shifting it left if length is at or above size to ensure a fixed overall length list (otherwise expanding list up to size)

  static inline void  CircAddShift_double(const double& item, double* buf, int& st_idx,
                                         int& length, const int size) {
    if(length >= size) {
      CircShiftLeft((1 + length) - size, st_idx, length, size); // make room
    }
    buf[CircIdx(length++, st_idx, size)] = item;      // set to the element at the end
  }
  // #CAT_CircModify add a new double item to a circular buffer, shifting it left if length is at or above size to ensure a fixed overall length list (otherwise expanding list up to size)

  static inline void  CircAddShift_int(const int& item, int* buf, int& st_idx,
                                         int& length, const int size) {
    if(length >= size) {
      CircShiftLeft((1 + length) - size, st_idx, length, size); // make room
    }
    buf[CircIdx(length++, st_idx, size)] = item;      // set to the element at the end
  }
  // #CAT_CircModify add a new int item to a circular buffer, shifting it left if length is at or above size to ensure a fixed overall length list (otherwise expanding list up to size)

  
};

#endif // CircBufferIndex_h
