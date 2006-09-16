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

#include "datagraph_qtso.h"

#define GRAPH_TOP_BORDER  5.0
#define GRAPH_BOT_BORDER 10.0
#define GRAPH_LFT_BORDER 10.0
#define GRAPH_RGT_BORDER 10.0

#include "ta_qt.h"

#include "icolor.h"
#include <icheckbox.h>

#include <qbrush.h>
#include <qevent.h>
#include <Q3Header>
#include <qlayout.h>
#include <qpainter.h>
#include <qpen.h>
#include <Q3Button>
#include <QPushButton>
#include <QScrollArea>

#include <Inventor/nodes/SoAsciiText.h>
#include <Inventor/nodes/SoFont.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoTransform.h>

using namespace Qt;
// various graph metrics -- these could possibly become static params somewhere

#define ORIGIN_OFFSET 0.08f // space between actual origin, and axes
#define UNIT_LEGEND_OFFSET 0.04f // space between end of axis and unit legend text
#define GRAPH_MARGIN 0.05f // unused blank space around border
#define ELEM_SPACE 0.05f // misc element spacing, ex. between color bar and graphs
#define TICK_SIZE 0.05f // size of ticks
#define TICK_OFFSET 0.04f // gap between tick and label
#define AXIS_LABEL_SIZE 0.08f // size of labels for axes
#define LINE_LABEL_SIZE 0.08f // size of labels for axes
#define AXIS_WIDTH 0.2f // how much space we use, not including the offset
#define T3BAR_HEIGHT 0.2f // height of color bar when it appears

//////////////////////////
// 	AxisView	//
//////////////////////////

AxisView* AxisView::New(AxisSpec* axis_spec) {
  AxisView* rval = new AxisView();
  axis_spec->AddDataView(rval);
  return rval;
}

void AxisView::Initialize() {
  axis_length = 1.0f; // dummy value
  start_tick = 0;
  tick_incr = 1;
  units = 1.;
  act_n_ticks = 11;
  range_scale = 1.0f;
  min_percent = max_percent = 0.0f;
}

void AxisView::Destroy() {
  Reset();
}

void AxisView::Copy_(const AxisView& cp) {
  //TODO axis length???
  range_scale = cp.range_scale;
  min_percent = cp.min_percent;
  max_percent = cp.max_percent;
  start_tick = cp.start_tick;
  tick_incr = cp.tick_incr;
  units = cp.units;
  act_n_ticks = cp.act_n_ticks;
}

AxisView::Axis AxisView::axis() {
  return (Axis)axis_spec()->axis();
}

bool AxisView::InitUpdateAxis(bool init) {
  bool rval = false;
  AxisSpec* as = axis_spec(); // cache
  if (as) {
    rval = as->InitUpdateAxis(init);
    if (rval)
      UpdateAxis();
  }
  return rval;
}


void AxisView::ComputeTickIncr() {
  AxisSpec* as = axis_spec(); // cache
  if (!as) return;

  float min = GetMin();
  float max = GetMax();
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

  tick_incr = nicenum((rng / (double)as->n_ticks), true);
  double newmin = floor(min / tick_incr) * tick_incr;

  // make sure we start within range
  start_tick = newmin;
  while(start_tick < min) start_tick += tick_incr;

  double units_val = MAX(fabs(min), fabs(max));
  int units_order;
  if (units_val == 0)
    units_order = 0;
  else
    units_order = (int)log10(units_val);

  units_order /= 3;		// introduce a factor of 2 rounding here..
  units_order *= 3;

  units = pow(10.0, (double)units_order);

  // empirically compute the actual number of ticks..
  float chk_max = max + (tick_incr / 100.0f); // give a little tolerance..
  float val;
  int i;
  for (i=0, val = start_tick; val <= chk_max; val += tick_incr, i++);
  act_n_ticks = i;
}

void AxisView::DataToPoint(float data, float& pt) {
  AxisSpec* axis_spec = this->axis_spec(); //cache
  float rng = axis_spec->range.Range(); //TODO: verify this -- s/b range of axis (not data)
  if (rng == 0.0f) pt = 0.0f;
  else pt = ((data - axis_spec->range.min) * axis_length) / rng;
}

float AxisView::GetMin() {
  AxisSpec* as = this->axis_spec(); //cache
  float r = as->eff_range.max - as->eff_range.min;
  float sc_r = range_scale * r;
  return as->eff_range.min - min_percent * MAX(r, sc_r);
}

float AxisView::GetMax(){ //return 10.0f; //TEMP
  AxisSpec* as = this->axis_spec(); //cache
  float r = as->eff_range.max - as->eff_range.min;
  float sc_r = range_scale * r;
  return as->eff_range.max - max_percent * MAX(r, sc_r);
}

void AxisView::Render_impl() {
  UpdateAxis();
  inherited::Render_impl();
}

void AxisView::Render_pre(taDataView* par) {
  T3Axis* t3ax = new T3Axis((T3Axis::Axis)axis(), this);
  SoFont* font = t3ax->labelFont();
  font->size.setValue(AXIS_LABEL_SIZE);
  m_node_so = t3ax;
  inherited::Render_pre(par);
}

void AxisView::UpdateAxis() {
  T3Axis* t3ax = node_so(); //cache
  if (!t3ax) return;
  AxisSpec* as = this->axis_spec(); //cache
  ComputeTickIncr();

  t3ax->clear();
  // for setting color
  SoMaterial* mat = t3ax->material();
  as->def_color()->copyTo(mat->diffuseColor);
  switch (axis()) {
  case X: UpdateAxis_X(t3ax, as);
    break;
  case Y: UpdateAxis_Y(t3ax, as);
    break;
  case Z: UpdateAxis_Z(t3ax, as);
    break;
  }
}

/* Axis rendering notes
  as->act_n_ticks is the number of sections, so tick marks will be +1 (to include ends)

*/
void AxisView::UpdateAxis_X(T3Axis* t3ax, AxisSpec* as) {
  iVec3f fm;
  iVec3f to;

  // axis line itself
  to.x = axis_length;
  t3ax->addLine(fm, to);

  // units legend
  fm.x = -UNIT_LEGEND_OFFSET;
  fm.y = -AXIS_LABEL_SIZE;
  String label = String(units,"%.5g") + "x";
  t3ax->addLabel(label.chars(), fm, SoAsciiText::LEFT);

  // ticks
  fm = 0.0f;
  to = 0.0f;
  fm.y = -(TICK_SIZE / 2.0f);
  to.y =  (TICK_SIZE / 2.0f);
  // get incr per tick; note: should always be valid
  float ax_incr = (act_n_ticks > 1) ? axis_length / (act_n_ticks - 1) : axis_length;

//nn  String label = String(as->units,"%5g") + "X";
  //TODO: first axis label
/*  tick[0] = lkit->vcenter	// perhaps a different font or color?
    (lkit->hcenter(lkit->r_margin(new ivLabel(label, font, c), 3.0), 1.0), 0.5); */

  int i;
  float val;
  for (i = 0, val = start_tick; i < act_n_ticks; val += tick_incr, ++i) {
    float lab_val = val / units;
    if (fabs(lab_val) < .001) {
      if (fabs(lab_val) < .0001)
	lab_val = 0.0f;		// the 0 can be screwy
      else lab_val = .001f;
    }
    label = String(lab_val);

    t3ax->addLine(fm, to);
    t3ax->addLabel(label.chars(),
      iVec3f(fm.x, fm.y - (TICK_OFFSET + AXIS_LABEL_SIZE), fm.z), SoAsciiText::CENTER);
    fm.x += ax_incr;
    to.x += ax_incr;
  }
}

void AxisView::UpdateAxis_Y(T3Axis* t3ax, AxisSpec* as) {
  iVec3f fm;
  iVec3f to;

  // axis line itself
  to.y = axis_length;
  t3ax->addLine(fm, to);

  // units legend
  fm.y = -(UNIT_LEGEND_OFFSET + (AXIS_LABEL_SIZE / 2.0f));
//  fm.x = 0.0f;
  String label = String(units,"%.5g") + "x";
  t3ax->addLabel(label.chars(), fm, SoAsciiText::LEFT);

  // ticks
  fm = 0.0f;
  to = 0.0f;
  fm.x = -(TICK_SIZE / 2.0f);
  to.x =  (TICK_SIZE / 2.0f);
  // get incr per tick; note: should always be valid
  float ax_incr = (act_n_ticks > 1) ? axis_length / (act_n_ticks - 1) : axis_length;

  int i;
  float val;
  for (i = 0, val = start_tick; i < act_n_ticks; val += tick_incr, ++i) {
    float lab_val = val / units;
    if (fabs(lab_val) < .001) {
      if (fabs(lab_val) < .0001)
	lab_val = 0.0f;		// the 0 can be screwy
      else lab_val = .001f;
    }
    label = String(lab_val);

    t3ax->addLine(fm, to);
    t3ax->addLabel(label.chars(),
      iVec3f(fm.x - TICK_OFFSET, fm.y - (AXIS_LABEL_SIZE / 2.0f), fm.z));
    fm.y += ax_incr;
    to.y += ax_incr;
  }
}

