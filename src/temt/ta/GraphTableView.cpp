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

#include "GraphTableView.h"
#include <GraphColView>
#include <DataTable>
#include <iGraphTableView_Panel>
#include <taDataLinkItr>
#include <T3DataViewFrame>
#include <MainWindowViewer>
#include <NewNetViewHelper>
#include <T3ExaminerViewer>
#include <T3GraphLine>
#include <T3GraphViewNode>
#include <iT3DataViewFrame>
#include <T3Axis>
#include <T3Color>
#include <SoLineBox3d>
#include <taVector2i>

#include <SoScrollBar>
#include <SoImageEx>

#include <Inventor/SbLinear.h>
#include <Inventor/fields/SoMFString.h>
#include <Inventor/nodes/SoAsciiText.h>
#include <Inventor/nodes/SoBaseColor.h>
#include <Inventor/nodes/SoCube.h>
#include <Inventor/nodes/SoDirectionalLight.h>
#include <Inventor/nodes/SoFont.h>
#include <Inventor/nodes/SoLightModel.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoPerspectiveCamera.h>
#include <Inventor/nodes/SoSelection.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/nodes/SoTranslation.h>
#include <Inventor/nodes/SoComplexity.h>
#include <Inventor/nodes/SoText2.h>
#include <Inventor/draggers/SoTransformBoxDragger.h>
#include <Inventor/nodes/SoEventCallback.h>
#include <Inventor/events/SoMouseButtonEvent.h>
#include <Inventor/actions/SoRayPickAction.h>
#include <Inventor/SoPickedPoint.h>
#include <Inventor/SoEventManager.h>

// #define UNIT_LEGEND_OFFSET 0.04f // space between end of axis and unit legend text
#define TICK_OFFSET 0.01f // gap between tick and label

float GraphTableView::tick_size = 0.05f;


GraphTableView* DataTable::NewGraphView(T3DataViewFrame* fr) {
  return GraphTableView::New(this, fr);
}

GraphTableView* DataTable::FindMakeGraphView(T3DataViewFrame* fr) {
  taDataLink* dl = data_link();
  if(dl) {
    taDataLinkItr itr;
    GraphTableView* el;
    FOR_DLC_EL_OF_TYPE(GraphTableView, el, dl, itr) {
      // update from user stuff
      el->InitFromUserData();
      el->InitDisplay();
      el->UpdateDisplay();
      fr = el->GetFrame();
      if(fr) {
        MainWindowViewer* mwv = GET_OWNER(fr, MainWindowViewer);
        if(mwv) {
          mwv->SelectT3ViewTabName(fr->name);
        }
      }
      return el;
    }
  }
  return GraphTableView::New(this, fr);
}

GraphTableView* DataTable::FindGraphView() {
  taDataLink* dl = data_link();
  if(dl) {
    taDataLinkItr itr;
    GraphTableView* el;
    FOR_DLC_EL_OF_TYPE(GraphTableView, el, dl, itr) {
      return el;
    }
  }
  return NULL;
}

bool DataTable::GraphViewGotoRow(int row_no) {
  GraphTableView* gv = FindGraphView();
  if(!gv) return false;
  gv->ViewRow_At(row_no);
  return true;
}

// Add a new GraphTableView object to the frame for the given DataTable.
GraphTableView* GraphTableView::New(DataTable* dt, T3DataViewFrame*& fr) {
  NewNetViewHelper new_net_view(fr, dt, "table");
  if (!new_net_view.isValid()) return NULL;

  GraphTableView* vw = new GraphTableView;
  fr->AddView(vw);
  vw->setDataTable(dt);
  // make sure we get it all setup!
  vw->BuildAll();

  new_net_view.showFrame();
  return vw;
}

void GraphTableView::Initialize() {
  view_rows = 10000;

  x_axis.axis = GraphAxisBase::X;
  x_axis.color.name = taMisc::t3d_text_color;
  x_axis.color.UpdateAfterEdit_NoGui(); // needed to pick up color name
  z_axis.axis = GraphAxisBase::Z;
  z_axis.color.name = taMisc::t3d_text_color;
  z_axis.color.UpdateAfterEdit_NoGui(); // needed to pick up color name
  plot_1.color.name = taMisc::t3d_text_color;
  plot_1.point_style = GraphPlotView::CIRCLE;
  plot_1.color.UpdateAfterEdit_NoGui(); // needed to pick up color name
  plot_2.color.name = "red";
  plot_2.point_style = GraphPlotView::SQUARE;
  plot_2.color.UpdateAfterEdit_NoGui();
  plot_3.color.name = "blue";
  plot_3.point_style = GraphPlotView::DIAMOND;
  plot_3.color.UpdateAfterEdit_NoGui();
  plot_4.color.name = "green3";
  plot_4.point_style = GraphPlotView::TRIANGLE;
  plot_4.color.UpdateAfterEdit_NoGui();
  plot_5.color.name = "purple";
  plot_5.point_style = GraphPlotView::PLUS;
  plot_5.color.UpdateAfterEdit_NoGui();
  plot_6.color.name = "orange";
  plot_6.point_style = GraphPlotView::CROSS;
  plot_6.color.UpdateAfterEdit_NoGui();
  plot_7.color.name = "brown";
  plot_7.point_style = GraphPlotView::STAR;
  plot_7.color.UpdateAfterEdit_NoGui();
  plot_8.color.name = "chartreuse";
  plot_8.point_style = GraphPlotView::MINUS;
  plot_8.color.UpdateAfterEdit_NoGui();

  graph_type = XY;
  plot_style = LINE;
  line_width = 2.0f;
  point_size = MEDIUM;
  point_spacing = 1;
  bar_space = .2f;
  color_mode = VALUE_COLOR;
  negative_draw = false;
  negative_draw_z = true;
  axis_font_size = .05f;
  label_font_size = .04f;
  label_spacing = -1;
  matrix_mode = SEP_GRAPHS;
  mat_layout = taMisc::BOT_ZERO;
  mat_odd_vert = true;
  scrolling_ = false;

  err_1.axis = GraphAxisBase::Y;
  err_2.axis = GraphAxisBase::Y;
  err_3.axis = GraphAxisBase::Y;
  err_4.axis = GraphAxisBase::Y;
  err_5.axis = GraphAxisBase::Y;
  err_6.axis = GraphAxisBase::Y;
  err_7.axis = GraphAxisBase::Y;
  err_8.axis = GraphAxisBase::Y;
  err_spacing = 1;
  err_bar_width = .02f;

  color_axis.axis = GraphAxisBase::Y;
  raster_axis.axis = GraphAxisBase::Y;

  thresh = .5f;
  thr_line_len = .48f;

  width = 1.0f;
  depth = 1.0f;

  two_d_font = false;   // true -- this is causing mysterious so crash, disabled for now
  two_d_font_scale = 350.0f;

  last_sel_pt = 0.0f;

  colorscale.auto_scale = false;
  colorscale.min = -1.0f; colorscale.max = 1.0f;

  children.SetBaseType(&TA_GraphColView); // subclasses need to set this to their type!

  n_plots = 1;
  do_matrix_plot = false;
  t3_x_axis = NULL;
  t3_x_axis_top = NULL;
  t3_x_axis_far = NULL;
  t3_x_axis_far_top = NULL;
  t3_y_axis = NULL;
  t3_y_axis_rt = NULL;
  t3_y_axis_far = NULL;
  t3_y_axis_far_rt = NULL;
  t3_z_axis = NULL;
  t3_z_axis_rt = NULL;
  t3_z_axis_top = NULL;
  t3_z_axis_top_rt = NULL;
}

void GraphTableView::InitLinks() {
  inherited::InitLinks();
  taBase::Own(x_axis, this);
  taBase::Own(z_axis, this);
  taBase::Own(plot_1, this);
  taBase::Own(plot_2, this);
  taBase::Own(plot_3, this);
  taBase::Own(plot_4, this);
  taBase::Own(plot_5, this);
  taBase::Own(plot_6, this);
  taBase::Own(plot_7, this);
  taBase::Own(plot_8, this);
  taBase::Own(err_1, this);
  taBase::Own(err_2, this);
  taBase::Own(err_3, this);
  taBase::Own(err_4, this);
  taBase::Own(err_5, this);
  taBase::Own(err_6, this);
  taBase::Own(err_7, this);
  taBase::Own(err_8, this);
  taBase::Own(color_axis, this);
  taBase::Own(raster_axis, this);
  taBase::Own(colorscale, this);
  taBase::Own(last_sel_pt, this);

  taBase::Own(main_y_plots, this);
  taBase::Own(alt_y_plots, this);

  all_plots[0] = &plot_1;
  all_plots[1] = &plot_2;
  all_plots[2] = &plot_3;
  all_plots[3] = &plot_4;
  all_plots[4] = &plot_5;
  all_plots[5] = &plot_6;
  all_plots[6] = &plot_7;
  all_plots[7] = &plot_8;

  all_errs[0] = &err_1;
  all_errs[1] = &err_2;
  all_errs[2] = &err_3;
  all_errs[3] = &err_4;
  all_errs[4] = &err_5;
  all_errs[5] = &err_6;
  all_errs[6] = &err_7;
  all_errs[7] = &err_8;
}

