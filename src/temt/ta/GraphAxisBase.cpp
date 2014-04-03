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

#include "GraphAxisBase.h"
#include <GraphTableView>
#include <GraphColView>
#include <DataCol>
#include <DataTable>
#include <T3Axis>
#include <taMath_float>
#include <iViewPanelOfGraphTable>
#include <taSvg>

#include <taMisc>

#include <Inventor/SbLinear.h>
#include <Inventor/fields/SoMFString.h>
#include <Inventor/nodes/SoAsciiText.h>
#include <Inventor/nodes/SoBaseColor.h>
#include <Inventor/nodes/SoFont.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoComplexity.h>
#include <Inventor/nodes/SoText2.h>

TA_BASEFUNS_CTORS_DEFN(GraphAxisBase);


#define UNIT_LEGEND_OFFSET 0.04f // space between end of axis and unit legend text
#define TICK_OFFSET 0.01f // gap between tick and label


void GraphAxisBase::Initialize() {
  on = true;
  axis = Y;
  col_lookup = NULL;
  is_matrix = false;
  n_cells = 1;
  matrix_cell = 0;
  col_list = NULL;
  color.name = taMisc::t3d_text_color;
  n_ticks = 10;
  axis_length = 1.0f;
  start_tick = 0;
  tick_incr = 1;
  act_n_ticks = 11;
  units = 1.;
}

void GraphAxisBase::Destroy() {
  CutLinks();
}

void GraphAxisBase::InitLinks() {
  inherited::InitLinks();
  taBase::Own(range, this);
  taBase::Own(data_range, this);
  taBase::Own(fixed_range, this);
  taBase::Own(color, this);

  GraphTableView* gtv = GetGTV();
  if(gtv) {
    taBase::SetPointer((taBase**)&col_list, &(gtv->children));
  }
}

void GraphAxisBase::CutLinks() {
  if(col_lookup)
    taBase::SetPointer((taBase**)&col_lookup, NULL);
  if(col_list)
    taBase::SetPointer((taBase**)&col_list, NULL);
  fixed_range.CutLinks();
  data_range.CutLinks();
  range.CutLinks();
  color.CutLinks();
  inherited::CutLinks();
}

GraphTableView* GraphAxisBase::GetGTV() {
  return GET_MY_OWNER(GraphTableView);
}

void GraphAxisBase::CopyFromView_base(GraphAxisBase* cp){
  on = cp->on;
  col_name = cp->col_name;
  fixed_range= cp->fixed_range;
  color= cp->color;
  n_ticks = cp->n_ticks;
}

void GraphAxisBase::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  color.UpdateAfterEdit_NoGui();
  UpdateFmColLookup();
  UpdateOnFlag();
  UpdateFmDataCol();
}

void GraphAxisBase::UpdateFmDataCol() {
  DataCol* dc = GetDAPtr();
  if(!dc) return;
  is_matrix = dc->is_matrix;
  if(is_matrix) {
    n_cells = dc->cell_geom.Product();
  }
  else {
    matrix_cell = 0;
    n_cells = 1;
  }
  is_string = dc->isString();
  if(is_matrix) {
    if(matrix_cell >= n_cells) 
      matrix_cell = n_cells-1;
    if((axis != Y || is_string) && matrix_cell < 0) // no all case for non-data guys
      matrix_cell = 0;
  }
}

void GraphAxisBase::UpdateOnFlag() {
  // nop at base level
}

void GraphAxisBase::UpdateFmColLookup() {
  if(col_lookup) {
    // first save fixed_range values back to that guy
    GraphColView* gcv = GetColPtr();
    if(gcv) {
      gcv->fixed_range = fixed_range;
    }
    col_name = col_lookup->GetName();
    fixed_range = col_lookup->fixed_range;           // get range from that guy
    if(taBase::GetRefn(col_lookup) <= 1) {
    }
    else {
      taBase::SetPointer((taBase**)&col_lookup, NULL); // reset as soon as used -- just a temp guy!
    }
  }
}

void GraphAxisBase::SetColPtr(GraphColView* cgv) {
  if(!cgv) {
    col_name = _nilString;
  }
  else {
    taBase::SetPointer((taBase**)&col_lookup, cgv);
    UpdateFmColLookup();
  }
  UpdateOnFlag();
  UpdateFmDataCol();
}