void AxisView::UpdateAxis_Z(T3Axis* t3ax, AxisSpec* as) {
  iVec3f fm;
  iVec3f to;

  // axis line itself
  to.z = axis_length;
  t3ax->addLine(fm, to);

  // units legend
//  fm.x = -(UNIT_LEGEND_OFFSET + (AXIS_LABEL_SIZE / 2.0f));
  fm.y = -(AXIS_LABEL_SIZE / 2.0f);
//  fm.x = 0.0f;
  String label = String(units,"%.5g") + "x";
  t3ax->addLabel(label.chars(), fm, SoAsciiText::RIGHT);

  // ticks
  fm = 0.0f;
  to = 0.0f;
  fm.x = -(TICK_SIZE / 2.0f);
  to.x =  (TICK_SIZE / 2.0f);
  // get incr per tick; note: should always be valid
  float ax_incr = (act_n_ticks > 1) ? axis_length / (act_n_ticks - 1) : axis_length;

  int i;
  float val;
  for (i = 0, val = start_tick; i < act_n_ticks; val += tick_incr, ++i) {
    float lab_val = val / units;
    if (fabs(lab_val) < .001) {
      if (fabs(lab_val) < .0001)
	lab_val = 0.0f;		// the 0 can be screwy
      else lab_val = .001f;
    }
    label = String(lab_val);

    t3ax->addLine(fm, to);
    t3ax->addLabel(label.chars(),
      iVec3f(fm.x - TICK_OFFSET, fm.y - (AXIS_LABEL_SIZE / 2.0f), fm.z));
    fm.z += ax_incr;
    to.z += ax_incr;
  }
}


//////////////////////////
// 	GraphLine	//
//////////////////////////

GraphLine* GraphLine::New(GraphColSpec* data) {
  GraphLine* rval = new GraphLine();
  data->AddDataView(rval);
  return rval;
}

void GraphLine::Initialize()
{
  last_coord = 0;
  n_shared = 0;
  share_i = 0;
  x_axis = NULL;
  y_axis = NULL;
  z_axis = NULL;

  ReInit();

}

void GraphLine::Destroy() {
  Reset();
  x_axis = NULL;
  y_axis = NULL;
  z_axis = NULL;
}

void GraphLine::DataToPoint(const iVec3f& data, iVec3f& pt) {
  if (x_axis) x_axis->DataToPoint(data.x, pt.x);
  if (y_axis) y_axis->DataToPoint(data.y, pt.y);
  if (z_axis) z_axis->DataToPoint(data.z, pt.z);
}

/*void GraphLine::drawclipped_gs(ivCanvas* c, ivCoord l, ivCoord b, ivCoord r, ivCoord t, Graphic* gs)
{
  ivCoord ll, bb, rr, tt;
  getbounds_gs(ll, bb, rr, tt, gs);
  BoxObj thisBox(ll, bb, rr, tt);
  BoxObj clipBox(l, b, r, t);
  if (clipBox.Intersects(thisBox)) {
    draw_gs(c, gs);
  }
} */

/*TODO void GraphLine::draw_gs(ivCanvas* c, Graphic* gs) {
  if(_ctrlpts <= 0) return;

  const iBrush* b =  brush();
  const iColor* stroke = gs->stroke();
  if((b == NULL) || (stroke == NULL)) return;

  ivTransformer* tx = gs->transformer();
  if (tx != nil) {
    c->push_transform();
    c->transform(*tx);
  }

  float zeroval = y_axis_spec()->range.min;

  // first go through and set control points to reflect transformed locations!
  bool no_y_vals = vertical == NO_VERTICAL;
  for(int i=0; i < _ctrlpts; i++) {
    float cox = eoff.x + (float)(trace_vals[i])*plwd*trace_incr.x;
    float coy = eoff.y + (float)(trace_vals[i])*plht*trace_incr.y;
    if(vertical == STACK_TRACES)
      coy += (float)(trace_vals[i]) * esc.y * y_axis_spec()->range.Range();
    float xv = esc.x * (data_x[i] - x_axis_spec()->range.min) + x_axis_spec()->range.min + cox;
    float y = (no_y_vals ? zeroval : data_y[i]);
    float yv = esc.y * (y - y_axis_spec()->range.min) + y_axis_spec()->range.min + coy;
    _x[i] = xv;
    _y[i] = yv;
  }
  recompute_shape();
  // allow room for coord labels!
  float min_ht = plht * 8.0f / MAX(line_width, 1.0f);
  if((_ymax - _ymin) < min_ht) {
    _ymax = _ymin + min_ht;
  }

  int i_st = 0;			// start index
  int i_ed = _ctrlpts-1;	// end index
  int i_in = 1;			// index increment
  if(trace_incr.y > 0) {
    i_st = _ctrlpts-1;
    i_ed = 0;
    i_in = -1;
  }

  if((n_traces > 1) && (trace_incr.x != 0.0f) || (trace_incr.y != 0.0f)) {
    float x0 = x_axis_spec()->range.min + eoff.x; float y0 = y_axis_spec()->range.min + eoff.y;
    float x1 = x_axis_spec()->range.min + esc.x * x_axis_spec()->range.Range() + eoff.x;
    float y1 = y_axis_spec()->range.min + esc.y * y_axis_spec()->range.Range() + eoff.y;
    float xt = x0 + (float)(n_traces-1)*plwd*trace_incr.x;
    float yt = y0 + (float)(n_traces-1)*plht*trace_incr.y;

    c->new_path();    c->move_to(x0, y0);    c->line_to(x1, y0);
    c->stroke(graph_spec()->dtvsp->bg_color.contrastcolor, width0_brush);
    c->new_path();    c->move_to(x0, y0);    c->line_to(x0, y1);
    c->stroke(graph_spec()->dtvsp->bg_color.contrastcolor, width0_brush);
    c->new_path();    c->move_to(x0, y0);    c->line_to(xt, yt);
    c->stroke(graph_spec()->dtvsp->bg_color.contrastcolor, width0_brush);
  }

  if(line_type == STRINGS) {		// plotting strings!
    int mxidx = MIN(string_vals.size, _ctrlpts);
    for(int i=0; i<mxidx; i++) {
      if((i - point_mod.off) % (point_mod.m) != 0) continue;
      String str = string_vals.FastEl(i);
      if(str.empty()) continue; // don't plot empty strings!
      RenderText(c, _x[i], _y[i], str, stroke);
    }
  }
  else {			// plotting lines!
    if((line_type == LINE) || (line_type == LINE_AND_POINTS)) {
      if(_ctrlpts == 1) {
	c->new_path();
	c->move_to(_x[0], _y[0]);
	c->line_to(_x[0] + plwd, _y[0]);
	c->stroke(stroke, b);
      }
      else {
	c->new_path();
	c->move_to(_x[i_st], _y[i_st]);
	for(int i = i_st+i_in; ((i_in > 0) ? (i <= i_ed) : (i >= i_ed)); i += i_in) {
	  if(!negative_draw &&
	     ((i_in > 0) ? (trace_vals[i] > trace_vals[i-1]) : (trace_vals[i+1] > trace_vals[i]))) {
	    c->stroke(stroke,b);
	    c->new_path();
	    c->move_to(_x[i], _y[i]);
	  }
	  else {
	    c->line_to(_x[i], _y[i]);
	  }
	}
	c->stroke(stroke, b);
      }
    }
    else if(line_type == TRACE_COLORS) {
      if(_ctrlpts == 1) {
	c->new_path();
	c->move_to(_x[0], _y[0]);
	c->line_to(_x[0] + plwd, _y[0]);
	c->stroke(GetTraceColor(trace_vals[0]), b);
      }
      else {
	c->new_path();
	c->move_to(_x[i_st], _y[i_st]);
	for(int i = i_st+i_in; ((i_in > 0) ? (i <= i_ed) : (i >= i_ed)); i += i_in) {
	  if((i_in > 0) ? (trace_vals[i] > trace_vals[i-1]) : (trace_vals[i+1] > trace_vals[i])) {
	    c->stroke(GetTraceColor(trace_vals[i-i_in]),b);
	    c->new_path();
	    c->move_to(_x[i], _y[i]);
	  }
	  else {
	    c->line_to(_x[i], _y[i]);
	  }
	}
	c->stroke(GetTraceColor(trace_vals[i_ed]), b);
      }
    }
    else if(line_type == VALUE_COLORS) {
      if(_ctrlpts == 1) {
	c->new_path();
	c->move_to(_x[0], _y[0]);
	c->line_to(_x[0] + plwd, _y[0]);
	c->stroke(GetValueColor(data_y[0]), b);
      }
      else {
	c->new_path();
	c->move_to(_x[i_st], _y[i_st]);
	for(int i = i_st+i_in; ((i_in > 0) ? (i <= i_ed) : (i >= i_ed)); i += i_in) {
	  if(!negative_draw &&
	     ((i_in > 0) ? (trace_vals[i] > trace_vals[i-1]) : (trace_vals[i+1] > trace_vals[i]))) {
	    c->new_path();
	    c->move_to(_x[i], _y[i]);
	  }
	  else {
	    c->line_to(.5 * (_x[i-i_in] + _x[i]), .5 * (_y[i-i_in] + _y[i]));
	    c->stroke(GetValueColor(data_y[i-i_in]), b);
	    c->new_path();
	    c->move_to(.5 * (_x[i-i_in] + _x[i]), .5 * (_y[i-i_in] + _y[i]));
	    c->line_to(_x[i], _y[i]);
	    c->stroke(GetValueColor(data_y[i]), b);
	    c->move_to(_x[i], _y[i]);
	  }
	}
      }
    }
    else if(line_type == THRESH_POINTS) {
      for(int i = 0; i < _ctrlpts; i++) {
	if(data_y[i] < thresh) continue;
	float xv = _x[i]; float yv = _y[i];
	c->fill_rect(xv - (.5 * plwd), yv - (.5 * plht), xv + (.5 * plwd), yv + (.5 * plht), stroke);
      }
    }
    if((line_type == POINTS) || (line_type == LINE_AND_POINTS)) {
      c->new_path();
      c->move_to(_x[0], _y[0]);
      for(int i = 0; i < _ctrlpts; i++) {
	if((i - point_mod.off) % (point_mod.m) != 0) continue;
	RenderPoint(c, _x[i], _y[i], stroke, gs->brush());
      }
    }
    if(coord_points.size > 0) {
      stroke = graph_spec()->dtvsp->bg_color.contrastcolor;
      for(int i=0;i<coord_points.size;i++) {
	int idx = coord_points.FastEl(i);
	float xv = _x[idx]; float yv = _y[idx];
	// draw little box around point
	c->rect(xv-plwd, yv-plht, xv+plwd, yv+plht, stroke, gs->brush());
	String num = String(data_x[idx], "(%g , ") + String(data_y[idx], "%g)");
	RenderText(c, xv, yv, num, stroke, true);
      }
    }
  }
  if(tx != nil) {
    c->pop_transform();
  }
}*/

