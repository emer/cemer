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

#ifndef axis_h
#define axis_h

/*
 * Axis - position tick marks as on axis
 */

#include <ta_misc/minmax.h>

#include <ta/enter_iv.h>
#include <InterViews/color.h>
#include <InterViews/patch.h>
#include <ta/leave_iv.h>

class ivPage;
class ivGlyph;
class ivFont;

class Axis : public ivPatch {
public:
  MinMax	range;		// display range of the axis data
  MinMax	true_range;     // actual min and max of data (including fixed range)
  MinMax	eff_range;     	// effective range: can be changed if display contains different info than basic graph (e.g., multi-traces)
  FixedMinMax	fixed_range;	// fixed range: optionally constrained range values
  int          	n_ticks;	// number of ticks desired
  float		start_tick;	// first tick mark here
  float       	tick_incr;	// increment for tick marks
  int		act_n_ticks;	// actual number of ticks
  double	units;		// order of the units displayed (i.e. divide by this)
  float		range_scale;	// re-scaling of range over actual based on zooming
  float		min_percent;	// percent of new (zoomed) range above min
  float		max_percent;	// percent of new (zoomed) range below max

  virtual float GetMin();	// get effective min value (includes min_percent)
  virtual float GetMax();	// get effective max value (includes max_percent)
  virtual float	GetRange() 	{ return GetMax() - GetMin(); }

  virtual void	InitRange(float first, float last);
  // initialize range to known good starting range values (fixed vals will still override)
  virtual bool 	UpdateRange(float first, float last);
  // update range with new min/max data -- returns true if range actually changed
  virtual void	ComputeTickIncr();
  virtual void 	UpdateAxis();

  virtual ivGlyph** Make_Ticks(const ivFont* font, const ivColor* c); 

  virtual void 	allocate(ivCanvas*, const ivAllocation&, ivExtension&);
  Axis(float first, float last, ivColor* c);
  virtual ~Axis();
protected:
  ivAllocation  axis_allocation; // this is here since patch's alloc is private!
  ivColor*      mycolor;
  ivPage* 	_page;
};

class YAxis : public Axis {
public:
  virtual ivGlyph** Make_Ticks(const ivFont* font, const ivColor* c);
  YAxis(float first, float last,ivColor*);
  virtual void request (ivRequisition& r) const;
  virtual void UpdateAxis();
protected:
  virtual ~YAxis();
};

class XAxis : public Axis {
public:
  virtual ivGlyph** Make_Ticks(const ivFont* font, const ivColor* c);
  XAxis(float first, float last,ivColor*);
  virtual void request (ivRequisition& r) const;
  virtual void UpdateAxis();
protected:
    virtual ~XAxis();
};

#endif // axis_h

