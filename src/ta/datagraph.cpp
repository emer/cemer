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



#include "datagraph.h"

//////////////////////////////////
// 	DA Graph View Specs	//
//////////////////////////////////

void GraphLineSpec::Initialize() {
  line_type = LINE;
  line_style = LineStyle_MIN;
  line_width = 0.0f;
  point_style = PointStyle_MIN;
  negative_draw = false;

  thresh = .5f;
  string_coords = NULL;
}

void GraphLineSpec::Destroy() {
  CutLinks();
}

void GraphLineSpec::InitLinks(){
  inherited::InitLinks();
//nuy  taBase::Own(range, this);
  taBase::Own(line_color, this);
/*nuke  if (taMisc::gui_active) {
    ta_color.SetColor(line_color.r,line_color.g,line_color.b,line_color.a);
  } */
//nuy  taBase::Own(point_mod, this);

/*obs  // if it is in a subgroup then it will have 3 layers of DT_ViewSpec
  // above it. 3 becuase the first DT_ViewSpec is actual in a gp.
  // if it is in a subgroup then defaul to haveing the first elements graph
  GraphSpec* myowner = GET_MY_OWNER(GraphSpec);
  if(myowner != NULL) myowner = GET_OWNER(myowner,GraphSpec);
  if(myowner != NULL) myowner = GET_OWNER(myowner,GraphSpec);
  if(myowner != NULL) {
    myowner = GET_MY_OWNER(DT_ViewSpec);
    //TODO(3.5): verify this! may not be equivalent to v32 which had set to the first col
    SetAxis(myowner->axes.SafeEl(0));
  } */
}

void GraphLineSpec::CutLinks(){
  taBase::DelPointer((TAPtr*)&string_coords);
//nuy  point_mod.CutLinks();
  line_color.CutLinks();
//nuy  range.CutLinks();
  inherited::CutLinks();
}

void GraphLineSpec::Copy_(const GraphLineSpec& cp){
//nuy  range = cp.range;
  line_color = cp.line_color;
  line_type = cp.line_type;
  line_style = cp.line_style;
  point_style = cp.point_style;
//nuy  point_mod = cp.point_mod;
  negative_draw = cp.negative_draw;
  thresh = cp.thresh;

/*TODO(3.5)
  if((cp.string_coords != NULL) && (owner != NULL)) {
    if(cp.string_coords->owner == owner) {
      SetStringCoords(cp.string_coords);
    }
    else {
      GraphLineSpec* ax = (GraphLineSpec*)((GraphSpec*)owner)->FindName(cp.string_coords->name);
      if(ax != NULL) {
	SetStringCoords(ax);
      }
    }
  } */
}

void GraphLineSpec::UpdateAfterEdit() {
  GraphSpec* graph_spec = graphSpec();
  if (graph_spec && !graph_spec->isUpdatingChildren())
    graph_spec->UpdateAfterEdit_ColSpec(this);
//TODO(3.5): many consistency and update checks



/*TODO(3.5)  if (axis_spec == NULL)		// don't like null axis specs..
    SetAxis(this);
  if(axis_spec != this) {
    if(axis_spec->axis_spec != axis_spec) {
      taMisc::Error("DA_GraphViewsSpec: can't have axis_spec with its own axis_spec != itself -- this is now fixed for axis spec:", axis_spec->name);
      axis_spec->SetAxis(axis_spec);
    }
  } */
  if((data_col) && data_col->InheritsFrom(TA_String_Data)) {
    line_type = STRINGS;
    if (!string_coords) FindStringCoords();
  }
/*TODO(3.5): transplant to axis spec
  if((vertical == NO_VERTICAL) && !((line_type == VALUE_COLORS) || (line_type == THRESH_POINTS))) {
    taMisc::Warning("*** GraphViewSpec:: vertical = NO_VERTICAL only sensible with line_type = COLORS or THRESH_POINTS, resetting to FULL_VERTICAL");
    vertical = FULL_VERTICAL;
  }
  if((vertical == STACK_TRACES) && ((trace_incr.x != 0.0f) || (trace_incr.y != 0.0f))) {
    taMisc::Warning("*** GraphViewSpec:: vertical = STACK_TRACES cannot be combined with trace_incr != 0, resetting trace_incr = 0");
    trace_incr.x = 0.0f; trace_incr.y = 0.0f;
  } */
  inherited::UpdateAfterEdit();
}

const iColor* GraphLineSpec::def_color() const {
  return line_color.color();
}