GraphColView* GraphAxisBase::GetColPtr() {
  if(col_name.empty()) return NULL;
  GraphTableView* gv = GET_MY_OWNER(GraphTableView);
  if(!gv) return NULL;
  return (GraphColView*)gv->children.FindName(col_name);
}

DataCol* GraphAxisBase::GetDAPtr() {
  GraphColView* cv = GetColPtr();
  if(!cv) return NULL;
  return cv->dataCol();
}

void GraphAxisBase::InitFromUserData() {
  GraphColView* cv = GetColPtr();
  if(!cv) return;
  DataCol* da = cv->dataCol();
  if(da->HasUserData("MIN")) {
    fixed_range.fix_min = true;
    fixed_range.min = da->GetUserDataAsFloat("MIN");
  }
  if(da->HasUserData("MAX")) {
    fixed_range.fix_max = true;
    fixed_range.max = da->GetUserDataAsFloat("MAX");
  }
  UpdateOnFlag();
  UpdateFmDataCol();
}

void GraphAxisBase::SetRange_impl(float first, float last) {
  data_range.Set(first, last);
  // update to relevant actual data values..
  if(!fixed_range.fix_min) fixed_range.min = data_range.min;
  if(!fixed_range.fix_max) fixed_range.max = data_range.max;
  fixed_range.FixRange(data_range); // keep range fixed!
  range = data_range;
  UpdateRange_impl(first, last);
}

// limits are needed to prevent numerical overflow!
static const float range_min_limit = 1.0e-18f;
static const float range_zero_range = 5.0e-17f; // half-range for zero-range values
static const float range_zero_label_range = 5.0e-6f; // for tick labels

// updates range based on new data and returns true if it really is a new range
bool GraphAxisBase::UpdateRange_impl(float first, float last) {
  data_range.UpdateRange(first);
  data_range.UpdateRange(last);

  if(((first >= range.min) && (last <= range.max)) && (range.Range() >= range_min_limit)) { // special case
    return false;                       // not changed
  }

  first = data_range.min;
  last = data_range.max;

  if (((last - first) < range_min_limit) || (range.Range() < range_min_limit)) {
    if (last - first >= range_min_limit) {
      range.max = last;
      range.min = first;
    } else {
      double maxval = MAX(fabs(last), fabs(first));
      if (maxval > .01f) {
        last += range_zero_range * maxval;
        first -= range_zero_range * maxval;
      } else {
        last += .51 * range_min_limit;
        first -= .51 * range_min_limit;
      }
    }
  }

  double rng = taMisc::NiceRoundNumber(last - first, false); // nicenum def'd in minmax
  rng = MAX(range_min_limit, rng);

  double inc = taMisc::NiceRoundNumber((rng / (double)n_ticks), true);
  double newmin = floor(first / inc) * inc;
  double newmax = ceil(last / inc) * inc;

  range.Set(newmin, newmax);
  fixed_range.FixRange(range);
  return true;                  // changed
}

void GraphAxisBase::ComputeRange() {
  if(!on) return;
  GraphColView* gcv = GetColPtr();
  if(gcv) {
    DataCol* da = gcv->dataCol();
    if(da->rows() == 0) {
      data_range.Set(0.0f, 1.0f);
    }
    else {
      if(is_matrix && matrix_cell >= 0) {
        int rwn = da->rows();
        float fval = da->GetValAsFloatM(0, matrix_cell);
        data_range.Set(fval, fval);
        for(int i = 1; i<rwn; i++) {
          data_range.UpdateRange(da->GetValAsFloatM(i, matrix_cell));
        }
      }
      else {
        da->GetMinMaxScale(data_range);
      }
    }
  }
  else {
    data_range.Set(0.0f, 1.0f);
  }
  SetRange_impl(data_range.min, data_range.max);
}

bool GraphAxisBase::UpdateRange() {
  bool rval = false;
  GraphColView* gcv = GetColPtr();
  if(gcv) {
    DataCol* da = gcv->dataCol();
    if(da->rows() > 0) {
      if(da->is_matrix) {
        float first = da->GetValAsFloatM(-1,0);
        float last = first;
        for(int i=1;i<da->cell_size();i++) {
          float val = da->GetValAsFloatM(-1,i);
          first = MIN(val, first);
          last = MAX(val, last);
        }
        rval = UpdateRange_impl(first, last);
      }
      else {
        float lstval = da->GetValAsFloat(-1);
        rval = UpdateRange_impl(lstval, lstval);
      }
    }
  }
  return rval;
}

