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
#include <iViewPanelOfGraphTable>
#include <taSigLinkItr>
#include <T3Panel>
#include <MainWindowViewer>
#include <NewViewHelper>
#include <T3ExaminerViewer>
#include <T3GraphLine>
#include <T3GraphBar>
#include <T3GraphViewNode>
#include <iT3Panel>
#include <T3Axis>
#include <T3Color>
#include <taVector2i>
#include <taSvg>
#include <float_Matrix>

#if (QT_VERSION >= 0x050000)
#include <QGuiApplication>
#endif

#ifdef TA_QT3D

#include <T3LineStrip>
#include <T3TwoDText>
#include <T3LineBox>

#else // TA_QT3D

#include <SoLineBox3d>
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

#endif // TA_QT3D

TA_BASEFUNS_CTORS_DEFN(GraphTableView);

// #define UNIT_LEGEND_OFFSET 0.04f // space between end of axis and unit legend text
#define TICK_OFFSET 0.01f // gap between tick and label

float GraphTableView::tick_size = 0.05f;


GraphTableView* DataTable::NewGraphView(T3Panel* fr) {
  return GraphTableView::New(this, fr);
}

GraphTableView* DataTable::FindMakeGraphView(T3Panel* fr) {
  taSigLink* dl = sig_link();
  if(dl) {
    taSigLinkItr itr;
    GraphTableView* el;
    FOR_DLC_EL_OF_TYPE(GraphTableView, el, dl, itr) {
      // update from user stuff
      el->UpdateFromDataTable(true); // pretend its the first time -- initfromuserdata
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
  taSigLink* dl = sig_link();
  if(dl) {
    taSigLinkItr itr;
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

bool DataTable::GraphViewDefaultStyles() {
  GraphTableView* gv = FindGraphView();
  if(!gv) return false;
  gv->CallFun("DefaultPlotStyles");
  return true;
}

bool DataTable::GraphViewLineStyle() {
  GraphTableView* gv = FindGraphView();
  if(!gv) return false;
  gv->CallFun("SetLineStyle");
  return true;
}

// Add a new GraphTableView object to the frame for the given DataTable.
GraphTableView* GraphTableView::New(DataTable* dt, T3Panel*& fr) {
  NewViewHelper new_net_view(fr, dt, "table");
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
  tot_plots = 6;
  
  x_axis.on = false;
  x_axis.axis = GraphAxisBase::X;
  x_axis.color.name = taMisc::t3d_text_color;
  x_axis.color.UpdateAfterEdit_NoGui(); // needed to pick up color name
  z_axis.on = false;
  z_axis.axis = GraphAxisBase::Z;
  z_axis.color.name = taMisc::t3d_text_color;
  z_axis.color.UpdateAfterEdit_NoGui(); // needed to pick up color name
  
  graph_type = XY;
  plot_style = LINE;
  line_width = 1.0f;
  dev_pix_ratio = 1.0f;
  point_size = .01f;
  point_spacing = 1;
  bar_space = .2f;
  bar_depth = 0.01f;
  color_mode = FIXED;
  negative_draw = false;
  negative_draw_z = true;
  axis_font_size = .05f;
  label_font_size = .04f;
  label_spacing = -1;
  matrix_mode = SEP_GRAPHS;
  mat_layout = taMisc::BOT_ZERO;
  mat_odd_vert = true;
  scrolling_ = false;
  
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
  colorscale.SetMinMax(-1.0f, 1.0f);
  
  children.SetBaseType(&TA_GraphColView); // subclasses need to set this to their type!
  
  first_mat = -1;
  graphs_n = 0;
  
  n_plots = 1;
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
  taBase::Own(plots, this);
  taBase::Own(errbars, this);
  
  taBase::Own(color_axis, this);
  taBase::Own(raster_axis, this);
  taBase::Own(colorscale, this);
  taBase::Own(last_sel_pt, this);
  
  taBase::Own(main_y_plots, this);
  taBase::Own(alt_y_plots, this);
}

void GraphTableView::CutLinks() {
  x_axis.CutLinks();
  z_axis.CutLinks();
  color_axis.CutLinks();
  raster_axis.CutLinks();
  colorscale.CutLinks();
  
  plots.CutLinks();
  errbars.CutLinks();
  
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
  tot_plots = cp->tot_plots;
  plots.SetSize(cp->plots.size);
  errbars.SetSize(cp->plots.size);
  for(int i=0;i<plots.size;i++) {
    plots[i]->CopyFromView(cp->plots[i]);
    errbars[i]->CopyFromView(cp->errbars[i]);
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

void GraphTableView::AllocPlotData() {
  if(plots.size != tot_plots) {
    errbars.SetSize(tot_plots);     // always keep sync'd
    plots.SetSize(tot_plots);
    DefaultPlotStyles();
  }
}

void GraphTableView::UpdateAfterEdit_impl(){
  inherited::UpdateAfterEdit_impl();
  
  AllocPlotData();
  
  if(point_size <= 0.0f)
    point_size = 0.01f;
  
  if(taMisc::is_loading) {
    return;                     // end of loading stuff
  }
  
  point_spacing = MAX(1, point_spacing);
  
  x_axis.axis_length = width;
  z_axis.axis_length = depth;
  
  x_axis.on = true;             // try to keep it on
  x_axis.UpdateOnFlag();
  x_axis.UpdateFmDataCol();
  
  // taMisc::DebugInfo("UAE x_axis is now:", x_axis.col_name, "on:", String(x_axis.on));
  
  z_axis.UpdateOnFlag();
  z_axis.UpdateFmDataCol();
  for(int i=0;i<plots.size;i++) {
    GraphPlotView* pl = plots[i];
    pl->name = name + "_plot_" + String(i+1);
    pl->UpdateOnFlag();
    pl->UpdateFmDataCol();
    if(i == 0) {
      // taMisc::DebugInfo("UAE plot_1 is now:", pl->col_name, "on:", String(pl->on));
    }
    GraphPlotView* epl = errbars[i];
    epl->name = name + "_err_" + String(i+1);
    epl->UpdateOnFlag();
    epl->UpdateFmDataCol();
    epl->axis = GraphAxisBase::Y;
  }
  color_axis.UpdateOnFlag();
  color_axis.UpdateFmDataCol();
  raster_axis.UpdateOnFlag();
  raster_axis.UpdateFmDataCol();
  
  DataTable* dt = dataTable();
  bool no_cols = true;
  if(dt && dt->data.size > 0)
    no_cols = false;
  
  ////////////////////// update y axes in use, etc
  
  main_y_plots.Reset();
  alt_y_plots.Reset();
  n_plots = 0;
  first_mat = -1;
  for(int i=0;i<plots.size;i++) {
    GraphPlotView* pl = plots[i];
    if(!pl->on) continue;
    n_plots++;
    if(pl->alt_y) alt_y_plots.Add(i);
    else          main_y_plots.Add(i);
    GraphColView* plc = plots[i]->GetColPtr();
    if(graph_type == MATRIX) {
      if(plc->dataCol()->is_matrix) {
        if(first_mat < 0) first_mat = i;
      }
    }
  }
  if(graph_type == MATRIX && n_plots > 1) {
    n_plots = 1;
    taMisc::Warning("GraphTableView -- for MATRIX graph type, only one matrix column can be plotted -- others being turned off");
    main_y_plots.Reset();
    alt_y_plots.Reset();
    for(int i=0;i<plots.size;i++) {
      GraphPlotView* pl = plots[i];
      if(!pl->on) continue;
      if(i == first_mat) continue;
      pl->on = false;           // turn off
      if(pl->alt_y) alt_y_plots.RemoveIdx(i);
      else          main_y_plots.RemoveIdx(i);
    }
  }
  
  if(main_y_plots.size > 0) {
    GraphPlotView* pl = plots[main_y_plots[0]];
    if(pl->isString()) {
      int non_str = -1;
      for(int i=1; i< main_y_plots.size; i++) {
        if(!plots[main_y_plots[i]]->isString()) {
          non_str = i;
          break;
        }
      }
      
      if(non_str == -1) {
        taMisc::Warning("GraphTableView -- can't have only a string value(s) plotted on main Y axis -- must also include a numeric column to provide Y axis values -- turning off plot!");
        for(int i=0; i< main_y_plots.size; i++) {
          plots[main_y_plots[i]]->on = false;
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
    GraphPlotView* pl = plots[alt_y_plots[0]];
    if(pl->isString()) {        // oops.. not again!
      int non_str = -1;
      for(int i=1; i< alt_y_plots.size; i++) {
        if(!plots[alt_y_plots[i]]->isString()) {
          non_str = i;
          break;
        }
      }
      
      if(non_str == -1) {
        taMisc::Warning("GraphTableView -- can't have only a string value(s) plotted on alt Y axis -- must also include a numeric column to provide Y axis values -- turning off plot!");
        for(int i=0; i< alt_y_plots.size; i++) {
          plots[alt_y_plots[i]]->on = false;
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
  
  ///////////////////////////////////////////////////////////
  // other axes
  
  if(!x_axis.on) {
    if(!no_cols && !x_axis.row_num) {
      taMisc::Warning("GraphTableView -- X axis is not on -- perhaps no valid col_name found for x_axis -- switching to row_num!");
      x_axis.row_num = true;
    }
    x_axis.on = true;
  }
  else {
    if(!no_cols && x_axis.isString() && !x_axis.row_num) {
      taMisc::Warning("GraphTableView -- X axis is a String -- switching to row_num!");
      x_axis.row_num = true;    // must be row num!
    }
  }
  
  if(!no_cols && z_axis.isString() && !z_axis.row_num) {
    taMisc::Warning("GraphTableView -- Z axis is a String -- switching to row_num!");
    z_axis.row_num = true;      // must be row num!
  }
  
  if(graph_type == RASTER) {
    if((plot_style != THRESH_LINE) && (plot_style !=  THRESH_POINT))
      color_mode = BY_VALUE;
    raster_axis.on = true;
    raster_axis.UpdateOnFlag();
    if(!raster_axis.on) {
      if(!no_cols) {
        taMisc::Warning("GraphTableView -- graph_type = RASTER and no valid col_name found for raster_axis -- nothing will be plotted!");
      }
    }
  }
  
  
  // 8/3/2015 (rohrlich - this warning is wrong - the plot is made
  // if you have BY_VARIABLE and a NULL color axis the colors are what you get with FIXED
  // if you have BY_GROUP and NULL the lines are all black or whatever is the first color of default list
  // so the plot is drawn, just not colored using a y_variable value to determine the color
#if 0
  if(color_mode == BY_VARIABLE || color_mode == BY_GROUP) {
    color_axis.on = true;
    color_axis.UpdateOnFlag();
    if(!color_axis.on) {
      if(!no_cols) {
        taMisc::Warning("GraphTableView -- color_mode = BY_VARIABLE and no valid col_name found for color_axis -- nothing will be plotted!");
      }
    }
  }
#endif

  if(color_mode == BY_GROUP) {
    if (!color_axis.group_by_initialized || last_color_axis.GetColPtr() != color_axis.GetColPtr()) {
      GraphColView* gcv = color_axis.GetColPtr();
      if (gcv) {
        DataCol* col = gcv->dataCol();
        if (col) {
          col->GetUniqueColumnValues(color_axis.group_by_values);
          color_axis.group_by_initialized = true;
          last_color_axis = color_axis;
        }
      }
    }
  }
}

GraphPlotView* GraphTableView::MainY() {
  if(main_y_plots.size > 0)
    return plots.SafeEl(main_y_plots[0]);
  return NULL;
}

GraphPlotView* GraphTableView::AltY() {
  if(alt_y_plots.size > 0)
    return plots.SafeEl(alt_y_plots[0]);
  return NULL;
}

void GraphTableView::DefaultPlotStyles(int start_y, int end_y) {
  const char* colors[] = {"black", "red", "blue", "green3", "purple", "orange", "brown", "chartreuse"};
  int n_colors = 8;
  const int styles[] = {GraphPlotView::CIRCLE, GraphPlotView::SQUARE,
    GraphPlotView::DIAMOND, GraphPlotView::TRIANGLE_UP,
    GraphPlotView::TRIANGLE_DN,
    GraphPlotView::PLUS, GraphPlotView::CROSS,
    GraphPlotView::STAR};
  int n_styles = 8;
  
  if(end_y < 0)
    end_y = plots.size+end_y+1;
  end_y = MIN(end_y, plots.size);
  
  for(int i=start_y-1; i<end_y; i++) {
    GraphPlotView* gpv = plots[i];
    gpv->color.setColorName(colors[i % n_colors]);
    gpv->point_style = (GraphPlotView::PointStyle)styles[i % n_styles];
    gpv->line_style = (GraphPlotView::LineStyle)(i / n_colors);
    gpv->UpdateAfterEdit_NoGui();
  }
}

void GraphTableView::SetLineStyle(GraphPlotView::LineStyle line_style,
                                  int start_y, int end_y) {
  if(end_y < 0)
    end_y = plots.size+end_y+1;
  end_y = MIN(end_y, plots.size);
  
  for(int i=start_y-1; i<end_y; i++) {
    GraphPlotView* gpv = plots[i];
    gpv->line_style = line_style;
    gpv->UpdateAfterEdit_NoGui();
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

#ifndef TA_QT3D

void GraphTableView_MouseCB(void* userData, SoEventCallback* ecb) {
  GraphTableView* nv = (GraphTableView*)userData;
  T3Panel* fr = nv->GetFrame();
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
#endif

void GraphTableView::Render_pre() {
  bool show_drag = manip_ctrl_on;
  T3ExaminerViewer* vw = GetViewer();
  if(vw) {
    vw->syncViewerMode();
    if(!vw->interactionModeOn())
      show_drag = false;
  }
  
#ifdef TA_QT3D
  setNode(new T3GraphViewNode(NULL, this, width, show_drag));

#else // TA_QT3D
  setNode(new T3GraphViewNode(this, width, show_drag));
  
  /*NOTES:
   1. Render_pre is only done once, or on STRUCT changes
   2. item should not take ownsership of a shared frame prop like bg color
   3. we can set the frame bg color to the default for graphs
   
   */
  
  SoEventCallback* ecb = new SoEventCallback;
  ecb->addEventCallback(SoMouseButtonEvent::getClassTypeId(), GraphTableView_MouseCB, this);
  node_so()->addChild(ecb);
#endif
  
  colorscale.SetColorSpec(colorscale.spec);  // Call set to force the saved color to be restored
  
  inherited::Render_pre();
}

void GraphTableView::Render_impl() {
  inherited::Render_impl();
  T3GraphViewNode* node_so = this->node_so(); // cache
  if(!node_so || !dataTable())
    return;
  
#if (QT_VERSION >= 0x050000)
  dev_pix_ratio = ((QGuiApplication*)QGuiApplication::instance())->devicePixelRatio();
#endif
  
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

#ifdef TA_QT3D
  // Render();                     // this is brute force and works, for now..
  // return;
  //  RemoveGraph();
#endif // TA_QT3D

  Render_impl();
}

void GraphTableView::SigRecvUpdateView_impl() {
  //   taMisc::Info("data update view");
  if(!display_on) return;
  int old_rows;
  int delta_rows = CheckRowsChanged(old_rows);
  
  if (!isVisible()) return;

#ifdef TA_QT3D
  // Render();                     // this is brute force and works, for now..
  // return;
  RemoveGraph();
#endif // TA_QT3D
  
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

#ifdef TA_QT3D
  // taMisc::ProcessEvents();
  RenderGraph();
#endif // TA_QT3D

}

void GraphTableView::ComputeAxisRanges() {
  UpdateAfterEdit_impl();
  
  GraphPlotView* mainy = MainY();
  GraphPlotView* alty = AltY();
  
  x_axis.ComputeRange();
  if(z_axis.on) {
    if(graph_type == MATRIX && matrix_mode == Z_INDEX && mainy) {
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
  }
  
  for(int i=0;i<plots.size;i++) {
    GraphPlotView* pl = plots[i];
    if(pl->on)
      pl->ComputeRange();
  }
  
  if(mainy) {
    for(int i=1;i<main_y_plots.size;i++) {
      GraphPlotView* pl = plots[main_y_plots[i]];
      if(pl->isString()) continue;
      mainy->UpdateRange_impl(pl->data_range.min, pl->data_range.max);
    }
  }
  
  if(alty) {
    for(int i=1;i<alt_y_plots.size;i++) {
      GraphPlotView* pl = plots[alt_y_plots[i]];
      if(pl->isString()) continue;
      alty->UpdateRange_impl(pl->data_range.min, pl->data_range.max);
    }
  }
  
  if(color_mode == BY_VARIABLE) {
    color_axis.ComputeRange();
  }
  else if(color_mode == BY_GROUP) {
    // the range is the number of groups
    color_axis.SetRange(0, color_axis.group_by_values.size - 1);
  }
  if(graph_type == RASTER) {
    raster_axis.ComputeRange();
  }
}

#ifndef TA_QT3D
void GraphTableView_RowScrollCB(SoScrollBar* sb, int val, void* user_data) {
  GraphTableView* gtv = (GraphTableView*)user_data;
  gtv->scrolling_ = true;
  gtv->ViewRow_At(val);
  gtv->scrolling_ = false;
}
#endif // TA_QT3D

void GraphTableView::SetScrollBars() {
  if(scrolling_) return;                     // don't redo if currently doing!
  T3GraphViewNode* node_so = this->node_so(); // cache
  if(!node_so) return;
  
#ifdef TA_QT3D

#else // TA_QT3D
  SoScrollBar* rsb = node_so->RowScrollBar();
  //  rsb->setMinimum(0);
  //  rsb->setSingleStep(1);
  int mx = MAX((rows() - view_rows), 0);
  rsb->setMaximum(mx);
  int pg_step = MAX(view_rows, 1);
  rsb->setPageStep(pg_step);
  rsb->setValue(MIN(view_range.min, mx));
  rsb->setValueChangedCB(GraphTableView_RowScrollCB, this);
#endif // TA_QT3D
}

void GraphTableView::Clear_impl() {
  RemoveGraph();
  inherited::Clear_impl();
}

void GraphTableView::OnWindowBind_impl(iT3Panel* vw) {
  inherited::OnWindowBind_impl(vw);
  if (!m_lvp) {
    m_lvp = new iViewPanelOfGraphTable(this);
    vw->RegisterPanel(m_lvp);
  }
}

void GraphTableView::RemoveGraph(){
  T3GraphViewNode* node_so = this->node_so();
  if (!node_so) return;
#ifdef TA_QT3D
  node_so->graphs->removeAllChildren();
  node_so->y_axes->removeAllChildren();
#else // TA_QT3D
  node_so->graphs()->removeAllChildren();
  node_so->y_axes()->removeAllChildren();
#endif // TA_QT3D
}

void GraphTableView::FindDefaultXZAxes() {
  // first, look for explicit flags
  bool set_x = x_axis.on;
  z_axis.on = false;            // assume we're not going to find it
  for(int i=children.size-1;i>=0;i--) {
    GraphColView* cvs = (GraphColView*)colView(i);
    DataCol* da = cvs->dataCol();
    if(da->HasUserData("X_AXIS")) {
      x_axis.col_name = cvs->name;
      x_axis.InitFromUserData();
      x_axis.row_num = false;
      x_axis.on = true;
      set_x = true;
      // taMisc::DebugInfo("set X to:", x_axis.col_name);
    }
    if(da->HasUserData("Z_AXIS")) {
      z_axis.col_name = cvs->name;
      z_axis.on = false;  // don't turn on by default - most graphs are 2D
      z_axis.InitFromUserData();
      z_axis.row_num = false;
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
      z_axis.on = false;  // don't turn on by default - most graphs are 2D
      z_axis.InitFromUserData();
      break;                    // done
    }
    else {
      x_axis.col_name = cvs->name;
      x_axis.InitFromUserData();
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
      set_x = true;
      break;
    }
  }
}

void GraphTableView::FindDefaultPlot1() {
  bool got_1 = plots[0]->on;
  for(int i=children.size-1;i>=0;i--) {
    GraphColView* cvs = (GraphColView*)colView(i);
    DataCol* da = cvs->dataCol();
    if(da->HasUserData("PLOT_1")) {
      plots[0]->col_name = cvs->name;
      plots[0]->on = true;
      plots[0]->InitFromUserData();
      // taMisc::DebugInfo("set plot 1 to:", plots[0]->col_name);
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
    plots[0]->col_name = cvs->name;
    plots[0]->on = true;
    plots[0]->InitFromUserData();
    break;                      // once you get one, that's it
  }
}


void GraphTableView::InitFromUserData() {
  AllocPlotData();
  FindDefaultXZAxes();
  FindDefaultPlot1();
  
  // next, find first float/double and that is the plot_1
  for(int i=0;i<children.size;i++) {
    GraphColView* cvs = (GraphColView*)colView(i);
    DataCol* da = cvs->dataCol();
    for(int i=1; i<plots.size; i++) {
      String pltst = "PLOT_" + String(i+1);
      if(da->HasUserData(pltst)) {
        plots[i]->col_name = cvs->name;  plots[i]->on = true;
        plots[i]->InitFromUserData();
        // taMisc::DebugInfo("init fm set:", pltst, "to:", plots[i]->col_name);
      }
    }
    for(int i=0; i<errbars.size; i++) {
      String pltst = "ERR_" + String(i+1);
      if(da->HasUserData(pltst)) {
        errbars[i]->col_name = cvs->name;  errbars[i]->on = true;
        errbars[i]->InitFromUserData();
      }
    }
    if(da->HasUserData("BY_VARIABLE")) {
      color_axis.col_name = cvs->name;   color_axis.on = true;
      color_axis.InitFromUserData();
    }
    if(da->HasUserData("RASTER_AXIS")) {
      raster_axis.col_name = cvs->name;  raster_axis.on = true;
      raster_axis.InitFromUserData();
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
    point_size = dt->GetUserDataAsFloat("POINT_SIZE");
  }
  if(dt->HasUserData("COLOR_MODE")) {
    int pstv = GetEnumVal(dt->GetUserDataAsString("COLOR_MODE"), enum_tp_nm);
    if(pstv >= 0)
      color_mode = (ColorMode)pstv;
  }
  if(dt->HasUserData("MATRIX_MODE")) {
    int pstv = GetEnumVal(dt->GetUserDataAsString("MATRIX_MODE"), enum_tp_nm);
    if(pstv >= 0) {
      graph_type = MATRIX;
      matrix_mode = (MatrixMode)pstv;
    }
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
  UpdateAfterEdit_impl();
}

void GraphTableView::UpdateFromDataTable_this(bool first) {
  inherited::UpdateFromDataTable_this(first);
  if(!first) {
    // Get unique values from group by column
    GraphColView* gcv = color_axis.GetColPtr();
    if (gcv) {
      DataCol* col = gcv->dataCol();
      if (col) {
        col->GetUniqueColumnValues(color_axis.group_by_values);
      }
    }
    UpdateAfterEdit_impl();
    return;
  }
  InitFromUserData();           // need to init before UAE
  
  UpdateAfterEdit_impl();
}

void GraphTableView::DataUnitsXForm(taVector3f& pos, taVector3f& size) {
  GraphPlotView* mainy = MainY();
  GraphPlotView* alty = AltY();
  
  if(x_axis.on) {
    pos.x = x_axis.DataToPlot(pos.x);
    size.x = x_axis.DistToPlot(size.x);
  }
  if(z_axis.on) {
    pos.z = z_axis.DataToPlot(pos.z);
    size.z = z_axis.DistToPlot(size.z);
  }
  if(graph_type == RASTER) {
    pos.y = raster_axis.DataToPlot(pos.y);
    size.y = raster_axis.DistToPlot(size.y);
  }
  else if(mainy) {
    if(mainy->eff_y_axis) {
      pos.y = mainy->eff_y_axis->DataToPlot(pos.y);
      size.y = mainy->eff_y_axis->DistToPlot(size.y);
    }
    else {
      pos.y = mainy->DataToPlot(pos.y);
      size.y = mainy->DistToPlot(size.y);
    }
  }
}

void GraphTableView::SaveImageSVG(const String& svg_fname) {
  T3ExaminerViewer* vw = GetViewer();
  if(!vw) return;
  render_svg = true;
  svg_str = "";
  svg_str << taSvg::Header(vw, this);
  taSvg::cur_inst->coord_mult.z = -1.0f;
  RenderGraph();
  RenderAnnoteSvg();
  svg_str << taSvg::Footer();
  render_svg = false;
  svg_str.SaveToFile(svg_fname);
  RenderGraph();                // fix it..
}

///////////////////////////////////////////////////////////////
//      Actual Rendering of graph display

void GraphTableView::RenderGraph() {
  //   taMisc::Info("render graph");
  graphs_n = 0;
  UpdateAfterEdit_impl();
  if(n_plots == 0 || !x_axis.on) return;
  
  if(render_svg) {
    // do the overall box around graph
    svg_str << taSvg::Path(x_axis.color.color(), 2.0f)
    << "M " << taSvg::Coords(0.0f, 0.0f, 0.0f)
    << "L " << taSvg::Coords(0.0f, 1.0f, 0.0f)
    << "L " << taSvg::Coords(width, 1.0f, 0.0f)
    << "L " << taSvg::Coords(width, 0.0f, 0.0f)
    << "L " << taSvg::Coords(0.0f, 0.0f, 0.0f)
    << taSvg::PathEnd();
  }
  
  RenderAxes();
  
  GraphPlotView* mainy = MainY();
  GraphPlotView* alty = AltY();
  
  if(graph_type == MATRIX && mainy && mainy->GetDAPtr()->is_matrix) {
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
  
  GraphPlotView* mainy = MainY();
  GraphPlotView* alty = AltY();
  
  if(!x_axis.on || !mainy) return;
  
  String* rnd_svg = NULL;
  if(render_svg) {
    rnd_svg = &svg_str;
    *rnd_svg << taSvg::Group();
  }
  
#ifdef TA_QT3D
  T3Entity* xax = node_so->x_axis;
  xax->setNodeUpdating(true);
  T3Entity* zax = node_so->z_axis;
  zax->setNodeUpdating(true);
  T3Entity* yax = node_so->y_axes;
  yax->setNodeUpdating(true);

  t3_x_axis = xax->findChild<T3Axis*>("x_bot");
  if(!t3_x_axis) {
    t3_x_axis = new T3Axis(xax, &x_axis, (T3Axis::Axis)x_axis.axis, axis_font_size, width);
    t3_x_axis->setObjectName("x_bot");
    t3_x_axis_top = new T3Axis(xax, &x_axis, (T3Axis::Axis)x_axis.axis, axis_font_size,
                               width);
    t3_x_axis_top->setObjectName("x_top");
  }
  else {
    t3_x_axis_top = xax->findChild<T3Axis*>("x_top");
  }
  
#else // TA_QT3D
  SoSeparator* xax = node_so->x_axis();
  xax->removeAllChildren();
  SoSeparator* zax = node_so->z_axis();
  zax->removeAllChildren();
  SoSeparator* yax = node_so->y_axes();
  yax->removeAllChildren();
  SoTranslation* tr;

  t3_x_axis = new T3Axis((T3Axis::Axis)x_axis.axis, &x_axis, axis_font_size);
  t3_x_axis_top = new T3Axis((T3Axis::Axis)x_axis.axis, &x_axis, axis_font_size);
  xax->addChild(t3_x_axis);
  xax->addChild(t3_x_axis_top);
  
#endif // TA_QT3D
  float ylen = plots[0]->axis_length;
  
  x_axis.RenderAxis(t3_x_axis, iVec3f(0.0f, 0.0f, 0.0f), 0, false, rnd_svg);
  if(rnd_svg) {                 // svg needs separate ticks-only pass
    x_axis.RenderAxis(t3_x_axis, iVec3f(0.0f, 0.0f, 0.0f), 0, true, rnd_svg);
  }
  
  x_axis.RenderAxis(t3_x_axis_top, iVec3f(0.0f, ylen, 0.0f), 1, true, rnd_svg); // ticks only on top


  if(z_axis.on) {
#ifdef TA_QT3D
    t3_x_axis_far = xax->findChild<T3Axis*>("x_far_bot");
    if(!t3_x_axis_far) {
      t3_x_axis_far = new T3Axis(xax, &x_axis, (T3Axis::Axis)x_axis.axis, axis_font_size,
                                 width);
      t3_x_axis_far->setObjectName("x_far_bot");
      t3_x_axis_far_top = new T3Axis(xax, &x_axis, (T3Axis::Axis)x_axis.axis, 
                                     axis_font_size, width);
      t3_x_axis_far_top->setObjectName("x_far_top");

      /////////////  Z
      t3_z_axis = new T3Axis(zax, &z_axis, (T3Axis::Axis)z_axis.axis, axis_font_size,
                             width);
      t3_z_axis->setObjectName("z_bot");
      t3_z_axis_rt = new T3Axis(zax, &z_axis, (T3Axis::Axis)z_axis.axis, axis_font_size,
                                width);
      t3_z_axis_rt->setObjectName("z_rt");
      t3_z_axis_top = new T3Axis(zax, &z_axis, (T3Axis::Axis)z_axis.axis, axis_font_size,
                                 width);
      t3_z_axis_top->setObjectName("z_top");
      t3_z_axis_top_rt = new T3Axis(zax, &z_axis, (T3Axis::Axis)z_axis.axis,
                                    axis_font_size, width);
      t3_z_axis_top_rt->setObjectName("z_top_rt");
    }
    else {
      t3_x_axis_far_top = xax->findChild<T3Axis*>("x_far_top");
      t3_z_axis = zax->findChild<T3Axis*>("z_bot");
      t3_z_axis_rt = zax->findChild<T3Axis*>("z_rt");
      t3_z_axis_top = zax->findChild<T3Axis*>("z_top");
      t3_z_axis_top_rt = zax->findChild<T3Axis*>("z_top_rt");
    }
    
#else // TA_QT3D
    t3_x_axis_far = new T3Axis((T3Axis::Axis)x_axis.axis, &x_axis, axis_font_size);
    t3_x_axis_far_top = new T3Axis((T3Axis::Axis)x_axis.axis, &x_axis, axis_font_size);
    xax->addChild(t3_x_axis_far_top);
    xax->addChild(t3_x_axis_far);

    /////////////  Z
    t3_z_axis = new T3Axis((T3Axis::Axis)z_axis.axis, &z_axis, axis_font_size);
    t3_z_axis_rt = new T3Axis((T3Axis::Axis)z_axis.axis, &z_axis, axis_font_size);
    t3_z_axis_top = new T3Axis((T3Axis::Axis)z_axis.axis, &z_axis, axis_font_size);
    t3_z_axis_top_rt = new T3Axis((T3Axis::Axis)z_axis.axis, &z_axis, axis_font_size);
    
    zax->addChild(t3_z_axis);
    zax->addChild(t3_z_axis_rt);
    zax->addChild(t3_z_axis_top_rt);
    zax->addChild(t3_z_axis_top);

#endif // TA_QT3D
    
    x_axis.RenderAxis(t3_x_axis_far_top, iVec3f(0.0f, ylen, z_axis.axis_length), 0,
                      true, rnd_svg); // ticks only
    x_axis.RenderAxis(t3_x_axis_far, iVec3f(0.0f, 0.0f, z_axis.axis_length), 0,
                      true, rnd_svg); // ticks only
    
    z_axis.RenderAxis(t3_z_axis, iVec3f(0.0f, 0.0f, 0.0f), 0, false, rnd_svg);
    if(rnd_svg) {                 // svg needs separate ticks-only pass
      z_axis.RenderAxis(t3_z_axis, iVec3f(0.0f, 0.0f, 0.0f), 0, true, rnd_svg);
    }
    z_axis.RenderAxis(t3_z_axis_rt, iVec3f(x_axis.axis_length, 0.0f, 0.0f), 0,
                      true, rnd_svg); // ticks only
    z_axis.RenderAxis(t3_z_axis_top_rt, iVec3f(x_axis.axis_length, ylen, 0.0f), 0,
                      true, rnd_svg);
    z_axis.RenderAxis(t3_z_axis_top, iVec3f(0.0f, ylen, 0.0f), 0,
                      true, rnd_svg);
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
#ifdef TA_QT3D
    t3_y_axis = new T3Axis(yax, &raster_axis, (T3Axis::Axis)raster_axis.axis, 
                           axis_font_size, width);
#else // TA_QT3D
    t3_y_axis = new T3Axis((T3Axis::Axis)raster_axis.axis, &raster_axis, axis_font_size);
    yax->addChild(t3_y_axis);
#endif // TA_QT3D

    raster_axis.RenderAxis(t3_y_axis, iVec3f(0.0f, 0.0f, 0.0f), 0,
                           false, rnd_svg); // raster axis is Y axis!
    
  }
  else {
#ifdef TA_QT3D
    t3_y_axis = yax->findChild<T3Axis*>("y_main");
    if(!t3_y_axis) {
      t3_y_axis = new T3Axis(yax, mainy, (T3Axis::Axis)mainy->axis, axis_font_size, width);
      t3_y_axis->setObjectName("y_main");
    }
#else // TA_QT3D
    t3_y_axis = new T3Axis((T3Axis::Axis)mainy->axis, mainy, axis_font_size);
    yax->addChild(t3_y_axis);
#endif // TA_QT3D
    
    mainy->RenderAxis(t3_y_axis, iVec3f(0.0f, 0.0f, 0.0f), 0, false, rnd_svg);
    if(rnd_svg) {
      mainy->RenderAxis(t3_y_axis, iVec3f(0.0f, 0.0f, 0.0f), 0, true, rnd_svg);
    }
    
    if(z_axis.on) {
#ifdef TA_QT3D
      t3_y_axis_far = yax->findChild<T3Axis*>("y_main_far");
      if(!t3_y_axis_far) {
        t3_y_axis_far = new T3Axis(yax, mainy, (T3Axis::Axis)mainy->axis, axis_font_size,
                                   width);
        t3_y_axis_far->setObjectName("y_main_far");
      }
#else // TA_QT3D
      t3_y_axis_far = new T3Axis((T3Axis::Axis)mainy->axis, mainy, axis_font_size);
      yax->addChild(t3_y_axis_far);
#endif // TA_QT3D
      mainy->RenderAxis(t3_y_axis_far, iVec3f(0.0f, 0.0f, z_axis.axis_length),
                        0, true, rnd_svg); // only ticks
    }
    else {
      t3_y_axis_far = NULL;
    }
    
    if(alty) { // second Y = 1
#ifdef TA_QT3D
      t3_y_axis_rt = yax->findChild<T3Axis*>("y_alt");
      if(!t3_y_axis_rt) {
        t3_y_axis_rt = new T3Axis(yax, alty, (T3Axis::Axis)alty->axis, axis_font_size, 1);
        t3_y_axis_rt->setObjectName("y_alt");
      }
#else // TA_QT3D
      t3_y_axis_rt = new T3Axis((T3Axis::Axis)alty->axis, alty, axis_font_size, 1); 
      yax->addChild(t3_y_axis_rt);
#endif // TA_QT3D
      
      alty->RenderAxis(t3_y_axis_rt, iVec3f(x_axis.axis_length, 0.0f, 0.0f), 1,
                         false, rnd_svg); // indicate second axis!
      
      if(rnd_svg) {
        alty->RenderAxis(t3_y_axis_rt, iVec3f(x_axis.axis_length, 0.0f, 0.0f),
                         1, true, rnd_svg);
      }
      
      if(z_axis.on) {
#ifdef TA_QT3D
        t3_y_axis_far_rt = yax->findChild<T3Axis*>("y_alt_far");
        if(!t3_y_axis_far_rt) {
          t3_y_axis_far_rt = new T3Axis(yax, alty, (T3Axis::Axis)alty->axis,
                                        axis_font_size, 1);
          t3_y_axis_far_rt->setObjectName("y_alt_far");
        }
#else // TA_QT3D
        t3_y_axis_far_rt = new T3Axis((T3Axis::Axis)alty->axis, alty, axis_font_size, 1);
        yax->addChild(t3_y_axis_far_rt);
#endif // TA_QT3D
        
        alty->RenderAxis(t3_y_axis_far_rt,
                         iVec3f(x_axis.axis_length, 0.0f, z_axis.axis_length), 1,
                         true, rnd_svg); // only ticks
      }
      else {
        t3_y_axis_far_rt = NULL;
      }
    }
    else {
      // rt
#ifdef TA_QT3D
      t3_y_axis_rt = yax->findChild<T3Axis*>("y_main_rt");
      if(!t3_y_axis_rt) {
        t3_y_axis_rt = new T3Axis(yax, mainy, (T3Axis::Axis)mainy->axis, axis_font_size,
                                  width);
        t3_y_axis_rt->setObjectName("y_main_rt");
      }
#else // TA_QT3D
      t3_y_axis_rt = new T3Axis((T3Axis::Axis)mainy->axis, mainy, axis_font_size);
      yax->addChild(t3_y_axis_rt);
#endif // TA_QT3D
      
      mainy->RenderAxis(t3_y_axis_rt, iVec3f(x_axis.axis_length, 0.0f, 0.0f), 0,
                        true, rnd_svg); // ticks
      
      if(z_axis.on) {
#ifdef TA_QT3D
        t3_y_axis_far_rt = yax->findChild<T3Axis*>("y_main_far_rt");
        if(!t3_y_axis_far_rt) {
          t3_y_axis_far_rt = new T3Axis(yax, mainy, (T3Axis::Axis)mainy->axis, 
                                        axis_font_size, width);
          t3_y_axis_far_rt->setObjectName("y_main_far_rt");
        }
#else // TA_QT3D
        t3_y_axis_far_rt = new T3Axis((T3Axis::Axis)mainy->axis, mainy, axis_font_size);
        yax->addChild(t3_y_axis_far_rt);
#endif // TA_QT3D
        mainy->RenderAxis(t3_y_axis_far_rt,
                          iVec3f(x_axis.axis_length, 0.0f, z_axis.axis_length), 0,
                          true, rnd_svg); // only ticks
      }
      else {
        t3_y_axis_far_rt = NULL;
      }
    }
  }

#ifdef TA_QT3D
  xax->setNodeUpdating(false);
  yax->setNodeUpdating(false);
  zax->setNodeUpdating(false);
#endif // TA_QT3D

  if(render_svg) {
    rnd_svg = &svg_str;
    *rnd_svg << taSvg::GroupEnd();
  }
}

void GraphTableView::RenderLegend_Ln(GraphPlotView& plv, T3GraphLine* t3gl,
                                     taVector2f& cur_tr, int group) {
  t3gl->clear();
  t3gl->startBatch();
  t3gl->setLineStyle((T3GraphLine::LineStyle)plv.line_style, dev_pix_ratio * line_width);
  t3gl->setMarkerSize(point_size);
  t3gl->setValueColorMode(false);
  if (color_mode == BY_GROUP) {
    iColor clr = GetValueColor(&color_axis, group);
    t3gl->setDefaultColor(clr);
  }
  else {
    t3gl->setDefaultColor(plv.color.color());
  }
  
  String label;
  if (color_mode == BY_GROUP) {
    label = plv.group_by_values[group];
  }
  else {
    label = plv.col_name;
  }
  taMisc::SpaceLabel(label);
  
  iVec3f st;
  iVec3f ed;
  ed.x = 1.5f * label_font_size;
  t3gl->moveTo(st);
  t3gl->lineTo(ed);
#ifdef TA_QT3D
  t3gl->textAt(iVec3f(ed.x + TICK_OFFSET,  ed.y, ed.z), label.chars());
#else // TA_QT3D
  t3gl->textAt(iVec3f(ed.x + TICK_OFFSET,  ed.y - (.5f * label_font_size), ed.z),
               label.chars());
#endif // TA_QT3D
  t3gl->finishBatch();
  
  if(render_svg) {
    svg_str << taSvg::GroupTranslate(cur_tr.x, -cur_tr.y)
    << taSvg::Path(plv.color.color(), dev_pix_ratio * line_width)
    << "M " << taSvg::Coords(st)
    << "L " << taSvg::Coords(ed)
    << taSvg::PathEnd();
    svg_str << taSvg::Text(label, ed.x + TICK_OFFSET, ed.y - (.5f * label_font_size),
                           ed.z, plv.color.color(), label_font_size, taSvg::LEFT)
    << taSvg::GroupEnd();
  }
}

void GraphTableView::RenderLegend() {
  T3GraphViewNode* node_so = this->node_so();
  if (!node_so)
    return;
  
  float ylen = plots[0]->axis_length;

  int n_plots_eff = 0;
  int max_label_len = 5;
  if (color_mode == BY_GROUP) {
    n_plots_eff = color_axis.group_by_values.size;
    if (main_y_plots.size == 0) return;
    for (int group=0; group<color_axis.group_by_values.size; group++) {
      max_label_len = MAX(color_axis.group_by_values[group].length(), max_label_len);
    }
  }
  else {
    n_plots_eff = main_y_plots.size + alt_y_plots.size;
    for(int i=0;i<main_y_plots.size;i++) {
      GraphPlotView* pl = plots[main_y_plots[i]];
      max_label_len = MAX(pl->col_name.length(), max_label_len);
    }
  }
  if(n_plots_eff == 0)
    return;    // nothing..

  float tot_char_wd = (width * 2.0f) / label_font_size; // rough heuristic total
  int n_across = (int)(tot_char_wd / (float)(max_label_len + 5)); // add 5 for the line
  if(n_across < 1) n_across = 1;
  
  int n_down = (int)(((float)n_plots_eff / (float)n_across) + 0.5f);
  if(n_down < 1) n_down = 1;
  while(n_down * n_across < n_plots_eff)
    n_down++;
  
#ifdef TA_QT3D
  T3Entity* leg = node_so->legend;
  const QObjectList& leg_ol = leg->children();
  int leg_n = 0;
  leg->Translate
    (QVector3D(-0.5f * width, 0.5f * ylen + (0.3f + (float)n_down * 1.1f) * label_font_size,
               0.0f));
#else // TA_QT3D
  SoSeparator* leg = node_so->legend();
  leg->removeAllChildren();
  // move to top
  SoTranslation* tr;
  tr = new SoTranslation();  leg->addChild(tr);
  tr->translation.setValue(0.0f, ylen + (0.3f + (float)n_down * 1.1f) * label_font_size,
                           0.0f);
#endif // TA_QT3D
  
  if(render_svg) {
    svg_str << taSvg::GroupTranslate(0.0f, -(ylen + 2.5f * label_font_size));
  }
  
  float over_amt = (width / (float)n_across) * .9f * x_axis.axis_length;
  float dn_amt = -1.1f * label_font_size;
  int xpos = 0;
  int ypos = 0;
  int mv_dn = n_down;
  int lidx = 0;
  
  taVector2f cur_tr;
  
  if (color_mode == BY_GROUP) {
    for (int group=0; group<color_axis.group_by_values.size; group++) {
      GraphPlotView* pl = plots[main_y_plots[0]];
#ifdef TA_QT3D
      T3GraphLine* ln = NULL;
      if(leg_n < leg_ol.count()) {
        ln = dynamic_cast<T3GraphLine*>(leg_ol.at(leg_n++));
      }
      if(!ln) {
        ln = new T3GraphLine(leg, pl, label_font_size, width);
        leg_n++;
      }
      ln->clear();
#else // TA_QT3D
      T3GraphLine* ln = new T3GraphLine(pl, label_font_size);
      leg->addChild(ln);
#endif // TA_QT3D
      pl->group_by_values = color_axis.group_by_values;  // copy the group_by values
      RenderLegend_Ln(*pl, ln, cur_tr, group);
#ifdef TA_QT3D
      xpos = lidx / n_down;
      ypos = lidx % n_down;
      lidx++;
      cur_tr.SetXY(over_amt * xpos, dn_amt * ypos);
      ln->Translate(cur_tr.x, cur_tr.y, 0.0f);
#else // TA_QT3D
      tr = new SoTranslation();  leg->addChild(tr);
      if(mv_dn > 1) {
        tr->translation.setValue(0.0f, dn_amt, 0.0f);
        cur_tr.y += dn_amt;
        mv_dn--;
      }
      else {
        tr->translation.setValue(over_amt, -dn_amt * (n_down-1), 0.0f);
        cur_tr.x += over_amt; cur_tr.y -= dn_amt * (n_down-1);
        mv_dn = n_down;
      }
#endif // TA_QT3D
    }
    // no alt-y -- when coloring by group the legend would be the same for alt-Y so don't draw it
  }
  else {
    // keep in same order as plotting: main then alt
    for(int i=0;i<main_y_plots.size;i++) {
      GraphPlotView* pl = plots[main_y_plots[i]];
#ifdef TA_QT3D
      T3GraphLine* ln = NULL;
      if(leg_n < leg_ol.count()) {
        ln = dynamic_cast<T3GraphLine*>(leg_ol.at(leg_n++));
      }
      if(!ln) {
        ln = new T3GraphLine(leg, pl, label_font_size, width);
        leg_n++;
      }
      ln->clear();
#else // TA_QT3D
      T3GraphLine* ln = new T3GraphLine(pl, label_font_size);
      leg->addChild(ln);
#endif // TA_QT3D
      RenderLegend_Ln(*pl, ln, cur_tr);
#ifdef TA_QT3D
      xpos = lidx / n_down;
      ypos = lidx % n_down;
      lidx++;
      cur_tr.SetXY(over_amt * xpos, dn_amt * ypos);
      ln->Translate(cur_tr.x, cur_tr.y, 0.0f);
#else // TA_QT3D
      tr = new SoTranslation();  leg->addChild(tr);
      if(mv_dn > 1) {
        tr->translation.setValue(0.0f, dn_amt, 0.0f);
        cur_tr.y += dn_amt;
        mv_dn--;
      }
      else {
        tr->translation.setValue(over_amt, -dn_amt * (n_down-1), 0.0f);
        cur_tr.x += over_amt; cur_tr.y -= dn_amt * (n_down-1);
        mv_dn = n_down;
      }
#endif // TA_QT3D
    }
    
    for(int i=0;i<alt_y_plots.size;i++) {
      GraphPlotView* pl = plots[alt_y_plots[i]];
#ifdef TA_QT3D
      T3GraphLine* ln = NULL;
      if(leg_n < leg_ol.count()) {
        ln = dynamic_cast<T3GraphLine*>(leg_ol.at(leg_n++));
      }
      if(!ln) {
        ln = new T3GraphLine(leg, pl, label_font_size, width);
        leg_n++;
      }
      ln->clear();
#else // TA_QT3D
      T3GraphLine* ln = new T3GraphLine(pl, label_font_size);
      leg->addChild(ln);
#endif // TA_QT3D
      RenderLegend_Ln(*pl, ln, cur_tr);
#ifdef TA_QT3D
      xpos = lidx / n_down;
      ypos = lidx % n_down;
      lidx++;
      cur_tr.SetXY(over_amt * xpos, dn_amt * ypos);
      ln->Translate(cur_tr.x, cur_tr.y, 0.0f);
#else // TA_QT3D
      tr = new SoTranslation();  leg->addChild(tr);
      if(mv_dn > 1) {
        tr->translation.setValue(0.0f, dn_amt, 0.0f);
        cur_tr.y += dn_amt;
        mv_dn--;
      }
      else {
        tr->translation.setValue(over_amt, -dn_amt * (n_down-1), 0.0f);
        cur_tr.x += over_amt; cur_tr.y -= dn_amt * (n_down-1);
        mv_dn = n_down;
      }
#endif // TA_QT3D
    }
  }
  
  if(render_svg) {
    svg_str << taSvg::GroupEnd();
  }
}

void GraphTableView::RenderGraph_XY() {
  T3GraphViewNode* node_so = this->node_so();
  if (!node_so) return;
  
  GraphPlotView* mainy = MainY();
  GraphPlotView* alty = AltY();
  
  if(!mainy) return;
  
  DataCol* da_1 = mainy->GetDAPtr();
  if(!da_1) return;
  
  float boxd = 1.0e-6f;
  if(z_axis.on)
    boxd = depth;
  
#ifdef TA_QT3D
  const QObjectList& gr_ol = node_so->graphs->children();
  T3Entity* gr1 = NULL;
  if(graphs_n < gr_ol.count()) {
    gr1 = dynamic_cast<T3Entity*>(gr_ol.at(graphs_n++));
  }
  if(!gr1) {
    gr1 = new T3Entity(node_so->graphs);
  }
  const QObjectList& gr1_ol = gr1->children();
  T3LineBox* lbox = NULL;
  if(gr1_ol.count() == 1) {
    lbox = dynamic_cast<T3LineBox*>(gr1_ol.at(0));
  }
  if(!lbox) {
    lbox = new T3LineBox(gr1, QVector3D(width, 1.0f, boxd));
  }
  if(z_axis.on) {
    lbox->TranslateZFrontTo(QVector3D(0,0,0));
  }
#else // TA_QT3D
  SoSeparator* graphs = node_so->graphs();
  graphs->removeAllChildren();	// this is the "nuclear option" that ensures full redraw
  
  SoSeparator* gr1 = new SoSeparator;
  graphs->addChild(gr1);
  
  // each graph has a box and lines..
  SoLineBox3d* lbox = new SoLineBox3d(width, 1.0f, boxd, false); // not centered
  gr1->addChild(lbox);
#endif // TA_QT3D
  
  if(render_svg) {              // group the lines in a box
    svg_str << "\n<g>\n";
  }
  
  for(int i=0;i<main_y_plots.size;i++) {
    GraphPlotView* pl = plots[main_y_plots[i]];
#ifdef TA_QT3D
    T3GraphLine* ln = new T3GraphLine(lbox, pl, label_font_size, width, z_axis.on);
#else // TA_QT3D
    T3GraphLine* ln = new T3GraphLine(pl, label_font_size);
    gr1->addChild(ln);
#endif // TA_QT3D
    if(pl->isString()) {
      PlotData_String(*pl, *mainy, ln);
    }
    else {
      if(pl->is_matrix && pl->matrix_cell < 0) {
        for(int ci = 0; ci < pl->n_cells; ci++) {
          int clr_inc = colorscale.chunks / pl->n_cells;
          clr_inc = MAX(1, clr_inc);
          int clr_idx = (ci * clr_inc) % colorscale.chunks;
          PlotData_XY(*pl, *errbars[main_y_plots[i]], *mainy, ln, ci, clr_idx);
        }
      }
      else {
        PlotData_XY(*pl, *errbars[main_y_plots[i]], *mainy, ln);
      }
    }
  }
  
  for(int i=0;i<alt_y_plots.size;i++) {
    GraphPlotView* pl = plots[alt_y_plots[i]];
#ifdef TA_QT3D
    T3GraphLine* ln = new T3GraphLine(lbox, pl, label_font_size, width, z_axis.on);
#else // TA_QT3D
    T3GraphLine* ln = new T3GraphLine(pl, label_font_size);
    gr1->addChild(ln);
#endif // TA_QT3D
    if(pl->isString()) {
      PlotData_String(*pl, *alty, ln);
    }
    else {
      if(pl->is_matrix && pl->matrix_cell < 0) {
        for(int ci = 0; ci < pl->n_cells; ci++) {
          int clr_inc = colorscale.chunks / pl->n_cells;
          clr_inc = MAX(1, clr_inc);
          int clr_idx = (ci * clr_inc) % colorscale.chunks;
          PlotData_XY(*pl, *errbars[alt_y_plots[i]], *alty, ln, ci, clr_idx);
        }
      }
      else {
        PlotData_XY(*pl, *errbars[alt_y_plots[i]], *alty, ln);
      }
    }
  }
  
  if(render_svg) {
    svg_str << "\n</g>\n";
  }
}

void GraphTableView::RenderGraph_Bar() {
  T3GraphViewNode* node_so = this->node_so();
  if (!node_so) return;
  
  GraphPlotView* mainy = MainY();
  GraphPlotView* alty = AltY();
  
  if(n_plots <= 0 || !mainy) return;
  
  DataCol* da_1 = mainy->GetDAPtr();
  if(!da_1) return;
  
  float boxd = 0.0f;
  if(z_axis.on)
    boxd = depth;
  
#ifdef TA_QT3D
  node_so->graphs->removeAllChildren(); // todo: try to re-use instead of destroy!
  T3Entity* gr1 = new T3Entity(node_so->graphs);
  T3LineBox* lbox = new T3LineBox(gr1, QVector3D(width, 1.0f, boxd));
  if(z_axis.on) {
    lbox->TranslateZFrontTo(QVector3D(0,0,0));
  }
#else // TA_QT3D
  SoSeparator* graphs = node_so->graphs();
  graphs->removeAllChildren();
  
  SoSeparator* gr1 = new SoSeparator;
  graphs->addChild(gr1);
  
  // each graph has a box and lines..
  SoLineBox3d* lbox = new SoLineBox3d(width, 1.0f, boxd, false); // not centered
  gr1->addChild(lbox);

#endif // TA_QT3D
  
  int n_tot = 0;
  for(int i=0;i<plots.size;i++) {
    GraphPlotView* pl = plots[i];
    if(!pl->on || pl->is_string) continue;
    if(pl->is_matrix && pl->matrix_cell < 0) {
      n_tot += pl->n_cells + 1; // one extra space btwn groups
    }
    else {
      n_tot++;
    }
  }
  
  if(n_tot == 0) return;
  
  bar_width = (1.0f - bar_space) / ((float)n_tot);
  float bar_off = - .5f * (1.0f - bar_space);
  
  for(int i=0;i<main_y_plots.size;i++) {
    GraphPlotView* pl = plots[main_y_plots[i]];
#ifdef TA_QT3D
    T3GraphLine* ln = new T3GraphLine(lbox, pl, label_font_size, width, z_axis.on);
#else // TA_QT3D
    T3GraphLine* ln = new T3GraphLine(pl, label_font_size);
    gr1->addChild(ln);
#endif // TA_QT3D
    if(pl->isString()) {
      PlotData_String(*pl, *mainy, ln);
    }
    else {
      if(pl->is_matrix && pl->matrix_cell < 0) {
        for(int ci = 0; ci < pl->n_cells; ci++) {
          int clr_inc = colorscale.chunks / pl->n_cells;
          clr_inc = MAX(1, clr_inc);
          int clr_idx = (ci * clr_inc) % colorscale.chunks;
          PlotData_Bar(gr1, *pl, *errbars[main_y_plots[i]], *mainy, ln, bar_off, ci,
                       clr_idx);
          bar_off += bar_width;
        }
      }
      else {
        PlotData_Bar(gr1, *pl, *errbars[main_y_plots[i]], *mainy, ln, bar_off);
      }
      bar_off += bar_width;
    }
  }
  
  for(int i=0;i<alt_y_plots.size;i++) {
    GraphPlotView* pl = plots[alt_y_plots[i]];
#ifdef TA_QT3D
    T3GraphLine* ln = new T3GraphLine(lbox, pl, label_font_size, width, z_axis.on);
#else // TA_QT3D
    T3GraphLine* ln = new T3GraphLine(pl, label_font_size);
    gr1->addChild(ln);
#endif // TA_QT3D
    if(pl->isString()) {
      PlotData_String(*pl, *alty, ln);
    }
    else {
      if(pl->is_matrix && pl->matrix_cell < 0) {
        for(int ci = 0; ci < pl->n_cells; ci++) {
          int clr_inc = colorscale.chunks / pl->n_cells;
          clr_inc = MAX(1, clr_inc);
          int clr_idx = (ci * clr_inc) % colorscale.chunks;
          PlotData_Bar(gr1, *pl, *errbars[alt_y_plots[i]], *alty, ln, bar_off, ci, clr_idx);
          bar_off += bar_width;
        }
      }
      else {
        PlotData_Bar(gr1, *pl, *errbars[alt_y_plots[i]], *alty, ln, bar_off);
      }
      bar_off += bar_width;
    }
  }
}

void GraphTableView::RenderGraph_Matrix_Zi() {
  T3GraphViewNode* node_so = this->node_so();
  if (!node_so) return;
  
  GraphPlotView* mainy = MainY();
  GraphPlotView* alty = AltY();
  if(!mainy) return;
  
  DataCol* da_1 = mainy->GetDAPtr();
  if(!da_1) return;
  
#ifdef TA_QT3D
  node_so->graphs->removeAllChildren(); // todo: try to re-use instead of destroy!
  T3Entity* gr1 = new T3Entity(node_so->graphs);
  T3LineBox* lbox = new T3LineBox(gr1, QVector3D(width, 1.0f, depth));
  lbox->TranslateZFrontTo(QVector3D(0,0,0));
#else // TA_QT3D
  SoSeparator* graphs = node_so->graphs();
  graphs->removeAllChildren();
  
  SoSeparator* gr1 = new SoSeparator;
  graphs->addChild(gr1);
  
  // each graph has a box and lines..
  SoLineBox3d* lbox = new SoLineBox3d(width, 1.0f, depth, false); // not centered
  gr1->addChild(lbox);
#endif // TA_QT3D
  
  for(int i=0;i<da_1->cell_size();i++) {
#ifdef TA_QT3D
    T3GraphLine* ln = new T3GraphLine(lbox, mainy, label_font_size, width, true);
#else // TA_QT3D
    T3GraphLine* ln = new T3GraphLine(mainy, label_font_size);
    gr1->addChild(ln);
#endif // TA_QT3D
    PlotData_XY(*mainy, *errbars[main_y_plots[0]], *mainy, ln, i);
  }

}

void GraphTableView::RenderGraph_Matrix_Sep() {
  T3GraphViewNode* node_so = this->node_so();
  if (!node_so) return;
  
  GraphPlotView* mainy = MainY();
  GraphPlotView* alty = AltY();
  if(!mainy) return;
  
  DataCol* da_1 = mainy->GetDAPtr();
  if(!da_1) return;
  
  GraphPlotView* erry = errbars[main_y_plots[0]];
  
  float boxd = .01f;
  if(z_axis.on)
    boxd = depth;
  
  MatrixGeom& mgeom = da_1->cell_geom;
  
  int geom_x, geom_y;
  mgeom.Get2DGeomGui(geom_x, geom_y, mat_odd_vert, 1);
  float cl_x = 1.0f / (float)geom_x;    // how big each cell is
  float cl_y = 1.0f / (float)geom_y;
  float max_xy = MAX(cl_x, cl_y);
  
#ifdef TA_QT3D
  node_so->graphs->removeAllChildren(); // todo: try to re-use instead of destroy!
  T3Entity* gr1 = new T3Entity(node_so->graphs);
#else // TA_QT3D
  SoSeparator* graphs = node_so->graphs();
  graphs->removeAllChildren();
#endif // TA_QT3D
  
  taVector2i pos;
  int idx = 0;
  if(mgeom.dims() <= 2) {
    for(pos.y=0; pos.y<geom_y; pos.y++) {
      for(pos.x=0; pos.x<geom_x; pos.x++) { // right to left
        float xp = ((float)pos.x) * cl_x;
        float yp = ((float)pos.y) * cl_y;
        
#ifdef TA_QT3D
        T3LineBox* lbox = new T3LineBox(gr1, QVector3D(width, 1.0f, boxd));
        lbox->Scale3D(cl_x, cl_y, max_xy);
        if(z_axis.on) {
          lbox->TranslateZFrontTo(QVector3D(-0.5f + xp + 0.5f * cl_x, -0.5f + yp + 0.5f * cl_y, 0));
        }
        else {
          lbox->Translate(-0.5f + xp + 0.5f * cl_x, -0.5f + yp + 0.5f * cl_y, 0.0f);
        }
        T3GraphLine* ln = new T3GraphLine(lbox, mainy, label_font_size, width, z_axis.on);
        // todo: need scale factor here..
        
#else // TA_QT3D
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
#endif // TA_QT3D
        
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
          
#ifdef TA_QT3D
          T3LineBox* lbox = new T3LineBox(gr1, QVector3D(width, 1.0f, boxd));
          lbox->Scale3D(cl_x, cl_y, max_xy);
          if(z_axis.on) {
            lbox->TranslateZFrontTo(QVector3D(-0.5f + xp + 0.5f * cl_x, -0.5f + yp + 0.5f * cl_y, 0));
          }
          else {
            lbox->Translate(-0.5f + xp + 0.5f * cl_x, -0.5f + yp + 0.5f * cl_y, 0.0f);
          }
          T3GraphLine* ln = new T3GraphLine(lbox, mainy, label_font_size, width, z_axis.on);
#else // TA_QT3D
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
#endif // TA_QT3D
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
            
#ifdef TA_QT3D
            T3LineBox* lbox = new T3LineBox(gr1, QVector3D(width, 1.0f, boxd));
            lbox->Scale3D(cl_x, cl_y, max_xy);
            if(z_axis.on) {
              lbox->TranslateZFrontTo(QVector3D(-0.5f + xp + 0.5f * cl_x, -0.5f + yp + 0.5f * cl_y, 0));
            }
            else {
              lbox->Translate(-0.5f + xp + 0.5f * cl_x, -0.5f + yp + 0.5f * cl_y, 0.0f);
            }
            T3GraphLine* ln = new T3GraphLine(lbox, mainy, label_font_size, width, z_axis.on);
            
#else // TA_QT3D
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
#endif // TA_QT3D
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
  }
  else if(val > ax_clr->range.max) {
    clr = colorscale.maxout.color();
  }
  else if(val < ax_clr->range.min) {
    clr = colorscale.minout.color();
  }
  else {
    int chnk = colorscale.chunks-1;
    float rval = ax_clr->range.Normalize(val);
    int idx = (int) ((rval * (float)chnk) + .5);
    idx = MAX(0, idx);
    idx = MIN(chnk, idx);
    clr = colorscale.GetColor(idx);
  }
  return clr;
}

void GraphTableView::PlotData_XY(GraphPlotView& plv, GraphPlotView& erv,
                                 GraphPlotView& yax,
                                 T3GraphLine* t3gl, int mat_cell,
                                 int clr_idx) {
  if(!t3gl) return;
  
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
  t3gl->setLineStyle((T3GraphLine::LineStyle)plv.line_style, dev_pix_ratio * line_width);
  t3gl->setMarkerSize(point_size);
  
  DataCol* da_er = erv.GetDAPtr();
  erv.matrix_cell = plv.matrix_cell;
  
  GraphAxisBase* ax_clr = NULL;
  DataCol* da_clr = NULL;
  if(color_mode == FIXED) {
    t3gl->setValueColorMode(false);
    t3gl->setDefaultColor(plv.color.color());
  }
  else {
    t3gl->setValueColorMode(true);
    if(color_mode == BY_VALUE) {
      ax_clr = &yax;
      da_clr = plv.GetDAPtr();
    }
    else { // BY_VARIABLE or BY_GROUP
      ax_clr = &color_axis;
      da_clr = color_axis.GetDAPtr();
    }
    if(!da_clr) {               // fallback
      t3gl->setValueColorMode(false);
      t3gl->setDefaultColor(plv.color.color());
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
      last_z = z_axis.GetDataVal(da_z, view_range.min);
  }
  if(x_axis.row_num)
    last_x = view_range.min;
  else
    last_x = x_axis.GetDataVal(da_x, view_range.min);
  iVec3f dat;                   // data point
  iVec3f plt;                   // plot coords
  iVec3f th_st;                 // start of thresholded line
  iVec3f th_ed;                 // end of thresholded line
  bool first = true;
  
  String  svg_markers;
  String  svg_labels;
  
  if(render_svg) {
    svg_str << taSvg::Path(plv.color.color(), dev_pix_ratio * line_width);
  }
  
  for (int row = view_range.min; row <= view_range.max; row++) {
    iColor clr; // only used for color modes
    bool clr_ok = false;
    bool new_trace = false;
    bool new_trace_z = false;
    bool out_of_bounds = false;
    if(x_axis.row_num) {
      dat.x = row;
    }
    else {
      dat.x = x_axis.GetDataVal(da_x, row);
    }
    if(dat.x < x_axis.range.min || dat.x > x_axis.range.max) continue;
    if(z_axis.on) {
      if(z_axis.row_num) {
        dat.z = row;
      }
      else if(da_z) {
        dat.z = z_axis.GetDataVal(da_z, row);
      }
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
      yval = plv.GetDataVal(da_y, row);
    }
    if((graph_type == RASTER) && da_rst) {
      dat.y = ax_rst->GetDataVal(da_rst, row);
    }
    else {
      dat.y = yval;
    }
    
    plt.x = x_axis.DataToPlot(dat.x);
    if((graph_type == RASTER) && da_rst) {
      if(dat.y < ax_rst->range.min || dat.y > ax_rst->range.max)
        continue;
      plt.y = ax_rst->DataToPlot(dat.y);
    }
    else {
      if(dat.y < yax.range.min) {
        plt.y = yax.DataToPlot(yax.range.min);
        out_of_bounds = true;
      }
      else if(dat.y > yax.range.max) {
        plt.y = yax.DataToPlot(yax.range.max);
        out_of_bounds = true;
      }
      else {
        plt.y = yax.DataToPlot(dat.y);
      }
    }
    if(z_axis.on || matz)
      plt.z = z_axis.DataToPlot(dat.z);
    
    if(dat.x < last_x)
      new_trace = true;
    if(z_axis.on && (dat.z < last_z))
      new_trace_z = true;
    
    if(clr_idx >= 0) {
      clr_ok = true;
      clr = colorscale.GetColor(clr_idx);
    }
    else if(da_clr) {
      clr_ok = true;
      if(color_mode == BY_VALUE) {
        clr = GetValueColor(ax_clr, yval);
      }
      else if (color_mode == BY_GROUP) {
        String group = da_clr->GetValAsString(row);
        int index = color_axis.group_by_values.FindEl(group);
        clr = GetValueColor(ax_clr, index);
      }
      else { // BY_VARIABLE
        clr = GetValueColor(ax_clr, ax_clr->GetDataVal(da_clr, row));
      }
    }
    
    if(render_svg) {
      if((row - view_range.min) % 10 == 1) {
        svg_str << "\n";
      }
    }

    // draw the line
    if(plot_style == LINE || plot_style == LINE_AND_POINTS) {
      if(first || (new_trace && !negative_draw) || (new_trace_z && !negative_draw_z))
      { // just starting out
        if(clr_ok)
          t3gl->moveTo(plt, clr);
        else
          t3gl->moveTo(plt);
        if(render_svg) {
          svg_str << "M " << taSvg::Coords(plt);
        }
      }
      else {
        if(clr_ok)
          t3gl->lineTo(plt, clr);
        else
          t3gl->lineTo(plt);
        if(render_svg) {
          svg_str << "L " << taSvg::Coords(plt);
        }
      }
    }
    else if(plot_style == THRESH_LINE) {
      if(yval >= thresh) {
        th_st = plt;  th_st.x = x_axis.DataToPlot(dat.x - thr_line_len);
        th_ed = plt;  th_ed.x = x_axis.DataToPlot(dat.x + thr_line_len);
        if(clr_ok) {
          t3gl->moveTo(th_st, clr);
          t3gl->lineTo(th_ed, clr);
        }
        else {
          t3gl->moveTo(th_st);
          t3gl->lineTo(th_ed);
        }
        if(render_svg) {
          svg_str << "M " << taSvg::Coords(th_st)
          << "L " << taSvg::Coords(th_ed);
        }
      }
    }
    else if(plot_style == THRESH_POINT) {
      if(yval >= thresh) {
        if(clr_ok)
          t3gl->markerAt(plt, (T3GraphLine::MarkerStyle)plv.point_style, clr);
        else
          t3gl->markerAt(plt, (T3GraphLine::MarkerStyle)plv.point_style);
        if(render_svg) {
          svg_markers << t3gl->markerAtSvg(plt, (T3GraphLine::MarkerStyle)plv.point_style);
        }
      }
    }
    
    // render marker, if any
    if((plot_style == POINTS) || (plot_style == LINE_AND_POINTS) || out_of_bounds) {
      T3GraphLine::MarkerStyle mrk = (T3GraphLine::MarkerStyle)plv.point_style;
      if(out_of_bounds)
        mrk = T3GraphLine::STAR; // indicates out of bounds
      
      if(row % point_spacing == 0 || out_of_bounds) {
        if(clr_ok)
          t3gl->markerAt(plt, mrk, clr);
        else
          t3gl->markerAt(plt, mrk);
        if(render_svg) {
          svg_markers << t3gl->markerAtSvg(plt, mrk);
        }
      }
    }
    
    if(erv.on && da_er && (row % err_spacing == 0)) {
      float err_dat;
      if(da_er->is_matrix && mat_cell >= 0) {
        err_dat = da_er->GetValAsFloatM(row, mat_cell);
      }
      else {
        err_dat = erv.GetDataVal(da_er, row);
      }
      float err_plt = err_dat;
      if(yax.range.Range() > 0.0f) err_plt /= yax.range.Range();
      
      if(clr_ok)
        t3gl->errBar(plt, err_plt, err_bar_width, clr);
      else
        t3gl->errBar(plt, err_plt, err_bar_width);
      
      if(render_svg) {
        svg_str
        // low bar
        << "\nM " << taSvg::Coords(plt.x-err_bar_width, plt.y-err_plt, plt.z)
        << "L " << taSvg::Coords(plt.x+err_bar_width, plt.y-err_plt, plt.z)
        << "M " << taSvg::Coords(plt.x-err_bar_width, plt.y+err_plt, plt.z)
        // high bar
        << "L " << taSvg::Coords(plt.x+err_bar_width, plt.y+err_plt, plt.z)
        << "M " << taSvg::Coords(plt.x, plt.y-err_plt, plt.z)
        // vert bar
        << "L " << taSvg::Coords(plt.x, plt.y+err_plt, plt.z)
        << "M " << taSvg::Coords(plt.x, plt.y, plt.z) << "\n"; // back home..
      }
    }
    
    if((label_spacing > 0) && (row % label_spacing == 0)) {
      String str = String(dat.y); // todo: could spec format..
      t3gl->textAt(iVec3f(plt.x,  plt.y - (.5f * label_font_size), plt.z),
                   str.chars());
      if(render_svg) {
        svg_labels << taSvg::Text(str, plt.x, plt.y - (.5f * label_font_size), plt.z,
                                  plv.color.color(), label_font_size, taSvg::LEFT);
      }
    }
    // post draw updates:
    first = false;
    last_x = dat.x;
    last_z = dat.z;
  }
  
  if(render_svg) {
    svg_str << taSvg::PathEnd();
    
    if(svg_markers.nonempty()) {
      svg_str << taSvg::Path(plv.color.color(), dev_pix_ratio * line_width)
      << svg_markers
      << taSvg::PathEnd();
    }
    
    if(svg_labels.nonempty()) {
      svg_str << svg_labels;
    }
  }
  
  t3gl->finishBatch();
}


#ifdef TA_QT3D
void GraphTableView::PlotData_Bar(T3Entity* gr1, GraphPlotView& plv, GraphPlotView& erv,
                                  GraphPlotView& yax,
                                  T3GraphLine* t3gl, float bar_off, int mat_cell,
                                  int clr_idx) {
#else // TA_QT3D
void GraphTableView::PlotData_Bar(SoSeparator* gr1, GraphPlotView& plv, GraphPlotView& erv,
                                  GraphPlotView& yax,
                                  T3GraphLine* t3gl, float bar_off, int mat_cell,
                                  int clr_idx) {
#endif // TA_QT3D
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
  t3gl->setLineStyle((T3GraphLine::LineStyle)plv.line_style, dev_pix_ratio * line_width);
  t3gl->setMarkerSize(point_size);
  
  DataCol* da_er = erv.GetDAPtr();
  erv.matrix_cell = plv.matrix_cell;
  
  GraphAxisBase* ax_clr = NULL;
  DataCol* da_clr = NULL;
  if(color_mode == FIXED) {
    t3gl->setValueColorMode(false);
    t3gl->setDefaultColor(plv.color.color());
  }
  else {
    t3gl->setValueColorMode(true);
    if(color_mode == BY_VALUE) {
      ax_clr = &yax;
      da_clr = plv.GetDAPtr();
    }
    else {
      ax_clr = &color_axis;
      da_clr = color_axis.GetDAPtr();
    }
    if(!da_clr) {               // fallback
      t3gl->setValueColorMode(false);
      t3gl->setDefaultColor(plv.color.color());
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
  
  float bar_wd_plt = bar_width * x_axis.axis_length / (float)view_range.Range();
  float bar_off_plt = bar_off * x_axis.axis_length / (float)view_range.Range();
  
  String svg_labels;
  String svg_bars;
  
  if(render_svg) {
    svg_str << taSvg::Path(plv.color.color(), -1.0f,
                           true, plv.color.color()); // fill, color
  }
  
  iVec3f dat;                   // data point
  iVec3f plt;                   // plot coords
  for (int row = view_range.min; row <= view_range.max; row++) {
    iColor clr; // only used for color modes
    bool clr_ok = false;
    if(x_axis.row_num)
      dat.x = row;
    else
      dat.x = x_axis.GetDataVal(da_x, row);
    if(dat.x < x_axis.range.min || dat.x > x_axis.range.max) continue;
    if(z_axis.on) {
      if(z_axis.row_num) {
        dat.z = row;
      }
      else if(da_z) {
        dat.z = z_axis.GetDataVal(da_z, row);
      }
      if(dat.z < z_axis.range.min || dat.z > z_axis.range.max) continue;
    }
    if((mat_cell >= 0) && (matrix_mode == Z_INDEX)) {
      dat.z = mat_cell;
    }
    if(mat_cell >= 0) {
      dat.y = da_y->GetValAsFloatM(row, mat_cell);
    }
    else {
      dat.y = plv.GetDataVal(da_y, row);
    }
    if(dat.y < yax.range.min || dat.y > yax.range.max) continue;
    
    plt.x = x_axis.DataToPlot(dat.x);
    plt.y = yax.DataToPlot(dat.y);
    
    if(z_axis.on || matz)
      plt.z = z_axis.DataToPlot(dat.z);
    
    if(clr_idx >= 0) {
      clr_ok = true;
      clr = colorscale.GetColor(clr_idx);
    }
    else if(da_clr) {
      clr_ok = true;
      if(color_mode == BY_VALUE) {
        clr = GetValueColor(ax_clr, dat.y);
      }
      else if (color_mode == BY_GROUP) {
        String group = da_clr->GetValAsString(row);
        int index = color_axis.group_by_values.FindEl(group);
        clr = GetValueColor(ax_clr, index);
      }
      else { // BY_VARIABLE
        clr = GetValueColor(ax_clr, ax_clr->GetDataVal(da_clr, row));
      }
    }
    
    iVec3f pt;
    iVec3f size;
    // draw the line
    pt = plt;  pt.x += bar_off_plt + bar_wd_plt * 0.5f;
    pt.y = 0.0f; pt.z -= bar_depth;
    size.x = bar_wd_plt;
    size.y = plt.y;
    size.z = bar_depth;

#ifdef TA_QT3D
    T3GraphBar* bar = new T3GraphBar(gr1, &plv, z_axis.on);
    if(clr_ok) {
      bar->SetBar(pt, size, clr);
    }
    else {
      bar->SetBar(pt, size, (T3Color)(plv.color.color()));
    }
#else // TA_QT3D
    T3GraphBar* bar = new T3GraphBar(&plv);
    if(clr_ok) {
      bar->SetBar(pt, size, clr);
    }
    else {
      bar->SetBar(pt, size, (T3Color)(plv.color.color()));
    }
    gr1->addChild(bar);
#endif // TA_QT3D
    
    if(render_svg) {
      if((row - view_range.min) % 10 == 1) {
        svg_str << "\n";
      }
      float hwd = 0.5f * bar_wd_plt;
      svg_str << "M " << taSvg::Coords(pt.x - hwd, pt.y, pt.z)
      << "L " << taSvg::Coords(pt.x - hwd, pt.y + size.y, pt.z)
      << "L " << taSvg::Coords(pt.x + hwd, pt.y + size.y, pt.z)
      << "L " << taSvg::Coords(pt.x + hwd, pt.y, pt.z);
    }
    
    if(erv.on && da_er && (row % err_spacing == 0)) {
      float err_dat;
      if(da_er->is_matrix && mat_cell >= 0) {
        err_dat = da_er->GetValAsFloatM(row, mat_cell);
      }
      else {
        err_dat = erv.GetDataVal(da_er, row);
      }
      float err_plt = err_dat;
      if(yax.range.Range() > 0.0f) err_plt /= yax.range.Range();
      
      pt = plt;  pt.x += bar_off_plt + bar_wd_plt * 0.5f;
      
      if(clr_ok)
        t3gl->errBar(pt, err_plt, err_bar_width, clr);
      else
        t3gl->errBar(pt, err_plt, err_bar_width);
      
      if(render_svg) {
        svg_bars
        // low bar
        << "\nM " << taSvg::Coords(plt.x-err_bar_width, plt.y-err_plt, plt.z)
        << "L " << taSvg::Coords(plt.x+err_bar_width, plt.y-err_plt, plt.z)
        << "M " << taSvg::Coords(plt.x-err_bar_width, plt.y+err_plt, plt.z)
        // high bar
        << "L " << taSvg::Coords(plt.x+err_bar_width, plt.y+err_plt, plt.z)
        << "M " << taSvg::Coords(plt.x, plt.y-err_plt, plt.z)
        // vert bar
        << "L " << taSvg::Coords(plt.x, plt.y+err_plt, plt.z)
        << "M " << taSvg::Coords(plt.x, plt.y, plt.z) << "\n"; // back home..
      }
    }
    
    if((label_spacing > 0) && (row % label_spacing == 0)) {
      String str = String(dat.y); // todo: could spec format..
      t3gl->textAt(iVec3f(plt.x,  plt.y - (.5f * label_font_size), plt.z),
                   str.chars());
      if(render_svg) {
        svg_labels << taSvg::Text(str, plt.x, plt.y - (.5f * label_font_size), plt.z,
                                  plv.color.color(), label_font_size, taSvg::LEFT);
      }
    }
  }
  
  if(render_svg) {
    svg_str << taSvg::PathEnd();
    
    if(svg_bars.nonempty()) {
      svg_str << taSvg::Path(plv.color.color(), dev_pix_ratio * line_width)
      << svg_bars
      << taSvg::PathEnd();
    }
    
    if(svg_labels.nonempty()) {
      svg_str << svg_labels;
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
  t3gl->setDefaultColor(plv_str.color.color());
  
  if(render_svg) {
    svg_str << taSvg::Group();
  }
  
  iVec3f dat;                   // data point
  iVec3f plt;                   // plot coords
  for (int row = view_range.min; row <= view_range.max; row++) {
    if(x_axis.row_num)
      dat.x = row;
    else
      dat.x = x_axis.GetDataVal(da_x, row);
    if(dat.x < x_axis.range.min || dat.x > x_axis.range.max) continue;
    if(z_axis.on) {
      if(z_axis.row_num) {
        dat.z = row;
      }
      else if(da_z) {
        dat.z = z_axis.GetDataVal(da_z, row);
      }
      if(dat.z < z_axis.range.min || dat.z > z_axis.range.max) continue;
    }
    dat.y = plv_y.GetDataVal(da_y, row);
    if(dat.y < plv_y.range.min || dat.y > plv_y.range.max) continue;
    
    plt.x = x_axis.DataToPlot(dat.x);
    plt.y = plv_y.DataToPlot(dat.y);
    if(z_axis.on)
      plt.z = z_axis.DataToPlot(dat.z);
    
    String str = plv_str.GetDataString(da_str, row);
    if(!str.empty()) {
      t3gl->textAt(iVec3f(plt.x,  plt.y - (.5f * label_font_size), plt.z),
                   str.chars());
      
      if(render_svg) {
        svg_str << taSvg::Text(str, plt.x, plt.y - (.5f * label_font_size), plt.z,
                               plv_str.color.color(), label_font_size, taSvg::LEFT);
      }
    }
  }
  t3gl->finishBatch();
  
  if(render_svg) {
    svg_str << taSvg::GroupEnd();
  }
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

iViewPanelOfGraphTable* GraphTableView::lvp() {
  return (iViewPanelOfGraphTable*)(iViewPanelOfDataTable*)m_lvp;
}

#ifndef TA_QT3D
 
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

#endif // TA_QT3D