bool GraphLine::DrawLastPoint() {
  bool rval = false;

  T3GraphLine* t3gl = node_so(); // cache
  if (!t3gl) return rval;


  GraphColSpec* col_spec = this->col_spec(); //cache
  // y data is our own data, unless plotting strings, then we get from another col
  if (col_spec->line_type == GraphColSpec::STRINGS) {
    da_y = (col_spec->string_coords) ? col_spec->string_coords->data_array : NULL;
    da_string = col_spec->data_array;
  } else {
    da_y = col_spec->data_array;
    da_string = NULL;
  }
  if (!da_y) return rval; // should normally exist

  //note: never need to worry about a jagged data array, since all last values will exist
  // as long as at least one of them does
  int i_y = da_y->AR()->size - 1;
  if (i_y < 0) return rval;

  da_x = (x_axis) ? x_axis->axis_spec()->spec->data_array : NULL;
  da_z = (z_axis) ? z_axis->axis_spec()->spec->data_array : NULL;

  rval = RenderPoint(i_y);

  return rval;
}

const iColor* GraphLine::GetValueColor(float val) {
  const iColor* clr = NULL;
  if (y_axis_spec()->range.Range() == 0) {
    clr = graph_spec()->scale->GetColor((int)((.5 * (float)(graph_spec()->scale->chunks-1)) + .5));
  } else if(val > y_axis_spec()->range.max) {
    clr = graph_spec()->scale->maxout.color();
  } else if(val < y_axis_spec()->range.min) {
    clr = graph_spec()->scale->minout.color();
  } else {
    int chnk = graph_spec()->scale->chunks-1;
    float rval = y_axis_spec()->range.Normalize(val);
    int idx = (int) ((rval * (float)chnk) + .5);
    idx = MAX(0, idx);
    idx = MIN(chnk, idx);
    clr = graph_spec()->scale->GetColor(idx);
  }
  return clr;
}

const iColor* GraphLine::GetTraceColor(int trace) {
  const iColor* clr = NULL;
  if (n_traces == 0) {
    clr = graph_spec()->scale->GetColor((int)((.5 * (float)(graph_spec()->scale->chunks-1)) + .5));
  } else {
    int chnk = graph_spec()->scale->chunks-1;
    float rval = (float)trace / (float)(n_traces - 1);
    int idx = (int) ((rval * (float)chnk) + .5);
    idx = MAX(0, idx);
    idx = MIN(chnk, idx);
    clr = graph_spec()->scale->GetColor(idx);
  }
  return clr;
}

void GraphLine::Redraw() {
  last_data = 0.0f;
  last_da_idx = -1;
  n_traces = 1;
  trace_idx = -1;

  T3GraphLine* t3gl = node_so(); // cache
  if (!t3gl) return;
  GraphColSpec* col_spec = this->col_spec(); //cache
  GraphSpec* gs = graph_spec(); // cache

  t3gl->clear();
  // if using any of the multi-color modes, need to set that, else set the default color
  val_color_mode = ( (col_spec->line_type == GraphColSpec::TRACE_COLORS)
    || (col_spec->line_type == GraphColSpec::VALUE_COLORS)
  );
  if (!val_color_mode) {
    // default color of line is color of axis
    t3gl->setDefaultColor((T3Color)(*col_spec->line_color.color()));
  }
  t3gl->setValueColorMode(val_color_mode);



  // y data is our own data, unless plotting strings, then we get from another col
  if (col_spec->line_type == GraphColSpec::STRINGS) {
    da_y = (col_spec->string_coords) ? col_spec->string_coords->data_array : NULL;
    da_string = col_spec->data_array;
  } else {
    da_y = col_spec->data_array;
    da_string = NULL;
  }
  if (!da_y) return; // should normally exist

  // calculate real data range indexes, based on view_range, (given possible ragged data array)
  int fm_idx; int to_idx; // absolute indexes relative to buffer start
  if (!gs->data_table->idx(gs->view_range.min, da_y->AR()->size, fm_idx)) {
    fm_idx = 0;
  }
  gs->data_table->idx(gs->view_range.max, da_y->AR()->size, to_idx);
  to_idx = MIN(to_idx, da_y->AR()->size - 1);


  da_x = (x_axis) ? x_axis->axis_spec()->spec->data_array : NULL;
  da_z = (z_axis) ? z_axis->axis_spec()->spec->data_array : NULL;

  iVec3f pt;

  // if using traces, scan all x's to determine # of traces
  int i_x;
//obs  if (((YAxisSpec*)(col_spec->axis_spec))->vertical == YAxisSpec::STACK_TRACES) {
  if (gs->graph_type == GraphSpec::STACK_TRACES) {
    pt.x = da_x->GetValAsFloat(0);
    for (int i = fm_idx + 1; i <= to_idx; ++i) {
      if (!gs->data_table->idx(i, da_x->AR()->size, i_x))  continue;
      float x = da_x->GetValAsFloat(i_x);
      if (x < pt.x) ++n_traces;
      pt.x = x;
    }
    // update n_traces in spec for Z
    ZAxisSpec* z_axis_spec = this->z_axis_spec();
    if (z_axis_spec) {
      z_axis_spec->n_traces = n_traces;
      // update the spec, and the image if necessary
      if (z_axis_spec->InitUpdateAxis(false)) {
        if (z_axis)
          z_axis->UpdateAxis();
      }
    }

  }

  // just issue a series of redraws
  for (int i = fm_idx; i <= to_idx; ++i) {
    RenderPoint(i);
  }
}

void GraphLine::ReInit_impl() {
//obs  ctrlpts(NULL,NULL,0);	// delete the coord space
//  data_x.Reset();  data_y.Reset();
//  coord_points.Reset();
//  string_vals.Reset();
//  trace_vals.Reset();
//  n_traces = 1;
  inherited::ReInit_impl();

}

void GraphLine::Render_impl() {
  T3GraphLine* t3gl = node_so(); // cache
  if (!t3gl) return;
  GraphColSpec* col_spec = this->col_spec(); //cache
  int line_style = col_spec->line_style;
  //precaution: make sure stays in range of so object styles
  if (line_style < T3GraphLine::LineStyle_MIN)
    line_style = T3GraphLine::LineStyle_MIN;
  else if (line_style > T3GraphLine::LineStyle_MAX)
    line_style = T3GraphLine::LineStyle_MAX;
  t3gl->setLineStyle((T3GraphLine::LineStyle)line_style, col_spec->line_width);

  inherited::Render_impl();
}

void GraphLine::Render_pre(taDataView* par) {
  m_node_so = new T3GraphLine(this);
  SoFont* font = node_so()->labelFont();
  font->size.setValue(LINE_LABEL_SIZE);
  inherited::Render_pre(par);
}

