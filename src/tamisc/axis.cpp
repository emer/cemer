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



// axis.cc
#include <ta_misc/axis.h>

#include <ta/enter_iv.h>
#include <InterViews/display.h>
#include <InterViews/font.h>
#include <InterViews/layout.h>
#include <InterViews/label.h>
#include <InterViews/page.h>
#include <InterViews/patch.h>
#include <InterViews/rule.h>
#include <InterViews/session.h>
#include <IV-look/kit.h>
#include <ta/leave_iv.h>

#include <math.h>
#include <ta_string/ta_string.h>


//////////////////////////
// 	Axis		//
//////////////////////////
 
Axis::Axis (float first, float last, ivColor* c) : ivPatch(nil) {
  _page = nil;
  eff_range.min = true_range.min = range.min = first;
  eff_range.max = true_range.max = range.max = last;
  start_tick = first;
  tick_incr = 1;
  n_ticks = 10;
  units = 1.;
  act_n_ticks = 11;
  range_scale = 1.0f;
  min_percent = max_percent = 0.0f;
  mycolor = c;
  ivResource::ref(mycolor);
}

Axis::~Axis() {
  ivResource::unref(mycolor);
}

void Axis::InitRange(float first, float last) {
  true_range.Set(first, last);
  fixed_range.FixRange(true_range); // keep range fixed!
  range = true_range;
  eff_range = range;
}

void Axis::allocate (ivCanvas* c, const ivAllocation& a, ivExtension& ext) {
  ivPatch::allocate(c,a,ext);
  if(!axis_allocation.equals(a,0.001f)) {
    UpdateAxis(); 
    ivPatch::allocate(c,a,ext);
  }
  axis_allocation = a;   
}


/* from xmgr, graphutils.c, copyright P. Turner
 * nicenum: find a "nice" number approximately equal to x
 * round if round=true, ceil if round=false
 */

static double nicenum(double x, bool round) {
  double y;
  if(x <= 0.0)
     return 0.0;
  int exp = (int)floor(log10(x));
  double f = x / pow(10.0, (double) exp);	/* fraction between 1 and 10 */
  if (round)
    if (f < 1.5)
      y = 1.;
    else if (f < 3.)
      y = 2.;
    else if (f < 7.)
      y = 5.;
    else
      y = 10.;
  else if (f <= 1.)
    y = 1.;
  else if (f <= 2.)
    y = 2.;
  else if (f <= 5.)
    y = 5.;
  else
    y = 10.;
  return y * pow(10.0, (double)exp);
}

void Axis::ComputeTickIncr() {
  double min = GetMin();
  double max = GetMax();
  double rng = nicenum(max-min, false);
  if(rng == 0.0) {
    double unitsval = fabs(min);
    if(unitsval == 0.0)
      units = 1.0;
    else
      units = log10(unitsval);
    start_tick = min;
    tick_incr = 1.0 * units;
    act_n_ticks = 1;
    return;
  }
    
  tick_incr = nicenum((rng / (double)n_ticks), true);
  double newmin = floor(min / tick_incr) * tick_incr;

  // make sure we start within range
  start_tick = newmin;
  while(start_tick < min) start_tick += tick_incr;

  double units_val = MAX(fabs(min), fabs(max));
  int units_order;
  if(units_val == 0)
    units_order = 0;
  else
    units_order = (int)log10(units_val);

  units_order /= 3;		// introduce a factor of 2 rounding here..
  units_order *= 3;		

  units = pow(10.0, (double)units_order);

  // empirically compute the actual number of ticks..
  double chk_max = max + (tick_incr / 100.0); // give a little tolerance..
  double val;
  int i;
  for (i=0, val = start_tick; val <= chk_max; val += tick_incr, i++);
  act_n_ticks = i;
}

float Axis::GetMin() {
  float r = eff_range.max - eff_range.min;
  float sc_r = range_scale * r;
  return eff_range.min - min_percent * MAX(r, sc_r);
}

float Axis::GetMax(){
  float r = eff_range.max - eff_range.min;
  float sc_r = range_scale * r;
  return eff_range.max - max_percent * MAX(r, sc_r);
}

