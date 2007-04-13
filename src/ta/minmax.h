// Copyright, 1995-2005, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of TA/CSS
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



#ifndef minmax_h
#define minmax_h

#include "ta_base.h"
#include "ta_def.h"
#include "ta_TA_type.h"

#ifndef __MAKETA__
# include <math.h>
#endif

double nicenum(double x, bool round);

// externals
class float_Matrix;


class TA_API MinMax : public taBase {
  // ##NO_TOKENS #NO_UPDATE_AFTER #INLINE #INLINE_DUMP ##CAT_Math minimum-maximum values
  INHERITED(taBase)
public:
  float		min;	// minimum value
  float		max;	// maximum value

  bool	operator != (const MinMax& mm) const
  { return ((mm.min != min) || (mm.max != max)); }

  bool	operator == (const MinMax& mm) const
  { return ((mm.min == min) && (mm.max == max)); }

  MinMax& operator =(float val) {min = max = val; return *this;}

  bool 	RangeTest(float val) const	{ return ((val > min) && (val < max)); }
  // test whether value is within the range (but not equal to max or min)
  bool 	RangeTestEq(float val) const	{ return ((val >= min) && (val <= max)); }
  // test whether value is within the range (or equal)

  bool  operator < (const float val) const	{ return (val < min); }
  bool  operator <= (const float val) const	{ return (val <= min); }
  bool  operator > (const float val) const	{ return (val > max); }
  bool  operator >= (const float val) const	{ return (val >= max); }

  void	Init(float it)	{ min = max = it; }  // initializes the max and min to this value
  void	Set(float mn, float mx)	{ min = mn; max = mx; }  // set values

  inline float	Range()	const		{ return (max - min); }
  inline float	Scale()	const
  { float rval = Range(); if(rval != 0.0f) rval = 1.0f / rval; return rval; }
  // scale is the inverse of range
  inline float	MidPoint() const	{ return 0.5f * (min + max); }
  // returns the range between the min and the max

  void	UpdateRange(MinMax& it)
  { min = MIN(it.min, min); max = MAX(it.max, max); }

  void	UpdateRange(float it)
  { min = MIN(it, min);	max = MAX(it, max); }  // updates the range

  void	SetRange(float_Matrix& mat); // set the range from a matrix
  
  void	MaxLT(float it)		{ max = MIN(it, max); }
  // max less than (or equal)

  void	MinGT(float it)		{ min = MAX(it, min); }
  // min greater than (or equal)

  void  WithinRange(MinMax& it)		// put my range within given one
  { min = MAX(it.min, min); max = MIN(it.max, max); }
  void  WithinRange(float min_, float max_) // #IGNORE put my range within given one
  { min = MAX(min_, min); max = MIN(max_, max); }

  float	Normalize(float val) const	{ return (val - min) * Scale(); }
  // normalize given value to 0-1 range given current in max

  float	Project(float val) const	{ return min + (val * Range()); }
  // project a normalized value into the current min-max range

  float	Clip(float val) const
  { val = MIN(max,val); val = MAX(min,val); return val; }
  // clip given value within current range

  TA_BASEFUNS_LITE(MinMax);
 protected:
  void	UpdateAfterEdit_impl();
private:
  void 	Initialize() 		{ min = max = 0.0f; }
  void 	Destroy()		{ };
  void 	Copy_(const MinMax& cp)	{ min = cp.min; max = cp.max; }
};

class TA_API MinMaxRange : public MinMax {
  // min-max values plus scale and range #INLINE #INLINE_DUMP
  INHERITED(MinMax)
public:
  float		range;		// #HIDDEN distance between min and max
  float		scale;		// #HIDDEN scale (1.0 / range)

  float	Normalize(float val) const	{ return (val - min) * scale; }
  // normalize given value to 0-1 range given current in max

  float	Project(float val) const	{ return min + (val * range); }
  // project a normalized value into the current min-max range

  TA_BASEFUNS_LITE(MinMaxRange);
protected:
  void	UpdateAfterEdit_impl()
  { inherited::UpdateAfterEdit_impl(); 
    range = Range(); if(range != 0.0f) scale = 1.0f / range; }
private:
  void 	Initialize() 		{ range = scale = 0.0f; }
  void 	Destroy()		{ };
  void 	Copy_(const MinMaxRange& cp)	{ range = cp.range; scale = cp.scale; }
};

class TA_API FixedMinMax : public taBase {
  // ##NO_TOKENS #NO_UPDATE_AFTER #INLINE #INLINE_DUMP ##CAT_Math minimum-maximum values with toggles for usage
INHERITED(taBase)
public:
  bool		fix_min;	// use fixed minimum value?
  float		min;		// minimum value
  bool		fix_max;	// use fixed maximum value?
  float		max;		// maximum value