bool GraphLine::RenderPoint(int row) {
  bool rval = false;
  T3GraphLine* t3gl = node_so(); // cache; note: caller checked
  GraphColSpec* col_spec = this->col_spec(); //cache
  GraphSpec* gs = this->graph_spec(); // cache
  YAxisSpec* y_axis_spec = this->y_axis_spec();

  iVec3f pt;
  int i_x; int i_y; int i_z; // translated row #'s, to account for jagged arrays

  // lookup or calculate the plot coordinates
  if (!gs->data_table->idx(row, da_x->AR()->size, i_x))  return rval;
  if (!gs->data_table->idx(row, da_y->AR()->size, i_y))  return rval;
  pt.x = da_x->GetValAsFloat(i_x);
  if (!y_axis_spec->no_vertical)
    pt.y = da_y->GetValAsFloat(i_y); //note: will be the plot point for STRINGS

  // see if we are starting a new line or trace -- used by some graph types
  bool new_trace = ((last_da_idx < 0) || (pt.x < last_data.x));
  if (new_trace) ++trace_idx;

  // z, if any, will depend on the mode
  if (gs->graph_type == GraphSpec::STACK_TRACES) {
    pt.z = trace_idx;
  } else if (gs->graph_type == GraphSpec::STACK_LINES) {
    pt.z = share_i;
  } else if (da_z) { //otherwise, if z contains data points
    if (!gs->data_table->idx(row, da_z->AR()->size, i_z))  return rval;
    pt.z = da_z->GetValAsFloat(i_z);
  }
  iVec3f coord;
  DataToPoint(pt, coord);

  const iColor* col = NULL; // only used for color modes

  // next section has major subsections depending on the overall type of Y graph this is
  if ((col_spec->line_type == GraphColSpec::LINE)
    || (col_spec->line_type == GraphColSpec::LINE_AND_POINTS)
    || (col_spec->line_type == GraphColSpec::TRACE_COLORS)
    || (col_spec->line_type == GraphColSpec::VALUE_COLORS)
  ) {
    if (col_spec->line_type == GraphColSpec::TRACE_COLORS) {
      col = GetTraceColor(trace_idx);
    } else if (col_spec->line_type == GraphColSpec::VALUE_COLORS) {
      col = GetValueColor(pt.y);
    }
    // need to do a "move to" if just starting, starting a new trace (trace mode), or
    // starting a new trace w/o negative_draw
    if ((last_da_idx < 0)
      || (new_trace && (gs->graph_type == GraphSpec::STACK_TRACES))
      || (new_trace && (!col_spec->negative_draw))
    ) {
      // if doing true tracing, update trace count and update z axis
      if (trace_idx >= 0) rval = true;
      ++trace_idx;
      if ((col_spec->line_type == GraphColSpec::TRACE_COLORS)
        || (col_spec->line_type == GraphColSpec::VALUE_COLORS)
      ) {
        t3gl->moveTo(coord, (T3Color)(*col));
      } else {
        t3gl->moveTo(coord);
      }
    } else {
      if ((col_spec->line_type == GraphColSpec::TRACE_COLORS)
        || (col_spec->line_type == GraphColSpec::VALUE_COLORS)
      ) {
        t3gl->lineTo(coord, (T3Color)(*col));
      } else {
        t3gl->lineTo(coord);
      }
    }
  } else if (col_spec->line_type == GraphColSpec::STRINGS) {
    String str;
    if (gs->data_table->idx(row, da_string->AR()->size, i_y)) {
      str = da_string->GetValAsString(i_y);
    }
    if (!str.empty())
      t3gl->textAt(
      iVec3f(coord.x,  coord.y - (LINE_LABEL_SIZE / 2.0f), coord.z),
      str.chars());
  }

  // render marker, if any
  if ((col_spec->line_type == GraphColSpec::POINTS) ||
      (col_spec->line_type == GraphColSpec::LINE_AND_POINTS) ||
      (col_spec->line_type == GraphColSpec::THRESH_POINTS)
  ) {
    int point_style;
    // no point drawn if thresholded and not above thresh
    if ((col_spec->line_type == GraphColSpec::THRESH_POINTS)
      && (pt.y < col_spec->thresh)) goto done;

    point_style = col_spec->point_style;
    // for safety, make sure value constrained to so node values
    if (point_style < T3GraphLine::MarkerStyle_MIN)
      point_style = T3GraphLine::MarkerStyle_MIN;
    else if (point_style > T3GraphLine::MarkerStyle_MAX)
      point_style = T3GraphLine::MarkerStyle_MAX;
    t3gl->markerAt(coord, (T3GraphLine::MarkerStyle)point_style);
  }

done:
  last_data = pt;
  ++last_da_idx;
  return rval;
}

/*TODO
bool GraphLine::grasp_move(const ivEvent&, Tool&, ivCoord ix, ivCoord iy) {
  float ax = 0;  float ay = 0;
  transformer()->inverse_transform(ix,iy,ax,ay);
  init_mv_x = ax;  init_mv_y = ay;
  ax = x_axis_spec()->range.Normalize(ax);  ay = y_axis_spec()->range.Normalize(ay);
  // operate on normalized coordinates!
  // button is stored from the grasp since the move event does not contain the propper button
  float xv = x_axis_spec()->range.Normalize(_x[0]);
  float yv = y_axis_spec()->range.Normalize(_y[0]);

  double d = sqrt((double) (((xv-ax) * (xv-ax)) + ((yv-ay) * (yv-ay))));
  int target =0;
  for (int i = 1; i < _ctrlpts; i++) {
    xv = x_axis_spec()->range.Normalize(_x[i]);  yv = y_axis_spec()->range.Normalize(_y[i]);
    double dist = sqrt((double) (((xv-ax) * (xv-ax)) + ((yv-ay) * (yv-ay))));
    if(dist < d) {
      d = dist; target = i;
    }
  }
  last_coord = target;
  coord_points.Add(last_coord);
  return true;
}

bool GraphLine::manip_move(const ivEvent& , Tool&, ivCoord ix, ivCoord iy,
			      ivCoord, ivCoord, ivCoord cx, ivCoord cy) {
  float nx, ny;
  transformer()->inverse_transform(cx,cy,nx,ny);
  float ax, ay;
  transformer()->inverse_transform(ix,iy,ax,ay);

  // compensate for moving of display in case that happened
  nx -= ax - init_mv_x;
  ny -= ay - init_mv_y;

  // scan outward from the last point
  float xv = _x[last_coord];
  float d = fabs(xv-nx);
  int target = last_coord;

  int mx = MAX(_ctrlpts - last_coord, last_coord);
  for (int i = 0; i < mx; i++) {
    if((last_coord + i) < _ctrlpts) {
      xv = _x[last_coord+i];
      if(fabs(xv-nx) < d) {
	d = fabs(xv-nx);
	target = last_coord + i;
      }
    }
    if((last_coord - i) >= 0) {
      xv = _x[last_coord-i];
      if(fabs(xv-nx) < d) {
	d = fabs(xv-nx);
	target = last_coord - i;
      }
    }
  }
  if(target < 0) target = 0;
  if(target >= _ctrlpts) target = _ctrlpts-1;

  last_coord = target;
  coord_points.Peek() = target;
  return true;
}

bool GraphLine::effect_move(const ivEvent&, Tool& ,ivCoord , ivCoord ,
				 ivCoord  , ivCoord ) {
  coord_points.Pop();
  coord_points.AddUnique(last_coord);
  return true;
}

bool GraphLine::effect_border() {
  return true;
}
*/

/*nn
//////////////////////////
// 	GraphViewer	//
//////////////////////////

GraphViewer::GraphViewer(GraphEditor* e, GraphView* gg, float w, float h, const iColor* bg)
  : GlyphViewer(w,h,bg)
{
  graph_spec() = e;
  graph = gg;
  init_xform = NULL;
}

GraphViewer::~GraphViewer() {
  if (init_xform) {
    ivResource::unref(init_xform);
    init_xform = NULL;
  }
}

void GraphViewer::root(GraphicMaster* g) {
  GlyphViewer::root(g);
  if(init_xform != NULL) { ivResource::unref(init_xform); init_xform = NULL; }
  ivTransformer* rootr = _root->transformer();
  if(rootr != NULL)
    init_xform = new ivTransformer(*rootr);
  else
    init_xform = new ivTransformer();
  ReScaleAxes();
}

void GraphViewer::ReScaleAxes(bool redraw) {
  if(init_xform==NULL) return;
  if(_root->transformer() == NULL) return;

  if(graph->x_axis != NULL) {
    AxisView* a = graph->x_axis;
    float vmax = 0;    float vmin = 0;
    float nvmin =0;    float nvmax =0;
    float bogus_y=0; // use vmax,vmin as temps at first;
    init_xform->inverse_transform(_a.left(),0,vmin,bogus_y);
    init_xform->inverse_transform(_a.right(),0,vmax,bogus_y);
    _root->transformer()->transform(vmin,0,nvmin,bogus_y);
    _root->transformer()->transform(vmax,0,nvmax,bogus_y);
    vmin = _a.left(); vmax = _a.right();
    float vrange = vmax - vmin;
    float nvrange = (nvmax - nvmin);
    a->min_percent = (nvmin - vmin)/(MAX(vrange,nvrange));
    a->max_percent = (nvmax - vmax)/(MAX(vrange,nvrange));
    a->range_scale = (vrange / nvrange);
    a->UpdateAxis();
    a->reallocate();
    if(redraw) a->redraw();
  }

  // scaling the axes
  for(int i=0;i<graph->y_axes.size;i++){
    AxisView* a = (AxisView*)graph->y_axes[i];
    float vmax = 0;    float vmin = 0;
    float nvmin =0;    float nvmax =0;
    float bogus_x=0;
    init_xform->inverse_transform(0,_a.bottom(),bogus_x,vmin);
    init_xform->inverse_transform(0,_a.top(),bogus_x,vmax);
    _root->transformer()->transform(0,vmin,bogus_x,nvmin);
    _root->transformer()->transform(0,vmax,bogus_x,nvmax);
    vmin = _a.bottom(); vmax = _a.top();
    float vrange = vmax - vmin;
    float nvrange = (nvmax - nvmin);
    a->min_percent = (nvmin - vmin)/(MAX(vrange,nvrange));
    a->max_percent = (nvmax - vmax)/(MAX(vrange,nvrange));
    a->range_scale = (vrange / nvrange);
    a->UpdateAxis();
    a->reallocate();
    if(redraw) a->redraw();
  }
}

void GraphViewer::rate_zoom() {
  GlyphViewer::rate_zoom();
  ReScaleAxes();
}

void GraphViewer::grab_scroll() {
  GlyphViewer::grab_scroll();
  ReScaleAxes(false);
}
*/
/*obs
//////////////////////////
// 	GrapherCallback	//
//////////////////////////

typedef void (GlyphViewer::*ToolSetter) (unsigned int);

class GrapherCallback : public ivAction {
public:
  GrapherCallback(GraphEditor*, ToolSetter, unsigned int);
  virtual void execute();
private:
  GraphEditor* _ge;
  ToolSetter _tool;
  unsigned _type;
};

GrapherCallback::GrapherCallback(GraphEditor* ge, ToolSetter tool, unsigned int t) {
  _ge = ge;
  _tool = tool;
  _type = t;
}

void GrapherCallback::execute() {
  GraphEditor* ge = _ge;
  ToolSetter tool = _tool;
  unsigned int type = _type;

  for(int i=0;i<ge->graphs.size;i++) {
    GraphView* gg = (GraphView*)ge->graphs[i];
    (gg->viewer->*tool)(type);
  }
} */