// limits are needed to prevent numerical overflow!
static const float range_min_limit = 1.0e-6;
static const float range_zero_range = 5.0e-5; // half-range for zero-range values

// updates range based on new data and returns true if it really is a new range
bool Axis::UpdateRange(float first, float last) {
  true_range.UpdateRange(first);
  true_range.UpdateRange(last);

  if(((first >= range.min) && (last <= range.max)) && (range.Range() >= range_min_limit)) { // special case
//     cerr << "not changing range: " << first << ", " << last << " rng: " << range.min << ", " << range.max << endl;
    eff_range = range;
    return false;			// not changed
  }

  first = true_range.min;
  last = true_range.max;

  if(((last - first) < range_min_limit) || (range.Range() < range_min_limit)) {
    if(last - first >= range_min_limit) {
      range.max = last;
      range.min = first;
    }
    else {
      double maxval = MAX(fabs(last), fabs(first));
      if(maxval > .01f) {
	last += range_zero_range * maxval;
	first -= range_zero_range * maxval;
      }
      else {
	last += .51 * range_min_limit;
	first -= .51 * range_min_limit;
      }
    }
  }

  double rng = nicenum(last - first, false);
  rng = MAX(range_min_limit, rng);
  
  double inc = nicenum((rng / (double)n_ticks), true);
  double newmin = floor(first / inc) * inc;
  double newmax = ceil(last / inc) * inc;

//   cerr << "rng: " << rng << ", inc: " << inc << ", newmin: " << newmin << ", newmax: " << newmax << endl;

  range.Set(newmin, newmax);
  fixed_range.FixRange(range);
  eff_range = range;
  return true;			// changed
}

void Axis::UpdateAxis() {
  // do nothing
}

ivGlyph** Axis::Make_Ticks(const ivFont*, const ivColor*) {
  // do nothing
  return NULL;
}



//////////////////////////
// 	YAxis		//
//////////////////////////
 
YAxis::YAxis (float first, float last,ivColor* c) : Axis(first, last,c) {
  _page = new ivPage(new ivVRule(mycolor, 1));
  ivResource::ref(_page);
  body(_page);
}

YAxis::~YAxis () {
  ivResource::unref(_page);
}

void YAxis::request (ivRequisition& r) const {
  ivMonoGlyph::request(r);
  ivRequirement& rx = r.x_requirement();
  ivRequirement& ry = r.y_requirement();
  rx.stretch(0.0);
  rx.shrink(0.0);
  rx.alignment(0.0);
  ry.stretch(fil);
  ry.shrink(fil);
  ry.alignment(1.0);
}


ivGlyph** YAxis::Make_Ticks (const ivFont* font, const ivColor* c) {
//   cerr << "Y: makin ticks, eff_range: " << eff_range.min << " max: " << eff_range.max
//        << " range: " << range.min << " max: " << range.max 
//        << " true range: " << true_range.min << " max: " << true_range.max
//        << endl;
  ivLayoutKit* lkit = ivLayoutKit::instance();
  ivGlyph** tick = new ivGlyph* [act_n_ticks + 1];
  double val;
  int i;
  String label = String(units,"%5g") + "X";
  tick[0] = lkit->vcenter	// perhaps a different font or color?
    (lkit->hcenter(lkit->r_margin(new ivLabel(label, font, c), 3.0), 1.0), 0.5);

  for (i=1, val = start_tick; i < act_n_ticks+1; val += tick_incr, i++) {
    double lab_val = val / units;
    if(fabs(lab_val) < .001) {
      if(fabs(lab_val) < .0001)
	lab_val = 0;		// the 0 can be screwy
      else lab_val = .001;
    }
    label = String(lab_val,"%5g") + "-";
    tick[i] = lkit->vcenter
      (lkit->hcenter(new ivLabel(label, font, c),1.0),0.5);
  }
  return tick;
}