void GraphTableView::CutLinks() {
  x_axis.CutLinks();
  z_axis.CutLinks();
  plot_1.CutLinks();
  plot_2.CutLinks();
  plot_3.CutLinks();
  plot_4.CutLinks();
  plot_5.CutLinks();
  plot_6.CutLinks();
  plot_7.CutLinks();
  plot_8.CutLinks();
  err_1.CutLinks();
  err_2.CutLinks();
  err_3.CutLinks();
  err_4.CutLinks();
  err_5.CutLinks();
  err_6.CutLinks();
  err_7.CutLinks();
  err_8.CutLinks();
  color_axis.CutLinks();
  raster_axis.CutLinks();
  colorscale.CutLinks();
  inherited::CutLinks();
}

void GraphTableView::CopyFromView(GraphTableView* cp) {
  graph_type = cp->graph_type;
  plot_style = cp->plot_style;
  negative_draw = cp->negative_draw;
  negative_draw_z = cp->negative_draw_z;
  line_width = cp->line_width;
  point_size = cp->point_size;
  point_spacing = cp->point_spacing;
  bar_space = cp->bar_space;
  label_spacing = cp->label_spacing;
  width = cp->width;
  depth = cp->depth;
  axis_font_size = cp->axis_font_size;
  label_font_size = cp->label_font_size;

  x_axis.CopyFromView(&(cp->x_axis));
  z_axis.CopyFromView(&(cp->z_axis));
  for(int i=0;i<max_plots;i++) {
    all_plots[i]->CopyFromView(cp->all_plots[i]);
    all_errs[i]->CopyFromView(cp->all_errs[i]);
  }

  err_spacing = cp->err_spacing;
  err_bar_width = cp->err_bar_width;

  color_mode = cp->color_mode;
  color_axis.CopyFromView(&(cp->color_axis));
  colorscale = cp->colorscale;

  raster_axis.CopyFromView(&(cp->raster_axis));
  thresh = cp->thresh;
  thr_line_len= cp->thr_line_len;
  matrix_mode = cp->matrix_mode;
  mat_layout = cp->mat_layout;
  mat_odd_vert = cp->mat_odd_vert;

  two_d_font = cp->two_d_font;
  two_d_font_scale = cp->two_d_font_scale;

  for(int i=0;i<children.size;i++) {
    GraphColView* cvs = (GraphColView*)colView(i);
    GraphColView* cpvs = (GraphColView*)cp->children.FindName(cvs->name);
    if(cpvs) {
      cvs->CopyFromView(cpvs);
    }
  }
  T3DataViewMain::CopyFromViewFrame(cp);
}

void GraphTableView::UpdateAfterEdit_impl(){
  inherited::UpdateAfterEdit_impl();

  if(taMisc::is_loading) return;

  point_spacing = MAX(1, point_spacing);

  x_axis.axis_length = width;
  z_axis.axis_length = depth;

  x_axis.on = true;             // try to keep it on
  x_axis.UpdateOnFlag();
  z_axis.UpdateOnFlag();
  for(int i=0;i<max_plots;i++) {
    GraphPlotView* pl = all_plots[i];
    pl->UpdateOnFlag();
    GraphPlotView* epl = all_errs[i];
    epl->UpdateOnFlag();
  }
  color_axis.UpdateOnFlag();
  raster_axis.UpdateOnFlag();

  DataTable* dt = dataTable();
  bool no_cols = true;
  if(dt && dt->data.size > 0)
    no_cols = false;

  ////////////////////// update y axes in use, etc

  main_y_plots.Reset();
  alt_y_plots.Reset();
  do_matrix_plot = false;
  n_plots = 0;
  int first_mat = -1;
  for(int i=0;i<max_plots;i++) {
    GraphPlotView* pl = all_plots[i];
    if(!pl->on) continue;
    n_plots++;
    if(pl->alt_y) alt_y_plots.Add(i);
    else          main_y_plots.Add(i);
    GraphColView* plc = all_plots[i]->GetColPtr();
    if(plc->dataCol()->is_matrix) {
      if(!do_matrix_plot) first_mat = i;
      do_matrix_plot = true;
    }
  }
  if(do_matrix_plot && n_plots > 1) {
    n_plots = 1;
    taMisc::Warning("GraphTableView -- a data column to be plotted is a matrix, so all other plots are being turned off (matrix can only be plotted by itself)");
    main_y_plots.Reset();
    alt_y_plots.Reset();
    for(int i=0;i<max_plots;i++) {
      GraphPlotView* pl = all_plots[i];
      if(!pl->on) continue;
      if(i == first_mat) continue;
      pl->on = false;           // turn off
      if(pl->alt_y) alt_y_plots.RemoveIdx(i);
      else          main_y_plots.RemoveIdx(i);
    }
  }

  if(main_y_plots.size > 0) {
    GraphPlotView* pl = all_plots[main_y_plots[0]];
    if(pl->isString()) {
      int non_str = -1;
      for(int i=1; i< main_y_plots.size; i++) {
        if(!all_plots[main_y_plots[i]]->isString()) {
          non_str = i;
          break;
        }
      }

      if(non_str == -1) {
        taMisc::Warning("GraphTableView -- can't have only a string value(s) plotted on main Y axis -- must also include a numeric column to provide Y axis values -- turning off plot!");
        for(int i=0; i< main_y_plots.size; i++) {
          all_plots[main_y_plots[i]]->on = false;
        }
        n_plots -= main_y_plots.size;
        main_y_plots.Reset();
      }
      else {
        main_y_plots.SwapIdx(0,non_str); // exchange
      }
    }
  }

  if(alt_y_plots.size > 0) {
    GraphPlotView* pl = all_plots[alt_y_plots[0]];
    if(pl->isString()) {        // oops.. not again!
      int non_str = -1;
      for(int i=1; i< alt_y_plots.size; i++) {
        if(!all_plots[alt_y_plots[i]]->isString()) {
          non_str = i;
          break;
        }
      }

      if(non_str == -1) {
        taMisc::Warning("GraphTableView -- can't have only a string value(s) plotted on alt Y axis -- must also include a numeric column to provide Y axis values -- turning off plot!");
        for(int i=0; i< alt_y_plots.size; i++) {
          all_plots[alt_y_plots[i]]->on = false;
        }
        n_plots -= alt_y_plots.size;
        alt_y_plots.Reset();
      }
      else {
        alt_y_plots.SwapIdx(0,non_str); // exchange
      }
    }
  }

  if(n_plots > 0 && main_y_plots.size == 0) { // no first y's so must be alts -- switch them!
    main_y_plots = alt_y_plots;
    alt_y_plots.Reset();
  }

  mainy = NULL;
  if(main_y_plots.size > 0) mainy = all_plots[main_y_plots[0]];
  alty = NULL;
  if(alt_y_plots.size > 0) alty = all_plots[alt_y_plots[0]];

  ///////////////////////////////////////////////////////////
  // other axes

  if(!x_axis.on) {
    if(!no_cols) {
      taMisc::Warning("GraphTableView -- X axis is not on -- perhaps no valid col_name found for x_axis -- switching to row_num!");
      x_axis.row_num = true;
      x_axis.on = true;
    }
  }
  else {
    if(x_axis.isString()) {
      x_axis.row_num = true;    // must be row num!
    }
    if(!x_axis.row_num) {
      GraphColView* xa = x_axis.GetColPtr();
      if(xa->dataCol()->is_matrix) {
        taMisc::Warning("GraphTableView -- column", x_axis.col_name, "is a matrix -- cannot be used for an X axis -- resetting to default");
        FindDefaultXZAxes();
      }
    }
  }

  if(z_axis.isString()) {
    z_axis.row_num = true;      // must be row num!
  }

  if(graph_type == RASTER) {
    if((plot_style != THRESH_LINE) && (plot_style !=  THRESH_POINT))
      color_mode = VALUE_COLOR;
    raster_axis.on = true;
    raster_axis.UpdateOnFlag();
    if(!raster_axis.on) {
      if(!no_cols) {
        taMisc::Warning("GraphTableView -- graph_type = RASTER and no valid col_name found for raster_axis -- nothing will be plotted!");
      }
    }
    else {
      GraphColView* ra = raster_axis.GetColPtr();
      if(ra->dataCol()->is_matrix) {
        taMisc::Warning("GraphTableView -- column", raster_axis.col_name, "is a matrix -- cannot be used for raster_axis");
        raster_axis.col_name = "";
        raster_axis.on = false;
      }
    }
  }

  if(color_mode == COLOR_AXIS) {
    color_axis.on = true;
    color_axis.UpdateOnFlag();
    if(!color_axis.on) {
      if(!no_cols) {
        taMisc::Warning("GraphTableView -- color_mode = COLOR_AXIS and no valid col_name found for color_axis -- nothing will be plotted!");
      }
    }
    else {
      GraphColView* ca = color_axis.GetColPtr();
      if(ca->dataCol()->is_matrix) {
        taMisc::Warning("GraphTableView -- column", color_axis.col_name, "is a matrix -- cannot be used for color_axis");
        color_axis.col_name = "";
        color_axis.on = false;
      }
    }
  }
}

const iColor GraphTableView::bgColor(bool& ok) const {
  ok = true;
  return colorscale.background;
}

void GraphTableView::UpdateName() {
  DataTable* dt = dataTable();
  if (dt) {
    if (!name.contains(dt->name))
      SetName(dt->name + "_Graph");
  }
  else {
    if (name.empty())
      SetName("graph_no_table");
  }
}