void GraphAxisBase::ComputeTicks() {
  double min = range.min;
  double max = range.max;
  double rng = taMisc::NiceRoundNumber(max-min, false);
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

//   if(rng < 1.0e-8) rng = 1.0e-8;

  tick_incr = taMisc::NiceRoundNumber((rng / (double)n_ticks), true);
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

  units_order /= 3;             // introduce a factor of 2 rounding here..
  units_order *= 3;

  units = pow(10.0, (double)units_order);

  // empirically compute the actual number of ticks..
  double chk_max = max + (tick_incr / 100.0f); // give a little tolerance..
  double val;
  int i;
  for (i=0, val = start_tick; val <= chk_max; val += tick_incr, i++);
  act_n_ticks = i;
}

void GraphAxisBase::RenderAxis(T3Axis* t3ax, const iVec3f& offi,
                               int n_ax, bool ticks_only, String* rnd_svg) {
  t3ax->clear();
  if(!on) return;
  ComputeTicks();               // do this always..
  SoMaterial* mat = t3ax->material();
  color.color().copyTo(mat->diffuseColor);

  iVec3f off = offi;

  if(rnd_svg) {
    off.z = -offi.z;             // ugh, this is backward for some reason..
  }

  if(rnd_svg && ticks_only) {
    *rnd_svg << taSvg::Path(color.color(), 2.0f);
  }

  switch (axis) {
  case X: RenderAxis_X(t3ax, off, ticks_only, rnd_svg);
    break;
  case Y: RenderAxis_Y(t3ax, off, n_ax, ticks_only, rnd_svg);
    break;
  case Z: RenderAxis_Z(t3ax, off, ticks_only, rnd_svg);
    break;
  }

  if(rnd_svg && ticks_only) {
    *rnd_svg << taSvg::PathEnd();
  }
}

/* Axis rendering notes
  act_n_ticks is the number of sections, so tick marks will be +1 (to include ends)

*/
void GraphAxisBase::RenderAxis_X(T3Axis* t3ax, const iVec3f& off,
                                 bool ticks_only, String* rnd_svg) {
  iVec3f fm = off;
  iVec3f to = off;

  // axis line itself
  to.x += axis_length;
  t3ax->addLine(fm, to);

  if(rnd_svg && ticks_only) {
    *rnd_svg << "M " << taSvg::Coords(fm)
             << "L " << taSvg::Coords(to);
  }

  bool use_str_labels = false;

  if(!ticks_only) {
    // units legend
    if(units != 1.0) {
      fm.x = off.x + axis_length + UNIT_LEGEND_OFFSET;
      fm.y = off.y + -(.5f * GraphTableView::tick_size + TICK_OFFSET + t3ax->fontSize());
      String label = "x " + String(units,"%.5g");
      t3ax->addLabel(label.chars(), fm, SoAsciiText::LEFT);

      if(rnd_svg) {
        *rnd_svg << taSvg::Text(label, fm, color.color(),
                                0.05f, taSvg::LEFT);
      }
    }

    if(!col_name.empty()) {
      fm.x = off.x + .5f * axis_length;
      fm.y = off.y + -(GraphTableView::tick_size + TICK_OFFSET + 1.5f * t3ax->fontSize());
      String label = col_name;
      taMisc::SpaceLabel(label);
      if(((GraphAxisView*)this)->row_num) {
        if(isString()) {
          use_str_labels = true;
        }
        else {
          label = "Row Number";
        }
      }
      t3ax->addLabel(label.chars(), fm, SoAsciiText::CENTER);

      if(rnd_svg) {
        *rnd_svg << taSvg::Text(label, fm, color.color(),
                                0.05f, taSvg::CENTER);
      }
    }
  }

  // ticks
  fm = off;
  to = off;
  fm.y = off.y + -(.5f * GraphTableView::tick_size);
  to.y = off.y + (.5f * GraphTableView::tick_size);

  float y_lab_off = (TICK_OFFSET + t3ax->fontSize());

  DataCol* da = GetDAPtr();

  int i;
  float val;
  String label;
  for (i = 0, val = start_tick; i < act_n_ticks; val += tick_incr, ++i) {
    fm.x = off.x + DataToPlot(val);
    to.x = off.x + DataToPlot(val);
    t3ax->addLine(fm, to);

    if(rnd_svg && ticks_only) {
      *rnd_svg << "M " << taSvg::Coords(fm)
               << "L " << taSvg::Coords(to);
    }

    if(!ticks_only) {
      float lab_val = val / units;
      if (fabsf(val / tick_incr) < range_zero_label_range)
        lab_val = 0.0f;         // the 0 can be screwy
      label = String(lab_val);
      if(use_str_labels && da) {
        int rnum = (int)lab_val;// lab_val is row number!
        if((float)rnum == lab_val && rnum >= 0 && rnum < da->rows()) // only int and in range
          label = da->GetValAsString(rnum);
        else
          label = "";           // empty it!
      }
      if(label.nonempty()) {
        t3ax->addLabel(label.chars(),
                     iVec3f(fm.x, fm.y - y_lab_off, fm.z),
                     SoAsciiText::CENTER);
        if(rnd_svg) {
          // todo: get font size from graph
          *rnd_svg << taSvg::Text(label, fm.x, fm.y-y_lab_off, fm.z, color.color(),
                                  0.05f, taSvg::CENTER);
        }
      }
    }
  }
}