//////////////////////////
// 	GraphView	//
//////////////////////////

GraphView* GraphView::New(GraphletSpec* data) {
  GraphView* rval = new GraphView();
  data->AddDataView(rval);
  return rval;
}


void GraphView::Initialize() {
  x_axis = NULL;
  z_axis = NULL;
  nodisp_x_axis = false;
  m_graph_spec = NULL;
}

void GraphView::Destroy() {
  Reset();
  CutLinks();
  m_graph_spec = NULL;
}

void GraphView::InitLinks() {
  inherited::InitLinks();
  taBase::Own(geom, this);
}

void GraphView::CutLinks() {
  geom.CutLinks();
  inherited::CutLinks();
}

bool GraphView::AddLastPoint() {
  bool redraw = false; // if any encountered,  just stop, since everything will get redrawn anyway
  // redraw all the existing lines
  for (int i = lines.size - 1; i >= 0; --i) {
    GraphLine* gl = (GraphLine*)lines.FastEl(i);
    if (gl->DrawLastPoint()) {
    redraw = true;
      break;
    }
  }
  return redraw;

}

void GraphView::BuildAll() {
  Reset();

  GraphSpec* gvs = graph_spec(); // cache
  AxisSpec* as;
  // z axis (if 3d)
  if (gvs->is3d()) {
    as = &(gvs->z_axis_spec);
    z_axis = AxisView::New(as);
    children.Add(z_axis);
  }
  // x axis
  as = &(gvs->x_axis_spec);
  x_axis = AxisView::New(as);
  children.Add(x_axis);


  // y axes and lines
  GraphletSpec* graphlet = this->graphlet(); // cache

  //note: list should never be empty, but can't display graph w/o one
  for (int i = 0; i < graphlet->size; ++i) {
    as = graphlet->FastEl(i);
    AxisView* av = AxisView::New(as);
    children.Add(av); // autoadds to y_axis list
    // lines for this axis
    taLeafItr j;
    GraphColSpec* col;
    FOR_ITR_EL(GraphColSpec, col, gvs->, j) {
      //must be for this axis, visible, and be a graph line
      if ( (col->axis_spec != as)
        || (!col->visible)
        || (!((col->col_type == GraphColSpec::AUTO) || (col->col_type == GraphColSpec::Y_AXIS)
           || (col->col_type == GraphColSpec::Y_DATA)))
      ) continue;
      GraphLine* gl = GraphLine::New(col);
      gl->x_axis = x_axis;
      gl->y_axis = av;
      gl->z_axis = z_axis;
      children.Add(gl);
    }
  }

  // lines

//  viewer->cur_tool(Tool::move);
}

void GraphView::ChildAdding(taDataView* child_) {
  inherited::ChildAdding(child_);
  T3DataView* child = dynamic_cast<T3DataView*>(child_);
  if (!child) return;
  TypeDef* typ = child->GetTypeDef();
  if (typ->InheritsFrom(&TA_GraphLine)) {
    lines.AddUnique(child);
  } else if (typ->InheritsFrom(&TA_AxisView)) {
    switch (((AxisView*)child)->axis()) {
    case AxisView::X: x_axis = (AxisView*)child; break;
    case AxisView::Y: y_axes.AddUnique(child); break;
    case AxisView::Z: z_axis = (AxisView*)child; break;
    }
  }
}

void GraphView::ChildRemoving(taDataView* child_) {
  //note: because we can call ourself recursively, we always make sure
  // we have fully completed ourself and our inherited, before reinvoking
  bool nuke_lines = false;
  T3DataView* child = dynamic_cast<T3DataView*>(child_);
  if (!child) goto done;
  if (child == x_axis) {x_axis = NULL; nuke_lines = true; goto done;}
  if (child == z_axis) {z_axis = NULL; nuke_lines = true; goto done;}
  if (lines.Remove(child)) goto done;
  // must be a y axis -- make sure not still connected to an existing line
  if (y_axes.Remove(child)) {
    inherited::ChildRemoving(child_);
    for (int i = lines.size - 1; i >=0; --i) {
      GraphLine* gl = (GraphLine*)lines.FastEl(i);
      if (gl->y_axis == child) {
        lines.Remove(i); //optimization
        delete gl; // will invoke us recursively
      }
    }
    return; // inherited already invoked
  }

done:
  inherited::ChildRemoving(child_);
  // make sure that if x or z removed, must recursively nuke all lines
  if (nuke_lines) while (lines.size > 0) {
    GraphLine* gl = (GraphLine*)lines.Pop();
    delete gl; // will invoke us recursively
  }
}

GraphSpec* GraphView::graph_spec() {
  if (!m_graph_spec) {
    m_graph_spec = (GraphSpec*)(m_data->GetOwner(&TA_GraphSpec));
  }
  return m_graph_spec;
}

bool GraphView::InitUpdateYAxes(bool init) {
  bool rval = false;
  for (int i = 0; i < y_axes.size; ++i) {
    AxisView* ax = (AxisView*)y_axes.FastEl(i);
    bool r =  ax->InitUpdateAxis(init);
    rval = rval || r;
  }
  return rval;
}

void GraphView::ReInit_impl() {
  //note: x and z axes have been dealt with
  InitUpdateYAxes(true);
//  RedrawLines();
  inherited::ReInit_impl(); // does Reinit on lines, which redraws them
}

void GraphView::Redraw() {
  InitUpdateYAxes(true);
  RedrawLines();
}

void GraphView::RedrawLines() {
  // redraw all the existing lines
  for (int i = lines.size - 1; i >= 0; --i) {
    GraphLine* gl = (GraphLine*)lines.FastEl(i);
    gl->Redraw();
  }
}

void GraphView::Render_impl() {
  // assert y-axis geometries
  float ax_len = geom.y - (origin.y + GRAPH_MARGIN);
  //note: list should never be empty, but can't display graph w/o one
  for (int j = 0; j < y_axes.size; ++j) {
    AxisView* av = (AxisView*)y_axes.FastEl(j);
    FloatTransform* ft = av->transform(true); //note: Inventor coords
    ft->translate.SetXYZ(origin.x - (((AXIS_WIDTH * j) + ORIGIN_OFFSET)),
      origin.y,
      -origin.z);
    av->axis_length = ax_len;
  }
  // x axis geometry
  ax_len = geom.x - (origin.x + GRAPH_MARGIN);
  if (x_axis) {
    FloatTransform* ft = x_axis->transform(true); //note: Inventor coords
    ft->translate.SetXYZ(origin.x,
      origin.y - ORIGIN_OFFSET,
      -origin.z);
    x_axis->axis_length = ax_len;
  }
  // z axis geometry (3d only)
  ax_len = geom.z - (origin.z + GRAPH_MARGIN);
  if (z_axis) {
    FloatTransform* ft = z_axis->transform(true); //note: Inventor coords
    ft->translate.SetXYZ(origin.x,
      origin.y,
      -(origin.z - ORIGIN_OFFSET));
    z_axis->axis_length = ax_len;
  }

  inherited::Render_impl();
}

void GraphView::Render_pre(taDataView* par) {
  m_node_so = new T3Graph(this);

  // origin is invariant (otherwise graphlets get rebuilt)
  origin.x = GRAPH_MARGIN + (y_axes.size * AXIS_WIDTH) + ORIGIN_OFFSET;
  origin.y = GRAPH_MARGIN + AXIS_WIDTH + ORIGIN_OFFSET;
  //TODO: z for 3d

  // origin of lines
  for (int i = 0; i < lines.size; ++i) {
    GraphLine* gl = (GraphLine*)lines.FastEl(i);
    FloatTransform* ft = gl->transform(true); //note: Inventor coords
    ft->translate.SetXYZ(origin.x, origin.y, -origin.z);
  }
  inherited::Render_pre(par);
}