const String GraphTableView::caption() const {
  String rval = inherited::caption();
  if(last_sel_col_nm.nonempty()) {
    rval += " pt: " + last_sel_col_nm + " = " + last_sel_pt.GetStr();
  }
  return rval;
}

void GraphTableView_MouseCB(void* userData, SoEventCallback* ecb) {
  GraphTableView* nv = (GraphTableView*)userData;
  T3DataViewFrame* fr = nv->GetFrame();
  SoMouseButtonEvent* mouseevent = (SoMouseButtonEvent*)ecb->getEvent();
  SoMouseButtonEvent::Button but = mouseevent->getButton();
  if(!SoMouseButtonEvent::isButtonReleaseEvent(mouseevent, but)) return; // only releases
  bool got_one = false;
  for(int i=0;i<fr->root_view.children.size;i++) {
    taDataView* dv = fr->root_view.children[i];
    if(dv->InheritsFrom(&TA_GraphTableView)) {
      GraphTableView* tgv = (GraphTableView*)dv;
      T3ExaminerViewer* viewer = tgv->GetViewer();
      SoRayPickAction rp( viewer->getViewportRegion());
      rp.setPoint(mouseevent->getPosition());
      rp.apply(viewer->quarter->getSoEventManager()->getSceneGraph()); // event mgr has full graph!

      SoPickedPoint* pp = rp.getPickedPoint(0);
      if(!pp) continue;
      SoNode* pobj = pp->getPath()->getNodeFromTail(2);
      if(!pobj) continue;
      if(!pobj->isOfType(T3GraphLine::getClassTypeId())) {
        continue;
      }
      GraphAxisBase* gab = static_cast<GraphAxisBase*>(((T3GraphLine*)pobj)->dataView());
      if(!gab) continue;
      if(!gab->InheritsFrom(&TA_GraphPlotView)) continue;
      GraphPlotView* gpv = (GraphPlotView*)gab;
      if(!gpv->eff_y_axis) continue;
      SbVec3f pt = pp->getObjectPoint(pobj);
      if(pt[1] == 0.0f && pt[2] == 0.0f) continue; // indicates an axis caption line!
      float xv = tgv->x_axis.range.Project(pt[0] / tgv->width);
      float yv = gpv->eff_y_axis->range.Project(pt[1]);
      float zv = 0.0f;
      if(tgv->z_axis.on)
        zv = tgv->z_axis.range.Project(pt[2] / tgv->depth);
      tgv->last_sel_pt.SetXYZ(xv, yv, zv);
      tgv->last_sel_col_nm = gpv->col_name;
      //      tgv->InitDisplay();
      tgv->UpdateDisplay();
      got_one = true;
      break;                    // once you get one, stop!
    }
  }
  if(got_one)
    ecb->setHandled();
}

void GraphTableView::Render_pre() {
  bool show_drag = manip_ctrl_on;
  T3ExaminerViewer* vw = GetViewer();
  if(vw) {
    vw->syncViewerMode();
    if(!vw->interactionModeOn())
      show_drag = false;
  }

  setNode(new T3GraphViewNode(this, width, show_drag));

/*NOTES:
1. Render_pre is only done once, or on STRUCT changes
2. item should not take ownsership of a shared frame prop like bg color
3. we can set the frame bg color to the default for graphs

*/

  SoEventCallback* ecb = new SoEventCallback;
  ecb->addEventCallback(SoMouseButtonEvent::getClassTypeId(), GraphTableView_MouseCB, this);
  node_so()->addChild(ecb);

  inherited::Render_pre();
}

void GraphTableView::Render_impl() {
  inherited::Render_impl();
  T3GraphViewNode* node_so = this->node_so(); // cache
  if(!node_so || !dataTable())
    return;

  node_so->setWidth(width);     // does a render too -- ensure always up to date on width
  int orig_rows;
  CheckRowsChanged(orig_rows);  // don't do anything with this here, but just make sure m_rows is up to date
  MakeViewRangeValid();
  ComputeAxisRanges();
  SetScrollBars();
  RenderGraph();
  //  UpdatePanel();            // otherwise doesn't get updated without explicit click..
  // nein!  this is called in UpdateDisplay -- gets called 2x with this!
}

void GraphTableView::Render_post() {
  inherited::Render_post();
}

void GraphTableView::InitDisplay(bool init_panel) {
  MakeViewRangeValid();
  RemoveGraph();
  if(init_panel) {
    InitPanel();
  }
}

void GraphTableView::UpdateDisplay(bool update_panel) {
  int old_rows;
  int delta_rows = CheckRowsChanged(old_rows);

  if (!isVisible()) return;

  if(delta_rows > 0) {
    // if we were not at the very end, then don't scroll, but do update the panel
    if(update_panel && (view_range.max < old_rows-1) && (view_range.max > 0)) {
      UpdatePanel();
      return;
    }
    // scroll down to end of new data
    view_range.max = m_rows - 1;
    view_range.min = view_range.max - view_rows + 1;
    view_range.min = MAX(0, view_range.min);
  }

  if (update_panel) UpdatePanel();
  Render_impl();
}

void GraphTableView::DataUpdateView_impl() {
//   taMisc::Info("data update view");
  if(!display_on) return;
  int old_rows;
  int delta_rows = CheckRowsChanged(old_rows);

  if (!isVisible()) return;

  if(delta_rows > 0) {
    // if we were not at the very end, then don't scroll, but do update the panel
    if((view_range.max < old_rows-1) && (view_range.max > 0)) {
      UpdatePanel();
      return;
    }
    // scroll down to end of new data
    view_range.max = m_rows - 1;
    view_range.min = view_range.max - view_rows + 1;
    view_range.min = MAX(0, view_range.min);
  }

  MakeViewRangeValid();
  ComputeAxisRanges();
  SetScrollBars();
  RenderGraph();
}

void GraphTableView::ComputeAxisRanges() {
  UpdateAfterEdit_impl();

  x_axis.ComputeRange();
  if(do_matrix_plot && matrix_mode == Z_INDEX && mainy) {
    DataCol* da_y = mainy->GetDAPtr();
    if(da_y && da_y->is_matrix) {
      z_axis.SetRange_impl(0.0f, da_y->cell_size());
    }
    else {
      z_axis.ComputeRange();
    }
  }
  else {
    z_axis.ComputeRange();
  }

  for(int i=0;i<max_plots;i++) {
    GraphPlotView* pl = all_plots[i];
    pl->ComputeRange();
  }

  if(mainy) {
    for(int i=1;i<main_y_plots.size;i++) {
      GraphPlotView* pl = all_plots[main_y_plots[i]];
      if(pl->isString()) continue;
      mainy->UpdateRange_impl(pl->data_range.min, pl->data_range.max);
    }
  }

  if(alty) {
    for(int i=1;i<alt_y_plots.size;i++) {
      GraphPlotView* pl = all_plots[alt_y_plots[i]];
      if(pl->isString()) continue;
      alty->UpdateRange_impl(pl->data_range.min, pl->data_range.max);
    }
  }

  if(color_mode == COLOR_AXIS)
    color_axis.ComputeRange();
  if(graph_type == RASTER)
    raster_axis.ComputeRange();
}

void GraphTableView_RowScrollCB(SoScrollBar* sb, int val, void* user_data) {
  GraphTableView* gtv = (GraphTableView*)user_data;
  gtv->scrolling_ = true;
  gtv->ViewRow_At(val);
  gtv->scrolling_ = false;
}

void GraphTableView::SetScrollBars() {
  if(scrolling_) return;                     // don't redo if currently doing!
  T3GraphViewNode* node_so = this->node_so(); // cache
  if(!node_so) return;

  SoScrollBar* rsb = node_so->RowScrollBar();
  //  rsb->setMinimum(0);
  //  rsb->setSingleStep(1);
  int mx = MAX((rows() - view_rows), 0);
  rsb->setMaximum(mx);
  int pg_step = MAX(view_rows, 1);
  rsb->setPageStep(pg_step);
  rsb->setValue(MIN(view_range.min, mx));
  rsb->setValueChangedCB(GraphTableView_RowScrollCB, this);
}

void GraphTableView::Clear_impl() {
  RemoveGraph();
  inherited::Clear_impl();
}

void GraphTableView::OnWindowBind_impl(iT3DataViewFrame* vw) {
  inherited::OnWindowBind_impl(vw);
  if (!m_lvp) {
    m_lvp = new iGraphTableView_Panel(this);
    vw->RegisterPanel(m_lvp);
  }
}

void GraphTableView::RemoveGraph(){
  T3GraphViewNode* node_so = this->node_so();
  if (!node_so) return;
  node_so->graphs()->removeAllChildren();
  node_so->y_axes()->removeAllChildren();
}

