/* -*- C++ -*- */
/*=============================================================================
//									      //
// This file is part of the TypeAccess/C-Super-Script software package.	      //
//									      //
// Copyright (C) 1995 Randall C. O'Reilly, Chadley K. Dawson, 		      //
//		      James L. McClelland, and Carnegie Mellon University     //
//     									      //
// Permission to use, copy, modify, and distribute this software and its      //
// documentation for any purpose is hereby granted without fee, provided that //
// the above copyright notice and this permission notice appear in all copies //
// of the software and related documentation.                                 //
// 									      //
// Note that the PDP++ software package, which contains this package, has a   //
// more restrictive copyright, which applies only to the PDP++-specific       //
// portions of the software, which are labeled as such.			      //
//									      //
// Note that the taString class, which is derived from the GNU String class,  //
// is Copyright (C) 1988 Free Software Foundation, written by Doug Lea, and   //
// is covered by the GNU General Public License, see ta_string.h.             //
// The iv_graphic library and some iv_misc classes were derived from the      //
// InterViews morpher example and other InterViews code, which is             //
// Copyright (C) 1987, 1988, 1989, 1990, 1991 Stanford University             //
// Copyright (C) 1991 Silicon Graphics, Inc.				      //
//									      //
// THE SOFTWARE IS PROVIDED "AS-IS" AND WITHOUT WARRANTY OF ANY KIND,         //
// EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY 	      //
// WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.  	      //
// 									      //
// IN NO EVENT SHALL CARNEGIE MELLON UNIVERSITY BE LIABLE FOR ANY SPECIAL,    //
// INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY KIND, OR ANY DAMAGES  //
// WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER OR NOT     //
// ADVISED OF THE POSSIBILITY OF DAMAGE, AND ON ANY THEORY OF LIABILITY,      //
// ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS        //
// SOFTWARE. 								      //
==============================================================================*/

#ifndef minmax_h
#define minmax_h

#include "ta_base.h"
#include "tamisc_TA_type.h"

#include <math.h>

double nicenum(double x, bool round);

class MinMax : public taBase {
  // ##NO_TOKENS #NO_UPDATE_AFTER #INLINE minimum-maximum values
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

  void	MaxLT(float it)		{ max = MIN(it, max); }
  // max less than (or equal)

  void	MinGT(float it)		{ min = MAX(it, min); }
  // min greater than (or equal)

  void  WithinRange(MinMax& it)		// put my range within given one
  { min = MAX(it.min, min); max = MIN(it.max, max); }

  float	Normalize(float val) const	{ return (val - min) * Scale(); }
  // normalize given value to 0-1 range given current in max

  float	Project(float val) const	{ return min + (val * Range()); }
  // project a normalized value into the current min-max range

  float	Clip(float val) const
  { val = MIN(max,val); val = MAX(min,val); return val; }
  // clip given value within current range

  void 	Initialize() 		{ min = max = 0.0f; }
  void 	Destroy()		{ };
  void 	Copy_(const MinMax& cp)	{ min = cp.min; max = cp.max; }
  COPY_FUNS(MinMax, taBase);
  TA_BASEFUNS(MinMax);
};

class MinMaxRange : public MinMax {
  // min-max values plus scale and range #INLINE
public:
  float		range;		// #HIDDEN distance between min and max
  float		scale;		// #HIDDEN scale (1.0 / range)

  void	UpdateAfterEdit()
  { range = Range(); if(range != 0.0f) scale = 1.0f / range; }

  float	Normalize(float val) const	{ return (val - min) * scale; }
  // normalize given value to 0-1 range given current in max

  float	Project(float val) const	{ return min + (val * range); }
  // project a normalized value into the current min-max range

  void 	Initialize() 		{ range = scale = 0.0f; }
  void 	Destroy()		{ };
  void 	Copy_(const MinMaxRange& cp)	{ range = cp.range; scale = cp.scale; }
  COPY_FUNS(MinMaxRange, MinMax);
  TA_BASEFUNS(MinMaxRange);
};

class FixedMinMax : public taBase {
  // ##NO_TOKENS #NO_UPDATE_AFTER #INLINE minimum-maximum values with toggles for usage
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

  void 	Initialize();
  void 	Destroy()		{ };
  void 	Copy_(const FixedMinMax& cp);
  COPY_FUNS(FixedMinMax, taBase);
  TA_BASEFUNS(FixedMinMax);
};

class Modulo : public taOBase {
  // ##NO_TOKENS ##NO_UPDATE_AFTER #INLINE modulo for things that happen periodically
public:
  bool	         flag;		// Modulo is active?
  int		 m;	        // Modulo N mod m, where N is counter
  int		 off;		// Modulo Offset (actually (N - off) mod m

  void	UpdateAfterEdit();
  void	Initialize();
  void 	Destroy()		{ };
  void	Copy_(const Modulo& cp);
  COPY_FUNS(Modulo, taOBase);
  TA_BASEFUNS(Modulo);
};

class MinMaxInt : public taBase {
  // ##NO_TOKENS #NO_UPDATE_AFTER #INLINE minimum-maximum values
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

  float	Normalize(int val) const	{ return (val - min) * Scale(); }
  // normalize given value to 0-1 range given current in max

  int	Project(int val) const	{ return min + (val * Range()); }
  // project a normalized value into the current min-max range

  int	Clip(int val) const
  { val = MIN(max,val); val = MAX(min,val); return val; }
  // clip given value within current range

  void 	Initialize() 		{ min = max = 0; }
  void 	Destroy()		{ };
  void 	Copy_(const MinMaxInt& cp)	{ min = cp.min; max = cp.max; }
  COPY_FUNS(MinMaxInt, taBase);
  TA_BASEFUNS(MinMaxInt);
};

#endif // minmax_h