void GraphAxisBase::RenderAxis_Y(T3Axis* t3ax, const iVec3f& off,
                                 int n_ax, bool ticks_only, String* rnd_svg) {
  iVec3f fm = off;
  iVec3f to = off;

  // axis line itself
  to.y = off.y + axis_length;
  t3ax->addLine(fm, to);

  if(rnd_svg && ticks_only) {
    *rnd_svg << "M " << taSvg::Coords(fm)
             << "L " << taSvg::Coords(to);
  }

  if(!ticks_only) {
    // units legend
    if(units != 1.0) {
      fm.y = off.y + axis_length + UNIT_LEGEND_OFFSET;
      String label = "x " + String(units,"%.5g");
      if(n_ax > 0) {
        fm.x = off.x + TICK_OFFSET;
        t3ax->addLabel(label.chars(), fm, SoAsciiText::LEFT);
        if(rnd_svg) {
          *rnd_svg << taSvg::Text(label, fm, color.color(),
                                  0.05f, taSvg::LEFT);
        }
      }
      else {
        fm.x = off.x + -TICK_OFFSET;
        t3ax->addLabel(label.chars(), fm, SoAsciiText::RIGHT);
        if(rnd_svg) {
          *rnd_svg << taSvg::Text(label, fm, color.color(),
                                  0.05f, taSvg::RIGHT);
        }
      }
    }

    if(!col_name.empty()) {
      SbRotation rot;
      rot.setValue(SbVec3f(0.0, 0.0f, 1.0f), .5f * taMath_float::pi);
      fm.y = off.y + .5f * axis_length;
      String label = col_name; taMisc::SpaceLabel(label);
      if(n_ax > 0) {
        fm.x = off.x + GraphTableView::tick_size + TICK_OFFSET + 1.3f * t3ax->fontSize();
        t3ax->addLabelRot(label.chars(), fm, SoAsciiText::CENTER, rot);
        if(rnd_svg) {
          *rnd_svg << taSvg::Text(label, fm, color.color(),
                                  0.05f, taSvg::CENTER, true); // vertical
        }
      }
      else {
        fm.x = off.x + -GraphTableView::tick_size - TICK_OFFSET - 1.3f * t3ax->fontSize();
        t3ax->addLabelRot(label.chars(), fm, SoAsciiText::CENTER, rot);
        if(rnd_svg) {
          *rnd_svg << taSvg::Text(label, fm, color.color(),
                                  0.05f, taSvg::CENTER, true); // vertical
        }
      }
    }
  }

  // ticks
  fm = off;
  to = off;
  fm.x = off.x -(GraphTableView::tick_size / 2.0f);
  to.x = off.x + (GraphTableView::tick_size / 2.0f);

  int i;
  float val;
  String label;
  for (i = 0, val = start_tick; i < act_n_ticks; val += tick_incr, ++i) {
    fm.y = off.y + DataToPlot(val);
    to.y = off.y + DataToPlot(val);
    t3ax->addLine(fm, to);

    if(rnd_svg && ticks_only) {
      *rnd_svg << "M " << taSvg::Coords(fm)
               << "L " << taSvg::Coords(to);
    }

    if(!ticks_only) {
      float lab_val = val / units;
      if (fabsf(val / tick_incr) < range_zero_label_range)
        lab_val = 0.0f;         // the 0 can be screwy
      label = String(lab_val);
      fm.y -= (.4f * t3ax->fontSize()); // center vertically
      if(n_ax > 0) {
        t3ax->addLabel(label.chars(),
                       iVec3f(to.x + TICK_OFFSET, fm.y, fm.z));
        if(rnd_svg) {
          *rnd_svg << taSvg::Text(label, to.x + TICK_OFFSET,
                                  fm.y, fm.z, color.color(), 0.05f, taSvg::LEFT);
        }
      }
      else {
        t3ax->addLabel(label.chars(),
                       iVec3f(fm.x - TICK_OFFSET, fm.y, fm.z));
        if(rnd_svg) {
          *rnd_svg << taSvg::Text(label, fm.x - TICK_OFFSET,
                                  fm.y, fm.z, color.color(), 0.05f, taSvg::RIGHT);
        }
      }
    }
  }
}