void GraphTableView::FindDefaultXZAxes() {
  // first, look for explicit flags
  bool set_x = false;
  z_axis.on = false;            // assume we're not going to find it
  for(int i=children.size-1;i>=0;i--) {
    GraphColView* cvs = (GraphColView*)colView(i);
    DataCol* da = cvs->dataCol();
    if(da->HasUserData("X_AXIS")) {
      x_axis.col_name = cvs->name;
      x_axis.InitFromUserData();
      x_axis.UpdateOnFlag();
      set_x = true;
    }
    if(da->HasUserData("Z_AXIS")) {
      z_axis.col_name = cvs->name;
      z_axis.on = true;
      z_axis.InitFromUserData();
      z_axis.UpdateOnFlag();
    }
  }
  if(set_x) return;

  // then, find X axis by getting *last* int col (first searching backwards), then Z is next
  for(int i=children.size-1;i>=0;i--) {
    GraphColView* cvs = (GraphColView*)colView(i);
    DataCol* da = cvs->dataCol();
    if(da->is_matrix || da->valType() != VT_INT) continue;
    if(set_x) {                 // must be Z
      z_axis.col_name = cvs->name;
      z_axis.on = true;         // found one
      z_axis.InitFromUserData();
      z_axis.UpdateOnFlag();
      break;                    // done
    }
    else {
      x_axis.col_name = cvs->name;
      x_axis.InitFromUserData();
      x_axis.UpdateOnFlag();
      set_x = true;
    }
  }
  if(!set_x) {                  // didn't find it -- look for first numeric column, just for x
    for(int i=0;i<children.size;i++) {
      GraphColView* cvs = (GraphColView*)colView(i);
      DataCol* da = cvs->dataCol();
      if(da->is_matrix || !da->isNumeric()) continue;
      x_axis.col_name = cvs->name;
      x_axis.InitFromUserData();
      x_axis.UpdateOnFlag();
      set_x = true;
      break;
    }
  }
}

void GraphTableView::FindDefaultPlot1() {
  bool got_1 = false;
  for(int i=children.size-1;i>=0;i--) {
    GraphColView* cvs = (GraphColView*)colView(i);
    DataCol* da = cvs->dataCol();
    if(da->HasUserData("PLOT_1")) {
      plot_1.col_name = cvs->name;
      plot_1.InitFromUserData();
      plot_1.UpdateOnFlag();
      got_1 = true;
    }
  }
  if(got_1) return;

  // next, find first float/double and that is the plot_1
  for(int i=0;i<children.size;i++) {
    GraphColView* cvs = (GraphColView*)colView(i);
    DataCol* da = cvs->dataCol();
    if((da->valType() != VT_FLOAT) && (da->valType() != VT_DOUBLE)) continue;
    if(x_axis.col_name == cvs->name) continue; // don't make it same as X!
    plot_1.col_name = cvs->name;
    plot_1.InitFromUserData();
    plot_1.UpdateOnFlag();
    break;                      // once you get one, that's it
  }
}


void GraphTableView::InitFromUserData() {
  FindDefaultXZAxes();
  FindDefaultPlot1();

  // next, find first float/double and that is the plot_1
  for(int i=0;i<children.size;i++) {
    GraphColView* cvs = (GraphColView*)colView(i);
    DataCol* da = cvs->dataCol();
    for(int i=1; i<max_plots; i++) {
      String pltst = "PLOT_" + String(i+1);
      if(da->HasUserData(pltst)) {
        all_plots[i]->col_name = cvs->name;  all_plots[i]->on = true;
        all_plots[i]->InitFromUserData();    all_plots[i]->UpdateOnFlag();
      }
    }
    for(int i=0; i<max_plots; i++) {
      String pltst = "ERR_" + String(i+1);
      if(da->HasUserData(pltst)) {
        all_errs[i]->col_name = cvs->name;  all_errs[i]->on = true;
        all_errs[i]->InitFromUserData();    all_errs[i]->UpdateOnFlag();
      }
    }
    if(da->HasUserData("COLOR_AXIS")) {
      color_axis.col_name = cvs->name;   color_axis.on = true;
      color_axis.InitFromUserData();     color_axis.UpdateOnFlag();
    }
    if(da->HasUserData("RASTER_AXIS")) {
      raster_axis.col_name = cvs->name;  raster_axis.on = true;
      raster_axis.InitFromUserData();    raster_axis.UpdateOnFlag();
    }
  }

  DataTable* dt = dataTable();
  if(dt->HasUserData("NO_Z_AXIS")) {
    z_axis.on = false;
  }
  String enum_tp_nm;
  if(dt->HasUserData("PLOT_STYLE")) {
    int pstv = GetEnumVal(dt->GetUserDataAsString("PLOT_STYLE"), enum_tp_nm);
    if(pstv >= 0)
      plot_style = (PlotStyle)pstv;
  }
  if(dt->HasUserData("GRAPH_TYPE")) {
    int pstv = GetEnumVal(dt->GetUserDataAsString("GRAPH_TYPE"), enum_tp_nm);
    if(pstv >= 0)
      graph_type = (GraphType)pstv;
  }
  if(dt->HasUserData("POINT_SIZE")) {
    int pstv = GetEnumVal(dt->GetUserDataAsString("POINT_SIZE"), enum_tp_nm);
    if(pstv >= 0)
      point_size = (PointSize)pstv;
  }
  if(dt->HasUserData("COLOR_MODE")) {
    int pstv = GetEnumVal(dt->GetUserDataAsString("COLOR_MODE"), enum_tp_nm);
    if(pstv >= 0)
      color_mode = (ColorMode)pstv;
  }
  if(dt->HasUserData("MATRIX_MODE")) {
    int pstv = GetEnumVal(dt->GetUserDataAsString("MATRIX_MODE"), enum_tp_nm);
    if(pstv >= 0)
      matrix_mode = (MatrixMode)pstv;
  }
  if(dt->HasUserData("LINE_WIDTH")) {
    line_width = dt->GetUserDataAsFloat("LINE_WIDTH");
  }
  if(dt->HasUserData("POINT_SPACING")) {
    point_spacing = dt->GetUserDataAsInt("POINT_SPACING");
  }
  if(dt->HasUserData("LABEL_SPACING")) {
    label_spacing = dt->GetUserDataAsInt("LABEL_SPACING");
  }
  if(dt->HasUserData("BAR_SPACE")) {
    bar_space = dt->GetUserDataAsFloat("BAR_SPACE");
  }
  if(dt->HasUserData("NEG_DRAW")) {
    negative_draw = dt->GetUserDataAsBool("NEG_DRAW");
  }
  if(dt->HasUserData("NEG_DRAW_Z")) {
    negative_draw_z = dt->GetUserDataAsBool("NEG_DRAW_Z");
  }
}

void GraphTableView::UpdateFromDataTable_this(bool first) {
  inherited::UpdateFromDataTable_this(first);
  if(!first) return;

  InitFromUserData();
}

///////////////////////////////////////////////////////////////
//      Actual Rendering of graph display

void GraphTableView::RenderGraph() {
//   taMisc::Info("render graph");
  UpdateAfterEdit_impl();
  if(n_plots == 0 || !x_axis.on) return;

  RenderAxes();

  if(do_matrix_plot && mainy && mainy->GetDAPtr()->is_matrix) {
    if(matrix_mode == SEP_GRAPHS)
      RenderGraph_Matrix_Sep();
    else
      RenderGraph_Matrix_Zi();
  }
  else {
    RenderLegend();
    if(graph_type == BAR)
      RenderGraph_Bar();
    else
      RenderGraph_XY();
  }
}