void GraphLineSpec::FindStringCoords() {
  // note: use my group, not the root group
  GraphSpec* myowner = GET_MY_OWNER(GraphSpec);
  if (!myowner) return; // shouldn't happen

  int idx = -1;
  // first check if we have an explicit col indicated -- if so, use that col
  String col = data_col->DispOptionAfter(" STRING_COORDS=");
  if (col.isInt()) {
    int idx = (int)col;
    if (idx < myowner->size)
      idx = -1;
  }


  if (idx < 0) { // initiate search from previous column if not explicitly marked
    idx = myowner->FindEl(this) - 1;
  }
  GraphLineSpec* prv_float = NULL;
  while ((prv_float == NULL) && (idx >= 0)) {
    prv_float = (GraphLineSpec*)myowner->FastEl(idx--);
    if (!prv_float->data_col->InheritsFrom(TA_float_Data))
      prv_float = NULL;
  }
  if(prv_float == NULL) {
    taMisc::Error("GraphViewSpec: Could not find number data for plotting string data:",
		  name);
    return;
  }
  SetStringCoords(prv_float);
}

bool GraphLineSpec::is_string() const {
  return (line_type == STRINGS);
}

void GraphLineSpec::GpShareAxis() {
  GraphSpec* myowner = GET_MY_OWNER(GraphSpec);
  if(myowner == NULL) return;
  myowner->ShareAxes();
}

void GraphLineSpec::GpSepAxes() {
  GraphSpec* myowner = GET_MY_OWNER(GraphSpec);
  if(myowner == NULL) return;
  myowner->SeparateAxes();
}


//////////////////////////
// 	AxisSpec	//
//////////////////////////

iColor AxisSpec::m_def_color; // black, for x and z
MinMax AxisSpec::temp_range;     // #IGNORE scratch pad

void AxisSpec::Initialize() {
  n_ticks = 10;

  spec = NULL; //set later
  spec_cnt = 0;
//  field = -1;
//  is_string = false;

  InitData();
}

void AxisSpec::Destroy() {
  CutLinks();
  spec = NULL;
}

void AxisSpec::InitLinks() {
  inherited::InitLinks();
  taBase::Own(range, this);
  taBase::Own(true_range, this);
  taBase::Own(eff_range, this);
  taBase::Own(fixed_range, this);
}

void AxisSpec::CutLinks() {
  fixed_range.CutLinks();
  eff_range.CutLinks();
  true_range.CutLinks();
  range.CutLinks();
  inherited::CutLinks();
}

//TODO: axes and specs copy of a whole object will be chicken/egg -- must fix up later
void AxisSpec::Copy_(const AxisSpec& cp) {
  range = cp.range;
  true_range = cp.true_range;
  eff_range = cp.eff_range;
  fixed_range = cp.fixed_range;
  n_ticks = cp.n_ticks;
}

void AxisSpec::UpdateAfterEdit() {
  GraphSpec* graph_spec = graphSpec();
  if (graph_spec && !graph_spec->isUpdatingChildren())
    graph_spec->UpdateAfterEdit_AxisSpec(this);
  inherited::UpdateAfterEdit();
}

const iColor* AxisSpec::def_color() const {
  return &m_def_color;
}

void AxisSpec::InitData() {
  //NOTE: this is called from Graph::Configure, so make sure not to add/remove fields from
  // this routine without determining the effect -- this routine is not a general-purpose
  // initializer

  float_Data* da = NULL;
  if ((spec != NULL) && (spec->data_col) && (spec->data_col->InheritsFrom(&TA_float_Data))) {
    da = (float_Data*)spec->data_col;
  }
  if (da != NULL)  temp_range.SetRange(da->ar);
  else temp_range.Init(0.0f);
  InitRange(temp_range.min, temp_range.max);
}

void AxisSpec::InitRange(float first, float last) {
  true_range.Set(first, last);
  fixed_range.FixRange(true_range); // keep range fixed!
  range = true_range;
  eff_range = range;
}

// limits are needed to prevent numerical overflow!
static const float range_min_limit = 1.0e-6f;
static const float range_zero_range = 5.0e-5f; // half-range for zero-range values

