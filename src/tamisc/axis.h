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