void GraphTableView::RenderAxes() {
  T3GraphViewNode* node_so = this->node_so();
  if (!node_so) return;

  if(!x_axis.on || !mainy) return;

  SoSeparator* xax = node_so->x_axis();
  xax->removeAllChildren();
  SoSeparator* zax = node_so->z_axis();
  zax->removeAllChildren();
  SoSeparator* yax = node_so->y_axes();
  yax->removeAllChildren();

  t3_x_axis = new T3Axis((T3Axis::Axis)x_axis.axis, &x_axis, axis_font_size);
  t3_x_axis_top = new T3Axis((T3Axis::Axis)x_axis.axis, &x_axis, axis_font_size);

  xax->addChild(t3_x_axis);

  float ylen = plot_1.axis_length;

  SoTranslation* tr;
  // top
  tr = new SoTranslation();  xax->addChild(tr);
  tr->translation.setValue(0.0f, ylen, 0.0f);
  xax->addChild(t3_x_axis_top);

  x_axis.RenderAxis(t3_x_axis);
  x_axis.RenderAxis(t3_x_axis_top, 0, true); // ticks only

  if(z_axis.on) {
    t3_x_axis_far = new T3Axis((T3Axis::Axis)x_axis.axis, &x_axis, axis_font_size);
    t3_x_axis_far_top = new T3Axis((T3Axis::Axis)x_axis.axis, &x_axis, axis_font_size);

    // far_top
    tr = new SoTranslation();   xax->addChild(tr);
    tr->translation.setValue(0.0f, 0.0, -z_axis.axis_length);
    xax->addChild(t3_x_axis_far_top);
    // far
    tr = new SoTranslation();  xax->addChild(tr);
    tr->translation.setValue(0.0f, -ylen, 0.0f);
    xax->addChild(t3_x_axis_far);

    x_axis.RenderAxis(t3_x_axis_far_top, 0, true); // ticks only
    x_axis.RenderAxis(t3_x_axis_far, 0, true); // ticks only

    /////////////  Z
    SoSeparator* zax = node_so->z_axis();
    t3_z_axis = new T3Axis((T3Axis::Axis)z_axis.axis, &z_axis, axis_font_size);
    t3_z_axis_rt = new T3Axis((T3Axis::Axis)z_axis.axis, &z_axis, axis_font_size);
    t3_z_axis_top = new T3Axis((T3Axis::Axis)z_axis.axis, &z_axis, axis_font_size);
    t3_z_axis_top_rt = new T3Axis((T3Axis::Axis)z_axis.axis, &z_axis, axis_font_size);

    zax->addChild(t3_z_axis);
    z_axis.RenderAxis(t3_z_axis);

    // rt
    tr = new SoTranslation();   zax->addChild(tr);
    tr->translation.setValue(x_axis.axis_length, 0.0f, 0.0f);
    zax->addChild(t3_z_axis_rt);
    // top_rt
    tr = new SoTranslation();   zax->addChild(tr);
    tr->translation.setValue(0.0f, ylen, 0.0f);
    zax->addChild(t3_z_axis_top_rt);
    // top
    tr = new SoTranslation();   zax->addChild(tr);
    tr->translation.setValue(-x_axis.axis_length, 0.0f, 0.0f);
    zax->addChild(t3_z_axis_top);

    z_axis.RenderAxis(t3_z_axis_rt, 0, true); // ticks only
    z_axis.RenderAxis(t3_z_axis_top_rt, 0, true);
    z_axis.RenderAxis(t3_z_axis_top, 0, true);
  }
  else {
    t3_x_axis_far = NULL;
    t3_x_axis_far_top = NULL;
    t3_z_axis = NULL;
    t3_z_axis_rt = NULL;
    t3_z_axis_top = NULL;
    t3_z_axis_top_rt = NULL;
  }

  if(graph_type == RASTER) {
    t3_y_axis = new T3Axis((T3Axis::Axis)raster_axis.axis, &raster_axis, axis_font_size);
    raster_axis.RenderAxis(t3_y_axis); // raster axis is Y axis!
    yax->addChild(t3_y_axis);

  }
  else {
    t3_y_axis = new T3Axis((T3Axis::Axis)mainy->axis, mainy, axis_font_size);
    mainy->RenderAxis(t3_y_axis);
    yax->addChild(t3_y_axis);

    if(z_axis.on) {
      t3_y_axis_far = new T3Axis((T3Axis::Axis)mainy->axis, mainy, axis_font_size);
      mainy->RenderAxis(t3_y_axis_far, 0, true); // only ticks
      // far
      tr = new SoTranslation();   yax->addChild(tr);
      tr->translation.setValue(0.0f, 0.0f, -z_axis.axis_length);
      yax->addChild(t3_y_axis_far);
      tr = new SoTranslation();   yax->addChild(tr); // reset this guy for next..
      tr->translation.setValue(0.0f, 0.0f, z_axis.axis_length);
    }
    else {
      t3_y_axis_far = NULL;
    }

    if(alty) {
      t3_y_axis_rt = new T3Axis((T3Axis::Axis)alty->axis, alty, axis_font_size, 1); // second Y = 1
      alty->RenderAxis(t3_y_axis_rt, 1); // indicate second axis!
      tr = new SoTranslation();  yax->addChild(tr);
      tr->translation.setValue(x_axis.axis_length, 0.0f, 0.0f); // put on right hand side!
      yax->addChild(t3_y_axis_rt);

      if(z_axis.on) {
        t3_y_axis_far_rt = new T3Axis((T3Axis::Axis)alty->axis, alty, axis_font_size, 1);
        alty->RenderAxis(t3_y_axis_far_rt, 1, true); // only ticks
        tr = new SoTranslation();   yax->addChild(tr);
        tr->translation.setValue(0.0f, 0.0f, -z_axis.axis_length);
        yax->addChild(t3_y_axis_far_rt);
      }
      else {
        t3_y_axis_far_rt = NULL;
      }
    }
    else {
      // rt
      t3_y_axis_rt = new T3Axis((T3Axis::Axis)mainy->axis, mainy, axis_font_size);
      mainy->RenderAxis(t3_y_axis_rt, 0, true); // ticks
      tr = new SoTranslation();   yax->addChild(tr);
      tr->translation.setValue(x_axis.axis_length, 0.0f, 0.0f);
      yax->addChild(t3_y_axis_rt);

      if(z_axis.on) {
        t3_y_axis_far_rt = new T3Axis((T3Axis::Axis)mainy->axis, mainy, axis_font_size);
        mainy->RenderAxis(t3_y_axis_far_rt, 0, true); // only ticks
        tr = new SoTranslation();   yax->addChild(tr);
        tr->translation.setValue(0.0f, 0.0f, -z_axis.axis_length);
        yax->addChild(t3_y_axis_far_rt);
      }
      else {
        t3_y_axis_far_rt = NULL;
      }
    }
  }
}

void GraphTableView::RenderLegend_Ln(GraphPlotView& plv, T3GraphLine* t3gl) {
  t3gl->clear();
  t3gl->startBatch();
  t3gl->setLineStyle((T3GraphLine::LineStyle)plv.line_style, line_width);
  t3gl->setMarkerSize((T3GraphLine::MarkerSize)point_size);
  t3gl->setValueColorMode(false);
  t3gl->setDefaultColor((T3Color)(plv.color.color()));

  String label = plv.col_name; taMisc::SpaceLabel(label);

  iVec3f st;
  iVec3f ed;
  ed.x = 1.5f * label_font_size;
  t3gl->moveTo(st);
  t3gl->lineTo(ed);
  t3gl->textAt(iVec3f(ed.x + TICK_OFFSET,  ed.y - (.5f * label_font_size), ed.z),
               label.chars());
  t3gl->finishBatch();
}

void GraphTableView::RenderLegend() {
  T3GraphViewNode* node_so = this->node_so();
  if (!node_so) return;

  float ylen = plot_1.axis_length;

  SoSeparator* leg = node_so->legend();
  leg->removeAllChildren();
  // move to top
  SoTranslation* tr;
  tr = new SoTranslation();  leg->addChild(tr);
  tr->translation.setValue(0.0f, ylen + 2.5f * axis_font_size, 0.0f);

  float over_amt = .33f * x_axis.axis_length;
  float dn_amt = -1.1f * axis_font_size;
  bool mv_dn = true;            // else over

  // keep in same order as plotting: main then alt
  for(int i=0;i<main_y_plots.size;i++) {
    GraphPlotView* pl = all_plots[main_y_plots[i]];
    T3GraphLine* ln = new T3GraphLine(pl, axis_font_size); leg->addChild(ln);
    RenderLegend_Ln(*pl, ln);
    tr = new SoTranslation();  leg->addChild(tr);
    if(mv_dn)    tr->translation.setValue(0.0f, dn_amt, 0.0f);
    else         tr->translation.setValue(over_amt, -dn_amt, 0.0f);
    mv_dn = !mv_dn;             // flip
  }

  for(int i=0;i<alt_y_plots.size;i++) {
    GraphPlotView* pl = all_plots[alt_y_plots[i]];
    T3GraphLine* ln = new T3GraphLine(pl, axis_font_size); leg->addChild(ln);
    RenderLegend_Ln(*pl, ln);
    tr = new SoTranslation();  leg->addChild(tr);
    if(mv_dn)    tr->translation.setValue(0.0f, dn_amt, 0.0f);
    else         tr->translation.setValue(over_amt, -dn_amt, 0.0f);
    mv_dn = !mv_dn;             // flip
  }
}

void GraphTableView::RenderGraph_XY() {
  T3GraphViewNode* node_so = this->node_so();
  if (!node_so) return;

  if(!mainy) return;

  DataCol* da_1 = mainy->GetDAPtr();
  if(!da_1) return;

  SoSeparator* graphs = node_so->graphs();
  graphs->removeAllChildren();	// this is the "nuclear option" that ensures full redraw
  // the code below also allows re-use, but this does NOT speed up rendering appreciably
  // and appears to be introducing some artifacts..

  SoSeparator* gr1 = NULL;
  if(graphs->getNumChildren() == 1) {
    gr1 = (SoSeparator*)graphs->getChild(0);
  }
  else {
    gr1 = new SoSeparator;
    graphs->addChild(gr1);
  }

  float boxd = 0.0f;
  if(z_axis.on)
    boxd = depth;

  // each graph has a box and lines..
  SoLineBox3d* lbox = NULL;
  if(gr1->getNumChildren() > 0) {
    lbox = (SoLineBox3d*)gr1->getChild(0);
  }
  else {
    lbox = new SoLineBox3d(width, 1.0f, boxd, false); // not centered
    gr1->addChild(lbox);
  }

  for(int i=0;i<main_y_plots.size;i++) {
    GraphPlotView* pl = all_plots[main_y_plots[i]];
    T3GraphLine* ln = NULL;
    if(gr1->getNumChildren() > i+1) {
      ln = (T3GraphLine*)gr1->getChild(i+1);
      ln->clear();
    }
    else {
      ln = new T3GraphLine(pl, label_font_size);
      gr1->addChild(ln);
    }
    if(pl->isString()) {
      PlotData_String(*pl, *mainy, ln);
    }
    else {
      PlotData_XY(*pl, *all_errs[main_y_plots[i]], *mainy, ln);
    }
  }

  int stidx = main_y_plots.size + 1;

  for(int i=0;i<alt_y_plots.size;i++) {
    GraphPlotView* pl = all_plots[alt_y_plots[i]];
    T3GraphLine* ln = NULL;
    if(gr1->getNumChildren() > stidx+i) {
      ln = (T3GraphLine*)gr1->getChild(stidx+i);
      ln->clear();
    }
    else {
      ln = new T3GraphLine(pl, label_font_size);
      gr1->addChild(ln);
    }
    if(pl->isString()) {
      PlotData_String(*pl, *alty, ln);
    }
    else {
      PlotData_XY(*pl, *all_errs[alt_y_plots[i]], *alty, ln);
    }
  }
}

