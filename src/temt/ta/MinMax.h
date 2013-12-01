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

#ifndef MinMax_h
#define MinMax_h 1

// parent includes:
#include <taOBase>

#ifndef __MAKETA__
# include <math.h>
#endif

// member includes:

// declare all other types mentioned but not required to include:
class float_Matrix; //

taTypeDef_Of(MinMax);

class TA_API MinMax : public taOBase {
  // ##NO_TOKENS #NO_UPDATE_AFTER #INLINE #INLINE_DUMP ##CAT_Math minimum-maximum values
  INHERITED(taOBase)
public:
  float         min;    // minimum value
  float         max;    // maximum value

  bool  operator != (const MinMax& mm) const
  { return ((mm.min != min) || (mm.max != max)); }

  bool  operator == (const MinMax& mm) const
  { return ((mm.min == min) && (mm.max == max)); }

  MinMax& operator =(float val) {min = max = val; return *this;}

  bool  RangeTest(float val) const      { return ((val > min) && (val < max)); }
  // test whether value is within the range (but not equal to max or min)
  bool  RangeTestEq(float val) const    { return ((val >= min) && (val <= max)); }
  // test whether value is within the range (or equal)

  bool  operator < (const float val) const      { return (val < min); }
  bool  operator <= (const float val) const     { return (val <= min); }
  bool  operator > (const float val) const      { return (val > max); }
  bool  operator >= (const float val) const     { return (val >= max); }

  void  Init(float it)  { min = max = it; }  // initializes the max and min to this value
  void  Set(float mn, float mx) { min = mn; max = mx; }  // set values

  inline float  Range() const           { return (max - min); }
  inline float  Scale() const
  { float rval = Range(); if(rval != 0.0f) rval = 1.0f / rval; return rval; }
  // scale is the inverse of range
  inline float  MidPoint() const        { return 0.5f * (min + max); }
  // returns the range between the min and the max

  void  UpdateRange(MinMax& it)
  { min = MIN(it.min, min); max = MAX(it.max, max); }

  void  UpdateRange(float it)
  { min = MIN(it, min); max = MAX(it, max); }  // updates the range

  void  SetRange(float_Matrix& mat); // set the range from a matrix

  void  MaxLT(float it)         { max = MIN(it, max); }
  // max less than (or equal)

  void  MinGT(float it)         { min = MAX(it, min); }
  // min greater than (or equal)

  void  WithinRange(MinMax& it)         // put my range within given one
  { min = MAX(it.min, min); max = MIN(it.max, max); }
  void  WithinRange(float min_, float max_) // #IGNORE put my range within given one
  { min = MAX(min_, min); max = MIN(max_, max); }
  void  SymRange() // symmetrize my range around zero, with max abs value of current min, max
  { float mxabs = MAX(fabsf(min), fabsf(max)); min = -mxabs; max = mxabs; }

  float Normalize(float val) const      { return (val - min) * Scale(); }
  // normalize given value to 0-1 range given current in max

  float Project(float val) const        { return min + (val * Range()); }
  // project a normalized value into the current min-max range

  float Clip(float val) const
  { val = MIN(max,val); val = MAX(min,val); return val; }
  // clip given value within current range

  TA_BASEFUNS_LITE(MinMax);
 protected:
  void  UpdateAfterEdit_impl();
private:
  void  Initialize()            { min = max = 0.0f; }
  void  Destroy()               { };
  void  Copy_(const MinMax& cp) { min = cp.min; max = cp.max; }
};

#endif // MinMax_h
