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

#ifndef MinMaxInt_h
#define MinMaxInt_h 1

// parent includes:
#include <taBase>

// member includes:

// declare all other types mentioned but not required to include:

taTypeDef_Of(MinMaxInt);

class TA_API MinMaxInt : public taBase {
  // ##NO_TOKENS #NO_UPDATE_AFTER #INLINE #INLINE_DUMP ##CAT_Math minimum-maximum integer values
  INHERITED(taBase)
public:
  int           min;    // minimum value
  int           max;    // maximum value

  bool  operator != (const MinMaxInt& mm) const
  { return ((mm.min != min) || (mm.max != max)); }

  bool  operator == (const MinMaxInt& mm) const
  { return ((mm.min == min) && (mm.max == max)); }

  MinMaxInt& operator =(int val) {min = max = val; return *this;}

  bool  RangeTest(int val) const        { return ((val > min) && (val < max)); }
  // test whether value is within the range (but not equal to max or min)
  bool  RangeTest(float val) const      { return ((val > min) && (val < max)); }
  // test whether value is within the range (but not equal to max or min)
  bool  RangeTestEq(int val) const      { return ((val >= min) && (val <= max)); }
  // test whether value is within the range (or equal)
  bool  RangeTestEq(float val) const    { return ((val >= min) && (val <= max)); }
  // test whether value is within the range (or equal)

  bool  operator < (const int val) const        { return (val < min); }
  bool  operator <= (const int val) const       { return (val <= min); }
  bool  operator > (const int val) const        { return (val > max); }
  bool  operator >= (const int val) const       { return (val >= max); }

  void  Init(int it)    { min = max = it; }  // initializes the max and min to this value

  inline int    Count() const           { return MAX((max - min + 1), 0); }
  inline int    Range() const           { return (max - min); }
  inline float  Scale() const
  { float rval = (float)Range(); if(rval != 0.0f) rval = 1.0f / rval; return rval; }
  // scale is the inverse of range
  inline int    MidPoint() const        { return (int)(0.5f * (float)(min + max)); }
  // returns the range between the min and the max

  void  UpdateRange(MinMaxInt& it)
  { min = MIN(it.min, min); max = MAX(it.max, max); }

  void  UpdateRange(int it)
  { min = MIN(it, min); max = MAX(it, max); }  // updates the range

  void  MaxLT(int it)           { max = MIN(it, max); }
  // max less than (or equal)

  void  MinGT(int it)           { min = MAX(it, min); }
  // min greater than (or equal)

  void  WithinRange(MinMaxInt& it)              // put my range within given one
  { min = MAX(it.min, min); max = MIN(it.max, max); }
  void  WithinRange(int min_, int max_) // #IGNORE put my range within given one
  { min = MAX(min_, min); max = MIN(max_, max); }

  float Normalize(int val) const        { return (val - min) * Scale(); }
  // normalize given value to 0-1 range given current in max

  int   Project(int val) const  { return min + (val * Range()); }
  // project a normalized value into the current min-max range

  int   Clip(int val) const
  { val = MIN(max,val); val = MAX(min,val); return val; }
  // clip given value within current range

  TA_BASEFUNS_LITE(MinMaxInt);
private:
  void  Initialize()            { min = max = 0; }
  void  Destroy()               { };
  void  Copy_(const MinMaxInt& cp)      { min = cp.min; max = cp.max; }
};

#endif // MinMaxInt_h