void GraphTableView::RenderGraph_Bar() {
  T3GraphViewNode* node_so = this->node_so();
  if (!node_so) return;

  if(n_plots <= 0) return;

  DataCol* da_1 = mainy->GetDAPtr();
  if(!da_1) return;

  SoSeparator* graphs = node_so->graphs();
  graphs->removeAllChildren();

  SoSeparator* gr1 = new SoSeparator;
  graphs->addChild(gr1);

  float boxd = 0.0f;
  if(z_axis.on)
    boxd = depth;

  int n_str = 0;
  for(int i=0;i<max_plots;i++) {
    GraphPlotView* pl = all_plots[i];
    if(pl->on && pl->isString()) n_str++;
  }

  bar_width = (1.0f - bar_space) / ((float)(n_plots - n_str));
  float bar_off = - .5f * (1.0f - bar_space);

  // each graph has a box and lines..
  SoLineBox3d* lbox = new SoLineBox3d(width, 1.0f, boxd, false); // not centered
  gr1->addChild(lbox);

  for(int i=0;i<main_y_plots.size;i++) {
    GraphPlotView* pl = all_plots[main_y_plots[i]];
    T3GraphLine* ln = new T3GraphLine(pl, label_font_size); gr1->addChild(ln);
    if(pl->isString()) {
      PlotData_String(*pl, *mainy, ln);
    }
    else {
      PlotData_Bar(*pl, *all_errs[main_y_plots[i]], *mainy, ln, bar_off);
      bar_off += bar_width;
    }
  }

  for(int i=0;i<alt_y_plots.size;i++) {
    GraphPlotView* pl = all_plots[alt_y_plots[i]];
    T3GraphLine* ln = new T3GraphLine(pl, label_font_size); gr1->addChild(ln);
    if(pl->isString()) {
      PlotData_String(*pl, *alty, ln);
    }
    else {
      PlotData_Bar(*pl, *all_errs[alt_y_plots[i]], *alty, ln, bar_off);
      bar_off += bar_width;
    }
  }
}

void GraphTableView::RenderGraph_Matrix_Zi() {
  T3GraphViewNode* node_so = this->node_so();
  if (!node_so) return;

  DataCol* da_1 = mainy->GetDAPtr();
  if(!da_1) return;

  SoSeparator* graphs = node_so->graphs();
  graphs->removeAllChildren();

  SoSeparator* gr1 = new SoSeparator;
  graphs->addChild(gr1);

  // each graph has a box and lines..
  SoLineBox3d* lbox = new SoLineBox3d(width, 1.0f, depth, false); // not centered
  gr1->addChild(lbox);

  for(int i=0;i<da_1->cell_size();i++) {
    T3GraphLine* ln = new T3GraphLine(mainy, label_font_size);
    gr1->addChild(ln);
    PlotData_XY(*mainy, *all_errs[main_y_plots[0]], *mainy, ln, i);
  }
}

void GraphTableView::RenderGraph_Matrix_Sep() {
  T3GraphViewNode* node_so = this->node_so();
  if (!node_so) return;

  DataCol* da_1 = mainy->GetDAPtr();
  if(!da_1) return;

  GraphPlotView* erry = all_errs[main_y_plots[0]];

  float boxd = .01f;
  if(z_axis.on)
    boxd = depth;

  SoSeparator* graphs = node_so->graphs();
  graphs->removeAllChildren();

  MatrixGeom& mgeom = da_1->cell_geom;

  int geom_x, geom_y;
  mgeom.Get2DGeomGui(geom_x, geom_y, mat_odd_vert, 1);
  float cl_x = 1.0f / (float)geom_x;    // how big each cell is
  float cl_y = 1.0f / (float)geom_y;
  float max_xy = MAX(cl_x, cl_y);

  taVector2i pos;
  int idx = 0;
  if(mgeom.dims() <= 2) {
    for(pos.y=0; pos.y<geom_y; pos.y++) {
      for(pos.x=0; pos.x<geom_x; pos.x++) { // right to left
        float xp = ((float)pos.x) * cl_x;
        float yp = ((float)pos.y) * cl_y;

        SoSeparator* gr = new SoSeparator;
        graphs->addChild(gr);

        SoTranslation* tr = new SoTranslation();
        gr->addChild(tr);
        tr->translation.setValue(xp, yp, 0.0f);

        // each graph has a box and lines..
        SoLineBox3d* lbox = new SoLineBox3d(width * cl_x, cl_y, boxd * max_xy, false); // not ctr
        gr->addChild(lbox);
        SoTransform* tx = new SoTransform();
        gr->addChild(tx);
        tx->scaleFactor.setValue(cl_x, cl_y, max_xy);
        T3GraphLine* ln = new T3GraphLine(mainy, label_font_size);
        gr->addChild(ln);
        if(mat_layout == taMisc::TOP_ZERO) {
          if(mgeom.dims() == 1)
            idx = MAX(geom_y-1-pos.y, pos.x);
          else
            idx = mgeom.IndexFmDims(pos.x, geom_y-1-pos.y);
        }
        else {
          if(mgeom.dims() == 1)
            idx = MAX(pos.y,pos.y);
          else
            idx = mgeom.IndexFmDims(pos.x, pos.y);
        }
        PlotData_XY(*mainy, *erry, *mainy, ln, idx);
      }
    }
  }
  else if(mgeom.dims() == 3) {
    int xmax = mgeom[0];
    int ymax = mgeom[1];
    int zmax = mgeom[2];
    for(int z=0; z<zmax; z++) {
      for(pos.y=0; pos.y<ymax; pos.y++) {
        for(pos.x=0; pos.x<xmax; pos.x++) {
          taVector2i apos = pos;
          if(mat_odd_vert)
            apos.y += z * (ymax+1);
          else
            apos.x += z * (xmax+1);
          float xp = ((float)apos.x) * cl_x;
          float yp = ((float)apos.y) * cl_y;

          SoSeparator* gr = new SoSeparator;
          graphs->addChild(gr);

          SoTranslation* tr = new SoTranslation();
          gr->addChild(tr);
          tr->translation.setValue(xp, yp, 0.0f);

          // each graph has a box and lines..
          SoLineBox3d* lbox = new SoLineBox3d(width * cl_x, cl_y, boxd * max_xy, false); // not ctr
          gr->addChild(lbox);
          SoTransform* tx = new SoTransform();
          gr->addChild(tx);
          tx->scaleFactor.setValue(cl_x, cl_y, max_xy);
          T3GraphLine* ln = new T3GraphLine(mainy, label_font_size);
          gr->addChild(ln);
          if(mat_layout == taMisc::TOP_ZERO)
            idx = mgeom.IndexFmDims(pos.x, ymax-1-pos.y, zmax-1-z);
          else
            idx = mgeom.IndexFmDims(pos.x, pos.y, z);
          PlotData_XY(*mainy, *erry, *mainy, ln, idx);
        }
      }
    }
  }
  else if(mgeom.dims() == 4) {
    int xmax = mgeom[0];
    int ymax = mgeom[1];
    int xxmax = mgeom[2];
    int yymax = mgeom[3];
    taVector2i opos;
    for(opos.y=0; opos.y<yymax; opos.y++) {
      for(opos.x=0; opos.x<xxmax; opos.x++) {
        for(pos.y=0; pos.y<ymax; pos.y++) {
          for(pos.x=0; pos.x<xmax; pos.x++) {
            taVector2i apos = pos;
            apos.x += opos.x * (xmax+1);
            apos.y += opos.y * (ymax+1);
            float xp = ((float)apos.x) * cl_x;
            float yp = ((float)apos.y) * cl_y;

            SoSeparator* gr = new SoSeparator;
            graphs->addChild(gr);

            SoTranslation* tr = new SoTranslation();
            gr->addChild(tr);
            tr->translation.setValue(xp, yp, 0.0f);

            // each graph has a box and lines..
            SoLineBox3d* lbox = new SoLineBox3d(width * cl_x, cl_y, boxd * max_xy, false); // not ctr
            gr->addChild(lbox);
            SoTransform* tx = new SoTransform();
            gr->addChild(tx);
            tx->scaleFactor.setValue(cl_x, cl_y, max_xy);
            T3GraphLine* ln = new T3GraphLine(mainy, label_font_size);
            gr->addChild(ln);
            if(mat_layout == taMisc::TOP_ZERO)
              idx = mgeom.IndexFmDims(pos.x, ymax-1-pos.y, opos.x, yymax-1-opos.y);
            else
              idx = mgeom.IndexFmDims(pos.x, pos.y, opos.x, opos.y);
            PlotData_XY(*mainy, *erry, *mainy, ln, idx);
          }
        }
      }
    }
  }
}

const iColor GraphTableView::GetValueColor(GraphAxisBase* ax_clr, float val) {
  iColor clr;

  if (ax_clr->range.Range() == 0) {
    clr = colorscale.GetColor((int)((.5f * (float)(colorscale.chunks-1)) + .5f));
  } else if(val > ax_clr->range.max) {
    clr = colorscale.maxout.color();
  } else if(val < ax_clr->range.min) {
    clr = colorscale.minout.color();
  } else {
    int chnk = colorscale.chunks-1;
    float rval = ax_clr->range.Normalize(val);
    int idx = (int) ((rval * (float)chnk) + .5);
    idx = MAX(0, idx);
    idx = MIN(chnk, idx);
    clr = colorscale.GetColor(idx);
  }
  return clr;
}