  void	Init(float it)	{ min = max = it; }  // initializes the max and min to this value
  void	InitFix(bool fx = false) { fix_min = fix_max = fx; }
  void	Set(float mn, float mx)	{ min = mn; max = mx; }
  void	SetMin(float mn) { min = mn; fix_min = true; }
  void	SetMax(float mx) { max = mx; fix_max = true; }
  void	FixRange(MinMax& mm) { if(fix_min) mm.min = min; if(fix_max) mm.max = max; }

  TA_BASEFUNS_LITE(FixedMinMax);
private:
  void 	Initialize();
  void 	Destroy()		{ };
  void 	Copy_(const FixedMinMax& cp);
};

class TA_API Modulo : public taOBase {
  // ##NO_TOKENS ##NO_UPDATE_AFTER #INLINE #INLINE_DUMP ##CAT_Math modulo for things that happen periodically
  INHERITED(taOBase)
public:
  bool	         flag;		// Modulo is active?
  int		 m;	        // Modulo N mod m, where N is counter
  int		 off;		// Modulo Offset (actually (N - off) mod m

  TA_BASEFUNS_LITE(Modulo);
protected:
  void	UpdateAfterEdit_impl();
private:
  void	Initialize();
  void 	Destroy()		{ };
  void	Copy_(const Modulo& cp);
};

class TA_API MinMaxInt : public taBase {
  // ##NO_TOKENS #NO_UPDATE_AFTER #INLINE #INLINE_DUMP ##CAT_Math minimum-maximum integer values
  INHERITED(taBase)
public:
  int		min;	// minimum value
  int		max;	// maximum value

  bool	operator != (const MinMaxInt& mm) const
  { return ((mm.min != min) || (mm.max != max)); }

  bool	operator == (const MinMaxInt& mm) const
  { return ((mm.min == min) && (mm.max == max)); }

  MinMaxInt& operator =(int val) {min = max = val; return *this;}

  bool 	RangeTest(int val) const	{ return ((val > min) && (val < max)); }
  // test whether value is within the range (but not equal to max or min)
  bool 	RangeTest(float val) const	{ return ((val > min) && (val < max)); }
  // test whether value is within the range (but not equal to max or min)
  bool 	RangeTestEq(int val) const	{ return ((val >= min) && (val <= max)); }
  // test whether value is within the range (or equal)
  bool 	RangeTestEq(float val) const	{ return ((val >= min) && (val <= max)); }
  // test whether value is within the range (or equal)

  bool  operator < (const int val) const	{ return (val < min); }
  bool  operator <= (const int val) const	{ return (val <= min); }
  bool  operator > (const int val) const	{ return (val > max); }
  bool  operator >= (const int val) const	{ return (val >= max); }

  void	Init(int it)	{ min = max = it; }  // initializes the max and min to this value

  inline int	Count()	const		{ return MAX((max - min + 1), 0); }
  inline int	Range()	const		{ return (max - min); }
  inline float	Scale()	const
  { float rval = (float)Range(); if(rval != 0.0f) rval = 1.0f / rval; return rval; }
  // scale is the inverse of range
  inline int	MidPoint() const	{ return (int)(0.5f * (float)(min + max)); }
  // returns the range between the min and the max

  void	UpdateRange(MinMaxInt& it)
  { min = MIN(it.min, min); max = MAX(it.max, max); }

  void	UpdateRange(int it)
  { min = MIN(it, min);	max = MAX(it, max); }  // updates the range

  void	MaxLT(int it)		{ max = MIN(it, max); }
  // max less than (or equal)

  void	MinGT(int it)		{ min = MAX(it, min); }
  // min greater than (or equal)

  void  WithinRange(MinMaxInt& it)		// put my range within given one
  { min = MAX(it.min, min); max = MIN(it.max, max); }
  void  WithinRange(int min_, int max_) // #IGNORE put my range within given one
  { min = MAX(min_, min); max = MIN(max_, max); }

  float	Normalize(int val) const	{ return (val - min) * Scale(); }
  // normalize given value to 0-1 range given current in max

  int	Project(int val) const	{ return min + (val * Range()); }
  // project a normalized value into the current min-max range

  int	Clip(int val) const
  { val = MIN(max,val); val = MAX(min,val); return val; }
  // clip given value within current range

  TA_BASEFUNS_LITE(MinMaxInt);
private:
  void 	Initialize() 		{ min = max = 0; }
  void 	Destroy()		{ };
  void 	Copy_(const MinMaxInt& cp)	{ min = cp.min; max = cp.max; }
};

#endif // minmax_h