void GraphView::Reset_impl() {
  lines.Reset(); //optimization
  y_axes.Reset(); //optimization
  inherited::Reset_impl();
}

/*void GraphView::SetXAxis(XAxisView* axis) {
  if (axis == NULL) return;
  graph_spec()->SetAxisFromSpec(x_axis);
}

void GraphView::AddYAxis(AxisSpec* axis) {
  AxisView* av = AxisView::New(axis);
  children.Add(av); // autoadds to list
  // the 35  vvv is to create space for the text numbers
//TODO  ivGlyph* axrep = layout->margin(but->axis,35,0,0,0);
} */

/*TODO
void GraphView::GetLook() {
  // margin = lrbt

  ivGlyph* whole_box;
  if(y_axis_box->count() > 0) {
    ivGlyph* ybox =
      layout->vbox
      (layout->margin(y_axis_patch,  5.0,  0.0,
		      GRAPH_BOT_BORDER, GRAPH_TOP_BORDER),
       layout->shape_of_xy
       (layout->margin(y_axis_patch, 5.0,  0.0,
		       GRAPH_BOT_BORDER, GRAPH_TOP_BORDER),
	layout->margin(x_axis_patch, GRAPH_LFT_BORDER, GRAPH_RGT_BORDER,
		       20.0, 0.0)));

    ivPolyGlyph* line_xax_box =
      layout->vbox
      (layout->hcenter(layout->margin(layout->flexible(linespace),
				      GRAPH_LFT_BORDER,GRAPH_RGT_BORDER,
				      GRAPH_BOT_BORDER,GRAPH_TOP_BORDER),0));

    if(!nodisp_x_axis)
      line_xax_box->append
	(layout->hcenter(layout->margin(x_axis_patch,
					GRAPH_LFT_BORDER,GRAPH_RGT_BORDER,
					20.0,0.0),0));

    whole_box = layout->hbox(ybox, line_xax_box);
  }
  else {
    whole_box = layout->hbox(taivM->hsep);
  }

  float width = 400.0f / graph_spec()->eff_layout.x;
  float height = 200.0f / graph_spec()->eff_layout.y;

  ivGlyph* body_hbox =
    layout->natural(whole_box, width, height);

  body(body_hbox);
  ivResource::flush();
} */

//////////////////////////
// 	GraphViews	//
//////////////////////////

GraphViews* GraphViews::New(GraphSpec* data) {
  GraphViews* rval = new GraphViews();
  data->AddDataView(rval);
  return rval;
}

void GraphViews::Initialize() {
}

void GraphViews::Destroy() {
  Reset();
  CutLinks();
}

void GraphViews::InitLinks() {
  inherited::InitLinks();
  taBase::Own(geom, this);
  taBase::Own(eff_layout, this);
}

void GraphViews::CutLinks() {
  eff_layout.CutLinks();
  geom.CutLinks();
  inherited::CutLinks();
}

bool GraphViews::AddLastPoint() {
  bool redraw = InitUpdateXZAxes(false);
  //TODO: this may not account for one graph with traces reflowing the z axis, thus necessitating
        // redraw for all!
  for (int i = 0; (i < children.size) && (!redraw); ++i) {
    GraphView* gv = (GraphView*)children.FastEl(i);
    redraw = gv->InitUpdateYAxes(false);
    if (!redraw) { // some axis has changed, so must redraw
      redraw = gv->AddLastPoint(); //note: could still cause this graph to completely redraw
    }
  }
  if (redraw)
    Redraw();
  return redraw;
}

void GraphViews::BuildAll() {
  Reset_impl(); // for 2nd+ call
  GraphSpec* graph = this->graph_spec(); //cache
  // make the graphlets
  for (int i = 0; i < graph->graphlets.size; ++i) {
    GraphletSpec* gls = graph->graphlets.FastEl(i);
    GraphView* gv = GraphView::New(gls);
    children.Add(gv);
    gv->BuildAll();
  }

/*TODO  // now get the y axes
  for (int i=0; i < graph->axes.size; i++) {
    Axis* ax = graph->axes->FastEl(i);
    if (!ax->spec->visible && !ax->is_shared_axis) continue;
    if ((ax == graph->x_axis) || (ax->axis != ax)) continue;
    DataArray_impl* da = ax->spec->data_array;
    if (da == NULL) continue;
    float amin = 0.0f; float amax = 0.0f;
    if (da->InheritsFrom(TA_float_Data)) {
      float_Data* fd = (float_Data*)da;
      amin = fd->ar.range.min; amax = fd->ar.range.max;
    }
    YAxisView* yax = YAxisView::New(amin,amax,but->spec->ta_color.color);
    but->axis = yax;
    but->x_axis = but->graph->x_axis;

    SetAxisFromSpec(yax, but);
    but->graph->AddYAxis(but);
  }

  // now share the y axes with all columns
  for(int i=0;i<buttons.size;i++) {
    iGraphButton* but = (iGraphButton*)buttons[i];
    if(!but->spec->visible && !but->is_shared_axis) continue;
    if((but == xbut) || (but->axis_but == but)) continue;
    but->axis = but->axis_but->axis;
    but->x_axis = but->graph->x_axis;
  }

  // do this part at end after graphs are rendered
  for(int i=0;i<n_graphs;i++) {
    GraphView* gg = (GraphView*)graphs[i];
    gg->GetLook();
    graph_grid->replace(i, gg);
  }

  graph_patch->body(graph_grid); // this should finally get rid of previous graph grid

  if(use_cbar)
    cbar_deck->flip_to(0);
  else
    cbar_deck->flip_to(1); */
}

void GraphViews::ChildAdding(taDataView* child_) {
  inherited::ChildAdding(child_);
  T3DataView* child = dynamic_cast<T3DataView*>(child_);
  if (!child) return;
  TypeDef* typ = child->GetTypeDef();
  if (typ->InheritsFrom(&TA_GraphView)) {
    graphs.AddUnique(child);
  }
}

void GraphViews::ChildRemoving(taDataView* child_) {
  T3DataView* child = dynamic_cast<T3DataView*>(child_);
  if (!child) goto done;
  if (graphs.Remove(child)) goto done;
done:
  inherited::ChildRemoving(child_);
}

void GraphViews::Clear_impl(taDataView* par) {
  if (par && par->InheritsFrom(&TA_T3DataView) && m_bar_so.ptr()) {
    ((T3DataView*)par)->AddRemoveChildNode(m_bar_so.ptr(), false);
  }
  m_bar_so = NULL;
  inherited::Clear_impl(par);
}

bool GraphViews::InitUpdateXZAxes(bool init) {
  GraphSpec* gvs = graph_spec(); // cache

  bool rval_x = gvs->x_axis_spec.InitUpdateAxis(init);
  bool rval_z = false;
  if (gvs->is3d()) {
    rval_z = gvs->z_axis_spec.InitUpdateAxis(init);
  }
  if (rval_x || rval_z || init) {
    for (int i = 0; i < children.size; ++i) {
      GraphView* gv = (GraphView*)children.FastEl(i);
      if ((rval_x || init) && gv->x_axis) //should always exist
        gv->x_axis->UpdateAxis();
      if ((rval_z || init) && gv->z_axis)
        gv->z_axis->UpdateAxis();
    }
  }
  return (rval_x || rval_z || init);
}

void GraphViews::Redraw() {
//TEMP
GraphSpec* gvs = graph_spec(); // cache
gvs->view_range.min = 0;
gvs->view_range.max = gvs->data_table->rows - 1;

  InitUpdateXZAxes(true); // ignore result
  for (int i = 0; i < children.size; ++i) {
    GraphView* gv = (GraphView*)children.FastEl(i);
    gv->Redraw();
  }
//  ReInit();
}

void GraphViews::ReInit_impl() {
  InitUpdateXZAxes(true); // ignore result
  inherited::ReInit_impl(); //reinits graphs
}

void GraphViews::Render_pre(taDataView* par) {
  GraphSpec* gvs = graph_spec(); // cache
  m_node_so = new T3NodeParent(this);

  // if using a color bar, then create its rep now
  //note: following if test is truly gross!
  if (gvs && gvs->use_cbar && par && par->InheritsFrom(&TA_T3DataView)) {
    T3CBar* t3cb = new T3CBar(gvs->scale, this);
    m_bar_so = t3cb;
    ((T3DataView*)par)->AddRemoveChildNode(t3cb, true);
  }

  inherited::Render_pre(par);
}