void GraphTableView::PlotData_XY(GraphPlotView& plv, GraphPlotView& erv, GraphPlotView& yax,
                                 T3GraphLine* t3gl, int mat_cell) {
  if(!t3gl) return;
  t3gl->clear();

  DataCol* da_y = plv.GetDAPtr();
  if(!da_y) return;
  DataCol* da_x = x_axis.GetDAPtr();
  DataCol* da_z = z_axis.GetDAPtr();

  DataTable* dt = dataTable();

  // this should only happen if we have no data at all..
  if((view_range.min < 0) || (view_range.min >= dt->rows)) return;
  if((view_range.min < 0) || (view_range.min >= dt->rows)) return;

  plv.eff_y_axis = &yax;                // set this for point clicking!

  t3gl->startBatch();
  t3gl->setLineStyle((T3GraphLine::LineStyle)plv.line_style, line_width);
  t3gl->setMarkerSize((T3GraphLine::MarkerSize)point_size);

  DataCol* da_er = erv.GetDAPtr();

  GraphAxisBase* ax_clr = NULL;
  DataCol* da_clr = NULL;
  if(color_mode == FIXED_COLOR) {
    t3gl->setValueColorMode(false);
    t3gl->setDefaultColor((T3Color)(plv.color.color()));
  }
  else {
    t3gl->setValueColorMode(true);
    if(color_mode == VALUE_COLOR) {
      ax_clr = &yax;
      da_clr = plv.GetDAPtr();
    }
    else {
      ax_clr = &color_axis;
      da_clr = color_axis.GetDAPtr();
    }
    if(!da_clr) {               // fallback
      t3gl->setValueColorMode(false);
      t3gl->setDefaultColor((T3Color)(plv.color.color()));
    }
  }

  GraphAxisBase* ax_rst = NULL;
  DataCol* da_rst = NULL;
  if(graph_type == RASTER) {
    ax_rst = &raster_axis;
    da_rst = raster_axis.GetDAPtr();
  }

  bool matz = false;
  if((mat_cell >= 0) && (matrix_mode == Z_INDEX)) matz = true;

  float last_x;
  float last_z = 0.0f;
  if(z_axis.on) {
    if(x_axis.row_num)
      last_z = view_range.min;
    else
      last_z = da_z->GetValAsFloat(view_range.min);
  }
  if(x_axis.row_num)
    last_x = view_range.min;
  else
    last_x = da_x->GetValAsFloat(view_range.min);
  iVec3f dat;                   // data point
  iVec3f plt;                   // plot coords
  iVec3f th_st;                 // start of thresholded line
  iVec3f th_ed;                 // end of thresholded line
  bool first = true;
  for (int row = view_range.min; row <= view_range.max; row++) {
    iColor clr; // only used for color modes
    bool clr_ok = false;
    bool new_trace = false;
    bool new_trace_z = false;
    if(x_axis.row_num)
      dat.x = row;
    else
      dat.x = da_x->GetValAsFloat(row);
    if(dat.x < x_axis.range.min || dat.x > x_axis.range.max) continue;
    if(z_axis.on) {
      if(z_axis.row_num)
        dat.z = row;
      else if(da_z)
        dat.z = da_z->GetValAsFloat(row);
      if(dat.z < z_axis.range.min || dat.z > z_axis.range.max) continue;
    }
    if((mat_cell >= 0) && (matrix_mode == Z_INDEX)) {
      dat.z = mat_cell;
    }
    float yval;
    if(mat_cell >= 0) {
      yval = da_y->GetValAsFloatM(row, mat_cell);
    }
    else {
      yval = da_y->GetValAsFloat(row);
    }
    if((graph_type == RASTER) && da_rst) {
      dat.y = da_rst->GetValAsFloat(row);
    }
    else {
      dat.y = yval;
    }

    plt.x = x_axis.DataToPlot(dat.x);
    if((graph_type == RASTER) && da_rst) {
      if(dat.y < ax_rst->range.min || dat.y > ax_rst->range.max) continue;
      plt.y = ax_rst->DataToPlot(dat.y);
    }
    else {
      if(dat.y < yax.range.min || dat.y > yax.range.max) continue;
      plt.y = yax.DataToPlot(dat.y);
    }
    if(z_axis.on || matz)
      plt.z = z_axis.DataToPlot(dat.z);

    if(dat.x < last_x)
      new_trace = true;
    if(z_axis.on && (dat.z < last_z))
      new_trace_z = true;


    if(da_clr) {
      clr_ok = true;
      if(color_mode == VALUE_COLOR) {
        clr = GetValueColor(ax_clr, yval);
      }
      else {
        clr = GetValueColor(ax_clr, da_clr->GetValAsFloat(row));
      }
    }

    // draw the line
    if(plot_style == LINE || plot_style == LINE_AND_POINTS) {
      if(first || (new_trace && !negative_draw) || (new_trace_z && !negative_draw_z))
      { // just starting out
        if(clr_ok)
          t3gl->moveTo(plt, (T3Color)(clr));
        else
          t3gl->moveTo(plt);
      }
      else {
        if(clr_ok)
          t3gl->lineTo(plt, (T3Color)(clr));
        else
          t3gl->lineTo(plt);
      }
    }
    else if(plot_style == THRESH_LINE) {
      if(yval >= thresh) {
        th_st = plt;  th_st.x = x_axis.DataToPlot(dat.x - thr_line_len);
        th_ed = plt;  th_ed.x = x_axis.DataToPlot(dat.x + thr_line_len);
        if(clr_ok) {
          t3gl->moveTo(th_st, (T3Color)(clr));
          t3gl->lineTo(th_ed, (T3Color)(clr));
        }
        else {
          t3gl->moveTo(th_st);
          t3gl->moveTo(th_ed);
        }
      }
    }
    else if(plot_style == THRESH_POINT) {
      if(yval >= thresh) {
        if(clr_ok)
          t3gl->markerAt(plt, (T3GraphLine::MarkerStyle)plv.point_style, (T3Color)(clr));
        else
          t3gl->markerAt(plt, (T3GraphLine::MarkerStyle)plv.point_style);
      }
    }

    // render marker, if any
    if((plot_style == POINTS) || (plot_style == LINE_AND_POINTS)) {
      if(row % point_spacing == 0) {
        if(clr_ok)
          t3gl->markerAt(plt, (T3GraphLine::MarkerStyle)plv.point_style, (T3Color)(clr));
        else
          t3gl->markerAt(plt, (T3GraphLine::MarkerStyle)plv.point_style);
      }
    }

    if(erv.on && da_er && (row % err_spacing == 0)) {
      float err_dat;
      if(da_er->is_matrix && mat_cell >= 0) {
        err_dat = da_er->GetValAsFloatM(row, mat_cell);
      }
      else {
        err_dat = da_er->GetValAsFloat(row);
      }
      float err_plt = err_dat;
      if(yax.range.Range() > 0.0f) err_plt /= yax.range.Range();

      if(clr_ok)
        t3gl->errBar(plt, err_plt, err_bar_width, (T3Color)(clr));
      else
        t3gl->errBar(plt, err_plt, err_bar_width);
    }

    if((label_spacing > 0) && (row % label_spacing == 0)) {
      String str = String(dat.y); // todo: could spec format..
      t3gl->textAt(iVec3f(plt.x,  plt.y - (.5f * label_font_size), plt.z),
                   str.chars());
    }
    // post draw updates:
    first = false;
    last_x = dat.x;
    last_z = dat.z;
  }
  t3gl->finishBatch();
}