void GraphAxisBase::RenderAxis_Z(T3Axis* t3ax, const iVec3f& off,
                                 bool ticks_only, String* rnd_svg) {
  iVec3f fm = off;
  iVec3f to = off;

  // axis line itself
  to.z = off.z + axis_length;
  t3ax->addLine(fm, to);

  bool use_str_labels = false;

  if(!ticks_only) {
    // units legend
    if(units != 1.0) {
      fm.z = off.z + axis_length + UNIT_LEGEND_OFFSET;
      fm.y = off.y + -(.5f * GraphTableView::tick_size + TICK_OFFSET + t3ax->fontSize());
      fm.x = off.x + -(TICK_OFFSET + 2.0f * t3ax->fontSize());
      String label = "x " + String(units,"%.5g");
      t3ax->addLabel(label.chars(), fm, SoAsciiText::RIGHT);
    }
    if(!col_name.empty()) {
      SbRotation rot;
      rot.setValue(SbVec3f(0.0, 1.0f, 0.0f), .5f * taMath_float::pi);

      fm.z = off.z + .5f * axis_length;
      fm.y = off.y + -(.5f * GraphTableView::tick_size + TICK_OFFSET + t3ax->fontSize());
      fm.x = off.x + -(TICK_OFFSET + 2.5f * t3ax->fontSize());
      String label = col_name;
      taMisc::SpaceLabel(label);
      if(((GraphAxisView*)this)->row_num) {
        if(isString()) {
          use_str_labels = true;
        }
        else {
          label = "Row Number";
        }
      }
      t3ax->addLabelRot(label.chars(), fm, SoAsciiText::CENTER, rot);
    }
  }

  // ticks
  fm = off;
  to = off;
  fm.x = off.x - (.5f * GraphTableView::tick_size);
  to.x = off.x + (.5f * GraphTableView::tick_size);

  float y_lab_off = (.5f * GraphTableView::tick_size + TICK_OFFSET + t3ax->fontSize());

  DataCol* da = GetDAPtr();

  int i;
  float val;
  String label;
  for (i = 0, val = start_tick; i < act_n_ticks; val += tick_incr, ++i) {
    fm.z = off.z + DataToPlot(val);
    to.z = off.z + DataToPlot(val);
    t3ax->addLine(fm, to);
    if(!ticks_only) {
      float lab_val = val / units;
      if (fabsf(val / tick_incr) < range_zero_label_range)
        lab_val = 0.0f;         // the 0 can be screwy
      label = String(lab_val);
      if(use_str_labels && da) {
        int rnum = (int)lab_val;// lab_val is row number!
        if(rnum >= 0 && rnum < da->rows())
          label = da->GetValAsString(rnum);
      }
      if(label.nonempty()) {
        t3ax->addLabel(label.chars(),
                       iVec3f(fm.x - TICK_OFFSET, fm.y - y_lab_off, fm.z));
      }
    }
  }
}