void GraphViews::Render_impl() {
  GraphSpec* gs = graph_spec(); // cache
  iVec3f act_geom = geom; // need to adjust for colorscale if used; note: in PDP frame

  //set the subgraph pos and geom values
  eff_layout = gs->graph_layout;
  eff_layout.FitN(graphs.size);
  while (eff_layout.x * eff_layout.y > graphs.size) {
    if (eff_layout.x >= eff_layout.y)
      eff_layout.x--;
    else
      eff_layout.y--;
    eff_layout.x = MAX(eff_layout.x, 1);
    eff_layout.y = MAX(eff_layout.y, 1);
  }
  if (eff_layout.x * eff_layout.y < graphs.size) {
    if(eff_layout.x >= eff_layout.y)
      eff_layout.x++;
    else
      eff_layout.y++;
  }
  eff_layout.FitN(graphs.size);	// double check!
  int xc = 0;
  int yc = eff_layout.y - 1;

  T3CBar* bar_so = this->bar_so();
  if (bar_so) {
    bar_so->SetDimensions(geom.x - (2 * GRAPH_MARGIN), T3BAR_HEIGHT);
    SoTransform* bar_txfm = bar_so->transform();
    bar_txfm->translation.setValue(geom.x / 2.0f, (bar_so->height / 2.0f) + ELEM_SPACE, 0.0f);
    act_geom.z -= bar_so->height + (2.0f * ELEM_SPACE); //note: still in PDP frame
  }
  // now, render out the graphlets
  for (int i=0; i<graphs.size; i++) {
    GraphView* gv = (GraphView*)graphs[i];
    //TODO: set pos and geom of graph
    gv->geom.x = act_geom.x / (float)eff_layout.x;
    gv->geom.y = act_geom.z / (float)eff_layout.y; //note: conversion from PDP to Graph frame
    gv->geom.z = act_geom.y ; //note: conversion from PDP to Graph frame

    gv->transform(true)->translate.x = gv->geom.x * (float)xc;
    gv->transform()->translate.y = (gv->geom.y * (float)yc) + (geom.z - act_geom.z);
// no PDP-y translate
    xc++;
    if (xc >= eff_layout.x) {
      xc = 0;
      yc--;
    }
  }
  inherited::Render_impl();
}

void GraphViews::Reset_impl() {
//  graphs.Reset(); //optimization
  inherited::Reset_impl();
}


//////////////////////////
//   iAxisButton	//
//////////////////////////

class iAxisButton: public Q3Button {
typedef Q3Button inherited;
public:
  iGraphButton*		gb() {return (iGraphButton*)parent();}
  iAxisButton(iGraphButton* parent);
  ~iAxisButton();
protected:
  void 		drawButton(QPainter *paint); // override
};

iAxisButton::iAxisButton(iGraphButton* parent)
:inherited(parent)
{
}

iAxisButton::~iAxisButton() {
}

void iAxisButton::drawButton(QPainter *p) {
  inherited::drawButton(p);
  iGraphButton* gb = this->gb();
  if (!gb) return;
  QRect r(1, 1, width() - 2, height() - 2);
  gb->paintAxis(*p, r);


}
//////////////////////////
// 	iGraphButton	//
//////////////////////////

//notes:
// checkboxes consume some dummy space for even empty labels
#define GBS_LINECOL_WD 60
#define GBS_VISCOL_WD 48
#define GBS_AXISCOL_WD 60
#define MIN_GBS_NAMECOL_WD 100
#define MIN_GB_WD (GBS_LINECOL_WD + GBS_VISCOL_WD + GBS_AXISCOL_WD + MIN_GBS_NAMECOL_WD)


iGraphButton::iGraphButton(int field_, GraphColSpec* line_, AxisSpec* axis_, QWidget* parent)
:inherited(parent)
{
  field = field_;
  line = line_;
  axis = axis_;
  btnLine = NULL;
  btnAxis = NULL;
  init();
}

void iGraphButton::init() {
  updating = 0;
  ++updating;
  setFixedHeight(taiM->button_height(taiMisc::sizMedium));
  setMinimumWidth(MIN_GB_WD);
  chkShow = new iCheckBox(this);
  chkShow->setMaximumWidth(GBS_VISCOL_WD);
  chkShow->move(GBS_LINECOL_WD + ((GBS_AXISCOL_WD - chkShow->width()) / 2),
    (height() - chkShow->height()) / 2);
  connect(chkShow, SIGNAL(toggled(bool)), this, SLOT(chkShow_toggled(bool)) );
  configure();
  --updating;
}

iGraphButton::~iGraphButton() {
  if (parent) {
    parent->ButtonDeleting(this);
    parent = NULL;
  }
  line = NULL;
  axis = NULL;
  btnLine = NULL;
  btnAxis = NULL;
}

void iGraphButton::btnAxis_pressed() {
  if (updating) return;
  //TODO: edit modally
  if (axis)
     axis->Edit();
}

void iGraphButton::btnLine_pressed() {
  if (updating) return;
  //TODO: edit modally
  if (line)
    line->Edit();
}

void iGraphButton::chkShow_toggled(bool on) {
  if (updating) return;
  ShowHideLine();
  if (line)
    line->DataChanged(DCR_ITEM_UPDATED); //obstabMisc::NotifyEdits(line);
  if (axis)
    axis->DataChanged(DCR_ITEM_UPDATED); //obstabMisc::NotifyEdits(axis);
}

void iGraphButton::configure() {
  ++updating;
  bool visible = false;
  bool visible_ro = false;
  //todo: create/delete buts as needed
  if (line) {
    if (!btnLine) {
      btnLine = new QPushButton(this);
      btnLine->setFixedHeight(taiM->button_height(taiMisc::sizMedium));
      btnLine->setMinimumWidth(MIN_GBS_NAMECOL_WD);
      btnLine->move(GBS_LINECOL_WD + GBS_VISCOL_WD + GBS_AXISCOL_WD,
          (height() - btnLine->height()) / 2);
      connect(btnLine, SIGNAL(pressed()), this, SLOT(btnLine_pressed()) );
      btnLine->show(); // needed after initial creation
    }
    // set label
    btnLine->setText(line->name); // TODO: maybe should be display name
    visible = line->visible;
    btnLine->update();
  } else {
     if (btnLine) {
      btnLine->deleteLater();
      btnLine = NULL;
    }
  }
  if (axis) {
    if (!btnAxis) {
      btnAxis = new iAxisButton(this);
      btnAxis->setFixedSize(GBS_AXISCOL_WD, taiM->button_height(taiMisc::sizMedium));
      btnAxis->move(GBS_LINECOL_WD + GBS_VISCOL_WD,
          (height() - btnAxis->height()) / 2);
      connect(btnAxis, SIGNAL(pressed()), this, SLOT(btnAxis_pressed()) );
      btnAxis->show(); // needed after initial creation
    }
    // if x or z, then disable the visibility
    visible_ro = ((axis->axis() == AxisSpec::X) ||(axis->axis() == AxisSpec::Z));
    // note: button paints itself
    btnAxis->update();

  } else {
     if (btnAxis) {
      btnAxis->deleteLater();
      btnAxis = NULL;
    }
  }
  chkShow->setChecked(visible);
  chkShow->setReadOnly(visible_ro);
  update();
  --updating;
}

void iGraphButton::SetColAxis(int field_, GraphColSpec* col_, AxisSpec* axis_, bool force) {
  if (!force && (field == field_) && (line == col_) && (axis == axis_)) return;
  field = field_;
  line = col_;
  axis = axis_;
  configure();
}


bool iGraphButton::IsChosen() {
  return chkShow->isChecked();
}

/*TODO: replace void iGraphButton::release(const ivEvent& e) {
  if(press_button == ivEvent::middle) {
    SetXAxis();
    tabMisc::NotifyEdits(spec);
    tabMisc::NotifyEdits(graph_spec()->owner); // the graphlogview
    return;
  }
  if(press_button == ivEvent::right) {
    osboolean chosen = state()->test(ivTelltaleState::is_chosen);
    ivButton::release(e);
    if(graph_spec()->x_axis_index == field) {
      state()->set(ivTelltaleState::is_active, true);
    }
    state()->set(ivTelltaleState::is_chosen, chosen);
    spec->Edit();
    return;
  }
  if(graph_spec()->x_axis_index != field) { // don't do x-axis
    ivButton::release(e);
    ToggleLine();
    tabMisc::NotifyEdits(spec);
  }
} */

void iGraphButton::paintAxis(QPainter& p, const QRect& r) {
  const iColor* color = NULL;
  String symbol;
  // if this actually is an axis, then we get color directly there, and render its symbol
  if (axis) {
    color = axis->def_color();
    switch (axis->axis()) {
    case AxisSpec::X: symbol = "X"; break;
    case AxisSpec::Y: symbol = "Y"; break;
    case AxisSpec::Z: symbol = "Z"; break;
    }
  } else {
    if (line && line->axis_spec) {
      color = line->axis_spec->def_color();
    }
  }
  paintLegend(p, r, color, symbol);
}

void iGraphButton::paintEvent(QPaintEvent* ev) {
  inherited::paintEvent(ev);
  QPainter p(this);
  QRect r;

  r.setRect(1, 1, GBS_LINECOL_WD - 2, height() - 2);
  // erase first
  p.setBackgroundColor(paletteBackgroundColor()); // this is for the text
  p.setBackgroundMode(OpaqueMode);
  p.fillRect(r, paletteBackgroundColor());
  p.translate(1, 1);
  paintLine(p, r);

  //note: axis button paints itself, so we don't; this is only when there is no button
  if (!btnAxis) {
    r.setRect(GBS_LINECOL_WD + GBS_VISCOL_WD + 1, 1, GBS_AXISCOL_WD - 2, height() - 2);
    p.fillRect(r, paletteBackgroundColor());
    p.translate(GBS_LINECOL_WD + GBS_VISCOL_WD + 1, 1);
    paintAxis(p, r);
  }
}