void GraphTableView::PlotData_Bar(GraphPlotView& plv, GraphPlotView& erv, GraphPlotView& yax,
                                  T3GraphLine* t3gl, float bar_off, int mat_cell) {
  t3gl->clear();

  DataCol* da_y = plv.GetDAPtr();
  if(!da_y) return;
  DataCol* da_x = x_axis.GetDAPtr();
  DataCol* da_z = z_axis.GetDAPtr();

  DataTable* dt = dataTable();

  // this should only happen if we have no data at all..
  if((view_range.min < 0) || (view_range.min >= dt->rows)) return;
  if((view_range.min < 0) || (view_range.min >= dt->rows)) return;

  plv.eff_y_axis = &yax;                // set this for point clicking!

  t3gl->startBatch();
  t3gl->setLineStyle((T3GraphLine::LineStyle)plv.line_style, line_width);
  t3gl->setMarkerSize((T3GraphLine::MarkerSize)point_size);

  DataCol* da_er = erv.GetDAPtr();

  GraphAxisBase* ax_clr = NULL;
  DataCol* da_clr = NULL;
  if(color_mode == FIXED_COLOR) {
    t3gl->setValueColorMode(false);
    t3gl->setDefaultColor((T3Color)(plv.color.color()));
  }
  else {
    t3gl->setValueColorMode(true);
    if(color_mode == VALUE_COLOR) {
      ax_clr = &yax;
      da_clr = plv.GetDAPtr();
    }
    else {
      ax_clr = &color_axis;
      da_clr = color_axis.GetDAPtr();
    }
    if(!da_clr) {               // fallback
      t3gl->setValueColorMode(false);
      t3gl->setDefaultColor((T3Color)(plv.color.color()));
    }
  }

  GraphAxisBase* ax_rst = NULL;
  DataCol* da_rst = NULL;
  if(graph_type == RASTER) {
    ax_rst = &raster_axis;
    da_rst = raster_axis.GetDAPtr();
  }

  bool matz = false;
  if((mat_cell >= 0) && (matrix_mode == Z_INDEX)) matz = true;

  float bar_wd_plt = bar_width * x_axis.axis_length / x_axis.range.Range();
  float bar_off_plt = bar_off * x_axis.axis_length / x_axis.range.Range();

  iVec3f dat;                   // data point
  iVec3f plt;                   // plot coords
  for (int row = view_range.min; row <= view_range.max; row++) {
    iColor clr; // only used for color modes
    bool clr_ok = false;
    if(x_axis.row_num)
      dat.x = row;
    else
      dat.x = da_x->GetValAsFloat(row);
    if(dat.x < x_axis.range.min || dat.x > x_axis.range.max) continue;
    if(z_axis.on) {
      if(z_axis.row_num)
        dat.z = row;
      else if(da_z)
        dat.z = da_z->GetValAsFloat(row);
      if(dat.z < z_axis.range.min || dat.z > z_axis.range.max) continue;
    }
    if((mat_cell >= 0) && (matrix_mode == Z_INDEX)) {
      dat.z = mat_cell;
    }
    if(mat_cell >= 0) {
      dat.y = da_y->GetValAsFloatM(row, mat_cell);
    }
    else {
      dat.y = da_y->GetValAsFloat(row);
    }
    if(dat.y < yax.range.min || dat.y > yax.range.max) continue;

    plt.x = x_axis.DataToPlot(dat.x);
    plt.y = yax.DataToPlot(dat.y);

    if(z_axis.on || matz)
      plt.z = z_axis.DataToPlot(dat.z);

    if(da_clr) {
      clr_ok = true;
      if(color_mode == VALUE_COLOR) {
        clr = GetValueColor(ax_clr, dat.y);
      }
      else {
        clr = GetValueColor(ax_clr, da_clr->GetValAsFloat(row));
      }
    }

    iVec3f pt;
    // draw the line
    if(clr_ok) {
      pt = plt;  pt.y = 0.0f;  pt.x += bar_off_plt;
      t3gl->moveTo(pt, (T3Color)(clr));
      pt.y = plt.y;
      t3gl->lineTo(pt, (T3Color)(clr));
      pt.x += bar_wd_plt;
      t3gl->lineTo(pt, (T3Color)(clr));
      pt.y = 0.0f;
      t3gl->lineTo(pt, (T3Color)(clr));
    }
    else {
      pt = plt;  pt.y = 0.0f;  pt.x += bar_off_plt;
      t3gl->moveTo(pt);
      pt.y = plt.y;
      t3gl->lineTo(pt);
      pt.x += bar_wd_plt;
      t3gl->lineTo(pt);
      pt.y = 0.0f;
      t3gl->lineTo(pt);
    }

    if(erv.on && da_er && (row % err_spacing == 0)) {
      float err_dat;
      if(da_er->is_matrix && mat_cell >= 0) {
        err_dat = da_er->GetValAsFloatM(row, mat_cell);
      }
      else {
        err_dat = da_er->GetValAsFloat(row);
      }
      float err_plt = err_dat;
      if(yax.range.Range() > 0.0f) err_plt /= yax.range.Range();

      pt = plt;  pt.x += bar_off_plt + bar_wd_plt * 0.5f;

      if(clr_ok)
        t3gl->errBar(pt, err_plt, err_bar_width, (T3Color)(clr));
      else
        t3gl->errBar(pt, err_plt, err_bar_width);
    }

    if((label_spacing > 0) && (row % label_spacing == 0)) {
      String str = String(dat.y); // todo: could spec format..
      t3gl->textAt(iVec3f(plt.x,  plt.y - (.5f * label_font_size), plt.z),
                   str.chars());
    }
  }
  t3gl->finishBatch();
}

void GraphTableView::PlotData_String(GraphPlotView& plv_str, GraphPlotView& plv_y, T3GraphLine* t3gl) {
  t3gl->clear();

  DataCol* da_y = plv_y.GetDAPtr();
  if(!da_y) return;
  DataCol* da_str = plv_str.GetDAPtr();
  if(!da_str) return;
  DataCol* da_x = x_axis.GetDAPtr();
  DataCol* da_z = z_axis.GetDAPtr();

  plv_str.eff_y_axis = &plv_y;          // set this for point clicking!

  // always plot in color assoc with plv_str, regardless of mode!
  t3gl->startBatch();
  t3gl->setValueColorMode(false);
  t3gl->setDefaultColor((T3Color)(plv_str.color.color()));

  iVec3f dat;                   // data point
  iVec3f plt;                   // plot coords
  for (int row = view_range.min; row <= view_range.max; row++) {
    if(x_axis.row_num)
      dat.x = row;
    else
      dat.x = da_x->GetValAsFloat(row);
    if(dat.x < x_axis.range.min || dat.x > x_axis.range.max) continue;
    if(z_axis.on) {
      if(z_axis.row_num)
        dat.z = row;
      else if(da_z)
        dat.z = da_z->GetValAsFloat(row);
      if(dat.z < z_axis.range.min || dat.z > z_axis.range.max) continue;
    }
    dat.y = da_y->GetValAsFloat(row);
    if(dat.y < plv_y.range.min || dat.y > plv_y.range.max) continue;

    plt.x = x_axis.DataToPlot(dat.x);
    plt.y = plv_y.DataToPlot(dat.y);
    if(z_axis.on)
      plt.z = z_axis.DataToPlot(dat.z);

    String str = da_str->GetValAsString(row);
    if(!str.empty()) {
      t3gl->textAt(iVec3f(plt.x,  plt.y - (.5f * label_font_size), plt.z),
                   str.chars());
    }
  }
  t3gl->finishBatch();
}

////////////////////////////////////////////////////////////////////////
// note on following: basically callbacks from view


void GraphTableView::SetColorSpec(ColorScaleSpec* color_spec) {
  colorscale.SetColorSpec(color_spec);
  UpdateDisplay(true);
}

void GraphTableView::setWidth(float wdth) {
  width = wdth;
  T3GraphViewNode* node_so = this->node_so();
  if (!node_so) return;
  node_so->setWidth(width);
  UpdateDisplay(true);
}

void GraphTableView::setScaleData(bool auto_scale_, float min_, float max_) {
  if ((colorscale.auto_scale == auto_scale_) && (colorscale.min == min_) && (colorscale.max == max_)) return;
  colorscale.auto_scale = auto_scale_;
  if(!colorscale.auto_scale)
    colorscale.SetMinMax(min_, max_);
  UpdateDisplay(true);
}



// callback for view transformer dragger
void T3GraphViewNode_DragFinishCB(void* userData, SoDragger* dragr) {
  SoTransformBoxDragger* dragger = (SoTransformBoxDragger*)dragr;
  T3GraphViewNode* vnd = (T3GraphViewNode*)userData;
  GraphTableView* nv = static_cast<GraphTableView*>(vnd->dataView());

  SbRotation cur_rot;
  cur_rot.setValue(SbVec3f(nv->main_xform.rotate.x, nv->main_xform.rotate.y,
                           nv->main_xform.rotate.z), nv->main_xform.rotate.rot);

  SbVec3f trans = dragger->translation.getValue();
  cur_rot.multVec(trans, trans); // rotate translation by current rotation
  trans[0] *= nv->main_xform.scale.x;
  trans[1] *= nv->main_xform.scale.y;
  trans[2] *= nv->main_xform.scale.z;
  taVector3f tr(trans[0], trans[1], trans[2]);
  nv->main_xform.translate += tr;

  const SbVec3f& scale = dragger->scaleFactor.getValue();
  taVector3f sc(scale[0], scale[1], scale[2]);
  if(sc < .1f) sc = .1f;        // prevent scale from going to small too fast!!
  nv->main_xform.scale *= sc;

  SbVec3f axis;
  float angle;
  dragger->rotation.getValue(axis, angle);
  if(axis[0] != 0.0f || axis[1] != 0.0f || axis[2] != 1.0f || angle != 0.0f) {
    SbRotation rot;
    rot.setValue(SbVec3f(axis[0], axis[1], axis[2]), angle);
    SbRotation nw_rot = rot * cur_rot;
    nw_rot.getValue(axis, angle);
    nv->main_xform.rotate.SetXYZR(axis[0], axis[1], axis[2], angle);
  }

  vnd->txfm_shape()->scaleFactor.setValue(1.0f, 1.0f, 1.0f);
  vnd->txfm_shape()->rotation.setValue(SbVec3f(0.0f, 0.0f, 1.0f), 0.0f);

  float frmg = T3GraphViewNode::frame_margin;
  float frmg2 = 2.0f * frmg;
  float wdth = vnd->getWidth();

  vnd->txfm_shape()->translation.setValue(.5f * wdth, .5f * (1.0f + frmg2), 0.0f);

  dragger->translation.setValue(0.0f, 0.0f, 0.0f);
  dragger->rotation.setValue(SbVec3f(0.0f, 0.0f, 1.0f), 0.0f);
  dragger->scaleFactor.setValue(1.0f, 1.0f, 1.0f);

  nv->UpdateDisplay();
}