void YAxis::UpdateAxis() {
  ivWidgetKit& kit = *ivWidgetKit::instance();

  ComputeTickIncr();

  ivDisplay* disp = ivSession::instance()->default_display();
  ivCoord one_pixel = disp->to_coord(1);
  ivCoord b, t;
  b = allocation().bottom(); t = allocation().top();
  ivCoord diff = (t - b) - one_pixel;

  ivGlyph** tick = Make_Ticks(kit.font(), mycolor);

  while(_page->count() > 0) _page->remove(_page->count()-1);

  int i;
  for (i = 0; i < act_n_ticks+1; i++) {
    _page->insert(i, tick[i]);
  }
  delete tick;   // --added by ckd to help fix memory leak--

  _page->move(0, 0, -10.0);	// units are "berange.min the axis.."

  // to add actual ticks, just add little line glphs to the _page 
  // and position with the _page->move thing..

  if (act_n_ticks == 1) {
    _page->move(1, 0, 0);
  }
  else if (act_n_ticks > 1) {
    ivCoord dy = diff / GetRange();
    ivCoord val; float min = GetMin();
    for(i=1, val = start_tick; (i < act_n_ticks+1); val += tick_incr, i++) {
      ivCoord pos = (val - min) * dy;
      _page->move(i, 0, pos);
    }
  }
}


//////////////////////////
// 	XAxis		//
//////////////////////////
 
XAxis::XAxis (float first, float last,ivColor* c) : Axis(first, last,c) {
  _page = new ivPage(new ivHRule(mycolor, 1));
  ivResource::ref(_page);
  body(_page);
}

XAxis::~XAxis () {
  ivResource::unref(_page);
}

void XAxis::request (ivRequisition& r) const {
  ivMonoGlyph::request(r);
  ivRequirement& rx = r.x_requirement();
  ivRequirement& ry = r.y_requirement();
  rx.stretch(fil);
  rx.shrink(fil);
  rx.alignment(0.0);
  ry.stretch(0.0);
  ry.shrink(0.0);
  ry.alignment(0.0);
}

ivGlyph** XAxis::Make_Ticks(const ivFont* font, const ivColor* c) {
//   cerr << "X: makin ticks, eff_range: " << eff_range.min << " max: " << eff_range.max
//        << " range: " << range.min << " max: " << range.max
//        << " true range: " << true_range.min << " max: " << true_range.max
//        << endl;
  ivLayoutKit* lkit = ivLayoutKit::instance();
  ivGlyph** tick = new ivGlyph* [act_n_ticks + 1];
  double val;
  int i;
  String label = String(units) + "X";
  tick[0] = lkit->hcenter	// perhaps a different font or color?
    (lkit->vcenter(lkit->t_margin(new ivLabel(label, font, c),3.0),1.0),0.5);

  for (i=1, val = start_tick; i < act_n_ticks+1; val += tick_incr, i++) {
    double lab_val = val / units;
    if(fabs(lab_val) < .001) {
      if(fabs(lab_val) < .0001)
	lab_val = 0;		// the 0 can be screwy
      else lab_val = .001;
    }
    label = String(lab_val);
    tick[i] = lkit->hcenter
      (lkit->vcenter(lkit->t_margin(new ivLabel(label, font, c),5.0),1.0),0.5);
  }
  return tick;
}

void XAxis::UpdateAxis() {
  ivWidgetKit& kit = *ivWidgetKit::instance();

  ComputeTickIncr();

  ivDisplay* disp = ivSession::instance()->default_display();
  ivCoord one_pixel = disp->to_coord(1);
  ivCoord l, r;
  l = allocation().left(); r = allocation().right();
  ivCoord diff = (r - l) - one_pixel;

  while(_page->count() > 0) _page->remove(_page->count()-1);

  ivGlyph** tick = Make_Ticks(kit.font(), mycolor);

  int i;
  for (i = 0; i < act_n_ticks+1; i++) {
    _page->insert(i, tick[i]);
  }
  delete tick;  // added by ckd to help fix memory leak

  _page->move(0, -30.0, 0);	// units are "left of the axis.."

  if (act_n_ticks == 1) {
    _page->move(1, 0, 0);
  }
  else if (act_n_ticks > 1) {
    ivCoord dx = diff / GetRange();
    ivCoord val; float min = GetMin();
    for(i=1, val = start_tick; (i < act_n_ticks+1); val += tick_incr, i++) {
      ivCoord pos = (val - min) * dx;
      _page->move(i, pos, 0);
    }
  }
}