void iGraphButton::paintLegend(QPainter& p, const QRect& r, const iColor* color, String symbol,
  bool draw_line, PenStyle ps)
{
  QColor c;
  if (color) c = (QColor)(*color); // else black

  QBrush b;
  b.setColor(c);
  b.setStyle(SolidPattern);
  p.setBrush(b);

  QPen pen;
  pen.setStyle(ps);
  pen.setWidth(4);
  pen.setColor(c);
  p.setPen(pen);

  // for simplicity, we just draw the line, then render the symbol, if any, over top
  if (draw_line) {
    int y12 = r.height() / 2;
    p.drawLine(2, y12,  r.width() - 4,  y12);
  }
  if (!symbol.empty()) {
    int tf = AlignHCenter | AlignVCenter;
    //TODO: prob need to figure text metrics, to specify size
    p.setBackgroundMode(OpaqueMode);
    p.drawText(2, 2,  r.width() - 4,  r.height() - 2, tf, symbol);
  }
}

void iGraphButton::paintLine(QPainter& p, const QRect& r) {
  const iColor* color = NULL;
  PenStyle ps = SolidLine;
  // we draw a line if: "is_line" (note: needn't be visible)
  bool draw_line = (line && (line->is_line()));
  if (draw_line) {
    switch (line->line_style) {
    case GraphColSpec::SOLID: ps = SolidLine; break;
    case GraphColSpec::DOT: ps = DotLine; break;
    case GraphColSpec::DASH: ps = DashLine; break;
    case GraphColSpec::DASH_DOT: ps = DashDotLine; break;
    default: ps = SolidLine; break; // shouldn't happen
    }
    color = line->def_color();
  }
  // 'n/a' conditions are: x-axis, z-axis, string
  String symbol;
  if ((axis && ((axis->axis() == AxisSpec::X) || (axis->axis() == AxisSpec::Z)))
  ) {
    symbol = "n/a";
  } else if (line && (line->is_string())) {
    symbol = "str";
    color = line->def_color();
  }

  paintLegend(p, r, color, symbol, draw_line, ps);
}


void iGraphButton::resizeEvent(QResizeEvent* ev) {
  inherited::resizeEvent(ev);
  if (btnLine) {
    btnLine->resize(((width() - btnLine->x()) - 1), btnLine->height());
  }
}

void iGraphButton::setShowLine(bool show) {
  ++updating;
  chkShow->setChecked(show);
  --updating;
}

QSize iGraphButton::sizeHint() const {
  int pref_wd = GBS_LINECOL_WD + GBS_VISCOL_WD + GBS_AXISCOL_WD;
  if (btnLine) {
    QSize lsz = btnLine->sizeHint();
    pref_wd += MAX(MIN_GBS_NAMECOL_WD, lsz.width());
  } else {
    pref_wd += MIN_GBS_NAMECOL_WD;
  }

  return QSize(pref_wd,  height());
}

void iGraphButton::ShowHideLine() {
  if (!line) return;
  setShowLine(line->setVisible(IsChosen())); // note: we don't always honor a set request
}

QSizePolicy iGraphButton::sizePolicy () const {
  return QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
}

//////////////////////////
// 	iGraphButtons	//
//////////////////////////

class GraphButtonsDataView: public taDataView {
typedef taDataView inherited;
public:
  static GraphButtonsDataView* New(GraphSpec* graph, iGraphButtons* gbs_);
  iGraphButtons*		gbs;
  override void		DataDestroying();
  GraphButtonsDataView() {}
  ~GraphButtonsDataView() {}
protected:
  override void		DataUpdateAfterEdit_impl() {Render_impl();}
  override void		Render_impl(); // called on any kind of update
};

GraphButtonsDataView* GraphButtonsDataView::New(GraphSpec* graph, iGraphButtons* gbs_)
{
  GraphButtonsDataView* rval = new GraphButtonsDataView();
  rval->gbs = gbs_;
  graph->AddDataView(rval);
  return rval;
}

void GraphButtonsDataView::DataDestroying() {
  if (gbs) {
    gbs->dv_graph = NULL; // avoid calling us back
    gbs->setGraph(NULL);
  }
  inherited::DataDestroying();
}

void GraphButtonsDataView::Render_impl() {
  if (gbs) {
    gbs->Rebuild();
  }
}


iGraphButtons::iGraphButtons(QWidget* parent)
:inherited(parent)
{
  m_graph = NULL;
  dv_graph = NULL;
  updating = 0;
  scr = new QScrollArea(this);
//  scr->setHScrollBarMode(Q3ScrollView::AlwaysOff);
//  scr->setVScrollBarMode(Q3ScrollView::AlwaysOn); //not super pretty, but eliminates complex sizing issues
  scr->setWidgetResizable(true);

  buts = new QWidget();
  layWidg = new QVBoxLayout(buts);

  hdr = new Q3Header(buts);
  hdr->setClickEnabled(false);
  hdr->setResizeEnabled(false);
  hdr->setMovingEnabled(false);
  hdr->addLabel("line", GBS_LINECOL_WD);
  hdr->addLabel("visible", GBS_VISCOL_WD);
  hdr->addLabel("axis", GBS_AXISCOL_WD);
  hdr->addLabel("column");
  hdr->setStretchEnabled(true, 3); // last line takes up the slack space
  hdr->adjustHeaderSize();
  layWidg->addWidget(hdr);
  layWidg->addStretch(); // note: always floats at end

  scr->setWidget(buts);
  buttons = new QObjectList();

  setMinimumSize(MIN_GB_WD, (hdr->height() + 40 )) ; // TODO: better value for height, ex. 2 buts

}

iGraphButtons::~iGraphButtons() {
  setGraph(NULL); // does a controlled delete of all buttons

  delete buttons;
  buttons = NULL;
}


void iGraphButtons::AddGraphButton(iGraphButton* gb) {
//  gb->reparent(buts->viewport(), QPoint()); // must layout controls later
//  buts->addChild(gb);
  gb->reparent(buts, QPoint());
  buttons->append(gb);
  int in_idx = buttons->count(); //note: 1 greater because of header
  layWidg->insertWidget(in_idx, gb);
  gb->parent = this;
  gb->show();
}

void iGraphButtons::ButtonDeleting(iGraphButton* gb) {
  buttons->remove(gb);
  gb->parent = NULL;
}

int iGraphButtons::buttonCount() {
  return buttons->count();
}

iGraphButton* iGraphButtons::button(int i){
  if ((i < 0) || (i >= (int)buttons->count()))
      return NULL;
  else return (iGraphButton*)buttons->at(i);
}

void iGraphButtons::Rebuild() {
  ++updating;
  int zombies = 0; // zombie is an axis w/o a line -- usually only for z that has no col
  if (m_graph) {
    taLeafItr itr;
    int i = 0;
    GraphColSpec* spec;
    AxisSpec* axis;
    iGraphButton* but;
    FOR_ITR_EL(GraphColSpec, spec, m_graph->, itr) {
      if (spec->is_axis()) {
        axis = spec->axis_spec;
      } else {
        axis = NULL;
      }
      but = button(i); // NULL if out of bounds
      if (but) { // existing
        but->SetColAxis(i, spec, axis, true);
      } else { // need new
        but = new iGraphButton(i, spec, axis);
        AddGraphButton(but);
      }
      ++i;
    }
    // now, deal with the extra Z axis needed in zombie situation
    axis = &(m_graph->z_axis_spec);
    if (!axis->spec) {
      ++zombies;
      but = button(m_graph->leaves); // NULL if out of bounds
      if (but) { // existing
        but->SetColAxis(-1, NULL, axis, true);
      } else { // need new
        but = new iGraphButton(-1, NULL, axis);
        AddGraphButton(but);
      }
    }
    // remove any extras (normally shouldn't be any at this point!)
    for (int i =  buttonCount() - 1; i >= (m_graph->leaves + zombies); --i) {
      RemoveGraphButton(i);
    }
  } else {
    for (int i = buttonCount() - 1; i >= 0; --i) {
      RemoveGraphButton(i);
    }
  }
  --updating;
}

void iGraphButtons::RemoveGraphButton(int i) {
  iGraphButton* gb = button(i);
  if (!gb) return;
  buttons->removeAt((uint)i);
  gb->parent = NULL; // prevents it calling us back on delete
  gb->deleteLater();
}

void iGraphButtons::resizeEvent(QResizeEvent* ev) {
  inherited::resizeEvent(ev);
  scr->resize(size());
}

void iGraphButtons::setGraph(GraphSpec* value, bool force_rebuild) {
  if ((m_graph == value) && !force_rebuild) return;
  if (m_graph != value) {
    if (dv_graph) {
      dv_graph->gbs = NULL; // prevent callback
      delete dv_graph;
      dv_graph = NULL; // prevent callback
    }
    if (value)
      dv_graph = GraphButtonsDataView::New(value, this);
  }
  m_graph = value;
  Rebuild();
}

QSize iGraphButtons::sizeHint() const {
  //we just hack the overhead outside the scroll region, so we don't get scroll bars by default.
  QSize rval(buts->width() + 4, buts->height() + 2);
  return rval;
}

QSizePolicy iGraphButtons::sizePolicy () const {
  return QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
}