// updates range based on new data and returns true if it really is a new range
bool AxisSpec::UpdateRange(float first, float last) {
  true_range.UpdateRange(first);
  true_range.UpdateRange(last);

  if(((first >= range.min) && (last <= range.max)) && (range.Range() >= range_min_limit)) { // special case
//     cerr << "not changing range: " << first << ", " << last << " rng: " << range.min << ", " << range.max << endl;
    eff_range = range;
    return false;			// not changed
  }

  first = true_range.min;
  last = true_range.max;

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


//////////////////////////////////
// 	XAxisSpec		//
//////////////////////////////////

void XAxisSpec::Initialize() {
  axis_type = COL_VALUE;
}

void XAxisSpec::Copy_(const XAxisSpec& cp) {
  axis_type = cp.axis_type;
}

void XAxisSpec::Config(GraphLineSpec* spec_) {
  inherited::Config(spec_);
  if (spec_) {
    if (spec_->col_type == GraphLineSpec::X_AXIS)
      axis_type = COL_VALUE;
  }
}

bool XAxisSpec::InitUpdateAxis(bool init) {
  float first = 0.0f;
  float last = 0.0f;
  GraphSpec* graph_spec = graphSpec(); // cache
  switch (axis_type) {
  case COL_VALUE:
    if (spec->data_col->InheritsFrom(&TA_float_Data)) {
      float_Data* da = (float_Data*)spec->data_col;
      if (da != NULL)  {
        temp_range.SetRange(da->ar);
        first = temp_range.min;
        last = temp_range.max;
      }
    }
    break;
  case ROW_NUM:
    first = graph_spec->view_range.min;
    last = graph_spec->view_range.max;
    break;
  }
  if (init) {
    InitRange(first, last);
    return true;
  } else {
    return UpdateRange(first, last);
  }
}


//////////////////////////////////
// 	YAxisSpec		//
//////////////////////////////////

void YAxisSpec::Initialize() {
  no_vertical = false;
}

void YAxisSpec::Copy_(const YAxisSpec& cp) {
  no_vertical = cp.no_vertical;
}

const iColor* YAxisSpec::def_color() const {
  if (spec) return spec->def_color();
  else return &m_def_color;
}

bool YAxisSpec::InitUpdateAxis(bool init) {
  float first = 0.0f;
  float last = 0.0f;

  if (!no_vertical && spec->data_col->InheritsFrom(&TA_float_Data)) { //note: should always be true
    float_Data* da = (float_Data*)spec->data_col;
    if (da != NULL)  {
      temp_range.SetRange(da->ar);
      first = temp_range.min;
      last = temp_range.max;
    }
  }
  if (init) {
    InitRange(first, last);
    return true;
  } else {
    return UpdateRange(first, last);
  }
}


//////////////////////////////////
// 	ZAxisSpec		//
//////////////////////////////////

void ZAxisSpec::Initialize() {
  n_traces = 0;
}

void ZAxisSpec::Copy_(const ZAxisSpec& cp) {
  n_traces = cp.n_traces;
}

bool ZAxisSpec::InitUpdateAxis(bool init) {
  float first = 0.0f;
  float last = 0.0f;
  switch (graphSpec()->graph_type) {
  case GraphSpec::TWOD: return false;
  case GraphSpec::THREED:
    if (spec->data_col->InheritsFrom(&TA_float_Data)) {
      float_Data* da = (float_Data*)spec->data_col;
      if (da != NULL)  {
        temp_range.SetRange(da->ar);
        first = temp_range.min;
        last = temp_range.max;
      }
    }
    break;
  //NOTE; for TRACES, the n_traces needs to have been determined in spec
  // this is usually done on rebuild_axes for LINES, and in the REDRAW for TRACES
  case GraphSpec::STACK_TRACES:
  case GraphSpec::STACK_LINES:
    last = n_traces;
    break;
  }
  if (init) {
    InitRange(first, last);
    return true;
  } else {
    return UpdateRange(first, last);
  }
}



//////////////////////////////////
// 	AxisSpec_List	//
//////////////////////////////////

YAxisSpec* YAxisSpec_List::FindBySpec(const GraphLineSpec* spec, int* idx) {
  for (int i = 0; i < size; ++i) {
    YAxisSpec* rval = FastEl(i);
    if (rval->spec == spec) {
      if (idx) *idx = i;
      return rval;
    }
  }
  if (idx) *idx = -1;
  return NULL;
}



//////////////////////////////////
// 	DG_GraphletSpec	//
//////////////////////////////////


//////////////////////////////////
// 	GraphSpec	//
//////////////////////////////////

void GraphSpec::Initialize() {
  graph_type = TWOD;
  background.name = "grey54";
  background.r = .541176f;
  background.g = .541176f;
  background.b = .541176f;
  color_type = C_RAINBOW;
  use_cbar = false;
  sequence_1 = COLORS;
  sequence_2 = LINES;
  sequence_3 = POINTS;

  // was in GraphEditor in 3.2
  last_col_cnt = 0;
  last_row_cnt = -2;
  view_bufsz = 10000;
  view_range.max = 10000;
  last_pt_offset = 0;

  scale = new ColorScale();
  axis_sharing = SHARED_AXES;
  graph_layout.x = 1;
  graph_layout.y = 10;
//nn?  n_graphs = 0;
  updating_children = 0;
}

void GraphSpec::Destroy(){
  CutLinks();
}

void GraphSpec::InitLinks() {
  inherited::InitLinks();
  taBase::Own(background, this);
  taBase::Own(y_axes, this);
  taBase::Own(x_axis_spec, this);
  taBase::Own(z_axis_spec, this);
  taBase::Own(line_specs, this);
  taBase::Own(graphlets, this);

  if(taMisc::gui_active) {
    bg_color.SetColor(background.r,background.g,background.b,background.a);
  }
  if (scale) {taBase::Own(scale, this);}
}

void GraphSpec::CutLinks() {
  if (scale) {scale->CutLinks(); scale = NULL;}
  line_specs.CutLinks();
  z_axis_spec.CutLinks();
  x_axis_spec.CutLinks();
  graphlets.CutLinks();
  y_axes.CutLinks();
  background.CutLinks();
  inherited::CutLinks();
}

void GraphSpec::Copy_(const GraphSpec& cp) {
  graph_type = cp.graph_type;
  background = cp.background;
  bg_color = cp.bg_color;
  color_type = cp.color_type;
  use_cbar = cp.use_cbar;
  sequence_1 = cp.sequence_1;
  sequence_2 = cp.sequence_2;
  sequence_3 = cp.sequence_3;
  axis_sharing = cp.axis_sharing;

  x_axis_spec = cp.x_axis_spec;
  //fixup for our own column -- looks up ordinally, but since we are copying, should work
  if (cp.x_axis_spec.spec) {
    int idx = cp.FindLeaf(cp.x_axis_spec.spec);
    if (idx >= 0)
      x_axis_spec.spec = (GraphLineSpec*)Leaf(idx);
  }

  z_axis_spec = cp.z_axis_spec;
  // fixup, as for x
  if (cp.z_axis_spec.spec) {
    int idx = cp.FindLeaf(cp.z_axis_spec.spec);
    if (idx >= 0)
      z_axis_spec.spec = (GraphLineSpec*)Leaf(idx);
  }

  y_axes = cp.y_axes;
  // have to set spec pointers in y_axes to our own copies
  for (int i = 0; i < cp.y_axes.size; ++i) {
    AxisSpec* cp_as = cp.y_axes.FastEl(i);
    GraphLineSpec* cp_gvs = cp_as->spec;
    if (cp_gvs) {
      GraphLineSpec* gvs = (GraphLineSpec*)FindLeafName(cp_as->name);
      AxisSpec* as = ((GraphSpec*)root_gp)->y_axes.SafeEl(i);
      if (as && gvs)
        as->spec = gvs;
    }
  }
  line_specs = cp.line_specs;
  graph_layout = cp.graph_layout;
  // we need to find our own copies of graphlets
  graphlets.EnforceSize(cp.graphlets.size);
  for (int i = 0; i < cp.graphlets.size; ++i) {
    GraphletSpec* cp_asl = cp.graphlets.FastEl(i);
    GraphletSpec* asl = graphlets.SafeEl(i);
    if (!asl) break; // shouldn't happen
    asl->Reset();
    for (int j = 0; j < cp_asl->size; ++j) {
      AxisSpec* cp_as = cp_asl->FastEl(j);
      AxisSpec* as = y_axes.FindName(cp_as->name);
      if (as) { // very bad if not found
        asl->Link(as);
      }
    }
  }

  // things from 3.2 GraphEditor
  last_col_cnt = cp.last_col_cnt;
  last_row_cnt = cp.last_row_cnt;
  view_bufsz = cp.view_bufsz;
  view_range = cp.view_range;
  last_pt_offset = cp.last_pt_offset;

  if (cp.scale) {
    (*scale) = *(cp.scale);
  }
}

void GraphSpec::UpdateAfterEdit() {
  if(taMisc::gui_active) {
    bg_color.SetColor(background.r,background.g,background.b,background.a);
  }
  inherited::UpdateAfterEdit();
}

void GraphSpec::UpdateAfterEdit_ColSpec(GraphLineSpec* cs) {
  if (!cs) return;
  ++updating_children;
  // if cs specifies a Z axis, then change graph to 3D
  if ((cs->col_type == GraphLineSpec::Z_AXIS) && (graph_type == TWOD)) {
    graph_type = THREED;
  }

  taLeafItr itr;
  GraphLineSpec* csi;
  FOR_ITR_EL(GraphLineSpec, csi, this->, itr) {
    if (csi == cs) continue;
    // if cs specifies an x or z axis, remove that from any others
    if ((csi->col_type == cs->col_type)
      && ((cs->col_type == GraphLineSpec::X_AXIS) || (cs->col_type == GraphLineSpec::Z_AXIS)))
    {
      csi->col_type = GraphLineSpec::AUTO;
    }
  }
  --updating_children;
  UpdateAfterEdit();
}

void GraphSpec::UpdateAfterEdit_AxisSpec(AxisSpec* as) {
  ++updating_children;
  //note: nothing here yet
  --updating_children;
  UpdateAfterEdit();
}

static char* c_rainbow_colors[] = {
  "red", "orange", "yellow", "green", "blue", "navy", "violet",
  "brown", "black", "white", "pink", "pale green", "light blue" };
static int c_rainbow_count = 13;

static char* c_greyscale_colors[] = {
  "grey0", "grey10", "grey20", "grey30", "grey40", "grey60", "grey70", "grey80",
  "grey90", "grey100"};
static int c_greyscale_count = 10;

static char* m_mono_colors[] = { "black", "white" };
static int m_mono_count = 2;

static char* p_rainbow_colors[] = {
  "red", "orange", "yellow", "green", "blue", "navy", "violet",
  "brown", "black", "grey50", "pink", "pale green", "light blue" };
static int p_rainbow_count = 13;

static char* p_greyscale_colors[] = {
  "grey0", "grey10", "grey20", "grey30", "grey40", "grey50", "grey60", "grey70",
  "grey80", "grey90"};
static int p_greyscale_count = 10;

void GraphSpec::ApplyOneFeature(GraphLineSpec* dagv, SequenceType seq,
				       int val, bool& set_ln, bool& set_pt) {
  if (seq == COLORS) {
    dagv->line_color.name = ColorName(val);
    dagv->line_color.UpdateAfterEdit();
  } else if(seq == LINES) {
    dagv->line_style = (GraphLineSpec::LineStyle)val;
    set_ln = true;
  } else if ((seq == POINTS) && (val != GraphLineSpec::PointStyle_NONE)) {
    dagv->point_style = (GraphLineSpec::PointStyle)val;
    set_pt = true;
  }
}

static char* p_mono_colors[] = { "black" };
static int p_mono_count = 1;

char* GraphSpec::ColorName(int color_no) {
  switch(color_type) {
  case C_RAINBOW:
    return c_rainbow_colors[color_no % c_rainbow_count];
  case C_GREYSCALE:
    return c_greyscale_colors[color_no % c_greyscale_count];
  case M_MONO:
    return m_mono_colors[color_no % m_mono_count];
  case P_RAINBOW:
    return p_rainbow_colors[color_no % p_rainbow_count];
  case P_GREYSCALE:
    return p_greyscale_colors[color_no % p_greyscale_count];
  case P_MONO:
    return p_mono_colors[color_no % p_mono_count];
  case CUSTOM:
    return "";
  }
  return "";
}

int GraphSpec::ColorCount() {
  switch(color_type) {
  case C_RAINBOW:
    return c_rainbow_count;
  case C_GREYSCALE:
    return c_greyscale_count;
  case M_MONO:
    return m_mono_count;
  case P_RAINBOW:
    return p_rainbow_count;
  case P_GREYSCALE:
    return p_greyscale_count;
  case P_MONO:
    return p_mono_count;
  case CUSTOM:
    return 0;
  }
  return 0;
}

void GraphSpec::CustomAxes() {
  axis_sharing = CUSTOM_AXES;
  //TODO(3.5): need to call up a UI dialog to let user share axes
  UpdateAfterEdit();
}

YAxisSpec* GraphSpec::FindYAxis(GraphLineSpec* spec) {
  YAxisSpec* rval = y_axes.FindBySpec(spec);
  return rval;
}

YAxisSpec* GraphSpec::FindMakeYAxis(GraphLineSpec* spec) {
  YAxisSpec* rval = y_axes.FindBySpec(spec);
  if (rval) {
    rval->Config(spec);
  } else {
    rval = MakeYAxis(spec);
  }
  return rval;
}

YAxisSpec* GraphSpec::MakeYAxis(GraphLineSpec* spec) {
  YAxisSpec* rval = (YAxisSpec*)y_axes.New(1);
  rval->Config(spec);
  // stamp explicitly in spec
  spec->col_type = GraphLineSpec::Y_AXIS;
  return rval;
}

void GraphSpec::AssertGraphlets() {
  switch (axis_sharing) {
  case SHARED_AXES: {
    // make sure only one list -- easiest is to just rebuild list, since the
    // views are not directly tied to this list
    graphlets.EnforceSize(1);
    GraphletSpec* gls = graphlets.FastEl(0);
    graph_layout = 1;
    int lnk_cnt = 0; // count of linked axes
    gls->Reset();
    for (int i = 0; i < y_axes.size; ++i) {
      AxisSpec* as = y_axes.FastEl(i);
      gls->Link(as);
      ++lnk_cnt;
    }
  } break;
  case SEPARATE_AXES: {
    // assign each y-axis to its own list
    int g_cnt = 0;
    for (int i = 0; i < y_axes.size; ++i) {
      /*TODO(3.5) revise
      AxisSpec* as = y_axes.FastEl(i);
      GraphLineSpec* gvs = as->spec;
      // put all but x&z y_axes in list
      if (gvs && gvs->visible &&
        !((gvs->col_type == GraphLineSpec::X_AXIS) ||
          (gvs->col_type == GraphLineSpec::Z_AXIS) ||
          (gvs->col_type == GraphLineSpec::HIDE))
      ) {
        graphlets.EnforceSize(g_cnt + 1);
        GraphletSpec* gls = graphlets.FastEl(g_cnt);
        gls->Reset();
        gls->Link(as);
        ++g_cnt;
      }*/
    }
  } break;
  case CUSTOM_AXES: {
    //TODO(3.5): need to make sure any new y_axes are assigned to a sensible
    // existing graphlet list
  } break;
  }

}

void GraphSpec::ReBuildAxes() {
  ++updating_children;

  // note: we look for Z first, assuming that leftmost col is one most
  // likely to be the desired Z axis (ex. Batch #)
  // we only force creation of a Z axis if not marked "NONE"
  if (graph_type == THREED) {
    ReBuildAxes_GetXZAxis(AxisSpec::Z, z_axis_spec, true);
  }

  ReBuildAxes_GetXZAxis(AxisSpec::X, x_axis_spec);

//TODO(3.5): need to do the SetOwner thingy
//      if (SetOwnerXAxis != NULL) SetOwnerXAxis(owner, x_axis_index);


  // reset everything and make sure every non-x/z has a y-axis spec
  // this will handle case of 1st time, cols added, or going from shared to non-shared col
  // also, check if we need a color bar
  use_cbar = false;

  // we loop through each group, doing the following in each group, in order:
  // * make any explicit Y AXIS's
  // * if no explicit one has been made, make first implicit one
  // * assign lines to axes, as follows:
  //    * Y AXIS is itself
  //    * AXIS=i directive gets linked to that line/axis (if exists)
  //    * AUTO with no other rule becomes its own Y axis
  taGroupItr itr;
  GraphSpec* gp;
  GraphLineSpec* spec;
  DataArray_impl* da;
  FOR_ITR_GP(GraphSpec, gp, this->, itr) {
    int y_ax_cnt = 0; // axes made this group
    int i;
    // explicit y axes
    for (i = 0; i < gp->size; ++i) {
      spec = (GraphLineSpec*)gp->FastEl(i);
      da = spec->data_col;
      if ((spec->col_type == GraphLineSpec::Y_AXIS) && (!da->is_string())) {
        MakeYAxis(spec);
        ++y_ax_cnt;
      }

    }

    // make one implicit master y axis per group, if no explicit one yet
    for (i = 0; ((y_ax_cnt == 0) && (i < gp->size)); ++i) {
      spec = (GraphLineSpec*)gp->FastEl(i);
      da = spec->data_col;
      if ( ((spec->col_type == GraphLineSpec::Y_DATA) || (spec->col_type == GraphLineSpec::AUTO))
        &&  (!da->is_string())
      ) {
        MakeYAxis(spec);
        ++y_ax_cnt;
      }
    }

    // now, link all potential Y DATA or indexed lines to an axis, plus additional tasks
    for (i = 0; (i < gp->size); ++i) {
      spec = (GraphLineSpec*)gp->FastEl(i);
      da = spec->data_col;
      if ( ((spec->col_type == GraphLineSpec::Y_DATA) || (spec->col_type == GraphLineSpec::AUTO))
        &&  (!da->is_string())
      ) {
        // if it has an explicit axis indicator, link to that
        String axis = da->DispOptionAfter(" AXIS=");
        if (axis.isInt()) {
          int ax_idx = (int)axis;
          if (ax_idx < gp->size) {
            GraphLineSpec* ax_spec = (GraphLineSpec*)gp->FastEl(ax_idx);
            if ((ax_spec->axis_spec && (ax_spec->axis_spec->axis() == AxisSpec::Y)))
              spec->SetAxis(ax_spec->axis_spec);
           }
        }
        // if still no axis, but is visible (indicating plotable data), we just make its own axis for it
        if (!spec->axis_spec && spec->visible) {
          MakeYAxis(spec);
          ++y_ax_cnt;
        }
      }



      if (spec->visible && ((spec->line_type == GraphLineSpec::VALUE_COLORS) ||
        (spec->line_type == GraphLineSpec::TRACE_COLORS)) )
        use_cbar = true;
    }

    // string data gets linked to the axis of its coords
    for (i = 0; (i < gp->size); ++i) {
      spec = (GraphLineSpec*)gp->FastEl(i);
      if ( (spec->visible) &&  (da->is_string()) ) {
        if (spec->string_coords && spec->string_coords->axis_spec) {
          spec->SetAxis(spec->string_coords->axis_spec);
        }
      }
    }
  }

  // delete orphaned y_axes (no longer has a spec)
  for (int i = y_axes.size - 1; i >= 0; --i) {
    AxisSpec* as = y_axes.FastEl(i);
    if (as->spec == NULL)
      y_axes.Remove(i);
  }

  //TODO(3.5): for STACK_LINES mode, need to have counted number of separate lines that
  // will be plotted on Z, and set in z.n_traces
  int tot_lines = 0; // total number of lines that will be plotted, for STACK_LINES
  if (graph_type == STACK_LINES) {
    taLeafItr jtr;
    GraphLineSpec* spec;
    FOR_ITR_EL(GraphLineSpec, spec, this->, jtr) {
      if (spec->visible && spec->is_line()) ++tot_lines;
    }

    // initialize the Z axis with the result
    z_axis_spec.InitRange(0, tot_lines - 1);

  }
use_cbar = true; //TEMP
  AssertGraphlets();
  --updating_children;
}

bool GraphSpec::ReBuildAxes_GetXZAxis(AxisSpec::Axis axis,
  AxisSpec& axis, bool force) 
{//TODO(3.5): fixup
  bool rval = false;
/*TODO  AxisSpec* tmp_axis = NULL;

  // first, check if a col is already marked as X/Z -- use it
  GraphLineSpec* spec;
  taLeafItr itr;
  FOR_ITR_EL(GraphLineSpec, spec, this->, itr) {
    if (spec->col_type == col_type) {
      axis.Config(spec);
      spec->visible = false; // axes not visible as lines
      //TODO(3.5): if found, we should probably nix out any subsequent Z_AXIS
      rval = true;
      goto exit;
    }
  }

  if (!force) goto exit;

  // none explicitly marked yet, so iterate all columns to find first candidate
  // criteria: float data, still marked AUTO, not shared
  FOR_ITR_EL(GraphLineSpec, spec, this->, itr) {
    DataArray_impl* xaa = spec->data_col;
    if ((xaa == NULL) || (xaa->is_string()))
      continue;
    if (spec->col_type == GraphLineSpec::AUTO) {
      tmp_axis = FindYAxis(spec);
      if (tmp_axis) {
        // if it already existed, may have been shared
        if (tmp_axis->spec_cnt > 1)
          continue;
        // release that as a Y axis
        tmp_axis->Close();
      }
      spec->col_type = col_type; // always stamp for axes
      axis.Config(spec);
      spec->visible = false; // axes not visible as lines
      rval = true;
      goto exit;
    }
  }

exit:
  if (rval && axis.spec) {
    axis.spec->visible = false;
  }*/
  return rval;
}

void GraphSpec::ReBuildFromDataTable() {
  ++updating_children;
  inherited::ReBuildFromDataTable();
  UpdateLineFeatures();
  ReBuildAxes();
  --updating_children;
}

void GraphSpec::SetBgColor() {
  switch(color_type) {
  case C_RAINBOW:
  case C_GREYSCALE:
    background.name = "grey54";
    background.UpdateAfterEdit();
    break;
  case M_MONO:
    background.name = "white";
    background.UpdateAfterEdit();
    background.a = 1;
    break;
  case P_RAINBOW:
  case P_GREYSCALE:
  case P_MONO:
    background.name = "white";
    background.UpdateAfterEdit();
    break;
  case CUSTOM:
    break;
  }
  if(taMisc::gui_active)
    bg_color.SetColor(background.r,background.g,background.b,background.a);
}


void GraphSpec::SetLineType(GraphLineSpec::LineType line_type) {
  if(line_type == GraphLineSpec::STRINGS) return;
  int i;
  for(i=0;i<size;i++) {
    GraphLineSpec* vs = (GraphLineSpec*)FastEl(i);
    if(vs->line_type != GraphLineSpec::STRINGS)
      vs->line_type = line_type;
  }
  for(i=0;i<gp.size;i++) {
    GraphSpec* nvs = (GraphSpec*)FastGp(i);
    nvs->SetLineType(line_type);
  }
  UpdateAfterEdit();
}

void GraphSpec::SetLineWidths(float line_width) {
  int i;
  for(i=0;i<size;i++) {
    GraphLineSpec* vs = (GraphLineSpec*)FastEl(i);
    vs->line_width = line_width;
  }
  for(i=0;i<gp.size;i++) {
    GraphSpec* nvs = (GraphSpec*)FastGp(i);
    nvs->SetLineWidths(line_width);
  }
  UpdateAfterEdit();
}

void GraphSpec::ShareAxes() {
  axis_sharing = SHARED_AXES;
  UpdateAfterEdit();
}

void GraphSpec::SeparateAxes() {
  axis_sharing = SEPARATE_AXES;
  UpdateAfterEdit();
}

void GraphSpec::UpdateLineFeatures(bool visible_only) {
  UpdateLineFeatures_impl(visible_only);
  UpdateAfterEdit();
}

void GraphSpec::UpdateLineFeatures_impl(bool visible_only) {
  int counts[4];
  counts[COLORS] = ColorCount();
  counts[LINES] = (GraphLineSpec::LineStyle_MAX - GraphLineSpec::LineStyle_MIN) + 1;
  counts[POINTS] = (GraphLineSpec::PointStyle_MAX - GraphLineSpec::PointStyle_NONE) + 1;
  counts[NONE] = 100000;	// should cover it..

  SetBgColor();

  //NOTE: for points, the first value is NONE, so that is how we mix no points with points
  int i=0;
  int cnt = 0;
  while(true) {
    int j = 0;
    while(true) {
      int k = 0;
      while(true) {
	GraphLineSpec* dagv = (GraphLineSpec*)Leaf(cnt++);
	if(dagv == NULL)
	  return;

	if((visible_only == true) && !dagv->visible) { // turn off
	  dagv->line_color.name = "black";
	  dagv->line_color.UpdateAfterEdit();
	  dagv->line_style = GraphLineSpec::LineStyle_MIN;
	  dagv->point_style = GraphLineSpec::PointStyle_MIN;
	  dagv->line_type = GraphLineSpec::LINE;
	  dagv->UpdateAfterEdit();
	  continue;
	}

	// initialize to default values first
	dagv->line_style = GraphLineSpec::LineStyle_MIN;
	dagv->point_style = GraphLineSpec::PointStyle_MIN;
	bool set_ln = false;
	bool set_pt = false;
	if (sequence_1 != NONE)
          ApplyOneFeature(dagv, sequence_1, k, set_ln, set_pt);
	if (sequence_2 != NONE)
	  ApplyOneFeature(dagv, sequence_2, j, set_ln, set_pt);
	if (sequence_3 != NONE)
	  ApplyOneFeature(dagv, sequence_3, i, set_ln, set_pt);

	if (set_ln && set_pt)
	  dagv->line_type = GraphLineSpec::LINE_AND_POINTS;
	else if(set_pt) {
	  dagv->line_type = GraphLineSpec::POINTS;
	} else	// default is line
	  dagv->line_type = GraphLineSpec::LINE;
	k++;
	if(k >= counts[sequence_1])			break;
      }
      j++;
      if(j >= counts[sequence_2])			break;
    }
    i++;
    if(i >= counts[sequence_3])				break;
  }
}

void GraphSpec::StackTraces() {
  graph_type = STACK_TRACES;
/*obs  for (int i=0; i< y_axes.size; i++) {
    YAxisSpec* vs = y_axes.FastEl(i);
    vs->vertical = YAxisSpec::STACK_TRACES;
  } */
  UpdateAfterEdit();
}

void GraphSpec::UnStackTraces() {
  graph_type = TWOD;
/*obs  for (int i=0; i< y_axes.size; i++) {
    YAxisSpec* vs = y_axes.FastEl(i);
    vs->vertical = YAxisSpec::FULL_VERTICAL;
  } */
  UpdateAfterEdit();
}

void GraphSpec::StackSharedAxes() {
  graph_type = STACK_LINES;
/*obs  for (int i=0; i< y_axes.size; i++) {
    YAxisSpec* vs = y_axes.FastEl(i);
    vs->shared_y = YAxisSpec::STACK_LINES;
  } */
  UpdateAfterEdit();
}

void GraphSpec::UnStackSharedAxes() {
  graph_type = TWOD;
/*obs  for (int i=0; i< y_axes.size; i++) {
    YAxisSpec* vs = y_axes.FastEl(i);
    vs->shared_y = YAxisSpec::OVERLAY_LINES;
  } */
  UpdateAfterEdit();
}

